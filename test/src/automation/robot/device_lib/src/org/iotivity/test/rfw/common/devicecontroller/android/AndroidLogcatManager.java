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

import org.iotivity.test.rfw.common.devicecontroller.interfaces.IDeviceLogManager;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObservable;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObserver;
import org.iotivity.test.rfw.common.devicecontroller.linux.LinuxProcessManager;

import com.android.ddmlib.IDevice;
import org.iotivity.test.rfw.common.devicecontroller.android.AndroidCommand;

class AndroidLogcatManager implements IDeviceLogManager {

    private String              deviceID;
    private LinuxProcessManager logProcessManager;

    public AndroidLogcatManager(IDevice device) {
        this.deviceID = device.getSerialNumber();
        logProcessManager = new LinuxProcessManager();

        String command = String.format("%s %s %s %s", AndroidCommand.ADB,
                AndroidCommand.SPECOPTION, deviceID, AndroidCommand.SHELL);
        logProcessManager.createProcess(deviceID, command);

        command = String.format("%s %s", AndroidCommand.LOGCAT,
                AndroidCommand.CLEAN);
        logProcessManager.inputProcessCommand(deviceID, command);

        logProcessManager.inputProcessCommand(deviceID, AndroidCommand.LOGCAT);
    }

    @Override
    public boolean startLogging(String logID) {

        String command = String.format("%s %s %s %s", AndroidCommand.ADB,
                AndroidCommand.SPECOPTION, deviceID, AndroidCommand.SHELL);
        logProcessManager.createProcess(logID, command);

        command = String.format("%s %s %s", AndroidCommand.LOGCAT,
                AndroidCommand.SHELL, logID);
        return logProcessManager.inputProcessCommand(logID, command);
    }

    @Override
    public boolean stopLogging(String logID) {
        return logProcessManager.removeProcess(logID);
    }

    @Override
    public void addObserver(String logID, ILogObserver observer) {
        logProcessManager.registerOutput(logID, observer);
    }

    @Override
    public void removeObserver(String logID, ILogObserver observer) {
        logProcessManager.unregisterOutput(logID, observer);
    }

    @Override
    public boolean stopAllLogging() {
        return logProcessManager.removeAllProcess();
    }

    @Override
    public ILogObservable getLogObservable(String logID) {
        return logProcessManager.getObserverable(logID);
    }

}
