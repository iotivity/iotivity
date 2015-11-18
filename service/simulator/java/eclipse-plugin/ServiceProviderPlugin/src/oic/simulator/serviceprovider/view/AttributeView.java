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

package oic.simulator.serviceprovider.view;

import java.util.List;

import oic.simulator.serviceprovider.Activator;
import oic.simulator.serviceprovider.listener.IAutomationUIListener;
import oic.simulator.serviceprovider.listener.IResourceModelChangedUIListener;
import oic.simulator.serviceprovider.listener.ISelectionChangedUIListener;
import oic.simulator.serviceprovider.manager.ResourceManager;
import oic.simulator.serviceprovider.manager.UiListenerHandler;
import oic.simulator.serviceprovider.model.AttributeElement;
import oic.simulator.serviceprovider.model.DataChangeListener;
import oic.simulator.serviceprovider.model.Device;
import oic.simulator.serviceprovider.model.LocalResourceAttribute;
import oic.simulator.serviceprovider.model.Resource;
import oic.simulator.serviceprovider.model.ResourceRepresentation;
import oic.simulator.serviceprovider.model.SingleResource;
import oic.simulator.serviceprovider.utils.Constants;
import oic.simulator.serviceprovider.utils.Utility;

import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.TreeViewerColumn;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeColumn;
import org.eclipse.ui.part.ViewPart;
import org.oic.simulator.AttributeValue;
import org.oic.simulator.AttributeValue.TypeInfo;
import org.oic.simulator.AttributeValue.ValueType;
import org.oic.simulator.SimulatorResourceAttribute;

/**
 * This class manages and shows the attribute view in the perspective.
 */
public class AttributeView extends ViewPart {

    public static final String              VIEW_ID        = "oic.simulator.serviceprovider.view.attribute";

    private TreeViewer                      attViewer;

    private AttributeEditingSupport         attributeEditor;

    private ISelectionChangedUIListener     resourceSelectionChangedListener;
    private IResourceModelChangedUIListener resourceModelChangedUIListener;
    private IAutomationUIListener           automationUIListener;

    private final String[]                  attTblHeaders  = { "Name", "Value",
            "Automation"                                  };
    private final Integer[]                 attTblColWidth = { 150, 190, 150 };

    private ResourceManager                 resourceManager;

