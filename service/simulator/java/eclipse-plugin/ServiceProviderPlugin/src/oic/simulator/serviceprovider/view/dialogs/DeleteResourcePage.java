/*
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package oic.simulator.serviceprovider.view.dialogs;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

import oic.simulator.serviceprovider.Activator;
import oic.simulator.serviceprovider.model.CollectionResource;
import oic.simulator.serviceprovider.model.Resource;
import oic.simulator.serviceprovider.model.SingleResource;
import oic.simulator.serviceprovider.utils.Constants;

import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;

/**
 * This class shows UI for deleting resources.
 */
public class DeleteResourcePage extends WizardPage {

    private CheckboxTreeViewer       collectionTreeViewer;
    private CheckboxTreeViewer       singleTreeViewer;

    private List<CollectionResource> collectionSourceList;
    private List<SingleResource>     singleSourceList;

    private TreeViewContentHelper    singleTreeViewContentHelper;
    private TreeViewContentHelper    collectionTreeViewContentHelper;

    protected DeleteResourcePage() {
        super("Delete Resources");

        singleSourceList = Activator.getDefault().getResourceManager()
                .getSingleResourceList();
        collectionSourceList = Activator.getDefault().getResourceManager()
                .getCollectionResourceList();
    }

    @Override
    public void createControl(Composite parent) {
        setPageComplete(false);
        setTitle("Delete Resources");
        setMessage("Select one or more single and/or collection resources for deletion.");

        Composite container = new Composite(parent, SWT.NONE);
        container.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        GridLayout layout = new GridLayout(4, true);
        container.setLayout(layout);

        createSingleResourcesArea(container);

        createCollectionResourcesArea(container);

        setControl(container);
    }

    private void createSingleResourcesArea(Composite container) {
        Composite singleContainer = new Composite(container, SWT.NONE);
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        gd.horizontalSpan = 2;
        singleContainer.setLayoutData(gd);
        GridLayout layout = new GridLayout();
        singleContainer.setLayout(layout);

        Label lbl = new Label(singleContainer, SWT.NONE);
        lbl.setText("Simple Resources:");
        gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        lbl.setLayoutData(gd);

        Group resourceGroup = new Group(singleContainer, SWT.NONE);

        Color color = Display.getDefault().getSystemColor(SWT.COLOR_WHITE);

        resourceGroup.setLayout(new GridLayout());
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        gd.heightHint = 300;
        gd.horizontalSpan = 2;
        resourceGroup.setLayoutData(gd);

        if (null == singleSourceList || singleSourceList.isEmpty()) {
            return;
        }

        singleTreeViewer = new CheckboxTreeViewer(resourceGroup);
        singleTreeViewer.getTree().setBackground(color);
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessVerticalSpace = true;
        gd.verticalAlignment = SWT.FILL;

        singleTreeViewer.getTree().setLayoutData(gd);
        singleTreeViewer
                .setContentProvider(new SingleResourceContentProvider());
        singleTreeViewer.setLabelProvider(new TreeLabelProvider());
        singleTreeViewer.setInput(new TreeContentProvider());
        singleTreeViewer.addCheckStateListener(new ICheckStateListener() {

            /*
             * (non-Javadoc)
             * 
             * @see
             * org.eclipse.jface.viewers.ICheckStateListener#checkStateChanged
             * (org.eclipse.jface.viewers.CheckStateChangedEvent)
             */
            @Override
            public void checkStateChanged(CheckStateChangedEvent e) {
                Object element = e.getElement();
                if (element instanceof TreeViewContentHelper) {
                    singleTreeViewer.setGrayed(singleTreeViewContentHelper,
                            false);
                    singleTreeViewer.setChecked(singleTreeViewContentHelper,
                            e.getChecked());
                    singleTreeViewer.setSubtreeChecked(element, e.getChecked());
                } else {
                    Object obj[] = singleTreeViewer.getCheckedElements();
                    if (null != obj && obj.length > 0) {
                        int checkedCount = obj.length;
                        boolean isParentGrayed = singleTreeViewer
                                .getChecked(singleTreeViewContentHelper);
                        boolean isParentChecked = singleTreeViewer
                                .getChecked(singleTreeViewContentHelper);
                        if (isParentChecked || isParentGrayed) {
                            checkedCount--;
                        }
                        if (checkedCount == singleSourceList.size()) {
                            singleTreeViewer.setGrayed(
                                    singleTreeViewContentHelper, false);
                            singleTreeViewer.setChecked(
                                    singleTreeViewContentHelper, true);
                        } else {
                            if (checkedCount > 0) {
                                singleTreeViewer.setGrayed(
                                        singleTreeViewContentHelper, true);
                                singleTreeViewer.setChecked(
                                        singleTreeViewContentHelper, true);
                            } else {
                                singleTreeViewer.setGrayed(
                                        singleTreeViewContentHelper, false);
                                singleTreeViewer.setChecked(
                                        singleTreeViewContentHelper, false);
                            }
                        }
                    }
                }
                setPageComplete(isSelectionDone());
            }
        });
        singleTreeViewer.expandAll();

    }

