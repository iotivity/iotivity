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

import org.iotivity.test.rfw.common.devicecontroller.android.AndroidCommand;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObservable;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObserver;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IProcessManager;
import org.iotivity.test.rfw.common.devicecontroller.linux.LinuxLogManager;
import org.iotivity.test.rfw.common.devicecontroller.linux.LinuxProcessManager;
import org.iotivity.test.rfw.common.util.IoTivityLogger;

import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenCommand;

public class TizenConsoleManager implements IProcessManager {

    private String              deviceID;
    private LinuxProcessManager processManager;
    private Logger              logger = IoTivityLogger.GetLogger();
    private LinuxLogManager     logManager;

    public TizenConsoleManager(String deviceID) {
        this.processManager = new LinuxProcessManager();
        this.deviceID = deviceID;

        logger.info("DeviceID: " + deviceID);
        enableRootMode();

        logManager = new LinuxLogManager(
                (LinuxProcessManager) this.processManager);
    }

    /*
     * This function is added to give support to get the TizenConsole Log
     * manager. This log manager will be needed to get the output of console
     * application. Default TizenDLog manager will not work for console
     * applications
     */
    public LinuxLogManager getConsoleLogManager() {
        return logManager;
    }

    /*
     * This function will enable root mode on device to execute commands
     */
    private void enableRootMode() {
        try {
            String command = String.format("%s %s %s %s", TizenCommand.SDB,
                    AndroidCommand.SPECOPTION, deviceID, TizenCommand.ROOT_ON);
            logger.info(command);

            Process process = Runtime.getRuntime().exec(command);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));

            String line;
            while ((line = in.readLine()) != null) {
                logger.info("ROOT ON output : " + line);
            }
        } catch (IOException e) {
            e.printStackTrace();
            logger.info("Exception occured in setting Root ON " + e.toString());
        }
    }

    @Override
    public boolean createProcess(String processName, String processCommand) {
        logger.info("CreateProcess ProcessName: " + processName + " Command :"
                + processCommand);
        String command = String.format("%s %s %s %s", TizenCommand.SDB,
                AndroidCommand.SPECOPTION, deviceID, TizenCommand.SHELL);
        logger.info(command);
        this.processManager.createProcess(processName, command);
        // in the beginning processcomannd was not passed to sdb shell because
        // if processCommand passed
        // to sdb shell in the beginning then input process command doesn't work
        this.processManager.inputProcessCommand(processName, processCommand);
        return true;
    }

    @Override
    public boolean removeProcess(String processName) {
        return this.processManager.removeProcess(processName);
    }

    @Override
    public boolean inputProcessCommand(String processName, String command) {
        logger.info("inputProcessCommand ProcessName: " + processName
                + " Command :" + command);

        return this.processManager.inputProcessCommand(processName, command);
    }

    @Override
    public boolean registerOutput(String processName, ILogObserver observer) {
        return this.processManager.registerOutput(processName, observer);
    }

    @Override
    public boolean unregisterOutput(String processName, ILogObserver observer) {
        return this.processManager.unregisterOutput(processName, observer);
    }

    @Override
    public boolean isProcessRunning(String processName) {
        this.processManager.isProcessRunning(processName);
        return false;
    }

    @Override
    public ILogObservable getObserverable(String processName) {
        return this.processManager.getObserverable(processName);
    }

    @Override
    public boolean removeAllProcess() {
        this.processManager.removeAllProcess();
        return false;
    }
}
