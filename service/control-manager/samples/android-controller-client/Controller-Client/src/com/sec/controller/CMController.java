//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package com.sec.controller;

import gen.cm.MyCMDevice;
import gen.devicefinder.CMDeviceFactory;
import gen.serialization.CMSerializationFactory;

import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;
import java.util.UUID;

import OC.Cm.CMErrorCode;
import OC.Cm.CMStates;
import OC.Cm.Configuration;
import OC.Cm.ControlManager;
import OC.Cm.Device;
import OC.Cm.DeviceToken;
import OC.Cm.DeviceType;
import OC.Cm.EasySetupNotification;
import OC.Cm.ICMListener;
import OC.Cm.DeviceFinder.IDeviceFinderListener;
import OC.Cm.Notification.INotificationListener;
import OC.Cm.Serialization.ISerializable;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.widget.Toast;

import com.sec.controller.ui.DeviceListView;

public class CMController {

    static final String   TAG             = "CMController";
    static CMController   instance        = null;
    static boolean        isStarted       = false;
    static public Context mContext        = null;

    ControlManager        mControlManager = null;
    MyCMDevice            mMyDevice       = null;
    OC.Cm.ICMListener     mCMListener     = null;

    private CMController(Context context) {

        mContext = context;
        mControlManager = ControlManager.getInstance(MyCMDevice.APP_ID);
        mMyDevice = MyCMDevice.getInstance(MyCMDevice.APP_ID);
        mCMListener = new CMListener();
    }

    public static CMController getCMControllerInstance(Context context) {

        if (instance == null) {
            instance = new CMController(context);
        }
        return instance;
    }

    public boolean startController() {

        if (isStarted == true) {

            Log.i(TAG, "Controller is already started");
            return true;
        }

        try {

            ControlManager.configCurrentClassLoader(mContext.getClassLoader());

            String ipAddress = getLocalIpAddress();
            String port = "9999";

            Configuration config = mControlManager.getConfiguration();

            mMyDevice.setUUID(UUID.randomUUID().toString());
            mMyDevice.setDeviceType(DeviceType.DeviceType_Gateway.ordinal());

            mMyDevice.setSupportedResourceType("Devices");
            mMyDevice.setSupportedResourceType("Device");
            mMyDevice.setSupportedResourceType("Capability");
            mMyDevice.setSupportedResourceType("Information");
            mMyDevice.setAddress(ipAddress + ":" + port);

            config.setDeviceFactory(new CMDeviceFactory());
            config.setSerializableDataFactory(new CMSerializationFactory());
            config.setResourceHandlerFactory(mMyDevice
                    .getResourceHandlerFactory());
            String dbPath = mContext.getFilesDir().getAbsolutePath()
                    + "/subscriptions.db";

            config.setSubscriptionDbPath(dbPath);
            mControlManager.setConfiguration(config);
            mControlManager.setCMListener(mCMListener);
            mControlManager.getDeviceFinder().setDeviceFinderListener(
                    new CMDeviceFinderListern());
            mControlManager
                    .addNotificationListener(new CMNotificationListener());

            mControlManager.start(mMyDevice);

            OC.Cm.ApplicationType apptype = config.getAppType();
            Log.i(TAG, "Application type = " + apptype);

        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            return false;
        }

        return true;
    }

    public boolean stopController() {

        try {
            if (isStarted == true) {

                mControlManager.removeCMListener(mCMListener);
                mCMListener = null;

                mControlManager.removeNotificationListener();

                Configuration config = mControlManager.getConfiguration();
                if (null != config) {
                    config.reset();
                }

                MyCMDevice.destroyInstance(MyCMDevice.APP_ID);

                mControlManager.stop();

                CMStates state = mControlManager.getCMState();
                if (CMStates.CM_STOPPED == state) {
                    Log.e("TAG", "Control Manager Stopped Completely");
                }
                if (CMStates.CM_STOPPING == state) {
                    Log.e("TAG", "Control Manager is stopping");
                }
                isStarted = false;

            }

        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            return false;
        }
        return true;
    }

