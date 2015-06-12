package org.iotivity.service.ssm.sample;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.UUID;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.ssm.DataReader;
import org.iotivity.service.ssm.IQueryEngineEvent;
import org.iotivity.service.ssm.ModelData;
import org.iotivity.service.ssm.R;
import org.iotivity.service.ssm.SSMInterface;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.AssetManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;

public class MainActivity extends Activity {

    private SSMInterface       SoftSensorManager         = null;
    private ArrayList<Integer> mRunningQueries           = new ArrayList<Integer>();

    private IQueryEngineEvent  mQueryEngineEventListener = null;
    private final String         LOG_TAG = this.getClass().getSimpleName();
    private static MainActivity  activityObj;

    void PrintLog(String log) {
        Message msg = new Message();
        Bundle data = new Bundle();
        data.putString("Log", log);
        msg.setData(data);
        logHandler.sendMessage(msg);
    }

    private Handler      logHandler         = new Handler() {
                                                @Override
                                                public void handleMessage(
                                                        Message msg) {
                                                    tvLog.append(msg.getData()
                                                            .getString("Log")
                                                            + "\n");
                                                    svLog.fullScroll(ScrollView.FOCUS_DOWN);
                                                }
                                            };

    private TextView     tvLog              = null;
    private ScrollView   svLog              = null;
    private EditText     edtQuery           = null;
    private EditText     edtUnregisterQuery = null;

    View.OnClickListener clickHandler       = null;

    View.OnClickListener textAddHandler     = null;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tvLog = (TextView) findViewById(R.id.txtLog);
        svLog = (ScrollView) findViewById(R.id.sclLog);
        edtQuery = (EditText) findViewById(R.id.editQuery);
        edtUnregisterQuery = (EditText) findViewById(R.id.editUnregisterQuery);
        
        // calling the method to check the Wi-fi connectivity and configuring
        // the OcPlatform
        configurePlatform();

        mQueryEngineEventListener = new IQueryEngineEvent() {
            @Override
            public void onQueryEngineEvent(int cqid, DataReader result) {
                Log.i("[SSM]", "event received! cqid=" + cqid);
                PrintLog("Event from cqid " + cqid + " has received");

                List<String> models = result.getAffectedModels();

                for (String modelName : models) {
                    PrintLog("Model: " + modelName);

                    try {
                        int dataCount = result.getModelDataCount(modelName);

                        for (int i = 0; i < dataCount; i++) {
                            ModelData modelData = result.getModelData(
                                    modelName, i);

                            for (int j = 0; j < modelData.getPropertyCount(); j++) {
                                PrintLog("Name: "
                                        + modelData.getPropertyName(j)
                                        + " Value: "
                                        + modelData.getPropertyValue(j));

                            }

                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                        PrintLog("Receiving Event from cqid " + cqid
                                + " failed");
                    }
                }
            }
        };

        clickHandler = new View.OnClickListener() {

            public void onClick(View v) {
                switch (v.getId()) {
                    case R.id.btnRegisterQuery:
                        int cqid = 0;
                        try {
                            cqid = SoftSensorManager.registerQuery(edtQuery
                                    .getText().toString(),
                                    mQueryEngineEventListener);
                            mRunningQueries.add(cqid);
                            PrintLog(edtQuery.getText().toString()
                                    + " has executed, cqid=" + cqid);
                        } catch (Exception e) {
                            PrintLog("Register Query failed");
                        }

                        break;

                    case R.id.btnUnregisterQuery:
                        Iterator<Integer> it = mRunningQueries.iterator();

                        while (it.hasNext()) {
                            if (it.next() == Integer
                                    .parseInt(edtUnregisterQuery.getText()
                                            .toString())) {
                                try {
                                    SoftSensorManager.unregisterQuery(Integer
                                            .parseInt(edtUnregisterQuery
                                                    .getText().toString()));
                                    PrintLog("Unregister Query has executed, cqid="
                                            + Integer
                                                    .parseInt(edtUnregisterQuery
                                                            .getText()
                                                            .toString()));
                                    it.remove();
                                } catch (NumberFormatException e) {
                                    PrintLog("Invalid Query Id");
                                } catch (Exception e) {
                                    PrintLog("UnRegister Query failed");
                                }
                                break;
                            }
                        }
                        break;

                    case R.id.btPlus:
                        int queryNum = 0;

                        try {
                            queryNum = Integer.parseInt(edtUnregisterQuery
                                    .getText().toString()) + 1;
                            edtUnregisterQuery.setText(queryNum + "");
                        } catch (NumberFormatException e) {
                            PrintLog("Invalid Query Id");
                        }
                        break;

                    case R.id.btMinus:
                        try {
                            queryNum = Integer.parseInt(edtUnregisterQuery
                                    .getText().toString()) - 1;
                            edtUnregisterQuery.setText(queryNum + "");
                        } catch (NumberFormatException e) {
                            PrintLog("Invalid Query Id");
                        }
                        break;
                }
            }
        };

        textAddHandler = new View.OnClickListener() {

            public void onClick(View v) {
                switch (v.getId()) {
                    case R.id.btClear:
                        edtQuery.setText("");
                        break;

                    case R.id.btLogClear:
                        tvLog.setText("");
                        break;

                    case R.id.btFullDevice:
                        edtQuery.setText("subscribe Device if Device.dataId != 0");
                        break;

                    case R.id.btDiscomfortIndex:
                        edtQuery.setText("subscribe Device.DiscomfortIndexSensor if Device.DiscomfortIndexSensor.discomfortIndex > 0");
                        break;
                }
            }
        };

        copyFiles("lib");

        SoftSensorManager = new SSMInterface();

        String initConfig = "<SSMCore>" + "<Device>" + "<UDN>" + getUUID()
                + "</UDN>" + "<Name>MyMobile</Name>" + "<Type>Mobile</Type>"
                + "</Device>" + "<Config>"
                + "<SoftSensorRepository>/data/data/" + getPackageName()
                + "/files/</SoftSensorRepository>"
                + "<SoftSensorDescription>/data/data/" + getPackageName()
                + "/files/SoftSensorDescription.xml</SoftSensorDescription>"
                + "</Config>" + "</SSMCore>";

        try {
            SoftSensorManager.startSSMCore(initConfig);
        } catch (Exception e) {
            e.printStackTrace();
        }

        findViewById(R.id.btnRegisterQuery).setOnClickListener(clickHandler);
        findViewById(R.id.btnUnregisterQuery).setOnClickListener(clickHandler);
        findViewById(R.id.btFullDevice).setOnClickListener(textAddHandler);
        findViewById(R.id.btDiscomfortIndex).setOnClickListener(textAddHandler);
        findViewById(R.id.btPlus).setOnClickListener(clickHandler);
        findViewById(R.id.btMinus).setOnClickListener(clickHandler);
        findViewById(R.id.btClear).setOnClickListener(textAddHandler);
        findViewById(R.id.btLogClear).setOnClickListener(textAddHandler);
    }