    private void createCollectionResourcesArea(Composite container) {
        Composite collectionContainer = new Composite(container, SWT.NONE);
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        gd.horizontalSpan = 2;
        collectionContainer.setLayoutData(gd);
        GridLayout layout = new GridLayout();
        collectionContainer.setLayout(layout);

        Label lbl = new Label(collectionContainer, SWT.NONE);
        lbl.setText("Collection Resources:");
        gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        // gd.grabExcessHorizontalSpace = true;
        // gd.horizontalAlignment = SWT.FILL;
        lbl.setLayoutData(gd);

        Group resourceGroup = new Group(collectionContainer, SWT.NONE);

        Color color = Display.getDefault().getSystemColor(SWT.COLOR_WHITE);

        resourceGroup.setLayout(new GridLayout());
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        gd.heightHint = 300;
        gd.horizontalSpan = 2;
        resourceGroup.setLayoutData(gd);

        if (null == collectionSourceList || collectionSourceList.isEmpty()) {
            return;
        }

        collectionTreeViewer = new CheckboxTreeViewer(resourceGroup);
        collectionTreeViewer.getTree().setBackground(color);
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessVerticalSpace = true;
        gd.verticalAlignment = SWT.FILL;

        collectionTreeViewer.getTree().setLayoutData(gd);
        collectionTreeViewer
                .setContentProvider(new CollectionResourceContentProvider());
        collectionTreeViewer.setLabelProvider(new TreeLabelProvider());
        collectionTreeViewer.setInput(new TreeContentProvider());
        collectionTreeViewer.addCheckStateListener(new ICheckStateListener() {

            @Override
            public void checkStateChanged(CheckStateChangedEvent e) {
                Object element = e.getElement();
                if (element instanceof TreeViewContentHelper) {
                    collectionTreeViewer.setGrayed(
                            collectionTreeViewContentHelper, false);
                    collectionTreeViewer.setChecked(
                            collectionTreeViewContentHelper, e.getChecked());
                    collectionTreeViewer.setSubtreeChecked(element,
                            e.getChecked());
                } else {
                    Object obj[] = collectionTreeViewer.getCheckedElements();
                    if (null != obj && obj.length > 0) {
                        int checkedCount = obj.length;
                        boolean isParentGrayed = collectionTreeViewer
                                .getChecked(collectionTreeViewContentHelper);
                        boolean isParentChecked = collectionTreeViewer
                                .getChecked(collectionTreeViewContentHelper);
                        if (isParentChecked || isParentGrayed) {
                            checkedCount--;
                        }
                        if (checkedCount == collectionSourceList.size()) {
                            collectionTreeViewer.setGrayed(
                                    collectionTreeViewContentHelper, false);
                            collectionTreeViewer.setChecked(
                                    collectionTreeViewContentHelper, true);
                        } else {
                            if (checkedCount > 0) {
                                collectionTreeViewer.setGrayed(
                                        collectionTreeViewContentHelper, true);
                                collectionTreeViewer.setChecked(
                                        collectionTreeViewContentHelper, true);
                            } else {
                                collectionTreeViewer.setGrayed(
                                        collectionTreeViewContentHelper, false);
                                collectionTreeViewer.setChecked(
                                        collectionTreeViewContentHelper, false);
                            }
                        }
                    }
                }
                setPageComplete(isSelectionDone());
            }
        });
        collectionTreeViewer.expandAll();
    }

