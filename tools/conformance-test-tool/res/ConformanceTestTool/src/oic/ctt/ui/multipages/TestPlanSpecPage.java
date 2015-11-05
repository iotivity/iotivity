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
package oic.ctt.ui.multipages;

import java.io.File;
import java.lang.reflect.InvocationTargetException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.LinkedHashMap;

import oic.ctt.ui.Activator;
import oic.ctt.ui.UIConst;
import oic.ctt.ui.util.TestCaseParser;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.MultiStatus;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.TreeEditor;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeColumn;
import org.eclipse.swt.widgets.TreeItem;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.forms.IManagedForm;
import org.eclipse.ui.forms.editor.FormEditor;
import org.eclipse.ui.forms.editor.FormPage;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;

public class TestPlanSpecPage extends FormPage {

    public static final String                                   ID                  = "oic.ctt.ui.editors.TestPlanSpecPage";

    public static String                                         TC_NAME             = null;
    public Tree                                                  tree                = null;
    public TreeColumn                                            columnStatus        = null;
    public TreeColumn                                            columnTestSuite     = null;
    public TreeColumn                                            columnTestCase      = null;
    public LinkedHashMap<Integer, LinkedHashMap<String, String>> list                = new LinkedHashMap<Integer, LinkedHashMap<String, String>>();
    public static int                                            currentPbarID       = 0;
    public int                                                   lengthArray         = 0;
    private ArrayList<Button>                                    checkBox_array;
    private String                                               planName            = "";
    public GC                                                    gc                  = null;

    private int                                                  idColnumber         = 1;
    private int                                                  statustextColnumber = 0;
    private int                                                  testcaseColnumber   = 2;

    public TestPlanSpecPage(FormEditor editor, String plan) {
        super(editor, "planSpecPage", plan);
        planName = plan;
    }

