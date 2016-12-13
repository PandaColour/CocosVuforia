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
                var translation = methodDeal.models[0].translation;
                var scale = methodDeal.models[0].scale;
                var quaternion = methodDeal.models[0].quaternion;

                boss1.setPosition3D(translation);
                boss1.setRotationQuat(quaternion);
                boss1.setScaleX(scale.x);
                boss1.setScaleY(scale.y);
                boss1.setScaleZ(scale.z);
                boss1.setVisible(true);
            }
        }
        else
        {
            boss1.setVisible(false);
        }
    }
};









