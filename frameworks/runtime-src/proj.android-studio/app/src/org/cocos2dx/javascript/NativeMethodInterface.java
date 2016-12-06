package org.cocos2dx.javascript;
import android.util.Log;
import android.content.pm.ActivityInfo;
import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;

import org.json.*;

class NativeMethodDeal{
    public void setParams(JSONObject dict) {
        params = dict;
    }

    public String callRealMethod() {
        String methodName = null;
        try {
            methodName = params.getString("methodName");
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return String.format("{\"result\": \"error\", \"error\" : \"native method: %s hasn't define.\"}", methodName);
    }

    protected JSONObject params;
}

@SuppressWarnings("unused")
public class NativeMethodInterface {
    public static String callNativeMethod(String param) {
        String ret = "";
        JSONObject dict = createJSONObject(param);
        String dealName = getDealName(dict);
        Class<?> className = null;
        try{
            className = Class.forName(dealName);
        }catch (Exception e) {
            e.printStackTrace();
        }

        NativeMethodDeal deal = null;
        if (className != null) {
            try {
                deal = (NativeMethodDeal)className.newInstance();
            } catch (InstantiationException e) {
                e.printStackTrace();
            } catch (IllegalAccessException e) {
                e.printStackTrace();
            }
        }

        if (deal == null)
        {
            deal = new NativeMethodDeal();
        }

        deal.setParams(dict);
        ret = deal.callRealMethod();
        return ret;
    }

    private static JSONObject createJSONObject(String param) {
        JSONObject dict = new JSONObject();
        try{
            dict = new JSONObject(param);
        }catch (Exception e) {
            // TODO: handle exception
            e.printStackTrace();
        }
        return dict;
    }

    private static String getDealName(JSONObject dict) {
        String className = "";
        try {
            className = dict.getString("methodName");
            className = "org.cocos2dx.javascript." + className;
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return className;
    }
}

@SuppressWarnings("unused")
class getStatusBarOrientation extends NativeMethodDeal{
    @Override
    public String callRealMethod() {
        String Orientation = "Unkown";
        int UIOrientation = AppActivity.getAppActivity().getRequestedOrientation();
        switch (UIOrientation) {
            case ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE:
            case ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE:
                Orientation = "LandscapeRight";
                break;
            case ActivityInfo.SCREEN_ORIENTATION_PORTRAIT:
            case ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT:
            case ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT:
                Orientation = "Portrait";
                break;
        }
        return String.format("{\"result\": \"%s\"}", Orientation);
    }
}

class turnScreanOrientation extends NativeMethodDeal {
    public static native void changeGLviewOrientation();
}

@SuppressWarnings("unused")
class turnScreanToHorizontal extends turnScreanOrientation{
    @Override
    public String callRealMethod() {
        AppActivity.getAppActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        changeGLviewOrientation();
        return String.format("{\"result\": \"success\"}");
    }
}

@SuppressWarnings("unused")
class turnScreanToVertical extends turnScreanOrientation{
    @Override
    public String callRealMethod() {
        AppActivity.getAppActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        changeGLviewOrientation();
        return String.format("{\"result\": \"success\"}");
    }
}

@SuppressWarnings("unused")
class showAR extends NativeMethodDeal {
    @Override
    public String callRealMethod() {
        Log.e("show ar", "callRealMethod:xxx " );
        AppActivity.getAppActivity().showAR();

        return String.format("{\"result\": \"success\"}");
    }
}

@SuppressWarnings("unused")
class stopAR extends NativeMethodDeal {
    @Override
    public String callRealMethod() {
        AppActivity.getAppActivity().stopAR();
        return String.format("{\"result\": \"success\"}");
    }
}


