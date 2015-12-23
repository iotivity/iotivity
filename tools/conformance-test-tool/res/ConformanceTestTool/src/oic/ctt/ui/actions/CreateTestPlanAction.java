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
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashMap;

import oic.ctt.ui.Activator;
import oic.ctt.ui.UIConst;
import oic.ctt.ui.util.DatabaseUtil;
import oic.ctt.ui.dialogs.CreateTestPlanDialog;
import static oic.ctt.ui.types.ImageFilePathType.*;
import static oic.ctt.ui.types.ToolTipTextType.*;
import static oic.ctt.ui.types.IDType.*;
import oic.ctt.ui.util.CTLogger;
import oic.ctt.ui.util.TestCaseParser;
import oic.ctt.ui.views.TestPlanView;
import oic.ctt.ui.views.TestSuiteView;
import static oic.ctt.ui.util.PopUpUtil.*;
import static oic.ctt.ui.util.DatabaseUtil.*;

import org.apache.commons.io.FileUtils;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory.IWorkbenchAction;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.slf4j.Logger;

public class CreateTestPlanAction extends Action implements IWorkbenchAction {
    private Logger              logger                                 = CTLogger
                                                                               .getInstance();
    private static final String KEY_TC_FULL_NAME                       = "TC FULL NAME";
    private static final String KEY_TC_NAME                            = "TC NAME";
    private static final String DUPLICATE_TEST_PLAN_NAME_ERROR_MESSAGE = "The plan name is already exist. \nPlease enter the another name.";
    private static final String INVALID_TEST_PLAN_NAME_ERROR_MESSAGE   = "You can't use \"/\" in the Test Plan name. \nPlease enter another name.";
    private static final String EMPTY_TEST_PLAN_NAME_ERROR_MESSAGE     = "You didn't enter the Test Plan name. \nPlease enter the new plan name.";
    private static final String COLUMNS_DETAILS                        = "id INTEGER PRIMARY_KEY, testsuite TEXT, testcase TEXT, testcase_fullname TEXT, checked BOOLEAN";
    private static final String TEST_PLAN_DB_UPDATE_STATEMENT          = "UPDATE tcinfo SET checked = 'true' WHERE testsuite = ? AND testcase_fullname = ?;";
    private static final String UNCHECKED_TEST_PLAN_ERROR_MESSAGE      = "You didn't enter the Test Plan name. \nPlease enter the new plan name";
    private static final String TEST_PLAN_SUCCESS_DIALOG_MESSAGE       = "Success to create a new Test Plan.\nTest Plan Name = ";
    private static final String TEST_PLAN_EXCEPTION_MESSAGE            = "Fail to create a new Test Plan";
    private String              testPlanName;
    private boolean             checkCancel                            = false;
    private File                currentPlanPath;
    private ArrayList<String>   checkedTestsuite;
    private ArrayList<String>   checkedTestcase;
    private int                 tccount;

    public CreateTestPlanAction() {
        super(TOOLTIP_TEXT_CREATE_TEST_PLAN.toString());
        this.setImageDescriptor(AbstractUIPlugin.imageDescriptorFromPlugin(
                Activator.PLUGIN_ID,
                IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_CREATE_TEST_PLAN.toString()));
        this.setToolTipText(TOOLTIP_TEXT_CREATE_TEST_PLAN.toString());
        setId(CREATE_TEST_PLAN_ACTION_ID.toString());
    }

