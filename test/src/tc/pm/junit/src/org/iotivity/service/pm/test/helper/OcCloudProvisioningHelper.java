/******************************************************************
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 ******************************************************************/
package org.iotivity.service.pm.test.helper;

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
import org.iotivity.service.pm.test.helper.PMConstants;

public class OcCloudProvisioningHelper
        implements OcCloudProvisioning.GetAclIdByDeviceListener,
        OcCloudProvisioning.GetIndividualAclInfoListener,
        OcCloudProvisioning.PostCRLListener,
        OcCloudProvisioning.RequestCertificateListener,
        OcCloudProvisioning.GetCRLListener {

    public static boolean mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
    public static String mFilePath = null;
    public static String mSqlDbPath = null;
    public static final int Error = -1;
    public static final int Info = 1;
    public static final String DEFAULT_HOST_IP = "127.0.0.1";
    public static final int DEFAULT_PORT = 5683;
    public static final String DEFAULT_DEVICE_ID = "11111100-1111-1111-11111111";
    public static final String DEFAULT_ACL_ID = "11111111-1111-1111-11111111";
    public static final String thisUpdate = "11111111-1111-1111-11111111";
    public static final String nextUpdate = "11111111-1111-1111-11110000";

    private static void showLog(int logType, String message) {

        // showOutPut(message);
        if (logType == 1) {
            Log.i(PMConstants.TAG, message);
        } else {
            Log.e(PMConstants.TAG, message);
        }
    }

    /**
     * Copy client db Cbor file from assets folder to app data files dir
     */
    public void copyCborFromAsset(Context context, String oicClientCborDbFile) {
        InputStream inputStream = null;
        OutputStream outputStream = null;
        int length;
        byte[] buffer = new byte[PMConstants.BUFFER_SIZE];
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
            Log.e(PMConstants.TAG, "Null pointer exception " + e.getMessage());
        } catch (FileNotFoundException e) {
            Log.e(PMConstants.TAG,
                    "Cbor svr db file not found " + e.getMessage());
        } catch (IOException e) {
            Log.e(PMConstants.TAG, oicClientCborDbFile + " file copy failed");
        } finally {
            if (inputStream != null) {
                try {
                    inputStream.close();
                } catch (IOException e) {
                    Log.e(PMConstants.TAG, e.getMessage());
                }
            }
            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (IOException e) {
                    Log.e(PMConstants.TAG, e.getMessage());
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
                Log.i(PMConstants.TAG, "Sql db directory created at " + dbPath);
            } else {
                Log.i(PMConstants.TAG, "Sql db directory exists at " + dbPath);
            }

            Log.i(PMConstants.TAG, " INIT ");
            OcProvisioning.provisionInit(dbPath + fileName);
            showLog(Info, " Initialization succesful");
        } catch (Exception e) {
            showLog(Error, "Initialization Error : " + e.getLocalizedMessage());
        }
    }

    public static OcAccountManager.OnPostListener onSignUp = new OcAccountManager.OnPostListener() {
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
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            OcAccountManager mAccountManager = OcPlatform
                    .constructAccountManagerObject(
                            "coap+tcp://107.109.211.225:5683",
                            EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP));

            mAccountManager.signUp(authProvider, authCode, onPostListener);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                Log.e(PMConstants.TAG, PMConstants.CallBackNotInvoked);
                PMConstants.mErrorMessage = PMConstants.CallBackNotInvoked;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean requestCertificate(OcCloudProvisioning cloudObj,
                                      RequestCertificateListener certificateIssueRequestListener) {
        try {
            showLog(Info, "requestCertificate IN");
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            cloudObj.requestCertificate(certificateIssueRequestListener);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                Log.e(PMConstants.TAG, PMConstants.CallBackNotInvoked);
                PMConstants.mErrorMessage = PMConstants.CallBackNotInvoked;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean getAclIdByDevice(OcCloudProvisioning cloudObj,
                                    String deviceId,
                                    GetAclIdByDeviceListener cloudAclIdGetByDeviceHandler) {
        try {
            showLog(Info, "getAclIdByDevice IN");
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            cloudObj.getAclIdByDevice(deviceId, cloudAclIdGetByDeviceHandler);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                Log.e(PMConstants.TAG, PMConstants.CallBackNotInvoked);
                PMConstants.mErrorMessage = PMConstants.CallBackNotInvoked;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean getIndividualAclInfo(OcCloudProvisioning cloudObj,
                                        String aclId,
                                        GetIndividualAclInfoListener cloudAclIndividualGetInfoHandler) {
        try {
            showLog(Info, "getIndividualAclInfo IN");
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            cloudObj.getIndividualAclInfo(aclId,
                    cloudAclIndividualGetInfoHandler);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                Log.e(PMConstants.TAG, PMConstants.CallBackNotInvoked);
                PMConstants.mErrorMessage = PMConstants.CallBackNotInvoked;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean getCRL(OcCloudProvisioning cloudObj,
                          GetCRLListener cloudGetCRLHandler) {
        try {
            showLog(Info, "getCRL IN");
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            cloudObj.getCRL(cloudGetCRLHandler);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                Log.e(PMConstants.TAG, PMConstants.CallBackNotInvoked);
                PMConstants.mErrorMessage = PMConstants.CallBackNotInvoked;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean postCRL(OcCloudProvisioning cloudObj, String thisUpdate,
                           String nextUpdate, String crl, ArrayList<String> serialNumbers,
                           PostCRLListener cloudPostCRLHandler) {
        try {
            showLog(Info, "postCRL IN");
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            cloudObj.postCRL(thisUpdate, nextUpdate, crl, serialNumbers,
                    cloudPostCRLHandler);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                Log.e(PMConstants.TAG, PMConstants.CallBackNotInvoked);
                PMConstants.mErrorMessage = PMConstants.CallBackNotInvoked;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    @Override
    public void getCRLListener(boolean arg0) {
        showLog(Info, "getCRLListener IN");
        mIsCbInvoked = PMConstants.CALLBACK_INVOKED;
    }

    @Override
    public void requestCertificateListener(boolean arg0) {
        showLog(Info, "requestCertificateListener IN");
        mIsCbInvoked = PMConstants.CALLBACK_INVOKED;

    }

    @Override
    public void postCRLListener(boolean arg0) {
        showLog(Info, "postCRLListener IN");
        mIsCbInvoked = PMConstants.CALLBACK_INVOKED;

    }

    @Override
    public void getIndividualAclInfoListener(boolean arg0) {
        showLog(Info, "getIndividualAclInfoListener IN");
        mIsCbInvoked = PMConstants.CALLBACK_INVOKED;

    }

    @Override
    public void getAclIdByDeviceListener(boolean arg0, String arg1) {
        showLog(Info, "getAclIdByDeviceListener IN");
        mIsCbInvoked = PMConstants.CALLBACK_INVOKED;

    }

    private boolean waitTillCBInvoke() {
        showLog(Info, "Checking if Callback is Invoked or Not");
        int count = 0;

        while (!mIsCbInvoked) {

            try {
                Thread.sleep(PMConstants.DELAY_SHORT);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            count++;
            Log.i(PMConstants.TAG, "Waiting for = " + count + " second/s");

            if (count > PMConstants.CALLBACK_TIMEOUT) {
                return PMConstants.CALLBACK_NOT_INVOKED;
            }
        }

        return PMConstants.CALLBACK_INVOKED;
    }

}
