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

import java.util.ArrayList;
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
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;

/**
 * This class shows UI for deleting resources.
 */
public class DeleteResourcePage extends WizardPage {

    private CheckboxTreeViewer       collectionTreeViewer;
    private CheckboxTreeViewer       singleTreeViewer;

    private Button                   selectAllSingle;
    private Button                   selectAllCollection;
    private Button                   deselectAllSingle;
    private Button                   deselectAllCollection;

    private List<CollectionResource> collectionSourceList;
    private List<SingleResource>     singleSourceList;

    private Set<CollectionResource>  selectedCollections;
    private Set<SingleResource>      selectedSingles;

    protected DeleteResourcePage() {
        super("Delete Resources");

        singleSourceList = Activator.getDefault().getResourceManager()
                .getSingleResourceList();
        collectionSourceList = Activator.getDefault().getResourceManager()
                .getCollectionResourceList();

        selectedCollections = new HashSet<CollectionResource>();
        selectedSingles = new HashSet<SingleResource>();
    }

    @Override
    public void createControl(Composite parent) {
        setPageComplete(false);
        setTitle("Delete Resources");
        setMessage("Select one or more single and/or collection resources for deletion.");

        Composite container = new Composite(parent, SWT.NONE);
        container.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        GridLayout layout = new GridLayout(5, true);
        container.setLayout(layout);

        createSingleResourcesArea(container);

        createCollectionResourcesArea(container);

        createSelectionControls(container);

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
        lbl.setText("Single Resources:");
        gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        // gd.grabExcessHorizontalSpace = true;
        // gd.horizontalAlignment = SWT.FILL;
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

        singleTreeViewer = new CheckboxTreeViewer(resourceGroup);
        singleTreeViewer.getTree().setBackground(color);
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessVerticalSpace = true;
        gd.verticalAlignment = SWT.FILL;
        // gd.heightHint = 300;
        singleTreeViewer.getTree().setLayoutData(gd);
        singleTreeViewer
                .setContentProvider(new SingleResourceContentProvider());
        singleTreeViewer.setLabelProvider(new TreeLabelProvider());
        singleTreeViewer.setInput(new Object());
        singleTreeViewer.addCheckStateListener(new ICheckStateListener() {

            @Override
            public void checkStateChanged(CheckStateChangedEvent e) {
                SingleResource res = (SingleResource) e.getElement();
                if (null != res) {
                    if (e.getChecked()) {
                        selectedSingles.add(res);
                        System.out.println("Checked" + res.getResourceName());
                    } else {
                        selectedSingles.remove(res);
                        System.out.println("Unchecked:" + res.getResourceName());
                    }
                    setPageComplete(isSelectionDone());
                }
            }
        });
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

        collectionTreeViewer = new CheckboxTreeViewer(resourceGroup);
        collectionTreeViewer.getTree().setBackground(color);
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessVerticalSpace = true;
        gd.verticalAlignment = SWT.FILL;
        // gd.heightHint = 300;
        collectionTreeViewer.getTree().setLayoutData(gd);
        collectionTreeViewer
                .setContentProvider(new CollectionResourceContentProvider());
        collectionTreeViewer.setLabelProvider(new TreeLabelProvider());
        collectionTreeViewer.setInput(new Object());
        collectionTreeViewer.addCheckStateListener(new ICheckStateListener() {

            @Override
            public void checkStateChanged(CheckStateChangedEvent e) {
                CollectionResource res = (CollectionResource) e.getElement();
                if (null != res) {
                    if (e.getChecked()) {
                        selectedCollections.add(res);
                        System.out.println("Checked" + res.getResourceName());
                    } else {
                        selectedCollections.remove(res);
                        System.out.println("Unchecked:" + res.getResourceName());
                    }
                    setPageComplete(isSelectionDone());
                }
            }
        });
    }

