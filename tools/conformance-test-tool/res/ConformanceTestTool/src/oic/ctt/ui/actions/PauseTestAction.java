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
import java.io.UnsupportedEncodingException;

import oic.ctt.ui.UIConst;
import oic.ctt.ui.types.ImageFilePathType;
import oic.ctt.ui.types.ToolTipTextType;
import oic.ctt.ui.util.CTLogger;
import static oic.ctt.ui.types.ToolTipTextType.*;
import static oic.ctt.ui.types.ImageFilePathType.*;
import static oic.ctt.ui.types.IDType.*;
import static oic.ctt.ui.actions.ActionsConstants.*;

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
import org.slf4j.Logger;

public class PauseTestAction extends Action implements ISelectionListener,
        IWorkbenchAction {
    private Logger                 logger    = CTLogger.getInstance();
    public final static String     ID        = "oic.ctt.ui.actions.StopTestAction";
    private IWorkbenchWindow       workbenchwindow;
    private ActionContributionItem pauseTest = null;
    private ActionContributionItem stopTest  = null;

    public PauseTestAction(IWorkbenchWindow window) {
        super(UIConst.TOOLBAR_TEXT_PAUSERESUME);
        this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
                PLUGIN_ID.toString(),
                IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_PAUSE_TEST.toString()));
        this.setToolTipText(TOOLTIP_TEXT_PAUSE_TEST.toString());
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
                    if (mitem.getText().equals("&" + MENU_ITEM_RUN_TEXT)
                            || mitem.getText().equals(MENU_ITEM_RUN_TEXT)) {
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
                                    if (pauseTest.getAction().getToolTipText() == TOOLTIP_TEXT_PAUSE_TEST
                                            .toString()) {
                                        pidProcess = ActionsConstants
                                                .executeCommand(new String[] {
                                                        COMMAND_TYPE_SHELL, C,
                                                        COMMAND_GREP + shellPid });
                                        String childPID = ActionsConstants
                                                .getProcessID(pidProcess);
                                        pidProcess = ActionsConstants
                                                .executeCommand(new String[] {
                                                        COMMAND_TYPE_SHELL, C,
                                                        COMMAND_GREP + childPID });
                                        String grandchildPID = ActionsConstants
                                                .getProcessID(pidProcess);

                                        if (grandchildPID != null) {
                                            processStat = executeCommandForChild(
                                                    grandchildPID, KILL_19);
                                        } else {
                                            pidProcess = ActionsConstants
                                                    .executeCommand(new String[] {
                                                            COMMAND_TYPE_SHELL,
                                                            C,
                                                            COMMAND_PROCESS
                                                                    + childPID
                                                                    + AWK_END_PRINT });
                                            String line_findJava = ActionsConstants
                                                    .getProcessID(pidProcess);
                                            if (line_findJava != null) {
                                                if (line_findJava
                                                        .contains(JYTHON_JAR_FILE_NAME)) {
                                                    processStat = executeCommandForShellPid(
                                                            shellPid, childPID,
                                                            PKILL_19_P);
                                                }
                                            }
                                        }
                                        if (processStat != null) {
                                            if (processStat.contains("T")) {
                                                changeActionState(
                                                        TOOLTIP_TEXT_RESUME_TEST,
                                                        IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_RESUME,
                                                        false);
                                            } else {
                                                logger.debug("Process didn't pause.");
                                            }
                                        } else {
                                            logger.debug("Process is null.");
                                        }
                                    } else {
                                        pidProcess = ActionsConstants
                                                .executeCommand(new String[] {
                                                        COMMAND_TYPE_SHELL, C,
                                                        COMMAND_GREP + shellPid });
                                        String childPID = ActionsConstants
                                                .getProcessID(pidProcess);
                                        pidProcess = ActionsConstants
                                                .executeCommand(new String[] {
                                                        COMMAND_TYPE_SHELL, C,
                                                        COMMAND_GREP + childPID });
                                        String grandchildPID = ActionsConstants
                                                .getProcessID(pidProcess);
                                        if (grandchildPID != null) {
                                            processStat = executeCommandForChild(
                                                    grandchildPID, KILL_18);
                                        } else {
                                            pidProcess = ActionsConstants
                                                    .executeCommand(new String[] {
                                                            COMMAND_TYPE_SHELL,
                                                            C,
                                                            COMMAND_PROCESS
                                                                    + childPID
                                                                    + AWK_END_PRINT });
                                            String line_findJava = ActionsConstants
                                                    .getProcessID(pidProcess);
                                            if (line_findJava != null) {
                                                if (line_findJava
                                                        .contains(JYTHON_JAR_FILE_NAME)) {
                                                    processStat = executeCommandForShellPid(
                                                            shellPid, childPID,
                                                            PKILL_18_P);
                                                }
                                            }
                                        }
                                        if (processStat != null) {
                                            if (processStat.contains("S")) {
                                                changeActionState(
                                                        TOOLTIP_TEXT_PAUSE_TEST,
                                                        IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_PAUSE_TEST,
                                                        true);
                                            } else {
                                                logger.debug("Process didn't resume.");
                                            }
                                        } else {
                                            logger.debug("Process is null.");
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

            private String executeCommandForShellPid(int shellPid,
                    String childPID, String command) throws IOException,
                    InterruptedException, UnsupportedEncodingException {
                String processStat;
                Process pidProcess;
                pidProcess = ActionsConstants.executeCommand(new String[] {
                        COMMAND_TYPE_SHELL, C, command + shellPid });
                Thread.sleep(100);
                pidProcess = ActionsConstants.executeCommand(new String[] {
                        COMMAND_TYPE_SHELL, C,
                        COMMAND_PROCESS_SEARCH + childPID + AWK_END_PRINT_$3 });
                processStat = ActionsConstants.getProcessID(pidProcess);
                return processStat;
            }

            private String executeCommandForChild(String childPid,
                    String command) throws IOException, InterruptedException,
                    UnsupportedEncodingException {
                String processStat;
                Process pidProcess;
                pidProcess = ActionsConstants.executeCommand(new String[] {
                        COMMAND_TYPE_SHELL, C, command + childPid });
                Thread.sleep(100);
                pidProcess = ActionsConstants.executeCommand(new String[] {
                        COMMAND_TYPE_SHELL, C,
                        COMMAND_PROCESS_SEARCH + childPid + AWK_END_PRINT_$3 });
                processStat = ActionsConstants.getProcessID(pidProcess);
                return processStat;
            }

            private void changeActionState(ToolTipTextType toolTipTypeText,
                    ImageFilePathType imageFilePath, boolean enable) {
                pauseTest.getAction()
                        .setImageDescriptor(
                                AbstractUIPlugin.imageDescriptorFromPlugin(
                                        PLUGIN_ID.toString(),
                                        imageFilePath.toString()));
                pauseTest.getAction()
                        .setToolTipText(toolTipTypeText.toString());
                stopTest.getAction().setEnabled(enable);
            }

        });
    }
}
