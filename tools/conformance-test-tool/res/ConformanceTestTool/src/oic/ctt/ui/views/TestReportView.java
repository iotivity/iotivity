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

public class TestReportView extends ViewPart {

    /**
     * The ID of the view as specified by the extension.
     */
    public static final String  ID                                    = TEST_REPORT_VIEW_ID
                                                                              .toString();

    private static final String EMPTY_FILE_NAME                       = "";
    private static final String UI_XML_MULTI_PAGE_EDITOR_PACKAGE_NAME = "org.eclipse.wst.xml.ui.internal.tabletree.XMLMultiPageEditorPart";
    private static final String UI_DEFAULT_TEXT_EDITOR_PACKAGE_NAME   = "org.eclipse.ui.DefaultTextEditor";
    private static final String UI_BROWSER_EDITOR_PACKAGE_NAME        = "org.eclipse.ui.browser.editor";
    private static final String REFRESH_ACTION_TEXT                   = "Refresh";
    private static final String SHELL_EXT_NAME                        = "sh";
    private static final String LOG_EXT_NAME                          = "log";
    private static final String HTML_EXT_NAME                         = "html";
    private static final String XML_EXT_NAME                          = "xml";
    private static final String POPUP_MENU_TEXT                       = "#PopupMenu";

    /**
     * This is a callback that will allow us to create the viewer and initialize
     * it.
     */
    TreeViewer                  treeViewer;

    /**
     * The constructor.
     */
    public TestReportView() {
    }

    public void createPartControl(Composite parent) {

        UIConst.setAssociateEditorToFile(EMPTY_FILE_NAME, XML_EXT_NAME,
                UI_XML_MULTI_PAGE_EDITOR_PACKAGE_NAME);
        UIConst.setAssociateEditorToFile(EMPTY_FILE_NAME, HTML_EXT_NAME,
                UI_BROWSER_EDITOR_PACKAGE_NAME);
        UIConst.setAssociateEditorToFile(EMPTY_FILE_NAME, LOG_EXT_NAME,
                UI_DEFAULT_TEXT_EDITOR_PACKAGE_NAME);
        UIConst.setAssociateEditorToFile(EMPTY_FILE_NAME, SHELL_EXT_NAME,
                UI_DEFAULT_TEXT_EDITOR_PACKAGE_NAME);

        // Update Test plan view, refresh tree view.
        final Action updateTestReportView = new Action(REFRESH_ACTION_TEXT) {
            @Override
            public void run() {
                super.run();
                treeViewer.refresh();
            }
        };

        updateTestReportView.setToolTipText(TOOLTIP_TEXT_REFRESH_VIEW
                .toString());
        updateTestReportView
                .setImageDescriptor(Activator
                        .getImageDescriptor(IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_REFRESH_ICON
                                .toString()));
        getViewSite().getActionBars().getToolBarManager()
                .add(updateTestReportView);

        final DeleteAction deleteAction = new DeleteAction(
                UIConst.TESTREPORT_PATH);
        getViewSite().getActionBars().getToolBarManager().add(deleteAction);

        treeViewer = new TreeViewer(parent, SWT.BORDER | SWT.MULTI);

        MenuManager menuMgr = new MenuManager(POPUP_MENU_TEXT); //$NON-NLS-1$
        menuMgr.setRemoveAllWhenShown(true);

        menuMgr.addMenuListener(new IMenuListener() {
            @Override
            public void menuAboutToShow(IMenuManager manager) {
                manager.add(updateTestReportView);

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
                        manager.add(deleteAction);
                    }
                }
            }
        });

        Menu menu = menuMgr.createContextMenu(treeViewer.getTree());
        treeViewer.getTree().setMenu(menu);
        getSite().registerContextMenu(menuMgr, treeViewer);

        // project tree input
        IWorkspace workspace = ResourcesPlugin.getWorkspace();
        IWorkspaceRoot root = workspace.getRoot();
        IProject newProjectHandle = root.getProject(UIConst.TESTREPORT_PATH);

        treeViewer.setContentProvider(new FileTreeContentProvider(this));
        treeViewer.setLabelProvider(new FileTreeLabelProvider(this));

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
                    updateTestReportView.run();
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
                TestReportView.this.fillContextMenu(manager);
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
        treeViewer.refresh();
    }

    public ISelection getSelectionTree() {
        return this.treeViewer.getSelection();
    }

    public void refreshTree() {
        this.treeViewer.refresh();
    }
}
