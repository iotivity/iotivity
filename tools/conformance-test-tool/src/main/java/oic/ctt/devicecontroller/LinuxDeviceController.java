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

import oic.ctt.devicecontroller.AbstractDeviceController;

public class LinuxDeviceController extends AbstractDeviceController {

    public LinuxDeviceController(String deviceID) {
        super(deviceID);

        this.processManager = new LinuxProcessManager();
        this.fileManager = new LinuxFileManager();
        this.networkManager = new LinuxNetworkController();
        this.logManager = new LinuxLogManager(
                (LinuxProcessManager) processManager);
    }

    @Override
    public void close() {
        processManager.removeAllProcess();
    }
}
