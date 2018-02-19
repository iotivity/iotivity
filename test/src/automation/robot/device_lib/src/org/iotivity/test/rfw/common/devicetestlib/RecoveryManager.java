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

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.List;

import org.iotivity.test.rfw.common.devicecontroller.datamodel.RecoveryProcess;
import org.iotivity.test.rfw.common.devicecontroller.datamodel.RecoveryType;

public class RecoveryManager {

    private static RecoveryManager self;
    private List<RecoveryProcess>  processList;

    private Thread                 monitorThread;
    // private String adbShellCommand = "adb shell -s ";

    private RecoveryManager() {
        processList = new ArrayList<RecoveryProcess>();

        monitorThread = new Thread() {
            public void run() {
                while (true) {

                    for (RecoveryProcess proc : processList) {
                        if (!findProcess(proc)) {

                        }
                    }

                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        };
        monitorThread.start();
    }

    public boolean isLinuxProcessRunning(String process) {
        return true;
    }

    public boolean isAndroidProcessRunning(String process) {
        return true;
    }

    public boolean isTizenProcessRunning(String process) {
        return true;
    }

    public String runProcess(String command) {
        Process proc;
        try {
            ProcessBuilder procBuilder = new ProcessBuilder(command.split(" "));
            procBuilder.directory(new File(System.getProperty("user.dir")));
            procBuilder.redirectErrorStream(true);
            proc = procBuilder.start();
            BufferedReader stdoutBufferedReader = new BufferedReader(
                    new InputStreamReader(proc.getInputStream()));
            BufferedWriter stdinBufferedWriter = new BufferedWriter(
                    new OutputStreamWriter(proc.getOutputStream()));

            stdinBufferedWriter.write(command);
            stdinBufferedWriter.newLine();
            stdinBufferedWriter.flush();
            Thread.sleep(500);

            String line = "";
            String totalLine = "";
            while ((line = stdoutBufferedReader.readLine()) != null)
                totalLine += line;

            return totalLine;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    public static RecoveryManager getRecoveryManager() {
        if (self == null)
            self = new RecoveryManager();
        return self;
    }

    public boolean findProcess(RecoveryProcess process) {
        if (process.getType() == RecoveryType.Linux) {

        } else if (process.getType() == RecoveryType.Android) {

        } else if (process.getType() == RecoveryType.Tizen) {

        } else if (process.getType() == RecoveryType.TizenUI) {

        }

        return true;
    }

    public void addMonitorProcess(String processName, String command,
            String deviceId, RecoveryType type) {
        for (RecoveryProcess proc : processList) {
            if (proc.getProcessName().equals(processName))
                return;
        }

        RecoveryProcess newProcess = new RecoveryProcess();
        newProcess.setDeviceId(deviceId);
        newProcess.setProcessCommand(command);
        newProcess.setProcessName(processName);
        newProcess.setType(type);

        processList.add(newProcess);
    }

    public void removeMonitorProcess(String processName) {
        if (processList.isEmpty())
            return;

        int processIndex = -1;
        for (int i = 0; i < processList.size(); i++) {
            if (processList.get(i).getProcessName().equals(processName)) {
                processIndex = i;
                break;
            }
        }

        if (processIndex > -1)
            processList.remove(processIndex);
    }
}
