#include "AppDelegate.h"
#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "main"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

using namespace cocos2d;

void cocos_android_app_init (JNIEnv* env) {
    LOGD("cocos_android_app_init");
    AppDelegate *pAppDelegate = new AppDelegate();
}

class ARDrawer : public cocos2d::Drawer
{
public:
    virtual void draw();
    virtual void customProject();
    virtual cocos2d::Mat4 getCustomProjectMat4();
    virtual cocos2d::Vec3 getCustomPoint(POINT_TYPE type);
    cocos2d::Mat4 getCustomCameraMat4();
    void setProjection(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);
private:
    float _fieldOfView;
    float _aspectRatio;
    cocos2d::Mat4 _projectionMatrix;
};

void ARDrawer::draw()
{
    // Vuforia::State state = Vuforia::Renderer::getInstance().begin();
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Vuforia::Renderer::getInstance().drawVideoBackground();
    // Vuforia::Renderer::getInstance().end();
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
    return _projectionMatrix;
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
    if (_fieldOfView == 180) zeye = -1429.0f;
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

void ARDrawer::setProjection(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
{
    _projectionMatrix.set(m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44);
}



extern "C"
{
	JNIEXPORT void Java_org_cocos2dx_javascript_turnScreanOrientation_changeGLviewOrientation(JNIEnv*  env, jobject thiz)
	{
		cocos2d::Size size = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
		cocos2d::Director::getInstance()->getOpenGLView()->setFrameSize(size.height, size.width);
	}

	JNIEXPORT void Java_org_cocos2dx_javascript_AppActivity_setDrawProjectMatrix(JNIEnv*  env, jobject thiz, jfloatArray mat4, jint len)
	{
		static ARDrawer drawer;

        jfloat* mat = env->GetFloatArrayElements(mat4, NULL);
        if (len == 16 && mat) {
            drawer.setProjection(mat[0], mat[1], mat[2], mat[3], mat[4], mat[5], mat[6], mat[7], mat[8], mat[9], mat[10], mat[11], mat[12], mat[13],
                mat[14], mat[15]);
            cocos2d::Director::getInstance()->setDrawer(&drawer);
            cocos2d::Director::getInstance()->setProjection(cocos2d::Director::Projection::CUSTOM);
        } 
        env->ReleaseFloatArrayElements(mat4, mat, 0);
	}

	JNIEXPORT void Java_org_cocos2dx_javascript_xxx_stopAR(JNIEnv*  env, jobject thiz, jint w, jint h)
	{
		cocos2d::Director::getInstance()->setDrawer(nullptr);
        cocos2d::Director::getInstance()->setProjection(cocos2d::Director::Projection::_3D);
	}
}
