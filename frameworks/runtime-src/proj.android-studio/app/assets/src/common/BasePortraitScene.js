/**
 * Created by Eric on 16/7/2.
 */
var BasePortraitScene = cc.Scene.extend({
    ctor: function () {
        var methodDeal = {methodName: "turnScreanToVertical"};
        BaseNative.callNativeMethod(methodDeal);
        var height = cc.winSize.height;
        var width = cc.winSize.width;
        if(height < width){
            cc.view.setDesignResolutionSize(height, width, cc.ResolutionPolicy.SHOW_ALL);
        } 
        this._super();
    }
});