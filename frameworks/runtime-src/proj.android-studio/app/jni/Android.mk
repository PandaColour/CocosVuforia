LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := Vuforia-prebuilt
LOCAL_SRC_FILES = vuforia/lib/$(TARGET_ARCH_ABI)/libVuforia.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/vuforia/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2djs_shared

LOCAL_MODULE_FILENAME := libcocos2djs

LOCAL_SHARED_LIBRARIES := Vuforia-prebuilt

LOCAL_SRC_FILES := hellojavascript/main.cpp \
                   ../../../Classes/AppDelegate.cpp 

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes \
                    $(LOCAL_PATH)/hellojavascript/include

LOCAL_STATIC_LIBRARIES := cocos2d_js_static

LOCAL_EXPORT_CFLAGS := -DCOCOS2D_DEBUG=2 -DCOCOS2D_JAVASCRIPT

include $(BUILD_SHARED_LIBRARY)


$(call import-module, scripting/js-bindings/proj.android)
