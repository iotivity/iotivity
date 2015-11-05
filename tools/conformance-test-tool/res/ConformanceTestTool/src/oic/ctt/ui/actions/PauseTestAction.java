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
import java.io.IOException;
import java.io.InputStreamReader;

import oic.ctt.ui.Activator;
import oic.ctt.ui.UIConst;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.plugin.AbstractUIPlugin;

public class PauseTestAction extends Action implements ISelectionListener,
        IWorkbenchAction {
    public final static String     ID        = "oic.ctt.ui.actions.StopTestAction";
    private IWorkbenchWindow       workbenchwindow;
    private ActionContributionItem pauseTest = null;
    private ActionContributionItem stopTest  = null;

    public PauseTestAction(IWorkbenchWindow window) {
        super(UIConst.TOOLBAR_TEXT_PAUSERESUME);
        this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
                Activator.PLUGIN_ID, "icons/pause.png"));
        this.setToolTipText("Pause Test ");
        this.setEnabled(false);
        workbenchwindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
    }

    @Override
    public void dispose() {
    }

    @Override
    public void selectionChanged(IWorkbenchPart part, ISelection selection) {
    }

    public void run() {
        Display.getDefault().syncExec(new Runnable() {
            @Override
            public void run() {
                boolean findAction = false;
                Menu menubar = workbenchwindow.getShell().getMenuBar();
                MenuItem[] mItems = menubar.getItems();
                int shellPid = StartTestAction.getShellPID();

                String processStat = null;
                for (MenuItem mitem : mItems) {
                    if (mitem.getText().equals("&Run")
                            || mitem.getText().equals("Run")) {
                        Menu menu = mitem.getMenu();

                        for (MenuItem menuItem : menu.getItems()) {
                            if (menuItem.getText().equals(
                                    UIConst.TOOLBAR_TEXT_STOP)) {
                                stopTest = (ActionContributionItem) menuItem
                                        .getData();
                            }
                            if (menuItem.getText().equals(
                                    UIConst.TOOLBAR_TEXT_PAUSERESUME)) {
                                pauseTest = (ActionContributionItem) menuItem
                                        .getData();
                                Process pidProcess = null;
                                try {
                                    if (pauseTest.getAction().getToolTipText() == "Pause Test ") {
                                        pidProcess = Runtime
                                                .getRuntime()
                                                .exec(new String[] { "/bin/sh",
                                                        "-c",
                                                        "pgrep -P " + shellPid });
                                        String childPID = new BufferedReader(
                                                new InputStreamReader(
                                                        pidProcess
                                                                .getInputStream(),
                                                        "UTF-8"), 1024)
                                                .readLine();
                                        pidProcess = Runtime
                                                .getRuntime()
                                                .exec(new String[] { "/bin/sh",
                                                        "-c",
                                                        "pgrep -P " + childPID });
                                        String grandchildPID = new BufferedReader(
                                                new InputStreamReader(
                                                        pidProcess
                                                                .getInputStream(),
                                                        "UTF-8"), 1024)
                                                .readLine();
                                        if (grandchildPID != null) {
                                            pidProcess = Runtime
                                                    .getRuntime()
                                                    .exec(new String[] {
                                                            "/bin/sh",
                                                            "-c",
                                                            "kill -19 "
                                                                    + grandchildPID });
                                            Thread.sleep(100);
                                            pidProcess = Runtime
                                                    .getRuntime()
                                                    .exec(new String[] {
                                                            "/bin/sh",
                                                            "-c",
                                                            "ps "
                                                                    + grandchildPID
                                                                    + "| awk END'{print $3}'" });
                                            processStat = new BufferedReader(
                                                    new InputStreamReader(
                                                            pidProcess
                                                                    .getInputStream(),
                                                            "UTF-8"), 1024)
                                                    .readLine();
                                        } else {
                                            pidProcess = Runtime
                                                    .getRuntime()
                                                    .exec(new String[] {
                                                            "/bin/sh",
                                                            "-c",
                                                            "ps -o command -p"
                                                                    + childPID
                                                                    + " | awk END'{print}'" });
                                            String line_findJava = new BufferedReader(
                                                    new InputStreamReader(
                                                            pidProcess
                                                                    .getInputStream(),
                                                            "UTF-8"), 1024)
                                                    .readLine();
                                            if (line_findJava != null) {
                                                if (line_findJava
                                                        .contains("jython.jar")) {
                                                    pidProcess = Runtime
                                                            .getRuntime()
                                                            .exec(new String[] {
                                                                    "/bin/sh",
                                                                    "-c",
                                                                    "pkill -19 -P "
                                                                            + shellPid });
                                                    Thread.sleep(100);
                                                    pidProcess = Runtime
                                                            .getRuntime()
                                                            .exec(new String[] {
                                                                    "/bin/sh",
                                                                    "-c",
                                                                    "ps "
                                                                            + childPID
                                                                            + "| awk END'{print $3}'" });
                                                    processStat = new BufferedReader(
                                                            new InputStreamReader(
                                                                    pidProcess
                                                                            .getInputStream(),
                                                                    "UTF-8"),
                                                            1024).readLine();
                                                }
                                            }
                                        }
                                        if (processStat != null) {
                                            if (processStat.contains("T")) {
                                                pauseTest
                                                        .getAction()
                                                        .setImageDescriptor(
                                                                AbstractUIPlugin
                                                                        .imageDescriptorFromPlugin(
                                                                                Activator.PLUGIN_ID,
                                                                                "icons/resume.png"));
                                                pauseTest.getAction()
                                                        .setToolTipText(
                                                                "Resume Test ");
                                                stopTest.getAction()
                                                        .setEnabled(false);
                                            } else {
                                                System.out
                                                        .println("[Debug] Process didn't pause.");
                                            }
                                        } else {
                                            System.out
                                                    .println("[Debug] Process is null.");
                                        }
                                    } else {
                                        pidProcess = Runtime
                                                .getRuntime()
                                                .exec(new String[] { "/bin/sh",
                                                        "-c",
                                                        "pgrep -P " + shellPid });
                                        String childPID = new BufferedReader(
                                                new InputStreamReader(
                                                        pidProcess
                                                                .getInputStream(),
                                                        "UTF-8"), 1024)
                                                .readLine();
                                        pidProcess = Runtime
                                                .getRuntime()
                                                .exec(new String[] { "/bin/sh",
                                                        "-c",
                                                        "pgrep -P " + childPID });
                                        String grandchildPID = new BufferedReader(
                                                new InputStreamReader(
                                                        pidProcess
                                                                .getInputStream(),
                                                        "UTF-8"), 1024)
                                                .readLine();
                                        if (grandchildPID != null) {
                                            pidProcess = Runtime
                                                    .getRuntime()
                                                    .exec(new String[] {
                                                            "/bin/sh",
                                                            "-c",
                                                            "kill -18 "
                                                                    + grandchildPID });
                                            Thread.sleep(100);
                                            pidProcess = Runtime
                                                    .getRuntime()
                                                    .exec(new String[] {
                                                            "/bin/sh",
                                                            "-c",
                                                            "ps "
                                                                    + grandchildPID
                                                                    + "| awk END'{print $3}'" });
                                            processStat = new BufferedReader(
                                                    new InputStreamReader(
                                                            pidProcess
                                                                    .getInputStream(),
                                                            "UTF-8"), 1024)
                                                    .readLine();
                                        } else {
                                            pidProcess = Runtime
                                                    .getRuntime()
                                                    .exec(new String[] {
                                                            "/bin/sh",
                                                            "-c",
                                                            "ps -o command -p"
                                                                    + childPID
                                                                    + " | awk END'{print}'" });
                                            String line_findJava = new BufferedReader(
                                                    new InputStreamReader(
                                                            pidProcess
                                                                    .getInputStream(),
                                                            "UTF-8"), 1024)
                                                    .readLine();
                                            if (line_findJava != null) {
                                                if (line_findJava
                                                        .contains("jython.jar")) {
                                                    pidProcess = Runtime
                                                            .getRuntime()
                                                            .exec(new String[] {
                                                                    "/bin/sh",
                                                                    "-c",
                                                                    "pkill -18 -P "
                                                                            + shellPid });
                                                    Thread.sleep(100);
                                                    pidProcess = Runtime
                                                            .getRuntime()
                                                            .exec(new String[] {
                                                                    "/bin/sh",
                                                                    "-c",
                                                                    "ps "
                                                                            + childPID
                                                                            + "| awk END'{print $3}'" });
                                                    processStat = new BufferedReader(
                                                            new InputStreamReader(
                                                                    pidProcess
                                                                            .getInputStream(),
                                                                    "UTF-8"),
                                                            1024).readLine();
                                                }
                                            }
                                        }
                                        if (processStat != null) {
                                            if (processStat.contains("S")) {
                                                pauseTest
                                                        .getAction()
                                                        .setImageDescriptor(
                                                                AbstractUIPlugin
                                                                        .imageDescriptorFromPlugin(
                                                                                Activator.PLUGIN_ID,
                                                                                "icons/pause.png"));
                                                pauseTest.getAction()
                                                        .setToolTipText(
                                                                "Pause Test ");
                                                stopTest.getAction()
                                                        .setEnabled(true);
                                            } else {
                                                System.out
                                                        .println("[Debug] Process didn't resume.");
                                            }
                                        } else {
                                            System.out
                                                    .println("[Debug] Process is null.");
                                        }
                                    }
                                    findAction = true;
                                    break;
                                } catch (IOException | InterruptedException e) {
                                    e.printStackTrace();
                                }
                            }
                        }
                        if (findAction) {
                            break;
                        }
                    }
                }
            }
        });
    }
}
