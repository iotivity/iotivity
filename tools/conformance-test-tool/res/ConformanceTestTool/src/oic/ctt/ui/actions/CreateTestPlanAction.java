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
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashMap;

import oic.ctt.ui.Activator;
import oic.ctt.ui.UIConst;
import oic.ctt.ui.dialogs.CreateTestPlanDialog;
import oic.ctt.ui.util.TestCaseParser;
import oic.ctt.ui.views.TestPlanView;
import oic.ctt.ui.views.TestSuiteView;

import org.apache.commons.io.FileUtils;
import org.eclipse.core.runtime.MultiStatus;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.plugin.AbstractUIPlugin;

public class CreateTestPlanAction extends Action implements IWorkbenchAction {

    private static final String ID = "oic.ctt.ui.actions.CreateTestPlan";

    public CreateTestPlanAction() {
        super("Create Test Plan");
        this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
                Activator.PLUGIN_ID, "icons/folder_add.png"));
        this.setToolTipText("Create Test Plan");
        setId(ID);
    }

    public void run() {

        Shell shell = PlatformUI.getWorkbench().getActiveWorkbenchWindow()
                .getShell();
        CreateTestPlanDialog dialog = new CreateTestPlanDialog(shell);
        dialog.create();

        if (dialog.open() == Window.OK) {
            String testPlanName = dialog.getTestPlanName();
            boolean checkCancel = false;
            IWorkbenchPage page = PlatformUI.getWorkbench()
                    .getActiveWorkbenchWindow().getActivePage();
            TestSuiteView testSuiteView = (TestSuiteView) page
                    .findView(TestSuiteView.ID);
            File currentPlanPath = null;

            if (testPlanName != null) {
                while (testPlanName.isEmpty()) {
                    MessageBox box = new MessageBox(PlatformUI.getWorkbench()
                            .getActiveWorkbenchWindow().getShell(),
                            SWT.ICON_ERROR | SWT.OK);
                    box.setMessage("You didn't enter the Test Plan name. \nPlease enter the new plan name.");
                    box.setText("ERROR");
                    box.open();
                    if (dialog.open() == Window.CANCEL) {
                        checkCancel = true;
                        break;
                    }
                    testPlanName = dialog.getTestPlanName();
                }
                while (testPlanName.contains("/")) {
                    MessageBox box = new MessageBox(PlatformUI.getWorkbench()
                            .getActiveWorkbenchWindow().getShell(),
                            SWT.ICON_ERROR | SWT.OK);
                    box.setMessage("You can't use \"/\" in the Test Plan name. \nPlease enter another name.");
                    box.setText("ERROR");
                    box.open();
                    if (dialog.open() == Window.CANCEL) {
                        checkCancel = true;
                        break;
                    }
                    testPlanName = dialog.getTestPlanName();
                }
            }

            boolean checkSameDir = true;
            while (checkSameDir) {

                testPlanName = dialog.getTestPlanName();
                currentPlanPath = new File(UIConst.ROOT_PATH
                        + UIConst.TESTPLAN_PATH + "/" + testPlanName);
                if (!currentPlanPath.exists()) {
                    System.out.println("creating directory: " + testPlanName);
                    currentPlanPath.mkdir();
                    checkSameDir = false;
                    break;
                } else { // if exist same directory.
                    checkSameDir = true;
                    MessageBox box = new MessageBox(PlatformUI.getWorkbench()
                            .getActiveWorkbenchWindow().getShell(),
                            SWT.ICON_ERROR | SWT.OK);
                    box.setMessage("The plan name is already exist. \nPlease enter the another name.");
                    box.setText("ERROR");
                    box.open();
                }
                if (dialog.open() == Window.CANCEL) {
                    checkCancel = true;
                    break;
                }
            }

            if (checkCancel == false) {

                if (testSuiteView != null) {
                    Object[] items = testSuiteView.getSelectedItems();
                    Arrays.sort(items);
                    String currenTestSuite = null;
                    Statement stat = null;
                    Connection conn = null;
                    PreparedStatement prepInsert = null;
                    PreparedStatement prepUpdate = null;
                    TestCaseParser parser = new TestCaseParser();
                    boolean checkCreate = true;
                    ArrayList<String> checkedTestsuite = new ArrayList<String>();
                    ArrayList<String> checkedTestcase = new ArrayList<String>();

                    try {
                        Class.forName("org.sqlite.JDBC");
                        conn = DriverManager.getConnection("jdbc:sqlite:"
                                + UIConst.ROOT_PATH + UIConst.TESTPLAN_PATH
                                + "/" + testPlanName + "/" + testPlanName
                                + ".db");
                        stat = conn.createStatement();
                        stat.executeUpdate("drop table if exists tcinfo;");
                        stat.executeUpdate("CREATE TABLE tcinfo (id INTEGER PRIMARY_KEY, testsuite TEXT, testcase TEXT, testcase_fullname TEXT, checked BOOLEAN);");
                        prepInsert = conn
                                .prepareStatement("INSERT INTO tcinfo VALUES (?, ?, ?, ?, ?);");
                        prepUpdate = conn
                                .prepareStatement("UPDATE tcinfo SET checked = 'true' WHERE testsuite = ? AND testcase_fullname = ?;");

                        int tccount = 0;
                        for (int i = 0; i < items.length; i++) {
                            if (items[i] instanceof File) {
                                File currentItem = (File) items[i];
                                if (currentItem.isFile()) {
                                    FileUtils.copyFileToDirectory(currentItem,
                                            currentPlanPath);
                                    FileUtils.waitFor(currentItem, 5);
                                    System.out.println(currentItem.getName());
                                } else if (currentItem.isFile() == false
                                        && currentItem.isDirectory() == false) {
                                    checkedTestsuite.add(currentItem
                                            .getParentFile().getName());
                                    checkedTestcase.add(currentItem.getName());
                                }
                            }
                        }
                        File[] planFiles = null;
                        planFiles = currentPlanPath.listFiles();
                        if (planFiles != null) {
                            Arrays.sort(planFiles);
                            for (int i = 0; i < planFiles.length; i++) {
                                if (planFiles[i] instanceof File) {
                                    File currentItem = (File) planFiles[i];
                                    currenTestSuite = currentItem.getName();

                                    if (currentItem.isFile()) {
                                        LinkedHashMap<Integer, LinkedHashMap<String, String>> list = parser
                                                .getDocumentHashMap(currentItem
                                                        .toPath());
                                        if (list != null) {
                                            for (int j = 0; j < list.size(); j++) {
                                                prepInsert.setInt(1, tccount);
                                                tccount++;
                                                prepInsert.setString(2,
                                                        currenTestSuite);
                                                prepInsert.setString(3, list
                                                        .get(j).get("TC NAME"));
                                                prepInsert
                                                        .setString(
                                                                4,
                                                                list.get(j)
                                                                        .get("TC FULL NAME"));
                                                boolean check = false;
                                                for (int k = 0; k < checkedTestsuite
                                                        .size(); k++) {
                                                    if (checkedTestcase
                                                            .get(k)
                                                            .equals(list
                                                                    .get(j)
                                                                    .get("TC FULL NAME"))) {
                                                        if (checkedTestsuite
                                                                .get(k)
                                                                .equals(currentItem
                                                                        .getName())) {
                                                            check = true;
                                                            break;
                                                        }
                                                    }
                                                }
                                                if (check) {
                                                    prepInsert.setString(5,
                                                            "true");
                                                } else {
                                                    prepInsert.setString(5,
                                                            "false");
                                                }

                                                prepInsert.addBatch();
                                            }
                                            conn.setAutoCommit(false);
                                            prepInsert.executeBatch();
                                            conn.setAutoCommit(true);
                                        }
                                    }
                                } else {
                                    checkCreate = false;
                                    System.err.println("Checked item ERROR.");
                                }
                            }
                        }

                        if (checkCreate) {

                            MessageDialog dialog1 = new MessageDialog(
                                    PlatformUI.getWorkbench()
                                            .getActiveWorkbenchWindow()
                                            .getShell(), "Information", null,
                                    "Success to create a new Test Plan.\nTest Plan Name = "
                                            + testPlanName,
                                    MessageDialog.INFORMATION,
                                    new String[] { "OK" }, 0);
                            dialog1.open();

                            IViewPart planView = page.showView(TestPlanView.ID);
                            ((TestPlanView) planView)
                                    .setFocusCurrentPlan(testPlanName);
                        } else {
                            MessageBox box = new MessageBox(PlatformUI
                                    .getWorkbench().getActiveWorkbenchWindow()
                                    .getShell(), SWT.ICON_ERROR | SWT.ERROR);
                            box.setMessage("Fail to create a new Test Plan.\nPlease try again.");
                            box.setText("ERROR");
                            box.open();
                        }
                    } catch (Exception e) {
                        MultiStatus status = UIConst.createMultiStatus(
                                e.getLocalizedMessage(), e);
                        ErrorDialog.openError(Display.getDefault()
                                .getActiveShell(), "Error",
                                "Fail to create a new Test Plan", status);
                        e.printStackTrace();
                    } finally {
                        try {
                            if (prepUpdate != null) {
                                prepUpdate.close();
                            }
                            if (prepInsert != null) {
                                prepInsert.close();
                            }
                            if (stat != null) {
                                stat.close();
                            }
                            if (conn != null) {
                                conn.close();
                            }
                        } catch (SQLException e) {
                            e.printStackTrace();
                        }
                    }
                }
            }
        }
    }

    public void dispose() {
    }

}