    public Set<SingleResource> getSelectedSingleResourcesList() {
        final Set<SingleResource> singles = new HashSet<SingleResource>();
        Display.getDefault().syncExec(new Runnable() {

            @Override
            public void run() {
                if (null == singleTreeViewer) {
                    return;
                }
                Object selection[] = singleTreeViewer.getCheckedElements();
                if (null == selection || selection.length < 1) {
                    return;
                }
                for (Object obj : selection) {
                    if (obj instanceof Resource) {
                        singles.add((SingleResource) obj);
                    }
                }
            }
        });
        return singles;
    }

    public Set<CollectionResource> getSelectedCollectionResourcesList() {
        final Set<CollectionResource> collections = new HashSet<CollectionResource>();
        Display.getDefault().syncExec(new Runnable() {

            @Override
            public void run() {
                if (null == collectionTreeViewer) {
                    return;
                }
                Object selection[] = collectionTreeViewer.getCheckedElements();
                if (null == selection || selection.length < 1) {
                    return;
                }
                for (Object obj : selection) {
                    if (obj instanceof Resource) {
                        collections.add((CollectionResource) obj);
                    }
                }
            }
        });
        return collections;
    }

    class SingleResourceContentProvider implements ITreeContentProvider {

        @Override
        public void dispose() {
        }

        @Override
        public void inputChanged(Viewer arg0, Object arg1, Object arg2) {
        }

        @Override
        public Object[] getChildren(Object parent) {
            if (parent instanceof TreeViewContentHelper) {
                return ((TreeViewContentHelper) parent).getResources()
                        .toArray();
            }
            return null;
        }

        @Override
        public Object[] getElements(Object parent) {
            Object obj[] = new Object[1];
            singleTreeViewContentHelper = new TreeViewContentHelper(
                    singleSourceList);
            obj[0] = singleTreeViewContentHelper;
            return obj;
        }

        @Override
        public Object getParent(Object child) {
            return null;
        }

        @Override
        public boolean hasChildren(Object parent) {
            if (parent instanceof TreeViewContentHelper) {
                return true;
            }
            return false;
        }
    }

    class CollectionResourceContentProvider implements ITreeContentProvider {

        @Override
        public void dispose() {
        }

        @Override
        public void inputChanged(Viewer arg0, Object arg1, Object arg2) {
        }

        @Override
        public Object[] getChildren(Object parent) {
            if (parent instanceof TreeViewContentHelper) {
                return ((TreeViewContentHelper) parent).getResources()
                        .toArray();
            }
            return null;
        }

        @Override
        public Object[] getElements(Object parent) {
            Object obj[] = new Object[1];
            collectionTreeViewContentHelper = new TreeViewContentHelper(
                    collectionSourceList);
            obj[0] = collectionTreeViewContentHelper;
            return obj;
        }

        @Override
        public Object getParent(Object child) {
            return null;
        }

        @Override
        public boolean hasChildren(Object parent) {
            if (parent instanceof TreeViewContentHelper) {
                return true;
            }
            return false;
        }
    }

    class TreeLabelProvider extends LabelProvider {
        @Override
        public String getText(Object element) {
            if (element instanceof TreeViewContentHelper) {
                return "All";
            }
            Resource res = (Resource) element;
            return res.getResourceName() + " (" + res.getResourceURI() + ")";
        }

        @Override
        public Image getImage(Object element) {
            if (element instanceof CollectionResource) {
                return Activator.getDefault().getImageRegistry()
                        .get(Constants.COLLECTION_RESOURCE);
            } else if (element instanceof SingleResource) {
                return Activator.getDefault().getImageRegistry()
                        .get(Constants.SINGLE_RESOURCE);
            } else {
                return null;
            }
        }
    }

    class TreeViewContentHelper {
        List<? extends Resource> resources;

        public TreeViewContentHelper(List<? extends Resource> resources) {
            this.resources = resources;
        }

        public void setResources(List<? extends Resource> resources) {
            this.resources = resources;
        }

        public List<? extends Resource> getResources() {
            return resources;
        }
    }

    public boolean isSelectionDone() {
        if (null != singleTreeViewer) {
            Object obj[] = singleTreeViewer.getCheckedElements();
            if (null != obj && obj.length > 0) {
                return true;
            }
        }
        if (null != collectionTreeViewer) {
            Object obj[] = collectionTreeViewer.getCheckedElements();
            if (null != obj && obj.length > 0) {
                return true;
            }
        }
        return false;
    }
}