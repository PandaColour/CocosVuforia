var BaseNative = {
    callNativeMethod: function (methodDeal) {
        var ret = JSON.stringify({result:"it not belong to ios or android."});
        switch (cc.sys.os) {
            case cc.sys.OS_ANDROID:
                ret = jsb.reflection.callStaticMethod("org/cocos2dx/javascript/NativeMethodInterface", "callNativeMethod", "(Ljava/lang/String;)Ljava/lang/String;", JSON.stringify(methodDeal));
                break;
            case cc.sys.OS_IOS:
                ret = jsb.reflection.callStaticMethod("NativeMethodInterface", "callNativeMethod:", JSON.stringify(methodDeal));
                break;
            default:
                cc.log("cc.sys.os:" + cc.sys.os)
        }
        return ret;
    }
};