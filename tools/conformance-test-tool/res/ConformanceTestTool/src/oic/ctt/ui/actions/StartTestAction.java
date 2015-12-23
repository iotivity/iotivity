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

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.FilenameFilter;
import java.io.IOException;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;

import oic.ctt.ui.UIConst;
import oic.ctt.ui.log.LogHandler;
import oic.ctt.ui.multipages.TestPlanMultiPageEditor;
import oic.ctt.ui.multipages.TestPlanSpecPage;
import static oic.ctt.ui.types.ToolTipTextType.*;
import static oic.ctt.ui.types.ImageFilePathType.*;
import static oic.ctt.ui.types.IDType.*;
import oic.ctt.ui.util.CTLogger;
import static oic.ctt.ui.util.PopUpUtil.*;
import static oic.ctt.ui.util.DatabaseUtil.*;
import oic.ctt.ui.views.LogView;

import org.apache.commons.exec.DefaultExecutor;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.slf4j.Logger;

public class StartTestAction extends Action implements ISelectionListener,
        IWorkbenchAction {
    private static final String          TCINFO_TABLE_TESTCASE_COLUMN               = "testcase_fullname";
    private static final String          TCINFO_TABLE_TESTSUITE_COLUMN              = "testsuite";
    private static final String          DIALOG_MESSAGE                             = "There is no selected TestCase. Please select at least one Testcase.";
    private static final String          TEST_PLAN_EXISTENCE_MESSAGE                = "Test Plan not exist";
    private static final String          TEST_PLAN_PAGE_CHECKING_MESSAGE            = "Not Test Plan Page.";
    private static Logger                logger                                     = CTLogger
                                                                                            .getInstance();
    private static final String          SHELL_SCRIPT_NAME                          = "testCommand.sh";
    private static final String          DATE_FORMAT_1                              = "yyyyMMdd-HHmmss_";
    private static final String          CLASS_OIC_CTT_UI_ACTIONS_START_TEST_ACTION = "class oic.ctt.ui.actions.JobProgressThread";
    private static final String          JYBOT_COMMAND_STRING                       = " jybot --listener oic.ctt.logmanager.collector.RobotLogConsumer -d ";
    private static final String          QUERY_TO_GET_CHECKED_ROW_FROM_TCINFO_TABLE = "select * from tcinfo where checked = 'true'";
    private static final String          QUERY_TO_COUNT_CHECKED_ROW                 = "select count(*) from tcinfo where checked = 'true'";
    public static final Shell            SHELL                                      = PlatformUI
                                                                                            .getWorkbench()
                                                                                            .getActiveWorkbenchWindow()
                                                                                            .getShell();

    public static ConsoleThread          consoleThread;
    private String                       planName;
    public static TestPlanSpecPage       specpage;
    public static File                   currentResultDir;
    public static int                    testcaseIndex;

    public static IWorkbenchWindow       workbenchwindow;
    private IWorkbenchPage               page;
    public static ActionContributionItem startTest;
    public static ActionContributionItem stopTest;
    public static ActionContributionItem pauseTest;
    public static boolean                testInprogress                             = false;
    private static LogView               mLogView;

    public static DefaultExecutor        executor;
    public static ProgressMonitorDialog  htmlMonitorDialog;

    private Action                       startConnTestAction;
    public static IRunnableWithProgress  runnableHtmlProgress;

    public StartTestAction(IWorkbenchWindow window) {
        super(UIConst.TOOLBAR_TEXT_START);
        this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
                PLUGIN_ID.toString(),
                IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_START_TEST.toString()));
        this.setToolTipText(TOOLTIP_TEXT_START_TEST.toString());
        doInitialization();
    }

    private void doInitialization() {
        testcaseIndex = 0;
        currentResultDir = null;
        workbenchwindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        page = workbenchwindow.getActivePage();
    }

    public static int getShellPID() {
        int pid = Integer.parseInt(java.lang.management.ManagementFactory
                .getRuntimeMXBean().getName().split("@")[0]);
        return pid;
    }

    @Override
    public void dispose() {
    }

    @Override
    public void selectionChanged(IWorkbenchPart part, ISelection selection) {
    }

    public void run() {

        String currentPerspective = page.getPerspective().getId();
        if (currentPerspective.equals(PERSPECTIVE_ID.toString())) {
            runTestPlan();
        } else {
            startConnTestAction.run();
        }

    }

    public void runTestPlan() {
        testInprogress = false;
        String libraryClasspathCommand = "";
        boolean zeroChecked = false;
        setSpecPageAndPlanName();
        libraryClasspathCommand = setlibraryClasspathCommand();
        String testTargetCommand = "";
        Connection conn = null;
        ResultSet checkdInfo = null;
        ResultSet countRow = null;
        String jybotCommand = "";
        String jythonPath = "";
        String command = "";

        try {
            conn = createDBConnection(planName);
            countRow = retrieveQueryResult(conn, QUERY_TO_COUNT_CHECKED_ROW);
            logger.info("Checked TC Count : "
                    + Integer.toString(countRow.getInt("count(*)")));
            if (countRow.getInt("count(*)") == 0) {
                zeroChecked = true;
            }
            if (zeroChecked == false) {
                checkdInfo = retrieveQueryResult(conn,
                        QUERY_TO_GET_CHECKED_ROW_FROM_TCINFO_TABLE);
                testcaseIndex = 0;
                testTargetCommand = setTestTargetCommand(checkdInfo);
            }

        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            AutoCloseable[] closeableObjects = { checkdInfo, conn, countRow };
            closeDataBaseObjects(closeableObjects);
        }
        if (zeroChecked == false) {
            String currentTime = getCurrentTime();
            String resultDir = UIConst.ROOT_PATH + UIConst.TESTREPORT_PATH
                    + "/" + currentTime + planName;
            currentResultDir = new File(resultDir);
            if (!currentResultDir.exists()) {
                currentResultDir.mkdir();
            }
            jybotCommand = JYBOT_COMMAND_STRING + "\""
                    + currentResultDir.getAbsolutePath() + "\"" + " -b "
                    + UIConst.DEBUG_FILE_NAME + " -C auto";
            jythonPath = " JYTHONPATH=" + UIConst.PROJECT_PATH + "libs/" + ":"
                    + UIConst.ROOT_PATH + "testsuite/" + ":"
                    + UIConst.PROJECT_PATH + ":";

            command = "CLASSPATH=$CLASSPATH:" + libraryClasspathCommand
                    + jythonPath + jybotCommand + testTargetCommand;
            logger.info("Test execution command : " + command);
            writeTestExecutaionCommandToShellScript(command);
            executeTestCommand(command);
        } else {
            displayMessageDialog(ACTIVE_WORKBENCH_WINDOW_SHELL, ERROR_TEXT,
                    DIALOG_MESSAGE, MessageDialog.ERROR,
                    new String[] { DIALOG_BUTTON_LABEL_OK });
        }
    }

    /**
     * This method is used to write test execution command to shell script
     * 
     * @param command
     *            This is the test execution command
     * @return nothing
     */
    private void writeTestExecutaionCommandToShellScript(String command) {
        FileWriter fw = null;

        try {
            File file = new File(currentResultDir, SHELL_SCRIPT_NAME);

            if (!file.exists()) {
                logger.info("file doesn't exists, creating it it");
                file.createNewFile();
            }

            fw = new FileWriter(file.getAbsoluteFile());
            BufferedWriter bw = new BufferedWriter(fw);
            bw.write(command);

            if (!file.canExecute()) {
                file.setExecutable(true);
            }
            bw.close();
            logger.info("Successfully Write test execution command to testCommand.sh");
        } catch (IOException e) {
            e.printStackTrace();
            if (fw != null) {
                try {
                    fw.close();
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
            }
        }
    }

    /**
     * This method is used to get current time
     * 
     * @param nothing
     * @return currentTime
     */
    private String getCurrentTime() {
        DateFormat date = new SimpleDateFormat(DATE_FORMAT_1);
        String currentTime = date.format(System.currentTimeMillis());
        return currentTime;
    }

    /**
     * This method is used to set library class path command
     * 
     * @param nothing
     * @return libraryClasspathCommand This is the command used to set library
     *         path
     */
    private String setlibraryClasspathCommand() {
        String[] libraryPath = { "bin/jar", "libs", "" };
        String libraryClasspathCommand = "";
        for (int i = 0; i < libraryPath.length; i++) {
            libraryClasspathCommand += UIConst.PROJECT_PATH + libraryPath[i]
                    + "/*";
            if (i < libraryPath.length - 1) {
                libraryClasspathCommand += ":";
            }
        }
        return libraryClasspathCommand;
    }

    /**
     * This method is used to set test plan spec page and plan name
     * 
     * @param nothing
     * @return libraryClasspathCommand This is the command used to set library
     *         path
     */
    private void setSpecPageAndPlanName() {
        if (page.getActiveEditor() != null) {
            if (page.getActiveEditor() instanceof TestPlanMultiPageEditor) {
                TestPlanMultiPageEditor editor = (TestPlanMultiPageEditor) page
                        .getActiveEditor();
                if (editor != null) {
                    specpage = (TestPlanSpecPage) editor
                            .getActivePageInstance();
                    planName = editor.getPartName();
                }
            } else {
                logger.info(TEST_PLAN_PAGE_CHECKING_MESSAGE);
            }
        } else {
            logger.info(TEST_PLAN_EXISTENCE_MESSAGE);
            return;
        }
    }

    /**
     * This mehtod is used to set target test command
     * 
     * @param checkdInfo
     *            this is database table query result set
     * @return testTargetCommand this is target test command
     * @throws SQLException
     */
    private String setTestTargetCommand(ResultSet checkdInfo)
            throws SQLException {
        String testCase = "";
        String testSuite = "";
        String prevTestSuite = "";
        String testTargetCommand = "";

        while (checkdInfo.next()) {
            testSuite = checkdInfo.getString(TCINFO_TABLE_TESTSUITE_COLUMN);
            testSuite = testSuite.split(UIConst.ROBOT_EXT)[0];
            testCase = checkdInfo.getString(TCINFO_TABLE_TESTCASE_COLUMN)
                    .trim();
            testCase = testCase.replaceAll("\"", "\\\\\"");
            if (testSuite.equals(prevTestSuite) == false) {
                testTargetCommand += " -s " + "\"" + testSuite + "\" -t "
                        + "\"" + testCase + "\"";
            } else {
                testTargetCommand += " -t " + "\"" + testCase + "\"";
            }
            prevTestSuite = testSuite;
            testcaseIndex++;
        }
        testTargetCommand += " " + "\"" + UIConst.ROOT_PATH
                + UIConst.TESTPLAN_PATH + "/" + planName + "\"";
        return testTargetCommand;
    }

    public static class MyFileNameFilter implements FilenameFilter {

        private String ext;

        public MyFileNameFilter(String ext) {
            this.ext = ext.toLowerCase();
        }

        @Override
        public boolean accept(File dir, String name) {
            return name.toLowerCase().endsWith(ext);
        }

    }

    private void executeTestCommand(final String command) {
        logger.debug(" executeTestCommand started.");
        final File debugLogFile = new File(currentResultDir,
                UIConst.DEBUG_FILE_NAME);

        if (mLogView == null) {
            mLogView = (LogView) PlatformUI.getWorkbench()
                    .getActiveWorkbenchWindow().getActivePage()
                    .findView(LogView.ID);
        }

        Thread logViewThread = new Thread() {
            public void run() {
                LogHandler.startLogViewer(mLogView);
            }
        };
        logViewThread.start();

        if (debugLogFile.exists()) {
            if (debugLogFile.delete()) {
                logger.debug(" Debugfile was deleted.");
            } else {
                logger.debug(" Debugfile wasn't deleted.");
            }
        }
        try {
            debugLogFile.createNewFile();
        } catch (IOException e3) {
            e3.printStackTrace();
        }
        consoleThread = new ConsoleThread();
        consoleThread.start();
    }

    public String getPlanName() {
        return planName;
    }

    public static File getFlowGraphFile() {

        boolean check = false;

        Job[] jobs = Job.getJobManager().find(null);
        for (Job job : jobs) {
            if (job.getClass().toString()
                    .startsWith(CLASS_OIC_CTT_UI_ACTIONS_START_TEST_ACTION)) {
                check = true;
                break;
            }
        }

        if (check) {
            File fileGraph = new File(currentResultDir,
                    UIConst.FLOWGRAPH_FILE_NAME);

            if (fileGraph.exists()) {
                return fileGraph;
            } else {
                return null;
            }
        } else {
            return null;
        }
    }
}