    @Override
    protected void onDestroy() {
        try {
            SoftSensorManager.stopSSMCore();
        } catch (Exception e) {
            e.printStackTrace();
        }

        super.onDestroy();
    }

    private String getUUID() {
        String PREF_UNIQUE_ID = "PREF_UNIQUE_ID";
        SharedPreferences sharedPrefs = getSharedPreferences(PREF_UNIQUE_ID,
                Context.MODE_PRIVATE);
        String uniqueID = sharedPrefs.getString(PREF_UNIQUE_ID, null);

        if (uniqueID == null) {
            uniqueID = UUID.randomUUID().toString();
            Editor editor = sharedPrefs.edit();
            editor.putString(PREF_UNIQUE_ID, uniqueID);
            editor.commit();
        }

        return uniqueID;
    }

    private void copyFiles(String path) {
        AssetManager assetManager = getAssets();
        String assets[] = null;

        try {
            assets = assetManager.list(path);

            if (assets.length == 0) {
                copyFile(path);
            } else {
                String fullPath = "/data/data/"
                        + this.getClass().getPackage().toString() + "/" + path;
                File dir = new File(fullPath);

                if (!dir.exists())
                    dir.mkdir();
                for (int i = 0; i < assets.length; ++i) {
                    copyFiles(path + "/" + assets[i]);
                }
            }
        } catch (IOException ex) {
            Log.e("tag", "I/O Exception", ex);
        }
    }

    private void copyFile(String filename) {
        AssetManager assetManager = getAssets();
        InputStream in = null;
        OutputStream out = null;

        try {
            in = assetManager.open(filename);
            out = openFileOutput(filename.split("/")[1], Context.MODE_PRIVATE);

            byte[] buffer = new byte[1024];
            int read;

            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }

            in.close();
            in = null;
            out.flush();
            out.close();
            out = null;
        } catch (Exception e) {
            Log.e("tag", e.getMessage());
        }
    }
    
    private void configurePlatform() {
        // local Variables
        ConnectivityManager connManager;
        NetworkInfo wifi;
        AlertDialog dialog;
        PlatformConfig platformConfigObj;

        // Check the wifi connectivity
        connManager = (ConnectivityManager) getSystemService(CONNECTIVITY_SERVICE);
        wifi = connManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
        if (false == wifi.isConnected()) {
            // WiFi is not connected close the application
            AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(this);
            dialogBuilder.setTitle("Error");
            dialogBuilder
                    .setMessage("WiFi is not enabled/connected! Please connect the WiFi and start application again...");
            dialogBuilder.setCancelable(false);
            dialogBuilder.setPositiveButton("OK", new OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    // Closing the application
                    activityObj.finish();
                }
            });

            dialog = dialogBuilder.create();
            dialog.show();
            return;
        }
        // If wifi is connected calling the configure method for configuring the
        // OcPlatform
        platformConfigObj = new PlatformConfig(this,ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW);

        Log.i(LOG_TAG, "Before Calling Configure of ocPlatform");
        OcPlatform.Configure(platformConfigObj);
        Log.i(LOG_TAG, "Configuration done Successfully");
    }
}
