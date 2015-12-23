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
package oic.ctt.ui.actions;

import static oic.ctt.ui.actions.StartTestAction.*;
import static oic.ctt.ui.actions.ActionsConstants.UNICODE_TYPE;
import static oic.ctt.ui.types.IDType.PLUGIN_ID;
import static oic.ctt.ui.types.ImageFilePathType.IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_PAUSE_TEST;
import static oic.ctt.ui.types.ToolTipTextType.*;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

import oic.ctt.ui.UIConst;
import oic.ctt.ui.util.CTLogger;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.slf4j.Logger;

public class JobProgressThread extends Job {
    private Logger              logger             = CTLogger.getInstance();
    private static final String TASK_STOP          = "Test Stopping...";
    private static final String LINE_MATCHING_TEXT = "START TEST:";
    private static final String BEGIN_TASK         = "Load Libraries";
    private static final String TASK_FINSHED       = "Finshed";
    private static final String TASK_CANCELLED     = "Canceled";
    private static final String TEST_INITIALIZING  = "Test Initializing...";

    public JobProgressThread(String name) {
        super(name);
    }

    boolean testCancel       = false;
    String  line             = "";
    String  tempLine         = "";
    String  currentSuiteName = "";
    int     currentPbarID    = 0;

    @Override
    protected void canceling() {
        super.canceling();
        super.setName(TASK_STOP);
        testCancel = true;
        consoleThread.interrupt();
        logger.info("Console thread inturrupted--Test Stopped.");
    }

