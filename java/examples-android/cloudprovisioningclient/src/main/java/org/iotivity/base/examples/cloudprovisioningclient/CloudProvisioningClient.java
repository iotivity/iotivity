/*
 * ******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
package org.iotivity.base.examples.cloudprovisioningclient;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Message;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import org.iotivity.base.ErrorCode;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcAccountManager;
import org.iotivity.base.OcCloudProvisioning;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcProvisioning;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.base.examples.cloudprovisioningclient.R;
import org.iotivity.base.*;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;

public class CloudProvisioningClient extends Activity implements OcAccountManager.OnPostListener {

    private static final String TAG = "Cloud Provisioning Client: ";
    private List<OicSecCloudAce> generateAce(String aclid)
    {
        String type1 = "type1";
        String type2 = "type2";
        int typeLen = 2;
        String interface1 = "interface1";
        String interface2 = "interface2";
        int interfaceLen = 2;


        ArrayList<String> types = new ArrayList<String>();
        types.add(type1);
        types.add(type2);

        ArrayList<String> interfaces = new ArrayList<String>();
        interfaces.add(interface1);
        interfaces.add(interface2);
        OicSecResr res = new OicSecResr("/oic/a", "testrel", types, typeLen,interfaces, 2);
        OicSecResr res1 =  new OicSecResr("/oic/b", "testrel", types, typeLen,interfaces, 2);
        List<OicSecResr> resources = new ArrayList<OicSecResr>();
        resources.add(res);
        resources.add(res1);

        List<String> recurrences = new ArrayList<String>();
        recurrences.add("1");
        recurrences.add("2");
        OicSecValidity validity1 = new  OicSecValidity("1", recurrences,2);
        OicSecValidity validity2 = new  OicSecValidity("2", recurrences,2);
        List<OicSecValidity> validities = new ArrayList<OicSecValidity>();
        validities.add(validity1);
        validities.add(validity2);

        OicSecCloudAce ace1 = new OicSecCloudAce(aclid,"72616E64-5069-6E44-6576-557569643030", 0,
                31,resources, validities);
        OicSecCloudAce ace2 = new OicSecCloudAce(aclid, "72616E64-5069-6E44-6576-557569643030", 0,
                8, resources, validities);

        List<OicSecCloudAce> aces = new ArrayList<OicSecCloudAce>();
        aces.add(ace1);
        aces.add(ace2);
        logMessage("out--generate ace");
        return aces;
    }


    OcAccountManager.OnPostListener onSignUp = new OcAccountManager.OnPostListener() {
        @Override
            public synchronized void onPostCompleted(List<OcHeaderOption> list,
                    OcRepresentation ocRepresentation) {
                logMessage("signUp was successful");
                try {
                    String mUserUuid = ocRepresentation.getValue("uid");
                    String mAccesstoken = ocRepresentation.getValue("accesstoken");

                    String mRefreshtoken = ocRepresentation.getValue("refreshtoken");
                    String tokenType = ocRepresentation.getValue("tokentype");

                    SharedPreferences.Editor editor = settingPreference.edit();
                    editor.putString("useruuid", mUserUuid);
                    editor.putString("accesstoken", mAccesstoken);
                    editor.commit();

                    logMessage("\tuserID: " + mUserUuid);
                    logMessage("\taccessToken: " + mAccesstoken);
                    logMessage("\trefreshToken: " + mRefreshtoken);
                    logMessage("\ttokenType: " + tokenType);

                    if (ocRepresentation.hasAttribute("expiresin")) {
                        int expiresIn = ocRepresentation.getValue("expiresin");
                        logMessage("\texpiresIn: " + expiresIn);
                    }

                    runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                            signupLyt.setVisibility(View.GONE);
                            signinLyt.setVisibility(View.VISIBLE);
                            }
                            });


                } catch (OcException e) {
                    Log.e(TAG, e.toString());
                }
            }


        @Override
            public synchronized void onPostFailed(Throwable throwable) {
                logMessage("Failed to signUp");
                if (throwable instanceof OcException) {
                    OcException ocEx = (OcException) throwable;
                    Log.e(TAG, ocEx.toString());
                    ErrorCode errCode = ocEx.getErrorCode();
                    logMessage("Error code: " + errCode);
                }
            }
    };
    OcAccountManager.OnPostListener onSignIn = new OcAccountManager.OnPostListener() {
        @Override
            public synchronized void onPostCompleted(List<OcHeaderOption> list,
                    OcRepresentation ocRepresentation) {
                logMessage("signIn was successful");
                runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                        signinLyt.setVisibility(View.GONE);
                        userid.setText(settingPreference.getString("useruuid", ""));
                        lyt1.setVisibility(View.VISIBLE);
                        lyt2.setVisibility(View.VISIBLE);
                        }
                        });


            }

        @Override
            public synchronized void onPostFailed(Throwable throwable) {
                logMessage("Failed to signIn");
                if (throwable instanceof OcException) {
                    OcException ocEx = (OcException) throwable;
                    Log.e(TAG, ocEx.toString());
                    ErrorCode errCode = ocEx.getErrorCode();
                    logMessage("Error code: " + errCode);
                }
            }
    };

    OcAccountManager.OnPostListener onSignOut = new OcAccountManager.OnPostListener() {
        @Override
            public synchronized void onPostCompleted(List<OcHeaderOption> list,
                    OcRepresentation ocRepresentation) {
                logMessage("signOut was successful");
                runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                        lyt1.setVisibility(View.GONE);
                        lyt2.setVisibility(View.GONE);
                        signinLyt.setVisibility(View.VISIBLE);
                        }
                        });

            }

        @Override
            public synchronized void onPostFailed(Throwable throwable) {
                logMessage("Failed to signOut");
                if (throwable instanceof OcException) {
                    OcException ocEx = (OcException) throwable;
                    Log.e(TAG, ocEx.toString());
                    ErrorCode errCode = ocEx.getErrorCode();
                    logMessage("Error code: " + errCode);
                }
            }
    };

    OcCloudProvisioning.GetIndividualAclInfoListener getIndividualAclInfoListener =
        new OcCloudProvisioning.GetIndividualAclInfoListener() {
            @Override
                public void getIndividualAclInfoListener(int result) {
                    Log.d(TAG, "Inside getIndividualAclInfoListener ");
                    if (result == 0) {
                        logMessage("Individual ACL Info Successfull !!");
                    } else {
                        logMessage("Error: Individual ACL Info failed !!");
                    }
                }
        };
    OcCloudProvisioning.RequestCertificateListener requestCertificateListener =
        new OcCloudProvisioning.RequestCertificateListener() {
            @Override
                public void requestCertificateListener(int result) {
                    Log.d(TAG, "Inside requestCertificateListener ");
                    if (result == 0) {
                        logMessage("Request certificate Successfull !!");
                    } else {
                        logMessage("Error: Request certificate failed !!");
                    }
                }
        };
    OcCloudProvisioning.GetCRLListener getCRLListener =
        new OcCloudProvisioning.GetCRLListener() {
            @Override
                public void getCRLListener(int result) {
                    Log.d(TAG, "Inside getCRLListener ");
                    if (result == 0) {
                        logMessage("Get CRL Successfull !!");
                    } else {
                        logMessage("Error: Get CRL failed !!");
                    }
                }
        };
    OcCloudProvisioning.PostCRLListener postCRLListener =
        new OcCloudProvisioning.PostCRLListener() {
            @Override
                public void postCRLListener(int result) {
                    Log.d(TAG, "Inside postCRLListener ");
                    if (result == 4) {
                        logMessage("Post CRL Successfull !!");
                    } else {
                        logMessage("Error: Post CRL failed !!");
                    }
                }
        };
    private static final int BUFFER_SIZE = 1024;
    private final int REQUEST_LOGIN = 1;
    Button signUp, signIn, signOut, getAclId, getAclInfo, requestCert, postCrl, getCrl, updateAce;
    TextView userid;
    LinearLayout lyt1, lyt2, signupLyt, signinLyt;
    // private TextView eventView;
    SharedPreferences settingPreference;
    OcCloudProvisioning ocCloudProvisioning;
    String createacl_Id = null;
    String acl_Id = null;
    OcCloudProvisioning.GetAclIdByDeviceListener getAclIdByDeviceListener =
        new OcCloudProvisioning.GetAclIdByDeviceListener() {
            @Override
                public void getAclIdByDeviceListener(int result, String aclId) {
                    Log.d(TAG, "Inside getAclIdByDeviceListener ");
                    if (result == 0) {
                        acl_Id = aclId;
                        logMessage("Acl Id by device !!" + acl_Id);
                    } else {
                        logMessage("Error: Acl Id by device failed !!");
                    }
                }
        };

    OcCloudProvisioning.CreateAclIdListener createAclIdListener =
        new OcCloudProvisioning.CreateAclIdListener() {
            @Override
                public void createAclIdListener(int result, String aclId) {
                    Log.d(TAG, "Inside createAclIdListener ");
                    if (result == 0) {
                        createacl_Id = aclId;
                        acl_Id = aclId;
                        logMessage("Acl Id by create aclid !!" + createacl_Id);
                    } else {
                        logMessage("Error: Acl Id by create aclid failed !!");
                    }
                }
        };
    OcCloudProvisioning.UpdateIndividualACLListener updateIndividualACLListener =
        new OcCloudProvisioning.UpdateIndividualACLListener(){
              public void updateIndividualACLListener(int result)
              {
                  Log.d(TAG, "Inside UpdateIndividualACLListener ");
                  if (result == 4) {
                      logMessage("Success UpdateIndividualACLListener");
                  } else {
                      logMessage("Error:UpdateIndividualACLListener = "+result);
                  }
              }
    };
    private OcAccountManager mAccountManager;
    private String filePath = "";
    private TextView mEventsTextView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_activity);

        signUp = (Button) findViewById(R.id.signup);
        signIn = (Button) findViewById(R.id.signin);
        signOut = (Button) findViewById(R.id.signout);
        getAclId = (Button) findViewById(R.id.getAclId);
        getAclInfo = (Button) findViewById(R.id.getAclInfo);
        requestCert = (Button) findViewById(R.id.request);
        postCrl = (Button) findViewById(R.id.postCRL);
        getCrl = (Button) findViewById(R.id.getCRL);

        updateAce = (Button) findViewById(R.id.updateAce);
        lyt1 = (LinearLayout) findViewById(R.id.lyt1);
        lyt2 = (LinearLayout) findViewById(R.id.lyt2);
        signupLyt = (LinearLayout) findViewById(R.id.signupLyt);
        signinLyt = (LinearLayout) findViewById(R.id.signinLyt);


        userid = (TextView) findViewById(R.id.userid);

        mEventsTextView = (TextView) findViewById(R.id.eventView);

        filePath = getFilesDir().getPath() + "/"; //  data/data/<package>/files/
        //copy CBOR file when application runs first time
        settingPreference = PreferenceManager.getDefaultSharedPreferences(this);
        boolean isFirstRun = settingPreference.getBoolean("FIRSTRUN", true);
        if (isFirstRun) {
            copyCborFromAsset();
            SharedPreferences.Editor editor = settingPreference.edit();
            editor.putBoolean("FIRSTRUN", false);
            editor.putString("IP", StringConstants.DEFAULT_COAP_DERVER_IP);
            editor.putString("PORT", StringConstants.DEFAULT_COAP_DERVER_PORT);
            editor.putString("DEVICEID", StringConstants.DEFAULT_DEVICE_ID);
            editor.putString("OWNERID", StringConstants.DEFAULT_OWNER_ID);
            editor.putString("SERIALNUMBER", StringConstants.DEFAULT_SERIAL_NUMBER);
            editor.commit();
        }
        if (settingPreference.getString("useruuid", "").equals("")) {

            lyt1.setVisibility(View.GONE);
            lyt2.setVisibility(View.GONE);
            signupLyt.setVisibility(View.VISIBLE);
            signinLyt.setVisibility(View.GONE);


        } else {
            userid.setText(settingPreference.getString("useruuid", ""));
            lyt1.setVisibility(View.VISIBLE);
            lyt2.setVisibility(View.VISIBLE);
            signupLyt.setVisibility(View.GONE);
            signinLyt.setVisibility(View.VISIBLE);
        }

        initOICStack();
        ocCloudProvisioning = new OcCloudProvisioning(settingPreference.getString("IP", ""),
                Integer.valueOf(settingPreference.getString("PORT", "")));

        signUp.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                signUp();
                }
                });
        signIn.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                signIn();
                }
                });
        signOut.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                signOut();
                }
                });
        getAclId.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                getAclId();
                }
                });
        getAclInfo.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                getAclInfo();
                }
                });
        requestCert.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                requestCert();
                }
                });
        postCrl.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                postCrl();
                }
                });
        getCrl.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                getCrl();
                }
                });
        updateAce.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                updateAces();
                }
                });
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        closeIOCStack();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_cloud_provision, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_settings:
                setDefualtSettings();
                return (true);
        }
        return (super.onOptionsItemSelected(item));
    }

    private void signIn() {
        try {
            logMessage("signIn");
            if(mAccountManager==null)
            {
                mAccountManager = OcPlatform.constructAccountManagerObject(
                        StringConstants.COAP_TCP + settingPreference.getString("IP",
                            StringConstants.DEFAULT_COAP_DERVER_IP) + ":" +
                        settingPreference.getString("PORT", StringConstants.DEFAULT_COAP_DERVER_PORT),
                        EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP));
            }

            mAccountManager.signIn(settingPreference.getString("useruuid", ""),
                    settingPreference.getString("accesstoken", ""), onSignIn);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    private void signOut() {
        try {
            logMessage("signOut");
            if(mAccountManager==null)
            {
                logMessage("Please signIn first");
                return;
            }

            mAccountManager.signOut(settingPreference.getString("accesstoken", ""),onSignOut);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }


    private void getAclId() {
        try {
            logMessage("getAclId");
            if(createacl_Id == null)
            {
                ocCloudProvisioning.createAclId(settingPreference.getString("OWNERID", ""),
                             settingPreference.getString("DEVICEID", ""), createAclIdListener);
            }
            else{
                ocCloudProvisioning.getAclIdByDevice(settingPreference.getString("DEVICEID", ""),
                        getAclIdByDeviceListener);
            }
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    private void getAclInfo() {
        try {
            logMessage("getAclInfo");
            ocCloudProvisioning.getIndividualAclInfo(acl_Id, getIndividualAclInfoListener);

        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    private void requestCert() {
        try {
            logMessage("requestCert");
            ocCloudProvisioning = new OcCloudProvisioning(settingPreference.getString("IP", ""),
                    Integer.valueOf(settingPreference.getString("PORT", "")));
            ocCloudProvisioning.requestCertificate(requestCertificateListener);

        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    private void postCrl() {
        try {
            logMessage("postCrl");
            ArrayList<String> arrayList = new ArrayList<>();
            arrayList.add(settingPreference.getString("SERIALNUMBER", "1234"));

            ocCloudProvisioning.postCRL("20160727000000", "20161027000000", null,
                    arrayList, postCRLListener);

        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    private void getCrl() {
        try {
            logMessage("getCrl");
            ocCloudProvisioning.getCRL(getCRLListener);

        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    private void updateAces()
    {
        try {
            logMessage("updateAce");
            ocCloudProvisioning.updateIndividualACL(acl_Id, generateAce(acl_Id),
                    updateIndividualACLListener);

        } catch (OcException e) {
            e.printStackTrace();
        }
    }
    private void signUp() {
        try {
            mAccountManager = OcPlatform.constructAccountManagerObject(
                    StringConstants.COAP_TCP + settingPreference.getString("IP",
                        StringConstants.DEFAULT_COAP_DERVER_IP) + settingPreference.getString("PORT",
                            StringConstants.DEFAULT_COAP_DERVER_PORT),
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP));
            logMessage(StringConstants.COAP_TCP + settingPreference.getString("IP",
                        StringConstants.DEFAULT_COAP_DERVER_IP) + settingPreference.getString("PORT",
                            StringConstants.DEFAULT_COAP_DERVER_PORT));
        } catch (OcException e) {
            e.printStackTrace();
        }

        Intent intentLogin = new Intent(this, LoginActivity.class);
        startActivityForResult(intentLogin, REQUEST_LOGIN);
    }


    /**
     * configure OIC platform and call findResource
     */
    private void initOICStack() {
        //create platform config
        PlatformConfig cfg = new PlatformConfig(
                this,
                ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER,
                "0.0.0.0", // bind to all available interfaces
                0,
                QualityOfService.LOW, filePath + StringConstants.OIC_CLIENT_CBOR_DB_FILE);
        OcPlatform.Configure(cfg);
        try {
            /*
             * Initialize DataBase
             */
            String sqlDbPath = getFilesDir().getAbsolutePath().replace("files", "databases") +
                File.separator;
            File file = new File(sqlDbPath);
            //check files directory exists
            if (!(file.isDirectory())) {
                file.mkdirs();
                Log.d(TAG, "Sql db directory created at " + sqlDbPath);
            }
            Log.d(TAG, "Sql db directory exists at " + sqlDbPath);
            OcProvisioning.provisionInit(sqlDbPath + StringConstants.OIC_SQL_DB_FILE);
        } catch (OcException e) {
            logMessage(TAG + "provisionInit error: " + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
    }

    private void closeIOCStack() {
        try {
            /*
             * Close DataBase
             */
            OcProvisioning.provisionClose();
        } catch (OcException e) {
            logMessage(TAG + "provisionClose error: " + e.getMessage());
            Log.e(TAG, e.getMessage());
        }
    }

    @Override
        public void onActivityResult(int requestCode, int resultCode, Intent data) {
            super.onActivityResult(requestCode, resultCode, data);
            if (resultCode == Activity.RESULT_OK && requestCode == REQUEST_LOGIN) {
                String mAuthCode = data.getStringExtra("authCode");


                logMessage("\tauthCode: " + mAuthCode);

                try {
                    logMessage("Sign Up");
                    mAccountManager = OcPlatform.constructAccountManagerObject(
                            StringConstants.COAP_TCP + settingPreference.getString("IP",
                                StringConstants.DEFAULT_COAP_DERVER_IP) + ":" +
                            settingPreference.getString("PORT", StringConstants.DEFAULT_COAP_DERVER_PORT),
                            EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP));

                    mAccountManager.signUp("github", mAuthCode, onSignUp);
                } catch (OcException e) {
                    e.printStackTrace();
                }
            }
        }


    /**
     * Copy svr db CBOR dat file from assets folder to app data files dir
     */
    private void copyCborFromAsset() {
        InputStream inputStream = null;
        OutputStream outputStream = null;
        int length;
        byte[] buffer = new byte[BUFFER_SIZE];
        try {
            inputStream = getAssets().open(StringConstants.OIC_CLIENT_CBOR_DB_FILE);
            File file = new File(filePath);
            //check files directory exists
            if (!(file.exists() && file.isDirectory())) {
                file.mkdirs();
            }
            outputStream = new FileOutputStream(filePath + StringConstants.OIC_CLIENT_CBOR_DB_FILE);
            while ((length = inputStream.read(buffer)) != -1) {
                outputStream.write(buffer, 0, length);
            }
        } catch (NullPointerException e) {
            logMessage(TAG + "Null pointer exception " + e.getMessage());
            Log.e(TAG, e.getMessage());
        } catch (FileNotFoundException e) {
            logMessage(TAG + "CBOR svr db file not found " + e.getMessage());
            Log.e(TAG, e.getMessage());
        } catch (IOException e) {
            logMessage(TAG + StringConstants.OIC_CLIENT_CBOR_DB_FILE + " file copy failed");
            Log.e(TAG, e.getMessage());
        } finally {
            if (inputStream != null) {
                try {
                    inputStream.close();
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                }
            }
            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                }
            }
        }
    }

    public void logMessage(String text) {
        logMsg(text);
    }

    public void logMsg(final String text) {
        runOnUiThread(new Runnable() {
                public void run() {
                Message msg = new Message();
                msg.obj = text;
                mEventsTextView.append(text);
                mEventsTextView.append("\n");
                }
                });
        Log.i(TAG, text);
        Intent intent = new Intent(getPackageName());
        intent.putExtra(StringConstants.MESSAGE, text);
        sendBroadcast(intent);
    }

    private void setDefualtSettings() {
        View setingLayout = getLayoutInflater().inflate(R.layout.setting_layout, null);

        final EditText ip = (EditText) setingLayout.findViewById(R.id.ip);
        final EditText port = (EditText) setingLayout.findViewById(R.id.port);
        final EditText deviceId = (EditText) setingLayout.findViewById(R.id.deviceId);
        final EditText serialNum = (EditText) setingLayout.findViewById(R.id.serialNum);

        ip.setText(settingPreference.getString("IP", ""));
        port.setText(settingPreference.getString("PORT", ""));
        deviceId.setText(settingPreference.getString("DEVICEID", ""));
        serialNum.setText(settingPreference.getString("SERIALNUMBER", "1234"));


        final AlertDialog.Builder builder = new AlertDialog.Builder(CloudProvisioningClient.this);
        builder.setView(setingLayout);
        builder.setPositiveButton("Submit", null).setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                Log.d(TAG, "AlertDialog onClick");

                }
                });
        final AlertDialog alertDialog = builder.create();
        alertDialog.setMessage("Settings");
        alertDialog.show();
        alertDialog.getButton(AlertDialog.BUTTON_POSITIVE).setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {

                String ip_address = ip.getText().toString();
                String port_num = port.getText().toString();
                String deviceId_val = deviceId.getText().toString();
                String serialNum_val = serialNum.getText().toString();

                SharedPreferences.Editor editor = settingPreference.edit();

                editor.putString("IP", ip_address);
                editor.putString("PORT", port_num);
                editor.putString("DEVICEID", deviceId_val);
                editor.putString("SERIALNUMBER", serialNum_val);
                editor.commit();

                alertDialog.cancel();
                }
                });
        alertDialog.getButton(AlertDialog.BUTTON_NEGATIVE).setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {

                alertDialog.cancel();
                }
                });

    }

    @Override
        public void onPostCompleted(List<OcHeaderOption> ocHeaderOptions, OcRepresentation ocRepresentation) {

        }

    @Override
        public void onPostFailed(Throwable throwable) {

        }


    /**
     * to display on Server Message on Client screen
     */
    public class MessageReceiver extends BroadcastReceiver {
        @Override
            public void onReceive(Context context, Intent intent) {
                final String message = intent.getStringExtra(StringConstants.MESSAGE);
                logMessage(message);
            }
    }
}
