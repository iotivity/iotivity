/**
 * ***************************************************************
 * <p>
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 * <p>
 * <p>
 * <p>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * <p>
 * ****************************************************************
 */

package org.iotivity.service.easysetup.mediator.ip;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.lang.reflect.Method;
import java.net.InetAddress;
import java.util.ArrayList;

import android.content.Context;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.util.Log;

import org.iotivity.service.easysetup.mediator.EnrolleeInfo;
import org.iotivity.service.easysetup.mediator.EnrolleeOnBoardingInfo;
import org.iotivity.service.easysetup.mediator.IOnBoardingStatus;

/**
 * WiFiSoftAPManager provides wrapper methods for accessing Android Soft Access Point functionality.
 * This is a convenient class, providing Android "WiFiManager" kind of equivalent class for Soft AP.
 * <p>
 * Note: Android doesn't provide public APIs for Soft Access Point feature access.
 * This class provides only reference implementation to use the Soft AP and it uses Java reflection
 * for access Soft Access point features.
 * </p>
 */
public class WiFiSoftAPManager {
    private final WifiManager mWifiManager;
    private Context context;
    static ArrayList<EnrolleeOnBoardingInfo> appNotification =
            new ArrayList<EnrolleeOnBoardingInfo>();
    IOnBoardingStatus finishListener = null;

    public enum WIFI_AP_STATE {
        WIFI_AP_STATE_DISABLING,
        WIFI_AP_STATE_DISABLED,
        WIFI_AP_STATE_ENABLING,
        WIFI_AP_STATE_ENABLED,
        WIFI_AP_STATE_FAILED
    }

    public WiFiSoftAPManager(Context context) {
        this.context = context;
        mWifiManager = (WifiManager) this.context
                .getSystemService(Context.WIFI_SERVICE);
    }

    /*
    * Utility API to check the validity of the MAC Address read from the ARP cache
    */
    private boolean CheckIfValidMacAddress(String macAddr) {
        if (macAddr.matches("..:..:..:..:..:..")) {
            return true;
        } else {
            return false;
        }
    }

    /*
    * The API is used for checking the device entry in the list maintained for the device
    * notifications.
    * If device entry is not found in the list, app is notified.
    * If the device entry is found in the device, as application is already notified it will
    * continue
    */
    private synchronized boolean CheckForDeviceEntryAndNotify(String ipAddr,
                                                              String macAddr, boolean isReachable) {
        final EnrolleeInfo result = new EnrolleeInfo();
        boolean deviceAddedToList = false;

        if (appNotification.size() > 0) {
            for (EnrolleeOnBoardingInfo ipDeviceOnBoardingNotification : appNotification) {
                EnrolleeOnBoardingInfo ipEnrolleeDevice = (EnrolleeOnBoardingInfo) ipDeviceOnBoardingNotification;
                boolean macAddressComparison = ipEnrolleeDevice.getHWAddr().equalsIgnoreCase(
                        macAddr) ? true : false;

                if (macAddressComparison) {
                    deviceAddedToList = true;

                    if (ipDeviceOnBoardingNotification
                            .isAdditionNotified()
                            && isReachable) {
                        continue;
                    } else if (ipDeviceOnBoardingNotification
                            .isRemovalNotified()
                            && !isReachable) {
                        continue;
                    } else {
                        result.setIpAddr(ipAddr);
                        result.setHWAddr(macAddr);
                        result.setReachable(isReachable);

                        appNotification
                                .remove(ipDeviceOnBoardingNotification);
                        if (isReachable) {
                            appNotification
                                    .add(new EnrolleeOnBoardingInfo(ipAddr, macAddr, "", isReachable,
                                            false, true));
                        } else {
                            appNotification
                                    .add(new EnrolleeOnBoardingInfo(ipAddr, macAddr, "", isReachable,
                                            true, false));
                        }

                        NotifyApplication(result);
                        return true;
                    }
                }
            }
            if (!deviceAddedToList) {
                if (isReachable) {
                    appNotification
                            .add(new EnrolleeOnBoardingInfo(ipAddr, macAddr, "", isReachable, false,
                                    true));
                } else {
                    appNotification
                            .add(new EnrolleeOnBoardingInfo(ipAddr, macAddr, "", isReachable, true,
                                    false));
                }

                result.setIpAddr(ipAddr);
                result.setHWAddr(macAddr);
                result.setReachable(isReachable);

                NotifyApplication(result);
                return true;
            }
        } else {
            if (isReachable) {
                appNotification
                        .add(new EnrolleeOnBoardingInfo(ipAddr, macAddr, "", isReachable, false,
                                true));
            } else {
                appNotification
                        .add(new EnrolleeOnBoardingInfo(ipAddr, macAddr, "", isReachable, true,
                                false));
            }

            result.setIpAddr(ipAddr);
            result.setHWAddr(macAddr);
            result.setReachable(isReachable);

            NotifyApplication(result);
            return true;
        }
        return false;
    }

