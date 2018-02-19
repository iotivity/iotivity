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

package org.iotivity.test.rfw.common.devicecontroller;

import java.util.logging.Logger;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.IDeviceBridgeManager;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IDeviceController;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IDeviceLogManager;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IFileManager;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.INetworkController;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IProcess;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IProcessManager;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IScreenController;
import org.iotivity.test.rfw.common.util.IoTivityLogger;

public abstract class AbstractDeviceController implements IDeviceController {

    protected String               deviceID;
    protected IProcessManager      processManager      = null;
    protected IProcessManager      consoleManager      = null;
    protected IFileManager         fileManager         = null;
    protected INetworkController   networkManager      = null;
    protected IProcess             process             = null;
    protected IScreenController    screenController    = null;
    protected IDeviceBridgeManager deviceBridgeManager = null;
    protected IDeviceLogManager    logManager          = null;

    protected Logger               logger              = IoTivityLogger
            .GetLogger();

    public AbstractDeviceController(String deviceID) {
        this.deviceID = deviceID;
    }

    @Override
    public IProcessManager getProcessManager() {
        return processManager;
    }

    @Override
    public IProcessManager getConsoleManager() {
        return consoleManager;
    }

    @Override
    public IFileManager getFileManager() {
        return fileManager;
    }

    @Override
    public INetworkController getNetworkManager() {
        return networkManager;
    }

    @Override
    public String getDeviceID() {
        return deviceID;
    }

    @Override
    public IDeviceBridgeManager getDeviceBridgeManager() {
        return deviceBridgeManager;
    }

    @Override
    public IDeviceLogManager getLogManager() {
        return logManager;
    }

    @Override
    public IScreenController getScreenController() {
        return screenController;
    }
}
