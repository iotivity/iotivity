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

import java.util.HashMap;

import org.slf4j.Logger;

import oic.ctt.devicecontroller.interfaces.ILogObservable;
import oic.ctt.devicecontroller.interfaces.ILogObserver;
import oic.ctt.devicecontroller.interfaces.IProcess;
import oic.ctt.devicecontroller.interfaces.IProcessManager;
import oic.ctt.logger.CTLogger;

public class LinuxProcessManager implements IProcessManager {
    private Logger                    logger = CTLogger.getInstance();
    private HashMap<String, IProcess> processMap;

    public LinuxProcessManager() {
        processMap = new HashMap<String, IProcess>();
    }

    @Override
    public boolean createProcess(String processName, String processCommand) {
        processName = convertProcessName(processName);
        processMap.put(processName, new LinuxProcess(processName,
                processCommand));

        logger.info("New process created with name " + processName
                + " and processCommand: " + processCommand
                + " putted into processMap successfull");

        String processMapKeys = "";
        for (String key : processMap.keySet()) {
            processMapKeys += key + ", ";

        }
        logger.info("Process map has : " + processMapKeys);

        return true;
    }

    @Override
    public boolean removeProcess(String processName) {
        processName = convertProcessName(processName);

        if (processMap.get(processName).isProcessRunning())
            processMap.get(processName).destroy();

        processMap.remove(processName);

        return true;
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

        logger.info("registerOutput processName " + processName + " observer ");
        String processMapKeys = "";
        for (String key : processMap.keySet()) {
            processMapKeys += key + ", ";

        }
        logger.info("Process map has : " + processMapKeys);

        if (processMap.containsKey(processName)) {
            logger.info("registerOutput processName " + processName
                    + " processMap.containsKey(processName) ");

            processMap.get(processName).getObserverable().addObserver(observer);
            processMap.get(processName).getObserverable().notifyObervers();

            return true;
        } else {
            logger.info("registerOutput returning False");
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
        String simpleProcessName = processName.replaceAll(" ", "_").split("/")[processName
                .replaceAll(" ", "_").split("/").length - 1];

        return simpleProcessName;
    }
}
