/**
 * ***************************************************************
 * <p/>
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 * <p/>
 * <p/>
 * <p/>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p/>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p/>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * <p/>
 * ****************************************************************
 */

package org.iotivity.service.easysetup;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcProvisioning;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.easysetup.mediator.CloudProvInfo;
import org.iotivity.service.easysetup.mediator.CloudProvisioningCallback;
import org.iotivity.service.easysetup.mediator.CloudProvisioningStatus;
import org.iotivity.service.easysetup.mediator.DataProvInfo;
import org.iotivity.service.easysetup.mediator.DataProvisioningCallback;
import org.iotivity.service.easysetup.mediator.DataProvisioningStatus;
import org.iotivity.service.easysetup.mediator.DeviceConfig;
import org.iotivity.service.easysetup.mediator.ESException;
import org.iotivity.service.easysetup.mediator.EasySetup;
import org.iotivity.service.easysetup.mediator.NetworkInfo;
import org.iotivity.service.easysetup.mediator.PropertyData;
import org.iotivity.service.easysetup.mediator.RemoteEnrollee;
import org.iotivity.service.easysetup.mediator.RequestPropertyDataCallback;
import org.iotivity.service.easysetup.mediator.RequestPropertyDataStatus;
import org.iotivity.service.easysetup.mediator.SecurityProvisioningCallback;
import org.iotivity.service.easysetup.mediator.SecurityProvisioningStatus;
import org.iotivity.service.easysetup.mediator.enums.ESCloudProvState;
import org.iotivity.service.easysetup.mediator.enums.ESResult;
import org.iotivity.service.easysetup.mediator.enums.WIFI_AUTHTYPE;
import org.iotivity.service.easysetup.mediator.enums.WIFI_ENCTYPE;
import org.iotivity.service.easysetup.mediator.enums.WIFI_FREQ;
import org.iotivity.service.easysetup.mediator.enums.WIFI_MODE;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;


public class EasysetupActivity extends Activity {
    private static final String TAG = "Easysetup Mediator: ";
    PlatformConfig cfg;

    private static final int BUFFER_SIZE = 1024;

    private String filePath = "";
    public static final String OIC_CLIENT_JSON_DB_FILE =  "oic_svr_db_client.dat";
    public static final String OIC_SQL_DB_FILE =  "PDM.db";

    private boolean isSecurityEnabled = false;

    ToggleButton mSecurityMode;

    RadioGroup mEasysetupProcess;
    RadioButton mSecProvProcess;
    RadioButton mReqPropertyDataProcess;
    RadioButton mDataProvProcess;
    RadioButton mCloudProvProcess;

    Button mInitRemoteEnrollee;
    Button mStartRequsetPropertyData;
    Button mStartSecProvisioning;
    Button mStartDataProvisioning;
    Button mStartCloudProvisioning;

    TextView mRequesetPropertyDataStateText;
    TextView mDevIDText;
    TextView mDevNameText;
    TextView mLanguageText;
    TextView mCountryText;
    TextView mWifiModeText;
    TextView mWifiFreqText;
    TextView mCloudAccessableText;
    TextView mSecStateText;
    TextView mSecDevIDText;
    TextView mDataProvStateText;
    TextView mCloudProvStaeText;

    EditText mEnrollerSsidText;
    EditText mEnrollerPWText;
    EditText mInputLanguageText;
    EditText mInputCountryText;
    EditText mAuthCodeText;
    EditText mAuthProviderText;
    EditText mCIServerText;

    LinearLayout mRequsetPropertyDataInfo;
    LinearLayout mSecProvisioningState;
    LinearLayout mDataProvisioningInfo;
    LinearLayout mCloudProvisioningInfo;

    Spinner mAuthType;
    Spinner mEncType;

