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
import oic.ctt.ui.Perspective;
import oic.ctt.ui.UIConst;

import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.plugin.AbstractUIPlugin;

public class StopTestAction extends Action implements ISelectionListener,
        IWorkbenchAction {
    public final static String     ID        = "oic.ctt.ui.actions.StopTestAction";
    private IWorkbenchWindow       workbenchwindow;
    private IWorkbenchPage         page;
    private ActionContributionItem pauseTest = null;
    private ActionContributionItem stopTest  = null;

    private Action                 stopConnTestAction;

    public StopTestAction(IWorkbenchWindow window) {
        super(UIConst.TOOLBAR_TEXT_STOP);
        this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
                Activator.PLUGIN_ID, "icons/stop.png"));
        this.setToolTipText("Stop Test");
        this.setEnabled(false);
        workbenchwindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        page = workbenchwindow.getActivePage();
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
            int shellPID = StartTestAction.getShellPID();

            Menu menubar = workbenchwindow.getShell().getMenuBar();
            MenuItem[] mItems = menubar.getItems();

            for (MenuItem mitem : mItems) {
                if (mitem.getText().equals("&Run")
                        || mitem.getText().equals("Run")) {
                    Menu menu = mitem.getMenu();

                    for (MenuItem menuItem : menu.getItems()) {
                        if (menuItem.getText()
                                .equals(UIConst.TOOLBAR_TEXT_STOP)) {
                            stopTest = (ActionContributionItem) menuItem
                                    .getData();
                            stopTest.getAction().setEnabled(false);
                        }
                        if (menuItem.getText().equals(
                                UIConst.TOOLBAR_TEXT_PAUSERESUME)) {
                            pauseTest = (ActionContributionItem) menuItem
                                    .getData();
                            pauseTest.getAction().setEnabled(false);
                        }
                    }
                }
            }
            try {
                Process pidKillProcess = null;
                pidKillProcess = Runtime.getRuntime()
                        .exec(new String[] { "/bin/sh", "-c",
                                "pgrep -P " + shellPID });
                String childPID = new BufferedReader(new InputStreamReader(
                        pidKillProcess.getInputStream(), "UTF-8"), 1024)
                        .readLine();
                pidKillProcess = Runtime.getRuntime()
                        .exec(new String[] { "/bin/sh", "-c",
                                "pgrep -P " + childPID });
                String grandchildPID = new BufferedReader(
                        new InputStreamReader(pidKillProcess.getInputStream(),
                                "UTF-8"), 1024).readLine();
                if (grandchildPID != null) {
                    pidKillProcess = Runtime.getRuntime().exec(
                            new String[] { "/bin/sh", "-c",
                                    "kill " + grandchildPID });
                } else {
                    pidKillProcess = Runtime.getRuntime().exec(
                            new String[] {
                                    "/bin/sh",
                                    "-c",
                                    "ps -o command -p " + childPID
                                            + " | awk END'{print}'" });

                    String line_findJava = new BufferedReader(
                            new InputStreamReader(
                                    pidKillProcess.getInputStream(), "UTF-8"),
                            1024).readLine();
                    if (line_findJava != null) {
                        if (line_findJava.contains("jython.jar")) {
                            pidKillProcess = Runtime.getRuntime().exec(
                                    new String[] { "/bin/sh", "-c",
                                            "kill" + childPID });
                        }
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

            Job[] jobs = Job.getJobManager().find(null);
            for (Job job : jobs) {
                if (job.getClass().toString()
                        .startsWith("class oic.ctt.ui.actions.StartTestAction")) {
                    job.cancel();
                    break;
                }
            }
        } else {
            stopConnTestAction.run();
        }
    }
}
