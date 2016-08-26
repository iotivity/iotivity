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

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;

import org.iotivity.base.OcDirectPairDevice;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPlatform.DirectPairingListener;
import org.iotivity.base.OcPlatform.FindDirectPairingListener;
import org.iotivity.base.OcPrmType;
import org.iotivity.base.OcProvisioning;
import org.iotivity.base.OcSecureResource;

import android.content.Context;
import android.util.Log;

import org.iotivity.service.testapp.framework.Base;
import org.iotivity.service.pm.PMConstants;

public class OCPlatformHelper {
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

    private static void showLog(int logType, String message) {

        Base.showOutPut(message);
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
            Log.e(PMConstants.TAG, "Cbor svr db file not found " + e.getMessage());
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

    public static OcPlatform.FindDirectPairingListener finddirectPairing = new OcPlatform.FindDirectPairingListener() {
        @Override
        public void onFindDirectPairingListener(
                List<OcDirectPairDevice> direcPairingDevice) {
            showLog(
                    Info,
                    "find direct pairable successfull ");
            discoveredListObj
                    .clear();
            discoveredPairableDevices = new ArrayList<OcDirectPairDevice>(
                    direcPairingDevice);
            for (int i = 0; i < direcPairingDevice
                    .size(); i++) {
                OcDirectPairDevice dpPairDevice = direcPairingDevice
                        .get(i);
                showLog(
                        Info,
                        "getHost from Find Direct Pairing "
                                + dpPairDevice
                                .getHost());
                discoveredListObj
                        .add(dpPairDevice);
            }
        }
    };

    /**
     * Method to find all devices which are eligible for direct pairing and return
     * the list.
     *
     * @param timeout                   timeout for discovering direct pair devices.
     * @param FindDirectPairingListener Callback function, which will receive the list of direct pairable
     *                                  devices.
     * @throws OcException
     */
    public boolean findDirectPairingDevices(int timeout,
                                            FindDirectPairingListener onFindDirectPairingListener) {
        try {
            showLog(Info, "Finding DirectPairing Devices");
            OcPlatform.findDirectPairingDevices(timeout, onFindDirectPairingListener);

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

    public static OcPlatform.DirectPairingListener doDPListener = new OcPlatform.DirectPairingListener() {
        @Override
        public void onDirectPairingListener(
                String devId,
                int result) {
            showLog(Info,
                    "Inside onDirectPairingListener");
            if (result == PMConstants.SUCCES_CODE) {
                pairedList
                        .clear();
                pairedList
                        .add(devId);

                showLog(
                        Info,
                        "direct pair successfull for "
                                + devId);
            } else {
                showLog(
                        Error,
                        "direct pairing failed");
            }

        }
    };

    /**
     * Method to perform direct pairing between two devices.
     *
     * @param peer                  Target peer
     * @param prmType               Pairing Method to be used for Pairing
     * @param pin                   pin
     * @param DirectPairingListener Callback function, which will be called after completion of direct
     *                              pairing.
     * @throws OcException
     */
    public boolean doDirectPairing(OcDirectPairDevice peer, OcPrmType prmType,
                                   String pin, DirectPairingListener onDirectPairingListener) {
        try {
            showLog(Info, "DirectPairing In Progress");
            OcPlatform.doDirectPairing(peer, prmType, pin, onDirectPairingListener);

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
