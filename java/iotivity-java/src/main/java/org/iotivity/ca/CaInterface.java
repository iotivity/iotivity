/*
 *******************************************************************
 *
 * Copyright 2015 Intel Corporation.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.ca;

import org.iotivity.base.OcException;
import org.iotivity.base.OcConnectivityType;

class BluetoothDevice {}

public class CaInterface {
    static {
    }
    private static volatile boolean isConnectionManagerInitialized = false;
    private static volatile boolean isBtPairingInitialized = false;

    public static void initialize()  {}

    /**
     *  Method start connection manager service.
     *  this method has to be called before other API call.
     *  @param context                                application context
     *  @param onConnectionManagerStateListener       connection state callback listener
     */
    public synchronized static void startManagerService(
            OnConnectionManagerStateListener onConnectionManagerStateListener) {
        if (!isConnectionManagerInitialized) {
            CaInterface.caManagerInitialize(onConnectionManagerStateListener);
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

    private static void caManagerInitialize(
            OnConnectionManagerStateListener onConnectionManagerStateListener) {}
    private static void caManagerTerminate() {}
    private static void caManagerSetAutoConnectionDeviceInfo(String address) {}
    private static void caManagerUnsetAutoConnectionDeviceInfo(String address) {}

    /**
     *  start bluetooth pairing service.
     *  @param context                      application context
     */
    public synchronized static void startBtPairingService(
            OnBtDeviceFoundListener listener) {
        if (!isBtPairingInitialized) {
            /* TODO */
            CaInterface.caBtPairingInitialize(listener);

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

    private static void caBtPairingInitialize(
            OnBtDeviceFoundListener listener) {}
    private static void caBtPairingTerminate() {}
    private static void caBtPairingStartScan() {}
    private static void caBtPairingStopScan() {}
    private static void caBtPairingCreateBond(BluetoothDevice device) {}
}
