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

import oic.ctt.ui.UIConst;
import static oic.ctt.ui.types.ToolTipTextType.*;
import static oic.ctt.ui.types.ImageFilePathType.*;
import static oic.ctt.ui.types.IDType.*;
import oic.ctt.ui.util.CTLogger;
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
import org.slf4j.Logger;

public class CreateTestSuiteAction extends Action implements
        ISelectionListener, IWorkbenchAction {
    private Logger              logger                            = CTLogger
                                                                          .getInstance();
    private static final String FILE_EXTENSION                    = "txt";
    private static final String TEST_SUITE_TEMPLATE_SRC_FILE_NAME = "test_suite_template.txt";
    private static final String NEW_TEST_SUITE_FILE_NAME          = "NewTestSuite.txt";
    private static final String SELECT_DIRECTORY_TEXT             = "Select Directory..";
    private static final String EDITOR_SRC_FILE_NAME              = "";

    public CreateTestSuiteAction() {
        super(TOOLTIP_TEXT_CREATE_TEST_SUITE.toString());
        this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
                PLUGIN_ID.toString(),
                IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_CREATE_TEST_SUITE.toString()));
        this.setToolTipText(TOOLTIP_TEXT_CREATE_TEST_SUITE.toString());
    }

    @Override
    public void dispose() {
    }

    @Override
    public void selectionChanged(IWorkbenchPart part, ISelection selection) {
    }

    public void run() {
        UIConst.setAssociateEditorToFile(EDITOR_SRC_FILE_NAME, FILE_EXTENSION,
                TEST_SUITE_MULTI_PAGE_EDITOR_ID.toString());

        IWorkbenchPage page = PlatformUI.getWorkbench()
                .getActiveWorkbenchWindow().getActivePage();
        TestSuiteView testSuiteView = (TestSuiteView) page
                .findView(TEST_SUITE_VIEW_ID.toString());

        FileDialog fileDialog = new FileDialog(Display.getCurrent()
                .getActiveShell(), SWT.SAVE);
        fileDialog.setText(SELECT_DIRECTORY_TEXT);
        fileDialog.setFilterPath(UIConst.ROOT_PATH + UIConst.TESTSUITE_PATH);
        fileDialog.setFileName(NEW_TEST_SUITE_FILE_NAME);
        fileDialog.setOverwrite(true);

        String fileName = fileDialog.open();
        if (fileName != null) {
            File newFile = new File(fileName);
            File srcFile = new File(UIConst.PROJECT_PATH
                    + UIConst.TEMPLETE_PATH, TEST_SUITE_TEMPLATE_SRC_FILE_NAME);
            logger.info("Source File :" + srcFile + " New File : " + newFile);
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
