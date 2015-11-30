package org.iotivity.service.sample.container;

import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.util.Log;
import android.view.KeyEvent;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.WindowManager;

/*
 * Torch is an LED flashlight.
 */
public class OicLightActivity extends Activity implements SurfaceHolder.Callback {

    private static final String TAG = OicLightActivity.class.getSimpleName();

    private static final String WAKE_LOCK_TAG = "TORCH_WAKE_LOCK";

    private View button;
    private View lightText;

    private WakeLock wakeLock;
    private boolean lightOn = false;

    private static OicLightActivity torch;

    public OicLightActivity() {
        super();
        torch = this;
    }

    public static OicLightActivity getTorch() {
        return torch;
    }

    /*
     * Called by the view (see main.xml)
     */
    public void toggleLight(View view) {
        toggleLight();
    }

    private void toggleLight() {
        Log.i(OicLightActivity.class.getName(), "Toggle light");
        if (lightOn) {
            turnLightOff();
        } else {
            turnLightOn();
        }
    }

    private void turnLightOn() {
        lightOn = true;
        Log.i(OicLightActivity.class.getName(), "Turn light on");
        lightText.setBackgroundColor(Color.WHITE);
    }

    private void turnLightOff() {
        lightOn = false;
        Log.i(OicLightActivity.class.getName(), "Turn light off");
        lightText.setBackgroundColor(Color.BLACK);
    }

    private void startWakeLock() {
        if (wakeLock == null) {
            Log.d(TAG, "wakeLock is null, getting a new WakeLock");
            PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
            Log.d(TAG, "PowerManager acquired");
            wakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, WAKE_LOCK_TAG);
            Log.d(TAG, "WakeLock set");
        }
        wakeLock.acquire();
        Log.d(TAG, "WakeLock acquired");
    }

    private void stopWakeLock() {
        if (wakeLock != null) {
            wakeLock.release();
            Log.d(TAG, "WakeLock released");
        }
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                toggleLight();
            }
        });
        disablePhoneSleep();
        Log.i(TAG, "onCreate");
    }

    private void disablePhoneSleep() {
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    public void onRestart() {
        super.onRestart();
        Log.i(TAG, "onRestart");
    }

    @Override
    public void onStart() {
        super.onStart();
        Log.i(TAG, "onStart");
    }

    @Override
    public void onResume() {
        super.onResume();
        turnLightOn();
        Log.i(TAG, "onResume");
    }

    @Override
    public void onPause() {
        super.onPause();
        turnLightOff();
        Log.i(TAG, "onPause");
    }

    @Override
    public void onStop() {
        super.onStop();
        torch = null;
        Log.i(TAG, "onStop");
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.i(TAG, "onDestroy");
    }

    @Override
    public boolean onKeyLongPress(int keyCode, KeyEvent event) {
        // When the search button is long pressed, quit
        if (keyCode == KeyEvent.KEYCODE_SEARCH) {
            finish();
            return true;
        }
        return false;
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int I, int J, int K) {
        Log.d(TAG, "surfaceChanged");
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surfaceDestroyed");
    }
}