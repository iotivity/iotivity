/******************************************************************
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************/

package org.iotivity.cloud;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.EnumSet;
import java.util.List;
import java.util.Properties;
import java.util.concurrent.ExecutionException;

import android.app.Activity;
import android.content.Context;
import android.util.Log;

import org.iotivity.base.ErrorCode;
import org.iotivity.base.OcAccountManager;
import org.iotivity.base.OcAccountManager.OnPostListener;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;

import org.iotivity.configuration.IConfiguration;
import org.iotivity.configuration.ReadConfigPropFile;
import org.iotivity.testcase.IoTivityLog;

public class OcAccountManagerHelper
        implements OcAccountManager.OnPostListener, IConfiguration {
    private static final String TAG                 = "OcAccountManagerHelper";
    private static final String CONFIG_FILE_PATH    = "/data/local/tmp/";
    private static final String CLOUD_PROPERTY_FILE = "cloud.properties";

    private enum CloudAuth
    {
        SIGNUP, SIGNIN, SIGNOUT
    };

    private enum LogLevel
    {
        INFO, ERROR, DEBUG
    };

    private static boolean    s_mIsCbInvoked = CALLBACK_NOT_INVOKED;
    private static Properties s_mProps;
    private static String     s_mFilePath;
    private static String     s_mFileName;
    private static File       s_mFile;
    public static CloudAuth   s_mMethodName;
    public static String      s_mCloudUid;
    public static String      s_mCloudAccessToken;
    public static String      s_mAuthCode;
    public static String      s_mErrorMessage;

    public static void init(String fileDir) {
        s_mProps = new Properties();
        ReadConfigPropFile.readConfigFile(CONFIG_FILE_PATH);
        s_mFilePath = fileDir;
        s_mFile = new File(s_mFilePath + CLOUD_PROPERTY_FILE);

        if (!s_mFile.exists()) {
            getAuthCode();
        }
    }

    private static void getAuthCode() {

        Log.d(TAG, "getAuthCode IN");
        GetAuthCode getContent = new GetAuthCode();

        try {
            OcAccountManagerHelper.s_mAuthCode = getContent.execute().get();
        } catch (InterruptedException e) {

            e.printStackTrace();
        } catch (ExecutionException e) {

            e.printStackTrace();
        }
        Log.d(TAG, "Auth Code : " + OcAccountManagerHelper.s_mAuthCode);

        Log.d(TAG, "getAuthCode Out");
    }

    public void readPropFile(Properties s_mProps, File file) {
        Log.d(TAG, "readPropFile IN");
        InputStream inputStream = null;
        try {
            inputStream = new FileInputStream(file);
            s_mProps.load(inputStream);
            s_mCloudUid = "";
            s_mCloudAccessToken = "";
            s_mCloudUid = s_mProps.getProperty("uId");
            s_mCloudAccessToken = s_mProps.getProperty("authToken");
            Log.d(TAG, "READ FILE : SIGNUP Successful");
            Log.d(TAG, "READ FILE : uid :" + s_mCloudUid);
            Log.d(TAG, "READ FILE : accesstoken :" + s_mCloudAccessToken);
            Log.d(TAG, "READ FILE : refreshtoken : null");

        } catch (FileNotFoundException e) {

            e.printStackTrace();
        } catch (IOException e) {

            e.printStackTrace();
        } finally {

            if (inputStream != null) {
                try {
                    inputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

        }
        Log.d(TAG, "readPropFile OUT");
    }

    public void writePropFile(Properties s_mProps, File file) {

        Log.d(TAG, "writePropFile IN");
        OutputStream outputStream = null;
        try {
            file.createNewFile();
            outputStream = new FileOutputStream(file);

            s_mProps.setProperty("uId", s_mCloudUid);
            s_mProps.setProperty("authToken", s_mCloudAccessToken);
            s_mProps.store(outputStream, null);
        } catch (FileNotFoundException e) {

            e.printStackTrace();
        } catch (IOException e) {

            e.printStackTrace();
        } finally {

            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

        }

        Log.d(TAG, "writePropFile OUT");

    }

    @Override
    public void onPostCompleted(List<OcHeaderOption> list,
            OcRepresentation ocRepresentation) {
        showLog(LogLevel.DEBUG, s_mMethodName.name() + " Successful");

        if (CloudAuth.SIGNUP == s_mMethodName) {

            try {
                if (ocRepresentation.hasAttribute("uid")) {
                    s_mCloudUid = ocRepresentation.getValue("uid");
                    showLog(LogLevel.DEBUG, "Obtained UID : " + s_mCloudUid);
                }

                if (ocRepresentation.hasAttribute("accesstoken")) {
                    s_mCloudAccessToken = ocRepresentation
                            .getValue("accesstoken");
                    showLog(LogLevel.DEBUG,
                            "Obtained accesstoken : " + s_mCloudAccessToken);
                }

                if (ocRepresentation.hasAttribute("refreshtoken")) {
                    showLog(LogLevel.DEBUG, "refreshtoken : "
                            + ocRepresentation.getValue("refreshtoken"));
                }

                if (ocRepresentation.hasAttribute("tokentype")) {
                    showLog(LogLevel.DEBUG, "tokentype : "
                            + ocRepresentation.getValue("tokentype"));
                }

            } catch (OcException e) {
                showLog(LogLevel.ERROR, "Error :" + e.getLocalizedMessage());
            }

            writePropFile(s_mProps, s_mFile);
        }

        s_mIsCbInvoked = CALLBACK_INVOKED;
    }

    @Override
    public void onPostFailed(Throwable throwable) {
        showLog(LogLevel.DEBUG, s_mMethodName.name() + " Failed");
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            showLog(LogLevel.ERROR, ocEx.toString());
            ErrorCode errCode = ocEx.getErrorCode();
            showLog(LogLevel.ERROR, "Error code: " + errCode);
        }

    }

    public static String getHostAddres(TLS type) {
        String hostAddress = "";
        if (TLS.ENABLED == type) {
            hostAddress += COAPS_PREFIX;
        } else {
            hostAddress += COAP_PREFIX;
        }

        hostAddress += (ReadConfigPropFile.s_mDeafaultCiServerIp + ":"
                + DEFAULT_CI_SERVER_PORT);
        showLog(LogLevel.DEBUG, "Host Address : " + hostAddress);
        return hostAddress;

    }

    public static OcAccountManager getAccountMangerInstance(TLS type) {

        OcAccountManager instance = null;
        try {
            instance = OcPlatform.constructAccountManagerObject(
                    getHostAddres(type),
                    EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP));
        } catch (OcException e) {

            showLog(LogLevel.ERROR, "Error : " + e.getLocalizedMessage());
            e.printStackTrace();
        }

        return instance;

    }

    public boolean singUp(OcAccountManager instance, String authProvider,
            String s_mAuthCode, OnPostListener onPostListener) {

        try {
            showLog(LogLevel.DEBUG, "SignUP IN");
            if (!s_mFile.exists()) {
                s_mMethodName = CloudAuth.SIGNUP;
                s_mIsCbInvoked = CALLBACK_NOT_INVOKED;
                instance.signUp(authProvider, s_mAuthCode, onPostListener);

                if (CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                    showLog(LogLevel.DEBUG, CALLBACK_NOT_INVOKED_MSG);
                    s_mErrorMessage = CALLBACK_NOT_INVOKED_MSG;
                    return false;
                }
            } else {
                readPropFile(s_mProps, s_mFile);
            }

            showLog(LogLevel.DEBUG, "SignUP OUT");
            return true;
        } catch (OcException e) {

            showLog(LogLevel.ERROR, "ERROR :" + e.getLocalizedMessage());
            e.printStackTrace();
            return false;
        }

    }

    public boolean singIn(OcAccountManager instance, String userUuid,
            String accessToken, OnPostListener onPostListener) {

        try {
            showLog(LogLevel.DEBUG, "SignIn IN");
            s_mMethodName = CloudAuth.SIGNIN;
            s_mIsCbInvoked = CALLBACK_NOT_INVOKED;

            showLog(LogLevel.DEBUG, "uid:\t" + userUuid);

            showLog(LogLevel.DEBUG, "accessToken : " + accessToken);

            instance.signIn(userUuid, accessToken, onPostListener);

            if (CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                showLog(LogLevel.DEBUG, CALLBACK_NOT_INVOKED_MSG);
                s_mErrorMessage = CALLBACK_NOT_INVOKED_MSG;
                return false;
            }

            showLog(LogLevel.DEBUG, "SignIn OUT");
            return true;
        } catch (OcException e) {

            showLog(LogLevel.ERROR, "ERROR :" + e.getLocalizedMessage());
            e.printStackTrace();
            return false;
        }

    }

    public boolean singOut(OcAccountManager instance, String accessToken,
            OnPostListener onPostListener) {

        try {
            showLog(LogLevel.DEBUG, "SignOut IN");

            s_mMethodName = CloudAuth.SIGNOUT;
            s_mIsCbInvoked = CALLBACK_NOT_INVOKED;

            showLog(LogLevel.DEBUG, "accessToken : " + accessToken);
            instance.signOut(accessToken, onPostListener);
            if (CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                showLog(LogLevel.DEBUG, CALLBACK_NOT_INVOKED_MSG);
                s_mErrorMessage = CALLBACK_NOT_INVOKED_MSG;
                return false;
            }

            showLog(LogLevel.DEBUG, "SignOut OUT");
            return true;
        } catch (Exception e) {
            showLog(LogLevel.ERROR, "ERROR :" + e.getLocalizedMessage());
            e.printStackTrace();
            return false;
        }

    }

    private boolean waitTillCBInvoke() {
        showLog(LogLevel.DEBUG, "Checking if Callback is Invoked or Not");
        int count = 0;

        while (!s_mIsCbInvoked) {

            try {
                Thread.sleep(DELAY_SHORT);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            count++;
            showLog(LogLevel.DEBUG, "Waiting for = " + count + " second/s");

            if (count > CALLBACK_TIMEOUT) {
                return CALLBACK_NOT_INVOKED;
            }
        }

        return CALLBACK_INVOKED;
    }

    private static void showLog(LogLevel logLevel, String message) {
        if (logLevel == LogLevel.ERROR) {
            Log.e(TAG, message);
        } else if (logLevel == LogLevel.DEBUG) {
            Log.d(TAG, message);
        } else {
            Log.i(TAG, message);
        }
    }
}
