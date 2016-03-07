/*
 * //******************************************************************
 * //
 * // Copyright 2015 Intel Corporation.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.ca;

import android.content.Context;
import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import org.iotivity.base.OcException;
import org.iotivity.base.OcConnectivityType;

public class CaInterface {
    static {
        System.loadLibrary("connectivity_abstraction");
        System.loadLibrary("ca-interface");
    }
    private static volatile boolean isConnectionManagerInitialized = false;
    private static volatile boolean isBtPairingInitialized = false;

    public static native void initialize(Activity activity, Context context);

    /**
     *  Method start connection manager service.
     *  this method has to be called before other API call.
     *  @param context                                application context
     *  @param onConnectionManagerStateListener       connection state callback listener
     */
    public synchronized static void startManagerService(Context context,
            OnConnectionManagerStateListener onConnectionManagerStateListener) {
        if (!isConnectionManagerInitialized) {
            CaInterface.caManagerInitialize(context, onConnectionManagerStateListener);
            isConnectionManagerInitialized = true;
        }
    }

    /**
     *  Method stop connection manager service.
     *  this method must be called, when Application is destroied.
     */
    public synchronized static void stopManagerService() {
        if (isConnectionManagerInitialized) {
            CaInterface.caManagerTerminate();
            isConnectionManagerInitialized = false;
        }
    }

    /**
     *  Method set device information for Auto-Connection.
     *  this method has to be called before FindResource is called.
     *  @param address                      LE address of scanned bluetooth device.
     */
    public synchronized static void setAutoConnectionDevice(String address)
            throws OcException {
        CaInterface.initCheckForConnectionManager();
        CaInterface.caManagerSetAutoConnectionDeviceInfo(address);
    }

    /**
     *  Method unset device information for Auto-Connection.
     *  @param address                      LE address of scanned bluetooth device.
     */
    public synchronized static void unsetAutoConnectionDevice(String address)
            throws OcException {
        CaInterface.initCheckForConnectionManager();
        CaInterface.caManagerUnsetAutoConnectionDeviceInfo(address);
    }

    /**
     *  Interface for connection manager state listener.
     *  Event listeners are notified asynchronously.
     */
    public interface OnConnectionManagerStateListener {
        public void onAdapterStateChanged(OcConnectivityType type, boolean enabled);
        public void onConnectionStateChanged(OcConnectivityType type, String address,
                boolean connected);
    }

    private static void initCheckForConnectionManager() {
        if (!isConnectionManagerInitialized) {
            throw new IllegalStateException("ConnectionManager must be started by making "
                    + "a call to CaInterface.startManagerService before any other API "
                    + "calls are permitted");
        }
    }

    private static native void caManagerInitialize(Context context,
            OnConnectionManagerStateListener onConnectionManagerStateListener);
    private static native void caManagerTerminate();
    private static native void caManagerSetAutoConnectionDeviceInfo(String address);
    private static native void caManagerUnsetAutoConnectionDeviceInfo(String address);

    /**
     *  start bluetooth pairing service.
     *  @param context                      application context
     */
    public synchronized static void startBtPairingService(Context context,
            OnBtDeviceFoundListener listener) {
        if (!isBtPairingInitialized) {
            CaInterface.caBtPairingInitialize(context, listener);

            isBtPairingInitialized = true;
        }
    }

    /**
     *  stop bluetooth pairing service.
     */
    public synchronized static void stopBtPairingService() {
        if (isBtPairingInitialized) {
            CaInterface.caBtPairingTerminate();

            isBtPairingInitialized = false;
        }
    }

    /**
     *  start bluetooth device scan.
     */
    public synchronized static void startScan()
            throws OcException {
        CaInterface.initCheckForBtPairingUtil();
        CaInterface.caBtPairingStartScan();
    }

    /**
     *  stop bluetooth device scan.
     */
    public synchronized static void stopScan()
            throws OcException {
        CaInterface.initCheckForBtPairingUtil();
        CaInterface.caBtPairingStopScan();
    }

    /**
     *  create bond
     */
    public synchronized static void createBond(BluetoothDevice device)
            throws OcException {
        CaInterface.initCheckForBtPairingUtil();
        CaInterface.caBtPairingCreateBond(device);
    }

    public interface OnBtDeviceFoundListener {
        public void onBtDeviceFound(BluetoothDevice device) throws OcException;
    }

    private static void initCheckForBtPairingUtil() {
        if (!isBtPairingInitialized) {
            throw new IllegalStateException("BT pairing Util must be started by making "
                    + "a call to CaInterface.startBtPairingService before any other API "
                    + "calls are permitted");
        }
    }

    private static native void caBtPairingInitialize(Context context,
            OnBtDeviceFoundListener listener);
    private static native void caBtPairingTerminate();
    private static native void caBtPairingStartScan();
    private static native void caBtPairingStopScan();
    private static native void caBtPairingCreateBond(BluetoothDevice device);
}