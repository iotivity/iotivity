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
     *
     *  @throws OcException If the connection manager is not initalized.
     *                      Check that startManagerService was called.
     */
    public synchronized static void setAutoConnectionDevice(String address)
            throws OcException {
        CaInterface.initCheckForConnectionManager();
        CaInterface.caManagerSetAutoConnectionDeviceInfo(address);
    }

    /**
     *  Method unset device information for Auto-Connection.
     *  @param address                      LE address of scanned bluetooth device.
     *
     *  @throws OcException  If the connection manager is not initalized.
     *                       Check that startManagerService was called.
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
     *  @param context    application context
     *  @param listener   listener callback called when the bluetooth device is found
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
     *
     *  @throws OcException If bluetooth pairing has not been initialized.
     *                      Check that startBtPairingService has been called.
     */
    public synchronized static void startScan()
            throws OcException {
        CaInterface.initCheckForBtPairingUtil();
        CaInterface.caBtPairingStartScan();
    }

    /**
     *  stop bluetooth device scan.
     *
     *  @throws OcException If bluetooth pairing has not been initialized.
     *                      Check that startBtPairingService has been called.
     */
    public synchronized static void stopScan()
            throws OcException {
        CaInterface.initCheckForBtPairingUtil();
        CaInterface.caBtPairingStopScan();
    }

    /**
     *  create bond
     *
     *  @param device  bluetooth device
     *  @throws OcException If bluetooth pairing has not been initialized.
     *                      Check that startBtPairingService has been called.
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

    /**
     *  set BLE scan interval time and working count.
     *  scanning logic ("start scan" to "stop scan") will be worked repeatly for workingCount.
     *  and if you choose '0' value for workingCount parameter,
     *  scanning will be worked continually as interval time.
     *  @param intervalTime                  interval time(Seconds).
     *  @param workingCount                  working count with interval time.
     */
    public synchronized static void setLeScanIntervalTime(int intervalTime, int workingCount){
        CaInterface.setLeScanIntervalTimeImpl(intervalTime, workingCount);
    }
    private static native void setLeScanIntervalTimeImpl(int intervalTime, int workingCount);

    /**
     *  stop BLE scan.
     *  if you want to start scan, it can be triggered by setLeScanIntervalTime or
     *  other request API like findResource.
     */
    public synchronized static void stopLeScan(){
        CaInterface.stopLeScanImpl();
    }
    private static native void stopLeScanImpl();

    /**
     *  start BLE Advertising.
     *  precondition : since android gatt server in IoTivity doesn't start to advertise by default.
     *                 (it can prevent battery consumption issue)
     *                 ble adv flag should be set to enable flag, if you want to use this method.
     *                 we provide BT configure setter API which has ble adv flag like below method
     *                 'void setBTConfigure(...)'
     *                 it should be set before call init ocstack like PlatformConfig setter.
     *
     *  1. gatt server stop advertisement after connect with someone by default.
     *     thur, you should call this method, if you want to connect others continuously.
     *
     *  2. if you want to start advertisement in initialization step,
     *     you can call just setBTConfigure API.
     *     since advertisement will be triggered by the flag, when gatt server is started.
     *     to conclude, you don't need to call startLeAdvertising API,
     *     if adv enable flag is set in setBTConfigure API.
     */
    public synchronized static void startLeAdvertising(){
        CaInterface.startLeAdvertisingImpl();
    }
    private static native void startLeAdvertisingImpl();

    /**
     *  stop BLE Advertising.
     *  it can stop to advertise le for gatt server even though there is no connected endpoint.
     *  it will help to reduce battery consumption,
     *  when someone doesn't want to advertise by user scenario.
     *
     *  if you want to stop advertiment in initialize step.
     *  you can call just setBTConfigure API with adv disable flag.
     */
    public synchronized static void stopLeAdvertising(){
        CaInterface.stopLeAdvertisingImpl();
    }
    private static native void stopLeAdvertisingImpl();

    /**
     *  set BT configure
     *  flags for BLE transport
     *  {@code
     *    0:        default BT flag
     *    0x1:      disable BLE advertisement.
     *    0x2:      enable BLE advertisement.
     *    (1 << 4): disable gatt server.
     *    (1 << 7): disable EDR transport.
     *  }
     *  @param flag use value indicated above
     */
    public synchronized static void setBTConfigure(int flag){
        CaInterface.setBTConfigureImpl(flag);
    }
    private static native void setBTConfigureImpl(int flag);

    public synchronized static int setCipherSuite(OicCipher cipher, OcConnectivityType connType){
        return CaInterface.setCipherSuiteImpl(cipher.getValue(), connType.getValue());
    }
    private static native int setCipherSuiteImpl(int cipher, int adapter);

    /**
     * Set Connection Manager configuration.
     * It can be set a connection manager configuration.(default: CA_CLOUD)
     * @param connPriority User Preference of connectivity channel for connection manager
     */
    public synchronized static void setConnectionUserConfig(CaIpConnectionPreference connPriority) {
        CaInterface.caManagerSetConnectionUserConfig(connPriority.getValue());
    }

    private static native void caManagerSetConnectionUserConfig(int connPriority);
}
