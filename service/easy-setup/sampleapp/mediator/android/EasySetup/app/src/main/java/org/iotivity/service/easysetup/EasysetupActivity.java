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
import android.content.Intent;
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

import org.iotivity.base.ErrorCode;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcAccountManager;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPresenceStatus;
import org.iotivity.base.OcProvisioning;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.easysetup.mediator.CloudProp;
import org.iotivity.service.easysetup.mediator.CloudPropProvisioningCallback;
import org.iotivity.service.easysetup.mediator.CloudPropProvisioningStatus;
import org.iotivity.service.easysetup.mediator.DeviceProp;
import org.iotivity.service.easysetup.mediator.DevicePropProvisioningCallback;
import org.iotivity.service.easysetup.mediator.DevicePropProvisioningStatus;
import org.iotivity.service.easysetup.mediator.ESConstants;
import org.iotivity.service.easysetup.mediator.ESException;
import org.iotivity.service.easysetup.mediator.EasySetup;
import org.iotivity.service.easysetup.mediator.EnrolleeConf;
import org.iotivity.service.easysetup.mediator.GetConfigurationCallback;
import org.iotivity.service.easysetup.mediator.GetConfigurationStatus;
import org.iotivity.service.easysetup.mediator.RemoteEnrollee;
import org.iotivity.service.easysetup.mediator.SecurityProvisioningCallback;
import org.iotivity.service.easysetup.mediator.SecurityProvisioningStatus;
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
import java.util.Arrays;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

