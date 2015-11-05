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
import java.util.ArrayList;

import oic.ctt.ui.Activator;
import oic.ctt.ui.UIConst;
import oic.ctt.ui.views.TestPlanView;
import oic.ctt.ui.views.TestReportView;
import oic.ctt.ui.views.TestSuiteView;

import org.apache.commons.io.FileUtils;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.plugin.AbstractUIPlugin;

public class DeleteAction extends Action implements ISelectionListener,
        IWorkbenchAction {

    public String category;

    public DeleteAction(String value) {
        super("Delete Selected Item");
        category = value;
        this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
                Activator.PLUGIN_ID, "icons/erase.png"));
        this.setToolTipText("Delete Selected Item (Del)");
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
        TestSuiteView testSuiteView = (TestSuiteView) page
                .findView(TestSuiteView.ID);
        TestPlanView testPlanView = (TestPlanView) page
                .findView(TestPlanView.ID);
        TestReportView testReportView = (TestReportView) page
                .findView(TestReportView.ID);

        ArrayList<File> dirList = new ArrayList<File>();
        ArrayList<File> fileList = new ArrayList<File>();
        String list = "";
        int count = 0;

        if (category.equals(UIConst.TESTSUITE_PATH)) {
            if (testSuiteView != null) {
                Object[] items = testSuiteView.getSelectedItems();

                for (int i = 0; i < items.length; i++) {
                    if (items[i] instanceof File) {
                        File selectItems = (File) items[i];
                        if (selectItems.exists()) {
                            if (selectItems.isFile()) {
                                if (count < 20) {
                                    list += selectItems.getName() + "\n";
                                } else if (count == 20) {
                                    list += "........................\n";
                                }
                                fileList.add(selectItems);
                                count++;
                            }
                            if (selectItems.isDirectory()) {
                                if (testSuiteView
                                        .getChildrenChecked(selectItems)) {
                                    if (count < 20) {
                                        list += selectItems.getName() + "\n";
                                    } else if (count == 20) {
                                        list += "........................\n";
                                    }
                                    dirList.add(selectItems);
                                    count++;
                                }

                            }
                        }
                    }
                }
                if (dirList.size() + fileList.size() > 0) {
                    MessageDialog dialog = new MessageDialog(Display
                            .getCurrent().getActiveShell(),
                            "Delete Test Suite", null,
                            "Are you sure you want to delete Test Suite?\n\n"
                                    + list + "\n"
                                    + (dirList.size() + fileList.size())
                                    + " item(s) selected.",
                            MessageDialog.QUESTION, new String[] { "Delete",
                                    "Cancel" }, 0);

                    int result = dialog.open();

                    if (result == 0) {
                        for (Object file : fileList.toArray()) {
                            File delFile = (File) file;
                            try {
                                FileUtils.forceDelete(delFile);
                                System.out.println("Delete Test Suite File : "
                                        + delFile.getPath());
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }
                        for (Object dir : dirList.toArray()) {
                            File delDir = (File) dir;
                            try {
                                FileUtils.deleteDirectory(delDir);
                                System.out
                                        .println("Delete Test Suite Directory : "
                                                + delDir.getPath());
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }

                        // Close Opened editors
                        IWorkbenchPage[] pages = PlatformUI.getWorkbench()
                                .getActiveWorkbenchWindow().getPages();
                        for (int i = 0; i < pages.length; i++) {
                            IEditorReference[] editors = pages[i]
                                    .getEditorReferences();
                            for (int j = 0; j < editors.length; j++) {
                                try {
                                    File file = ((FileEditorInput) editors[j]
                                            .getEditorInput()).getPath()
                                            .toFile();
                                    System.out.println(file.getAbsolutePath());
                                    if (file.exists() == false) {
                                        pages[i].closeEditor(
                                                editors[j].getEditor(true),
                                                true);
                                    }
                                } catch (PartInitException e) {
                                    e.printStackTrace();
                                }
                            }
                        }
                    }
                    if (category.equals(UIConst.TESTSUITE_PATH)) {
                        testSuiteView.setAllUnChecked();
                        testSuiteView.refreshTree();
                    }
                } else {
                    MessageDialog dialog = new MessageDialog(Display
                            .getCurrent().getActiveShell(), "Delete Test Item",
                            null, "Not selected item.\n\n" + dirList.size()
                                    + " item(s) selected.",
                            MessageDialog.WARNING, new String[] { "OK" }, 0);

                    dialog.open();
                }
            }
        } else if (category.equals(UIConst.TESTPLAN_PATH)
                || category.equals(UIConst.TESTREPORT_PATH)) {
            IStructuredSelection selection = null;

            if (category.equals(UIConst.TESTPLAN_PATH)) {
                if (testPlanView != null) {
                    selection = (IStructuredSelection) testPlanView
                            .getSelectionTree();
                }
            } else if (category.equals(UIConst.TESTREPORT_PATH)) {
                if (testReportView != null) {
                    selection = (IStructuredSelection) testReportView
                            .getSelectionTree();
                }
            }
            if (selection != null) {
                for (Object dir : selection.toList()) {
                    if (dir instanceof File) {
                        File dirPath = (File) dir;
                        if (dirPath.exists()) {
                            if (dirPath.isDirectory()) {
                                if (count < 20) {
                                    list += dirPath.getName() + "\n";
                                } else if (count == 20) {
                                    list += "........................\n";
                                }
                                dirList.add((File) dir);
                                count++;
                            }
                        }
                    }
                }

                if (dirList.size() > 0) {
                    MessageDialog dialog = new MessageDialog(Display
                            .getCurrent().getActiveShell(), "Delete Test Item",
                            null,
                            "Are you sure you want to delete Test Item?\n\n"
                                    + list + "\n" + dirList.size()
                                    + " item(s) selected.",
                            MessageDialog.QUESTION, new String[] { "Delete",
                                    "Cancel" }, 0);

                    int result = dialog.open();

                    if (result == 0) {
                        for (Object dir : dirList.toArray()) {
                            File dirPath2 = (File) dir;
                            try {
                                FileUtils.deleteDirectory(dirPath2);
                                System.out.println("Delete Test Item : "
                                        + dirPath2.getPath());
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }

                        // Close Opened editors
                        IWorkbenchPage[] pages = PlatformUI.getWorkbench()
                                .getActiveWorkbenchWindow().getPages();
                        for (int i = 0; i < pages.length; i++) {
                            IEditorReference[] editors = pages[i]
                                    .getEditorReferences();
                            for (int j = 0; j < editors.length; j++) {
                                try {
                                    File file = ((FileEditorInput) editors[j]
                                            .getEditorInput()).getPath()
                                            .toFile();
                                    System.out.println(file.getAbsolutePath());
                                    if (file.exists() == false) {
                                        pages[i].closeEditor(
                                                editors[j].getEditor(true),
                                                true);
                                    }
                                } catch (PartInitException e) {
                                    e.printStackTrace();
                                }
                            }
                        }
                    }
                    if (category.equals(UIConst.TESTSUITE_PATH)) {
                        if (testSuiteView != null) {
                            testSuiteView.refreshTree();
                        }
                    } else if (category.equals(UIConst.TESTPLAN_PATH)) {
                        if (testPlanView != null) {
                            testPlanView.refreshTree();
                        }
                    } else if (category.equals(UIConst.TESTREPORT_PATH)) {
                        if (testReportView != null) {
                            testReportView.refreshTree();
                        }
                    }
                } else {
                    MessageDialog dialog = new MessageDialog(Display
                            .getCurrent().getActiveShell(), "Delete Test Item",
                            null, "Not selected directory.\n\n"
                                    + dirList.size() + " item(s) selected.",
                            MessageDialog.WARNING, new String[] { "OK" }, 0);

                    dialog.open();
                }
            } else {
                System.out.println("[Debug] Selection is null.");
            }
        }
    }
}
