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

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;

import oic.ctt.ui.UIConst;
import oic.ctt.ui.util.CTLogger;
import static oic.ctt.ui.actions.StartTestAction.*;

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
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.IJobChangeListener;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.slf4j.Logger;

public class ConsoleThread extends Thread {
    private Logger              logger                         = CTLogger
                                                                       .getInstance();
    private static final String MENU_ITEM_ALTERNATE_TEXT       = "&Run";
    private static final String MENU_ITEM_TEXT                 = "Run";
    private static final String JOB_NAME                       = "Test Initializing...";
    private static final String EDITOR_ID                      = "org.eclipse.ui.browser.editor";
    private static final String TEST_COMMAND_SHELL_SCRIPT_NAME = "/testCommand.sh";
    private static final int[]  EXIT_VALUES_LIST               = new int[] { 0,
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
            20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
            37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
            54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
            71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87,
            88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, -559038737 };
    IRunnableWithProgress       runnableHtmlProgress;

    public ConsoleThread() {
        runnableHtmlProgress = new HtmlProgressThread();
    }

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
                            logger.info("htmlMonitorDialog started running");
                            htmlMonitorDialog = new ProgressMonitorDialog(SHELL);
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
        Job progress = new JobProgressThread(JOB_NAME);
        progress.addJobChangeListener(progressListner);
        progress.schedule();

        Display.getDefault().syncExec(new Runnable() {
            @Override
            public void run() {
                boolean findAction = false;
                Menu menubar = workbenchwindow.getShell().getMenuBar();
                MenuItem[] mItems = menubar.getItems();
                for (MenuItem mitem : mItems) {
                    if (mitem.getText().equals(MENU_ITEM_ALTERNATE_TEXT)
                            || mitem.getText().equals(MENU_ITEM_TEXT)) {
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
                currentResultDir.getAbsolutePath()
                        + TEST_COMMAND_SHELL_SCRIPT_NAME);
        try {
            executor.setExitValues(EXIT_VALUES_LIST);
            executor.execute(commandLine);

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
                    logger.debug("htmlMonitorDialog cancelled");
                    htmlMonitorDialog.getProgressMonitor().setCanceled(true);
                }

            }
        });
        openBrowser();
    }

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
                    logger.info(ifile.getFullPath().toString());
                    IWorkbench workbench = PlatformUI.getWorkbench();
                    IWorkbenchWindow window = workbench
                            .getActiveWorkbenchWindow();
                    IWorkbenchPage page = window.getActivePage();

                    try {
                        Thread.sleep(500);
                        IDE.openEditor(page, ifile, EDITOR_ID);
                    } catch (PartInitException | InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            });
        }
    }
}