    private void createSelectionControls(Composite container) {
        Composite innerComp = new Composite(container, SWT.NULL);
        GridLayout layout = new GridLayout();
        // layout.verticalSpacing = 10;
        layout.marginTop = 15;
        innerComp.setLayout(layout);
        GridData gd = new GridData();
        gd.verticalAlignment = SWT.TOP;
        innerComp.setLayoutData(gd);

        selectAllSingle = new Button(innerComp, SWT.PUSH);
        selectAllSingle.setText("Select All Single");
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        // gd.widthHint = 70;
        selectAllSingle.setLayoutData(gd);
        selectAllSingle.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                check(singleTreeViewer, true);
            }
        });

        selectAllCollection = new Button(innerComp, SWT.PUSH);
        selectAllCollection.setText("Select All Collection");
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        // gd.widthHint = 70;
        selectAllCollection.setLayoutData(gd);
        selectAllCollection.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                check(collectionTreeViewer, true);
            }
        });

        deselectAllSingle = new Button(innerComp, SWT.PUSH);
        deselectAllSingle.setText("Deselect All Single");
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        // gd.widthHint = 70;
        deselectAllSingle.setLayoutData(gd);
        deselectAllSingle.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                check(singleTreeViewer, false);
            }
        });

        deselectAllCollection = new Button(innerComp, SWT.PUSH);
        deselectAllCollection.setText("Deselect All Collection");
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        // gd.widthHint = 70;
        deselectAllCollection.setLayoutData(gd);

        deselectAllCollection.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                check(collectionTreeViewer, false);
            }
        });
    }

    private void check(CheckboxTreeViewer viewer, boolean toCheck) {
        Tree tree = viewer.getTree();
        if (null != tree) {
            TreeItem[] items = tree.getItems();
            if (null != items && items.length > 0) {
                Object obj;
                for (TreeItem item : items) {
                    obj = item.getData();
                    if (null != obj) {
                        viewer.setChecked(obj, toCheck);
                        updateSelectionList((Resource) obj, toCheck);
                    }
                }
                setPageComplete(isSelectionDone());
            }
        }
    }

    private void updateSelectionList(Resource res, boolean isChecked) {
        if (res instanceof SingleResource) {
            if (isChecked) {
                selectedSingles.add((SingleResource) res);
            } else {
                selectedSingles.remove((SingleResource) res);
            }
        } else {
            if (isChecked) {
                selectedCollections.add((CollectionResource) res);
            } else {
                selectedCollections.remove((CollectionResource) res);
            }
        }
    }

    public Set<SingleResource> getSelectedSingleResourcesList() {
        return selectedSingles;
    }

    public Set<CollectionResource> getSelectedCollectionResourcesList() {
        return selectedCollections;
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
            return null;
        }

        @Override
        public Object[] getElements(Object parent) {
            if (null == singleSourceList) {
                singleSourceList = new ArrayList<SingleResource>();
            }
            return singleSourceList.toArray();
        }

        @Override
        public Object getParent(Object child) {
            return null;
        }

        @Override
        public boolean hasChildren(Object parent) {
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
            return null;
        }

        @Override
        public Object[] getElements(Object parent) {
            if (null == collectionSourceList) {
                collectionSourceList = new ArrayList<CollectionResource>();
            }
            return collectionSourceList.toArray();
        }

        @Override
        public Object getParent(Object child) {
            return null;
        }

        @Override
        public boolean hasChildren(Object parent) {
            return false;
        }
    }

    class TreeLabelProvider extends LabelProvider {
        @Override
        public String getText(Object element) {
            Resource res = (Resource) element;
            return res.getResourceName();
        }

        @Override
        public Image getImage(Object element) {
            if (element instanceof CollectionResource) {
                return Activator.getDefault().getImageRegistry()
                        .get(Constants.COLLECTION_RESOURCE);
            } else {
                return Activator.getDefault().getImageRegistry()
                        .get(Constants.SINGLE_RESOURCE);
            }
        }
    }

    public boolean isSelectionDone() {
        boolean done = true;
        Object obj[] = singleTreeViewer.getCheckedElements();
        if (null == obj || obj.length < 1) {
            obj = collectionTreeViewer.getCheckedElements();
            if (null == obj || obj.length < 1) {
                done = false;
            }
        }
        return done;
    }
}