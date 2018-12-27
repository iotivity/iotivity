/******************************************************************
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 *
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
 ******************************************************************/
package org.iotivity.test.cs.tc.helper;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;

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
import org.iotivity.test.cs.tc.helper.CSConstants;
import org.iotivity.testcase.IoTivityLog;
import org.iotivity.testcase.IoTivityTc;
import org.iotivity.test.ri.common.RIHelperCommon;

public class OcCloudProvisioningHelper extends RIHelperCommon
        implements OcCloudProvisioning.GetAclIdByDeviceListener,
        OcCloudProvisioning.GetIndividualAclInfoListener,
        OcCloudProvisioning.PostCRLListener,
        OcCloudProvisioning.RequestCertificateListener,
        OcCloudProvisioning.GetCRLListener, OcAccountManager.OnPostListener {

    public static boolean      mIsCbInvoked      = CSConstants.CALLBACK_NOT_INVOKED;
    public static String       mFilePath         = null;
    public static String       mSqlDbPath        = null;
    public static final int    Error             = -1;
    public static final int    Info              = 1;
    public static final String DEFAULT_HOST_IP   = "127.0.0.1";

    public static final int    DEFAULT_PORT      = 5683;

    public static final String DEFAULT_DEVICE_ID = "11111100-1111-1111-11111111";
    public static final String DEFAULT_ACL_ID    = "11111111-1111-1111-11111111";
    public static final String thisUpdate        = "11111111-1111-1111-11111111";
    public static final String nextUpdate        = "11111111-1111-1111-11110000";

    public OcCloudProvisioningHelper(IoTivityTc iotivityTcObj) {
        super(iotivityTcObj);
    }

    private static void showIoTivityLog(int IoTivityLogType, String message) {

        if (IoTivityLogType == 1) {
            IoTivityLog.i(CSConstants.TAG, message);
        } else {
            IoTivityLog.e(CSConstants.TAG, message);
        }
    }

    public void initOICStack(String dbPath, String fileName) {
        try {
            File file = new File(dbPath);
            // check files directory exists
            if (!(file.exists())) {
                file.mkdirs();
                IoTivityLog.i(CSConstants.TAG, "Sql db directory created at " + dbPath);
            } else {
                IoTivityLog.i(CSConstants.TAG, "Sql db directory exists at " + dbPath);
            }

            IoTivityLog.i(CSConstants.TAG, " INIT ");
            OcProvisioning.provisionInit(dbPath + fileName);
            showIoTivityLog(Info, " Initialization succesful");
        } catch (Exception e) {
            showIoTivityLog(Error, "Initialization Error : " + e.getLocalizedMessage());
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

                showIoTivityLog(Info, "\tuserID: " + mUserUuid);
                showIoTivityLog(Info, "\taccessToken: " + mAccesstoken);
                showIoTivityLog(Info, "\trefreshToken: " + mRefreshtoken);
                showIoTivityLog(Info, "\ttokenType: " + tokenType);

                if (ocRepresentation.hasAttribute("expiresin")) {
                    int expiresIn = ocRepresentation.getValue("expiresin");
                    showIoTivityLog(Info, "\texpiresIn: " + expiresIn);
                }

            } catch (OcException e) {
                showIoTivityLog(Error, e.toString());
            }
        }

        @Override
        public synchronized void onPostFailed(Throwable throwable) {
            showIoTivityLog(Error, "Failed to signUp");
            if (throwable instanceof OcException) {
                OcException ocEx = (OcException) throwable;
                showIoTivityLog(Error, ocEx.toString());
                ErrorCode errCode = ocEx.getErrorCode();
                showIoTivityLog(Error, "Error code: " + errCode);
            }
        }
    };

    public boolean signUp(String authProvider, String authCode,
            OnPostListener onPostListener) {
        try {
            showIoTivityLog(Info, "requestCertificate IN");
            mIsCbInvoked = CSConstants.CALLBACK_NOT_INVOKED;
            OcAccountManager mAccountManager = OcPlatform
                    .constructAccountManagerObject(
                            "coap+tcp://107.109.211.225:5683",
                            EnumSet.of(OcConnectivityType.CT_ADAPTER_TCP));

            mAccountManager.signUp(authProvider, authCode, onPostListener);

            if (CSConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                IoTivityLog.e(CSConstants.TAG, CSConstants.CALLBACK_NOT_INVOKED_MSG);
                CSConstants.mErrorMessage = CSConstants.CALLBACK_NOT_INVOKED_MSG;
                return false;
            }

            return true;
        } catch (OcException e) {
            showIoTivityLog(Error, e.getErrorCode().name());
            CSConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean requestCertificate(OcCloudProvisioning cloudObj,
            RequestCertificateListener certificateIssueRequestListener) {
        try {
            showIoTivityLog(Info, "requestCertificate IN");
            mIsCbInvoked = CSConstants.CALLBACK_NOT_INVOKED;
            cloudObj.requestCertificate(certificateIssueRequestListener);

            if (CSConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                IoTivityLog.e(CSConstants.TAG, CSConstants.CALLBACK_NOT_INVOKED_MSG);
                CSConstants.mErrorMessage = CSConstants.CALLBACK_NOT_INVOKED_MSG;
                return false;
            }

            return true;
        } catch (OcException e) {
            showIoTivityLog(Error, e.getErrorCode().name());
            CSConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean getAclIdByDevice(OcCloudProvisioning cloudObj,
            String deviceId,
            GetAclIdByDeviceListener cloudAclIdGetByDeviceHandler) {
        try {
            showIoTivityLog(Info, "getAclIdByDevice IN");
            mIsCbInvoked = CSConstants.CALLBACK_NOT_INVOKED;
            cloudObj.getAclIdByDevice(deviceId, cloudAclIdGetByDeviceHandler);

            if (CSConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                IoTivityLog.e(CSConstants.TAG, CSConstants.CALLBACK_NOT_INVOKED_MSG);
                CSConstants.mErrorMessage = CSConstants.CALLBACK_NOT_INVOKED_MSG;
                return false;
            }

            return true;
        } catch (OcException e) {
            showIoTivityLog(Error, e.getErrorCode().name());
            CSConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean getIndividualAclInfo(OcCloudProvisioning cloudObj,
            String aclId,
            GetIndividualAclInfoListener cloudAclIndividualGetInfoHandler) {
        try {
            showIoTivityLog(Info, "getIndividualAclInfo IN");
            mIsCbInvoked = CSConstants.CALLBACK_NOT_INVOKED;
            cloudObj.getIndividualAclInfo(aclId,
                    cloudAclIndividualGetInfoHandler);

            if (CSConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                IoTivityLog.e(CSConstants.TAG, CSConstants.CALLBACK_NOT_INVOKED_MSG);
                CSConstants.mErrorMessage = CSConstants.CALLBACK_NOT_INVOKED_MSG;
                return false;
            }

            return true;
        } catch (OcException e) {
            showIoTivityLog(Error, e.getErrorCode().name());
            CSConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean getCRL(OcCloudProvisioning cloudObj,
            GetCRLListener cloudGetCRLHandler) {
        try {
            showIoTivityLog(Info, "getCRL IN");
            mIsCbInvoked = CSConstants.CALLBACK_NOT_INVOKED;
            cloudObj.getCRL(cloudGetCRLHandler);

            if (CSConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                IoTivityLog.e(CSConstants.TAG, CSConstants.CALLBACK_NOT_INVOKED_MSG);
                CSConstants.mErrorMessage = CSConstants.CALLBACK_NOT_INVOKED_MSG;
                return false;
            }

            return true;
        } catch (OcException e) {
            showIoTivityLog(Error, e.getErrorCode().name());
            CSConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean postCRL(OcCloudProvisioning cloudObj, String thisUpdate,
            String nextUpdate, String crl, ArrayList<String> serialNumbers,
            PostCRLListener cloudPostCRLHandler) {
        try {
            showIoTivityLog(Info, "postCRL IN");
            mIsCbInvoked = CSConstants.CALLBACK_NOT_INVOKED;
            cloudObj.postCRL(thisUpdate, nextUpdate, crl, serialNumbers,
                    cloudPostCRLHandler);

            if (CSConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                IoTivityLog.e(CSConstants.TAG, CSConstants.CALLBACK_NOT_INVOKED_MSG);
                CSConstants.mErrorMessage = CSConstants.CALLBACK_NOT_INVOKED_MSG;
                return false;
            }

            return true;
        } catch (OcException e) {
            showIoTivityLog(Error, e.getErrorCode().name());
            CSConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    @Override
    public void getAclIdByDeviceListener(int i, String aclId) {
        showIoTivityLog(Info, "getAclIdByDeviceListener IN");
        showIoTivityLog(Info, "ACL ID = " + aclId);
        mIsCbInvoked = CSConstants.CALLBACK_INVOKED;
    }

    @Override
    public void getCRLListener(int i) {
        showIoTivityLog(Info, "getCRLListener IN");
        showIoTivityLog(Info, "getCRLListener Callback result = " + i);
        mIsCbInvoked = CSConstants.CALLBACK_INVOKED;
    }

    @Override
    public void getIndividualAclInfoListener(int i) {
        showIoTivityLog(Info, "getIndividualAclInfoListener IN");
        showIoTivityLog(Info, "getIndividualAclInfoListener Callback result = " + i);
        mIsCbInvoked = CSConstants.CALLBACK_INVOKED;
    }

    @Override
    public void postCRLListener(int i) {
        showIoTivityLog(Info, "postCRLListener IN");
        showIoTivityLog(Info, "postCRLListener Callback result = " + i);
        mIsCbInvoked = CSConstants.CALLBACK_INVOKED;
    }

    @Override
    public void requestCertificateListener(int i) {
        showIoTivityLog(Info, "requestCertificateListener IN");
        showIoTivityLog(Info, "requestCertificateListener Callback result = " + i);
        mIsCbInvoked = CSConstants.CALLBACK_INVOKED;
    }

    private boolean waitTillCBInvoke() {
        showIoTivityLog(Info, "Checking if Callback is Invoked or Not");
        int count = 0;

        while (!mIsCbInvoked) {

            try {
                Thread.sleep(CSConstants.DELAY_SHORT);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            count++;
            IoTivityLog.i(CSConstants.TAG, "Waiting for = " + count + " second/s");

            if (count > CSConstants.CALLBACK_TIMEOUT) {
                return CSConstants.CALLBACK_NOT_INVOKED;
            }
        }

        return CSConstants.CALLBACK_INVOKED;
    }

    @Override
    public void onPostCompleted(List<OcHeaderOption> arg0, OcRepresentation arg1) {

    }

    @Override
    public void onPostFailed(Throwable arg0) {

    }

}
