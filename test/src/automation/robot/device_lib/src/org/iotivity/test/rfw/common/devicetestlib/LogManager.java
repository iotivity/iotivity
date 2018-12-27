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

package org.iotivity.test.rfw.common.devicetestlib;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Logger;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.IDeviceController;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObserver;
import org.iotivity.test.rfw.common.devicecontroller.linux.LinuxProcessObserver;
import org.iotivity.test.rfw.common.util.FileUtil;
import org.iotivity.test.rfw.common.util.IoTivityLogger;

public class LogManager {
    private static LogManager instance = new LogManager();

    public static LogManager getInstance() {
        return instance;
    }

    private Map<String, ILogObserver>              deviceObserverList;
    private Map<String, Map<String, ILogObserver>> processObserverList;

    private String                                 baseLogDir;
    private String                                 logDir = "";
    private Logger                                 logger = IoTivityLogger
            .GetLogger();

    private LogManager() {
        deviceObserverList = new HashMap<String, ILogObserver>();
        processObserverList = new HashMap<String, Map<String, ILogObserver>>();

        GregorianCalendar gregoriCalender = new GregorianCalendar();
        SimpleDateFormat dateFormat = new SimpleDateFormat("yyyyMMddHHmmss");
        Date date = gregoriCalender.getTime();
        logDir = dateFormat.format(date);

        baseLogDir = FileUtil.getWorkingDir() + "/logs/" + logDir;
        logDir = baseLogDir;
        FileUtil.makeDir(baseLogDir);

        logger.info("LogManager Loaded");
    }

    public boolean registerDeviceLogger(String deviceID,
            IDeviceController device, boolean makeFile) {
        LinuxProcessObserver deviceObserver = new LinuxProcessObserver(deviceID,
                null, makeFile, logDir);

        deviceObserver.registerObserverable(device);
        deviceObserverList.put(deviceID, deviceObserver);
        return true;
    }

    public boolean registerProcessLogger(String deviceID,
            IDeviceController device, String processID, boolean makeFile) {
        LinuxProcessObserver processObserver = new LinuxProcessObserver(
                processID, null, makeFile, logDir);
        processObserver.registerObserverable(device);

        if (!processObserverList.containsKey(deviceID)) {
            Map<String, ILogObserver> processObserverMap = new HashMap<String, ILogObserver>();
            processObserverMap.put(processID, processObserver);
            processObserverList.put(deviceID, processObserverMap);
        } else {
            try {
                Map<String, ILogObserver> tempMap = processObserverList
                        .get(deviceID);
                if (tempMap != null) {
                    tempMap.put(processID, processObserver);
                }
            }
            catch(Exception ex) {
                logger.info("exception occured when get from processObserverList");
            }
        }
        return true;
    }

    /*
     * Function to register console process logger for tizen console app
     */
    public boolean registerConsoleProcessLogger(String deviceID,
            IDeviceController device, String processID, boolean makeFile) {
        logger.info("registerProcessLogger deviceID: " + deviceID
                + " ControllerDeviceID:" + device.getDeviceID() + " processID :"
                + processID);

        LinuxProcessObserver processObserver = new LinuxProcessObserver(
                processID, null, makeFile, logDir);
        processObserver.registerConsoleObserverable(device);

        if (!processObserverList.containsKey(deviceID)) {
            Map<String, ILogObserver> processObserverMap = new HashMap<String, ILogObserver>();
            processObserverMap.put(processID, processObserver);
            processObserverList.put(deviceID, processObserverMap);
        } else {
            Map<String, ILogObserver> tempMap = processObserverList
                    .get(deviceID);
            if (tempMap != null)
                tempMap.put(processID, processObserver);
        }
        return true;
    }

    public ILogObserver getConsoleProcessLogger(String deviceID,
            String processID) {
        Map<String, ILogObserver> tempMap = processObserverList.get(deviceID);
        if (tempMap != null)
            return tempMap.get(processID);
        return null;
    }

    public ILogObserver getDeviceLogger(String deviceID) {
        return deviceObserverList.get(deviceID);
    }

    public ILogObserver getProcessLogger(String deviceID, String processID) {
        // logger.info("GetProcessLogger called");
        Map<String, ILogObserver> tempMap = processObserverList.get(deviceID);
        if (tempMap != null) {
            return tempMap.get(processID);
        }
        return null;
    }

    public boolean unregisterDeviceLogger(String deviceID,
            IDeviceController device) {
        deviceObserverList.remove(deviceID).unregisterObserverable(device);
        return true;
    }

    public boolean unregisterProcessLogger(String deviceID,
            IDeviceController device, String processID) {
        Map<String, ILogObserver> tempMap = processObserverList.get(deviceID);
        if (tempMap != null) {
            tempMap.remove(processID).unregisterObserverable(device);
            return true;
        }
        return false;
    }

    public void setLogPath(String path) {
        logDir = baseLogDir + "/" + path;
        FileUtil.makeDir(logDir);
    }

    public String getLogPath() {
        return logDir;
    }
}
