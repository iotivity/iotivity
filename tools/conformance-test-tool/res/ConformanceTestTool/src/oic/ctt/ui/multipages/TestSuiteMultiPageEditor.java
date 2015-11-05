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
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Arrays;
import java.util.LinkedHashMap;

import oic.ctt.ui.UIConst;
import oic.ctt.ui.util.TestCaseParser;
import oic.ctt.ui.views.TestPlanView;
import oic.ctt.ui.views.TestSuiteView;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.MultiStatus;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.IPageChangedListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IEditorReference;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.editors.text.TextEditor;
import org.eclipse.ui.forms.editor.FormEditor;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.part.FileEditorInput;

import com.nitorcreations.robotframework.eclipseide.editors.RobotFrameworkTextfileEditor;

/**
 * An example showing how to create a multi-page editor. This example has 3
 * pages:
 * <ul>
 * <li>page 0 contains a nested text editor.
 * <li>page 1 allows you to change the font used in page 2
 * <li>page 2 shows the words in page 0 in sorted order
 * </ul>
 */
public class TestSuiteMultiPageEditor extends FormEditor implements
        ISelectionListener {

    public static final String ID      = "oic.ctt.ui.multipages.TestSuiteMultiPageEditor";

    private TextEditor         editor;
    public TestSuiteSpecPage   testspecpage;
    int                        tccount = 0;

    /**
     * Creates a multi-page editor example.
     */
    public TestSuiteMultiPageEditor() {
        super();
    }

    public void setEditorTitle(String name) {
        setPartName(name);
    }

    void createPage_Editor() {
        FileInputStream fileInputStream = null;
        try {
            try {
                editor = new RobotFrameworkTextfileEditor();
                fileInputStream = new FileInputStream(UIConst.PROJECT_PATH
                        + "icons/robot.png");
                int index = addPage(editor, getEditorInput());
                setPageText(index, "Source");
                setPageImage(index, new Image(null, fileInputStream));
            } catch (PartInitException e) {
                ErrorDialog.openError(getSite().getShell(),
                        "Error creating nested text editor", null,
                        e.getStatus());
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }
            if (fileInputStream != null) {

                fileInputStream.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Creates the pages of the multi-page editor.
     */

    protected void createPages() {
        FileInputStream fileInputStream = null;
        try {
            try {
                testspecpage = new TestSuiteSpecPage(this, "Editor");
                fileInputStream = new FileInputStream(UIConst.PROJECT_PATH
                        + "icons/settings.gif");
                int index = addPage(testspecpage);
                setPageText(index, "Test Suite Specification");
                setPageImage(index, new Image(null, fileInputStream));

                createPage_Editor();
                setPartName(editor.getTitle());

            } catch (PartInitException | FileNotFoundException e) {
                e.printStackTrace();

            }
            if (fileInputStream != null) {

                fileInputStream.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * The <code>MultiPageEditorPart</code> implementation of this
     * <code>IWorkbenchPart</code> method disposes all nested editors.
     * Subclasses may extend.
     */
    public void dispose() {
        super.dispose();
    }

    /**
     * Saves the multi-page editor's document.
     */
    public void doSave(IProgressMonitor monitor) {
        IEditorPart editorPart = getEditor(1);
        if (editorPart != null) {
            editorPart.doSave(monitor);
            System.out.println("doSave");
            TestCaseParser parser = new TestCaseParser();
            LinkedHashMap<Integer, LinkedHashMap<String, String>> listNew = new LinkedHashMap<Integer, LinkedHashMap<String, String>>();
            IFile ifile = (IFile) getEditorInput().getAdapter(IFile.class);
            String filePath = ifile.getRawLocation().toOSString();
            listNew = parser.getDocumentHashMap(filePath);
            if (listNew != null) {
                if (listNew.equals(testspecpage.list) == false) {
                    System.out.println("Test Case Changed.");
                    removePage(0);
                    FileInputStream fileInputStream = null;
                    try {
                        try {
                            testspecpage = new TestSuiteSpecPage(this, "Editor");
                            fileInputStream = new FileInputStream(
                                    UIConst.PROJECT_PATH + "icons/settings.gif");
                            addPage(0, testspecpage);
                            setPageText(0, "Test Suite Specification");
                            setPageImage(0, new Image(null, fileInputStream));
                            setActivePage(0);
                            setActivePage(1);
                        } catch (PartInitException e) {
                            e.printStackTrace();
                        } catch (FileNotFoundException e) {
                            e.printStackTrace();
                        }
                        if (fileInputStream != null) {
                            fileInputStream.close();
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    if (ifile.getFullPath().toOSString()
                            .startsWith("/" + UIConst.TESTPLAN_PATH)) {
                        System.out.println("Update Test Plan");
                        setNewDatabase(ifile.getParent().getName());

                        IWorkbenchPage[] pages = PlatformUI.getWorkbench()
                                .getActiveWorkbenchWindow().getPages();
                        for (int i = 0; i < pages.length; i++) {
                            IEditorReference[] editors = pages[i]
                                    .getEditorReferences();
                            for (int j = 0; j < editors.length; j++) {
                                try {
                                    if (editors[j].getEditor(true) instanceof TestPlanMultiPageEditor) {
                                        TestPlanMultiPageEditor editor = (TestPlanMultiPageEditor) editors[j]
                                                .getEditor(true);
                                        editor.createPages();
                                        editor.setActivePage("planSpecPage");
                                    }

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
                }
            }
        }
        IWorkbenchPage page = PlatformUI.getWorkbench()
                .getActiveWorkbenchWindow().getActivePage();
        TestSuiteView testSuiteView = (TestSuiteView) page
                .findView(TestSuiteView.ID);
        TestPlanView testPlanView = (TestPlanView) page
                .findView(TestPlanView.ID);
        testSuiteView.refreshTree();
        testPlanView.refreshTree();
    }

    private void setNewDatabase(String planName) {
        File planDir = new File(UIConst.ROOT_PATH + UIConst.TESTPLAN_PATH + "/"
                + planName);
        File[] planFiles = null;
        LinkedHashMap<Integer, LinkedHashMap<String, String>> tcList = new LinkedHashMap<Integer, LinkedHashMap<String, String>>();

        tccount = 0;

        if (planDir.isDirectory()) {
            planFiles = planDir.listFiles();
            if (planFiles != null) {
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
                    insertTCInfoToDatabase(tcList, testsuiteName, planName);
                    System.out.println((i + 1) + " / " + (planFiles.length)
                            + " " + planFiles[i].getName());
                }
            }
        }
    }

    private void insertTCInfoToDatabase(
            LinkedHashMap<Integer, LinkedHashMap<String, String>> tcList,
            String suiteName, String planName) {

        Statement stat = null;
        Connection conn = null;
        PreparedStatement prepInsert = null;

        try {
            Class.forName("org.sqlite.JDBC");
            conn = DriverManager.getConnection("jdbc:sqlite:"
                    + UIConst.ROOT_PATH + UIConst.TESTPLAN_PATH + "/"
                    + planName + "/" + planName + ".db");
            stat = conn.createStatement();
            if (tccount == 0) {
                stat.executeUpdate("drop table if exists tcinfo;");
                stat.executeUpdate("CREATE TABLE tcinfo (id INTEGER PRIMARY_KEY, testsuite TEXT, testcase TEXT, testcase_fullname TEXT, checked BOOLEAN);");
            }
            prepInsert = conn
                    .prepareStatement("INSERT INTO tcinfo VALUES (?, ?, ?, ?, ?);");

            if (tcList != null) {
                for (int j = 0; j < tcList.size(); j++) {
                    prepInsert.setInt(1, tccount);
                    tccount++;
                    prepInsert.setString(2, suiteName);
                    prepInsert.setString(3, tcList.get(j).get("TC NAME"));
                    prepInsert.setString(4, tcList.get(j).get("TC FULL NAME"));
                    prepInsert.setString(5, "true");
                    prepInsert.addBatch();
                }
                conn.setAutoCommit(false);
                prepInsert.executeBatch();
                conn.setAutoCommit(true);
            }
        } catch (Exception e) {
            MultiStatus status = UIConst.createMultiStatus(
                    e.getLocalizedMessage(), e);
            ErrorDialog.openError(Display.getDefault().getActiveShell(),
                    "Error", "Fail to update Database", status);
            e.printStackTrace();
        } finally {
            try {
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

    /**
     * Saves the multi-page editor's document as another file. Also updates the
     * text for page 0's tab, and updates this multi-page editor's input to
     * correspond to the nested editor's.
     */
    public void doSaveAs() {
        IEditorPart editor = getEditor(1);
        if (editor != null) {
            editor.doSaveAs();
            setPageText(1, editor.getTitle());
            setInput(editor.getEditorInput());
        }
    }

    /*
     * (non-Javadoc) Method declared on IEditorPart
     */
    public void gotoMarker(IMarker marker) {
        setActivePage(1);
        IEditorPart editorPart = getEditor(1);
        if (editorPart != null) {
            IDE.gotoMarker(editorPart, marker);
        }
    }

    /*
     * (non-Javadoc) Method declared on IEditorPart.
     */
    public boolean isSaveAsAllowed() {
        return true;
    }

    @Override
    protected void addPages() {

    }

    public void addPageChangedListener(IPageChangedListener listener) {
        super.addPageChangedListener(listener);
    }

    @Override
    public void selectionChanged(IWorkbenchPart paramIWorkbenchPart,
            ISelection paramISelection) {
    }

}
