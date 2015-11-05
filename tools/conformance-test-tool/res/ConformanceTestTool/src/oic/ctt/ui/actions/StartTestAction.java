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

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.lang.reflect.InvocationTargetException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

import oic.ctt.ui.Activator;
import oic.ctt.ui.Perspective;
import oic.ctt.ui.UIConst;
import oic.ctt.ui.log.LogHandler;
import oic.ctt.ui.multipages.TestPlanMultiPageEditor;
import oic.ctt.ui.multipages.TestPlanSpecPage;
import oic.ctt.ui.views.LogView;

import org.apache.commons.exec.CommandLine;
import org.apache.commons.exec.DefaultExecutor;
import org.apache.commons.exec.ExecuteException;
import org.apache.commons.exec.PumpStreamHandler;
import org.apache.commons.io.output.ByteArrayOutputStream;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.IJobChangeListener;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.plugin.AbstractUIPlugin;

public class StartTestAction extends Action implements ISelectionListener,
        IWorkbenchAction {
    public final static String     ID                = "oic.ctt.ui.actions.StartTestAction";

    Shell                          shell             = PlatformUI
                                                             .getWorkbench()
                                                             .getActiveWorkbenchWindow()
                                                             .getShell();

    private static Thread          consoleThread;
    private String                 planName;
    private TestPlanSpecPage       specpage;
    public static File             currentResultDir;
    private static int             testcaseIndex;

    private IWorkbenchWindow       workbenchwindow;
    private IWorkbenchPage         page;
    private ActionContributionItem startTest         = null;
    private ActionContributionItem stopTest          = null;
    private ActionContributionItem pauseTest         = null;
    private boolean                testInprogress    = false;
    private static LogView         mLogView;

    static DefaultExecutor         executor          = null;
    ProgressMonitorDialog          htmlMonitorDialog = null;

    private Action                 startConnTestAction;

    public StartTestAction(IWorkbenchWindow window) {
        // TODO Auto-generated constructor stub
        super(UIConst.TOOLBAR_TEXT_START);
        this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
                Activator.PLUGIN_ID, "icons/run.png"));
        this.setToolTipText("Start Test");
        testcaseIndex = 0;

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
        if (currentPerspective.equals(Perspective.ID)) {
            runTestPlan();
        } else {
            startConnTestAction.run();
        }

    }

    public void runTestPlan() {
        testInprogress = false;
        boolean zeroChecked = false;

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
                System.out.println("Not Test Plan Page.");
            }
        } else {
            System.out.println("Not exist Test Plan.");
            return;
        }

        String[] libraryPath = { "bin/jar", "libs", "" };
        String libraryClasspathCommand = "";

        for (int i = 0; i < libraryPath.length; i++) {
            libraryClasspathCommand += UIConst.PROJECT_PATH + libraryPath[i]
                    + "/*";
            if (i < libraryPath.length - 1) {
                libraryClasspathCommand += ":";
            }
        }

        // Get DB data
        Statement stat = null;
        Connection conn = null;
        String testCase = "";
        String testSuite = "";
        String prevTestSuite = "";
        String testTargetCommand = "";
        ResultSet checkdInfo = null;
        ResultSet countRow = null;

        try {
            Class.forName("org.sqlite.JDBC");
            System.out.println("jdbc:sqlite:" + UIConst.ROOT_PATH
                    + UIConst.TESTPLAN_PATH + "/" + planName + "/" + planName
                    + ".db");
            conn = DriverManager.getConnection("jdbc:sqlite:"
                    + UIConst.ROOT_PATH + UIConst.TESTPLAN_PATH + "/"
                    + planName + "/" + planName + ".db");
            stat = conn.createStatement();

            countRow = stat
                    .executeQuery("select count(*) from tcinfo where checked = 'true'");
            System.out.println(countRow.getInt("count(*)"));
            if (countRow.getInt("count(*)") == 0) {
                zeroChecked = true;
            }
            if (zeroChecked == false) {
                checkdInfo = stat
                        .executeQuery("select * from tcinfo where checked = 'true'");
                testcaseIndex = 0;
                while (checkdInfo.next()) {
                    testSuite = checkdInfo.getString("testsuite");
                    testSuite = testSuite.split(UIConst.ROBOT_EXT)[0];
                    testCase = checkdInfo.getString("testcase_fullname").trim();
                    testCase = testCase.replaceAll("\"", "\\\\\"");
                    if (testSuite.equals(prevTestSuite) == false) {
                        testTargetCommand += " -s " + "\"" + testSuite
                                + "\" -t " + "\"" + testCase + "\"";
                    } else {
                        testTargetCommand += " -t " + "\"" + testCase + "\"";
                    }
                    prevTestSuite = testSuite;
                    testcaseIndex++;
                }
                testTargetCommand += " " + "\"" + UIConst.ROOT_PATH
                        + UIConst.TESTPLAN_PATH + "/" + planName + "\"";
            }

        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (checkdInfo != null) {
                    checkdInfo.close();
                }
                if (stat != null) {
                    stat.close();
                }
                if (conn != null) {
                    conn.close();
                }
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
        if (zeroChecked == false) {
            DateFormat date = new SimpleDateFormat("yyyyMMdd-HHmmss_");
            String currentTime = date.format(System.currentTimeMillis());
            String resultDir = UIConst.ROOT_PATH + UIConst.TESTREPORT_PATH
                    + "/" + currentTime + planName;
            currentResultDir = new File(resultDir);
            if (!currentResultDir.exists()) {
                currentResultDir.mkdir();
            }
            String jybotCommand = " jybot --listener oic.ctt.logmanager.collector.RobotLogConsumer -d "
                    + "\""
                    + currentResultDir.getAbsolutePath()
                    + "\""
                    + " -b "
                    + UIConst.DEBUG_FILE_NAME + " -C auto";
            String jythonPath = " JYTHONPATH=" + UIConst.PROJECT_PATH + "libs/"
                    + ":" + UIConst.ROOT_PATH + "testsuite/" + ":"
                    + UIConst.PROJECT_PATH + ":";

            String command = "CLASSPATH=$CLASSPATH:" + libraryClasspathCommand
                    + jythonPath + jybotCommand + testTargetCommand;

            System.out.println(command);

            FileWriter fw = null;

            try {
                File file = new File(currentResultDir, "testCommand.sh");

                // if file doesn't exists, then create it
                if (!file.exists()) {
                    file.createNewFile();
                }

                fw = new FileWriter(file.getAbsoluteFile());
                BufferedWriter bw = new BufferedWriter(fw);
                bw.write(command);

                if (!file.canExecute()) {
                    file.setExecutable(true);
                }
                bw.close();

                System.out.println("Done");
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
            executeTestCommand(command);
        } else {
            MessageDialog dialog1 = new MessageDialog(
                    PlatformUI.getWorkbench().getActiveWorkbenchWindow()
                            .getShell(),
                    "Error",
                    null,
                    "There is no selected TestCase. Please select at least one Testcase.",
                    MessageDialog.ERROR, new String[] { "OK" }, 0);
            dialog1.open();
        }
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
                System.out.println("[Debug] Debugfile was deleted.");
            } else {
                System.out.println("[Debug] Debugfile wasn't deleted.");
            }
        }
        try {
            debugLogFile.createNewFile();
        } catch (IOException e3) {
            e3.printStackTrace();
        }

        IWorkbenchPage page = PlatformUI.getWorkbench()
                .getActiveWorkbenchWindow().getActivePage();

        IRunnableWithProgress runnableHtmlProgress = new IRunnableWithProgress() {
            @Override
            public void run(IProgressMonitor monitor)
                    throws InvocationTargetException, InterruptedException {
                monitor.beginTask("Test Reporting...", IProgressMonitor.UNKNOWN);

                while (!monitor.isCanceled()) {
                    Thread.sleep(100);
                }
                monitor.done();
            }

        };

        consoleThread = new Thread() {
            public void run() {

                IJobChangeListener progressListner = new IJobChangeListener() {

                    @Override
                    public void sleeping(IJobChangeEvent arg0) {
                    }

                    @Override
                    public void scheduled(IJobChangeEvent arg0) {
                    }

                    @Override
                    public void running(IJobChangeEvent arg0) {
                    }

                    @Override
                    public void done(IJobChangeEvent arg0) {
                        Display.getDefault().asyncExec(new Runnable() {

                            @Override
                            public void run() {
                                try {
                                    htmlMonitorDialog = new ProgressMonitorDialog(
                                            shell);
                                    htmlMonitorDialog.run(true, true,
                                            runnableHtmlProgress);

                                } catch (InvocationTargetException e) {
                                    e.printStackTrace();
                                } catch (InterruptedException e) {
                                    e.printStackTrace();
                                }
                            }
                        });

                    }

                    @Override
                    public void awake(IJobChangeEvent arg0) {
                    }

                    @Override
                    public void aboutToRun(IJobChangeEvent arg0) {
                    }
                };
                progress.addJobChangeListener(progressListner);
                progress.schedule();

                Display.getDefault().syncExec(new Runnable() {
                    @Override
                    public void run() {
                        boolean findAction = false;
                        Menu menubar = workbenchwindow.getShell().getMenuBar();
                        MenuItem[] mItems = menubar.getItems();
                        for (MenuItem mitem : mItems) {
                            if (mitem.getText().equals("&Run")
                                    || mitem.getText().equals("Run")) {
                                Menu menu = mitem.getMenu();

                                for (MenuItem menuItem : menu.getItems()) {
                                    if (menuItem.getText().equals(
                                            UIConst.TOOLBAR_TEXT_START)) {
                                        startTest = (ActionContributionItem) menuItem
                                                .getData();
                                        startTest.getAction().setEnabled(false);
                                    }
                                    if (menuItem.getText().equals(
                                            UIConst.TOOLBAR_TEXT_STOP)) {
                                        stopTest = (ActionContributionItem) menuItem
                                                .getData();
                                        stopTest.getAction().setEnabled(true);
                                    }
                                    if (menuItem.getText().equals(
                                            UIConst.TOOLBAR_TEXT_PAUSERESUME)) {
                                        pauseTest = (ActionContributionItem) menuItem
                                                .getData();
                                        pauseTest.getAction().setEnabled(true);
                                        findAction = true;
                                        break;
                                    }
                                }
                                if (findAction) {
                                    break;
                                }
                            }
                        }
                    }
                });

                executor = new DefaultExecutor();
                ByteArrayOutputStream baos = new ByteArrayOutputStream();
                PumpStreamHandler streamHandler = new PumpStreamHandler(baos);
                executor.setStreamHandler(streamHandler);
                CommandLine commandLine = new CommandLine(
                        currentResultDir.getAbsolutePath() + "/testCommand.sh");
                try {
                    executor.setExitValues(new int[] { 0, 1, 2, 3, 4, 5, 6, 7,
                            8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                            21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
                            34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
                            47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
                            60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72,
                            73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
                            86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98,
                            99, 100, -559038737 });
                    int exitCode = executor.execute(commandLine);

                } catch (ExecuteException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }

                try {
                    progress.join();
                    progress.removeJobChangeListener(progressListner);

                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                Display.getDefault().syncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (htmlMonitorDialog != null) {
                            htmlMonitorDialog.getProgressMonitor().setCanceled(
                                    true);
                        }

                    }
                });

                openBrowser();
            }

        };
        consoleThread.start();

    }

    public Job progress = new Job("Test Initializing...") {

                            boolean testCancel = false;

                            @Override
                            protected void canceling() {
                                super.canceling();
                                super.setName("Test Stopping...");
                                testCancel = true;
                                consoleThread.interrupt();
                                System.out.println("Test Stopped.");
                            }

                            String         line             = "";
                            String         tempLine         = "";
                            String         currentSuiteName = "";
                            int            currentPbarID    = 0;

                            IWorkbenchPage page             = PlatformUI
                                                                    .getWorkbench()
                                                                    .getActiveWorkbenchWindow()
                                                                    .getActivePage();

                            @Override
                            protected IStatus run(IProgressMonitor monitor) {
                                int result = 0;

                                currentPbarID = 0;
                                Boolean endFlag = false;
                                String progressStr = "";

                                specpage.setInitStatus();

                                final File file = new File(currentResultDir,
                                        UIConst.DEBUG_FILE_NAME);

                                // file exist
                                if (!file.exists()) {
                                    System.out.println("not exist : "
                                            + file.toString());
                                    return Status.CANCEL_STATUS;
                                }

                                System.out.println("file is : "
                                        + file.toString());
                                FileInputStream fis = null;
                                InputStreamReader isr = null;
                                BufferedReader reader = null;

                                FileWriter fwGraph = null;
                                BufferedWriter bwGraph = null;

                                try {

                                    fis = new FileInputStream(file);
                                    isr = new InputStreamReader(fis, "UTF-8");
                                    reader = new BufferedReader(isr, 1024);
                                    super.setName("Test Initializing...");
                                    monitor.beginTask("Load Libraries",
                                            testcaseIndex);

                                    testCancel = false;

                                    while (true) {
                                        // create Graph data file.
                                        File fileGraph = new File(
                                                currentResultDir,
                                                UIConst.FLOWGRAPH_FILE_NAME);

                                        if (!fileGraph.exists()) {
                                            fileGraph.createNewFile();
                                        }
                                        fwGraph = new FileWriter(
                                                fileGraph.getAbsoluteFile(),
                                                true);
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
                                            if (line.contains("START TEST:")) {
                                                bwGraph.write(line + "\n");

                                                result++;

                                                String strTime = line
                                                        .substring(0, 21);
                                                long longTime = getTimestamp(strTime);

                                                String tcFullName = line
                                                        .substring(
                                                                line.indexOf("TEST: ") + 6,
                                                                line.lastIndexOf(" ["));
                                                super.setName((currentPbarID + 1)
                                                        + "/"
                                                        + testcaseIndex
                                                        + " " + tcFullName);
                                                progressStr = "";

                                                String tcName = "";
                                                if (tcFullName.contains("]")) {
                                                    tcName = tcFullName
                                                            .split("\\]")[1]
                                                            .trim();
                                                } else {
                                                    tcName = tcFullName;
                                                }
                                                specpage.setStatusOnRuntime(
                                                        currentSuiteName,
                                                        tcName,
                                                        UIConst.STATUS_IN_PROGRESS);
                                            } else if (line
                                                    .contains("END TEST:")) {
                                                bwGraph.write(line + "\n");

                                                tempLine = line
                                                        .split("END TEST: ")[1];
                                                String tcFullName = null;
                                                if (tempLine != null) {
                                                    tempLine = tempLine.trim();
                                                    if (tempLine
                                                            .contains("- PASS (")
                                                            || tempLine
                                                                    .contains("- FAIL (")) {
                                                        tcFullName = tempLine
                                                                .substring(
                                                                        0,
                                                                        tempLine.lastIndexOf("-") - 1);
                                                    } else {
                                                        tcFullName = tempLine
                                                                .substring(
                                                                        0,
                                                                        tempLine.lastIndexOf("(") - 1);
                                                    }
                                                    String tcName = "";
                                                    if (tcFullName
                                                            .contains("]")) {
                                                        tcName = tcFullName
                                                                .split("\\]")[1]
                                                                .trim();
                                                    } else {
                                                        tcName = tcFullName;
                                                    }

                                                    String strTime = line
                                                            .substring(0, 21);
                                                    long longTime = getTimestamp(strTime);

                                                    String tcPassFail = tempLine
                                                            .substring(
                                                                    tempLine.lastIndexOf("-") + 2,
                                                                    tempLine.lastIndexOf("(") - 1);
                                                    if (tcPassFail
                                                            .equals("PASS")) {
                                                        specpage.setStatusOnRuntime(
                                                                currentSuiteName,
                                                                tcName,
                                                                UIConst.STATUS_PASS);
                                                    } else if (tcPassFail
                                                            .equals("FAIL")) {
                                                        specpage.setStatusOnRuntime(
                                                                currentSuiteName,
                                                                tcName,
                                                                UIConst.STATUS_FAIL);
                                                    } else {

                                                        specpage.setStatusOnRuntime(
                                                                currentSuiteName,
                                                                tcName,
                                                                UIConst.STATUS_DONE);
                                                    }

                                                    currentPbarID++;
                                                }
                                                monitor.worked(1);
                                                Thread.sleep(10);
                                            } else if (line
                                                    .contains("START SUITE:")) {
                                                String tempLine = line
                                                        .split("START SUITE: ")[1]
                                                        .split(" \\[")[0];
                                                if (tempLine.contains(".")) {
                                                    currentSuiteName = tempLine
                                                            .split("\\.")[1];
                                                }
                                            } else if (line
                                                    .contains("END SUITE:")) {
                                                if (line.contains("INFO - + END SUITE:")) {
                                                    endFlag = true;
                                                }
                                                progressStr = "";
                                                currentSuiteName = "";
                                            } else if (line.contains("- @@")) {
                                                bwGraph.write(line + "\n");
                                                String newStr = line
                                                        .replaceAll("\\\\n",
                                                                "\n");
                                                String[] parsedLine = newStr
                                                        .split("@@");
                                                String strTime = parsedLine[0]
                                                        .substring(0, 21);
                                                String direction = parsedLine[1]
                                                        .substring(0, 2);
                                                String lifeLine = parsedLine[1]
                                                        .substring(2).trim();
                                                String message = parsedLine[2]
                                                        .trim();

                                                long longTime = getTimestamp(strTime);
                                                String startLifeline = "TEST FRAMEWORK";

                                            } else if (line
                                                    .contains("START SETUP:")) {
                                                bwGraph.write(line + "\n");

                                                String strTime = line
                                                        .substring(0, 21);
                                                long longTime = getTimestamp(strTime);
                                                String setupName = line
                                                        .substring(
                                                                line.indexOf("SETUP: ") + 7,
                                                                line.lastIndexOf(" ["));

                                                super.setName("TEST SETUP...");
                                            } else if (line
                                                    .contains("END SETUP:")) {
                                                bwGraph.write(line + "\n");
                                                tempLine = line
                                                        .split("END SETUP: ")[1];
                                                String strTime = line
                                                        .substring(0, 21);
                                                long longTime = getTimestamp(strTime);
                                                tempLine = tempLine.trim();

                                                String setupName = null;
                                                if (tempLine
                                                        .contains("- PASS (")
                                                        || tempLine
                                                                .contains("- FAIL (")) {
                                                    setupName = tempLine
                                                            .substring(
                                                                    0,
                                                                    tempLine.lastIndexOf("-") - 1);
                                                } else {
                                                    setupName = tempLine
                                                            .substring(
                                                                    0,
                                                                    tempLine.lastIndexOf("(") - 1);
                                                }

                                                String tcPassFail = line
                                                        .substring(
                                                                line.lastIndexOf("-") + 2,
                                                                line.lastIndexOf("(") - 1);

                                            } else if (line
                                                    .contains("START TEARDOWN:")) {
                                                bwGraph.write(line + "\n");

                                                String strTime = line
                                                        .substring(0, 21);
                                                long longTime = getTimestamp(strTime);
                                                String teardownName = line
                                                        .substring(
                                                                line.indexOf("TEARDOWN: ") + 10,
                                                                line.lastIndexOf(" ["));

                                                super.setName("TEST TEARDOWN...");
                                            } else if (line
                                                    .contains("END TEARDOWN:")) {
                                                bwGraph.write(line + "\n");
                                                tempLine = line
                                                        .split("END TEARDOWN: ")[1];
                                                String strTime = line
                                                        .substring(0, 21);
                                                long longTime = getTimestamp(strTime);
                                                tempLine = tempLine.trim();

                                                String teardownName = null;
                                                if (tempLine
                                                        .contains("- PASS (")
                                                        || tempLine
                                                                .contains("- FAIL (")) {
                                                    teardownName = tempLine
                                                            .substring(
                                                                    0,
                                                                    tempLine.lastIndexOf("-") - 1);
                                                } else {
                                                    teardownName = tempLine
                                                            .substring(
                                                                    0,
                                                                    tempLine.lastIndexOf("(") - 1);
                                                }

                                                String tcPassFail = line
                                                        .substring(
                                                                line.lastIndexOf("-") + 2,
                                                                line.lastIndexOf("(") - 1);
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
                                        monitor.setTaskName("Canceled");
                                        monitor.setCanceled(true);
                                    } else {
                                        monitor.setTaskName("Finshed");
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

                                System.out.println("tc sum is : " + result);
                                Display.getDefault().syncExec(new Runnable() {
                                    @Override
                                    public void run() {
                                        if (stopTest != null) {
                                            stopTest.getAction().setEnabled(
                                                    false);
                                        }
                                        if (pauseTest != null) {
                                            pauseTest.getAction()
                                                    .setToolTipText(
                                                            "Pause Test ");
                                            pauseTest
                                                    .getAction()
                                                    .setImageDescriptor(
                                                            AbstractUIPlugin
                                                                    .imageDescriptorFromPlugin(
                                                                            Activator.PLUGIN_ID,
                                                                            "icons/pause.png"));
                                            pauseTest.getAction().setEnabled(
                                                    false);
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

                        };

    void openBrowser() {
        if (testInprogress) {
            Display.getDefault().syncExec(new Runnable() {
                @Override
                public void run() {
                    IWorkspace workspace = ResourcesPlugin.getWorkspace();
                    IWorkspaceRoot root = workspace.getRoot();
                    final IProject newProjectHandle = root
                            .getProject(UIConst.TESTREPORT_PATH);
                    IFile ifile = newProjectHandle.getFile(currentResultDir
                            .getName() + UIConst.REPORT_FILE_NAME);
                    System.out.println(ifile.getFullPath());
                    IWorkbench workbench = PlatformUI.getWorkbench();
                    IWorkbenchWindow window = workbench
                            .getActiveWorkbenchWindow();
                    IWorkbenchPage page = window.getActivePage();

                    try {
                        Thread.sleep(500);
                        IDE.openEditor(page, ifile,
                                "org.eclipse.ui.browser.editor");
                    } catch (PartInitException | InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            });
        }
    }

    public String getPlanName() {
        return planName;
    }

    public static File getFlowGraphFile() {

        boolean check = false;

        Job[] jobs = Job.getJobManager().find(null);
        for (Job job : jobs) {
            if (job.getClass().toString()
                    .startsWith("class oic.ctt.ui.actions.StartTestAction")) {
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
