package org.cocos2dx.javascript;

import android.app.Activity;
import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;

import com.vuforia.Device;
import com.vuforia.State;
import com.vuforia.Vuforia;

import org.cocos2dx.lib.Cocos2dxGLSurfaceView;

import javax.microedition.khronos.opengles.GL10;


public class CocosGameView extends Cocos2dxGLSurfaceView implements VuforiaAppRendererControl{
    private static String TAG = "CocosGameView";

    public CocosGameView(Activity activity) {
        super(activity);
    }

    @Override
    public void renderFrame(State state, float[] projectionMatrix) {
        Log.d(TAG, "renderFrame: xxxx");
    }

}
