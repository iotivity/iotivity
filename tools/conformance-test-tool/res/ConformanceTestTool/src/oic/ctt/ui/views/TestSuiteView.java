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
package oic.ctt.ui.views;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;

import oic.ctt.ui.Activator;
import oic.ctt.ui.UIConst;
import oic.ctt.ui.actions.CreateTestPlanAction;
import oic.ctt.ui.actions.CreateTestSuiteAction;
import oic.ctt.ui.actions.DeleteAction;
import oic.ctt.ui.multipages.TestSuiteMultiPageEditor;
import oic.ctt.ui.multipages.TestSuiteSpecPage;
import oic.ctt.ui.util.TestCaseParser;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPath;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.FindReplaceDocumentAdapter;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerSorter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.TreeItem;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.part.ViewPart;
import org.eclipse.ui.texteditor.IDocumentProvider;

import com.nitorcreations.robotframework.eclipseide.editors.RobotFrameworkTextfileEditor;

/**
 * This sample class demonstrates how to plug-in a new workbench view. The view
 * shows data obtained from the model. The sample creates a dummy model on the
 * fly, but a real implementation would connect to the model available either in
 * this or another plug-in (e.g. the workspace). The view is connected to the
 * model using a content provider.
 * <p>
 * The view uses a label provider to define how model objects should be
 * presented in the view. Each view can present the same model objects using
 * different labels and icons, if needed. Alternatively, a single label provider
 * can be shared between views in order to ensure that objects of the same type
 * are presented in the same way everywhere.
 * <p>
 */

public class TestSuiteView extends ViewPart {

    /**
     * The ID of the view as specified by the extension.
     */
    public static final String       ID        = "oic.ctt.ui.views.TestSuiteView";
    private static TestSuiteSpecPage specpage  = null;
    public static TreeItem           prev_item = null;

    /*
     * The content provider class is responsible for providing objects to the
     * view. It can wrap existing objects in adapters or simply return objects
     * as-is. These objects may be sensitive to the current input of the view,
     * or ignore it and always show the same content (like Task List, for
     * example).
     */

    static class ViewContentProvider implements IStructuredContentProvider {
        public void inputChanged(Viewer v, Object oldInput, Object newInput) {
        }

        public void dispose() {
        }

        public Object[] getElements(Object parent) {
            return new String[] {};
        }
    }

    static class ViewLabelProvider extends LabelProvider implements
            ITableLabelProvider {
        public String getColumnText(Object obj, int index) {
            return getText(obj);
        }

        public Image getColumnImage(Object obj, int index) {
            return getImage(obj);
        }

        public Image getImage(Object obj) {
            return PlatformUI.getWorkbench().getSharedImages()
                    .getImage(ISharedImages.IMG_OBJ_ELEMENT);
        }
    }

    static class FileTreeContentProvider implements ITreeContentProvider {

        @Override
        public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
        }

        @Override
        public void dispose() {
        }

        @Override
        public Object[] getElements(Object inputElement) {
            return getChildren(inputElement);
        }

        @Override
        public Object[] getChildren(Object parentElement) {
            if (parentElement instanceof File) {
                Object[] child = ((File) parentElement)
                        .listFiles(new FilenameFilter() {

                            @Override
                            public boolean accept(File dir, String name) {
                                if (name.contains(".project")
                                        || name.endsWith("~")) {
                                    return false;
                                } else {
                                    return true;
                                }
                            }
                        });

                File[] tcname = null;

                if (child == null && ((File) parentElement).isFile()) {
                    TestCaseParser parser = new TestCaseParser();
                    LinkedHashMap<Integer, LinkedHashMap<String, String>> list = new LinkedHashMap<Integer, LinkedHashMap<String, String>>();
                    list = parser.getDocumentHashMap(((File) parentElement)
                            .getAbsolutePath());

                    if (list != null) {
                        if (list.size() > 0) {
                            tcname = new File[list.size()];
                            for (int i = 0; i < list.size(); i++) {
                                String tcFullName = list.get(i).get(
                                        "TC FULL NAME");
                                File temp = new File(
                                        ((File) parentElement)
                                                .getAbsolutePath(),
                                        tcFullName);
                                tcname[i] = temp;
                            }
                        }
                    }
                    return tcname == null ? new Object[0] : tcname;
                }
                return child == null ? new Object[0] : child;
            }

            return new Object[0];
        }

