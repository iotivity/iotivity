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

package org.iotivity.test.pm.tc.helper;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;

import org.iotivity.base.CredType;
import org.iotivity.base.PinType;
import org.iotivity.base.MVJustWorksOptionMask;
import org.iotivity.base.DeviceStatus;
import org.iotivity.base.KeySize;
import org.iotivity.base.MomType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPrmType;
import org.iotivity.base.OcProvisioning;
import org.iotivity.base.OwnedStatus;
import org.iotivity.base.OcProvisioning.DisplayPinListener;
import org.iotivity.base.OcProvisioning.DisplayNumListener;
import org.iotivity.base.OcProvisioning.ConfirmNumListener;
import org.iotivity.base.OcProvisioning.PinCallbackListener;
import org.iotivity.base.OcSecureResource.DoOwnershipTransferListener;
import org.iotivity.base.OcSecureResource.ProvisionAclListener;
import org.iotivity.base.OcSecureResource.ChangeMOTModeListener;
import org.iotivity.base.OcSecureResource.ProvisionPreConfigPINListener;
import org.iotivity.base.OcSecureResource.DoMultipleOwnershipTransferListener;
import org.iotivity.base.OcSecureResource.SelectOTMMethodListener;
import org.iotivity.base.OcSecureResource.ProvisionCredentialsListener;
import org.iotivity.base.OcSecureResource.ProvisionDirectPairingListener;
import org.iotivity.base.OcSecureResource.ProvisionPairwiseDevicesListener;
import org.iotivity.base.OcSecureResource.RemoveDeviceListener;
import org.iotivity.base.OcSecureResource.UnlinkDevicesListener;
import org.iotivity.base.OxmType;
import org.iotivity.base.ProvisionResult;
import org.iotivity.base.OcSecureResource;
import org.iotivity.base.OicSecAce;
import org.iotivity.base.OicSecAcl;
import org.iotivity.base.OicSecPdAcl;
import org.iotivity.base.OicSecResr;
import org.iotivity.base.OicSecValidity;

import org.iotivity.configuration.ReadConfigPropFile;
import org.iotivity.testcase.IoTivityLog;
import org.iotivity.testcase.IoTivityTc;
import org.iotivity.test.ri.common.RIHelperCommon;
import org.iotivity.test.pm.tc.helper.PMHelper;
import org.iotivity.test.pm.tc.helper.PMConstants;

