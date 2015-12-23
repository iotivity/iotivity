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

import java.io.File;
import java.io.IOException;

import oic.ctt.ui.Activator;
import oic.ctt.ui.UIConst;
import static oic.ctt.ui.types.ToolTipTextType.*;
import static oic.ctt.ui.types.ImageFilePathType.*;
import static oic.ctt.ui.types.IDType.*;
import oic.ctt.ui.util.CTLogger;
import static oic.ctt.ui.util.PopUpUtil.*;
import oic.ctt.ui.views.TestPlanView;

import org.apache.commons.io.FileUtils;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.slf4j.Logger;

public class ImportTestPlanAction extends Action implements ISelectionListener,
        IWorkbenchAction {
    private Logger              logger                             = CTLogger
                                                                           .getInstance();
    private static final String DIRECTORY_DIALOG_TEST_PLAN_MESSAGE = "Select Plan Directory..";
    private static final String TEST_PLAN_EXIST_ERROR_MESSAGE      = "The plan name is already exist. \nPlease check the plan name.";

    public ImportTestPlanAction() {
        super(TOOLTIP_TEXT_IMPORT_TEST_PLAN.toString());
        this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
                Activator.PLUGIN_ID,
                IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_IMPORT_TEST_PLAN.toString()));
        this.setToolTipText(TOOLTIP_TEXT_IMPORT_TEST_PLAN.toString());
    }

    @Override
    public void dispose() {
    }

    @Override
    public void selectionChanged(IWorkbenchPart part, ISelection selection) {
    }

    public void run() {

        IWorkbenchPage page = PlatformUI.getWorkbench()
                .getActiveWorkbenchWindow().getActivePage();
        TestPlanView testPlanView = (TestPlanView) page
                .findView(TEST_PLAN_VIEW_ID.toString());
        String dirName = displayDirectoryDialog(ACTIVE_SHELL,
                DIRECTORY_DIALOG_TEST_PLAN_MESSAGE, UIConst.PROJECT_PATH);
        logger.info("Directory name : " + dirName);
        if (dirName != null) {
            File importDir = new File(dirName);

            File existTestDir = new File(UIConst.ROOT_PATH
                    + UIConst.TESTPLAN_PATH + "/" + importDir.getName());
            if (existTestDir.exists()) {
                logger.info("Directory " + dirName + " exists");
                displayMessageBox(ACTIVE_WORKBENCH_WINDOW_SHELL,
                        TEST_PLAN_EXIST_ERROR_MESSAGE, ERROR_TEXT,
                        SWT.ICON_ERROR | SWT.OK);
            }

            try {
                FileUtils.copyDirectoryToDirectory(importDir, new File(
                        UIConst.ROOT_PATH + UIConst.TESTPLAN_PATH));
                if (testPlanView != null) {
                    testPlanView.setFocusCurrentPlan(importDir.getName());
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }
}
