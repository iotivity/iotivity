
package com.iotivity.jar;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattServerCallback;
import android.bluetooth.BluetoothGattService;

public class CALeInterface {
    
    public CALeInterface() {
        CARegisterLeScanCallback(mLeScanCallback);
        CARegisterLeGattCallback(mGattCallback);
//        CARegisterLeGattServerCallback(mGattServerCallback);
//        CARegisterBluetoothLeAdvertiseCallback(mAdvertiseCallback);
//        CARegisterBluetoothLeScanCallback(mScanCallback);
    }
    
    public static void getLeScanCallback() {
        CARegisterLeScanCallback(mLeScanCallback);
    }
    
    public static void getLeGattCallback() {
        CARegisterLeGattCallback(mGattCallback);
    }
    
//    public static void getLeGattServerCallback() {
//        CARegisterLeGattServerCallback(mGattServerCallback);
//    }
    
//    public static void getBluetoothLeAdvertiseCallback() {
//        CARegisterBluetoothLeAdvertiseCallback(mAdvertiseCallback);
//    }
//    
//    public static void getBluetoothLeScanCallback() {
//        CARegisterBluetoothLeScanCallback(mScanCallback);
//    }

    public native static void CARegisterLeScanCallback(BluetoothAdapter.LeScanCallback callback);
    public native static void CARegisterLeGattCallback(BluetoothGattCallback callback);
    public native static void CARegisterLeGattServerCallback(BluetoothGattServerCallback callback);
//    public native static void CARegisterBluetoothLeAdvertiseCallback(AdvertiseCallback callback);
//    public native static void CARegisterBluetoothLeScanCallback(ScanCallback callback);
    
    // BluetoothAdapter.LeScanCallback
    public native static void CALeScanCallback(BluetoothDevice device, int rssi, byte[] scanRecord);
    
    // BluetoothGattCallback
    public native static void CALeGattConnectionStateChangeCallback(BluetoothGatt gatt, int status, int newState);
    
    public native static void CALeGattServicesDiscoveredCallback(BluetoothGatt gatt, int status);
    
    public native static void CALeGattCharacteristicReadCallback(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, String data, int status);
    
    public native static void CALeGattCharacteristicWriteCallback(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, String data, int status);
    
    public native static void CALeGattCharacteristicChangedCallback(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic);
    
    public native static void CALeGattDescriptorReadCallback(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status);
    
    public native static void CALeGattDescriptorWriteCallback(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status);
    
    public native static void CALeGattReliableWriteCompletedCallback(BluetoothGatt gatt, int status);
    
    public native static void CALeGattReadRemoteRssiCallback(BluetoothGatt gatt, int rssi, int status);
    
    // BluetoothGattServerCallback
    public native static void CALeGattServerConnectionStateChangeCallback(BluetoothDevice device, int status, int newState);
    
    public native static void CALeGattServerServiceAddedCallback(int status, BluetoothGattService service);
    
    public native static void CALeGattServerCharacteristicReadRequestCallback(BluetoothDevice device,
            int requestId, int offset, BluetoothGattCharacteristic characteristic);
    
    public native static void CALeGattServerCharacteristicWriteRequestCallback(BluetoothDevice device, int requestId,
            BluetoothGattCharacteristic characteristic, boolean preparedWrite, boolean responseNeeded, int offset, byte[] value);
    
    public native static void CALeGattServerDescriptorReadRequestCallback(BluetoothDevice device,
            int requestId, int offset, BluetoothGattDescriptor descriptor);
    
    public native static void CALeGattServerDescriptorWriteRequestCallback(BluetoothDevice device, int requestId,
            BluetoothGattDescriptor descriptor, boolean preparedWrite, boolean responseNeeded, int offset, byte[] value);
    
    public native static void CALeGattServerExecuteWriteCallback(BluetoothDevice device, int requestId, boolean execute);
    
    public native static void CALeGattServerNotificationSentCallback(BluetoothDevice device, int status);

    // AdvertiseCallback
//    public native static void CALeAdvertiseStartSuccessCallback(AdvertiseSettings settingsInEffect);
//    
//    public native static void CALeAdvertiseStartFailureCallback(int errorCode);
    
    // ScanCallback
//    public native static void CABluetoothLeScanResultCallback(int callbackType, ScanResult result);
//    
//    public native static void CABluetoothLeBatchScanResultsCallback(List<ScanResult> results);
//    
//    public native static void CABluetoothLeScanFailedCallback(int errorCode);
    
     
    private static BluetoothAdapter.LeScanCallback mLeScanCallback = new BluetoothAdapter.LeScanCallback() {
        
        @Override
        public void onLeScan(BluetoothDevice device, int rssi, byte[] scanRecord) {
        	
            try {
                CALeScanCallback(device, rssi, scanRecord);	
            } catch(UnsatisfiedLinkError e) {
            
            }
        }
    };