public class PMHelper extends RIHelperCommon
        implements OcSecureResource.DoOwnershipTransferListener,
        OcSecureResource.DoMultipleOwnershipTransferListener,
        OcSecureResource.ProvisionDirectPairingListener,
        OcSecureResource.ProvisionPairwiseDevicesListener {

    public static List<OcSecureResource> mUnownedDeviceList, mOwnedDeviceList,
            mMOTEnableDeviceList, mMOTEnableOwnedDeviceList;
    public static int                    mUnownedDeviceCount, mOwnedDeviceCount,
            mMOTEnableDeviceCount, mMOTEnableOwnedDeviceCount;
    private ArrayList<String>            allowedDevList   = new ArrayList<String>();

    public static boolean                mIsCbInvoked     = PMConstants.CALLBACK_NOT_INVOKED;
    public static String                 mFilePath        = null;
    public static String                 mSqlDbPath       = null;
    public static String                 mServerFilePath  = null;
    public static String                 mServerSqlDbPath = null;

    public static final int              Error            = -1;
    public static final int              Info             = 1;

    public PMHelper(IoTivityTc iotivityTcObj) {
        super(iotivityTcObj);
        allowedDevList.add("12345678-1234-1234-1234-123456780001");
        allowedDevList.add("12345678-1234-1234-1234-123456789012");
        ReadConfigPropFile.readConfigFile(CONFIG_FILE_PATH);
    }

    private static void showLog(int logType, String message) {
        if (logType == 1) {
            IoTivityLog.i(PMConstants.TAG, message);
        } else {
            IoTivityLog.e(PMConstants.TAG, message);
        }
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
            IoTivityLog.i(PMConstants.TAG,
                    "Waiting for = " + count + " second/s");

            if (count > PMConstants.CALLBACK_TIMEOUT) {
                return PMConstants.CALLBACK_NOT_INVOKED;
            }
        }

        return PMConstants.CALLBACK_INVOKED;
    }

    public void initOICStack(String dbPath, String fileName) {
        try {
            File file = new File(dbPath);
            File file1 = new File(dbPath + fileName);

            if (file1.exists()) {
                file1.delete();
            }

            if (!(file.exists())) {
                file.mkdirs();
                IoTivityLog.i(PMConstants.TAG,
                        "Sql db directory created at " + dbPath);
            } else {
                IoTivityLog.i(PMConstants.TAG,
                        "Sql db directory exists at " + dbPath);
            }

            IoTivityLog.i(PMConstants.TAG, " INIT ");
            OcProvisioning.provisionInit(dbPath + fileName);
            IoTivityLog.i(PMConstants.TAG, "Initialization succesful");
        } catch (Exception e) {
            IoTivityLog.i(PMConstants.TAG,
                    "Initialization Error : " + e.getLocalizedMessage());
        }
    }

    public boolean discoverUnownedDevices(int timeout) {
        try {
            showLog(Info, "Discovering Unowned Devices");
            List<OcSecureResource> tempDeviceList = new ArrayList<>();
            tempDeviceList.clear();
            tempDeviceList = OcProvisioning.discoverUnownedDevices(timeout);
            mUnownedDeviceCount = tempDeviceList.size();
            showLog(Info, "Unowned Device Count : " + mUnownedDeviceCount);

            mUnownedDeviceList = new ArrayList<OcSecureResource>();

            for (int i = 0; i < mUnownedDeviceCount; i++) {
                showLog(Info,
                        (i + 1) + ". Unowned Device ID: "
                                + tempDeviceList.get(i).getDeviceID()
                                + " , IP Address: "
                                + tempDeviceList.get(i).getIpAddr());
            }

            showLog(Info,
                    "Expected Ipv4 :" + ReadConfigPropFile.s_mTcpCIServerIp);
            showLog(Info,
                    "Expected Ipv6 :" + ReadConfigPropFile.s_mTcpCIServerIpv6);

            // Filtering Devices based on device ID and IP
            for (int i = 0; i < mUnownedDeviceCount; i++) {
                for (String allowedId : allowedDevList) {
                    if (tempDeviceList.get(i).getDeviceID().equals(allowedId)) {
                        if (ReadConfigPropFile.s_mTcpCIServerIp
                                .equals(tempDeviceList.get(i).getIpAddr())
                                || ReadConfigPropFile.s_mTcpCIServerIpv6.equals(
                                        tempDeviceList.get(i).getIpAddr())) {
                            mUnownedDeviceList.add(tempDeviceList.get(i));
                        }
                    }
                }
            }

            mUnownedDeviceCount = mUnownedDeviceList.size();
            showLog(Info, "Total Found Filtered Unowned Device : "
                    + mUnownedDeviceCount);

            for (int i = 0; i < mUnownedDeviceCount; i++) {
                showLog(Info,
                        (i + 1) + ". Unowned Device ID: "
                                + mUnownedDeviceList.get(i).getDeviceID()
                                + " , IP Address: "
                                + mUnownedDeviceList.get(i).getIpAddr());
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean discoverOwnedDevices(int timeout) {
        try {
            showLog(Info, "Discovering Owned Devices");
            List<OcSecureResource> tempDeviceList = OcProvisioning
                    .discoverOwnedDevices(timeout);
            mOwnedDeviceCount = tempDeviceList.size();
            mOwnedDeviceList = new ArrayList<OcSecureResource>();

            for (int i = 0; i < mOwnedDeviceCount; i++) {
                showLog(Info,
                        (i + 1) + ". Owned Device ID: "
                                + tempDeviceList.get(i).getDeviceID()
                                + " , IP Address: "
                                + tempDeviceList.get(i).getIpAddr());
            }

            for (int i = 0; i < mOwnedDeviceCount; i++) {

                for (String allowedId : allowedDevList) {
                    if (tempDeviceList.get(i).getDeviceID().equals(allowedId)) {
                        mOwnedDeviceList.add(tempDeviceList.get(i));
                    }
                }
            }

            mOwnedDeviceCount = mOwnedDeviceList.size();
            showLog(Info,
                    "Total Found Filtered Owned Device : " + mOwnedDeviceCount);

            for (int i = 0; i < mOwnedDeviceCount; i++) {
                showLog(Info, (i + 1) + ". Owned Device ID: "
                        + mOwnedDeviceList.get(i).getDeviceID());
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean discoverMOTEnabledDevices(int timeout) {
        try {
            showLog(Info, "Discovering MOT Enable Devices");
            mMOTEnableDeviceList = OcProvisioning
                    .discoverMOTEnabledDevices(timeout);
            mMOTEnableDeviceCount = mMOTEnableDeviceList.size();
            showLog(Info,
                    "Total Found Owned Device : " + mMOTEnableDeviceCount);

            for (int i = 0; i < mMOTEnableDeviceCount; i++) {
                showLog(Info, (i + 1) + ". Owned Device ID: "
                        + mMOTEnableDeviceList.get(i).getDeviceID());
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean discoverMOTEnabledOwnedDevices(int timeout) {
        try {
            showLog(Info, "Discovering Owned Devices");
            mMOTEnableOwnedDeviceList = OcProvisioning
                    .discoverMOTEnabledOwnedDevices(timeout);
            mMOTEnableOwnedDeviceCount = mMOTEnableOwnedDeviceList.size();
            showLog(Info,
                    "Total Found Owned Device : " + mMOTEnableOwnedDeviceCount);

            for (int i = 0; i < mMOTEnableOwnedDeviceCount; i++) {
                showLog(Info, (i + 1) + ". Owned Device ID: "
                        + mMOTEnableOwnedDeviceList.get(i).getDeviceID());
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean setOwnershipTransferCBdata(OxmType type,
            PinCallbackListener pinCallbackListener) {
        try {
            OcProvisioning.SetownershipTransferCBdata(type,
                    pinCallbackListener);
            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean setPinType(int pinSize, EnumSet<PinType> pinType) {

        try {
            OcProvisioning.setPinType(pinSize, pinType);
            return true;

        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean setMVJustWorksOptions(
            EnumSet<MVJustWorksOptionMask> optionMask) {

        try {
            OcProvisioning.setMVJustWorksOptions(optionMask);
            return true;

        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean setDisplayPinListener(
            DisplayPinListener displayPinListener) {
        try {
            OcProvisioning.setDisplayPinListener(displayPinListener);
            return true;

        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean setDisplayNumListener(
            DisplayNumListener displayNumListener) {
        try {
            OcProvisioning.setDisplayNumListener(displayNumListener);
            return true;

        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean setConfirmNumListener(
            ConfirmNumListener confirmNumListener) {
        try {
            OcProvisioning.setConfirmNumListener(confirmNumListener);
            return true;

        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean getDeviceStatusList(int timeout) {
        try {
            List<OcSecureResource> deviceList = new ArrayList<OcSecureResource>(
                    OcProvisioning.getDeviceStatusList(timeout));

            for (int i = 0; i < deviceList.size(); i++) {
                IoTivityLog.i(PMConstants.TAG, (i + 1) + ". Device ID: "
                        + deviceList.get(i).getDeviceID());
            }

            return true;
        } catch (OcException e) {
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean doOwnershipTransfer(
            DoOwnershipTransferListener doOwnershipTransferListener) {
        try {
            showLog(Info, "Transferring Ownership");
            for (int i = 0; i < mUnownedDeviceList.size(); i++) {
                mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
                showLog(Info,
                        "Ownership Trasfer for Device[" + (i + 1) + "] "
                                + mUnownedDeviceList.get(i).getDeviceID()
                                + " , IP Address: "
                                + mUnownedDeviceList.get(i).getIpAddr());
                mUnownedDeviceList.get(i)
                        .doOwnershipTransfer(doOwnershipTransferListener);

                if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                    showLog(Error, PMConstants.CALLBACK_NOT_INVOKED_MSG);
                    PMConstants.mErrorMessage = PMConstants.CALLBACK_NOT_INVOKED_MSG;
                    return false;
                }
            }
            return true;

        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean changeMOTMode(OcSecureResource device1, MomType momType,
            ChangeMOTModeListener changeMOTModeListener) {
        try {
            showLog(Info, "Change MOT Mode");
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            device1.changeMOTMode(momType, changeMOTModeListener);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                IoTivityLog.e(PMConstants.TAG, PMConstants.CALLBACK_NOT_INVOKED_MSG);
                PMConstants.mErrorMessage = PMConstants.CALLBACK_NOT_INVOKED_MSG;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean provisionPreConfigPIN(OcSecureResource device1, String pin,
            ProvisionPreConfigPINListener provisionPreConfigPINListener) {
        try {
            showLog(Info, "Provision PreConfig Pin");
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            device1.provisionPreConfigPIN(pin, provisionPreConfigPINListener);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                IoTivityLog.e(PMConstants.TAG, PMConstants.CALLBACK_NOT_INVOKED_MSG);
                PMConstants.mErrorMessage = PMConstants.CALLBACK_NOT_INVOKED_MSG;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean selectMOTMethod(OcSecureResource device1, OxmType oxmType,
            SelectOTMMethodListener selectOTMMethodListener) {
        try {
            showLog(Info, "Select MOT Method");
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            device1.selectMOTMethod(oxmType, selectOTMMethodListener);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                IoTivityLog.e(PMConstants.TAG, PMConstants.CALLBACK_NOT_INVOKED_MSG);
                PMConstants.mErrorMessage = PMConstants.CALLBACK_NOT_INVOKED_MSG;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean addPreConfigPIN(OcSecureResource device1, String pin) {
        try {
            showLog(Info, "Add PreConfig Pin");
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            device1.addPreConfigPIN(pin);

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean doMultipleOwnershipTransfer(
            DoMultipleOwnershipTransferListener doMultipleOwnershipTransferListener) {
        try {
            showLog(Info, "Transferring Multiple Ownership");
            for (int i = 0; i < mMOTEnableDeviceList.size(); i++) {
                mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
                showLog(Info, "Multiple Ownership Trasfer for Device[" + i + 1
                        + "] " + mMOTEnableDeviceList.get(i).getDeviceID());
                mMOTEnableDeviceList.get(i).doMultipleOwnershipTransfer(
                        doMultipleOwnershipTransferListener);

                if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                    showLog(Error, PMConstants.CALLBACK_NOT_INVOKED_MSG);
                    PMConstants.mErrorMessage = PMConstants.CALLBACK_NOT_INVOKED_MSG;
                    return false;
                }
            }
            return true;

        } catch (Exception e) {
            showLog(Error, e.getMessage());
            PMConstants.mErrorMessage = e.getMessage();
            return false;
        }
    }

    public boolean provisionACL(OcSecureResource device1, Object acl,
            ProvisionAclListener provisionACLListener) {
        try {
            showLog(Info, "Provisioning ACL");
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            device1.provisionACL(acl, provisionACLListener);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                IoTivityLog.e(PMConstants.TAG, PMConstants.CALLBACK_NOT_INVOKED_MSG);
                PMConstants.mErrorMessage = PMConstants.CALLBACK_NOT_INVOKED_MSG;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean saveACL(Object acl) {
        try {
            showLog(Info, "saveACL ACL");
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            OcProvisioning.saveACL(acl);

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean provisionPairwiseDevices(EnumSet<CredType> credTypeSet,
            KeySize keysize, OcSecureResource device1, Object acl1,
            OcSecureResource device2, Object acl2,
            ProvisionPairwiseDevicesListener provisionPairwiseDevicesListener) {
        try {
            showLog(Info, "Provisioning Pairwise Devices");

            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            device1.provisionPairwiseDevices(credTypeSet, keysize, acl1,
                    device2, acl2, provisionPairwiseDevicesListener);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                IoTivityLog.e(PMConstants.TAG, PMConstants.CALLBACK_NOT_INVOKED_MSG);
                PMConstants.mErrorMessage = PMConstants.CALLBACK_NOT_INVOKED_MSG;
                return false;
            }
            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean provisionCredentials(EnumSet<CredType> credTypeSet,
            KeySize keysize, OcSecureResource device1, OcSecureResource device2,
            ProvisionCredentialsListener provisionCredentialsListener) {
        try {
            showLog(Info, "Provisioning Credential");

            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            device1.provisionCredentials(credTypeSet, keysize, device2,
                    provisionCredentialsListener);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                showLog(Error, PMConstants.CALLBACK_NOT_INVOKED_MSG);
                PMConstants.mErrorMessage = PMConstants.CALLBACK_NOT_INVOKED_MSG;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean unlinkDevices(OcSecureResource sourceDevice,
            OcSecureResource targetDevice,
            UnlinkDevicesListener unlinkDevicesListener) {
        try {
            showLog(Info, "Unlink Devices");
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            sourceDevice.unlinkDevices(targetDevice, unlinkDevicesListener);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                IoTivityLog.e(PMConstants.TAG, PMConstants.CALLBACK_NOT_INVOKED_MSG);
                PMConstants.mErrorMessage = PMConstants.CALLBACK_NOT_INVOKED_MSG;
                return false;
            }

            return true;
        } catch (OcException e) {
            showLog(Error, e.getErrorCode().name());
            PMConstants.mErrorMessage = e.getErrorCode().name();
            return false;
        }
    }

    public boolean removeDevice(OcSecureResource sourceDevice, int timeout,
            RemoveDeviceListener removeDeviceListener) {
        try {
            showLog(Info, "Remove Devices");
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            sourceDevice.removeDevice(timeout, removeDeviceListener);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                IoTivityLog.e(PMConstants.TAG, PMConstants.CALLBACK_NOT_INVOKED_MSG);
                PMConstants.mErrorMessage = PMConstants.CALLBACK_NOT_INVOKED_MSG;
                return false;
            }

            return true;
        } catch (Exception e) {
            showLog(Error, e.getLocalizedMessage());
            PMConstants.mErrorMessage = e.getLocalizedMessage();
            return false;
        }
    }

    public boolean getLinkedDevices(OcSecureResource sourceDevice) {
        try {
            showLog(Info, "Getlinked Devices");
            List<String> linkedDevices = sourceDevice.getLinkedDevices();
            showLog(Info, "DeviceList " + linkedDevices.size());
            for (int i = 0; i < linkedDevices.size(); i++) {
                showLog(Info, "Device No :" + i + " Device ID : "
                        + linkedDevices.get(i));
            }

            return true;
        } catch (Exception e) {
            showLog(Error, e.getLocalizedMessage());
            PMConstants.mErrorMessage = e.getLocalizedMessage();
            return false;
        }
    }

    public boolean getIpAddr(OcSecureResource device) {
        try {
            String ipAddress = device.getIpAddr();
            showLog(Info, "Device IP Address: " + ipAddress);
            return true;
        } catch (Exception e) {
            showLog(Error, e.getLocalizedMessage());
            PMConstants.mErrorMessage = e.getLocalizedMessage();
            return false;
        }
    }

    public boolean getDeviceID(OcSecureResource device) {
        try {
            String deviceID = device.getDeviceID();
            showLog(Info, "Device ID: " + deviceID);
            return true;
        } catch (Exception e) {
            showLog(Error, e.getLocalizedMessage());
            PMConstants.mErrorMessage = e.getLocalizedMessage();
            return false;
        }
    }

    public boolean getDeviceStatus(OcSecureResource device,
            DeviceStatus expectedStatus) {
        try {
            DeviceStatus actualStatus = device.getDeviceStatus();
            showLog(Info, "Device Status: " + actualStatus.name());

            if (expectedStatus != actualStatus) {
                PMConstants.mErrorMessage = "Wrong Device Status";
                return false;
            }
            return true;
        } catch (Exception e) {
            showLog(Error, e.getLocalizedMessage());
            PMConstants.mErrorMessage = e.getLocalizedMessage();
            return false;
        }

    }

    public boolean getOwnedStatus(OcSecureResource device,
            OwnedStatus expectedOwnedStatus) {
        try {
            OwnedStatus actualOwnedStatus = device.getOwnedStatus();
            IoTivityLog.i(PMConstants.TAG,
                    "Device Owned Status: " + actualOwnedStatus.name());

            if (expectedOwnedStatus != actualOwnedStatus) {
                PMConstants.mErrorMessage = "Wrong Device Owned Status";
                return false;
            }

            return true;
        } catch (Exception e) {
            showLog(Error, e.getLocalizedMessage());
            PMConstants.mErrorMessage = e.getLocalizedMessage();
            return false;
        }
    }

    public List<OicSecAcl> createAcl(OcSecureResource ownedDev) {
        OcSecureResource ownedDevice01 = ownedDev;
        List<OicSecResr> resources1 = new ArrayList<OicSecResr>();
        List<OicSecValidity> validities = new ArrayList<OicSecValidity>();

        List<String> types1 = new ArrayList<String>();
        types1.add(PMConstants.RESOURCE_TYPE_1A);
        types1.add(PMConstants.RESOURCE_TYPE_1B);

        List<String> interfaces = new ArrayList<String>();
        interfaces.add(PMConstants.RESOURCE_INTERFACE_1);
        interfaces.add(PMConstants.RESOURCE_INTERFACE_2);

        OicSecResr oicSecResr1a = new OicSecResr(PMConstants.HREF_RESOURCES_1A,
                "", types1, 2, interfaces, 2);
        OicSecResr oicSecResr1b = new OicSecResr(PMConstants.HREF_RESOURCES_1B,
                "", types1, 2, interfaces, 2);
        resources1.add(oicSecResr1a);
        resources1.add(oicSecResr1b);

        List<String> recurrences = new ArrayList<String>();
        recurrences.add(PMConstants.DEFAULT_RECURRENCES_1);
        recurrences.add(PMConstants.DEFAULT_RECURRENCES_2);
        OicSecValidity oicSecValidity = new OicSecValidity(
                PMConstants.DEFAULT_PERIOD, recurrences, 2);
        validities.add(oicSecValidity);

        OicSecAce oicsecace1 = new OicSecAce(ownedDevice01.getDeviceID(),
                PMConstants.DEFAULT_PERMISSION, resources1, validities);

        List<OicSecAce> oicSecAces1 = new ArrayList<OicSecAce>();
        oicSecAces1.add(oicsecace1);

        OicSecAcl acl1 = new OicSecAcl(PMConstants.DEFAULT_ROWNER_ID,
                oicSecAces1);

        List<OicSecAcl> aclList = new ArrayList<OicSecAcl>();
        aclList.add(acl1);

        return aclList;

    }

    public List<OicSecAcl> createAcl(OcSecureResource ownedDev1,
            OcSecureResource ownedDev2) {
        OcSecureResource ownedDevice01 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ZERO);
        OcSecureResource ownedDevice02 = PMHelper.mOwnedDeviceList
                .get(PMConstants.DEVICE_INDEX_ONE);
        List<OicSecResr> resources1 = new ArrayList<OicSecResr>();
        List<OicSecResr> resources2 = new ArrayList<OicSecResr>();
        List<OicSecValidity> validities = new ArrayList<OicSecValidity>();

        List<String> types1 = new ArrayList<String>();
        types1.add(PMConstants.RESOURCE_TYPE_1A);
        types1.add(PMConstants.RESOURCE_TYPE_1B);
        List<String> types2 = new ArrayList<String>();
        types2.add(PMConstants.RESOURCE_TYPE_2A);
        types2.add(PMConstants.RESOURCE_TYPE_2B);
        List<String> interfaces = new ArrayList<String>();
        interfaces.add(PMConstants.RESOURCE_INTERFACE_1);
        interfaces.add(PMConstants.RESOURCE_INTERFACE_2);

        OicSecResr oicSecResr1a = new OicSecResr(PMConstants.HREF_RESOURCES_1A,
                "", types1, 2, interfaces, 2);
        OicSecResr oicSecResr1b = new OicSecResr(PMConstants.HREF_RESOURCES_1B,
                "", types1, 2, interfaces, 2);
        resources1.add(oicSecResr1a);
        resources1.add(oicSecResr1b);

        OicSecResr oicSecResr2a = new OicSecResr(PMConstants.HREF_RESOURCES_2A,
                "", types2, 2, interfaces, 2);
        OicSecResr oicSecResr2b = new OicSecResr(PMConstants.HREF_RESOURCES_2B,
                "", types2, 2, interfaces, 2);
        resources2.add(oicSecResr2a);
        resources2.add(oicSecResr2b);

        List<String> recurrences = new ArrayList<String>();
        recurrences.add(PMConstants.DEFAULT_RECURRENCES_1);
        recurrences.add(PMConstants.DEFAULT_RECURRENCES_2);
        OicSecValidity oicSecValidity = new OicSecValidity(
                PMConstants.DEFAULT_PERIOD, recurrences, 2);
        validities.add(oicSecValidity);

        OicSecAce oicsecace1 = new OicSecAce(ownedDevice01.getDeviceID(),
                PMConstants.DEFAULT_PERMISSION, resources1, validities);
        OicSecAce oicsecace2 = new OicSecAce(ownedDevice02.getDeviceID(),
                PMConstants.DEFAULT_PERMISSION, resources2, validities);

        List<OicSecAce> oicSecAces1 = new ArrayList<OicSecAce>();
        oicSecAces1.add(oicsecace1);
        List<OicSecAce> oicSecAces2 = new ArrayList<OicSecAce>();
        oicSecAces2.add(oicsecace2);

        OicSecAcl acl1 = new OicSecAcl(PMConstants.DEFAULT_ROWNER_ID,
                oicSecAces1);
        OicSecAcl acl2 = new OicSecAcl(PMConstants.DEFAULT_ROWNER_ID,
                oicSecAces2);

        List<OicSecAcl> aclList = new ArrayList<OicSecAcl>();
        aclList.add(acl1);
        aclList.add(acl2);

        return aclList;

    }

    public static OcProvisioning.DisplayPinListener              displayPinListener            = new OcProvisioning.DisplayPinListener() {

                                                                                                   @Override
                                                                                                   public void displayPinListener(
                                                                                                           String pinCode) {
                                                                                                       showLog(Info,
                                                                                                               "Genearated Pin Code : "
                                                                                                                       + pinCode);
                                                                                                   }
                                                                                               };

    public static OcProvisioning.DisplayNumListener              displayNumListener            = new OcProvisioning.DisplayNumListener() {

                                                                                                   @Override
                                                                                                   public int displayNumListener(
                                                                                                           String verifyNum) {
                                                                                                       showLog(Info,
                                                                                                               "Genearated Pin Code : "
                                                                                                                       + verifyNum);
                                                                                                       return 1;
                                                                                                   }
                                                                                               };

    public static OcProvisioning.ConfirmNumListener              confirmNumListener            = new OcProvisioning.ConfirmNumListener() {

                                                                                                   @Override
                                                                                                   public int confirmNumListener() {
                                                                                                       // showLog(Info,
                                                                                                       // "Genearated
                                                                                                       // Pin
                                                                                                       // Code
                                                                                                       // :
                                                                                                       // "
                                                                                                       // +
                                                                                                       // verifyNum);
                                                                                                       return 1;
                                                                                                   }
                                                                                               };

    public static OcProvisioning.PinCallbackListener             pinCallbackListener           = new OcProvisioning.PinCallbackListener() {
                                                                                                   @Override
                                                                                                   public String pinCallbackListener() {
                                                                                                       showLog(Info,
                                                                                                               "Inside Pin Callback ");
                                                                                                       mIsCbInvoked = PMConstants.CALLBACK_INVOKED;
                                                                                                       return PMConstants.InputPinCBCallbackInvoked;
                                                                                                   }
                                                                                               };

    public static OcSecureResource.ProvisionAclListener          provisionAclListener          = new OcSecureResource.ProvisionAclListener() {
                                                                                                   @Override
                                                                                                   public void provisionAclListener(
                                                                                                           List<ProvisionResult> provisionResults,
                                                                                                           int hasError) {
                                                                                                       showLog(Info,
                                                                                                               "ProvisionAclListener IN -> Error Code : "
                                                                                                                       + hasError);

                                                                                                       if (hasError == PMConstants.ERROR_CODE) {
                                                                                                           IoTivityLog
                                                                                                                   .e(PMConstants.TAG,
                                                                                                                           "ACL Provisioning Failed");
                                                                                                       } else {
                                                                                                           IoTivityLog
                                                                                                                   .i(PMConstants.TAG,
                                                                                                                           "ACL Provisioning Done!");
                                                                                                       }

                                                                                                       mIsCbInvoked = PMConstants.CALLBACK_INVOKED;
                                                                                                       IoTivityLog
                                                                                                               .i(PMConstants.TAG,
                                                                                                                       "ProvisionAclListener OUT");
                                                                                                   }
                                                                                               };

    public static OcSecureResource.ChangeMOTModeListener         changeMOTModeListener         = new OcSecureResource.ChangeMOTModeListener() {
                                                                                                   @Override
                                                                                                   public void changeMOTModeListener(
                                                                                                           List<ProvisionResult> provisionResults,
                                                                                                           int hasError) {
                                                                                                       showLog(Info,
                                                                                                               "ChangeMOTModeListener IN -> Error Code : "
                                                                                                                       + hasError);

                                                                                                       if (hasError == PMConstants.ERROR_CODE) {
                                                                                                           IoTivityLog
                                                                                                                   .e(PMConstants.TAG,
                                                                                                                           "ACL Provisioning Failed");
                                                                                                       } else {
                                                                                                           IoTivityLog
                                                                                                                   .i(PMConstants.TAG,
                                                                                                                           "ACL Provisioning Done!");
                                                                                                       }

                                                                                                       mIsCbInvoked = PMConstants.CALLBACK_INVOKED;
                                                                                                       IoTivityLog
                                                                                                               .i(PMConstants.TAG,
                                                                                                                       "ChangeMOTModeListener OUT");
                                                                                                   }
                                                                                               };

    public static OcSecureResource.ProvisionPreConfigPINListener provisionPreConfigPINListener = new OcSecureResource.ProvisionPreConfigPINListener() {
                                                                                                   @Override
                                                                                                   public void provisionPreConfigPINListener(
                                                                                                           List<ProvisionResult> provisionResults,
                                                                                                           int hasError) {
                                                                                                       showLog(Info,
                                                                                                               "ProvisionPreConfigPINListener IN -> Error Code : "
                                                                                                                       + hasError);

                                                                                                       if (hasError == PMConstants.ERROR_CODE) {
                                                                                                           IoTivityLog
                                                                                                                   .e(PMConstants.TAG,
                                                                                                                           "ACL Provisioning Failed");
                                                                                                       } else {
                                                                                                           IoTivityLog
                                                                                                                   .i(PMConstants.TAG,
                                                                                                                           "ACL Provisioning Done!");
                                                                                                       }

                                                                                                       mIsCbInvoked = PMConstants.CALLBACK_INVOKED;
                                                                                                       IoTivityLog
                                                                                                               .i(PMConstants.TAG,
                                                                                                                       "ProvisionPreConfigPINListener OUT");
                                                                                                   }
                                                                                               };

    public static OcSecureResource.SelectOTMMethodListener       selectOTMMethodListener       = new OcSecureResource.SelectOTMMethodListener() {
                                                                                                   @Override
                                                                                                   public void selectOTMMethodListener(
                                                                                                           List<ProvisionResult> provisionResults,
                                                                                                           int hasError) {
                                                                                                       showLog(Info,
                                                                                                               "SelectOTMMethodListener IN -> Error Code : "
                                                                                                                       + hasError);

                                                                                                       if (hasError == PMConstants.ERROR_CODE) {
                                                                                                           IoTivityLog
                                                                                                                   .e(PMConstants.TAG,
                                                                                                                           "ACL Provisioning Failed");
                                                                                                       } else {
                                                                                                           IoTivityLog
                                                                                                                   .i(PMConstants.TAG,
                                                                                                                           "ACL Provisioning Done!");
                                                                                                       }

                                                                                                       mIsCbInvoked = PMConstants.CALLBACK_INVOKED;
                                                                                                       IoTivityLog
                                                                                                               .i(PMConstants.TAG,
                                                                                                                       "SelectOTMMethodListener OUT");
                                                                                                   }
                                                                                               };

    public static OcSecureResource.ProvisionCredentialsListener  provisionCredentialsListener  = new OcSecureResource.ProvisionCredentialsListener() {
                                                                                                   @Override
                                                                                                   public void provisionCredentialsListener(
                                                                                                           List<ProvisionResult> provisionResults,
                                                                                                           int hasError) {
                                                                                                       showLog(Info,
                                                                                                               "ProvisionCredentialsListener IN -> Error Code : "
                                                                                                                       + hasError);

                                                                                                       if (hasError == PMConstants.ERROR_CODE) {
                                                                                                           showLog(Error,
                                                                                                                   "Provision Credentials failed !!");
                                                                                                       } else {
                                                                                                           showLog(Info,
                                                                                                                   "Provision Credentials Done !!");
                                                                                                       }

                                                                                                       mIsCbInvoked = PMConstants.CALLBACK_INVOKED;
                                                                                                       showLog(Info,
                                                                                                               "ProvisionCredentialsListener OUT");
                                                                                                   }
                                                                                               };

    public static OcSecureResource.UnlinkDevicesListener         unlinkDevicesListener         = new OcSecureResource.UnlinkDevicesListener() {
                                                                                                   @Override
                                                                                                   public void unlinkDevicesListener(
                                                                                                           List<ProvisionResult> provisionResults,
                                                                                                           int hasError) {
                                                                                                       showLog(Info,
                                                                                                               "unlinkDevicesListener IN -> Error Code : "
                                                                                                                       + hasError);

                                                                                                       if (hasError == PMConstants.ERROR_CODE) {
                                                                                                           showLog(Error,
                                                                                                                   "Unlinking Device failed !!");
                                                                                                       } else {
                                                                                                           showLog(Info,
                                                                                                                   "Unlinking Device Done !!");
                                                                                                       }

                                                                                                       mIsCbInvoked = PMConstants.CALLBACK_INVOKED;
                                                                                                       showLog(Info,
                                                                                                               "unlinkDevicesListener OUT");
                                                                                                   }
                                                                                               };

    public static OcSecureResource.RemoveDeviceListener          removeDeviceListener          = new OcSecureResource.RemoveDeviceListener() {
                                                                                                   @Override
                                                                                                   public void removeDeviceListener(
                                                                                                           List<ProvisionResult> provisionResults,
                                                                                                           int hasError) {
                                                                                                       showLog(Info,
                                                                                                               "removeDeviceListener IN -> Error Code : "
                                                                                                                       + hasError);

                                                                                                       if (hasError == PMConstants.ERROR_CODE) {
                                                                                                           IoTivityLog
                                                                                                                   .e(PMConstants.TAG,
                                                                                                                           "Removing Device failed !!");
                                                                                                       } else {
                                                                                                           showLog(Info,
                                                                                                                   "Removing Device Done !!");
                                                                                                       }

                                                                                                       mIsCbInvoked = PMConstants.CALLBACK_INVOKED;
                                                                                                       showLog(Info,
                                                                                                               "removeDeviceListener OUT");
                                                                                                   }
                                                                                               };

    // Interface Methods
    @Override
    public void provisionPairwiseDevicesListener(
            List<ProvisionResult> ProvisionResultList, int hasError) {
        showLog(Info, "provisionPairwiseDevicesListener IN -> Error Code : "
                + hasError);

        for (int i = 0; i < ProvisionResultList.size(); i++) {
            ProvisionResult pResult = ProvisionResultList.get(i);
            showLog(Info, "provisionPairwiseDevices Result for "
                    + pResult.getDevId() + "is " + pResult.getResult());
        }

        mIsCbInvoked = PMConstants.CALLBACK_INVOKED;
        showLog(Info, "provisionPairwiseDevicesListener OUT");
    }

    @Override
    synchronized public void doMultipleOwnershipTransferListener(
            List<ProvisionResult> ProvisionResultList, int hasError) {
        showLog(Info, "doMultipleOwnershipTransferListener IN -> Error Code : "
                + hasError);
        ProvisionResult pResult = ProvisionResultList.get(0);

        if (hasError == PMConstants.ERROR_CODE) {
            showLog(Error,
                    "Ownership Transfer Failed for " + pResult.getDevId());
        } else {
            showLog(Info,
                    "Ownership Transfer Successful for " + pResult.getDevId());
        }

        mIsCbInvoked = PMConstants.CALLBACK_INVOKED;
        showLog(Info, "doMultipleOwnershipTransferListener OUT");
    }

    @Override
    synchronized public void doOwnershipTransferListener(
            List<ProvisionResult> ProvisionResultList, int hasError) {
        showLog(Info,
                "doOwnershipTransferListener IN -> Error Code : " + hasError);
        ProvisionResult pResult = ProvisionResultList.get(0);

        if (hasError == PMConstants.ERROR_CODE) {
            showLog(Error,
                    "Ownership Transfer Failed for " + pResult.getDevId());
        } else {
            showLog(Info,
                    "Ownership Transfer Successful for " + pResult.getDevId());
        }

        mIsCbInvoked = PMConstants.CALLBACK_INVOKED;
        showLog(Info, "doOwnershipTransferListener OUT");
    }

    @Override
    public void provisionDirectPairingListener(
            List<ProvisionResult> ProvisionResultList, int hasError) {
        showLog(Info, "provisionDirectPairingListener IN -> Error Code : "
                + hasError);
        ProvisionResult pResult = ProvisionResultList.get(0);

        if (hasError == PMConstants.ERROR_CODE) {
            showLog(Error,
                    "Provision DirectPairing Failed for " + pResult.getDevId());
        } else {
            showLog(Info, "Provision DirectPairing  Successful for "
                    + pResult.getDevId());
        }

        mIsCbInvoked = PMConstants.CALLBACK_INVOKED;
        showLog(Info, "provisionDirectPairingListener OUT");
    }

    public static void delay(int second) {
        try {
            Thread.sleep(second * 1000);
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
}
