/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
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
 *
 ******************************************************************/

package org.iotivity.test.rfw.common.devicecontroller.tizen;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.INetworkController;
import org.iotivity.test.rfw.common.util.SystemUtil;

import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenCommand;

class TizenNetworkManager implements INetworkController {

    private String deviceID = null;

    public TizenNetworkManager(String deviceID) {
        this.deviceID = deviceID;
    }

    @Override
    public boolean wifiOn() {

        String[] command = { TizenCommand.SDB, TizenCommand.SPECOPTION,
                this.deviceID, TizenCommand.SHELL, TizenCommand.ROOT,
                TizenCommand.CLEAN, TizenCommand.WIFI_SERVICE_ENABLE };
        try {
            Process process = Runtime.getRuntime().exec(command);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));
            String line = null;
            SystemUtil.sleep(1000);
            while ((line = in.readLine()) != null) {
                if (line.contains("fail"))
                    return false;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return true;
    }

    @Override
    public boolean wifiOff() {
        String[] command = { TizenCommand.SDB, TizenCommand.SPECOPTION,
                this.deviceID, TizenCommand.SHELL, TizenCommand.ROOT,
                TizenCommand.CLEAN, TizenCommand.WIFI_SERVICE_DISABLE };
        try {
            Process process = Runtime.getRuntime().exec(command);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));

            String line = null;
            SystemUtil.sleep(1000);
            while ((line = in.readLine()) != null) {
                if (line.contains("fail"))
                    return false;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return true;
    }

    @Override
    public boolean blueToothOn() {
        String[] command = { TizenCommand.SDB, TizenCommand.SPECOPTION,
                this.deviceID, TizenCommand.SHELL, TizenCommand.ROOT,
                TizenCommand.CLEAN, TizenCommand.BLUETOOTH_SERVICE_ENABLE };
        try {
            Process process = Runtime.getRuntime().exec(command);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));
            String line = null;
            SystemUtil.sleep(1000);
            while ((line = in.readLine()) != null) {
                if (line.contains("fail"))
                    return false;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return true;

    }

    @Override
    public boolean blueToothOff() {
        String[] command = { TizenCommand.SDB, TizenCommand.SPECOPTION,
                this.deviceID, TizenCommand.SHELL, TizenCommand.ROOT,
                TizenCommand.CLEAN, TizenCommand.BLUETOOTH_SERVICE_DISABLE };
        try {
            Process process = Runtime.getRuntime().exec(command);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));
            String line = null;
            SystemUtil.sleep(1000);
            while ((line = in.readLine()) != null) {
                if (line.contains("fail"))
                    return false;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return true;
    }

    @Override
    public String getIPAddress() {

        String line = "NULL";
        String[] command = { TizenCommand.SDB, TizenCommand.SPECOPTION,
                this.deviceID, TizenCommand.SHELL, TizenCommand.ROOT,
                TizenCommand.CLEAN, TizenCommand.GET_IP_ADDRESS };

        try {
            Process process = Runtime.getRuntime().exec(command);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));

            SystemUtil.sleep(1000);
            while ((line = in.readLine()) != null) {
                if (line.contains("fail"))
                    return "Bluetooth Address failed";
                else if (line.contains("")) {
                    if (line.contains("IP Address:")) {
                        int start = line.indexOf("IP Address: ")
                                + "IP Address: ".length();
                        String ret = line.substring(start, line.length());
                        return ret;
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
            line = "Exception " + e.toString();
        }

        return line;
    }

    @Override
    public String getBluetoothMacAddress() {

        String line = "NULL";
        String[] command = { TizenCommand.SDB, TizenCommand.SPECOPTION,
                this.deviceID, TizenCommand.SHELL, TizenCommand.ROOT,
                TizenCommand.CLEAN, TizenCommand.GET_MAC_ADDRESS };

        // String cmd = "service call bluetooth_manager 10";
        try {
            Process process = Runtime.getRuntime().exec(command);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));

            SystemUtil.sleep(1000);
            while ((line = in.readLine()) != null) {
                if (line.contains("fail"))
                    return "Bluetooth Address failed";
                else if (line.contains("")) {
                    if (line.contains("Adapter address:")) {
                        int start = line.indexOf("Adapter address: ")
                                + "Adapter address: ".length();
                        int end = line.indexOf(".");
                        String ret = line.substring(start, end);
                        return ret;
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return line;
    }

    @Override
    public ArrayList<String> getNetworkInformations() {
        return null;
    }

    @Override
    public boolean setSDKPath(String sdkPath) {
        return false;
    }

}