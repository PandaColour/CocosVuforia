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


extern "C"
{
	JNIEXPORT void Java_org_cocos2dx_javascript_turnScreanOrientation_changeGLviewOrientation(JNIEnv*  env, jobject thiz, jint w, jint h)
	{
		cocos2d::Size size = cocos2d::Director::getInstance()->getOpenGLView()->getFrameSize();
		cocos2d::Director::getInstance()->getOpenGLView()->setFrameSize(size.height, size.width);
	}
}
