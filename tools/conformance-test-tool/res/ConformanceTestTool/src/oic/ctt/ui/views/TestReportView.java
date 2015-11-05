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
import java.util.List;

import oic.ctt.ui.Activator;
import oic.ctt.ui.UIConst;
import oic.ctt.ui.actions.DeleteAction;

import org.eclipse.core.resources.IFile;
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
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerSorter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
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
    public static final String ID = "oic.ctt.ui.views.TestReportView";

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
            Object[] child = ((File) parentElement)
                    .listFiles(new FilenameFilter() {

                        @Override
                        public boolean accept(File dir, String name) {
                            if (name.contains(".project") || name.endsWith("~")
                                    || name.contains(".xml")) {
                                return false;
                            } else {
                                return true;
                            }
                        }
                    });
            return child == null ? new Object[0] : child;
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
        private Image graph;

        public FileTreeLabelProvider() {
            listeners = new ArrayList();
            FileInputStream[] fileInputStream = new FileInputStream[4];
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

                        fileInputStream[3] = new FileInputStream(
                                UIConst.PROJECT_PATH + "icons/sequence.gif");
                        graph = new Image(null, fileInputStream[3]);
                    } catch (FileNotFoundException e) {
                        System.out.println("icon error.");
                    }
                    for (int i = 0; i < 4; i++) {
                        if (fileInputStream[i] != null) {
                            fileInputStream[i].close();
                        }
                    }
                }

            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        public Image getImage(Object arg0) {
            if (((File) arg0).isDirectory()) {
                return dir;
            } else {
                String filename = ((File) arg0).getName();
                if (filename.equals(UIConst.FLOWGRAPH_FILE_NAME)) {
                    return graph;
                } else {
                    return file;
                }
            }
        }

        public String getText(Object arg0) {
            if (arg0 instanceof File) {
                String text = ((File) arg0).getName();
                if (((File) arg0).getName().length() == 0) {
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

    /**
     * This is a callback that will allow us to create the viewer and initialize
     * it.
     */
    TreeViewer treeViewer;

    /**
     * The constructor.
     */
    public TestReportView() {
    }

    public void createPartControl(Composite parent) {

        UIConst.setAssociateEditorToFile("", "xml",
                "org.eclipse.wst.xml.ui.internal.tabletree.XMLMultiPageEditorPart");
        UIConst.setAssociateEditorToFile("", "html",
                "org.eclipse.ui.browser.editor");
        UIConst.setAssociateEditorToFile("", "log",
                "org.eclipse.ui.DefaultTextEditor");
        UIConst.setAssociateEditorToFile("", "sh",
                "org.eclipse.ui.DefaultTextEditor");

        // Update Test plan view, refresh tree view.
        final Action updateTestReportView = new Action("Refresh") {
            @Override
            public void run() {
                super.run();
                treeViewer.refresh();
            }
        };
        updateTestReportView.setToolTipText("Refresh (F5)");
        updateTestReportView.setImageDescriptor(Activator
                .getImageDescriptor("icons/refresh.gif"));
        getViewSite().getActionBars().getToolBarManager()
                .add(updateTestReportView);

        final DeleteAction deleteAction = new DeleteAction(
                UIConst.TESTREPORT_PATH);
        getViewSite().getActionBars().getToolBarManager().add(deleteAction);

        treeViewer = new TreeViewer(parent, SWT.BORDER | SWT.MULTI);

        MenuManager menuMgr = new MenuManager("#PopupMenu"); //$NON-NLS-1$
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

        treeViewer.setContentProvider(new FileTreeContentProvider());
        treeViewer.setLabelProvider(new FileTreeLabelProvider());

        treeViewer.setSorter(new ViewerSorter());
        treeViewer.setInput(newProjectHandle.getLocation().toFile());

        // get Workbench resouces
        IWorkbench workbench = PlatformUI.getWorkbench();
        IWorkbenchWindow window = workbench.getActiveWorkbenchWindow();
        final IWorkbenchPage page = window.getActivePage();

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
                        .getProject(UIConst.TESTREPORT_PATH);

                IStructuredSelection selection = (IStructuredSelection) arg0
                        .getSelection();

                if (selection.getFirstElement() instanceof File) {
                    File file = (File) selection.getFirstElement();

                    if (file.isFile()) {
                        String findName = UIConst.getIFileFindName(file);
                        IFile ifile = newProjectHandle.getFile(findName);

                        try {
                            IEditorPart editorpart = IDE.openEditor(page,
                                    ifile, true);

                        } catch (PartInitException e) {
                            e.printStackTrace();
                        }
                    } else if (file.isDirectory()) {
                    } else {
                        MessageBox box = new MessageBox(PlatformUI
                                .getWorkbench().getActiveWorkbenchWindow()
                                .getShell(), SWT.ICON_WARNING | SWT.OK);
                        box.setMessage("Can't open the file. Please select the report file.");
                        box.setText("Information");
                        box.open();
                    }
                } else if (selection.getFirstElement() instanceof String) {
                    System.out
                            .println("[Debug] Selection.getFirstElement is String");
                }
            }
        });

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
        MenuManager menuMgr = new MenuManager("#PopupMenu");
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
