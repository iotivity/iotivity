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
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

package org.iotivity.ca;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;

public class CaBtPairingInterface {
    private static Context mContext;
    private static volatile boolean isBtInitialized = false;

    private CaBtPairingInterface(Context context) {
        synchronized(CaBtPairingInterface.class) {
            mContext = context;
        }
        if (!isBtInitialized) {
            registerIntentFilter();
            isBtInitialized = true;
        }
    }

    private static IntentFilter registerIntentFilter() {
        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        filter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        filter.addAction(BluetoothDevice.ACTION_FOUND);
        mContext.registerReceiver(mReceiver, filter);
        return filter;
    }

    public static void destroyEdrInterface() {
        if (isBtInitialized) {
            mContext.unregisterReceiver(mReceiver);
            isBtInitialized = false;
        }
    }

    private native static void oicEdrStateChangedCallback(int state);

    private native static void oicEdrBondStateChangedCallback(String addr);

    private native static void oicEdrFoundDeviceCallback(BluetoothDevice device);

    private static final BroadcastReceiver mReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {

            String action = intent.getAction();

            if (action != null && action.equals(BluetoothDevice.ACTION_FOUND)) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

                // if found device is not paired with this device
                if (device.getBondState() != BluetoothDevice.BOND_BONDED)
                {
                    oicEdrFoundDeviceCallback(device);
                }
            }

            if (action != null && action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {

                int state =
                        intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);

                // STATE_ON:12, STATE_OFF:10
                if (state == BluetoothAdapter.STATE_ON || state == BluetoothAdapter.STATE_OFF)
                {
                    oicEdrStateChangedCallback(state);
                }
            }

            if (action != null && action.equals(BluetoothDevice.ACTION_BOND_STATE_CHANGED)) {

                int bondState = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE,
                                                   BluetoothDevice.ERROR);

                // unpairing event
                if (bondState == BluetoothDevice.BOND_NONE) {
                    if ((intent.getIntExtra(BluetoothDevice.EXTRA_PREVIOUS_BOND_STATE,
                                            BluetoothDevice.ERROR)
                                                == BluetoothDevice.BOND_BONDED)) {
                        BluetoothDevice device
                            = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

                        oicEdrBondStateChangedCallback(device.getAddress());
                    }
                }
            }
        }
    };
}
