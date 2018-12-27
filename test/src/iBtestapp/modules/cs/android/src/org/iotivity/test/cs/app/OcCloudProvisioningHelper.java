/******************************************************************
 *
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
 *
 *
 ******************************************************************/
package org.iotivity.test.cs.app;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;

import android.content.Context;
import android.util.Log;

import org.iotivity.base.OcCloudProvisioning;
import org.iotivity.base.ErrorCode;
import org.iotivity.base.OcAccountManager;
import org.iotivity.base.OcAccountManager.OnPostListener;
import org.iotivity.base.OcCloudProvisioning.GetAclIdByDeviceListener;
import org.iotivity.base.OcCloudProvisioning.GetCRLListener;
import org.iotivity.base.OcCloudProvisioning.GetIndividualAclInfoListener;
import org.iotivity.base.OcCloudProvisioning.PostCRLListener;
import org.iotivity.base.OcCloudProvisioning.RequestCertificateListener;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcProvisioning;
import org.iotivity.base.OcRepresentation;
import org.iotivity.test.cs.app.CSConstants;

public class OcCloudProvisioningHelper implements GetAclIdByDeviceListener,
        GetIndividualAclInfoListener, PostCRLListener,
        RequestCertificateListener, GetCRLListener, OnPostListener {

    public static boolean      mIsCbInvoked      = CSConstants.CALLBACK_NOT_INVOKED;
    public static String       mFilePath         = null;
    public static String       mSqlDbPath        = null;
    public static final int    Error             = -1;
    public static final int    Info              = 1;
    public static final String DEFAULT_HOST_IP   = "127.0.0.1";

    public static final int    DEFAULT_PORT      = 5683;
    public static final String DEFAULT_OWNER_ID  = "4d83e543-22c4-4f58-bd67-8846c2cde4ba";
    public static final String DEFAULT_DEVICE_ID = "99999999-0000-0000-0000-0001200";
    public static final String DEFAULT_ACL_ID    = "11111111-1111-1111-11111111";
    public static final String thisUpdate        = "201712010000000";
    public static final String nextUpdate        = "20171202000000";

    private static void showLog(int logType, String message) {

        // showOutPut(message);
        if (logType == 1) {
            Log.i(CSConstants.TAG, message);
        } else {
            Log.e(CSConstants.TAG, message);
        }
    }

    /**
     * Copy client db Cbor file from assets folder to app data files dir
     */
    public void copyCborFromAsset(Context context, String oicClientCborDbFile) {
        InputStream inputStream = null;
        OutputStream outputStream = null;
        int length;
        byte[] buffer = new byte[CSConstants.BUFFER_SIZE];
        String filePath = context.getFilesDir().getPath() + "/"; // data/data/<package>/files/

        try {
            inputStream = context.getAssets().open(oicClientCborDbFile);
            File file = new File(filePath);
            // check files directory exists
            if (!(file.exists() && file.isDirectory())) {
                file.mkdirs();
            }
            outputStream = new FileOutputStream(filePath + oicClientCborDbFile);
            while ((length = inputStream.read(buffer)) != -1) {
                outputStream.write(buffer, 0, length);
            }
        } catch (NullPointerException e) {
            Log.e(CSConstants.TAG, "Null pointer exception " + e.getMessage());
        } catch (FileNotFoundException e) {
            Log.e(CSConstants.TAG,
                    "Cbor svr db file not found " + e.getMessage());
        } catch (IOException e) {
            Log.e(CSConstants.TAG, oicClientCborDbFile + " file copy failed");
        } finally {
            if (inputStream != null) {
                try {
                    inputStream.close();
                } catch (IOException e) {
                    Log.e(CSConstants.TAG, e.getMessage());
                }
            }
            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (IOException e) {
                    Log.e(CSConstants.TAG, e.getMessage());
                }
            }
        }
    }

    public void initOICStack(String dbPath, String fileName) {
        try {
            File file = new File(dbPath);
            // check files directory exists
            if (!(file.exists())) {
                file.mkdirs();
                Log.i(CSConstants.TAG, "Sql db directory created at " + dbPath);
            } else {
                Log.i(CSConstants.TAG, "Sql db directory exists at " + dbPath);
            }

            Log.i(CSConstants.TAG, " INIT ");
            OcProvisioning.provisionInit(dbPath + fileName);
            showLog(Info, " Initialization succesful");
        } catch (Exception e) {
            showLog(Error, "Initialization Error : " + e.getLocalizedMessage());
        }
    }

    public static OnPostListener onSignUp = new OnPostListener() {
        @Override
        public synchronized void onPostCompleted(List<OcHeaderOption> list,
                OcRepresentation ocRepresentation) {

            try {
                String mUserUuid = ocRepresentation.getValue("uid");
                String mAccesstoken = ocRepresentation.getValue("accesstoken");

                String mRefreshtoken = ocRepresentation
                        .getValue("refreshtoken");
                String tokenType = ocRepresentation.getValue("tokentype");

                showLog(Info, "\tuserID: " + mUserUuid);
                showLog(Info, "\taccessToken: " + mAccesstoken);
                showLog(Info, "\trefreshToken: " + mRefreshtoken);
                showLog(Info, "\ttokenType: " + tokenType);

                if (ocRepresentation.hasAttribute("expiresin")) {
                    int expiresIn = ocRepresentation.getValue("expiresin");
                    showLog(Info, "\texpiresIn: " + expiresIn);
                }

            } catch (OcException e) {
                showLog(Error, e.toString());
            }
        }

        @Override
        public synchronized void onPostFailed(Throwable throwable) {
            showLog(Error, "Failed to signUp");
            if (throwable instanceof OcException) {
                OcException ocEx = (OcException) throwable;
                showLog(Error, ocEx.toString());
                ErrorCode errCode = ocEx.getErrorCode();
                showLog(Error, "Error code: " + errCode);
            }
        }
    };

    public boolean signUp(String authProvider, String authCode,
            OnPostListener onPostListener) {
        try {
            showLog(Info, "requestCertificate IN");
            mIsCbInvoked = CSConstants.CALLBACK_NOT_INVOKED;
            OcAccountManager mAccountManager = OcPlatform
                    .constructAccountManagerObject(
                            "coap+tcp://107.109.211.225:5683",
                            EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP));

            mAccountManager.signUp(authProvider, authCode, onPostListener);

            if (CSConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                Log.e(CSConstants.TAG, CSConstants.CallBackNotInvoked);
                CSConstants.mErrorMessage = CSConstants.CallBackNotInvoked;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            CSConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean requestCertificate(OcCloudProvisioning cloudObj,
            RequestCertificateListener certificateIssueRequestListener) {
        try {
            showLog(Info, "requestCertificate IN");
            mIsCbInvoked = CSConstants.CALLBACK_NOT_INVOKED;
            cloudObj.requestCertificate(certificateIssueRequestListener);

            if (CSConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                Log.e(CSConstants.TAG, CSConstants.CallBackNotInvoked);
                CSConstants.mErrorMessage = CSConstants.CallBackNotInvoked;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            CSConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean getAclIdByDevice(OcCloudProvisioning cloudObj,
            String deviceId,
            GetAclIdByDeviceListener cloudAclIdGetByDeviceHandler) {
        try {
            showLog(Info, "getAclIdByDevice IN");
            mIsCbInvoked = CSConstants.CALLBACK_NOT_INVOKED;
            cloudObj.getAclIdByDevice(deviceId, cloudAclIdGetByDeviceHandler);

            if (CSConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                Log.e(CSConstants.TAG, CSConstants.CallBackNotInvoked);
                CSConstants.mErrorMessage = CSConstants.CallBackNotInvoked;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            CSConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean getIndividualAclInfo(OcCloudProvisioning cloudObj,
            String aclId,
            GetIndividualAclInfoListener cloudAclIndividualGetInfoHandler) {
        try {
            showLog(Info, "getIndividualAclInfo IN");
            mIsCbInvoked = CSConstants.CALLBACK_NOT_INVOKED;
            cloudObj.getIndividualAclInfo(aclId,
                    cloudAclIndividualGetInfoHandler);

            if (CSConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                Log.e(CSConstants.TAG, CSConstants.CallBackNotInvoked);
                CSConstants.mErrorMessage = CSConstants.CallBackNotInvoked;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            CSConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean getCRL(OcCloudProvisioning cloudObj,
            GetCRLListener cloudGetCRLHandler) {
        try {
            showLog(Info, "getCRL IN");
            mIsCbInvoked = CSConstants.CALLBACK_NOT_INVOKED;
            cloudObj.getCRL(cloudGetCRLHandler);

            if (CSConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                Log.e(CSConstants.TAG, CSConstants.CallBackNotInvoked);
                CSConstants.mErrorMessage = CSConstants.CallBackNotInvoked;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            CSConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean postCRL(OcCloudProvisioning cloudObj, String thisUpdate,
            String nextUpdate, String crl, ArrayList<String> serialNumbers,
            PostCRLListener cloudPostCRLHandler) {
        try {
            showLog(Info, "postCRL IN");
            mIsCbInvoked = CSConstants.CALLBACK_NOT_INVOKED;
            cloudObj.postCRL(thisUpdate, nextUpdate, crl, serialNumbers,
                    cloudPostCRLHandler);

            if (CSConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                Log.e(CSConstants.TAG, CSConstants.CallBackNotInvoked);
                CSConstants.mErrorMessage = CSConstants.CallBackNotInvoked;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            CSConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    private boolean waitTillCBInvoke() {
        showLog(Info, "Checking if Callback is Invoked or Not");
        int count = 0;

        while (!mIsCbInvoked) {

            try {
                Thread.sleep(CSConstants.DELAY_SHORT);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            count++;
            Log.i(CSConstants.TAG, "Waiting for = " + count + " second/s");

            if (count > CSConstants.CALLBACK_TIMEOUT) {
                return CSConstants.CALLBACK_NOT_INVOKED;
            }
        }

        return CSConstants.CALLBACK_INVOKED;
    }

    @Override
    public void onPostCompleted(List<OcHeaderOption> arg0,
            OcRepresentation arg1) {
        // TODO Auto-generated method stub

    }

    @Override
    public void onPostFailed(Throwable arg0) {
        // TODO Auto-generated method stub

    }

    @Override
    public void getAclIdByDeviceListener(int i, String s) {
        showLog(Info, "getAclIdByDeviceListener IN");
        mIsCbInvoked = CSConstants.CALLBACK_INVOKED;
    }

    @Override
    public void getCRLListener(int i) {
        showLog(Info, "getCRLListener IN");
        mIsCbInvoked = CSConstants.CALLBACK_INVOKED;
    }

    @Override
    public void getIndividualAclInfoListener(int i) {
        showLog(Info, "getIndividualAclInfoListener IN");
        mIsCbInvoked = CSConstants.CALLBACK_INVOKED;
    }

    @Override
    public void postCRLListener(int i) {
        showLog(Info, "postCRLListener IN");
        mIsCbInvoked = CSConstants.CALLBACK_INVOKED;
    }

    @Override
    public void requestCertificateListener(int i) {
        showLog(Info, "requestCertificateListener IN");
        mIsCbInvoked = CSConstants.CALLBACK_INVOKED;
    }
}
