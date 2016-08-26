/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/
package org.iotivity.service.pm;

import java.util.EnumSet;
import java.util.List;

import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcDirectPairDevice;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPrmType;
import org.iotivity.base.OcSecureResource;
import org.iotivity.base.OcDirectPairDevice.DirectPairingListener;
import org.iotivity.base.OcDirectPairDevice.GetDirectPairedListener;
import org.iotivity.service.testapp.framework.Base;

import android.util.Log;

public class OcDirectPairDeviceHelper implements DirectPairingListener,
        GetDirectPairedListener {

    public static List<OcSecureResource> mUnownedDeviceList, mOwnedDeviceList;
    public static int mUnownedDeviceCount,
            mOwnedDeviceCount;
    public static boolean mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
    public static String mFilePath = null;
    public static String mSqlDbPath = null;
    public static final int Error = -1;
    public static final int Info = 1;

    public static List<OcDirectPairDevice> discoveredPairableDevices;
    public static List<String> pairedList;
    public static List<OcDirectPairDevice> discoveredListObj;

    @Override
    public void directPairingListener(String devId, int result) {
        showLog(Info, "Inside onDirectPairingListener");
        if (result == PMConstants.SUCCES_CODE) {
            pairedList.clear();
            pairedList.add(devId);

            showLog(Info, "direct pair successfull for " + devId);
        } else {
            showLog(Error, "direct pairing failed");
        }
        mIsCbInvoked = PMConstants.CALLBACK_INVOKED;
    }

    @Override
    public void getDirectPairedListener(List<String> ocPairedDeviceList) {
        showLog(Info, "Inside getDirectPairedListener");
        showLog(Info, "Total Paired Devices : " + ocPairedDeviceList.size());
        for (int i = 0; i < ocPairedDeviceList.size(); i++) {
            showLog(Info, "Paired Device ID " + i + ": " + ocPairedDeviceList.get(i));
        }
        mIsCbInvoked = PMConstants.CALLBACK_INVOKED;
    }

    /**
     * Method to perform direct pairing between two devices.
     *
     * @param DirectPairingListener Callback function, which will be called after completion of direct
     *                              pairing.
     * @throws OcException
     */
    public boolean doDirectPairing(OcDirectPairDevice devInstance,
                                   OcDirectPairDevice peer, OcPrmType prmType, String pin,
                                   DirectPairingListener onDirectPairingListener) {
        try {
            showLog(Info, "DirectPairing In Progress");
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            devInstance.doDirectPairing(peer, prmType, pin, onDirectPairingListener);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                showLog(Error, PMConstants.CallBackNotInvoked);
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

    public boolean getDirectPairedDevices(OcDirectPairDevice devInstance,
                                          GetDirectPairedListener getDirectPairedListener) {
        try {
            showLog(Info, "Getting Direct Paired Devices");
            mIsCbInvoked = PMConstants.CALLBACK_NOT_INVOKED;
            devInstance.getDirectPairedDevices(getDirectPairedListener);

            if (PMConstants.CALLBACK_NOT_INVOKED == waitTillCBInvoke()) {
                showLog(Error, PMConstants.CallBackNotInvoked);
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

    public boolean getPairingMethodList(OcDirectPairDevice devInstance) {
        try {
            showLog(Info, "Getting Connectivity Type Set");

            List<Integer> pairingMethodList = devInstance.getPairingMethodList();

            for (int i = 0; i < pairingMethodList.size(); i++) {
                showLog(Info, (i + 1) + ". Supported Pairing Methods : "
                        + pairingMethodList.get(i));

            }

            return true;

        } catch (Exception e) {
            showLog(Error, e.getLocalizedMessage());
            PMConstants.mErrorMessage = e.getLocalizedMessage();
            return false;
        }
    }

    public boolean getConnectivityTypeSet(OcDirectPairDevice devInstance) {
        try {
            showLog(Info, "Getting Connectivity Type Set");
            EnumSet<OcConnectivityType> connectivityTypeList = devInstance
                    .getConnectivityTypeSet();

            for (OcConnectivityType connectivityType : connectivityTypeList) {
                showLog(Info, "Supported Conncetivity Types : " + connectivityType);
            }

            return true;

        } catch (Exception e) {
            showLog(Error, e.getLocalizedMessage());
            PMConstants.mErrorMessage = e.getLocalizedMessage();
            return false;
        }
    }

    private static void showLog(int logType, String message) {

        Base.showOutPut(message);
        if (logType == 1) {
            Log.i(PMConstants.TAG, message);
        } else {
            Log.e(PMConstants.TAG, message);
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
            Log.i(PMConstants.TAG, "Waiting for = " + count + " second/s");

            if (count > PMConstants.CALLBACK_TIMEOUT) {
                return PMConstants.CALLBACK_NOT_INVOKED;
            }
        }

        return PMConstants.CALLBACK_INVOKED;
    }

}
