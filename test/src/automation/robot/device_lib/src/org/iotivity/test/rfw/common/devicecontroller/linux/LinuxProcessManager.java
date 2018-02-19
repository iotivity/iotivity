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

import java.util.HashMap;
import java.util.logging.Logger;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObservable;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObserver;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IProcess;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IProcessManager;
import org.iotivity.test.rfw.common.util.IoTivityLogger;

import org.iotivity.test.rfw.common.devicecontroller.linux.LinuxProcess;

public class LinuxProcessManager implements IProcessManager {
    private Logger                    logger = IoTivityLogger.GetLogger();
    private HashMap<String, IProcess> processMap;

    public LinuxProcessManager() {
        processMap = new HashMap<String, IProcess>();
    }

    @Override
    public boolean createProcess(String processName, String processCommand) {
        processName = convertProcessName(processName);

        LinuxProcess linuxProcess = new LinuxProcess(processName, processCommand);

        processMap.put(processName, linuxProcess);

        return true;
    }

    @Override
    public boolean removeProcess(String processName) {
        try {
            processName = convertProcessName(processName);

            if (processMap.get(processName).isProcessRunning())
                processMap.get(processName).destroy();

            processMap.remove(processName);

            return true;
        } catch (Exception ex) {
            return false;
        }
    }

    @Override
    public boolean removeAllProcess() {
        for (String tempProcessID : processMap.keySet()) {
            if (processMap.get(tempProcessID).isProcessRunning())
                processMap.get(tempProcessID).destroy();
        }
        processMap.clear();

        return true;
    }

    @Override
    public boolean inputProcessCommand(String processName, String command) {
        processName = convertProcessName(processName);

        if (processMap.containsKey(processName))
            return processMap.get(processName).inputCommand(command);
        else
            return false;
    }

    @Override
    public boolean registerOutput(String processName, ILogObserver observer) {
        processName = convertProcessName(processName);

        if (processMap.containsKey(processName)) {

            ILogObservable logObservable = processMap.get(processName).getObserverable();
            logObservable.addObserver(observer);
            logObservable.notifyObervers();

            return true;
        } else {
            return false;
        }
    }

    @Override
    public boolean unregisterOutput(String processName, ILogObserver observer) {
        processName = convertProcessName(processName);

        if (processMap.containsKey(processName))
            return processMap.get(processName).getObserverable()
                    .removeObserver(observer);
        else
            return false;
    }

    @Override
    public boolean isProcessRunning(String processName) {
        processName = convertProcessName(processName);

        if (processMap.containsKey(processName))
            return processMap.get(processName).isProcessRunning();
        else
            return false;
    }

    @Override
    public ILogObservable getObserverable(String processName) {
        processName = convertProcessName(processName);

        if (processMap.containsKey(processName))
            return processMap.get(processName).getObserverable();

        return null;
    }

    private String convertProcessName(String processName) {
        String simpleProcessName = processName.replaceAll(" ", "_").split(
                "/")[processName.replaceAll(" ", "_").split("/").length - 1];

        return simpleProcessName;
    }
}
