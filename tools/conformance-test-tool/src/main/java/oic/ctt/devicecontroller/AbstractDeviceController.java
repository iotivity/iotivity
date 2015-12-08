//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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
package oic.ctt.devicecontroller;

import oic.ctt.devicecontroller.interfaces.IDeviceController;
import oic.ctt.devicecontroller.interfaces.IDeviceLogManager;
import oic.ctt.devicecontroller.interfaces.IFileManager;
import oic.ctt.devicecontroller.interfaces.INetworkController;
import oic.ctt.devicecontroller.interfaces.IProcess;
import oic.ctt.devicecontroller.interfaces.IProcessManager;

public abstract class AbstractDeviceController implements IDeviceController {

    protected String             deviceID;
    protected IProcessManager    processManager = null;
    protected IProcessManager    consoleManager = null;
    protected IFileManager       fileManager    = null;
    protected INetworkController networkManager = null;
    protected IProcess           process        = null;
    protected IDeviceLogManager  logManager     = null;


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
    public IDeviceLogManager getLogManager() {
        return logManager;
    }

}
