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

package org.iotivity.test.rfw.common.devicecontroller.android;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.logging.Logger;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.INetworkController;
import org.iotivity.test.rfw.common.util.IoTivityLogger;
import org.iotivity.test.rfw.common.util.SystemUtil;

import org.iotivity.test.rfw.common.devicecontroller.android.AndroidCommand;

class AndroidNetworkManager implements INetworkController {

    private Logger logger  = IoTivityLogger.GetLogger();
    private String sdkPath = "";

    @Override
    public boolean wifiOn() {

        String[] command = { AndroidCommand.ADB, AndroidCommand.SHELL,
                AndroidCommand.ROOT, AndroidCommand.CLEAN,
                AndroidCommand.WIFI_SERVICE_ENABLE };

        try {

            BufferedReader in = getProcessBuilderReader(command);

            SystemUtil.sleep(1000);

            String line = null;
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

        String[] command = { AndroidCommand.ADB, AndroidCommand.SHELL,
                AndroidCommand.ROOT, AndroidCommand.CLEAN,
                AndroidCommand.WIFI_SERVICE_DISABLE };

        try {

            BufferedReader in = getProcessBuilderReader(command);

            SystemUtil.sleep(1000);

            String line = null;
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

        try {
            Process process = Runtime.getRuntime()
                    .exec(AndroidCommand.BLUETOOTH_SERVICE_ENABLE);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));

            SystemUtil.sleep(1000);

            String line = null;
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

        String[] command = { AndroidCommand.ADB, AndroidCommand.SHELL,
                AndroidCommand.ROOT, AndroidCommand.CLEAN,
                AndroidCommand.BLUETOOTH_SERVICE_DISABLE };

        try {

            BufferedReader in = getProcessBuilderReader(command);

            SystemUtil.sleep(1000);

            String line = null;
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

        String line = null;
        String ret = null;
        try {
            Process process = Runtime.getRuntime()
                    .exec(AndroidCommand.GET_IP_CMD);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));

            SystemUtil.sleep(1000);

            while ((line = in.readLine()) != null) {
                logger.info(line + "\n");
                if (line.contains("fail"))
                    return null;
                else if (line.contains("wlan0")) {
                    if (line.contains("inet")) {
                        int start = line.indexOf("inet") + "inet".length();
                        int end = line.indexOf("/");
                        ret = line.substring(start, end);
                        return ret;
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        return ret;
    }

    @Override
    public String getBluetoothMacAddress() {
        String line = null;
        try {

            Process process = Runtime.getRuntime()
                    .exec(AndroidCommand.GET_MAC_ADDRESS);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));

            SystemUtil.sleep(1000);
            while ((line = in.readLine()) != null) {
                if (line.contains("fail"))
                    return null;
                else
                    return line;
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
        if (sdkPath != null) {
            this.sdkPath = sdkPath;
            return true;
        } else
            return false;
    }

    public BufferedReader getProcessBuilderReader(String[] command) {
        ProcessBuilder probuilder = null;
        Process process = null;
        try {
            probuilder = new ProcessBuilder(command);

            // You can set up your work directory
            probuilder.directory(new File(sdkPath));
            process = probuilder.start();

        } catch (Exception e) {
            e.printStackTrace();
        }

        return new BufferedReader(
                new InputStreamReader(process.getInputStream()));
    }

}