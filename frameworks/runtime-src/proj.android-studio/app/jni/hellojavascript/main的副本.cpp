#include "AppDelegate.h"
#include "cocos2d.h"
#include "ScriptingCore.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>


#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <Vuforia/Vuforia.h>
#include <Vuforia/CameraDevice.h>
#include <Vuforia/Renderer.h>
#include <Vuforia/VideoBackgroundConfig.h>
#include <Vuforia/Trackable.h>
#include <Vuforia/TrackableResult.h>
#include <Vuforia/Tool.h>
#include <Vuforia/Tracker.h>
#include <Vuforia/TrackerManager.h>
#include <Vuforia/ObjectTracker.h>
#include <Vuforia/CameraCalibration.h>
#include <Vuforia/UpdateCallback.h>
#include <Vuforia/DataSet.h>
#include <Vuforia/Device.h>
#include <Vuforia/RenderingPrimitives.h>
#include <Vuforia/GLRenderer.h>
#include <Vuforia/StateUpdater.h>
#include <Vuforia/ViewList.h>


#define  LOG_TAG    "main"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

using namespace cocos2d;

void cocos_android_app_init (JNIEnv* env) {
    LOGD("cocos_android_app_init");
    AppDelegate *pAppDelegate = new AppDelegate();
}


#ifndef _VUFORIA_CUBE_SHADERS_H_
#define _VUFORIA_CUBE_SHADERS_H_


static const char* cubeMeshVertexShader = " \
  \
attribute vec4 vertexPosition; \
attribute vec2 vertexTexCoord; \
 \
varying vec2 texCoord; \
 \
uniform mat4 modelViewProjectionMatrix; \
 \
void main() \
{ \
   gl_Position = modelViewProjectionMatrix * vertexPosition; \
   texCoord = vertexTexCoord; \
} \
";


static const char* cubeFragmentShader = " \
 \
precision mediump float; \
 \
varying vec2 texCoord; \
 \
uniform sampler2D texSampler2D; \
 \
void main() \
{ \
   gl_FragColor = texture2D(texSampler2D, texCoord); \
} \
";

#endif // _VUFORIA_CUBE_SHADERS_H_

class ARDrawer : public cocos2d::Drawer
{
public:
    virtual void draw();
    virtual void customProject();
    virtual cocos2d::Mat4 getCustomProjectMat4();
    virtual cocos2d::Vec3 getCustomPoint(POINT_TYPE type);
    void drawVideoBackground();
    cocos2d::Mat4 getCustomCameraMat4();
    void updateRenderingPrimitives();
    float getSceneScaleFactor();
    void drawbg();
private:
    float _fieldOfView;
    float _aspectRatio;
    cocos2d::CustomCommand _customCommand;
    void initRendering();
    unsigned int createProgramFromBuffer(const char* vertexShaderBuffer,
                                     const char* fragmentShaderBuffer);
    void scalePoseMatrix(float x, float y, float z, float* matrix);

    bool bT = false;
    
private:
    unsigned int vbShaderProgramID    = 0;
    GLint vbVertexHandle              = 0;
    GLint vbTextureCoordHandle        = 0;
    GLint vbMvpMatrixHandle           = 0;
    GLint vbTexSampler2DHandle        = 0;
    bool  bInitRender = false;

    Vuforia::RenderingPrimitives* renderingPrimitives = NULL;
    pthread_mutex_t renderingPrimitivesMutex;
};


void ARDrawer::scalePoseMatrix(float x, float y, float z, float* matrix)
{
    // Sanity check
    if (!matrix)
        return;

    // matrix * scale_matrix
    matrix[0]  *= x;
    matrix[1]  *= x;
    matrix[2]  *= x;
    matrix[3]  *= x;
                     
    matrix[4]  *= y;
    matrix[5]  *= y;
    matrix[6]  *= y;
    matrix[7]  *= y;
                     
    matrix[8]  *= z;
    matrix[9]  *= z;
    matrix[10] *= z;
    matrix[11] *= z;
}


