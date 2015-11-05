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
import oic.ctt.ui.views.TestPlanView;

import org.apache.commons.io.FileUtils;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.plugin.AbstractUIPlugin;

public class ImportTestPlanAction extends Action implements ISelectionListener,
        IWorkbenchAction {
    public ImportTestPlanAction() {
        super("Import Test Plan");
        this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
                Activator.PLUGIN_ID, "icons/import.png"));
        this.setToolTipText("Import Test Plan.");
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
                .findView(TestPlanView.ID);

        DirectoryDialog dirDialog = new DirectoryDialog(Display.getCurrent()
                .getActiveShell(), SWT.MULTI);
        dirDialog.setText("Select Plan Directory..");
        dirDialog.setFilterPath(UIConst.PROJECT_PATH);
        String dirName = dirDialog.open();
        System.out.println(dirName);
        if (dirName != null) {
            File importDir = new File(dirName);

            File existTestDir = new File(UIConst.ROOT_PATH
                    + UIConst.TESTPLAN_PATH + "/" + importDir.getName());
            if (existTestDir.exists()) {
                System.out.println("exist directory");
                MessageBox box = new MessageBox(PlatformUI.getWorkbench()
                        .getActiveWorkbenchWindow().getShell(), SWT.ICON_ERROR
                        | SWT.OK);
                box.setMessage("The plan name is already exist. \nPlease check the plan name.");
                box.setText("ERROR");
                box.open();
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
