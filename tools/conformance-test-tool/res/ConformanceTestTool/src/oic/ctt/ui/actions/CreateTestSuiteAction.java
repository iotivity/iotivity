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
import oic.ctt.ui.multipages.TestSuiteMultiPageEditor;
import oic.ctt.ui.views.TestSuiteView;

import org.apache.commons.io.FileUtils;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.plugin.AbstractUIPlugin;

public class CreateTestSuiteAction extends Action implements
        ISelectionListener, IWorkbenchAction {

    public CreateTestSuiteAction() {
        super("Create Test Suite");
        this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
                Activator.PLUGIN_ID, "icons/document_new.png"));
        this.setToolTipText("Create New Test Suite.");
    }

    @Override
    public void dispose() {
    }

    @Override
    public void selectionChanged(IWorkbenchPart part, ISelection selection) {
    }

    public void run() {
        UIConst.setAssociateEditorToFile("", "txt", TestSuiteMultiPageEditor.ID);

        IWorkbenchPage page = PlatformUI.getWorkbench()
                .getActiveWorkbenchWindow().getActivePage();
        TestSuiteView testSuiteView = (TestSuiteView) page
                .findView(TestSuiteView.ID);

        FileDialog fileDialog = new FileDialog(Display.getCurrent()
                .getActiveShell(), SWT.SAVE);
        fileDialog.setText("Select Directory..");
        fileDialog.setFilterPath(UIConst.ROOT_PATH + UIConst.TESTSUITE_PATH);
        fileDialog.setFileName("NewTestSuite.txt");
        fileDialog.setOverwrite(true);

        String fileName = fileDialog.open();
        System.out.println(fileName);
        if (fileName != null) {
            File newFile = new File(fileName);
            File srcFile = new File(UIConst.PROJECT_PATH
                    + UIConst.TEMPLETE_PATH, "test_suite_template.txt");
            if (!newFile.exists()) {
                try {
                    FileUtils.copyFile(srcFile, newFile);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (testSuiteView != null) {
                testSuiteView.refreshTree();
            }
            testSuiteView.setFocusCurrentSuite(newFile);
        }
    }
}
