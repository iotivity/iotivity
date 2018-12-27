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

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.logging.Logger;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.IDeviceController;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObserver;
import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenConsoleManager;
import org.iotivity.test.rfw.common.util.IoTivityLogger;

public class LinuxProcessObserver implements ILogObserver {
    private File          logFile;
    private OutputStream  logOutStream;
    private String        logObserverID;
    private StringBuilder logBuf;
    private boolean       isEnabled = true;
    private boolean       makeFile  = false;
    private String        pattern;
    private Logger        logger    = IoTivityLogger.GetLogger();

    public LinuxProcessObserver(String logObserverID, String pattern,
            boolean makeFile, String logDirPath) {
        this.logObserverID = logObserverID;
        this.logBuf = new StringBuilder();
        this.pattern = pattern;
        this.makeFile = makeFile;
        logFile = new File(logDirPath + "/" + logObserverID + ".log");

        try {
            logOutStream = new FileOutputStream(logFile);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    }

    @Override
    public boolean startLogging() {
        isEnabled = true;

        return true;
    }

    @Override
    public boolean stopLogging() {
        isEnabled = false;

        return true;
    }

    @Override
    public boolean cleanLog() {
        logBuf.delete(0, logBuf.length());
        return true;
    }

    @Override
    public String getLogs(String filter) {
        //synchronized (this) {
            StringBuilder filteredBuf = new StringBuilder();
            String[] copyofLogBuf = logBuf.toString().split("\n");
            for (String line : copyofLogBuf) {
                if (line.contains(filter)) {
                    filteredBuf.append(line);
                }
            }

            return filteredBuf.toString();
        //}
    }

    @Override
    public String getLogs() {
        //synchronized (this) {
            return logBuf.toString();
        //}
    }

    @Override
    public boolean registerObserverable(IDeviceController controller) {
        logger.info("registerObserverable");

        controller.getLogManager().addObserver(logObserverID, this);

        return true;
    }

    /*
     * This function will register the observable only for Console to device
     * controller This function added to give support Tizen Console
     */
    public boolean registerConsoleObserverable(IDeviceController controller) {
        logger.info("registerConsoleObserverable");

        TizenConsoleManager tizenConsoleManger = (TizenConsoleManager) controller
                .getConsoleManager();
        tizenConsoleManger.getConsoleLogManager().addObserver(logObserverID,
                this);

        logger.info("registerConsoleObserverable DONE");

        return true;
    }

    @Override
    public boolean unregisterObserverable(IDeviceController controller) {
        try {
            logOutStream.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        controller.getLogManager().removeObserver(logObserverID, this);

        return true;
    }

    private void writeFile(String log) {
        if (makeFile) {
            try {
                logOutStream.write(log.getBytes(), 0, log.getBytes().length);
                logOutStream.flush();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void update(String log) {
        if (isEnabled) {
            //synchronized (this) {
                if ((pattern == null || pattern.isEmpty())) {
                    logBuf.append(log);
                    writeFile(log);
                    return;
                }
                if (log.contains(pattern)) {
                    logBuf.append(log);
                    writeFile(log);
                }
            //}
        }
    }
}
