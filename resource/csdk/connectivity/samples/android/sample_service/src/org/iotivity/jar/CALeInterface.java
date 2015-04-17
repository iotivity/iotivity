package org.iotivity.jar;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattServerCallback;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.le.AdvertiseCallback;
import android.bluetooth.le.AdvertiseSettings;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

public class CALeInterface {

    public CALeInterface(Context context) {

        CARegisterLeScanCallback(mLeScanCallback);
        CARegisterLeGattCallback(mGattCallback);
        CARegisterLeGattServerCallback(mGattServerCallback);
        CARegisterBluetoothLeAdvertiseCallback(mAdvertiseCallback);

        registerIntentFilter(context);
    }

    public static void getLeScanCallback() {
        CARegisterLeScanCallback(mLeScanCallback);
    }

    public static void getLeGattCallback() {
        CARegisterLeGattCallback(mGattCallback);
    }

    public static void getLeGattServerCallback() {
        CARegisterLeGattServerCallback(mGattServerCallback);
    }

    public static void getBluetoothLeAdvertiseCallback() {
        CARegisterBluetoothLeAdvertiseCallback(mAdvertiseCallback);
    }

    public static IntentFilter registerIntentFilter(Context context) {
        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        context.registerReceiver(mReceiver, filter);
        return filter;
    }

    public native static void CARegisterLeScanCallback(
            BluetoothAdapter.LeScanCallback callback);

    public native static void CARegisterLeGattCallback(
            BluetoothGattCallback callback);

    public native static void CARegisterLeGattServerCallback(
            BluetoothGattServerCallback callback);

    public native static void CARegisterBluetoothLeAdvertiseCallback(
            AdvertiseCallback callback);

    // BluetoothAdapter.LeScanCallback
    public native static void CALeScanCallback(BluetoothDevice device,
            int rssi, byte[] scanRecord);

    // BluetoothGattCallback
    public native static void CALeGattConnectionStateChangeCallback(
            BluetoothGatt gatt, int status, int newState);

    public native static void CALeGattServicesDiscoveredCallback(
            BluetoothGatt gatt, int status);

    public native static void CALeGattCharacteristicReadCallback(
            BluetoothGatt gatt, BluetoothGattCharacteristic characteristic,
            byte[] data, int status);

    public native static void CALeGattCharacteristicWriteCallback(
            BluetoothGatt gatt, BluetoothGattCharacteristic characteristic,
            byte[] data, int status);

    public native static void CALeGattCharacteristicChangedCallback(
            BluetoothGatt gatt, BluetoothGattCharacteristic characteristic,
            byte[] data);

    public native static void CALeGattDescriptorReadCallback(
            BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status);

    public native static void CALeGattDescriptorWriteCallback(
            BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status);

    public native static void CALeGattReliableWriteCompletedCallback(
            BluetoothGatt gatt, int status);

    public native static void CALeGattReadRemoteRssiCallback(
            BluetoothGatt gatt, int rssi, int status);

    // BluetoothGattServerCallback
    public native static void CALeGattServerConnectionStateChangeCallback(
            BluetoothDevice device, int status, int newState);

    public native static void CALeGattServerServiceAddedCallback(int status,
            BluetoothGattService service);

    public native static void CALeGattServerCharacteristicReadRequestCallback(
            BluetoothDevice device, int requestId, int offset,
            BluetoothGattCharacteristic characteristic, byte[] data);

    public native static void CALeGattServerCharacteristicWriteRequestCallback(
            BluetoothDevice device, int requestId,
            BluetoothGattCharacteristic characteristic, byte[] data,
            boolean preparedWrite, boolean responseNeeded, int offset,
            byte[] value);

    public native static void CALeGattServerDescriptorReadRequestCallback(
            BluetoothDevice device, int requestId, int offset,
            BluetoothGattDescriptor descriptor);

    public native static void CALeGattServerDescriptorWriteRequestCallback(
            BluetoothDevice device, int requestId,
            BluetoothGattDescriptor descriptor, boolean preparedWrite,
            boolean responseNeeded, int offset, byte[] value);

    public native static void CALeGattServerExecuteWriteCallback(
            BluetoothDevice device, int requestId, boolean execute);

    public native static void CALeGattServerNotificationSentCallback(
            BluetoothDevice device, int status);

    // AdvertiseCallback
    public native static void CALeAdvertiseStartSuccessCallback(
            AdvertiseSettings settingsInEffect);

    public native static void CALeAdvertiseStartFailureCallback(int errorCode);

    // Network Monitor
    public native static void CALeStateChangedCallback(int state);

    // Callback
    private static BluetoothAdapter.LeScanCallback mLeScanCallback
        = new BluetoothAdapter.LeScanCallback() {

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

            CALeGattCharacteristicReadCallback(gatt, characteristic,
                    characteristic.getValue(), status);
        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt,
                BluetoothGattCharacteristic characteristic, int status) {
            super.onCharacteristicWrite(gatt, characteristic, status);

            CALeGattCharacteristicWriteCallback(gatt, characteristic,
                    characteristic.getValue(), status);
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt,
                BluetoothGattCharacteristic characteristic) {
            super.onCharacteristicChanged(gatt, characteristic);

            CALeGattCharacteristicChangedCallback(gatt, characteristic,
                    characteristic.getValue());
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

    private static final BluetoothGattServerCallback mGattServerCallback
        = new BluetoothGattServerCallback() {

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

            CALeGattServerCharacteristicReadRequestCallback(device, requestId, offset,
                    characteristic, characteristic.getValue());
        }

        @Override
        public void onCharacteristicWriteRequest(BluetoothDevice device,
                int requestId, BluetoothGattCharacteristic characteristic,
                boolean preparedWrite, boolean responseNeeded, int offset,
                byte[] value) {
            super.onCharacteristicWriteRequest(device, requestId, characteristic,
                    preparedWrite, responseNeeded, offset, value);

            CALeGattServerCharacteristicWriteRequestCallback(device, requestId, characteristic,
                    value, preparedWrite, responseNeeded, offset, value);
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

            CALeGattServerDescriptorWriteRequestCallback(device, requestId, descriptor,
                    preparedWrite, responseNeeded, offset, value);
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

    private static final BroadcastReceiver mReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {

            String action = intent.getAction();

            if (action != null && action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {

                int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,
                        BluetoothAdapter.ERROR);
                // STATE_ON:12, STATE_OFF:10
                if (state == BluetoothAdapter.STATE_ON || state == BluetoothAdapter.STATE_OFF) {
                    CALeStateChangedCallback(state);
                }
            }
        }
    };
}