    public AttributeView() {

        resourceManager = Activator.getDefault().getResourceManager();

        resourceSelectionChangedListener = new ISelectionChangedUIListener() {

            @Override
            public void onResourceSelectionChange(final Resource resource) {
                Display.getDefault().asyncExec(new Runnable() {
                    @Override
                    public void run() {
                        if (null != attViewer) {
                            if (null != resource
                                    && null != resource
                                            .getResourceRepresentation())
                                attViewer.setInput(resource
                                        .getResourceRepresentation());
                            /*
                             * updateViewer(getData(resource)); Tree tree =
                             * attViewer.getTree(); if (!tree.isDisposed()) { if
                             * (null != resource && (resource instanceof
                             * SingleResource && ((SingleResource) resource)
                             * .isResourceAutomationInProgress())) {
                             * tree.setEnabled(false); } else {
                             * tree.setEnabled(true); } }
                             */
                        }
                    }
                });
            }

            @Override
            public void onDeviceSelectionChange(Device dev) {
                Display.getDefault().asyncExec(new Runnable() {
                    @Override
                    public void run() {
                        updateViewer(null);
                    }
                });
            }
        };

        resourceModelChangedUIListener = new IResourceModelChangedUIListener() {

            @Override
            public void onResourceModelChange(final Resource resource) {
                Display.getDefault().asyncExec(new Runnable() {
                    @Override
                    public void run() {
                        // Handle the notification only if it is for the current
                        // resource in selection
                        Resource resourceInSelection = resourceManager
                                .getCurrentResourceInSelection();
                        if (null == resourceInSelection) {
                            return;
                        }
                        /*
                         * if (resource != resourceInSelection) { // This
                         * notification is for a different resource // whose
                         * attributes are not // currently not being shown in
                         * UI. So ignoring this // notification. return; } //
                         * Refresh the table viewers which will display // the
                         * updated values if (null != attViewer) { if (resource
                         * instanceof CollectionResource) {
                         * updateViewer(getData(resource)); } else {
                         * updateViewer(getData(resource)); } }
                         */
                    }
                });
            }
        };

        automationUIListener = new IAutomationUIListener() {

            @Override
            public void onResourceAutomationStart(final SingleResource resource) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (null == resource) {
                            return;
                        }
                        Resource resourceInSelection = resourceManager
                                .getCurrentResourceInSelection();
                        if (null == resourceInSelection) {
                            return;
                        }
                        // Checking whether attributes view is currently
                        // displaying the attributes of the
                        // resource whose automation has just started
                        if (resource == resourceInSelection) {
                            Tree tree;
                            tree = attViewer.getTree();
                            if (!tree.isDisposed()) {
                                attViewer.refresh();

                                // Disabling the table to prevent interactions
                                // during the automation
                                tree.setEnabled(false);
                                tree.deselectAll();
                            }
                        }
                    }
                });
            }

            @Override
            public void onAutomationComplete(final SingleResource resource,
                    final String attName) {
                // This method notifies the completion of attribute level
                // automation.
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (null == resource) {
                            return;
                        }
                        // Check if the given resourceURI is the uri of the
                        // resource whose attributes are currently being
                        // displayed by this view.
                        Resource resourceInSelection = resourceManager
                                .getCurrentResourceInSelection();
                        if (null == resourceInSelection) {
                            return;
                        }
                        if (resource != resourceInSelection) {
                            return;
                        }
                        Tree tree;
                        tree = attViewer.getTree();
                        if (!tree.isDisposed()) {
                            if (null != attName) {
                                // Attribute level automation has stopped
                                LocalResourceAttribute att = resourceManager
                                        .getAttributeByResourceURI(resource,
                                                attName);
                                if (null == att) {
                                    return;
                                } else {
                                    attViewer.update(att, null);
                                }
                            } else {
                                // Resource level automation has stopped
                                // Enabling the table which was disabled at the
                                // beginning of automation
                                tree.setEnabled(true);
                                attViewer.refresh();
                            }
                        }
                    }
                });
            }
        };
    }

    @Override
    public void createPartControl(Composite parent) {
        Color color = Display.getDefault().getSystemColor(SWT.COLOR_WHITE);

        parent.setLayout(new GridLayout());
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        parent.setLayoutData(gd);

        Group attGroup = new Group(parent, SWT.NONE);
        attGroup.setLayout(new GridLayout());
        gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        attGroup.setLayoutData(gd);
        attGroup.setText("Attributes");
        attGroup.setBackground(color);

        Tree addressTree = new Tree(attGroup, SWT.SINGLE | SWT.BORDER
                | SWT.H_SCROLL | SWT.V_SCROLL | SWT.FULL_SELECTION);
        addressTree.setHeaderVisible(true);

        attViewer = new TreeViewer(addressTree);

        createAttributeColumns(attViewer);

        // make lines and header visible
        Tree tree = attViewer.getTree();
        tree.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        tree.setHeaderVisible(true);
        tree.setLinesVisible(true);

        attViewer.setContentProvider(new AttributeContentProvider());
        attViewer.setLabelProvider(new AttributeLabelProvider());

        addManagerListeners();

        // Check whether there is any resource selected already
        Resource resource = resourceManager.getCurrentResourceInSelection();
        if (resource != null) {
            attViewer.setInput(resource.getResourceRepresentation());
        }
    }

    public void createAttributeColumns(TreeViewer viewer) {
        Tree tree = viewer.getTree();

        attributeEditor = new AttributeEditingSupport();

        TreeColumn attName = new TreeColumn(tree, SWT.NONE);
        attName.setWidth(attTblColWidth[0]);
        attName.setText(attTblHeaders[0]);

        TreeColumn attValue = new TreeColumn(tree, SWT.NONE);
        attValue.setWidth(attTblColWidth[1]);
        attValue.setText(attTblHeaders[1]);
        TreeViewerColumn attValueVwrCol = new TreeViewerColumn(attViewer,
                attValue);
        attValueVwrCol.setEditingSupport(attributeEditor
                .createAttributeValueEditor(attViewer));

        TreeColumn automation = new TreeColumn(tree, SWT.NONE);
        automation.setWidth(attTblColWidth[2]);
        automation.setText(attTblHeaders[2]);
        TreeViewerColumn automationVwrCol = new TreeViewerColumn(attViewer,
                automation);
        automationVwrCol.setEditingSupport(attributeEditor
                .createAutomationEditor(attViewer));
    }

    private void addManagerListeners() {
        UiListenerHandler.getInstance().addResourceSelectionChangedUIListener(
                resourceSelectionChangedListener);
        UiListenerHandler.getInstance().addResourceModelChangedUIListener(
                resourceModelChangedUIListener);
        UiListenerHandler.getInstance().addAutomationUIListener(
                automationUIListener);
    }

    private List<LocalResourceAttribute> getData(Resource resource) {
        if (null != resource) {
            List<LocalResourceAttribute> attList = resourceManager
                    .getAttributes((Resource) resource);
            // List<LocalResourceAttribute> attList =
            // Utility.getDummyAttributes();
            return attList;
        } else {
            return null;
        }
    }

    private void updateViewer(List<LocalResourceAttribute> attList) {
        Tree tree = attViewer.getTree();;
        if (null != attList) {
            if (null != tree && !tree.isDisposed()) {
                tree.setLinesVisible(true);
                attViewer.setInput(attList.toArray());
            }
        } else {
            // Clear the attributes table viewer
            if (null != attViewer) {
                if (null != tree && !tree.isDisposed()) {
                    // tbl.deselectAll();
                    tree.removeAll();
                    tree.setLinesVisible(false);
                }
            }
        }
    }

    class AttributeContentProvider implements ITreeContentProvider,
            DataChangeListener {

        private TreeViewer mTreeViewer;

        @Override
        public void dispose() {
        }

        @Override
        public void inputChanged(Viewer viewer, Object oldAttribute,
                Object newAttribute) {
            mTreeViewer = (TreeViewer) viewer;
        }

        @Override
        public Object[] getChildren(Object attribute) {
            if (attribute instanceof AttributeElement) {
                return ((AttributeElement) attribute).getChildren().values()
                        .toArray();
            }

            return new Object[0];
        }

        @Override
        public Object getParent(Object attribute) {
            if (attribute instanceof AttributeElement)
                return ((AttributeElement) attribute).getParent();
            return null;
        }

        @Override
        public boolean hasChildren(Object attribute) {
            if (attribute instanceof AttributeElement)
                return ((AttributeElement) attribute).hasChildren();
            return false;
        }

        @Override
        public Object[] getElements(Object resourceModel) {
            if (resourceModel instanceof ResourceRepresentation) {
                ((ResourceRepresentation) resourceModel).setListener(this);
                return ((ResourceRepresentation) resourceModel).getAttributes()
                        .values().toArray();
            }

            return new Object[0];
        }

        @Override
        public void add(final AttributeElement attribute) {
            Display.getDefault().asyncExec(new Runnable() {
                @Override
                public void run() {
                    mTreeViewer.refresh(attribute.getParent());
                }
            });
        }

        @Override
        public void remove(final AttributeElement attribute) {
            Display.getDefault().asyncExec(new Runnable() {
                @Override
                public void run() {
                    mTreeViewer.refresh(attribute.getParent());
                }
            });
        }

        @Override
        public void update(final AttributeElement attribute) {
            Display.getDefault().asyncExec(new Runnable() {
                @Override
                public void run() {
                    mTreeViewer.update(attribute, null);
                }
            });
        }
    }

    class AttributeLabelProvider implements ITableLabelProvider {

        @Override
        public void addListener(ILabelProviderListener arg0) {
        }

        @Override
        public void dispose() {
        }

        @Override
        public boolean isLabelProperty(Object arg0, String arg1) {
            return false;
        }

        @Override
        public void removeListener(ILabelProviderListener arg0) {

        }

        @Override
        public Image getColumnImage(Object element, int col) {
            if (col == 2) {
                if (element instanceof AttributeElement) {
                    // Ignore for collection resource
                    Resource res = resourceManager
                            .getCurrentResourceInSelection();
                    if (res instanceof SingleResource) {
                        AttributeElement attrElement = (AttributeElement) element;
                        SimulatorResourceAttribute attribute = attrElement
                                .getSimulatorResourceAttribute();
                        TypeInfo type = attribute.value().typeInfo();
                        if (attrElement.isAutoUpdateSupport()
                                && !attrElement.isReadOnly()) {
                            if (attrElement.isAutoUpdateInProgress()) {
                                return Activator.getDefault()
                                        .getImageRegistry()
                                        .get(Constants.CHECKED);
                            } else {
                                return Activator.getDefault()
                                        .getImageRegistry()
                                        .get(Constants.UNCHECKED);
                            }
                        }
                    }
                }
            }
            return null;
        }

        @Override
        public String getColumnText(Object element, int column) {
            if (element instanceof AttributeElement) {
                AttributeElement attrElement = (AttributeElement) element;
                switch (column) {
                    case 0: // Attribute name column
                    {
                        SimulatorResourceAttribute attribute = attrElement
                                .getSimulatorResourceAttribute();
                        return attribute.name();
                    }

                    case 1: // Attribute value column
                    {
                        SimulatorResourceAttribute attribute = attrElement
                                .getSimulatorResourceAttribute();

                        if (attribute.value().typeInfo().mBaseType != ValueType.RESOURCEMODEL)
                            return Utility.getAttributeValueAsString(attribute
                                    .value());
                        return null;
                    }

                    case 2: {
                        // Ignore for collection resource
                        Resource res = resourceManager
                                .getCurrentResourceInSelection();
                        if (res instanceof SingleResource) {
                            SimulatorResourceAttribute attribute = attrElement
                                    .getSimulatorResourceAttribute();
                            TypeInfo type = attribute.value().typeInfo();
                            if (type.mType == AttributeValue.ValueType.ARRAY) {
                                if (type.mBaseType != AttributeValue.ValueType.RESOURCEMODEL) {
                                    return "NA";
                                }
                            } else if (type.mType != AttributeValue.ValueType.RESOURCEMODEL) {
                                Object parent = attrElement.getParent();
                                if (null != parent
                                        && !(parent instanceof ResourceRepresentation)) {
                                    return "NA";
                                } else if (attrElement.isReadOnly()) {
                                    return "Read-only";
                                } else if (attrElement.isAutoUpdateSupport()) {
                                    if (attrElement.isAutoUpdateInProgress())
                                        return Constants.ENABLED;
                                    else
                                        return Constants.DISABLED;
                                }
                            }
                        }

                        return "";
                    }
                }
            }

            return null;
        }

    }

    @Override
    public void dispose() {
        // Unregister the selection listener
        if (null != resourceSelectionChangedListener) {
            UiListenerHandler.getInstance()
                    .removeResourceSelectionChangedUIListener(
                            resourceSelectionChangedListener);
        }

        // Unregister the model change listener
        if (null != resourceModelChangedUIListener) {
            UiListenerHandler.getInstance()
                    .removeResourceModelChangedUIListener(
                            resourceModelChangedUIListener);
        }

        // Unregister the automation complete listener
        if (null != automationUIListener) {
            UiListenerHandler.getInstance().removeAutomationUIListener(
                    automationUIListener);
        }

        super.dispose();
    }

    @Override
    public void setFocus() {

    }
}