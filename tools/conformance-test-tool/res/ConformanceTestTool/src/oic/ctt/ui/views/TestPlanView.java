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
import oic.ctt.ui.actions.DeleteAction;
import oic.ctt.ui.actions.ImportTestPlanAction;
import oic.ctt.ui.multipages.TestSuiteMultiPageEditor;
import static oic.ctt.ui.types.IDType.*;
import static oic.ctt.ui.types.ToolTipTextType.*;
import static oic.ctt.ui.types.ImageFilePathType.*;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.ViewerSorter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;

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

public class TestPlanView extends ViewPart {

    /**
     * The ID of the view as specified by the extension.
     */
    public static final String  ID                  = TEST_PLAN_VIEW_ID
                                                            .toString();

    private static final String POPUP_MENU_TEXT     = "#PopupMenu";
    private static final String REFRESH_ACTION_TEXT = "Refresh";

    /**
     * This is a callback that will allow us to create the viewer and initialize
     * it.
     */
    TreeViewer                  treeViewer;

    /**
     * The constructor.
     */
    public TestPlanView() {
    }

    public void createPartControl(Composite parent) {

        // set file association
        UIConst.setAssociateEditorToFile("", "txt", TestSuiteMultiPageEditor.ID);

        // Update Test plan view, refresh tree view.
        final Action updateTestPlanView = new Action(REFRESH_ACTION_TEXT) {
            @Override
            public void run() {
                super.run();
                treeViewer.refresh();
            }
        };

        updateTestPlanView.setToolTipText(TOOLTIP_TEXT_REFRESH_TEST_VIEW
                .toString());
        updateTestPlanView
                .setImageDescriptor(Activator
                        .getImageDescriptor(IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_REFRESH_ICON
                                .toString()));
        getViewSite().getActionBars().getToolBarManager()
                .add(updateTestPlanView);

        final ImportTestPlanAction importTestPlanAction = new ImportTestPlanAction();
        getViewSite().getActionBars().getToolBarManager()
                .add(importTestPlanAction);

        final DeleteAction deleteAction = new DeleteAction(
                UIConst.TESTPLAN_PATH);
        getViewSite().getActionBars().getToolBarManager().add(deleteAction);

        treeViewer = new TreeViewer(parent, SWT.BORDER | SWT.MULTI);

        MenuManager menuMgr = new MenuManager(POPUP_MENU_TEXT); //$NON-NLS-1$
        menuMgr.setRemoveAllWhenShown(true);

        menuMgr.addMenuListener(new IMenuListener() {
            @Override
            public void menuAboutToShow(IMenuManager manager) {
                manager.add(importTestPlanAction);
                manager.add(updateTestPlanView);

                if (treeViewer.getSelection() instanceof IStructuredSelection) {
                    IStructuredSelection selection = (IStructuredSelection) treeViewer
                            .getSelection();
                    boolean check = false;

                    for (Object obj : selection.toList()) {
                        if ((obj instanceof File)
                                && (((File) obj).isDirectory())) {
                            check = true;
                        }
                    }

                    if (check) {
                        manager.add(deleteAction);
                    }
                }
            }
        });

        Menu menu = menuMgr.createContextMenu(treeViewer.getTree());
        treeViewer.getTree().setMenu(menu);
        getSite().registerContextMenu(menuMgr, treeViewer);

        treeViewer.setContentProvider(new FileTreeContentProvider(this));
        treeViewer.setLabelProvider(new FileTreeLabelProvider(this));

        // project tree input
        IWorkspace workspace = ResourcesPlugin.getWorkspace();
        IWorkspaceRoot root = workspace.getRoot();

        IProject newProjectHandle = root.getProject(UIConst.TESTPLAN_PATH);

        treeViewer.setSorter(new ViewerSorter());
        treeViewer.setInput(newProjectHandle.getLocation().toFile());

        // get Workbench resouces
        IWorkbench workbench = PlatformUI.getWorkbench();
        IWorkbenchWindow window = workbench.getActiveWorkbenchWindow();
        final IWorkbenchPage page = window.getActivePage();

        IDoubleClickListener doubleClickListener = new DoubleClickListenerThread(
                page, this);
        treeViewer.addDoubleClickListener(doubleClickListener);

        makeActions();
        hookDoubleClickAction();

        treeViewer.getTree().addKeyListener(new KeyListener() {

            @Override
            public void keyPressed(KeyEvent arg0) {
                if (arg0.keyCode == SWT.F5) {
                    updateTestPlanView.run();
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

    private void hookContextMenu() {
        MenuManager menuMgr = new MenuManager(POPUP_MENU_TEXT);
        menuMgr.setRemoveAllWhenShown(true);
        menuMgr.addMenuListener(new IMenuListener() {
            public void menuAboutToShow(IMenuManager manager) {
                TestPlanView.this.fillContextMenu(manager);
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

    public void setFocusCurrentPlan(String planName) {
        treeViewer.refresh();
        File plan = new File(UIConst.ROOT_PATH + UIConst.TESTPLAN_PATH,
                planName);
        treeViewer.setSelection(new StructuredSelection(plan));
    }
}
