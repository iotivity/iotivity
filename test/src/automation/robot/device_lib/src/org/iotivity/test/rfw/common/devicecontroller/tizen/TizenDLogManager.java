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

import java.util.logging.Logger;

import org.iotivity.test.rfw.common.devicecontroller.android.AndroidCommand;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IDeviceLogManager;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObservable;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObserver;
import org.iotivity.test.rfw.common.devicecontroller.linux.LinuxProcessManager;
import org.iotivity.test.rfw.common.util.IoTivityLogger;

import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenCommand;
import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenConsoleManager;

class TizenDLogManager implements IDeviceLogManager {

    private String              deviceID;
    private LinuxProcessManager logProcessManager;
    private TizenConsoleManager consoleManager;
    private Logger              logger = IoTivityLogger.GetLogger();

    public TizenDLogManager(String deviceID,
            TizenConsoleManager consoleManager) {
        this.deviceID = deviceID;
        this.consoleManager = consoleManager;

        logProcessManager = new LinuxProcessManager();

        String command = String.format("%s %s %s %s", TizenCommand.SDB,
                AndroidCommand.SPECOPTION, deviceID, TizenCommand.SHELL);
        logger.info(command);

        logProcessManager.createProcess(deviceID, command);

        command = String.format("%s %s", TizenCommand.SDB,
                TizenCommand.DLOGUTIL, TizenCommand.CLEAN);
        logger.info(command);

        logProcessManager.inputProcessCommand(deviceID, command);
        logProcessManager.inputProcessCommand(deviceID, TizenCommand.DLOGUTIL);
    }

    @Override
    public boolean startLogging(String logID) {

        String command = String.format("%s %s %s %s", TizenCommand.SDB,
                AndroidCommand.SPECOPTION, deviceID, TizenCommand.SHELL);
        logger.info(command);

        logProcessManager.createProcess(logID, command);

        command = String.format("%s %s", TizenCommand.DLOGUTIL, logID);
        logger.info(command);
        return logProcessManager.inputProcessCommand(logID, command);
    }

    @Override
    public boolean stopLogging(String logID) {
        return logProcessManager.removeProcess(logID);
    }

    @Override
    public boolean stopAllLogging() {
        return logProcessManager.removeAllProcess();
    }

    @Override
    public void addObserver(String logID, ILogObserver observer) {
        logProcessManager.registerOutput(logID, observer);
        consoleManager.registerOutput(logID, observer);
    }

    @Override
    public void removeObserver(String logID, ILogObserver observer) {
        logProcessManager.unregisterOutput(logID, observer);
        consoleManager.registerOutput(logID, observer);
    }

    @Override
    public ILogObservable getLogObservable(String logID) {
        if (logProcessManager.getObserverable(logID) != null)
            return logProcessManager.getObserverable(logID);
        else
            return consoleManager.getObserverable(logID);
    }

}
