


var HelloWorldLayer = cc.Layer.extend({
    sprite:null,
    _camControlNode:null,
    _camNode:null,
    ctor:function () {
        //////////////////////////////
        // 1. super init first
        this._super();

        /////////////////////////////
        // 2. add a menu item with "X" image, which is clicked to quit the program
        //    you may modify it.
        // ask the window size
        var size = cc.winSize;
        var s = cc.winSize;

        /////////////////////////////
        // 3. add your codes below...
        // add a label shows "Hello World"
        // create and initialize a label
        var helloLabel = new cc.LabelTTF("Hello World", "Arial", 38);
        // position the label on the center of the screen
        helloLabel.x = size.width / 2;
        helloLabel.y = size.height / 2 + 200;
        // add the label as a child to this layer
        this.addChild(helloLabel, 5);

        //3D models
        var sprite1 = new jsb.Sprite3D("res/Sprite3DTest/boss.c3b");
        sprite1.setPosition3D(cc.math.vec3(size.width/2, size.height/2, 0));
        sprite1.setName("boss1");
        sprite1.setScale(20);
        sprite1.setVisible(true);
        sprite1.setCullFaceEnabled(false);
        this.addChild(sprite1);

        var item2 = new cc.MenuItemImage(res.CloseNormal_png, res.CloseSelected_png, this._menuCallback.bind(this, "stopAR"), this);
        item2.setScale(3);
        this.btn2 = new cc.Menu(item2);
        this.btn2.attr({x: size.width - 100, y: 100});
        this.btn2.setName("stop");
        this.addChild(this.btn2, 1);

        var item1 = new cc.MenuItemImage(res.CloseNormal_png, res.CloseSelected_png, this._menuCallback.bind(this, "showAR"), this);
        item1.setScale(3);
        this.btn1 = new cc.Menu(item1);
        this.btn1.attr({x: size.width - 100, y: 400});
        this.addChild(this.btn1, 1);

        this.scheduleOnce(this.once, 2);

        return true;
    },
    once: function(){
        var methodDeal = {methodName: "showAR"};
        BaseNative.callNativeMethod(methodDeal);
    },
    _menuCallback:function (stage) {
      if (stage == "showAR") {
        var methodDeal = {methodName: "showAR"};
        BaseNative.callNativeMethod(methodDeal);
      }

      if (stage == "stopAR") {
        var methodDeal = {methodName: "stopAR"};
        BaseNative.callNativeMethod(methodDeal);
      }
    }
});

var HelloWorldScene = BaseLandscapeScene.extend({
    onEnter:function () {
        this._super();
        var layer = new HelloWorldLayer();
        layer.setName("model")
        this.addChild(layer);
    }
});