        @Override
        public Object getParent(Object element) {
            return ((File) element).getParentFile();
        }

        @Override
        public boolean hasChildren(Object element) {
            Object[] object = getChildren(element);
            if (object == null) {
                System.out.println(element.toString());
            }
            return getChildren(element).length > 0;
        }
    }

    static class FileTreeLabelProvider implements ILabelProvider {
        private List  listeners;
        private Image file;
        private Image dir;
        private Image tc;

        public FileTreeLabelProvider() {
            listeners = new ArrayList();
            FileInputStream[] fileInputStream = new FileInputStream[3];
            try {

                File f = new File(UIConst.PROJECT_PATH + "icons/");
                if (f.exists()) {
                    try {
                        fileInputStream[0] = new FileInputStream(
                                UIConst.PROJECT_PATH + "icons/file2.gif");
                        file = new Image(null, fileInputStream[0]);

                        fileInputStream[1] = new FileInputStream(
                                UIConst.PROJECT_PATH + "icons/folder.gif");
                        dir = new Image(null, fileInputStream[1]);

                        fileInputStream[2] = new FileInputStream(
                                UIConst.PROJECT_PATH + "icons/test.gif");
                        tc = new Image(null, fileInputStream[2]);
                        System.out
                                .println("Found icons directory(PROJECT_PATH). PATH = "
                                        + UIConst.PROJECT_PATH + "icons/");

                    } catch (FileNotFoundException e) {
                        System.out.println("icon error.");
                    }
                }
                for (int i = 0; i < 3; i++) {
                    if (fileInputStream[i] != null) {
                        fileInputStream[i].close();
                    }
                }

            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        public Image getImage(Object arg0) {
            Image image = null;

            if (arg0 instanceof File) {
                if (((File) arg0).isDirectory()) {
                    image = dir;
                } else if (((File) arg0).isFile()) {
                    image = file;
                } else {
                    image = tc;
                }
            }
            return image;
        }

        public String getText(Object arg0) {
            if (arg0 instanceof File) {
                String text = ((File) arg0).getName();
                if (text.length() == 0) {
                    text = ((File) arg0).getPath();
                }
                return text;
            } else {
                return (String) arg0;
            }
        }

        public void addListener(ILabelProviderListener arg0) {
            listeners.add(arg0);
        }

        public void dispose() {
            // Dispose the images
            if (dir != null) {
                dir.dispose();
            }
            if (file != null) {
                file.dispose();
            }
            if (tc != null) {
                tc.dispose();
            }
        }

        public boolean isLabelProperty(Object arg0, String arg1) {
            return false;
        }

        public void removeListener(ILabelProviderListener arg0) {
            listeners.remove(arg0);
        }
    }

    private ITreeContentProvider getContentProvider() {
        return ((ITreeContentProvider) treeViewer.getContentProvider());
    }

    static class NameSorter extends ViewerSorter {

    }

    /**
     * The constructor.
     */
    public TestSuiteView() {
    }

    /**
     * This is a callback that will allow us to create the viewer and initialize
     * it.
     */
    public CheckboxTreeViewer    treeViewer;
    private CreateTestPlanAction createTestPlanAction;

    public void createPartControl(Composite parent) {

        UIConst.setAssociateEditorToFile("", "txt", TestSuiteMultiPageEditor.ID); // set
                                                                                  // file
                                                                                  // association

        // Update Test suite view, refresh tree view.
        final Action updateTestSuiteView = new Action("Refresh") {
            @Override
            public void run() {
                super.run();
                treeViewer.refresh();

            }
        };
        updateTestSuiteView.setToolTipText("Refresh (F5)");
        updateTestSuiteView.setImageDescriptor(Activator
                .getImageDescriptor("icons/refresh.gif"));
        getViewSite().getActionBars().getToolBarManager()
                .add(updateTestSuiteView);

        final CreateTestSuiteAction createTestSuiteAction = new CreateTestSuiteAction();
        getViewSite().getActionBars().getToolBarManager()
                .add(createTestSuiteAction);

        // Check test case or test suite, and make the new test plan action.
        createTestPlanAction = new CreateTestPlanAction();
        createTestPlanAction.setEnabled(false);
        getViewSite().getActionBars().getToolBarManager()
                .add(createTestPlanAction);

        final DeleteAction deleteAction = new DeleteAction(
                UIConst.TESTSUITE_PATH);
        getViewSite().getActionBars().getToolBarManager().add(deleteAction);

        // project tree input
        IWorkspace workspace = ResourcesPlugin.getWorkspace();
        IWorkspaceRoot root = workspace.getRoot();
        IProject newProjectHandle = root.getProject(UIConst.TESTSUITE_PATH);

        if (newProjectHandle != null) {
            System.out.println("newProjectHandle.getLocation()="
                    + newProjectHandle.getLocation());

            treeViewer = new CheckboxTreeViewer(parent, SWT.BORDER | SWT.CHECK);

            if (treeViewer != null) {
                treeViewer.setContentProvider(new FileTreeContentProvider());
                treeViewer.setLabelProvider(new FileTreeLabelProvider());

                treeViewer.setSorter(new ViewerSorter());
                IPath newProjectHandleLocation = newProjectHandle.getLocation();
                if (newProjectHandleLocation != null) {
                    if (treeViewer != null) {
                        treeViewer.setInput(newProjectHandleLocation.toFile());
                    }
                }
            }

            // get Workbench resouces
            IWorkbench workbench = PlatformUI.getWorkbench();
            IWorkbenchWindow window = workbench.getActiveWorkbenchWindow();
            final IWorkbenchPage page = window.getActivePage();

            treeViewer.addCheckStateListener(new ICheckStateListener() {

                public void checkStateChanged(CheckStateChangedEvent e) {
                    if (e.getChecked()) {
                        treeViewer.setSubtreeChecked(e.getElement(), true);
                    } else {
                        treeViewer.setSubtreeChecked(e.getElement(), false);
                    }

                    File element = (File) e.getElement();
                    if (element != null) {
                        if (treeViewer.getChecked(element)) {
                            while (true) {
                                File elementParent = element.getParentFile();
                                if (elementParent != null) {
                                    element = elementParent;
                                    if (element != null) {
                                        if (element.getName().equals(
                                                UIConst.TESTSUITE_PATH)) {
                                            break;
                                        }
                                        System.out.println(element.getName());
                                        treeViewer.setChecked(element, true);
                                    }
                                } else {
                                    break;
                                }
                            }

                        } else {
                            while (true) {
                                if (element != null) {
                                    element = element.getParentFile();
                                    if (element != null) {
                                        System.out.println(element.getName());
                                        if (element.getName().equals(
                                                UIConst.TESTSUITE_PATH)) {
                                            break;
                                        }
                                        boolean check = false;
                                        File[] children = (File[]) ((ITreeContentProvider) treeViewer
                                                .getContentProvider())
                                                .getChildren(element);
                                        for (int i = 0; i < children.length; i++) {
                                            if (treeViewer
                                                    .getChecked(children[i])) {
                                                check = true;
                                                break;
                                            }
                                        }
                                        if (check == false) {
                                            treeViewer.setChecked(element,
                                                    false);
                                        }
                                    } else {
                                        System.out
                                                .println("[Debug] element is null");
                                    }
                                } else {
                                    System.out
                                            .println("[Debug] element is null");
                                }

                            }
                        }

                        if (getSelectedItems().length > 0) {
                            createTestPlanAction.setEnabled(true);
                        } else {
                            createTestPlanAction.setEnabled(false);
                        }
                    }
                }
            });

            treeViewer.addDoubleClickListener(new IDoubleClickListener() {
                public void doubleClick(DoubleClickEvent arg0) {
                    TreeViewer viewer = (TreeViewer) arg0.getViewer();
                    Object obj = ((IStructuredSelection) arg0.getSelection())
                            .getFirstElement();
                    if (viewer.getExpandedState(obj)) {
                        viewer.collapseToLevel(obj, 1);
                    } else {
                        viewer.expandToLevel(obj, 1);
                    }

                    IWorkspace workspace = ResourcesPlugin.getWorkspace();
                    IWorkspaceRoot root = workspace.getRoot();
                    IProject newProjectHandle = root
                            .getProject(UIConst.TESTSUITE_PATH);
                    IStructuredSelection selection = (IStructuredSelection) arg0
                            .getSelection();

                    if (selection.getFirstElement() instanceof File) {
                        File file = (File) selection.getFirstElement();
                        String findName = UIConst.getIFileFindName(file);
                        IFile ifile = newProjectHandle.getFile(findName);

                        if (file.isFile()) {
                            try {
                                IEditorPart editorpart = IDE.openEditor(page,
                                        ifile, true);

                            } catch (PartInitException e) {
                                e.printStackTrace();
                            }
                        } else if (file.isDirectory()) {
                        } else {
                            System.out.println("testcase click :"
                                    + file.getName());
                            findName = UIConst.getIFileFindName(file
                                    .getParentFile());
                            ifile = newProjectHandle.getFile(findName);
                            IEditorPart editorpart = null;

                            try {
                                editorpart = IDE.openEditor(page, ifile, true);

                            } catch (PartInitException e) {
                                e.printStackTrace();
                            }

                            if (editorpart instanceof TestSuiteMultiPageEditor) {
                                TestSuiteMultiPageEditor suiteEditor = (TestSuiteMultiPageEditor) editorpart;
                                if (suiteEditor.getActivePageInstance() instanceof TestSuiteSpecPage) {
                                    TestSuiteSpecPage specpage = (TestSuiteSpecPage) suiteEditor
                                            .getActivePageInstance();
                                    String tcName = "";
                                    String tcFullName = "";
                                    tcFullName = file.getName();
                                    if (tcFullName.contains("]")) {
                                        tcName = tcFullName.split("\\]")[1]
                                                .trim();
                                    } else {
                                        tcName = tcFullName;
                                    }
                                    specpage.setSelectionItem(tcName);

                                } else if (suiteEditor.getActivePageInstance() == null) {
                                    if (suiteEditor.getActiveEditor() instanceof RobotFrameworkTextfileEditor) {
                                        RobotFrameworkTextfileEditor editor = (RobotFrameworkTextfileEditor) suiteEditor
                                                .getActiveEditor();
                                        IDocumentProvider provider = editor
                                                .getDocumentProvider();
                                        IDocument document = provider
                                                .getDocument(editor
                                                        .getEditorInput());
                                        FindReplaceDocumentAdapter documentAdapter = new FindReplaceDocumentAdapter(
                                                document);
                                        try {
                                            IRegion findTextoffset = documentAdapter
                                                    .find(0, file.getName(),
                                                            true, false, false,
                                                            false);
                                            int lineNumber = document
                                                    .getLineOfOffset(findTextoffset
                                                            .getOffset());
                                            int start = document
                                                    .getLineOffset(lineNumber);
                                            editor.selectAndReveal(start, 0);
                                        } catch (BadLocationException e) {
                                            e.printStackTrace();
                                        }

                                    }
                                }
                            }
                        }
                    }
                }
            });

            MenuManager menuMgr = new MenuManager("#PopupMenu");
            menuMgr.setRemoveAllWhenShown(true);

            menuMgr.addMenuListener(new IMenuListener() {
                @Override
                public void menuAboutToShow(IMenuManager manager) {

                    if (treeViewer.getSelection() instanceof IStructuredSelection) {
                        IStructuredSelection selection = (IStructuredSelection) treeViewer
                                .getSelection();
                        boolean check = false;

                        for (Object obj : selection.toList()) {
                            if (obj instanceof File) {
                                if (((File) obj).isDirectory()) {
                                    check = true;
                                }
                            }
                        }
                        if (check) {
                            manager.add(createTestSuiteAction);
                        }
                    }
                    manager.add(updateTestSuiteView);
                }
            });
            Menu menu = menuMgr.createContextMenu(treeViewer.getTree());
            treeViewer.getTree().setMenu(menu);
            getSite().registerContextMenu(menuMgr, treeViewer);

            makeActions();
            hookDoubleClickAction();

            treeViewer.getTree().addKeyListener(new KeyListener() {

                @Override
                public void keyPressed(KeyEvent arg0) {
                    if (arg0.keyCode == SWT.F5) {
                        updateTestSuiteView.run();
                    }
                    if (arg0.keyCode == SWT.DEL) {
                        deleteAction.run();
                    }
                }

                @Override
                public void keyReleased(KeyEvent arg0) {
                }
            });

        }
    }

    public Object[] getSelectedItems() {
        CheckboxTreeViewer checkTreeViewer = (CheckboxTreeViewer) treeViewer;
        return checkTreeViewer.getCheckedElements();
    }

    public static String getPath(TreeItem selectedItem) {
        StringBuilder builder = new StringBuilder();
        buildPath(selectedItem, builder);
        return builder.toString();
    }

    private static void buildPath(TreeItem item, StringBuilder builder) {
        if (item.getParentItem() != null) {

            buildPath(item.getParentItem(), builder); // build path

            builder.append('/');
        }
        builder.append(item.getText());
    }

    private void hookContextMenu() {
        MenuManager menuMgr = new MenuManager("#PopupMenu");
        menuMgr.setRemoveAllWhenShown(true);
        menuMgr.addMenuListener(new IMenuListener() {
            public void menuAboutToShow(IMenuManager manager) {
                TestSuiteView.this.fillContextMenu(manager);
            }
        });
    }

    private void contributeToActionBars() {
        IActionBars bars = getViewSite().getActionBars();
        fillLocalPullDown(bars.getMenuManager());
        fillLocalToolBar(bars.getToolBarManager());
    }

    private void fillLocalPullDown(IMenuManager manager) {
        manager.add(new Separator());
    }

    private void fillContextMenu(IMenuManager manager) {
        // Other plug-ins can contribute there actions here
        manager.add(new Separator(IWorkbenchActionConstants.MB_ADDITIONS));
    }

    private void fillLocalToolBar(IToolBarManager manager) {
    }

    private void makeActions() {
    }

    private void hookDoubleClickAction() {
    }

    /**
     * Passing the focus request to the viewer's control.
     */
    public void setFocus() {
    }

    public void refreshTree() {
        this.treeViewer.refresh();
    }

    public ISelection getSelectionTree() {
        return this.treeViewer.getSelection();
    }

    public boolean getChildrenChecked(File element) {
        boolean check = false;
        File[] children = (File[]) ((ITreeContentProvider) treeViewer
                .getContentProvider()).getChildren(element);

        for (int i = 0; i < children.length; i++) {
            if (treeViewer.getChecked(children[i])) {
                check = true;
            } else {
                check = false;
                break;
            }
        }
        return check;
    }

    public void setFocusCurrentSuite(File suite) {
        treeViewer.setSelection(new StructuredSelection(suite));
    }

    public void setAllUnChecked() {
        Object[] object = treeViewer.getCheckedElements();
        for (int i = 0; i < object.length; i++) {
            treeViewer.setSubtreeChecked(object[i], false);
        }
        getViewSite().getActionBars().getToolBarManager().getItems();
        if (getSelectedItems().length > 0) {
            createTestPlanAction.setEnabled(true);
        } else {
            createTestPlanAction.setEnabled(false);
        }

    }
}
