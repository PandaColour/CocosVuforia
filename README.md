# CocosVuforia
Cocos2d-x  add Vuforia

# 命令行 
cocos compile -s /*yourpath*/ -p android --ap android-22 -m debug  --android-studio --app-abi armeabi-v7a
如果 cocos compile 出现 “无可用平台”提示,把文件夹下 cocos-project.json 重命名为 .cocos-project.json


#实现cocos2dx 和 vuforia 结合
ios版本
安卓版本
cocos2dx 自动的UI系统全部不能使用了,按钮等控件需要自己重现实现,如果使用原来的控件,响应点和显示点屏幕水平中线对称

#2016/12/12
ios版本是完全支持的
android版本的 Vuforia::Renderer::getInstance().drawVideoBackground(); 把摄像头图像画在了最上层，虽然可以使用 Vuforia::State state.begin().getFrame().getImage(in idx) 把摄像头图像取出自己绘制在最底层，希望能够改进掉 

#2017/1/11
可以肯定是android drawVideoBackground绘制的时候破坏了opengl的某些内部context，并不是绘制层次的问题

#2017/1/13
解决安卓摄像头绘制，虽然并不是官方推荐的方法