void ARDrawer::updateRenderingPrimitives()
{
    initRendering();
    pthread_mutex_lock(&renderingPrimitivesMutex);
    if (renderingPrimitives != NULL)
    {
        delete renderingPrimitives;
        renderingPrimitives = NULL;
    }
    renderingPrimitives = new Vuforia::RenderingPrimitives(Vuforia::Device::getInstance().getRenderingPrimitives());
    pthread_mutex_unlock(&renderingPrimitivesMutex);
}

void ARDrawer::initRendering()
{
    if (!bInitRender){
        glClearColor(0.0f, 0.0f, 0.0f, Vuforia::requiresAlpha() ? 0.0f : 1.0f);  
        vbShaderProgramID = createProgramFromBuffer(cubeMeshVertexShader, cubeFragmentShader);
        vbVertexHandle        = glGetAttribLocation(vbShaderProgramID, "vertexPosition");
        vbTextureCoordHandle  = glGetAttribLocation(vbShaderProgramID, "vertexTexCoord");
        vbMvpMatrixHandle     = glGetUniformLocation(vbShaderProgramID, "modelViewProjectionMatrix");
        vbTexSampler2DHandle  = glGetUniformLocation(vbShaderProgramID, "texSampler2D");
        bInitRender = true;
    }
}

unsigned int
ARDrawer::createProgramFromBuffer(const char* vertexShaderBuffer,
                                     const char* fragmentShaderBuffer)
{
#ifdef USE_OPENGL_ES_2_0    

    GLuint vertexShader = initShader(GL_VERTEX_SHADER, vertexShaderBuffer);
    if (!vertexShader)
        return 0;    

    GLuint fragmentShader = initShader(GL_FRAGMENT_SHADER,
                                        fragmentShaderBuffer);
    if (!fragmentShader)
        return 0;

    GLuint program = glCreateProgram();
    if (program)
    {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        
        glAttachShader(program, fragmentShader);
        checkGlError("glAttachShader");
        
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        
        if (linkStatus != GL_TRUE)
        {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength)
            {
                char* buf = (char*) malloc(bufLength);
                if (buf)
                {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGD("Could not link program: %s", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
#else
    return 0;
#endif
}

float ARDrawer::getSceneScaleFactor()
{
    static const float VIRTUAL_FOV_Y_DEGS = 85.0f;
    Vuforia::Vec2F fovVector = Vuforia::CameraDevice::getInstance().getCameraCalibration().getFieldOfViewRads();
    float cameraFovYRads = fovVector.data[1];
    float virtualFovYRads = VIRTUAL_FOV_Y_DEGS * M_PI / 180;
    return tan(cameraFovYRads / 2) / tan(virtualFovYRads / 2);
}

void ARDrawer::drawVideoBackground() {
    LOGD("drawVideoBackground");
    int vbVideoTextureUnit = 0;
    Vuforia::GLTextureUnit tex;
    tex.mTextureUnit = vbVideoTextureUnit;

    if (! Vuforia::Renderer::getInstance().updateVideoBackgroundTexture(&tex))
    {
        LOGD("Unable to bind video background texture!!");
        return;
    }

    Vuforia::Matrix44F vbProjectionMatrix = Vuforia::Tool::convert2GLMatrix(
        renderingPrimitives->getVideoBackgroundProjectionMatrix(Vuforia::VIEW_SINGULAR, Vuforia::COORDINATE_SYSTEM_CAMERA));

    if (Vuforia::Device::getInstance().isViewerActive())
    {
        float sceneScaleFactor = getSceneScaleFactor();
        scalePoseMatrix(sceneScaleFactor, sceneScaleFactor, 1.0f, vbProjectionMatrix.data);
    }

    

    GLboolean depthTest = false;
    GLboolean cullTest = false;
    GLboolean scissorsTest = false;

    glGetBooleanv(GL_DEPTH_TEST, &depthTest);
    glGetBooleanv(GL_CULL_FACE, &cullTest);
    glGetBooleanv(GL_SCISSOR_TEST, &scissorsTest);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);


    const Vuforia::Mesh& vbMesh = renderingPrimitives->getVideoBackgroundMesh(Vuforia::VIEW_SINGULAR);

    glUseProgram(vbShaderProgramID);
    glVertexAttribPointer(vbVertexHandle, 3, GL_FLOAT, false, 0, vbMesh.getPositionCoordinates());
    glVertexAttribPointer(vbTextureCoordHandle, 2, GL_FLOAT, false, 0, vbMesh.getUVCoordinates());

    glUniform1i(vbTexSampler2DHandle, vbVideoTextureUnit);

    glEnableVertexAttribArray(vbVertexHandle);
    glEnableVertexAttribArray(vbTextureCoordHandle);

    glUniformMatrix4fv(vbMvpMatrixHandle, 1, GL_FALSE, vbProjectionMatrix.data);


    glDrawElements(GL_TRIANGLES, vbMesh.getNumTriangles() * 3, GL_UNSIGNED_SHORT,
                   vbMesh.getTriangles());

    glDisableVertexAttribArray(vbVertexHandle);
    glDisableVertexAttribArray(vbTextureCoordHandle);


    if(depthTest)
        glEnable(GL_DEPTH_TEST);

    if(cullTest)
        glEnable(GL_CULL_FACE);

    if(scissorsTest)
        glEnable(GL_SCISSOR_TEST);
        
    LOGD("drawVideoBackground out");
}

void ARDrawer::draw()
{
    Vuforia::State state = Vuforia::Renderer::getInstance().begin();
    Vuforia::Renderer::getInstance().drawVideoBackground();
    Vuforia::Renderer::getInstance().end();

    // LOGD("draw in");
    // pthread_mutex_lock(&renderingPrimitivesMutex);
    // Vuforia::State state = Vuforia::Renderer::getInstance().begin();
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Vuforia::Renderer::getInstance().drawVideoBackground();
    // Vuforia::Renderer::getInstance().end();
    // pthread_mutex_unlock(&renderingPrimitivesMutex);
    // LOGD("draw out");

    //drawVideoBackground();
    _customCommand.init(-100);
    _customCommand.func = CC_CALLBACK_0(ARDrawer::drawbg, this);
    cocos2d::Director::getInstance()->getRenderer()->addCommand(&_customCommand, 0);

    //drawbg();
}

void ARDrawer::drawbg()
{
    // Vuforia::State state = Vuforia::Renderer::getInstance().begin();
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Vuforia::Renderer::getInstance().drawVideoBackground();
    // Vuforia::Renderer::getInstance().end();


    pthread_mutex_lock(&renderingPrimitivesMutex);

    Vuforia::Renderer& renderer = Vuforia::Renderer::getInstance();
    const Vuforia::State state = Vuforia::TrackerManager::getInstance().getStateUpdater().updateState();
    renderer.begin(state);

    Vuforia::ViewList& viewList = renderingPrimitives->getRenderingViews();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int viewIdx = 0; viewIdx < viewList.getNumViews(); viewIdx++) {
        Vuforia::VIEW vw = viewList.getView(viewIdx);

        //Vuforia::Vec4I viewport;
        //viewport = renderingPrimitives->getViewport(vw);

        //glViewport(viewport.data[0], viewport.data[1], viewport.data[2], viewport.data[3]);
        //glScissor(viewport.data[0], viewport.data[1], viewport.data[2], viewport.data[3]);

        if (vw != Vuforia::VIEW_POSTPROCESS) {
            Vuforia::Renderer::getInstance().drawVideoBackground();
            cocos2d::Director::getInstance()->getOpenGLView()->flush();
            //drawVideoBackground();
        } else {
            LOGD("Vuforia::VIEW_POSTPROCESS");
        }
    }

    renderer.end();
    pthread_mutex_unlock(&renderingPrimitivesMutex);
}

void ARDrawer::customProject()
{
    cocos2d::Mat4 projectionMatrix = getCustomProjectMat4();
    auto director = cocos2d::Director::getInstance();
    director->loadIdentityMatrix(cocos2d::MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    director->multiplyMatrix(cocos2d::MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, projectionMatrix);
    cocos2d::Mat4 matrixLookup = getCustomCameraMat4();
    director->multiplyMatrix(cocos2d::MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, matrixLookup);
    director->loadIdentityMatrix(cocos2d::MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

cocos2d::Mat4 ARDrawer::getCustomProjectMat4()
{
    const Vuforia::CameraCalibration& cameraCalibration = Vuforia::CameraDevice::getInstance().getCameraCalibration();
    Vuforia::Matrix44F projectionMatrix = Vuforia::Tool::getProjectionGL(cameraCalibration, 2.0f, 5000.0f);
    cocos2d::Mat4 matrixPerspective;
    for (int i = 0; i < 16; ++i) {
        matrixPerspective.m[i] = projectionMatrix.data[i];
    }
    _fieldOfView = MATH_RAD_TO_DEG(2 * atan(1.0/matrixPerspective.m[5]));
    //_aspectRatio = matrixPerspective.m[5]/matrixPerspective.m[0];
    return matrixPerspective;
}

cocos2d::Mat4 ARDrawer::getCustomCameraMat4()
{
    cocos2d::Mat4 matrixLookup;
    cocos2d::Mat4::createLookAt(getCustomPoint(POINT_TYPE::POINT_EYE), getCustomPoint(POINT_TYPE::POINT_CENTER), getCustomPoint(POINT_TYPE::POINT_UP), &matrixLookup);
    return matrixLookup;
}

cocos2d::Vec3 ARDrawer::getCustomPoint(POINT_TYPE type)
{
    cocos2d::Size size = cocos2d::Director::getInstance()->getWinSize();
    
    float zeye = (size.height * 0.5f) / tan(_fieldOfView*0.5f*3.1415926/180.0);
    if (_fieldOfView == 180) 
        zeye = -1429.0f;
    cocos2d::Vec3 point;
    switch (type) {
        case cocos2d::Drawer::POINT_TYPE::POINT_EYE:
            point = cocos2d::Vec3(size.width/2, size.height/2, zeye);
            break;
        case cocos2d::Drawer::POINT_TYPE::POINT_CENTER:
            point = cocos2d::Vec3(size.width/2, size.height/2, zeye - 10.0f);
            break;
        case cocos2d::Drawer::POINT_TYPE::POINT_UP:
            point = cocos2d::Vec3(0.0f, 1.0f, 0.0f);
            break;
        default:
            break;
    }
    return point;
}


extern "C"
{
	JNIEXPORT void Java_org_cocos2dx_javascript_turnScreanOrientation_changeGLviewOrientation(JNIEnv*  env, jobject thiz)
	{
		cocos2d::Size size = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
		cocos2d::Director::getInstance()->getOpenGLView()->setFrameSize(size.height, size.width);
	}

	JNIEXPORT void Java_org_cocos2dx_javascript_AppActivity_startCocosAR(JNIEnv*  env, jobject thiz)
	{
		static ARDrawer drawer;
        drawer.updateRenderingPrimitives();
        cocos2d::Director::getInstance()->setDrawer(&drawer);
        cocos2d::Director::getInstance()->setProjection(cocos2d::Director::Projection::CUSTOM);
	}

	JNIEXPORT void Java_org_cocos2dx_javascript_AppActivity_stopCocosAR(JNIEnv*  env, jobject thiz)
	{
		cocos2d::Director::getInstance()->setDrawer(nullptr);
        cocos2d::Director::getInstance()->setProjection(cocos2d::Director::Projection::_3D);
	}


}
