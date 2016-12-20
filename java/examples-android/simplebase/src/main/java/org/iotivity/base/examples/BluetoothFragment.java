/*
 * ******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples;

import android.app.Activity;
import android.app.Fragment;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.ParcelUuid;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcResource;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.ca.CaBtPairingInterface;
import org.iotivity.ca.CaInterface;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;

/**
 * This class is for testing of Bluetooth util.
 */
public class BluetoothFragment extends Fragment implements
        CaInterface.OnBtDeviceFoundListener,
        CaInterface.OnConnectionManagerStateListener {

    private static final String      TAG                  = "OCF_SIMPLE_BLUETOOTH";
    private static final String      CA_GATT_SERVICE_UUID = "ADE3D529-C784-4F63-A987-EB69F70EE816";

    private Activity                   mActivity;
    private Context                    mContext;
    private BluetoothAdapter           mBluetoothAdapter;
    private static final int           REQUEST_ENABLE_BT  = 1;
    private static final long          SCAN_PERIOD        = 10000;
    private BluetoothLeScanner         mLEScanner;
    private ScanSettings               mScanSettings;
    private List<ScanFilter>           mScanFilters;
    private BluetoothGatt              mGatt;
    private ArrayList<String>          mItems;
    private ArrayList<BluetoothDevice> mBluetoothDevices;
    private ArrayAdapter<String>       mAdapters;

    private boolean                    mIsScanning;
    private boolean                    mIsBTSelected;

    private Button                     mBtButton;
    private Button                     mLeButton;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_bluetooth, container, false);
        mBtButton = (Button) rootView.findViewById(R.id.btn_bt);
        mBtButton.setOnClickListener(scanButtonListener(true));

        mLeButton = (Button) rootView.findViewById(R.id.btn_le);
        mLeButton.setOnClickListener(scanButtonListener(false));

        ListView listView = (ListView) rootView.findViewById(R.id.list_view);
        mItems = new ArrayList<String>();
        mAdapters = new ArrayAdapter<String>(mActivity, android.R.layout.simple_list_item_1, mItems);
        listView.setAdapter(mAdapters);
        listView.setOnItemClickListener(itemClickListener);
        listView.setOnItemLongClickListener(itemLongClickListener);

        return rootView;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mActivity = getActivity();
        mContext = mActivity.getBaseContext();

        mBluetoothDevices = new ArrayList<BluetoothDevice>();

        if (!mActivity.getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Common.showToast(mContext, "BLE Not Supported");
        }
        final BluetoothManager bluetoothManager = (BluetoothManager) mActivity
                                                  .getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();

        PlatformConfig cfg = new PlatformConfig(mActivity, mContext,
                                                ServiceType.IN_PROC,
                                                ModeType.CLIENT,
                                                Common.IP_ADDRESS,
                                                Common.IP_PORT,
                                                QualityOfService.LOW);

        OcPlatform.Configure(cfg);
        CaInterface.startBtPairingService(mContext, this);
        CaInterface.startManagerService(mContext, this);
    }

    @Override
    public void onResume() {
        super.onResume();
        if (mBluetoothAdapter == null || !mBluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        } else {
            if (Build.VERSION.SDK_INT >= 21) {
                mLEScanner = mBluetoothAdapter.getBluetoothLeScanner();
                if (mScanSettings == null) {
                    mScanSettings = new ScanSettings.Builder()
                                               .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
                                               .build();
                }

                if (mScanFilters == null) {
                    mScanFilters = new ArrayList<ScanFilter>();
                    ScanFilter scanFilter = new ScanFilter.Builder()
                            .setServiceUuid(ParcelUuid.fromString(CA_GATT_SERVICE_UUID)).build();
                    mScanFilters.add(scanFilter);
                }
            }
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mBluetoothAdapter != null && mBluetoothAdapter.isEnabled()) {
            scanLeDevice(false);
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        CaInterface.stopBtPairingService();
        CaInterface.stopManagerService();
        if (mGatt != null) {
            mGatt.close();
            mGatt = null;
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_ENABLE_BT) {
            if (resultCode == Activity.RESULT_CANCELED) {
                mActivity.finish();
                return;
            }
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    View.OnClickListener scanButtonListener(final boolean isBT) {
        return new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!mIsScanning) {
                    mItems.clear();
                    mBluetoothDevices.clear();
                    mAdapters.notifyDataSetChanged();
                    mIsBTSelected = isBT;
                }

                if (isBT) {
                    if (mIsScanning) {
                        // BT Stop
                        try {
                            CaInterface.stopScan();
                        } catch (OcException e) {
                            e.printStackTrace();
                        }
                        mBtButton.setText(R.string.bt_scan);
                        mLeButton.setVisibility(View.VISIBLE);
                    } else {
                        // BT Scan
                        try {
                            CaInterface.startScan();
                        } catch (OcException e) {
                            e.printStackTrace();
                        }
                        mBtButton.setText(R.string.stop_scan);
                        mLeButton.setVisibility(View.GONE);
                    }
                } else {
                    if (mIsScanning) {
                        // LE Stop
                        mLeButton.setText(R.string.le_scan);
                        mBtButton.setVisibility(View.VISIBLE);
                    } else {
                        // LE Scan
                        mLeButton.setText(R.string.stop_scan);
                        mBtButton.setVisibility(View.GONE);
                    }
                    scanLeDevice(!mIsScanning);
                }
                mIsScanning = !mIsScanning;
            }
        };
    }

    @Override
    public synchronized void onBtDeviceFound(BluetoothDevice device) throws OcException {
        Log.i(TAG, "onBtDeviceFound address : " + device.getAddress());

        addItemToList(device);
    }

    private AdapterView.OnItemClickListener itemClickListener = new AdapterView
                                                                .OnItemClickListener() {
        public void onItemClick(AdapterView<?> adapterView, View clickedView, int pos, long id) {
            StringBuilder sb = new StringBuilder();

            if (mIsBTSelected) {
                sb.append("Pairing with : ");
                try {
                    CaInterface.createBond(mBluetoothDevices.get(pos));
                } catch (OcException e) {
                    e.printStackTrace();
                }
            } else {
                sb.append("Set Connect with : ");
                try {
                    final String address = mBluetoothDevices.get(pos).getAddress();
                    Common.setLeAddress(address);
                    CaInterface.setAutoConnectionDevice(address);
                    OcPlatform.OnResourceFoundListener resourceFoundListener =
                            new OcPlatform.OnResourceFoundListener() {
                        @Override
                        public void onResourceFound(OcResource ocResource) {
                            Log.i(TAG, "onResourceFound : " + ocResource.getUri());
                        }

                        @Override
                        public void onFindResourceFailed(Throwable throwable, String uri) {
                           Log.i(TAG, "findResource request has failed");
                           Log.e(TAG, throwable.toString());
                        }
                    };
                    OcPlatform.findResource("", address + OcPlatform.WELL_KNOWN_QUERY,
                                            EnumSet.of(OcConnectivityType.CT_ADAPTER_GATT_BTLE),
                                            resourceFoundListener, QualityOfService.LOW);
                } catch (OcException e) {
                    e.printStackTrace();
                }
            }
            Common.showToast(mContext, sb.append(mItems.get(pos)).toString());
        }
    };

    private AdapterView.OnItemLongClickListener itemLongClickListener = new AdapterView
                                                                        .OnItemLongClickListener() {
        @Override
        public boolean onItemLongClick(AdapterView<?> adapterView, View view, int pos, long id) {
            StringBuilder sb = new StringBuilder();

            if (!mIsBTSelected) {
                sb.append("Unset Connect with : ");
                try {
                    CaInterface.unsetAutoConnectionDevice(mBluetoothDevices.get(pos).getAddress());
                } catch (OcException e) {
                    e.printStackTrace();
                }
            }
            Common.showToast(mContext, sb.append(mItems.get(pos)).toString());

            return true;
        }
    };

    private void scanLeDevice(final boolean enable) {
        if (enable) {
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    mLEScanner.stopScan(mScanCallback);
                }
            }, SCAN_PERIOD);

            mLEScanner.startScan(mScanFilters, mScanSettings, mScanCallback);
        } else {
            mLEScanner.stopScan(mScanCallback);
        }
    }

    private ScanCallback mScanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            Log.i(TAG, "onScanResult : " + result.toString());

            addItemToList(result.getDevice());
        }

        @Override
        public void onScanFailed(int errorCode) {
            Log.e(TAG, "Scan Failed, Error Code : " + errorCode);
        }
    };

    private void addItemToList(BluetoothDevice btDevice) {
        StringBuilder sb = new StringBuilder(btDevice.toString());
        sb.append(" (");
        if (btDevice.getName() != null) {
            sb.append(btDevice.getName().toString());
        } else {
            sb.append("NULL");
        }
        sb.append(")");

        if (mItems.contains(sb.toString())) {
            // Duplicate
        } else {
            // add item
            mItems.add(sb.toString());
            mBluetoothDevices.add(btDevice);
            mAdapters.notifyDataSetChanged();
        }
    }

    @Override
    public void onAdapterStateChanged(OcConnectivityType type, boolean enabled) {
        final String msg = getString(R.string.action_onadapterstatechanged) + enabled;
        Log.i(TAG, msg);
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Common.showToast(mContext, msg);
            }
        });
    }

    @Override
    public void onConnectionStateChanged(OcConnectivityType type,
                                         String address, boolean connected) {
        Log.i(TAG, "onConnectionStateChanged address: " + address);
        final String msg = getString(R.string.action_onconnectionstatechanged) + connected;
        Log.i(TAG, msg);
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Common.showToast(mContext, msg);
            }
        });
    }
}
