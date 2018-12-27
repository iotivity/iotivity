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

import org.iotivity.test.rfw.common.devicecontroller.interfaces.IDeviceLogManager;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObservable;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObserver;

import org.iotivity.test.rfw.common.devicecontroller.linux.LinuxProcessManager;

public class LinuxLogManager implements IDeviceLogManager {

    LinuxProcessManager processManager;

    public LinuxLogManager(LinuxProcessManager processManager) {
        this.processManager = processManager;
    }

    @Override
    public boolean startLogging(String logID) {
        return false;
    }

    @Override
    public boolean stopLogging(String logID) {
        return false;
    }

    @Override
    public void addObserver(String logID, ILogObserver observer) {
        processManager.registerOutput(logID, observer);
    }

    @Override
    public void removeObserver(String logID, ILogObserver observer) {
        processManager.unregisterOutput(logID, observer);
    }

    @Override
    public boolean stopAllLogging() {
        return false;
    }

    @Override
    public ILogObservable getLogObservable(String logID) {
        return processManager.getObserverable(logID);
    }

}
