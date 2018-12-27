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

import org.iotivity.test.rfw.common.devicecontroller.AbstractDeviceController;
import org.iotivity.test.rfw.common.util.SystemUtil;

import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenApplicationManager;
import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenConsoleManager;
import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenDLogManager;
import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenFileManager;
import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenNetworkManager;
import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenSDBManager;
import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenScreenController;

public class TizenDeviceController extends AbstractDeviceController {

    public TizenDeviceController(String deviceID) throws Exception {
        super(deviceID);
        if (deviceID.contains(".")) {
            if (!connectWifiDevice(deviceID))
                throw new Exception("Fail to connect with TCP/IP");
        }

        this.deviceBridgeManager = new TizenSDBManager(deviceID);
        this.screenController = new TizenScreenController(
                (TizenSDBManager) this.deviceBridgeManager);
        this.consoleManager = new TizenConsoleManager(deviceID);
        this.logManager = new TizenDLogManager(deviceID,
                (TizenConsoleManager) this.consoleManager);
        this.processManager = new TizenApplicationManager(
                (TizenSDBManager) this.deviceBridgeManager);
        this.fileManager = new TizenFileManager(deviceID);
        this.networkManager = new TizenNetworkManager(deviceID);
    }

    @Override
    public void close() {
        try {
            logManager.stopAllLogging();
            screenController.closeControlSocket();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private boolean connectWifiDevice(String ipaddress) {
        String cmd = "sdb";
        try {
            String connecct = "connect";
            String[] cmds = { cmd, connecct, ipaddress };
            Process process = Runtime.getRuntime().exec(cmds);
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
}
