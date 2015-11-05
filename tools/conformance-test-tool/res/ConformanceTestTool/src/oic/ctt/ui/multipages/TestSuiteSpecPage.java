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
import java.util.Iterator;
import java.util.LinkedHashMap;

import oic.ctt.ui.UIConst;
import oic.ctt.ui.util.TestCaseParser;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.TouchEvent;
import org.eclipse.swt.events.TouchListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeColumn;
import org.eclipse.swt.widgets.TreeItem;
import org.eclipse.ui.forms.IManagedForm;
import org.eclipse.ui.forms.editor.FormEditor;
import org.eclipse.ui.forms.editor.FormPage;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.ui.forms.widgets.ScrolledForm;
import org.eclipse.ui.part.WorkbenchPart;

public class TestSuiteSpecPage extends FormPage {

    public static final String                                   ID           = "oic.ctt.ui.editors.TestSuiteSpecPage";

    public static String                                         TC_NAME      = null;
    public Tree                                                  tree;
    public static int                                            length_array = 0;
    public GC                                                    gc           = null;
    public LinkedHashMap<Integer, LinkedHashMap<String, String>> list         = new LinkedHashMap<Integer, LinkedHashMap<String, String>>();

    public TestSuiteSpecPage(FormEditor editor, String tcname) {
        super(editor, "first", tcname);
    }

    protected void createFormContent(IManagedForm managedForm) {
        ScrolledForm form = managedForm.getForm();
        final FormToolkit toolkit = managedForm.getToolkit();
        form.setText("Test Suite Specification - " + getPartName());
        managedForm.getForm().getBody()
                .setLayout(new FillLayout(SWT.HORIZONTAL));

        tree = new Tree(managedForm.getForm().getBody(), SWT.BORDER
                | SWT.H_SCROLL | SWT.V_SCROLL | SWT.FULL_SELECTION);

        managedForm.getToolkit().adapt(tree);
        managedForm.getToolkit().paintBordersFor(tree);

        tree.setHeaderVisible(true);
        tree.setLinesVisible(true);

        gc = new GC(new Label(tree, SWT.NONE));

        tree.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent e) {
                TreeItem selectItem = (TreeItem) e.item;
                System.out.println("widgetSelected");
            }
        });
        tree.addTouchListener(new TouchListener() {

            @Override
            public void touch(TouchEvent arg0) {
                System.out.println("addTouchListener");

            }
        });

        if (tree != null) {
            final TreeColumn columnTestSuite = new TreeColumn(tree, SWT.LEFT);
            columnTestSuite.setText("Test Case ID / Test Spec");
            columnTestSuite.setWidth(200);
            final TreeColumn columnTestCase = new TreeColumn(tree, SWT.LEFT);
            columnTestCase.setText("Test Case Name / Spec Description");
            columnTestCase.setWidth(600);

            IWorkspace workspace = ResourcesPlugin.getWorkspace();
            IWorkspaceRoot root = workspace.getRoot();

            String filePath = getEditorInput().toString().split("\\(")[1]
                    .split("\\)")[0];
            File openFile = new File(UIConst.PROJECT_PATH, filePath);
            String projectName = filePath.split("/")[1];

            final IProject newProjectHandle = root.getProject(projectName);
            String findName = UIConst.getIFileFindName(openFile);
            final IFile ifile = (IFile) newProjectHandle.findMember(findName);
            TestCaseParser parser = new TestCaseParser();
            list = parser.getDocumentHashMap(ifile.getLocation().toOSString());
            addTestCases(list);
            String path = ifile.getLocation().toString();
            if (path.contains(UIConst.ROOT_PATH)) {
                form.setText("Test Suite Specification - "
                        + path.split(UIConst.ROOT_PATH)[1]);
            } else {
                form.setText("Test Suite Specification - " + path);
            }

            // If none Test case, Focus to Robot Editor
            if (length_array == 0) {
                System.out.println("length_array==0");
                getEditor().setActiveEditor(
                        getEditor().findEditors(getEditorInput())[0]);
            }
        }
    }

    public void addTestCases(
            LinkedHashMap<Integer, LinkedHashMap<String, String>> list) {

        if (tree != null) {
            tree.removeAll();
        }

        String keyID = "TC ID";
        String keyTCName = "TC NAME";
        String keyTCFullName = "TC FULL NAME";

        length_array = list.size();

        for (int i = 0; i < length_array; i++) {

            TreeItem item = new TreeItem(tree, SWT.NONE);
            item.setText(new String[] { list.get(i).get(keyID),
                    list.get(i).get(keyTCName) });
            item.setFont(new Font(Display.getCurrent(), "Ubuntu", 11, SWT.BOLD));
            item.setBackground(new Color(null, 255, 236, 225));

            LinkedHashMap<String, String> map = list.get(i);
            Iterator<String> iterator = map.keySet().iterator();

            while (iterator.hasNext()) {

                String key = (String) iterator.next();
                if (key.equals(keyTCName) || key.equals(keyID)
                        || key.equals(keyTCFullName)) {
                    continue;
                }
                final TreeItem subItem = new TreeItem(item, SWT.NONE);
                String value = "";
                if (map.get(key) != null) {
                    value = map.get(key).replaceAll("(\\\\r\\\\n|\\\\n)", "\n");
                }

                int columnWidth = tree.getColumn(1).getWidth();
                String[] values = new String[] { key,
                        autoMultiline(tree, value, columnWidth), "" };
                subItem.setText(values);
                subItem.setData(value);
            }
        }
    }

    private String autoMultiline(Composite parent, String str, int width) {
        int stringWidth = gc.stringExtent(str).x;
        int mHint = 30;
        int length = 0;
        String result = "";

        if (stringWidth > (width - mHint)) {
            String[] split = str.split("\n");
            if (split.length > 1) {
                for (int i = 0; i < split.length; i++) {
                    length = gc.stringExtent(split[i]).x;
                    if (length > (width - mHint)) {
                        split[i] = addLinefeed(split[i], width - mHint);
                    }
                    result += split[i];
                    if (i < split.length - 1) {
                        result += "\n";
                    }
                }
            } else {
                if (stringWidth > (width - mHint)) {
                    result = addLinefeed(str, width - mHint);
                } else {
                    result = str;
                }
            }
        } else {
            result = str;
        }

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

    public String getFilePath() {
        System.out.println(((WorkbenchPart) getSite().getPart()).getPartName());
        return getPartName();
    }

    public Tree getTree() {
        return tree;
    }

    public void setSelectionItem(String name) {
        TreeItem[] items = tree.getItems();
        ScrolledForm form = getManagedForm().getForm();
        Rectangle bound = null;
        for (int i = 0; i < items.length; i++) {
            if (items[i].getText(1).equals(name)) {
                tree.setSelection(items[i]);
                bound = items[i].getBounds();
                form.setOrigin(bound.x, bound.y);
                break;
            }
        }
    }

    @Override
    public void dispose() {
        super.dispose();
    }
}
