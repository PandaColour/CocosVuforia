package org.cocos2dx.javascript;

import android.content.res.Configuration;
import android.graphics.Point;
import android.opengl.GLES20;
import android.opengl.Matrix;
import android.os.Message;
import android.util.Log;

import com.vuforia.COORDINATE_SYSTEM_TYPE;
import com.vuforia.CameraDevice;
import com.vuforia.Device;
import com.vuforia.GLTextureUnit;
import com.vuforia.Matrix34F;
import com.vuforia.Matrix44F;
import com.vuforia.Mesh;
import com.vuforia.Renderer;
import com.vuforia.RenderingPrimitives;
import com.vuforia.State;
import com.vuforia.Tool;
import com.vuforia.Trackable;
import com.vuforia.TrackableResult;
import com.vuforia.TrackerManager;
import com.vuforia.VIDEO_BACKGROUND_REFLECTION;
import com.vuforia.VIEW;
import com.vuforia.Vec2F;
import com.vuforia.Vec2I;
import com.vuforia.Vec4I;
import com.vuforia.VideoBackgroundConfig;
import com.vuforia.VideoMode;
import com.vuforia.ViewList;
import com.vuforia.Vuforia;

import org.cocos2dx.javascript.utils.*;

import org.cocos2dx.lib.Cocos2dxRenderer;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.lang.reflect.Array;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class VuforiaRender extends Cocos2dxRenderer{
    private static final String LOGTAG = "VuforiaRender";
    private VuforiaApplicationSession vuforiaAppSession;
    private float mNearPlane = 10.0f;
    private float mFarPlane = 5000.0f;

    private GLTextureUnit videoBackgroundTex = null;

    private int vbShaderProgramID = 0;
    private int vbTexSampler2DHandle = 0;
    private int vbVertexHandle = 0;
    private int vbTexCoordHandle = 0;
    private int vbProjectionMatrixHandle = 0;

    private boolean mIsActive = false;

    private int mScreenWidth = 0;
    private int mScreenHeight = 0;
    private boolean mIsPortrait = false;


    //Gray scale background
    private int vbVertexPositionHandle;
    private int vbVertexTexCoordHandle;
    private int vbTouchLocationXHandle;
    private int vbTouchLocationYHandle;
    private float[] vbOrthoProjMatrix;
    private int viewportPosition_x;
    private int viewportPosition_y;
    private int viewportSize_x;
    private int viewportSize_y;


    Timer timer = new Timer();
    TimerTask task = new TimerTask() {
        @Override
        public void run() {
            onConfigurationChanged(mIsActive);
        }
    };

    public VuforiaRender(VuforiaApplicationSession session) {
        vuforiaAppSession = session;
    }

    public void setActive(boolean active)
    {
        mIsActive = active;

        if(mIsActive)
            configureVideoBackground();
    }

    public void configureVideoBackground()
    {
        CameraDevice cameraDevice = CameraDevice.getInstance();
        VideoMode vm = cameraDevice.getVideoMode(CameraDevice.MODE.MODE_DEFAULT);

        VideoBackgroundConfig config = new VideoBackgroundConfig();
        config.setEnabled(true);
        config.setPosition(new Vec2I(0, 0));

        int xSize = 0, ySize = 0;
        // We keep the aspect ratio to keep the video correctly rendered. If it is portrait we
        // preserve the height and scale width and vice versa if it is landscape, we preserve
        // the width and we check if the selected values fill the screen, otherwise we invert
        // the selection
        if (mIsPortrait)
        {
            xSize = (int) (vm.getHeight() * (mScreenHeight / (float) vm
                    .getWidth()));
            ySize = mScreenHeight;

            if (xSize < mScreenWidth)
            {
                xSize = mScreenWidth;
                ySize = (int) (mScreenWidth * (vm.getWidth() / (float) vm
                        .getHeight()));
            }
        } else
        {
            xSize = mScreenWidth;
            ySize = (int) (vm.getHeight() * (mScreenWidth / (float) vm
                    .getWidth()));

            if (ySize < mScreenHeight)
            {
                xSize = (int) (mScreenHeight * (vm.getWidth() / (float) vm
                        .getHeight()));
                ySize = mScreenHeight;
            }
        }

        config.setSize(new Vec2I(xSize, ySize));

        Log.i(LOGTAG, "Configure Video Background : Video (" + vm.getWidth()
                + " , " + vm.getHeight() + "), Screen (" + mScreenWidth + " , "
                + mScreenHeight + "), mSize (" + xSize + " , " + ySize + ")");

        if (xSize == 0 || ySize == 0 ) {
            timer.schedule(task, 1000);
        } else {
            Renderer.getInstance().setVideoBackgroundConfig(config);
        }
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        vuforiaAppSession.onSurfaceCreated();
        //initRendering();
        super.onSurfaceCreated(gl, config);

    }

    void initRendering()
    {
        vbShaderProgramID = SampleUtils.createProgramFromShaderSrc(VideoBackgroundShader.VB_VERTEX_SHADER,
                VideoBackgroundShader.VB_FRAGMENT_SHADER);

        // Rendering configuration for video background
        if (vbShaderProgramID > 0)
        {
            // Activate shader:
            GLES20.glUseProgram(vbShaderProgramID);

            // Retrieve handler for texture sampler shader uniform variable:
            vbTexSampler2DHandle = GLES20.glGetUniformLocation(vbShaderProgramID, "texSampler2D");

            // Retrieve handler for projection matrix shader uniform variable:
            vbProjectionMatrixHandle = GLES20.glGetUniformLocation(vbShaderProgramID, "projectionMatrix");

            vbVertexHandle = GLES20.glGetAttribLocation(vbShaderProgramID, "vertexPosition");
            vbTexCoordHandle = GLES20.glGetAttribLocation(vbShaderProgramID, "vertexTexCoord");
            vbProjectionMatrixHandle = GLES20.glGetUniformLocation(vbShaderProgramID, "projectionMatrix");
            vbTexSampler2DHandle = GLES20.glGetUniformLocation(vbShaderProgramID, "texSampler2D");

            // Stop using the program
            GLES20.glUseProgram(0);
        }

        vbShaderProgramID = SampleUtils.createProgramFromShaderSrc(
                Shaders.BTA_VERTEX_SHADER, Shaders.BTA_FRAGMENT_SHADER);

        if (vbShaderProgramID > 0)
        {
            // Activate shader:
            GLES20.glUseProgram(vbShaderProgramID);

            // Retrieve handler for vertex position shader attribute variable:
            vbVertexPositionHandle = GLES20.glGetAttribLocation(
                    vbShaderProgramID, "vertexPosition");

            // Retrieve handler for texture coordinate shader attribute
            // variable:
            vbVertexTexCoordHandle = GLES20.glGetAttribLocation(
                    vbShaderProgramID, "vertexTexCoord");

            // Retrieve handler for texture sampler shader uniform variable:
            vbTexSampler2DHandle = GLES20.glGetUniformLocation(
                    vbShaderProgramID, "texSampler2D");

            // Retrieve handler for projection matrix shader uniform variable:
            vbProjectionMatrixHandle = GLES20.glGetUniformLocation(
                    vbShaderProgramID, "projectionMatrix");

            // Retrieve handler for projection matrix shader uniform variable:
            vbTouchLocationXHandle = GLES20.glGetUniformLocation(
                    vbShaderProgramID, "touchLocation_x");

            // Retrieve handler for projection matrix shader uniform variable:
            vbTouchLocationYHandle = GLES20.glGetUniformLocation(
                    vbShaderProgramID, "touchLocation_y");

            // Set the orthographic matrix
            vbOrthoProjMatrix = SampleUtils.getOrthoMatrix(-1.0f, 1.0f, -1.0f,
                    1.0f, -1.0f, 1.0f);

            // Stop using the program
            GLES20.glUseProgram(0);
        }


        videoBackgroundTex = new GLTextureUnit();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        vuforiaAppSession.onSurfaceChanged(width, height);
        super.onSurfaceChanged(gl, width, height);
        onConfigurationChanged(mIsActive);
        //initRendering();
    }

    public void onConfigurationChanged(boolean isARActive)
    {
        updateActivityOrientation();
        storeScreenDimensions();

        if(isARActive)
            configureVideoBackground();
    }

    private void storeScreenDimensions()
    {
        // Query display dimensions:
        Point size = new Point();
        AppActivity.getAppActivity().getWindowManager().getDefaultDisplay().getRealSize(size);
        mScreenWidth = size.x;
        mScreenHeight = size.y;
    }

    @Override
    public void onDrawFrame(GL10 gl){
        super.onDrawFrame(gl);
    }

    private void updateActivityOrientation()
    {
        Configuration config = AppActivity.getAppActivity().getResources().getConfiguration();

        switch (config.orientation)
        {
            case Configuration.ORIENTATION_PORTRAIT:
                mIsPortrait = true;
                break;
            case Configuration.ORIENTATION_LANDSCAPE:
                mIsPortrait = false;
                break;
            case Configuration.ORIENTATION_UNDEFINED:
            default:
                break;
        }

        Log.i(LOGTAG, "Activity is in "
                + (mIsPortrait ? "PORTRAIT" : "LANDSCAPE"));
    }

}
