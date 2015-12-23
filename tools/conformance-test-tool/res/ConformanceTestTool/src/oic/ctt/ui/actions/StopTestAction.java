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

import oic.ctt.ui.Activator;
import oic.ctt.ui.UIConst;
import oic.ctt.ui.util.CTLogger;
import static oic.ctt.ui.types.ToolTipTextType.*;
import static oic.ctt.ui.types.ImageFilePathType.*;
import static oic.ctt.ui.types.IDType.*;
import static oic.ctt.ui.actions.ActionsConstants.*;

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
import org.slf4j.Logger;

public class StopTestAction extends Action implements ISelectionListener,
        IWorkbenchAction {
    private Logger                 logger                                       = CTLogger
                                                                                        .getInstance();
    private static final String    KILL                                         = "kill";
    private static final String    CLASS_OIC_CTT_UI_ACTIONS_JOB_PROGRESS_THREAD = "class oic.ctt.ui.actions.JobProgressThread";
    private IWorkbenchWindow       workbenchwindow;
    private IWorkbenchPage         page;
    private ActionContributionItem pauseTest                                    = null;
    private ActionContributionItem stopTest                                     = null;

    private Action                 stopConnTestAction;

    public StopTestAction(IWorkbenchWindow window) {
        super(UIConst.TOOLBAR_TEXT_STOP);
        this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
                Activator.PLUGIN_ID,
                IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_STOP_TEST.toString()));
        this.setToolTipText(TOOLTIP_TEXT_STOP_TEST.toString());
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
        if (currentPerspective.equals(PERSPECTIVE_ID.toString())) {
            int shellPID = StartTestAction.getShellPID();
            changeMenuItemsState();
            try {
                Process pidKillProcess = null;
                pidKillProcess = ActionsConstants.executeCommand(new String[] {
                        COMMAND_TYPE_SHELL, C, COMMAND_GREP + shellPID });
                String childPID = getProcessID(pidKillProcess);
                pidKillProcess = ActionsConstants.executeCommand(new String[] {
                        COMMAND_TYPE_SHELL, C, COMMAND_GREP + childPID });
                String grandchildPID = getProcessID(pidKillProcess);
                if (grandchildPID != null) {
                    pidKillProcess = ActionsConstants
                            .executeCommand(new String[] { COMMAND_TYPE_SHELL,
                                    C, COMMAND_KILL + grandchildPID });
                } else {
                    pidKillProcess = Runtime
                            .getRuntime()
                            .exec(new String[] { COMMAND_TYPE_SHELL, C,
                                    COMMAND_PROCESS + childPID + AWK_END_PRINT });

                    String line_findJava = getProcessID(pidKillProcess);
                    if (line_findJava != null) {
                        if (line_findJava.contains(JYTHON_JAR_FILE_NAME)) {
                            pidKillProcess = ActionsConstants
                                    .executeCommand(new String[] {
                                            COMMAND_TYPE_SHELL, C,
                                            KILL + childPID });
                        }
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
            cancelStartTestAction();
        } else {
            stopConnTestAction.run();
        }
    }

    /**
     * cancels a list of jobs
     */
    private void cancelStartTestAction() {
        Job[] jobs = Job.getJobManager().find(null);
        for (Job job : jobs) {
            if (job.getClass().toString()
                    .startsWith(CLASS_OIC_CTT_UI_ACTIONS_JOB_PROGRESS_THREAD)) {
                job.cancel();
                break;
            }
        }
    }

    /**
     * changes the state of a menu item control
     */
    private void changeMenuItemsState() {
        Menu menubar = workbenchwindow.getShell().getMenuBar();
        MenuItem[] mItems = menubar.getItems();

        for (MenuItem mitem : mItems) {
            if (mitem.getText().equals("&" + MENU_ITEM_RUN_TEXT)
                    || mitem.getText().equals(MENU_ITEM_RUN_TEXT)) {
                Menu menu = mitem.getMenu();

                for (MenuItem menuItem : menu.getItems()) {
                    if (menuItem.getText().equals(UIConst.TOOLBAR_TEXT_STOP)) {
                        stopTest = (ActionContributionItem) menuItem.getData();
                        stopTest.getAction().setEnabled(false);
                        logger.info("stopTest initialized");
                    }
                    if (menuItem.getText().equals(
                            UIConst.TOOLBAR_TEXT_PAUSERESUME)) {
                        pauseTest = (ActionContributionItem) menuItem.getData();
                        pauseTest.getAction().setEnabled(false);
                        logger.info("pause initialized");
                    }
                }
            }
        }
    }
}
