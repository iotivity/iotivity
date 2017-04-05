/******************************************************************
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

public class OcAccountManagerHelper
        implements OcAccountManager.OnPostListener, IConfiguration {

    private static boolean mIsCbInvoked = CALLBACK_NOT_INVOKED;

    private enum CloudAuth
    {
        SIGNUP, SIGNIN, SIGNOUT
    };

    private enum LogLevel
    {
        INFO, ERROR, DEBUG
    };

    private static Properties props;
    private static String     filePath;
    private static String     fileName;
    private static File       file;
    public static CloudAuth   mMethodName;

    public static String      s_CloudUid;
    public static String      s_CloudAccesstoken;
    public static String      authCode;
    public static String      mErrorMessage;

    private static void getAuthCode() {

        Log.d(COMMON_TAG, "getAuthCode IN");
        GetAuthCode getXML = new GetAuthCode();

        try {
            OcAccountManagerHelper.authCode = getXML.execute().get();
        } catch (InterruptedException e) {

            e.printStackTrace();
        } catch (ExecutionException e) {

            e.printStackTrace();
        }
        Log.d(COMMON_TAG, "Auth Code : " + OcAccountManagerHelper.authCode);

        Log.d(COMMON_TAG, "getAuthCode Out");
    }

    public static void init(Context context) {
        props = new Properties();

        filePath = context.getFilesDir().getPath() + "/"; // data/data/<package>/files/
        fileName = "cloud.properties";
        file = new File(filePath + fileName);

        if (!file.exists()) {
            getAuthCode();
        }

    }

    public void readPropFile(Properties props, File file) {
        Log.d(COMMON_TAG, "readPropFile IN");
        InputStream inputStream = null;
        try {
            inputStream = new FileInputStream(file);
            props.load(inputStream);
            s_CloudUid = "";
            s_CloudAccesstoken = "";
            s_CloudUid = props.getProperty("uId");
            s_CloudAccesstoken = props.getProperty("authToken");
            Log.d(COMMON_TAG, "READ FILE : uid :" + s_CloudUid);
            Log.d(COMMON_TAG,
                    "READ FILE : CloudAccesstoken :" + s_CloudAccesstoken);

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
        Log.d(COMMON_TAG, "readPropFile OUT");
    }

    public void writePropFile(Properties props, File file) {

        Log.d(COMMON_TAG, "writePropFile IN");
        OutputStream outputStream = null;
        try {
            file.createNewFile();
            outputStream = new FileOutputStream(file);

            props.setProperty("uId", s_CloudUid);
            props.setProperty("authToken", s_CloudAccesstoken);
            props.store(outputStream, null);
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

        Log.d(COMMON_TAG, "writePropFile OUT");

    }

    @Override
    public void onPostCompleted(List<OcHeaderOption> list,
            OcRepresentation ocRepresentation) {
        showLog(LogLevel.DEBUG, mMethodName.name() + " Successful");

        if (CloudAuth.SIGNUP == mMethodName) {

            try {
                if (ocRepresentation.hasAttribute("uid")) {
                    s_CloudUid = ocRepresentation.getValue("uid");
                }

                if (ocRepresentation.hasAttribute("accesstoken")) {
                    s_CloudAccesstoken = ocRepresentation
                            .getValue("accesstoken");
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

            writePropFile(props, file);
        }

        mIsCbInvoked = CALLBACK_INVOKED;
    }

    @Override
    public void onPostFailed(Throwable throwable) {
        showLog(LogLevel.DEBUG, mMethodName.name() + " Failed");
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            showLog(LogLevel.ERROR, ocEx.toString());
            ErrorCode errCode = ocEx.getErrorCode();
            showLog(LogLevel.ERROR, "Error code: " + errCode);
        }

        mIsCbInvoked = CALLBACK_INVOKED;
    }

    private static String getHostAddres(TLS type) {
        String hostAddress = "";
        if (TLS.ENABLED == type) {
            hostAddress += COAPS_PREFIX;
        } else {
            hostAddress += COAP_PREFIX;
        }

        hostAddress += (ReadConfigPropFile.s_DeafaultCiServerIp + ":"
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
            String authCode, OnPostListener onPostListener) {

        try {
            showLog(LogLevel.DEBUG, "SignUP IN");

            if (!file.exists()) {
                mMethodName = CloudAuth.SIGNUP;
                mIsCbInvoked = CALLBACK_NOT_INVOKED;
                instance.signUp(authProvider, authCode, onPostListener);

                if (CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                    showLog(LogLevel.DEBUG, CallBackNotInvoked);
                    mErrorMessage = CallBackNotInvoked;
                    return false;
                }
            } else {
                readPropFile(props, file);
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
            mMethodName = CloudAuth.SIGNIN;
            mIsCbInvoked = CALLBACK_NOT_INVOKED;

            showLog(LogLevel.DEBUG, "userUuid : " + userUuid);

            showLog(LogLevel.DEBUG, "accessToken : " + accessToken);

            instance.signIn(userUuid, accessToken, onPostListener);

            if (CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                showLog(LogLevel.DEBUG, CallBackNotInvoked);
                mErrorMessage = CallBackNotInvoked;
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

            mMethodName = CloudAuth.SIGNOUT;
            mIsCbInvoked = CALLBACK_NOT_INVOKED;

            showLog(LogLevel.DEBUG, "accessToken : " + accessToken);
            instance.signOut(accessToken, onPostListener);
            if (CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                showLog(LogLevel.DEBUG, CallBackNotInvoked);
                mErrorMessage = CallBackNotInvoked;
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

        while (!mIsCbInvoked) {

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
            Log.e(COMMON_TAG, message);
        } else if (logLevel == LogLevel.DEBUG) {
            Log.d(COMMON_TAG, message);
        } else {
            Log.i(COMMON_TAG, message);
        }
    }
}
