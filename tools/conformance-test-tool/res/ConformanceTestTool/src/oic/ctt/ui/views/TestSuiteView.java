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

import oic.ctt.ui.Activator;
import oic.ctt.ui.UIConst;
import oic.ctt.ui.actions.CreateTestPlanAction;
import oic.ctt.ui.actions.CreateTestSuiteAction;
import oic.ctt.ui.actions.DeleteAction;
import oic.ctt.ui.multipages.TestSuiteMultiPageEditor;
import oic.ctt.ui.util.CTLogger;
import static oic.ctt.ui.types.IDType.*;
import static oic.ctt.ui.types.ImageFilePathType.*;
import static oic.ctt.ui.types.ToolTipTextType.*;

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
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.StructuredSelection;
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
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;
import org.slf4j.Logger;

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
    public static final String  ID              = TEST_SUITE_VIEW_ID.toString();

    public static TreeItem      prev_item       = null;

    private static final String POPUP_MENU_TEXT = "#PopupMenu";
    private Logger              logger          = CTLogger.getInstance();

    /*
     * The content provider class is responsible for providing objects to the
     * view. It can wrap existing objects in adapters or simply return objects
     * as-is. These objects may be sensitive to the current input of the view,
     * or ignore it and always show the same content (like Task List, for
     * example).
     */

    static class ViewContentProvider implements IStructuredContentProvider {
        @Override
        public void inputChanged(Viewer v, Object oldInput, Object newInput) {
        }

        @Override
        public void dispose() {
        }

        @Override
        public Object[] getElements(Object parent) {
            return new String[] {};
        }
    }

    static class ViewLabelProvider extends LabelProvider implements
            ITableLabelProvider {
        @Override
        public String getColumnText(Object obj, int index) {
            return getText(obj);
        }

        @Override
        public Image getColumnImage(Object obj, int index) {
            return getImage(obj);
        }

        public Image getImage(Object obj) {
            return PlatformUI.getWorkbench().getSharedImages()
                    .getImage(ISharedImages.IMG_OBJ_ELEMENT);
        }
    }

    private ITreeContentProvider getContentProvider() {
        return ((ITreeContentProvider) treeViewer.getContentProvider());
    }

    static class NameSorter extends ViewerSorter {}

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

        // set file association
        UIConst.setAssociateEditorToFile("", "txt", TestSuiteMultiPageEditor.ID);

        // Update Test suite view, refresh tree view.
        final Action updateTestSuiteView = new Action("Refresh") {
            @Override
            public void run() {
                super.run();
                treeViewer.refresh();
            }
        };

        updateTestSuiteView.setToolTipText(TOOLTIP_TEXT_REFRESH_TEST_VIEW
                .toString());
        updateTestSuiteView
                .setImageDescriptor(Activator
                        .getImageDescriptor(IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_REFRESH_ICON
                                .toString()));
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
            logger.info("newProjectHandle.getLocation()="
                    + newProjectHandle.getLocation());

            treeViewer = new CheckboxTreeViewer(parent, SWT.BORDER | SWT.CHECK);

            if (treeViewer != null) {
                treeViewer
                        .setContentProvider(new FileTreeContentProvider(this));
                treeViewer.setLabelProvider(new FileTreeLabelProvider(this));

                treeViewer.setSorter(new ViewerSorter());
                IPath newProjectHandleLocation = newProjectHandle.getLocation();
                if (newProjectHandleLocation != null) {
                    if (treeViewer != null) {
                        treeViewer.setInput(newProjectHandleLocation.toFile());
                    }
                }
            }

            // get Workbench resources
            IWorkbench workbench = PlatformUI.getWorkbench();
            IWorkbenchWindow window = workbench.getActiveWorkbenchWindow();
            final IWorkbenchPage page = window.getActivePage();

            ICheckStateListener checkStateListener = new CheckStateListenerThread(
                    treeViewer, this, createTestPlanAction);
            treeViewer.addCheckStateListener(checkStateListener);

            IDoubleClickListener doubleClickListener = new DoubleClickListenerThread(
                    page, this);
            treeViewer.addDoubleClickListener(doubleClickListener);

            MenuManager menuMgr = new MenuManager(POPUP_MENU_TEXT);
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

            // build path
            buildPath(item.getParentItem(), builder);
            builder.append('/');
        }
        builder.append(item.getText());
    }

    private void hookContextMenu() {
        MenuManager menuMgr = new MenuManager(POPUP_MENU_TEXT);
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
        File[] children = (File[]) (this.getContentProvider()
                .getChildren(element));

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