    public void run() {
        CreateTestPlanDialog dialog = new CreateTestPlanDialog(
                ACTIVE_WORKBENCH_WINDOW_SHELL);
        dialog.create();

        if (dialog.open() == Window.OK) {
            testPlanName = dialog.getTestPlanName();
            checkCancel = false;
            IWorkbenchPage page = PlatformUI.getWorkbench()
                    .getActiveWorkbenchWindow().getActivePage();
            TestSuiteView testSuiteView = (TestSuiteView) page
                    .findView(TEST_SUITE_VIEW_ID.toString());
            currentPlanPath = null;

            if (testPlanName != null) {
                checkInvalidTestPlanName(dialog);
            }

            createCurrentPlan(dialog);

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
                    checkedTestsuite = new ArrayList<String>();
                    checkedTestcase = new ArrayList<String>();

                    try {
                        conn = createDBConnection(testPlanName);
                        createDBTable(TABLE_NAME, COLUMNS_DETAILS, conn);
                        prepInsert = insertIntoTable(conn, TABLE_NAME,
                                DEFAULT_INSERT_VALUES);
                        prepUpdate = updateTable(conn,
                                TEST_PLAN_DB_UPDATE_STATEMENT);
                        tccount = 0;
                        addSelectedSuiteAndTestCase(items, checkedTestsuite,
                                checkedTestcase);
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
                                            addBatch(currenTestSuite,
                                                    prepInsert, currentItem,
                                                    list);
                                            conn.setAutoCommit(false);
                                            prepInsert.executeBatch();
                                            conn.setAutoCommit(true);
                                        }
                                    }
                                } else {
                                    checkCreate = false;
                                    logger.error("Checked item ERROR.");
                                }
                            }
                        }

                        if (checkCreate) {
                            displayMessageDialog(ACTIVE_WORKBENCH_WINDOW_SHELL,
                                    DIALOG_TITLE,
                                    TEST_PLAN_SUCCESS_DIALOG_MESSAGE
                                            + testPlanName,
                                    MessageDialog.INFORMATION,
                                    new String[] { DIALOG_BUTTON_LABEL_OK });

                            IViewPart planView = page
                                    .showView(TEST_PLAN_VIEW_ID.toString());
                            ((TestPlanView) planView)
                                    .setFocusCurrentPlan(testPlanName);
                        } else {
                            displayMessageBox(ACTIVE_WORKBENCH_WINDOW_SHELL,
                                    UNCHECKED_TEST_PLAN_ERROR_MESSAGE,
                                    ERROR_TEXT, SWT.ICON_ERROR | SWT.ERROR);
                        }
                    } catch (Exception e) {
                        displayErrorDialog(e, ERROR_TEXT,
                                TEST_PLAN_EXCEPTION_MESSAGE);
                    } finally {
                        AutoCloseable[] autoCloseable = { stat, prepInsert,
                                prepUpdate, conn };
                        DatabaseUtil.closeDataBaseObjects(autoCloseable);
                    }
                }
            }
        }
    }

    /**
     * Adds a set of parameters to this <code>PreparedStatement</code> object's
     * batch of commands.
     * 
     * @param currenTestSuite
     *            name of current suite
     * @param prepInsert
     *            PreparedStatement object
     * @param currentItem
     *            current file
     * @param list
     *            current item list
     * @throws SQLException
     */
    private void addBatch(String currenTestSuite, PreparedStatement prepInsert,
            File currentItem,
            LinkedHashMap<Integer, LinkedHashMap<String, String>> list)
            throws SQLException {
        for (int j = 0; j < list.size(); j++) {
            prepInsert.setInt(1, tccount);
            tccount++;
            prepInsert.setString(2, currenTestSuite);
            prepInsert.setString(3, list.get(j).get(KEY_TC_NAME));
            prepInsert.setString(4, list.get(j).get(KEY_TC_FULL_NAME));
            if (isSuiteAndTestChecked(currentItem, list, j)) {
                prepInsert.setString(5, "true");
            } else {
                prepInsert.setString(5, "false");
            }

            prepInsert.addBatch();
        }
    }

    /**
     * check if suite or test case checked or not
     * 
     * @param currentItem
     *            current file
     * @param list
     *            current item list
     * @param j
     *            current index
     * @return check true or false
     */
    private boolean isSuiteAndTestChecked(File currentItem,
            LinkedHashMap<Integer, LinkedHashMap<String, String>> list, int j) {
        boolean check = false;
        for (int k = 0; k < checkedTestsuite.size(); k++) {
            if (checkedTestcase.get(k)
                    .equals(list.get(j).get(KEY_TC_FULL_NAME))) {
                if (checkedTestsuite.get(k).equals(currentItem.getName())) {
                    check = true;
                    break;
                }
            }
        }
        return check;
    }

    /**
     * add selected items to suite and test case object list
     * 
     * @param items
     *            items to be added
     * @param checkedTestsuite
     *            list of checked test suites
     * @param checkedTestcase
     *            list of checked test cases
     * @throws IOException
     */
    private void addSelectedSuiteAndTestCase(Object[] items,
            ArrayList<String> checkedTestsuite,
            ArrayList<String> checkedTestcase) throws IOException {
        for (int i = 0; i < items.length; i++) {
            if (items[i] instanceof File) {
                File currentItem = (File) items[i];
                if (currentItem.isFile()) {
                    FileUtils.copyFileToDirectory(currentItem, currentPlanPath);
                    FileUtils.waitFor(currentItem, 5);
                    logger.info("Process map has : " + currentItem.getName());
                } else if (currentItem.isFile() == false
                        && currentItem.isDirectory() == false) {
                    checkedTestsuite.add(currentItem.getParentFile().getName());
                    checkedTestcase.add(currentItem.getName());
                }
            }
        }
    }

    /**
     * This method creates current test plan
     * 
     * @param dialog
     *            used to get test plan name
     */
    private void createCurrentPlan(CreateTestPlanDialog dialog) {
        boolean checkSameDir = true;
        while (checkSameDir) {
            testPlanName = dialog.getTestPlanName();
            currentPlanPath = new File(UIConst.ROOT_PATH
                    + UIConst.TESTPLAN_PATH + "/" + testPlanName);
            if (!currentPlanPath.exists()) {
                logger.info("creating directory: " + testPlanName);
                currentPlanPath.mkdir();
                checkSameDir = false;
                break;
            } else {
                logger.info("Same directory already exist");
                checkSameDir = true;
                displayMessageBox(ACTIVE_WORKBENCH_WINDOW_SHELL,
                        DUPLICATE_TEST_PLAN_NAME_ERROR_MESSAGE, ERROR_TEXT,
                        SWT.ICON_ERROR | SWT.OK);
            }
            if (dialog.open() == Window.CANCEL) {
                checkCancel = true;
                break;
            }
        }
    }

    /**
     * This method checks if test plan name is empty or invalid
     * 
     * @param dialog
     */
    private void checkInvalidTestPlanName(CreateTestPlanDialog dialog) {
        while (testPlanName.isEmpty()) {
            displayMessageBox(ACTIVE_WORKBENCH_WINDOW_SHELL,
                    EMPTY_TEST_PLAN_NAME_ERROR_MESSAGE, ERROR_TEXT,
                    SWT.ICON_ERROR | SWT.OK);
            if (dialog.open() == Window.CANCEL) {
                checkCancel = true;
                break;
            }
            testPlanName = dialog.getTestPlanName();
        }
        while (testPlanName.contains("/")) {
            displayMessageBox(ACTIVE_WORKBENCH_WINDOW_SHELL,
                    INVALID_TEST_PLAN_NAME_ERROR_MESSAGE, ERROR_TEXT,
                    SWT.ICON_ERROR | SWT.OK);
            if (dialog.open() == Window.CANCEL) {
                checkCancel = true;
                break;
            }
            testPlanName = dialog.getTestPlanName();
        }
    }

    public void dispose() {
    }

}
