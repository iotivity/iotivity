package org.iotivity.test.ri.app;

import android.util.Log;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;

/**
 * Created by TutorialsPoint7 on 8/23/2016.
 */
public class InputReceiver extends BroadcastReceiver{
   @Override
   public void onReceive(Context context, Intent intent) {
      String data = intent.getStringExtra("input");
      Log.d("InputReceiver", "Intent Detected.");
      Toast.makeText(context, "Intent Detected: " + data, Toast.LENGTH_LONG).show();
   }
}
