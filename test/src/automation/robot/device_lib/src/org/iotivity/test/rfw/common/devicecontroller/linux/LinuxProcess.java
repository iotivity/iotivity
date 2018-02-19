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

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.logging.Logger;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObservable;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObserver;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IProcess;
import org.iotivity.test.rfw.common.util.IoTivityLogger;
import org.iotivity.test.rfw.common.util.SystemUtil;

public class LinuxProcess implements IProcess {
    private ProcessLogObservable processLogObservable;
    private Process              consoleProcess;
    private Thread               outPutThread;
    private InputStream          stdoutReader;
    private BufferedWriter       stdintBufferedWriter;
    private Logger               logger = IoTivityLogger.GetLogger();;

    public LinuxProcess(String processName, String cmd) {
        try {
            logger.info("CMD to processBuilder : " + cmd);

            ProcessBuilder procBuilder = new ProcessBuilder(cmd.split(" "));
            procBuilder.directory(new File(System.getProperty("user.dir")));
            procBuilder.redirectErrorStream(true);
            consoleProcess = procBuilder.start();

            SystemUtil.sleep(1000);

            stdoutReader = consoleProcess.getInputStream();
            stdintBufferedWriter = new BufferedWriter(
                    new OutputStreamWriter(consoleProcess.getOutputStream()));
            processLogObservable = new ProcessLogObservable();

            logger.info("Process " + processName + " With Command : " + cmd
                    + "Created successfully");
        } catch (IOException e) {
            e.printStackTrace();
            logger.info("Exception Occured " + e.toString());
        }
    }

    @Override
    public boolean isProcessRunning() {
        try {
            consoleProcess.exitValue();

            return false;
        } catch (IllegalThreadStateException e) {
            return true;
        }
    }

    @Override
    public boolean inputCommand(String command) {
        logger.info("command " + command);

        try {
            if (this.isProcessRunning()) {
                stdintBufferedWriter.write(command);
                stdintBufferedWriter.newLine();
                stdintBufferedWriter.flush();
                Thread.sleep(500);
                logger.info("Input Command done : " + command);
            } else
                return false;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    @Override
    public boolean destroy() {
        try {
            if (outPutThread != null) {
                while (outPutThread.isInterrupted() == false) {
                    int tryInterruptCount = 0;
                    outPutThread.interrupt();

                    try {
                        Thread.sleep(1000);
                        if (tryInterruptCount > 10)
                            break;
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }

            stdoutReader.close();
            stdintBufferedWriter.close();
            consoleProcess.destroy();

            return true;
        } catch (IOException e) {
            logger.info(
                    "Exception occured in destroying process " + e.toString());
            e.printStackTrace();

            return false;
        }
    }

    @Override
    public ILogObservable getObserverable() {
        return processLogObservable;
    }

    public class ProcessLogObservable implements ILogObservable {
        private ArrayList<ILogObserver> oberverList;

        public ProcessLogObservable() {
            this.oberverList = new ArrayList<ILogObserver>();
        }

        @Override
        public boolean addObserver(ILogObserver observer) {
            return oberverList.add(observer);
        }

        @Override
        public boolean removeObserver(ILogObserver observer) {
            return oberverList.remove(observer);
        }

        @Override
        public void notifyObervers() {
            if (outPutThread != null && outPutThread.isAlive())
                return;
            outPutThread = new Thread() {
                public void run() {
                    try {
                        BufferedReader in = new BufferedReader(
                                new InputStreamReader(stdoutReader, "UTF-8"));
                        for (;;) {
                            try {
                                String inputLine = in.readLine();
                                if (inputLine != null) {
                                    for (ILogObserver observer : oberverList) {
                                        observer.update(inputLine + "\n");
                                    }
                                    System.out.println(inputLine);
                                } else {
                                    logger.info(
                                            "Breaking reading line from input stream");
                                    break;
                                }
                            } catch (Exception e) {
                                logger.info(
                                        "Exception occured " + e.toString());
                                System.err.println(e.toString());
                                break;
                            }
                        }
                    } catch (Exception anExc) {
                        anExc.printStackTrace();
                    }
                }
            };
            outPutThread.start();
        }
    }
}
