#include "AppDelegate.h"
#include "cocos2d.h"
#include "ScriptingCore.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"


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
#include <Vuforia/View.h>
#include <Vuforia/Image.h>
#include <Vuforia/Frame.h>
#include <Vuforia/RenderingPrimitives.h>

#include <thread>

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
private:
    void dealMatix(Vuforia::State state);
private:
    pthread_mutex_t renderingPrimitivesMutex;
    float _fieldOfView;
    float _aspectRatio;
};


void ARDrawer::draw()
{
    Vuforia::State state = Vuforia::Renderer::getInstance().begin();
    cocos2d::Texture2D* texture2d = new cocos2d::Texture2D();
    texture2d->autorelease();

    Vuforia::setFrameFormat(Vuforia::RGB565, true);
    Vuforia::Image *imageRGB565 = NULL;
    Vuforia::Frame frame = state.getFrame();
     
    for (int i = 0; i < frame.getNumImages(); ++i) {
        const Vuforia::Image *image = frame.getImage(i);
        if (image->getFormat() == Vuforia::RGB565) {
            imageRGB565 = (Vuforia::Image*)image;
            break;
        }
    }

    if (imageRGB565) {
        texture2d->initWithData(imageRGB565->getPixels(),
        imageRGB565->getBufferWidth() * imageRGB565->getBufferHeight() * 2,
        Texture2D::PixelFormat::RGB565,
        imageRGB565->getBufferWidth(),
        imageRGB565->getBufferHeight(),
        cocos2d::Size(imageRGB565->getBufferWidth(), imageRGB565->getBufferHeight()));

        cocos2d::Sprite* sprite = (cocos2d::Sprite*)cocos2d::Director::getInstance()->getRunningScene()->getChildByName("arRoot##");

        if (sprite == nullptr) {
            sprite = cocos2d::Sprite::createWithTexture(texture2d);
            cocos2d::Size size = cocos2d::Director::getInstance()->getWinSize();
            sprite->setPosition(cocos2d::Vec2(size.width/2, size.height/2));
            cocos2d::Size contentsize = sprite->getContentSize();
            sprite->setScale(contentsize.width/size.width, -contentsize.height/size.height);
            sprite->setName("arRoot##");
            sprite->setGlobalZOrder(-1);
            cocos2d::Director::getInstance()->getRunningScene()->addChild(sprite);
        }

        if (sprite) {
            sprite->setTexture(texture2d); 
        }
    }
    dealMatix(state);
    Vuforia::Renderer::getInstance().end();
}

void ARDrawer::dealMatix(Vuforia::State state)
{
    rapidjson::Document writedoc;
    writedoc.SetObject();
    rapidjson::Document::AllocatorType& allocator = writedoc.GetAllocator();
    
    rapidjson::Value models(rapidjson::kArrayType);
    for (int i = 0; i < state.getNumTrackableResults(); ++i) {
        const Vuforia::TrackableResult* result = state.getTrackableResult(i);
        const Vuforia::Trackable& trackable = result->getTrackable();
        Vuforia::Matrix44F modelViewMatrix = Vuforia::Tool::convertPose2GLMatrix(result->getPose());
        
        cocos2d::Mat4 mMatrix;
        for (int i = 0; i < 16; ++i) {
            mMatrix.m[i] = modelViewMatrix.data[i];
        };
        float scala = 3.0;
        mMatrix.m[12] += (mMatrix.m[8]  * scala);
        mMatrix.m[13] += (mMatrix.m[9]  * scala);
        mMatrix.m[14] += (mMatrix.m[10] * scala);
        mMatrix.m[15] += (mMatrix.m[11] * scala);
        mMatrix.scale(scala, scala, scala);
        
        cocos2d::Vec3 eye = getCustomPoint(cocos2d::Drawer::POINT_TYPE::POINT_EYE);
        cocos2d::Mat4 cocosMatrix;
        cocos2d::Mat4::createTranslation(eye, &cocosMatrix);
        cocosMatrix.multiply(mMatrix);
        
        cocos2d::Quaternion quat;
        cocos2d::Vec3 scale;
        cocos2d::Vec3 translation;
        cocosMatrix.decompose(&scale, &quat, &translation);
        
        rapidjson::Value quaternion(rapidjson::kObjectType);
        quaternion.AddMember("x", quat.x, allocator);
        quaternion.AddMember("y", quat.y, allocator);
        quaternion.AddMember("z", quat.z, allocator);
        quaternion.AddMember("w", quat.w, allocator);
        
        rapidjson::Value scaleXYZ(rapidjson::kObjectType);
        scaleXYZ.AddMember("x", scale.x, allocator);
        scaleXYZ.AddMember("y", scale.y, allocator);
        scaleXYZ.AddMember("z", scale.z, allocator);

        rapidjson::Value translationXYZ(rapidjson::kObjectType);
        translationXYZ.AddMember("x", translation.x, allocator);
        translationXYZ.AddMember("y", translation.y, allocator);
        translationXYZ.AddMember("z", translation.z, allocator);
        
        rapidjson::Value model(rapidjson::kObjectType);
        rapidjson::Value name(rapidjson::kStringType);
        name.SetString(trackable.getName(), rapidjson::SizeType(strlen(trackable.getName())));
        model.AddMember("name", name, allocator);
        model.AddMember("quaternion", quaternion, allocator);
        model.AddMember("scale", scaleXYZ, allocator);
        model.AddMember("translation", translationXYZ, allocator);
        
        models.PushBack(model, allocator);
    }
    writedoc.AddMember("module", "AR", allocator);
    writedoc.AddMember("models", models, allocator);
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writedoc.Accept(writer);

    std::string json = buffer.GetString();
    std::string jsMethod = std::string("globle.callJsMedthod('") + json + "')";
    ScriptingCore::getInstance()->evalString(jsMethod.c_str(), nullptr);
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
        cocos2d::Director::getInstance()->setDrawer(&drawer);
        cocos2d::Director::getInstance()->setProjection(cocos2d::Director::Projection::CUSTOM);
	}

	JNIEXPORT void Java_org_cocos2dx_javascript_AppActivity_stopCocosAR(JNIEnv*  env, jobject thiz)
	{
		cocos2d::Director::getInstance()->setDrawer(nullptr);
        cocos2d::Director::getInstance()->setProjection(cocos2d::Director::Projection::_3D);
        cocos2d::Sprite* sprite = (cocos2d::Sprite*)cocos2d::Director::getInstance()->getRunningScene()->getChildByName("arRoot##");
        if (sprite != nullptr) {
            sprite -> removeFromParent();
        }
	}


}