    EasySetup mEasySetup;
    RemoteEnrollee mRemoteEnrollee;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.easysetup_main);

        mSecurityMode = (ToggleButton) findViewById(R.id.btn_Security);

        mEasysetupProcess = (RadioGroup) findViewById(R.id.rg_EasysetupProcess);

        mSecProvProcess = (RadioButton) findViewById(R.id.btn_SecurityProv);
        mReqPropertyDataProcess = (RadioButton) findViewById(R.id.btn_requestPropertyData);
        mDataProvProcess = (RadioButton) findViewById(R.id.btn_DataProv);
        mCloudProvProcess = (RadioButton) findViewById(R.id.btn_CloudProv);

        mInitRemoteEnrollee = (Button) findViewById(R.id.btn_initRemoteEnrollee);
        mStartRequsetPropertyData =
                (Button) findViewById(R.id.btn_startRequsetPropertyData);
        mStartSecProvisioning = (Button) findViewById(R.id.btn_startSecProv);
        mStartDataProvisioning = (Button) findViewById(R.id.btn_startDataProv);
        mStartCloudProvisioning = (Button) findViewById(R.id.btn_startCloudProv);

        mRequesetPropertyDataStateText =
                (TextView) findViewById(R.id.txt_requesetPropertyDataState);
        mDevIDText = (TextView) findViewById(R.id.txt_devID);
        mDevNameText = (TextView) findViewById(R.id.txt_devName);
        mLanguageText = (TextView) findViewById(R.id.txt_language);
        mCountryText = (TextView) findViewById(R.id.txt_country);
        mWifiModeText = (TextView) findViewById(R.id.txt_wifiMode);
        mWifiFreqText = (TextView) findViewById(R.id.txt_wifiFreq);
        mCloudAccessableText = (TextView) findViewById(R.id.txt_cloudAccessable);
        mSecStateText = (TextView) findViewById(R.id.txt_secState);
        mSecDevIDText = (TextView) findViewById(R.id.txt_secDevID);
        mDataProvStateText = (TextView) findViewById(R.id.txt_dataProvisioningState);
        mCloudProvStaeText = (TextView) findViewById(R.id.txt_cloudProvisioningState);

        mEnrollerSsidText = (EditText) findViewById(R.id.editText_EnrollerSSID);
        mEnrollerPWText = (EditText) findViewById(R.id.editText_EnrollerPW);
        mInputLanguageText = (EditText) findViewById(R.id.editText_Language);
        mInputCountryText = (EditText) findViewById(R.id.editText_Country);
        mAuthCodeText = (EditText) findViewById(R.id.editText_authcode);
        mAuthProviderText = (EditText) findViewById(R.id.editText_authprovider);
        mCIServerText = (EditText) findViewById(R.id.editText_ciserver);

        mRequsetPropertyDataInfo =
                (LinearLayout) findViewById(R.id.layout_RequestPropertyData);
        mSecProvisioningState = (LinearLayout) findViewById(R.id.layout_SecProvisioning);
        mDataProvisioningInfo = (LinearLayout) findViewById(R.id.layout_DataProvisioning);
        mCloudProvisioningInfo = (LinearLayout) findViewById(R.id.layout_CloudProvisioning);

        mAuthType = (Spinner) findViewById(R.id.spinner_authType);
        mEncType = (Spinner) findViewById(R.id.spinner_encType);

        mEasysetupProcess.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                mRequsetPropertyDataInfo.setVisibility(View.GONE);
                mSecProvisioningState.setVisibility(View.GONE);
                mDataProvisioningInfo.setVisibility(View.GONE);
                mCloudProvisioningInfo.setVisibility(View.GONE);

                switch (checkedId) {
                    case R.id.btn_SecurityProv:
                        mSecProvisioningState.setVisibility(View.VISIBLE);
                        break;

                    case R.id.btn_requestPropertyData:
                        mRequsetPropertyDataInfo.setVisibility(View.VISIBLE);
                        break;

                    case R.id.btn_DataProv:
                        mDataProvisioningInfo.setVisibility(View.VISIBLE);
                        break;

                    case R.id.btn_CloudProv:
                        mCloudProvisioningInfo.setVisibility(View.VISIBLE);
                        break;
                }
            }
        });

        ArrayAdapter<CharSequence> adAuthType, adEnctype;

        adAuthType = ArrayAdapter.createFromResource(this, R.array.auth_type,
                android.R.layout.simple_spinner_item);
        adAuthType.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        adEnctype = ArrayAdapter.createFromResource(this, R.array.enc_type,
                android.R.layout.simple_spinner_item);
        adEnctype.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        mAuthType.setAdapter(adAuthType);
        mAuthType.setSelection(0);

        mEncType.setAdapter(adEnctype);
        mEncType.setSelection(0);

        addListenerForDiscoverEnrollee();
        addListenerForStartSecProvisioning();
        addListenerForStartRequsetPropertyData();
        addListenerForStartDataProvisioning();
        addListenerForStartCloudProvisioning();

        mSecurityMode.setClickable(false);
        mSecProvProcess.setEnabled(false);
        mReqPropertyDataProcess.setEnabled(false);
        mDataProvProcess.setEnabled(false);
        mCloudProvProcess.setEnabled(false);

        mEasySetup = EasySetup.getInstance(getApplicationContext());
        mRemoteEnrollee = mEasySetup.createRemoteEnrollee();

        initOICStack();
    }

    private void initOICStack() {
        filePath = getFilesDir().getPath() + "/";

        SharedPreferences wmbPreference = PreferenceManager.getDefaultSharedPreferences
                (getApplicationContext());
        boolean isFirstRun = wmbPreference.getBoolean("FIRSTRUN", true);
        if (isFirstRun) {
            if(!copyJsonFromAsset())
            {
                Log.e(TAG, "initOICStack error: " + "copyJsonFromAsset()");
                Toast.makeText(this,"Can't Copy DB file from asset, please retry start SampleApp.",
                    Toast.LENGTH_LONG).show();
                return;
            }
            SharedPreferences.Editor editor = wmbPreference.edit();
            editor.putBoolean("FIRSTRUN", false);
            editor.commit();
        }

        cfg = new PlatformConfig(
                this,
                ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER,
                "0.0.0.0", // bind to all available interfaces
                0,
                QualityOfService.LOW, filePath + OIC_CLIENT_JSON_DB_FILE);
        try {
            /*
             * Initialize DataBase
             */

            OcPlatform.Configure(cfg);

            String sqlDbPath = getFilesDir().getAbsolutePath().replace("files", "databases") +
                    File.separator;
            File file = new File(sqlDbPath);
            //check files directory exists
            if (!(file.isDirectory())) {
                file.mkdirs();
                Log.d(TAG, "Sql db directory created at " + sqlDbPath);
            }
            Log.d(TAG, "Sql db directory exists at " + sqlDbPath);

            //SQLiteDatabase.openOrCreateDatabase(sqlDbPath+ OIC_SQL_DB_FILE, null);
            OcProvisioning.provisionInit(sqlDbPath + OIC_SQL_DB_FILE);
        } catch (OcException e) {
            logMessage(TAG + "provisionInit error: " + e.getMessage());
            Log.e(TAG, e.getMessage());
            Toast.makeText(this,"provisionInit error: " + e.getMessage(),
                    Toast.LENGTH_LONG).show();
        } catch (UnsatisfiedLinkError e) {

            // Note : Easy setup is built with SECURED = 0, but user still selects Security feature
            // while running the Mediator App it couldn't find "libocprovision.so".
            // As per the programmer guide, security feature should be invoked only if build is done with SECURED = 1.
            Log.e(TAG, " Easy setup is built with secured  = 0, but executed with security feature");
            Toast.makeText(this,"Security is not enabled [Easy setup is built with SECURED = 0]",
                    Toast.LENGTH_LONG).show();
            mSecurityMode.setChecked(false);
        }
        mSecurityMode.setChecked(true);
    }

    private void addListenerForDiscoverEnrollee() {
        mInitRemoteEnrollee.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Thread thread = new Thread() {
                    boolean result = true;
                    @Override
                    public void run() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mInitRemoteEnrollee.setEnabled(false);
                            }
                        });

                        try {
                            mRemoteEnrollee.initRemoteEnrollee();
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    mInitRemoteEnrollee.setText("Founded");
                                    mSecProvProcess.setEnabled(true);
                                    mReqPropertyDataProcess.setEnabled(true);
                                    mDataProvProcess.setEnabled(true);
                                    mCloudProvProcess.setEnabled(true);
                                }
                            });
                        }
                        catch (ESException e) {
                            e.printStackTrace();
                            result = false;
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    mInitRemoteEnrollee.setEnabled(true);
                                }
                            });
                        }
                    }
                };

                thread.start();
            }
        });
    }

    private void addListenerForStartSecProvisioning() {
        mStartSecProvisioning.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Thread thread = new Thread() {
                    @Override
                    public void run() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mStartSecProvisioning.setEnabled(false);
                            }
                        });

                        try {
                            mRemoteEnrollee.startSecurityProvisioning(new SecurityProvisioningCallback() {
                                @Override
                                public void onProgress(final SecurityProvisioningStatus securityProvisioningStatus) {
                                    if(securityProvisioningStatus.getESResult() == ESResult.ES_OK) {
                                        runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                mSecStateText.setText("Success");
                                                mSecDevIDText.setText(securityProvisioningStatus.getDevUUID());
                                            }
                                        });
                                    }
                                    else {
                                        runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                mSecStateText.setText("Failed");
                                                mStartSecProvisioning.setEnabled(true);
                                            }
                                        });
                                    }
                                }
                            });
                        } catch (ESException e) {
                            e.printStackTrace();
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    mStartSecProvisioning.setEnabled(true);
                                }
                            });
                        }
                    }
                };

                thread.start();
            }
        });
    }

    private void addListenerForStartRequsetPropertyData(){
        mStartRequsetPropertyData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Thread thread = new Thread() {
                    @Override
                    public void run() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mRequesetPropertyDataStateText.setText("Process");
                                mStartRequsetPropertyData.setEnabled(false);
                            }
                        });

                        try {
                            mRemoteEnrollee.requestPropertyData(new RequestPropertyDataCallback() {
                                @Override
                                public void onProgress(RequestPropertyDataStatus requestPropertyDataStatus) {
                                    if(requestPropertyDataStatus.getESResult() == ESResult.ES_OK) {

                                        final PropertyData propertyData = requestPropertyDataStatus.getPropertyData();
                                        final DeviceConfig devConf = propertyData.getDeviceConfig();
                                        final NetworkInfo netInfo = propertyData.getNetworkInfo();
                                        runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                mRequesetPropertyDataStateText.setText("Success");
                                                mDevIDText.setText(devConf.getId());
                                                mDevNameText.setText(devConf.getName());
                                                mLanguageText.setText(devConf.getLanguage());
                                                mCountryText.setText(devConf.getCountry());
                                                setWifiModeTypes(netInfo.getWifiModeTypes());
                                                setWifiFreq(netInfo.getWifiFreq());

                                                if(propertyData.isCloudable()) {
                                                    mCloudAccessableText.setText("TRUE");
                                                }
                                                else {
                                                    mCloudAccessableText.setText("FALSE");
                                                }
                                            }
                                        });

                                    }
                                    else {
                                        runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                mRequesetPropertyDataStateText.setText("Failed");
                                                mStartRequsetPropertyData.setEnabled(true);
                                            }
                                        });
                                    }
                                }
                            });
                        } catch (ESException e) {
                            e.printStackTrace();
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    mRequesetPropertyDataStateText.setText("Failed");
                                    mStartRequsetPropertyData.setEnabled(true);
                                }
                            });
                        }
                    }
                };

                thread.start();
            }
        });
    }

    private void addListenerForStartDataProvisioning() {
        mStartDataProvisioning.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Thread thread = new Thread() {
                    @Override
                    public void run() {
                        try {
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    mDataProvStateText.setText("Progress");
                                    mStartDataProvisioning.setEnabled(false);
                                }
                            });

                            String enrollerSSID = mEnrollerSsidText.getText().toString();
                            String enrollerPW = mEnrollerPWText.getText().toString();
                            WIFI_AUTHTYPE authType =
                                    WIFI_AUTHTYPE.fromInt(mAuthType.getSelectedItemPosition());
                            WIFI_ENCTYPE encType =
                                    WIFI_ENCTYPE.fromInt(mEncType.getSelectedItemPosition());
                            String inputLanguage = mInputLanguageText.getText().toString();
                            String inputCountry = mInputCountryText.getText().toString();

                            DataProvInfo dataProvInfo =
                                    new DataProvInfo(enrollerSSID, enrollerPW, authType, encType,
                                                    inputLanguage, inputCountry);

                            mRemoteEnrollee.setDataProvInfo(dataProvInfo);

                            mRemoteEnrollee.startDataProvisioning(new DataProvisioningCallback() {
                                @Override
                                public void onProgress(DataProvisioningStatus dataProvisioningStatus) {
                                    final ESResult result = dataProvisioningStatus.getESResult();
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            if(result.equals(ESResult.ES_OK)) {
                                                mDataProvStateText.setText("Success");
                                            }
                                            else if(result.equals(ESResult.ES_ERROR)) {
                                                mDataProvStateText.setText("Failed");
                                            }
                                            else if(result.equals(ESResult.ES_UNAUTHORIZED)) {
                                                mDataProvStateText.setText("Failed. Need SecProv");
                                            }
                                            mStartDataProvisioning.setEnabled(true);
                                        }
                                    });
                                }
                            });
                        } catch (ESException e) {
                            e.printStackTrace();
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    mDataProvStateText.setText("Failed");
                                    mStartDataProvisioning.setEnabled(true);
                                }
                            });
                        }
                    }
                };

                thread.start();
            }
        });
    }

    private void addListenerForStartCloudProvisioning() {
        mStartCloudProvisioning.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Thread thread = new Thread() {
                    @Override
                    public void run() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mCloudProvStaeText.setText("Progress");
                                mStartCloudProvisioning.setEnabled(false);
                            }
                        });

                        try {
                            String authCode = mAuthCodeText.getText().toString();
                            String authProvider = mAuthProviderText.getText().toString();
                            String ciserver = mCIServerText.getText().toString();

                            CloudProvInfo cloudProvInfo =
                                    new CloudProvInfo(authCode, authProvider, ciserver);
                            mRemoteEnrollee.setCloudProvInfo(cloudProvInfo);

                            mRemoteEnrollee.startCloudProvisioning(new CloudProvisioningCallback() {
                                @Override
                                public void onProgress(CloudProvisioningStatus cloudProvisioningStatus) {
                                    final ESResult result = cloudProvisioningStatus.getESResult();
                                    final ESCloudProvState state = cloudProvisioningStatus.getESCloudState();
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            if(result.equals(ESResult.ES_OK)) {
                                                if(state.equals(ESCloudProvState.ES_CLOUD_ENROLLEE_FOUND)) {
                                                    mCloudProvStaeText.setText("Found Resource");
                                                }
                                                else if(state.equals(ESCloudProvState.ES_CLOUD_PROVISIONING_SUCCESS)) {
                                                    mCloudProvStaeText.setText("Success");
                                                }
                                            }
                                            else {
                                                if(state.equals(ESCloudProvState.ES_CLOUD_ENROLLEE_NOT_FOUND)) {
                                                    mCloudProvStaeText.setText("Not Found Resource");
                                                }
                                                else if(state.equals(ESCloudProvState.ES_CLOUD_PROVISIONING_ERROR)) {
                                                    mCloudProvStaeText.setText("Failed");
                                                }
                                                mStartCloudProvisioning.setEnabled(true);
                                            }
                                        }
                                    });
                                }
                            });
                        } catch (ESException e) {
                            e.printStackTrace();
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    mCloudProvStaeText.setText("Failed");
                                    mStartCloudProvisioning.setEnabled(true);
                                }
                            });
                        }
                    }
                };

                thread.start();
            }
        });
    }

    private boolean copyJsonFromAsset() {
        InputStream inputStream = null;
        OutputStream outputStream = null;
        int length;
        byte[] buffer = new byte[BUFFER_SIZE];
        try {
            inputStream = getAssets().open(OIC_CLIENT_JSON_DB_FILE);
            File file = new File(filePath);
            //check files directory exists
            if (!(file.exists() && file.isDirectory())) {
                file.mkdirs();
            }
            outputStream = new FileOutputStream(filePath + OIC_CLIENT_JSON_DB_FILE);
            while ((length = inputStream.read(buffer)) != -1) {
                outputStream.write(buffer, 0, length);
            }
        } catch (NullPointerException e) {
            logMessage(TAG + "Null pointer exception " + e.getMessage());
            Log.e(TAG, e.getMessage());
            return false;
        } catch (FileNotFoundException e) {
            logMessage(TAG + "Json svr db file not found " + e.getMessage());
            Log.e(TAG, e.getMessage());
            return false;
        } catch (IOException e) {
            logMessage(TAG + OIC_CLIENT_JSON_DB_FILE + " file copy failed");
            Log.e(TAG, e.getMessage());
            return false;
        } finally {
            if (inputStream != null) {
                try {
                    inputStream.close();
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                    return false;
                }
            }
            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                    return false;
                }
            }
        }
        return true;
    }

    public void logMessage(String text) {

    }

    public void setWifiModeTypes(ArrayList<WIFI_MODE> types) {
        String temp = "WIFI - ";

        for(WIFI_MODE type : types) {
            if(type.equals(WIFI_MODE.WIFI_11A)) {
                temp = temp + "11A ";
            }
            else if(type.equals(WIFI_MODE.WIFI_11B)) {
                temp = temp + "11B ";
            }
            else if(type.equals(WIFI_MODE.WIFI_11G)) {
                temp = temp + "11G ";
            }
            else if(type.equals(WIFI_MODE.WIFI_11N)) {
                temp = temp + "11N ";
            }
            else if(type.equals(WIFI_MODE.WIFI_11AC)) {
                temp = temp + "11AC ";
            }
        }
        final String modeTypes = temp;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mWifiModeText.setText(modeTypes);
            }
        });
    }

    public void setWifiFreq(final WIFI_FREQ freq) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(freq.equals(WIFI_FREQ.WIFI_24G)) {
                    mWifiFreqText.setText("2.4G");
                }
                else if(freq.equals(WIFI_FREQ.WIFI_5G)) {
                    mWifiFreqText.setText("5G");
                }
                else if(freq.equals(WIFI_FREQ.WIFI_BOTH)) {
                    mWifiFreqText.setText("2.4G & 5G");
                }
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }
}