    @Override
    protected IStatus run(IProgressMonitor monitor) {
        int result = 0;

        currentPbarID = 0;
        Boolean endFlag = false;
        String progressStr = "";

        specpage.setInitStatus();

        final File file = new File(currentResultDir, UIConst.DEBUG_FILE_NAME);

        if (!file.exists()) {
            logger.info("File : " + file.toString() + " not exist");
            return Status.CANCEL_STATUS;
        }
        logger.info("File is : " + file.toString());
        FileInputStream fis = null;
        InputStreamReader isr = null;
        BufferedReader reader = null;

        FileWriter fwGraph = null;
        BufferedWriter bwGraph = null;

        try {

            fis = new FileInputStream(file);
            logger.info("fis initialized");
            isr = new InputStreamReader(fis, UNICODE_TYPE);
            logger.info("isr initialized");
            reader = new BufferedReader(isr, 1024);
            logger.info("reader initialized");
            super.setName(TEST_INITIALIZING);
            monitor.beginTask(BEGIN_TASK, testcaseIndex);

            testCancel = false;

            while (true) {
                // create Graph data file.
                File fileGraph = new File(currentResultDir,
                        UIConst.FLOWGRAPH_FILE_NAME);

                if (!fileGraph.exists()) {
                    fileGraph.createNewFile();
                }
                fwGraph = new FileWriter(fileGraph.getAbsoluteFile(), true);
                bwGraph = new BufferedWriter(fwGraph);

                if (testCancel) {
                    if (!testInprogress) {
                        break;
                    }
                }

                line = reader.readLine();
                tempLine = line;
                if (line != null) {
                    testInprogress = true;
                    if (line.contains(LINE_MATCHING_TEXT)) {
                        bwGraph.write(line + "\n");

                        result++;

                        String strTime = line.substring(0, 21);
                        getTimestamp(strTime);

                        String tcFullName = line.substring(
                                line.indexOf("TEST: ") + 6,
                                line.lastIndexOf(" ["));
                        super.setName((currentPbarID + 1) + "/" + testcaseIndex
                                + " " + tcFullName);
                        progressStr = "";

                        String tcName = "";
                        if (tcFullName.contains("]")) {
                            tcName = tcFullName.split("\\]")[1].trim();
                        } else {
                            tcName = tcFullName;
                        }
                        specpage.setStatusOnRuntime(currentSuiteName, tcName,
                                UIConst.STATUS_IN_PROGRESS);
                    } else if (line.contains("END TEST:")) {
                        bwGraph.write(line + "\n");

                        tempLine = line.split("END TEST: ")[1];
                        String tcFullName = null;
                        if (tempLine != null) {
                            tempLine = tempLine.trim();
                            if (tempLine.contains("- PASS (")
                                    || tempLine.contains("- FAIL (")) {
                                tcFullName = tempLine.substring(0,
                                        tempLine.lastIndexOf("-") - 1);
                            } else {
                                tcFullName = tempLine.substring(0,
                                        tempLine.lastIndexOf("(") - 1);
                            }
                            String tcName = "";
                            if (tcFullName.contains("]")) {
                                tcName = tcFullName.split("\\]")[1].trim();
                            } else {
                                tcName = tcFullName;
                            }

                            String tcPassFail = tempLine.substring(
                                    tempLine.lastIndexOf("-") + 2,
                                    tempLine.lastIndexOf("(") - 1);
                            if (tcPassFail.equals("PASS")) {
                                specpage.setStatusOnRuntime(currentSuiteName,
                                        tcName, UIConst.STATUS_PASS);
                            } else if (tcPassFail.equals("FAIL")) {
                                specpage.setStatusOnRuntime(currentSuiteName,
                                        tcName, UIConst.STATUS_FAIL);
                            } else {

                                specpage.setStatusOnRuntime(currentSuiteName,
                                        tcName, UIConst.STATUS_DONE);
                            }

                            currentPbarID++;
                        }
                        monitor.worked(1);
                        Thread.sleep(10);
                    } else if (line.contains("START SUITE:")) {
                        String tempLine = line.split("START SUITE: ")[1]
                                .split(" \\[")[0];
                        if (tempLine.contains(".")) {
                            currentSuiteName = tempLine.split("\\.")[1];
                        }
                    } else if (line.contains("END SUITE:")) {
                        if (line.contains("INFO - + END SUITE:")) {
                            endFlag = true;
                        }
                        progressStr = "";
                        currentSuiteName = "";
                    } else if (line.contains("- @@")) {
                        bwGraph.write(line + "\n");

                    } else if (line.contains("START SETUP:")) {
                        bwGraph.write(line + "\n");

                        super.setName("TEST SETUP...");
                    } else if (line.contains("END SETUP:")) {
                        bwGraph.write(line + "\n");
                        tempLine = line.split("END SETUP: ")[1];
                        tempLine = tempLine.trim();

                        if (tempLine.contains("- PASS (")
                                || tempLine.contains("- FAIL (")) {
                        } else {
                        }

                    } else if (line.contains("START TEARDOWN:")) {
                        bwGraph.write(line + "\n");

                        super.setName("TEST TEARDOWN...");
                    } else if (line.contains("END TEARDOWN:")) {
                        bwGraph.write(line + "\n");
                        tempLine = line.split("END TEARDOWN: ")[1];
                        tempLine = tempLine.trim();

                        if (tempLine.contains("- PASS (")
                                || tempLine.contains("- FAIL (")) {
                        }
                    }
                } else {
                    if (!fileGraph.canExecute()) {
                        fileGraph.setExecutable(true);
                    }
                    bwGraph.close();
                    fwGraph.close();

                    progressStr += "▶";
                    if (progressStr.length() > 10) {
                        progressStr = "";
                    }
                    monitor.subTask(progressStr);
                    if (endFlag) {
                        break;
                    } else {
                        Thread.sleep(500);
                    }
                }

                if (!fileGraph.canExecute()) {
                    fileGraph.setExecutable(true);
                }
                bwGraph.close();
                fwGraph.close();

            }

            if (testCancel) {
                monitor.setTaskName(TASK_CANCELLED);
                monitor.setCanceled(true);
            } else {
                monitor.setTaskName(TASK_FINSHED);
                monitor.done();
            }
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
            try {
                if (bwGraph != null) {
                    bwGraph.close();
                }
                if (fwGraph != null) {
                    fwGraph.close();
                }
            } catch (IOException e1) {
                e1.printStackTrace();
            }

        }

        try {
            fis.close();
            isr.close();
            reader.close();

        } catch (IOException e) {
            e.printStackTrace();
        }
        logger.info("tc sum is : " + result);
        Display.getDefault().syncExec(new Runnable() {
            @Override
            public void run() {
                if (stopTest != null) {
                    stopTest.getAction().setEnabled(false);
                }
                if (pauseTest != null) {
                    pauseTest.getAction().setToolTipText(
                            TOOLTIP_TEXT_PAUSE_TEST.toString());
                    pauseTest.getAction().setImageDescriptor(
                            AbstractUIPlugin.imageDescriptorFromPlugin(
                                    PLUGIN_ID.toString(),
                                    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_PAUSE_TEST
                                            .toString()));
                    pauseTest.getAction().setEnabled(false);
                }
            }
        });
        return Status.OK_STATUS;
    }

    private long getTimestamp(String time) {
        Date parsedDate = null;
        try {
            DateFormat dateFormat = new SimpleDateFormat(
                    "yyyyMMdd hh:mm:ss.SSS");
            parsedDate = dateFormat.parse(time);
            return parsedDate.getTime();

        } catch (ParseException e) {
            e.printStackTrace();
        }
        return 0;
    }

}