    private static final BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {

        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            super.onConnectionStateChange(gatt, status, newState);
            
            CALeGattConnectionStateChangeCallback(gatt, status, newState);
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            super.onServicesDiscovered(gatt, status);
            
            CALeGattServicesDiscoveredCallback(gatt, status);
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt,
                BluetoothGattCharacteristic characteristic, int status) {
            super.onCharacteristicRead(gatt, characteristic, status);
            
            String data = new String(characteristic.getValue());
            CALeGattCharacteristicReadCallback(gatt, characteristic, data, status);
        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt,
                BluetoothGattCharacteristic characteristic, int status) {
            super.onCharacteristicWrite(gatt, characteristic, status);
            
            String data = new String(characteristic.getValue());
            CALeGattCharacteristicWriteCallback(gatt, characteristic, data, status);
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt,
                BluetoothGattCharacteristic characteristic) {
            super.onCharacteristicChanged(gatt, characteristic);
            
            String data = new String(characteristic.getValue());
            CALeGattCharacteristicChangedCallback(gatt, characteristic, data);
        }

        @Override
        public void onDescriptorRead(BluetoothGatt gatt, BluetoothGattDescriptor descriptor,
                int status) {
            super.onDescriptorRead(gatt, descriptor, status);
            
            CALeGattDescriptorReadCallback(gatt, descriptor, status);
        }

        @Override
        public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor,
                int status) {
            super.onDescriptorWrite(gatt, descriptor, status);
            
            CALeGattDescriptorWriteCallback(gatt, descriptor, status);
        }

        @Override
        public void onReliableWriteCompleted(BluetoothGatt gatt, int status) {
            super.onReliableWriteCompleted(gatt, status);
            
            CALeGattReliableWriteCompletedCallback(gatt, status);
        }

        @Override
        public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status) {
            super.onReadRemoteRssi(gatt, rssi, status);
            
            CALeGattReadRemoteRssiCallback(gatt, rssi, status);
        }
    };
    
    /*
    private static final BluetoothGattServerCallback mGattServerCallback = new BluetoothGattServerCallback() {

        @Override
        public void onConnectionStateChange(BluetoothDevice device, int status,
                int newState) {
            super.onConnectionStateChange(device, status, newState);
            
            CALeGattServerConnectionStateChangeCallback(device, status, newState);
        }

        @Override
        public void onServiceAdded(int status, BluetoothGattService service) {
            super.onServiceAdded(status, service);
            
            CALeGattServerServiceAddedCallback(status, service);
        }

        @Override
        public void onCharacteristicReadRequest(BluetoothDevice device,
                int requestId, int offset,
                BluetoothGattCharacteristic characteristic) {
            super.onCharacteristicReadRequest(device, requestId, offset, characteristic);
            
            CALeGattServerCharacteristicReadRequestCallback(device, requestId, offset, characteristic);
        }

        @Override
        public void onCharacteristicWriteRequest(BluetoothDevice device,
                int requestId, BluetoothGattCharacteristic characteristic,
                boolean preparedWrite, boolean responseNeeded, int offset,
                byte[] value) {
            super.onCharacteristicWriteRequest(device, requestId, characteristic,
                    preparedWrite, responseNeeded, offset, value);
            
            CALeGattServerCharacteristicWriteRequestCallback(device, requestId, characteristic,
                    preparedWrite, responseNeeded, offset, value);
        }

        @Override
        public void onDescriptorReadRequest(BluetoothDevice device,
                int requestId, int offset, BluetoothGattDescriptor descriptor) {
            super.onDescriptorReadRequest(device, requestId, offset, descriptor);
            
            CALeGattServerDescriptorReadRequestCallback(device, requestId, offset, descriptor);
        }

        @Override
        public void onDescriptorWriteRequest(BluetoothDevice device,
                int requestId, BluetoothGattDescriptor descriptor,
                boolean preparedWrite, boolean responseNeeded, int offset,
                byte[] value) {
            super.onDescriptorWriteRequest(device, requestId, descriptor, preparedWrite,
                    responseNeeded, offset, value);
            
            CALeGattServerDescriptorWriteRequestCallback(device, requestId, descriptor, preparedWrite,
                    responseNeeded, offset, value);
        }

        @Override
        public void onExecuteWrite(BluetoothDevice device, int requestId,
                boolean execute) {
            super.onExecuteWrite(device, requestId, execute);
            
            CALeGattServerExecuteWriteCallback(device, requestId, execute);
        }

        @Override
        public void onNotificationSent(BluetoothDevice device, int status) {
            super.onNotificationSent(device, status);
            
            CALeGattServerNotificationSentCallback(device, status);
        }
    };
    
    private static final AdvertiseCallback mAdvertiseCallback = new AdvertiseCallback() {

        @Override
        public void onStartSuccess(AdvertiseSettings settingsInEffect) {
            super.onStartSuccess(settingsInEffect);
            
            CALeAdvertiseStartSuccessCallback(settingsInEffect);
        }

        @Override
        public void onStartFailure(int errorCode) {
            super.onStartFailure(errorCode);
            
            CALeAdvertiseStartFailureCallback(errorCode);
        }
    };
    
    private static final ScanCallback mScanCallback = new ScanCallback() {

        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            super.onScanResult(callbackType, result);
            
            CABluetoothLeScanResultCallback(callbackType, result);
        }

        @Override
        public void onBatchScanResults(List<ScanResult> results) {
            super.onBatchScanResults(results);
            
            CABluetoothLeBatchScanResultsCallback(results);
        }

        @Override
        public void onScanFailed(int errorCode) {
            super.onScanFailed(errorCode);
            
            CABluetoothLeScanFailedCallback(errorCode);
        }
    };
    */
}
