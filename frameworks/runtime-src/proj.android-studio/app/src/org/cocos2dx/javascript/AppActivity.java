/****************************************************************************
Copyright (c) 2015 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.javascript;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
import android.graphics.Color;
import android.net.Uri;
import android.nfc.Tag;
import android.opengl.GLES20;
import android.opengl.Matrix;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;

import com.vuforia.COORDINATE_SYSTEM_TYPE;
import com.vuforia.CameraCalibration;
import com.vuforia.CameraDevice;
import com.vuforia.DataSet;
import com.vuforia.Device;
import com.vuforia.Matrix34F;
import com.vuforia.Matrix44F;
import com.vuforia.ObjectTracker;
import com.vuforia.Renderer;
import com.vuforia.STORAGE_TYPE;
import com.vuforia.State;
import com.vuforia.Tool;
import com.vuforia.Trackable;
import com.vuforia.Tracker;
import com.vuforia.TrackerManager;
import com.vuforia.VIEW;
import com.vuforia.Vec4I;
import com.vuforia.VideoBackgroundConfig;
import com.vuforia.ViewList;
import com.vuforia.RenderingPrimitives;
import com.vuforia.Vuforia;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxEditBox;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.cocos2dx.lib.Cocos2dxRenderer;
import org.cocos2dx.lib.ResizeLayout;

import java.util.ArrayList;
import java.util.Vector;

public class AppActivity extends Cocos2dxActivity implements VuforiaControl {

    @Override
    public Cocos2dxGLSurfaceView onCreateView() {
        Cocos2dxGLSurfaceView glSurfaceView = new Cocos2dxGLSurfaceView(this);
        glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);
        return glSurfaceView;
    }

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        vuforiaAppSession = new VuforiaApplicationSession(this);
        mIsDroidDevice = android.os.Build.MODEL.toLowerCase().startsWith(
                "droid");
        super.onCreate(savedInstanceState);
    }

    @Override
    public void init() {

        // FrameLayout
        ViewGroup.LayoutParams framelayout_params =
                new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                        ViewGroup.LayoutParams.MATCH_PARENT);

        mFrameLayout = new ResizeLayout(this);

        mFrameLayout.setLayoutParams(framelayout_params);

        // Cocos2dxEditText layout
        ViewGroup.LayoutParams edittext_layout_params =
                new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                        ViewGroup.LayoutParams.WRAP_CONTENT);
        Cocos2dxEditBox edittext = new Cocos2dxEditBox(this);
        edittext.setLayoutParams(edittext_layout_params);


        mFrameLayout.addView(edittext);

        // Cocos2dxGLSurfaceView
        this.mGLSurfaceView = this.onCreateView();

        // ...add to FrameLayout
        mFrameLayout.addView(this.mGLSurfaceView);

        // Switch to supported OpenGL (ARGB888) mode on emulator
        if (isAndroidEmulator())
            this.mGLSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);

        vuforiaRender = new VuforiaRender(vuforiaAppSession);
        this.mGLSurfaceView.setCocos2dxRenderer(vuforiaRender);
        this.mGLSurfaceView.setCocos2dxEditText(edittext);

        // Set framelayout as the content view
        setContentView(mFrameLayout);
    }

    public AppActivity() {
        super();
        s_self = this;
    }

    private static AppActivity s_self;
    public static AppActivity getAppActivity() {
        return s_self;
    }

    private static final String LOGTAG = "cocos vuforia";
    private VuforiaApplicationSession vuforiaAppSession = null;
    private VuforiaRender vuforiaRender = null;

    private DataSet mCurrentDataset;
    private int mCurrentDatasetSelectionIndex = 0;
    private ArrayList<String> mDatasetStrings = new ArrayList<String>();
    private boolean mSwitchDatasetAsap = false;

    private boolean mIsDroidDevice = false;
    private boolean mIsActive = false;
    private boolean bChangeProject = false;

    public boolean getAcitve() {
        return mIsActive;
    }

    @Override
    protected void onResume()
    {
        Log.d(LOGTAG, "onResume");
        super.onResume();

        // This is needed for some Droid devices to force portrait
        if (mIsDroidDevice)
        {
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        }

        try
        {
            vuforiaAppSession.resumeAR();
        } catch (VuforiaApplicationException e)
        {
            Log.e(LOGTAG, e.getString());
        }

        super.onResume();
    }

    public void showAR() {
        mDatasetStrings.add("res/StonesAndChips.xml");
        vuforiaAppSession
                .initAR(this, ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

//        float rawProjectionMatrixGL[] = {1.69032f,0.0f,0.0f,0.0f,0.0f,-3.0050135f,0.0f,0.0f, 0.0f, -0.0027777778f, 1.004008f, 1.0f, 0.0f, 0.0f, -20.040081f, 0.0f};
//
//
//        setDrawProjectMatrix(rawProjectionMatrixGL, 16);
    }

    public void stopAR() {
        try {
            bChangeProject = false;
            vuforiaAppSession.stopAR();
            stopCocosAR();
        } catch (VuforiaApplicationException e) {
            Log.e(LOGTAG, e.getString());
        }
    }

    private native void startCocosAR();
    private native void stopCocosAR();

    // Methods to load and destroy tracking data.
    @Override
    public boolean doLoadTrackersData() {
        TrackerManager tManager = TrackerManager.getInstance();
        ObjectTracker objectTracker = (ObjectTracker) tManager
                .getTracker(ObjectTracker.getClassType());
        if (objectTracker == null)
            return false;

        if (mCurrentDataset == null)
            mCurrentDataset = objectTracker.createDataSet();

        if (mCurrentDataset == null)
            return false;

        if (!mCurrentDataset.load(
                mDatasetStrings.get(mCurrentDatasetSelectionIndex),
                STORAGE_TYPE.STORAGE_APPRESOURCE))
            return false;

        if (!objectTracker.activateDataSet(mCurrentDataset))
            return false;

        int numTrackables = mCurrentDataset.getNumTrackables();
        for (int count = 0; count < numTrackables; count++) {
            Trackable trackable = mCurrentDataset.getTrackable(count);
            trackable.startExtendedTracking();

            String name = "Current Dataset : " + trackable.getName();
            trackable.setUserData(name);
            Log.d(LOGTAG, "UserData:Set the following user data "
                    + (String) trackable.getUserData());
        }

        return true;
    }

    @Override
    public boolean doUnloadTrackersData() {
        // Indicate if the trackers were unloaded correctly
        boolean result = true;

        TrackerManager tManager = TrackerManager.getInstance();
        ObjectTracker objectTracker = (ObjectTracker) tManager
                .getTracker(ObjectTracker.getClassType());
        if (objectTracker == null)
            return false;

        if (mCurrentDataset != null && mCurrentDataset.isActive()) {
            if (objectTracker.getActiveDataSet().equals(mCurrentDataset)
                    && !objectTracker.deactivateDataSet(mCurrentDataset)) {
                result = false;
            } else if (!objectTracker.destroyDataSet(mCurrentDataset)) {
                result = false;
            }

            mCurrentDataset = null;
        }

        return result;
    }

    @Override
    public void onInitARDone(VuforiaApplicationException exception) {

        if (exception == null) {
            initApplicationAR();
            vuforiaRender.setActive(true);
            try {
                vuforiaAppSession.startAR(CameraDevice.CAMERA_DIRECTION.CAMERA_DIRECTION_BACK);
            } catch (VuforiaApplicationException e) {
                Log.e("vuforia", e.getString());
            }

            DisplayMetrics metrics = new DisplayMetrics();
            getWindowManager().getDefaultDisplay().getMetrics(metrics);

            boolean result = CameraDevice.getInstance().setFocusMode(
                    CameraDevice.FOCUS_MODE.FOCUS_MODE_CONTINUOUSAUTO);

            if (!result)
                Log.e(LOGTAG, "Unable to enable continuous autofocus");

        }
    }

    private  void initApplicationAR(){
        Device device = Device.getInstance();
        device.setViewerActive(false); // Indicates if the app will be using a viewer, stereo mode and initializes the rendering primitives
        device.setMode(Device.MODE.MODE_AR); // Select if we will be in AR or VR mode
    }

    @Override
    public void onVuforiaUpdate(State state) {
        if (mSwitchDatasetAsap) {
            mSwitchDatasetAsap = false;
            TrackerManager tm = TrackerManager.getInstance();
            ObjectTracker ot = (ObjectTracker) tm.getTracker(ObjectTracker
                    .getClassType());
            if (ot == null || mCurrentDataset == null
                    || ot.getActiveDataSet() == null) {
                Log.d(LOGTAG, "Failed to swap datasets");
                return;
            }

            doUnloadTrackersData();
            doLoadTrackersData();
        }
    }


    @Override
    public boolean doInitTrackers() {
        // Indicate if the trackers were initialized correctly
        boolean result = true;

        TrackerManager tManager = TrackerManager.getInstance();
        Tracker tracker;

        // Trying to initialize the image tracker
        tracker = tManager.initTracker(ObjectTracker.getClassType());
        if (tracker == null) {
            Log.e(
                    LOGTAG,
                    "Tracker not initialized. Tracker already initialized or the camera is already started");
            result = false;
        } else {
            Log.i(LOGTAG, "Tracker successfully initialized");
        }
        return result;
    }


    @Override
    public boolean doStartTrackers() {
        // Indicate if the trackers were started correctly
        boolean result = true;

        Tracker objectTracker = TrackerManager.getInstance().getTracker(
                ObjectTracker.getClassType());
        if (objectTracker != null) {
            objectTracker.start();
            mIsActive = true;
            startCocosAR();
        }
        return result;
    }



    @Override
    public boolean doStopTrackers() {
        // Indicate if the trackers were stopped correctly
        boolean result = true;

        Tracker objectTracker = TrackerManager.getInstance().getTracker(
                ObjectTracker.getClassType());
        if (objectTracker != null)
            objectTracker.stop();

        return result;
    }

    @Override
    public boolean doDeinitTrackers() {
        // Indicate if the trackers were deinitialized correctly
        boolean result = true;

        TrackerManager tManager = TrackerManager.getInstance();
        tManager.deinitTracker(ObjectTracker.getClassType());

        return result;
    }
}