public class EasysetupActivity extends Activity
                                implements OcPlatform.OnPresenceListener,
                                           OcResource.OnObserveListener{
    private static final String TAG = "Easysetup Mediator: ";
    private static final String SUCCESS = "Success";
    private static final String FAILED = "Failed";
    private static final String ACCESSTOKEN = "accesstoken";
    private static final String REFRESHTOKEN = "refreshtoken";
    private static final String COMMUNICATION_ERROR = "Communication Error";
    private static final String deviceID = "9E09F4FE-978A-4BC3-B356-1F93BCA37829";
    private static final String CIServer = "coap+tcp://13.124.29.169:5683";
    PlatformConfig cfg;
    OcAccountManager m_accountManager = null;
    private static final int BUFFER_SIZE = 1024;
    private String filePath = "";
    public static final String OIC_CLIENT_JSON_DB_FILE =  "oic_svr_db_client.dat";
    public static final String OIC_SQL_DB_FILE =  "PDM.db";

    private boolean isFirstTime = true;

    String mEnrolleeDeviceID;
    String mAuthCode;
    String mAuthProvider;
    String mRefreshtoken;
    String mUserID;
    String mAccessToken;
    String mEnrolleeAuthCode;
    byte[] mCertificate;
    int mCredID;

    ToggleButton mSecurityMode;

    RadioGroup mEasysetupProcess;
    RadioButton mConfigureSecProcess;
    RadioButton mGetConfigurationProcess;
    RadioButton mProvisionDevPropProcess;
    RadioButton mProvisionCloudPropProcess;

    Button mDiscoverResource;
    Button mStartGetConfiguration;
    Button mStartConfigureSec;
    Button mStartProvisionDevProp;
    Button mStartProvisionCloudProp;

    TextView mGetconfigurationStateText;
    TextView mDevNameText;
    TextView mModelNumberText;
    TextView mLanguageText;
    TextView mCountryText;
    TextView mWifiModeText;
    TextView mWifiFreqText;
    TextView mCloudAccessableText;
    TextView mSecStateText;
    TextView mSecDevIDText;
    TextView mProvisionDevPropState;
    TextView mProvisionCloudPropState;

    EditText mEnrollerSsidText;
    EditText mEnrollerPWText;
    EditText mInputLanguageText;
    EditText mInputCountryText;
    EditText mInputLocationText;
    EditText mAuthCodeText;
    EditText mAuthProviderText;
    EditText mCIServerText;

    LinearLayout mGetConfigurationInfo;
    LinearLayout mConfigureSecInfo;
    LinearLayout mProvisionDevPropInfo;
    LinearLayout mProvisionCloudPropInfo;

    Spinner mAuthType;
    Spinner mEncType;

    EasySetup mEasySetup;
    RemoteEnrollee mRemoteEnrollee;

    Activity mActivity;
    Context mContext;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.easysetup_main);

        mActivity = EasysetupActivity.this;
        mContext = mActivity.getBaseContext();

        mSecurityMode = (ToggleButton) findViewById(R.id.btn_Security);

        mEasysetupProcess = (RadioGroup) findViewById(R.id.rg_EasysetupProcess);

        mConfigureSecProcess = (RadioButton) findViewById(R.id.btn_configurSec);
        mGetConfigurationProcess = (RadioButton) findViewById(R.id.btn_getConfiguration);
        mProvisionDevPropProcess = (RadioButton) findViewById(R.id.btn_provisionDevConf);
        mProvisionCloudPropProcess =
                (RadioButton) findViewById(R.id.btn_provisionCloudConf);

        mDiscoverResource = (Button) findViewById(R.id.btn_discoverResource);
        mStartGetConfiguration =
                (Button) findViewById(R.id.btn_startGetConfiguration);
        mStartConfigureSec = (Button) findViewById(R.id.btn_startConfigureSec);
        mStartProvisionDevProp = (Button) findViewById(R.id.btn_startProvisionDevConf);
        mStartProvisionCloudProp = (Button) findViewById(R.id.btn_startProvisionCloudConf);

        mGetconfigurationStateText =
                (TextView) findViewById(R.id.txt_getConfigurationState);
        mDevNameText = (TextView) findViewById(R.id.txt_devName);
        mModelNumberText = (TextView) findViewById(R.id.txt_modelNumber);
        mLanguageText = (TextView) findViewById(R.id.txt_language);
        mCountryText = (TextView) findViewById(R.id.txt_country);
        mWifiModeText = (TextView) findViewById(R.id.txt_wifiMode);
        mWifiFreqText = (TextView) findViewById(R.id.txt_wifiFreq);
        mCloudAccessableText = (TextView) findViewById(R.id.txt_cloudAccessable);
        mSecStateText = (TextView) findViewById(R.id.txt_secState);
        mSecDevIDText = (TextView) findViewById(R.id.txt_secDevID);
        mProvisionDevPropState = (TextView) findViewById(R.id.txt_provisionDevConfState);
        mProvisionCloudPropState =
                (TextView) findViewById(R.id.txt_provisionCloudConfState);

        mEnrollerSsidText = (EditText) findViewById(R.id.editText_EnrollerSSID);
        mEnrollerPWText = (EditText) findViewById(R.id.editText_EnrollerPW);
        mInputLanguageText = (EditText) findViewById(R.id.editText_Language);
        mInputCountryText = (EditText) findViewById(R.id.editText_Country);
        mInputLocationText = (EditText) findViewById(R.id.editText_Location);
        mAuthCodeText = (EditText) findViewById(R.id.editText_authcode);
        mAuthProviderText = (EditText) findViewById(R.id.editText_authprovider);
        mCIServerText = (EditText) findViewById(R.id.editText_ciserver);

        mGetConfigurationInfo =
                (LinearLayout) findViewById(R.id.layout_GetConfiguration);
        mConfigureSecInfo = (LinearLayout) findViewById(R.id.layout_ConfigurSec);
        mProvisionDevPropInfo = (LinearLayout) findViewById(R.id.layout_ProvisionDevConf);
        mProvisionCloudPropInfo = (LinearLayout) findViewById(R.id.layout_ProvisionCloudConf);

        mAuthType = (Spinner) findViewById(R.id.spinner_authType);
        mEncType = (Spinner) findViewById(R.id.spinner_encType);

        mEasysetupProcess.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                mGetConfigurationInfo.setVisibility(View.GONE);
                mConfigureSecInfo.setVisibility(View.GONE);
                mProvisionDevPropInfo.setVisibility(View.GONE);
                mProvisionCloudPropInfo.setVisibility(View.GONE);

                switch (checkedId) {
                    case R.id.btn_configurSec:
                        mConfigureSecInfo.setVisibility(View.VISIBLE);
                        break;

                    case R.id.btn_getConfiguration:
                        mGetConfigurationInfo.setVisibility(View.VISIBLE);
                        break;

                    case R.id.btn_provisionDevConf:
                        mProvisionDevPropInfo.setVisibility(View.VISIBLE);
                        break;

                    case R.id.btn_provisionCloudConf:
                        Log.d(TAG, "Starting login activity");
                        Intent intent = new Intent(EasysetupActivity.this, LoginActivity.class);
                        startActivityForResult(intent, 2);
                        mProvisionCloudPropInfo.setVisibility(View.VISIBLE);
                        break;

                    default:
                        break;
                }
            }
        });

        ArrayAdapter<CharSequence> adAuthType;
        ArrayAdapter<CharSequence> adEnctype;

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
        addListenerForStartConfigureSec();
        addListenerForStartGetConfiguration();
        addListenerForStartProvisionDevProp();
        addListenerForStartProvisionCloudProp();

        mSecurityMode.setClickable(false);
        mConfigureSecProcess.setEnabled(false);
        mGetConfigurationProcess.setEnabled(false);
        mProvisionDevPropProcess.setEnabled(false);
        mProvisionCloudPropProcess.setEnabled(false);

        mEasySetup = EasySetup.getInstance(getApplicationContext());

        initOICStack();

        try {
            m_accountManager = OcPlatform.constructAccountManagerObject(
                    CIServer,
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP));

            Log.e(TAG, "constructAccountManagerObject is successful");
        } catch (OcException e) {
            Log.e(TAG, e.toString());
            Log.e(TAG,"Failed to constructAccountManagerObject");
        }
        SharedPreferences settings =
                                getApplicationContext().getSharedPreferences("IoTivityCloud", 0);
        mAccessToken = settings.getString(ACCESSTOKEN, null);
        mRefreshtoken = settings.getString(REFRESHTOKEN, null);
        mUserID = settings.getString("uid", null);

        if(mRefreshtoken == null)
        {
            Log.d(TAG, "Can not find refresh token");
        }

        if(mAccessToken == null && mRefreshtoken == null)
        {
            Log.d(TAG, "Starting login activity");
            Intent intent = new Intent(EasysetupActivity.this, LoginActivity.class);
            startActivityForResult(intent, 1);
        }
        else if(mAccessToken != null)
        {
            SignInDevice();
        }
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
                QualityOfService.HIGH, filePath + OIC_CLIENT_JSON_DB_FILE);
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

            OcProvisioning.provisionInit(sqlDbPath + OIC_SQL_DB_FILE);
            mSecurityMode.setChecked(true);
        } catch (OcException e) {
            Log.e(TAG, "provisionInit error: " + e.getMessage());
            Toast.makeText(this,"provisionInit error: " + e.getMessage(),
                    Toast.LENGTH_LONG).show();
            mSecurityMode.setChecked(false);
            return;
        } catch (UnsatisfiedLinkError e) {

            // Note : Easy setup is built with SECURED = 0, but user still selects Security feature
            // while running the Mediator App it couldn't find "libocprovision.so".
            // As per the programmer guide, security feature should be invoked only if build is done with SECURED = 1.
            mSecurityMode.setChecked(false);
            Log.e(TAG, " Easy setup is built with secured  = 0, but executed with security feature");
            Toast.makeText(this,"Security is not enabled [Easy setup is built with SECURED = 0]",
                    Toast.LENGTH_LONG).show();
            return;
        }
    }

    private void closeIOCStack() {
        try {
            /*
             * Close DataBase
             */
            OcProvisioning.provisionClose();
        } catch (OcException e) {
            Log.e(TAG, "provisionClose error: " + e.getMessage());
        } catch (UnsatisfiedLinkError e) {
            // Note : Easy setup is built with SECURED = 0, but user still selects Security feature
            // while running the Mediator App it couldn't find "libocprovision.so".
            // As per the programmer guide, security feature should be invoked only if build is done with SECURED = 1.
            mSecurityMode.setChecked(false);
            Log.e(TAG, "Easy setup is built with secured = 0, but executed with security feature");
            return;
        }
    }

    private String getSecureUDPEndpoint(OcResource ocResource) {
        if(null == ocResource) {
            return null;
        }

        String udpEndpoint = null;
        List<String> endpoints = ocResource.getAllHosts();
        if(null != endpoints && !endpoints.isEmpty())
        {
            Log.d(TAG, "Endpoints of the resource: " + endpoints);
            Iterator<String> itr = endpoints.iterator();
            while(itr.hasNext())
            {
                String endpoint = itr.next();
                if(null != endpoint && endpoint.contains("coaps://"))
                {
                    udpEndpoint = endpoint;
                    Log.d(TAG, "Found coaps endpoint: setHost() returned " + udpEndpoint);
                    break;
                }
            }
        }
        else
        {
            Log.d(TAG, "No endpoints found");
        }
        return udpEndpoint;
    }

    OcPlatform.OnResourceFoundListener listener =
            new OcPlatform.OnResourceFoundListener() {
                @Override
                public void onFindResourceFailed(Throwable throwable, String s) {
                    Log.e(TAG, "Failed found resource, ecode: " + s);
                }
                @Override
                public void onResourceFound(OcResource ocResource) {
                    synchronized (mActivity) {
                        if(isFirstTime){
                            if (null == ocResource) {
                                Log.e(TAG, "Found resource is invalid");
                                return;
                            }

                            if(ocResource.getHost().contains("+tcp")) {
                                Log.d(TAG, "Recv Found resource event  from tcp port," +
                                    "ignoring URI : " + ocResource.getUri());
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        mDiscoverResource.setEnabled(true);
                                    }
                                });
                                return;
                            }

                            // Get the resource URI
                            String resourceUri = ocResource.getUri();
                            // Get the resource host address
                            String hostAddress = ocResource.getHost();
                            Log.d(TAG,"URI of the resource: " + resourceUri);
                            Log.d(TAG,"Host address of the resource: " + hostAddress);

                            if(mSecurityMode.isChecked())
                            {
                                String endpoint = getSecureUDPEndpoint(ocResource);
                                if(null != endpoint && !endpoint.isEmpty()) {
                                    // Change the host of the resource to secure endpoint.
                                    ocResource.setHost(endpoint);
                                }
                            }

                            mRemoteEnrollee = mEasySetup.createRemoteEnrollee(ocResource);

                            if(mRemoteEnrollee == null) {
                                Log.d(TAG, "Recv Found resource event," +
                                    "ignoring URI : " + ocResource.getUri());
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        mDiscoverResource.setEnabled(true);
                                    }
                                });
                                return;
                            }

                            isFirstTime = false;
                            mEnrolleeDeviceID = ocResource.getServerId();

                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    mDiscoverResource.setText("Found");
                                    if(mSecurityMode.isChecked()) {
                                        mConfigureSecProcess.setEnabled(true);
                                    }
                                    mGetConfigurationProcess.setEnabled(true);
                                    mProvisionDevPropProcess.setEnabled(true);
                                    mProvisionCloudPropProcess.setEnabled(true);
                                }
                            });
                        }
                    }
                }
            };

    private void addListenerForDiscoverEnrollee() {
        mDiscoverResource.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Thread thread = new Thread() {
                    boolean result = true;
                    @Override
                    public void run() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mDiscoverResource.setEnabled(false);
                            }
                        });

                        try {
                            String requestUri = OcPlatform.WELL_KNOWN_QUERY + "?rt=" + ESConstants.OC_RSRVD_ES_RES_TYPE_EASYSETUP;
                            OcPlatform.findResource("",
                                    requestUri,
                                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                                    listener
                            );
                        }
                        catch (OcException e) {
                            e.printStackTrace();
                            result = false;
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    mDiscoverResource.setEnabled(true);
                                }
                            });
                        }
                    }
                };

                thread.start();
            }
        });
    }

    private void addListenerForStartConfigureSec() {
        mStartConfigureSec.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Thread thread = new Thread() {
                    @Override
                    public void run() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mStartConfigureSec.setEnabled(false);
                            }
                        });

                        try {
                            mRemoteEnrollee.provisionSecurity(new SecurityProvisioningCallback() {
                                @Override
                                public void onProgress(final SecurityProvisioningStatus securityProvisioningStatus) {
                                    if(securityProvisioningStatus.getESResult() == ESResult.ES_OK) {
                                        runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                mSecStateText.setText(SUCCESS);
                                                mSecDevIDText.setText(securityProvisioningStatus.getDevUUID());
                                            }
                                        });
                                    }
                                    else if(securityProvisioningStatus.getESResult()
                                            == ESResult.ES_SECURE_RESOURCE_DISCOVERY_FAILURE) {
                                        runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                mSecStateText.setText("Not found Secure Resource");
                                                mStartConfigureSec.setEnabled(true);
                                            }
                                        });
                                    }
                                    else if(securityProvisioningStatus.getESResult()
                                            == ESResult.ES_OWNERSHIP_TRANSFER_FAILURE) {
                                        runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                mSecStateText.setText("Ownership transfer failed");
                                                mStartConfigureSec.setEnabled(true);
                                            }
                                        });
                                    }
                                    else {
                                        runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                mSecStateText.setText(FAILED);
                                                mStartConfigureSec.setEnabled(true);
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
                                    mStartConfigureSec.setEnabled(true);
                                }
                            });
                        }
                    }
                };

                thread.start();
            }
        });
    }

    private void addListenerForStartGetConfiguration(){
        mStartGetConfiguration.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Thread thread = new Thread() {
                    @Override
                    public void run() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mGetconfigurationStateText.setText("Process");
                                mStartGetConfiguration.setEnabled(false);
                            }
                        });

                        try {
                            mRemoteEnrollee.getConfiguration(new GetConfigurationCallback() {
                                @Override
                                public void onProgress(GetConfigurationStatus getConfigurationStatus) {
                                    if(getConfigurationStatus.getESResult() == ESResult.ES_OK) {
                                        final EnrolleeConf enrolleeConf = getConfigurationStatus.getEnrolleeConf();
                                        runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                mGetconfigurationStateText.setText(SUCCESS);
                                                mDevNameText.setText(enrolleeConf.getDeviceName());
                                                mModelNumberText.setText(enrolleeConf.getModelNumber());
                                                setWifiModes(enrolleeConf.getWiFiModes());
                                                setWifiFreq(enrolleeConf.getWiFiFreq());

                                                if(enrolleeConf.isCloudAccessible()) {
                                                    mCloudAccessableText.setText("TRUE");
                                                }
                                                else {
                                                    mCloudAccessableText.setText("FALSE");
                                                }
                                            }
                                        });
                                    }
                                    else if(getConfigurationStatus.getESResult() == ESResult.ES_COMMUNICATION_ERROR)
                                    {
                                        runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                mGetconfigurationStateText.setText(COMMUNICATION_ERROR);
                                                mStartGetConfiguration.setEnabled(true);
                                            }
                                        });
                                    }
                                    else {
                                        runOnUiThread(new Runnable() {
                                            @Override
                                            public void run() {
                                                mGetconfigurationStateText.setText(FAILED);
                                                mStartGetConfiguration.setEnabled(true);
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
                                    mGetconfigurationStateText.setText(FAILED);
                                    mStartGetConfiguration.setEnabled(true);
                                }
                            });
                        }
                    }
                };

                thread.start();
            }
        });
    }

    private void addListenerForStartProvisionDevProp() {
        mStartProvisionDevProp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Thread thread = new Thread() {
                    @Override
                    public void run() {
                        try {
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    mProvisionDevPropState.setText("Progress");
                                    mStartProvisionDevProp.setEnabled(false);
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
                            String inputLocation = mInputLocationText.getText().toString();

                            DeviceProp deviceProp = new DeviceProp();
                            deviceProp.setWiFiProp(enrollerSSID, enrollerPW, authType, encType);
                            deviceProp.setDevConfProp(inputLanguage, inputCountry, inputLocation);

                            mRemoteEnrollee.provisionDeviceProperties(deviceProp, new DevicePropProvisioningCallback() {
                                @Override
                                public void onProgress(DevicePropProvisioningStatus devPropProvisioningStatus) {
                                    final ESResult result = devPropProvisioningStatus.getESResult();
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            if(result.equals(ESResult.ES_OK)) {
                                                mProvisionDevPropState.setText(SUCCESS);
                                            }
                                            else if(result.equals(ESResult.ES_ERROR)) {
                                                mProvisionDevPropState.setText(FAILED);
                                            }
                                            else if(result.equals(ESResult.ES_COMMUNICATION_ERROR)) {
                                                mProvisionDevPropState.setText(COMMUNICATION_ERROR);
                                            }
                                            mStartProvisionDevProp.setEnabled(true);
                                        }
                                    });
                                }
                            });
                        } catch (ESException e) {
                            e.printStackTrace();
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    mProvisionDevPropState.setText(FAILED);
                                    mStartProvisionDevProp.setEnabled(true);
                                }
                            });
                        }
                    }
                };

                thread.start();
            }
        });
    }

    private void addListenerForStartProvisionCloudProp() {
        mStartProvisionCloudProp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Thread thread = new Thread() {
                    @Override
                    public void run() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mProvisionCloudPropState.setText("Progress");
                                mStartProvisionCloudProp.setEnabled(false);
                            }
                        });

                        try {
                            String authCode = mAuthCodeText.getText().toString();
                            String authProvider = mAuthProviderText.getText().toString();
                            String ciserver = mCIServerText.getText().toString();

                            CloudProp cloudProp = new CloudProp();
                            cloudProp.setCloudProp(authCode, authProvider, ciserver);
                            cloudProp.setCloudID("f002ae8b-c42c-40d3-8b8d-1927c17bd1b3");
                            cloudProp.setCredID(1);

                            mRemoteEnrollee.provisionCloudProperties(cloudProp, new CloudPropProvisioningCallback() {
                                @Override
                                public void onProgress(CloudPropProvisioningStatus cloudProvisioningStatus) {
                                    final ESResult result = cloudProvisioningStatus.getESResult();
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            if(result.equals(ESResult.ES_ENROLLEE_DISCOVERY_FAILURE)) {
                                                mProvisionCloudPropState.setText("Not Found Resource");
                                            }
                                            else if(result.equals(ESResult.ES_OK)) {
                                                mProvisionCloudPropState.setText("Cloud Provisioning succeeds");
                                            }
                                            else if(result.equals(ESResult.ES_ACL_PROVISIONING_FAILURE)){
                                                mProvisionCloudPropState.setText("ACL-provisioning fails");
                                            }
                                            else if(result.equals(ESResult.ES_CERT_PROVISIONING_FAILURE)){
                                                mProvisionCloudPropState.setText("CERT-provisioning fails");
                                            }
                                            else if(result.equals(ESResult.ES_COMMUNICATION_ERROR)){
                                                mProvisionCloudPropState.setText(COMMUNICATION_ERROR);
                                            }
                                            else {
                                                mProvisionCloudPropState.setText("Cloud Provisioning fails");
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
                                    mProvisionCloudPropState.setText(FAILED);
                                    mStartProvisionCloudProp.setEnabled(true);
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
        boolean retVal = true;
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
            Log.e(TAG, "Null pointer exception: " + e.getMessage());
            retVal = false;
        } catch (FileNotFoundException e) {
            Log.e(TAG, "Json svr db file not found: " + e.getMessage());
            retVal = false;
        } catch (IOException e) {
            Log.e(TAG, OIC_CLIENT_JSON_DB_FILE + " file copy failed: " + e.getMessage());
            retVal = false;
        } finally {
            if (inputStream != null) {
                try {
                    inputStream.close();
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                    retVal = false;
                }
            }
            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                    retVal = false;
                }
            }
        }
        return retVal;
    }

    public void setWifiModes(ArrayList<WIFI_MODE> types) {
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
    public void onPresence(OcPresenceStatus status, int sequence, String host) {
        final String strStaus = status.getValue();
        Log.d(TAG, "Presence response: " + strStaus + " sequence: " + sequence + " host: " + host);
        runOnUiThread(new Runnable()
        {
            @Override
            public void run() {
                Toast.makeText(EasysetupActivity.this, "Easy-Setup completed", Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if(resultCode==RESULT_OK && requestCode==1){
            mAuthCode = data.getStringExtra("authCode");
            mAuthProvider = data.getStringExtra("authProvider");
            String text = "Received authCode= " + mAuthCode;
            Log.d(TAG, text);
            SignUpDevice();
        }
        else if(resultCode==RESULT_OK && requestCode==2)
        {
            mEnrolleeAuthCode = data.getStringExtra("authCode");
            mAuthProvider = data.getStringExtra("authProvider");
            mAuthCodeText.setText(mEnrolleeAuthCode);
            mAuthProviderText.setText(mAuthProvider);
            mAuthCodeText.setEnabled(false);
            mAuthProviderText.setEnabled(false);

            subscribeDevicePresence();
        }
    }

    OcResource.OnPostListener onRefreshTokenPost = new OcResource.OnPostListener() {
        @Override
        public void onPostCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
            Log.d(TAG, "onRefreshTokenPost..");
            try {
                mAccessToken = ocRepresentation.getValue(ACCESSTOKEN);
                mRefreshtoken = ocRepresentation.getValue(REFRESHTOKEN);

                saveCloudTokenAtSharedPreferences();
            }
            catch (OcException e)
            {
                e.printStackTrace();
            }

            SignInDevice();
        }

        @Override
        public void onPostFailed(Throwable throwable) {
            Log.d(TAG, "onRefreshTokenPost failed..");
        }
    };

    public void RefreshToken() {
        try {
            OcResource authResource = OcPlatform.constructResourceObject(CIServer, "/.well-known/ocf/account/tokenrefresh",
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP, OcConnectivityType.CT_IP_USE_V4),
                    false, Arrays.asList("oic.wk.account"), Arrays.asList(OcPlatform.DEFAULT_INTERFACE));
            OcRepresentation rep = new OcRepresentation();

            runOnUiThread(new Runnable()
            {
                @Override
                public void run() {
                    Toast.makeText(EasysetupActivity.this, "RefreshToken in progress..", Toast.LENGTH_SHORT).show();
                }
            });

            rep.setValue("di", deviceID);
            rep.setValue("granttype", "refresh_token");
            rep.setValue(REFRESHTOKEN, mRefreshtoken);
            rep.setValue("uid", mUserID);
            authResource.post(rep, new HashMap<String, String>(), onRefreshTokenPost);
        }
        catch(OcException e)
        {
            e.printStackTrace();
        }

        Log.d(TAG, "No error while executing login");
    }

    private void saveCloudTokenAtSharedPreferences() {
        Log.d(TAG, "accesstoken: " + mAccessToken);
        SharedPreferences settings = getApplicationContext().getSharedPreferences("IoTivityCloud", 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString(ACCESSTOKEN, mAccessToken);
        editor.putString(REFRESHTOKEN, mRefreshtoken);
        editor.putString("uid", mUserID);

        if(editor.commit())
            Log.d(TAG, "accesstoken saved");
        else
            Log.d(TAG, "accesstoken not saved");
    }

    OcAccountManager.OnPostListener onSignUpPost = new OcAccountManager.OnPostListener() {
        @Override
        public void onPostCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
            Log.d(TAG, "onSignUpPost..");
            try {
                runOnUiThread(new Runnable()
                {
                    @Override
                    public void run() {
                        Toast.makeText(EasysetupActivity.this, "Sign-up completed", Toast.LENGTH_SHORT).show();
                    }
                });

                mAccessToken = ocRepresentation.getValue(ACCESSTOKEN);
                mUserID = ocRepresentation.getValue("uid");

                if(mAccessToken != null)
                {
                    saveCloudTokenAtSharedPreferences();
                    SignInDevice();
                }
                //TODO : save certificate
            }
            catch (OcException e)
            {
                e.printStackTrace();
            }
        }

        @Override
        public void onPostFailed(Throwable throwable) {
            Log.d(TAG, "onSignUpPost failed.. : " + throwable.getMessage());
        }
    };

    private void SignUpDevice() {
        try {
            Log.d(TAG, "SignUpDevice..");

            runOnUiThread(new Runnable()
            {
                @Override
                public void run() {
                    Toast.makeText(EasysetupActivity.this, "SignUpDevice in progress..", Toast.LENGTH_SHORT).show();
                }
            });

            if(m_accountManager != null) {
                m_accountManager.signUp(mAuthProvider, mAuthCode, onSignUpPost);
            }
        }
        catch(OcException e)
        {
            e.printStackTrace();
        }

        Log.d(TAG, "No error while executing SignUp");
    }

    OcAccountManager.OnPostListener onSignInPost = new OcAccountManager.OnPostListener() {
        @Override
        public void onPostCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
            Log.d(TAG, "onSignInPost..");

            runOnUiThread(new Runnable()
            {
                @Override
                public void run() {
                    Toast.makeText(EasysetupActivity.this, "Sign-in completed", Toast.LENGTH_SHORT).show();
                }
            });
        }

        @Override
        public void onPostFailed(Throwable ex) {
            if (ex instanceof OcException) {
                OcException ocEx = (OcException) ex;
                ErrorCode errCode = ocEx.getErrorCode();
                Log.e(TAG, ocEx.getMessage());
                if (ErrorCode.UNAUTHORIZED_REQ != errCode) {
                    RefreshToken();
                }
            }
        }
    };

    private void SignInDevice() {
        try {
            Log.d(TAG, "SignInDevice..");

            runOnUiThread(new Runnable()
            {
                @Override
                public void run() {
                    Toast.makeText(EasysetupActivity.this, "SignInDevice in progress..", Toast.LENGTH_SHORT).show();
                }
            });
            if(m_accountManager != null) {
                m_accountManager.signIn(mUserID, mAccessToken, onSignInPost);
            }
        }
        catch(OcException e)
        {
            e.printStackTrace();
        }

        Log.d(TAG, "No error while executing login");
    }

    @Override
    public void onObserveCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation, int i) {
        Log.d(TAG,"onObserveCompleted");
    }

    @Override
    public void onObserveFailed(Throwable throwable) {
        Log.d(TAG,"onObserveFailed");
    }

    public void subscribeDevicePresence()
    {
        List<String> deviceIDs = new ArrayList<String>();
        deviceIDs.add(mEnrolleeDeviceID);

        try {

            OcPlatform.subscribeDevicePresence(CIServer, deviceIDs, EnumSet.of(OcConnectivityType.
                                               CT_ADAPTER_TCP), this);
        } catch(OcException e)
        {
            e.printStackTrace();
        }

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        closeIOCStack();
    }
}
