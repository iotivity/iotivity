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
import java.io.IOException;
import java.io.InputStreamReader;

import org.iotivity.test.rfw.common.devicecontroller.AbstractDeviceController;
import org.iotivity.test.rfw.common.util.SystemUtil;

import com.android.ddmlib.IDevice;
import org.iotivity.test.rfw.common.devicecontroller.android.AndroidADBManager;
import org.iotivity.test.rfw.common.devicecontroller.android.AndroidApplicationManager;
import org.iotivity.test.rfw.common.devicecontroller.android.AndroidCommand;
import org.iotivity.test.rfw.common.devicecontroller.android.AndroidLogcatManager;
import org.iotivity.test.rfw.common.devicecontroller.android.AndroidNetworkManager;
import org.iotivity.test.rfw.common.devicecontroller.android.AndroidScreenController;

public class AndroidDeviceController extends AbstractDeviceController {
    private IDevice device;

    public AndroidDeviceController(String deviceID) throws Exception {
        super(deviceID);
        if (deviceID.contains(".")) {
            if (!connectWifiDevice(deviceID))
                throw new Exception("Fail to connect with TCP/IP");
        }

        this.deviceBridgeManager = new AndroidADBManager(deviceID);
        AndroidADBManager adbManager = (AndroidADBManager) deviceBridgeManager;

        device = adbManager.getDevice();
        this.logManager = new AndroidLogcatManager(device);
        this.screenController = new AndroidScreenController(
                adbManager.getMonkeyPort());
        this.processManager = new AndroidApplicationManager(adbManager);
        this.networkManager = new AndroidNetworkManager();
    }

    @Override
    public void close() {
        try {
            this.logManager.stopAllLogging();
            screenController.closeControlSocket();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private boolean connectWifiDevice(String ipaddress) {

        try {
            String[] cmds = { AndroidCommand.ADB, AndroidCommand.CONNECT,
                    ipaddress };
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