    private String getLocalIpAddress() {
        try {
            for (Enumeration<NetworkInterface> en = NetworkInterface
                    .getNetworkInterfaces(); en.hasMoreElements();) {
                NetworkInterface intf = en.nextElement();
                for (Enumeration<InetAddress> enumIpAddr = intf
                        .getInetAddresses(); enumIpAddr.hasMoreElements();) {
                    InetAddress inetAddress = enumIpAddr.nextElement();
                    if ((!inetAddress.isLoopbackAddress())
                            && (inetAddress instanceof Inet4Address)) {
                        String ip = inetAddress.getHostAddress();
                        return ip;
                    }
                }
            }
        } catch (SocketException ex) {
            Log.e("TAG", ex.toString());
        }
        return null;
    }

    public static void displayToastMessage(final String message) {

        MainActivity.mMainActivity.runOnUiThread(new Runnable() {
            public void run() {
                Toast.makeText(mContext, message, Toast.LENGTH_LONG).show();
            }
        });

    }

}

class CMDeviceFinderListern implements IDeviceFinderListener {

    @Override
    public void OnDeviceAdded(Device device) {
        // TODO Auto-generated method stub

        try {
            Log.i("CMDeviceFinderListern",
                    "OnDeviceAdded: Device discovered = "
                            + device.getDeviceType());

            Intent intent = new Intent(CMController.mContext,
                    DeviceListView.class);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            CMController.mContext.startActivity(intent);

        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    @Override
    public void OnDeviceRemoved(Device device) {
        // TODO Auto-generated method stub

    }

    @Override
    public void OnDeviceUpdated(Device device) {
        // TODO Auto-generated method stub

    }

    @Override
    public void OnDeviceError(Device device) {
        // TODO Auto-generated method stub

    }
}

class CMListener implements ICMListener {

    @Override
    public void onStarted(String address) {
        // TODO Auto-generated method stub
        Log.i("CMListener", "onStarted address = " + address);
        CMController.isStarted = true;
    }

    @Override
    public void onStopped() {
        // TODO Auto-generated method stub
        CMController.isStarted = false;
        Log.e("TAG", "Control Manager Stopped Completely");
    }

    @Override
    public void onError(CMErrorCode errorCode, String errorMessage) {
        // TODO Auto-generated method stub

    }

    @Override
    public String getMyDeviceToken() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void updateUUIDAndTokenMap(DeviceToken token) {
        // TODO Auto-generated method stub

    }

    @Override
    public void tokenRequestNotification(String uuid) {
        // TODO Auto-generated method stub

    }

    @Override
    public void easySetupNotification(EasySetupNotification notif) {
        // TODO Auto-generated method stub

    }

    @Override
    public void scsErrorNotification(int errorCode) {
        // TODO Auto-generated method stub

    }

    @Override
    public void OnChannelCreated(String channelName, int connID) {
        // TODO Auto-generated method stub

    }

    @Override
    public String getAuthCode() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public String getAccessToken() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public String getRefreshToken() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public String getUserID() {
        // TODO Auto-generated method stub
        return null;
    }

}

class CMNotificationListener implements INotificationListener {

    @Override
    public void onNotificationReceived(String uuid, String resource,
            String elementType, ISerializable notification,
            String subscriptionURI, String eventTime) {
        // TODO Auto-generated method stub

        Log.i("CMNotificationListener", "onNotificationReceived resource = "
                + resource);
        Log.i("CMNotificationListener",
                "onNotificationReceived subscriptionURI = " + subscriptionURI);

        CMController.displayToastMessage("Notification : Received resource =  "
                + resource + ", URL = " + subscriptionURI);
    }

    @Override
    public void onMulticastedNotifcationReceived(Device device,
            String elementType, ISerializable notification) {
        // TODO Auto-generated method stub

    }

}