    /**
     * Start WiFi Soft AccessPoint mode with the specified configuration.
     * If the Soft AP is already running, this API call will update the new configuration.
     * <p>
     * Note: Starting Wi-Fi Soft Access Point will disable the Wi-Fi normal operation.
     * </p>
     *
     * @param wifiConfig SSID, security and channel details as part of WifiConfiguration
     * @return {@code true} if the operation succeeds, {@code false} otherwise
     */
    public boolean setWifiApEnabled(WifiConfiguration wifiConfig,
                                    boolean enabled) {
        try {
            // Stopping Wi-Fi mode
            if (enabled) {
                mWifiManager.setWifiEnabled(false);
            }

            Method method = mWifiManager.getClass().getMethod(
                    "setWifiApEnabled", WifiConfiguration.class, boolean.class);
            return (Boolean) method.invoke(mWifiManager, wifiConfig, enabled);
        } catch (Exception e) {
            Log.e(this.getClass().toString(), "", e);
            return false;
        }
    }

    /**
     * Fetch the current state of the Wi-Fi Soft AP
     *
     * @return {@link WIFI_AP_STATE}
     */
    public WIFI_AP_STATE getWifiApState() {
        try {
            Method method = mWifiManager.getClass().getMethod("getWifiApState");

            int currentWiFiState = ((Integer) method.invoke(mWifiManager));

            return WIFI_AP_STATE.class.getEnumConstants()[currentWiFiState];
        } catch (Exception e) {
            Log.e(this.getClass().toString(), "", e);
            return WIFI_AP_STATE.WIFI_AP_STATE_FAILED;
        }
    }

    /**
     * Fetch the current Wi-Fi AP Configuration.
     *
     * @return AP details in {@link WifiConfiguration}
     */
    public WifiConfiguration getWifiApConfiguration() {
        try {
            Method method = mWifiManager.getClass().getMethod(
                    "getWifiApConfiguration");
            return (WifiConfiguration) method.invoke(mWifiManager);
        } catch (Exception e) {
            Log.e(this.getClass().toString(), "", e);
            return null;
        }
    }

    /**
     * Set/Update the Wi-Fi AP Configuration.
     *
     * @return {@code true} if the operation succeeds, {@code false} otherwise
     */
    public boolean setWifiApConfiguration(WifiConfiguration wifiConfig) {
        try {
            Method method = mWifiManager.getClass().getMethod(
                    "setWifiApConfiguration", WifiConfiguration.class);
            return (Boolean) method.invoke(mWifiManager, wifiConfig);
        } catch (Exception e) {
            Log.e(this.getClass().toString(), "", e);
            return false;
        }
    }

    /**
     * Gets a list of the Soft AP clients connected to the Wi-Fi Soft Access point
     *
     * @param finishListener   Interface called when the scan method finishes
     * @param reachableTimeout Reachable Timeout in miliseconds
     */
    public void getClientList(IOnBoardingStatus finishListener, final int reachableTimeout) {
        this.finishListener = finishListener;
        Runnable runnable = new Runnable() {
            public void run() {

                BufferedReader bufferedReader = null;

                try {
                    // Note : This is a reference implementation for getting the list of Enrollee's
                    // connected to the Soft AP.
                    // There is no Android API for getting list of connected devices to the Soft AP.
                    // The connected device information is fetched from Arp cache for Soft AP and
                    // it is maintained in the file "/proc/net/arp"
                    bufferedReader = new BufferedReader(new FileReader("/proc/net/arp"));
                    String line;

                    while ((line = bufferedReader.readLine()) != null) {
                        //ARP entries are splitted using Regex for getting the IP and MAC Address
                        // info
                        String[] arpEntry = line.split(" +");

                        if ((arpEntry != null) && (arpEntry.length >= 4)) {
                            String ipAddr = arpEntry[0];
                            String macAddr = arpEntry[3];


                            // Checking if the string is matching MAC Address is matching the
                            // standard MAC address format.
                            // If the entry is not matching with MAC address format,
                            // it will continue
                            if (CheckIfValidMacAddress(macAddr)) {
                                boolean isReachable = InetAddress.getByName(
                                        ipAddr).isReachable(
                                        reachableTimeout);

                                Log.i("exec statement", ipAddr);
                                Log.i("Return Value", " " + isReachable);

                                // Checking if the app notification entries are available in the
                                // list for the current device
                                // API returns true is there is a notification to the application.
                                // API returns false if there is no entry or if device is
                                // already notified
                                if (CheckForDeviceEntryAndNotify(ipAddr, macAddr, isReachable)) {
                                    break;
                                }
                            }
                        }
                    }
                } catch (Exception e) {
                    Log.e(this.getClass().toString(), e.toString());
                } finally {
                    try {
                        bufferedReader.close();
                    } catch (IOException e) {
                        Log.e(this.getClass().toString(), e.getMessage());
                    }
                }
            }
        };

        Thread mythread = new Thread(runnable);
        mythread.start();
    }

    void NotifyApplication(final EnrolleeInfo result) {
        // Get a handler that can be used to post to the main thread
        Handler mainHandler = new Handler(context.getMainLooper());
        Runnable myRunnable = new Runnable() {
            @Override
            public void run() {
                finishListener.deviceOnBoardingStatus(result);
            }
        };
        mainHandler.post(myRunnable);
    }
}