    protected void createFormContent(IManagedForm managedForm) {
        ScrolledForm form = managedForm.getForm();
        final FormToolkit toolkit = managedForm.getToolkit();
        form.getBody().setLayout(new FillLayout(SWT.HORIZONTAL));

        // Update checked information of test case
        Action updateTestPlanAction = new Action("Update Test Plan Information") {
            @Override
            public void run() {
                updateCheckBox();
                super.run();
            }
        };
        updateTestPlanAction.setImageDescriptor(Activator
                .getImageDescriptor("icons/save.png"));
        form.getToolBarManager().add(updateTestPlanAction);
        System.out.println("getEditorInput().getName() = " + planName);

        tree = new Tree(managedForm.getForm().getBody(), SWT.H_SCROLL
                | SWT.V_SCROLL | SWT.FULL_SELECTION | SWT.MULTI | SWT.WRAP);

        toolkit.adapt(tree);
        toolkit.paintBordersFor(tree);

        tree.setHeaderVisible(true);
        tree.setLinesVisible(true);
        gc = new GC(new Label(tree, SWT.NONE));

        ProgressMonitorDialog progressMonitorDlg = new ProgressMonitorDialog(
                form.getShell());

        IRunnableWithProgress runnableProgress = new IRunnableWithProgress() {
            @Override
            public void run(IProgressMonitor monitor)
                    throws InvocationTargetException, InterruptedException {
                if (tree != null) {
                    Display.getDefault().syncExec(new Runnable() {
                        @Override
                        public void run() {
                            columnStatus = new TreeColumn(tree, SWT.CENTER
                                    | SWT.MULTI | SWT.WRAP, statustextColnumber);
                            columnStatus.setText("Status");
                            columnStatus.setWidth(120);
                            columnStatus.setResizable(false);

                            columnTestSuite = new TreeColumn(tree, SWT.CENTER
                                    | SWT.MULTI | SWT.WRAP, idColnumber);
                            columnTestSuite.setText("TC ID / Test Spec");
                            columnTestSuite.setWidth(140);
                            columnTestSuite.setResizable(true);

                            columnTestCase = new TreeColumn(tree, SWT.LEFT
                                    | SWT.MULTI | SWT.WRAP, testcaseColnumber);
                            columnTestCase
                                    .setText("Test Case Name / Spec Description");
                            columnTestCase.setWidth(700);
                            columnTestCase.setResizable(true);
                        }
                    });

                    IWorkspace workspace = ResourcesPlugin.getWorkspace();
                    IWorkspaceRoot root = workspace.getRoot();

                    final IProject newProjectHandle = root
                            .getProject(UIConst.TESTPLAN_PATH);
                    String findName = getEditorInput().getName();

                    File planDir = new File(UIConst.ROOT_PATH
                            + UIConst.TESTPLAN_PATH + "/" + findName);
                    File[] planFiles = null;
                    LinkedHashMap<Integer, LinkedHashMap<String, String>> tcList = new LinkedHashMap<Integer, LinkedHashMap<String, String>>();

                    checkBox_array = new ArrayList<Button>();

                    if (planDir.isDirectory()) {
                        planFiles = planDir.listFiles();
                        if (planFiles != null) {
                            monitor.beginTask("Load Test Plan Specification.",
                                    planFiles.length);

                            Arrays.sort(planFiles);
                            TestCaseParser parser = new TestCaseParser();
                            String testsuiteName = null;

                            for (int i = 0; i < planFiles.length; i++) {
                                if (planFiles[i].getName().contains(".db")) {
                                    continue;
                                }
                                tcList.clear();
                                tcList = parser.getDocumentHashMap(planFiles[i]
                                        .getAbsolutePath());
                                testsuiteName = planFiles[i].getName();
                                if (tcList != null) {
                                    addTestCases(tcList, testsuiteName);
                                    Display.getDefault().syncExec(
                                            new Runnable() {
                                                @Override
                                                public void run() {
                                                    form.update();
                                                }
                                            });

                                }
                                System.out.println((i + 1) + " / "
                                        + (planFiles.length) + " "
                                        + planFiles[i].getName());
                                monitor.worked(1);
                                monitor.subTask((i + 1) + " / "
                                        + (planFiles.length) + "   "
                                        + planFiles[i].getName());
                            }

                            Display.getDefault().syncExec(new Runnable() {
                                @Override
                                public void run() {
                                    monitor.subTask("Processing Database.");

                                    Statement stat = null;
                                    Connection conn = null;
                                    ResultSet checkdInfo = null;
                                    try {
                                        // Get DB data
                                        Class.forName("org.sqlite.JDBC");
                                        conn = DriverManager
                                                .getConnection("jdbc:sqlite:"
                                                        + UIConst.ROOT_PATH
                                                        + UIConst.TESTPLAN_PATH
                                                        + "/"
                                                        + getEditorInput()
                                                                .getName()
                                                        + "/"
                                                        + getEditorInput()
                                                                .getName()
                                                        + ".db");
                                        stat = conn.createStatement();

                                        checkdInfo = stat
                                                .executeQuery("select * from tcinfo where checked = 'true'");

                                        int id = 0;
                                        String suiteName = "";
                                        String tcName = "";

                                        while (checkdInfo.next()) {
                                            id = checkdInfo.getInt("id");
                                            suiteName = checkdInfo
                                                    .getString("testsuite");
                                            tcName = checkdInfo
                                                    .getString("testcase");

                                            if (((String) checkBox_array
                                                    .get(id).getData())
                                                    .equals(suiteName)
                                                    && checkBox_array.get(id)
                                                            .getText()
                                                            .equals(tcName)) {
                                                if (checkBox_array.get(id)
                                                        .getSelection() == false) {

                                                    checkBox_array.get(id)
                                                            .setSelection(true);
                                                    tree.getItem(id)
                                                            .setImage(
                                                                    statustextColnumber,
                                                                    Activator
                                                                            .getImageDescriptor(
                                                                                    "icons/nottested.gif")
                                                                            .createImage());
                                                    tree.getItem(id)
                                                            .setText(
                                                                    statustextColnumber,
                                                                    UIConst.STATUS_NOT_TESTED);
                                                }
                                            } else {
                                                System.err
                                                        .println("TC List order does not match the DB.");
                                            }

                                        }
                                    } catch (SQLException e) {
                                        e.printStackTrace();
                                        MultiStatus status = UIConst.createMultiStatus(
                                                e.getLocalizedMessage(), e);
                                        ErrorDialog
                                                .openError(
                                                        Display.getDefault()
                                                                .getActiveShell(),
                                                        "Database Error",
                                                        "Sorry, you can't use this Test Plan files.\nTest Plan : "
                                                                + findName
                                                                + "\nPlease delete it and create a new test plan.\nBecause Test Plan DB schema has changed.\nIf you Create a new Test plan, you will have no problem.",
                                                        status);
                                    } catch (ClassNotFoundException e) {
                                        e.printStackTrace();
                                    } finally {
                                        try {
                                            if (checkdInfo != null) {
                                                checkdInfo.close();
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
                            });

                        }
                    } else {
                        System.err.println("Test Plan Path ERROR.");
                    }

                    Display.getDefault().syncExec(new Runnable() {
                        @Override
                        public void run() {
                            int sumWidth = columnTestSuite.getWidth()
                                    + columnTestCase.getWidth()
                                    + columnStatus.getWidth();
                            System.out.println("sumWidth = " + sumWidth);
                            form.setText("Test Plan Specification - "
                                    + findName);
                            form.getToolBarManager().update(true);
                        }
                    });
                    monitor.done();
                }
            }

        };

        try {
            progressMonitorDlg.run(true, false, runnableProgress);
        } catch (InvocationTargetException e1) {
            e1.printStackTrace();
        } catch (InterruptedException e1) {
            e1.printStackTrace();
        }

    }

    public void addTestCases(
            LinkedHashMap<Integer, LinkedHashMap<String, String>> list,
            String testsuite) {

        String keyID = "TC ID";
        String keyTCName = "TC NAME";
        String keyTCFullName = "TC FULL NAME";
        lengthArray += list.size();
        System.out.println("length_array = " + lengthArray);

        Display.getDefault().syncExec(new Runnable() {
            @Override
            public void run() {

                for (int i = 0; i < list.size(); i++) {
                    TreeItem item = new TreeItem(tree, SWT.MULTI | SWT.WRAP
                            | SWT.RESIZE);

                    String[] itemValues = new String[] { "", "", "", "" };
                    itemValues[idColnumber] = list.get(i).get(keyID);
                    itemValues[testcaseColnumber] = "";
                    itemValues[statustextColnumber] = "";

                    item.setText(itemValues);
                    item.setFont(new Font(Display.getCurrent(), "Ubuntu", 11,
                            SWT.BOLD));
                    item.setBackground(new Color(null, 255, 235, 225));
                    setCheckBox(item, i, list.get(i).get(keyTCName), testsuite);

                    LinkedHashMap<String, String> map = list.get(i);
                    Iterator<String> iterator = map.keySet().iterator();

                    while (iterator.hasNext()) {

                        String key = (String) iterator.next();
                        if (key.equals(keyTCName) || key.equals(keyID)
                                || key.equals(keyTCFullName)) {
                            continue;
                        }
                        final TreeItem subItem = new TreeItem(item, SWT.MULTI
                                | SWT.WRAP | SWT.RESIZE);
                        String value = "";
                        if (map.get(key) != null) {
                            value = map.get(key).replaceAll(
                                    "(\\\\r\\\\n|\\\\n)", "\n");
                        }

                        int columnWidth = tree.getColumn(testcaseColnumber)
                                .getWidth();
                        String[] subitemValues = new String[] { "", "", "", "" };
                        subitemValues[statustextColnumber] = "";
                        subitemValues[idColnumber] = key;
                        long startTime = System.currentTimeMillis();
                        subitemValues[testcaseColnumber] = autoMultiline(tree,
                                value, columnWidth);
                        long lTime = System.currentTimeMillis() - startTime;
                        subItem.setText(subitemValues);
                        subItem.setData(value);

                    }
                }
            }
        });
    }

    private String autoMultiline(Composite parent, String str, int width) {

        long startTime = System.currentTimeMillis();
        int stringWidth = gc.stringExtent(str).x;
        long lTime = System.currentTimeMillis() - startTime;
        int mHint = 30;
        int length = 0;
        String result = "";

        if (stringWidth > (width - mHint)) {
            startTime = System.currentTimeMillis();
            String[] split = str.split("\n");
            lTime = System.currentTimeMillis() - startTime;
            if (split.length > 1) {
                for (int i = 0; i < split.length; i++) {
                    startTime = System.currentTimeMillis();
                    length = gc.stringExtent(split[i]).x;
                    if (length > (width - mHint)) {
                        split[i] = addLinefeed(split[i], width - mHint);
                    }
                    result += split[i];
                    if (i < split.length - 1) {
                        result += "\n";
                    }
                    lTime = System.currentTimeMillis() - startTime;
                }
            } else {
                startTime = System.currentTimeMillis();
                if (stringWidth > (width - mHint)) {
                    result = addLinefeed(str, width - mHint);
                } else {
                    result = str;
                }
                lTime = System.currentTimeMillis() - startTime;
            }
        } else {
            result = str;
        }
        startTime = System.currentTimeMillis();
        lTime = System.currentTimeMillis() - startTime;
        return result;
    }

    private String addLinefeed(String str, int width) {

        String[] split = str.split(" ");
        String result = "";
        int lengthCurrent = 0;
        int lengthTotal = 0;
        int lengthSpace = gc.stringExtent(" ").x;

        for (int i = 0; i < split.length; i++) {
            lengthCurrent = gc.stringExtent(split[i]).x;

            if (lengthTotal + lengthCurrent > width) {
                result += "\n" + split[i];
                lengthTotal = 0;
            } else {
                result += split[i] + " ";
                lengthTotal += lengthCurrent + lengthSpace;
            }
        }

        return result;
    }

    public void clearTree() {
        tree.removeAll();
    }

    public void setCheckBox(TreeItem item, int number, String str,
            String strSuite) {
        TreeEditor editor = new TreeEditor(tree);
        editor.grabHorizontal = editor.grabVertical = true;
        Button check = new Button(tree, SWT.CHECK);
        check.setBackground(item.getBackground());
        check.setText(str);
        check.setData(strSuite);
        check.setToolTipText("[TEST SUITE] " + strSuite + "\n[TEST CASE] "
                + str);
        check.setFont(new Font(Display.getCurrent(), "Ubuntu", 11, SWT.BOLD));

        checkBox_array.add(check);
        editor.setEditor(check, item, testcaseColnumber);
    }

    public void updateCheckBox() {
        Statement stat = null;
        Connection conn = null;
        PreparedStatement prepUpdate = null;
        try {
            conn = DriverManager.getConnection("jdbc:sqlite:"
                    + UIConst.ROOT_PATH + UIConst.TESTPLAN_PATH + "/"
                    + getEditorInput().getName() + "/"
                    + getEditorInput().getName() + ".db");
            stat = conn.createStatement();
            prepUpdate = conn
                    .prepareStatement("UPDATE tcinfo SET checked = ? WHERE id = ? AND testsuite = ? AND testcase = ?;");
            for (int i = 0; i < checkBox_array.size(); i++) {
                boolean checked = checkBox_array.get(i).getSelection();
                String checkedStr = null;
                if (checked) {
                    checkedStr = "true";
                } else {
                    checkedStr = "false";
                }

                prepUpdate.setString(1, checkedStr);
                prepUpdate.setInt(2, i);
                prepUpdate.setString(3, (String) checkBox_array.get(i)
                        .getData());
                prepUpdate.setString(4, checkBox_array.get(i).getText());
                prepUpdate.addBatch();
                System.out.println(i + " " + checkedStr);
            }
            conn.setAutoCommit(false);
            prepUpdate.executeBatch();
            conn.setAutoCommit(true);

            MessageBox box = new MessageBox(PlatformUI.getWorkbench()
                    .getActiveWorkbenchWindow().getShell(), SWT.ICON_WORKING
                    | SWT.OK);
            box.setMessage("Success to update this Test Plan.");
            box.setText("Information");
            box.open();

        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (prepUpdate != null) {
                    prepUpdate.close();
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

    public void setStatusOnRuntime(final String testsuite,
            final String testcase, String status) {
        Display.getDefault().asyncExec(new Runnable() {
            @Override
            public void run() {
                if (status.equals(UIConst.STATUS_DONE)) {

                    for (int i = 0; i < checkBox_array.size(); i++) {
                        if (tree.getItem(i).getText(statustextColnumber)
                                .equals(UIConst.STATUS_DONE)) {
                            continue;
                        }

                        String testsuite_remove_ext = ((String) checkBox_array
                                .get(i).getData()).split(UIConst.ROBOT_EXT)[0];
                        testsuite_remove_ext = testsuite_remove_ext.replaceAll(
                                "_", " ").toLowerCase();
                        if (testsuite_remove_ext.equals(testsuite.toLowerCase())
                                && checkBox_array.get(i).getText()
                                        .equals(testcase)) {
                            tree.getItem(i).setText(statustextColnumber,
                                    UIConst.STATUS_DONE);
                            tree.getItem(i).setImage(
                                    statustextColnumber,
                                    Activator.getImageDescriptor(
                                            "icons/exclamation.png")
                                            .createImage());
                            setSelectionItem(tree.getItem(i));
                            break;
                        }
                    }
                } else if (status.equals(UIConst.STATUS_IN_PROGRESS)) {
                    for (int i = 0; i < checkBox_array.size(); i++) {
                        if (tree.getItem(i).getText(statustextColnumber)
                                .equals(UIConst.STATUS_IN_PROGRESS)) {
                            continue;
                        }
                        String testsuite_remove_ext = ((String) checkBox_array
                                .get(i).getData()).split(UIConst.ROBOT_EXT)[0];
                        testsuite_remove_ext = testsuite_remove_ext.replaceAll(
                                "_", " ").toLowerCase();
                        if (testsuite_remove_ext.equals(testsuite.toLowerCase())
                                && checkBox_array.get(i).getText()
                                        .equals(testcase)) {
                            tree.getItem(i).setText(statustextColnumber,
                                    UIConst.STATUS_IN_PROGRESS);
                            tree.getItem(i).setImage(
                                    statustextColnumber,
                                    Activator.getImageDescriptor(
                                            "icons/play2.png").createImage());
                            setSelectionItem(tree.getItem(i));
                            break;
                        }
                    }
                } else if (status.equals(UIConst.STATUS_PASS)) {
                    for (int i = 0; i < checkBox_array.size(); i++) {
                        if (tree.getItem(i).getText(statustextColnumber)
                                .equals(UIConst.STATUS_PASS)) {
                            continue;
                        }
                        String testsuite_remove_ext = ((String) checkBox_array
                                .get(i).getData()).split(UIConst.ROBOT_EXT)[0];
                        testsuite_remove_ext = testsuite_remove_ext.replaceAll(
                                "_", " ").toLowerCase();
                        if (testsuite_remove_ext.equals(testsuite.toLowerCase())
                                && checkBox_array.get(i).getText()
                                        .equals(testcase)) {
                            tree.getItem(i).setText(statustextColnumber,
                                    UIConst.STATUS_PASS);
                            tree.getItem(i).setImage(
                                    statustextColnumber,
                                    Activator.getImageDescriptor(
                                            "icons/pass.png").createImage());
                            setSelectionItem(tree.getItem(i));
                            break;
                        }
                    }
                } else if (status.equals(UIConst.STATUS_FAIL)) {
                    for (int i = 0; i < checkBox_array.size(); i++) {
                        if (tree.getItem(i).getText(statustextColnumber)
                                .equals(UIConst.STATUS_FAIL)) {
                            continue;
                        }
                        String testsuite_remove_ext = ((String) checkBox_array
                                .get(i).getData()).split(UIConst.ROBOT_EXT)[0];
                        testsuite_remove_ext = testsuite_remove_ext.replaceAll(
                                "_", " ").toLowerCase();
                        if (testsuite_remove_ext.equals(testsuite.toLowerCase())
                                && checkBox_array.get(i).getText()
                                        .equals(testcase)) {
                            tree.getItem(i).setText(statustextColnumber,
                                    UIConst.STATUS_FAIL);
                            tree.getItem(i).setImage(
                                    statustextColnumber,
                                    Activator.getImageDescriptor(
                                            "icons/fail.png").createImage());
                            setSelectionItem(tree.getItem(i));
                            break;
                        }
                    }
                }
            }
        });

    }

    public void setInitStatus() {
        Display.getDefault().asyncExec(new Runnable() {
            @Override
            public void run() {
                for (int i = 0; i < checkBox_array.size(); i++) {
                    checkBox_array.get(i).setSelection(false);
                }

                // Get DB data
                Statement stat = null;
                Connection conn = null;
                String testCase = "";
                String testSuite = "";
                ResultSet checkdInfo = null;

                try {
                    Class.forName("org.sqlite.JDBC");
                    conn = DriverManager.getConnection("jdbc:sqlite:"
                            + UIConst.ROOT_PATH + UIConst.TESTPLAN_PATH + "/"
                            + planName + "/" + planName + ".db");
                    stat = conn.createStatement();

                    checkdInfo = stat
                            .executeQuery("select * from tcinfo where checked = 'true'");
                    while (checkdInfo.next()) {
                        testSuite = checkdInfo.getString("testsuite");
                        testSuite = testSuite.split(UIConst.ROBOT_EXT)[0]
                                .replaceAll("_", " ");
                        testCase = checkdInfo.getString("testcase").trim();
                        for (int i = 0; i < checkBox_array.size(); i++) {
                            String testsuite_remove_ext = ((String) checkBox_array
                                    .get(i).getData()).split(UIConst.ROBOT_EXT)[0];
                            testsuite_remove_ext = testsuite_remove_ext
                                    .replaceAll("_", " ").toLowerCase();
                            if (testsuite_remove_ext.equals(testSuite
                                    .toLowerCase())
                                    && checkBox_array.get(i).getText()
                                            .equals(testCase)) {
                                tree.getItem(i).setText(statustextColnumber,
                                        UIConst.STATUS_NOT_TESTED);
                                tree.getItem(i).setImage(
                                        statustextColnumber,
                                        Activator.getImageDescriptor(
                                                "icons/nottested.gif")
                                                .createImage());
                                checkBox_array.get(i).setSelection(true);
                                break;
                            }
                        }
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                } finally {
                    try {
                        if (checkdInfo != null) {
                            checkdInfo.close();
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
        });
    }

    private static String getTCNameByDebugLog(String str) {
        String tcname = "";

        return tcname;
    }

    @Override
    public void dispose() {
        super.dispose();
    }

    public void setSelectionItem(TreeItem item) {
        ScrolledForm form = getManagedForm().getForm();
        Rectangle bound = null;
        tree.setSelection(item);
        bound = item.getBounds();
        form.setOrigin(bound.x, bound.y);
    }
}
