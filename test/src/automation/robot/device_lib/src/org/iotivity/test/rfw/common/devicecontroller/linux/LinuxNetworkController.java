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

package org.iotivity.test.rfw.common.devicecontroller.linux;

import java.util.ArrayList;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.INetworkController;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IProcessObserver;
import org.iotivity.test.rfw.common.util.SystemUtil;

import org.iotivity.test.rfw.common.devicecontroller.linux.LinuxProcess;

class LinuxNetworkController implements INetworkController {
    private static final String WIFI_PREFIX     = "wlan";
    private static final String IP_IF_UP_CMD    = "ifup ";
    private static final String IP_IF_DOWN_CMD  = "ifdown ";
    private static final String ALL_NETWORK_CMD = "ls /sys/class/net";

    private ArrayList<String>   availableNetworks;
    private StringBuffer        resultStream;

    public LinuxNetworkController() {
        availableNetworks = new ArrayList<String>();
        resultStream = new StringBuffer();
        getNetworkInformations();
    }

    @Override
    public boolean wifiOn() {
        for (String networkIface : availableNetworks) {
            if (networkIface.startsWith(WIFI_PREFIX)) {
                runNetworkCMD(IP_IF_UP_CMD + networkIface);
            }
        }

        return true;
    }

    @Override
    public boolean wifiOff() {
        for (String networkIface : availableNetworks) {
            if (networkIface.startsWith(WIFI_PREFIX)) {
                runNetworkCMD(IP_IF_DOWN_CMD + networkIface);
            }
        }

        return true;
    }

    @Override
    public boolean blueToothOn() {
        return false;
    }

    @Override
    public boolean blueToothOff() {
        return false;
    }

    @Override
    public String getIPAddress() {
        return "";
    }

    @Override
    public String getBluetoothMacAddress() {
        return "";
    }

    @Override
    public ArrayList<String> getNetworkInformations() {
        runNetworkCMD(ALL_NETWORK_CMD);

        while (resultStream.equals("")) {
            SystemUtil.sleep(200);
        }

        String temp = System.getProperty("line.separator");

        String[] networks = resultStream.toString().split(temp);
        for (String networkIfcae : networks) {
            availableNetworks.add(networkIfcae);
        }

        return availableNetworks;
    }

    private void runNetworkCMD(String networkCMD) {
        resultStream.setLength(0);
        new LinuxProcess("getNetwork", networkCMD);
    }

    class Observation implements IProcessObserver {
        @Override
        //public synchronized void update(byte[] stdoutLine, int len) {
        public void update(byte[] stdoutLine, int len) {
            resultStream.append(new String(stdoutLine));
        }
    }

    @Override
    public boolean setSDKPath(String sdkPath) {
        return false;
    }
}
