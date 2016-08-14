var globle = {
    callJsMedthod: function (param) {
        var methodDeal = JSON.parse(param);
        var scene = cc.director.getRunningScene();
        var cammer = scene.getChildByName("root");
        var layer = scene.getChildByName("model");
        var boss1 = layer.getChildByName("boss1");
        var stop = layer.getChildByName("stop");

        if (methodDeal.models.length > 0)
        {
            if(methodDeal.models[0].name == "stones")
            {
                var size = cc.winSize;
                cc.log([size.width, size.height]);
                var cammerPos = cc.Camera.getDefaultCamera().getPosition3D();
                cc.log([cammerPos.x, cammerPos.y, cammerPos.z]);

                var modelView = methodDeal.models[0].modelViewMatrix;
                var methodDeal = {methodName: "decomposeMatrix", modelViewMatrix: modelView};
                var result = JSON.parse(BaseNative.callNativeMethod(methodDeal));

                var translation = result.translation;
                var scale = result.scale;
                var quaternion = result.quaternion;

                boss1.setPosition3D(cc.math.vec3(translation[0], translation[1], translation[2]));
                boss1.setRotationQuat(cc.math.quaternion(quaternion[0], quaternion[1], quaternion[2], quaternion[3]));
                boss1.setScaleX(scale[0]);
                boss1.setScaleY(scale[1]);
                boss1.setScaleZ(scale[2]);
                boss1.setVisible(true);
            }
        }
        else
        {
            boss1.setVisible(false);
        }
    }
};









