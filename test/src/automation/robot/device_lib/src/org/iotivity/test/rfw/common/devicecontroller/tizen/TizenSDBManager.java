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
import java.util.logging.Logger;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.IDeviceBridgeManager;
import org.iotivity.test.rfw.common.devicecontroller.linux.LinuxProcessManager;
import org.iotivity.test.rfw.common.util.IoTivityLogger;
import org.iotivity.test.rfw.common.util.SystemUtil;

import com.android.ddmlib.IShellOutputReceiver;
import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenCommand;

class TizenSDBManager implements IDeviceBridgeManager {

    private LinuxProcessManager sdbProcessManager;
    private String              deviceID;
    private Logger              logger = IoTivityLogger.GetLogger();

    public TizenSDBManager(String deviceID) {
        startServer();
        this.deviceID = deviceID;
        this.sdbProcessManager = new LinuxProcessManager();
        executeAsyncCommand(TizenCommand.ROOT_ON, null);
    }

    public String getDeviceID() {
        return deviceID;
    }

    public void setDeviceID(String deviceID) {
        this.deviceID = deviceID;
    }

    @Override
    public void startServer() {
        try {
            String command = String.format("%s %s", TizenCommand.SDB,
                    TizenCommand.StartADBServer);
            logger.info(command);

            Process process = Runtime.getRuntime().exec(command);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));

            while (in.readLine() != null) {
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void stopServer() {
        String command = String.format("%s %s", TizenCommand.SDB,
                TizenCommand.KillADBServer);
        logger.info(command);

        sdbProcessManager.createProcess("StopSDBServer", command);
    }

    @Override
    public void reboot(String msg) {
        executeAsyncCommand(TizenCommand.RebootEvent, null);
    }

    @Override
    public void startService(String serviceName, String className) {
        String command = String.format("%s %s", TizenCommand.StartService,
                serviceName);
        logger.info(command);

        executeAsyncCommand(command, null);
    }

    @Override
    public void stopService(String serviceName) {
        String command = String.format("%s %s", TizenCommand.StopService,
                serviceName);
        logger.info(command);

        executeAsyncCommand(command, null);
    }

    @Override
    public boolean isInstalledApp(String appName) {
        return false;
    }

    @Override
    public boolean isRunningApp(String appName) {
        return false;
    }

    @Override
    public boolean installApplication(String path) {

        String command = String.format("%s %s %s %s %s", TizenCommand.SDB,
                TizenCommand.SPECOPTION, this.deviceID,
                TizenCommand.INSTALL_APP, path);
        logger.info(command);

        sdbProcessManager.createProcess("InstallApp", command);
        return false;
    }

    @Override
    public boolean unInstallApp(String packageName) {
        return false;
    }

    @Override
    public void executeAsyncCommand(String cmd, IShellOutputReceiver logger) {
        String command = String.format("%s %s %s %s", TizenCommand.SDB,
                TizenCommand.SPECOPTION, this.deviceID, cmd);

        sdbProcessManager.createProcess(cmd, command);
    }

    @Override
    public boolean connectWifiDevice() {
        String command = String.format("%s %s %s", TizenCommand.SDB,
                TizenCommand.CONNECT, this.getDeviceID());
        logger.info(command);

        sdbProcessManager.createProcess(this.getDeviceID(), command);
        // System.out.println(SDBPath + " connect " + this.getDeviceID());
        SystemUtil.sleep(2000);

        return true;
    }

}
