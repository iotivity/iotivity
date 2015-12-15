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
import oic.simulator.serviceprovider.listener.ISelectionChangedUIListener;
import oic.simulator.serviceprovider.manager.ResourceManager;
import oic.simulator.serviceprovider.manager.UiListenerHandler;
import oic.simulator.serviceprovider.model.Device;
import oic.simulator.serviceprovider.model.MetaProperty;
import oic.simulator.serviceprovider.model.Resource;
import oic.simulator.serviceprovider.utils.Constants;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.ColumnLabelProvider;
import org.eclipse.jface.viewers.EditingSupport;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.StyledCellLabelProvider;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TableViewerColumn;
import org.eclipse.jface.viewers.TextCellEditor;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerCell;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Table;
import org.eclipse.ui.part.ViewPart;
import org.oic.simulator.SimulatorException;

/**
 * This class manages and shows the meta properties view in the perspective.
 */
public class MetaPropertiesView extends ViewPart {

    public static final String          VIEW_ID       = "oic.simulator.serviceprovider.view.metaproperties";

    private TableViewer                 tableViewer;

    private final String[]              columnHeaders = { "Property", "Value" };

    private final Integer[]             columnWidth   = { 150, 150 };

    private ISelectionChangedUIListener resourceSelectionChangedListener;

    private ResourceManager             resourceManagerRef;

    private List<MetaProperty>          properties;

    private boolean                     enable_edit;
    private Button                      editBtn;
    private Button                      cancelBtn;

    public MetaPropertiesView() {

        resourceManagerRef = Activator.getDefault().getResourceManager();

        resourceSelectionChangedListener = new ISelectionChangedUIListener() {

            @Override
            public void onResourceSelectionChange(final Resource resource) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (null != tableViewer) {
                            properties = getData(resource);
                            updateViewer(properties);
                        }
                        updateEditControls(resource);
                    }
                });
            }

            @Override
            public void onDeviceSelectionChange(final Device dev) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (null != tableViewer) {
                            properties = getData(dev);
                            updateViewer(properties);
                        }
                        updateEditControls(dev);
                    }
                });
            }
        };
    }

    @Override
    public void createPartControl(final Composite parent) {
        parent.setLayout(new GridLayout(2, false));

        tableViewer = new TableViewer(parent, SWT.SINGLE | SWT.H_SCROLL
                | SWT.V_SCROLL | SWT.FULL_SELECTION | SWT.BORDER);

        createColumns(tableViewer);

        // Make lines and header visible
        final Table table = tableViewer.getTable();
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        gd.horizontalSpan = 2;
        table.setLayoutData(gd);
        table.setHeaderVisible(true);
        table.setLinesVisible(true);

        tableViewer.setContentProvider(new PropertycontentProvider());

        editBtn = new Button(parent, SWT.PUSH);
        editBtn.setText("Edit");
        gd = new GridData();
        gd.widthHint = 50;
        editBtn.setLayoutData(gd);
        editBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                if (editBtn.getText().equals("Edit")) {
                    cancelBtn.setEnabled(true);
                    editBtn.setText("Save");
                    enable_edit = true;
                } else {
                    boolean result = false;
                    Resource resourceInSelection = resourceManagerRef
                            .getCurrentResourceInSelection();
                    if (null != resourceInSelection) {

                        // Null Check
                        result = resourceManagerRef.isPropertyValueInvalid(
                                resourceInSelection, properties,
                                Constants.RESOURCE_URI);
                        if (result) {
                            MessageDialog.openError(parent.getShell(),
                                    "Invalid Input", "Resource URI is invalid.");
                            return;
                        }

                        result = resourceManagerRef.isPropertyValueInvalid(
                                resourceInSelection, properties,
                                Constants.RESOURCE_NAME);
                        if (result) {
                            MessageDialog.openError(parent.getShell(),
                                    "Invalid Input",
                                    "Resource Name is invalid.");
                            return;
                        }

                        boolean update = false;
                        boolean uriChange = false;
                        boolean nameChange = false;
                        if (resourceManagerRef.isPropValueChanged(
                                resourceInSelection, properties,
                                Constants.RESOURCE_NAME)) {
                            update = true;
                            nameChange = true;
                        }
                        if (resourceManagerRef.isPropValueChanged(
                                resourceInSelection, properties,
                                Constants.RESOURCE_URI)) {
                            // Check whether the new URI is unique.
                            if (!resourceManagerRef.isUriUnique(properties)) {
                                MessageDialog.openError(parent.getShell(),
                                        "Resource URI in use",
                                        "Resource URI is in use. Please try a different URI.");
                                return;
                            }

                            update = true;
                            uriChange = true;

                            if (resourceManagerRef
                                    .isResourceStarted(resourceInSelection)) {
                                update = MessageDialog.openQuestion(
                                        parent.getShell(), "Save Details",
                                        "Resource will be restarted to take the changes."
                                                + " Do you want to continue?");
                                if (!update) {
                                    return;
                                }
                            }
                        }
                        if (update) {
                            try {
                                result = Activator
                                        .getDefault()
                                        .getResourceManager()
                                        .updateResourceProperties(
                                                resourceManagerRef
                                                        .getCurrentResourceInSelection(),
                                                properties, uriChange,
                                                nameChange);
                            } catch (SimulatorException ex) {
                                result = false;
                            }
                            if (result) {
                                MessageDialog.openInformation(
                                        parent.getShell(), "Operation status",
                                        "Resource properties updated.");
                            } else {
                                MessageDialog.openInformation(
                                        parent.getShell(), "Operation status",
                                        "Failed to update the resource properties.");

                                // Reset the old property values.
                                properties = getData(resourceManagerRef
                                        .getCurrentResourceInSelection());
                                updateViewer(properties);
                            }
                        }
                    } else {
                        Device dev = resourceManagerRef
                                .getCurrentDeviceInSelection();

                        // Null check
                        result = resourceManagerRef.isPropertyValueInvalid(dev,
                                properties, Constants.DEVICE_NAME);
                        if (result) {
                            MessageDialog.openError(parent.getShell(),
                                    "Invalid Input", "Device Name is invalid.");
                            return;
                        }

                        if (resourceManagerRef.isPropValueChanged(dev,
                                properties, Constants.DEVICE_NAME)) {
                            resourceManagerRef.updateDeviceProperties(dev,
                                    properties);
                        }

                    }
                    cancelBtn.setEnabled(false);
                    editBtn.setText("Edit");
                    enable_edit = false;
                }
            }
        });

        cancelBtn = new Button(parent, SWT.PUSH);
        cancelBtn.setText("Cancel");
        cancelBtn.setEnabled(false);
        gd = new GridData();
        gd.widthHint = 70;
        cancelBtn.setLayoutData(gd);
        cancelBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                Resource res = resourceManagerRef
                        .getCurrentResourceInSelection();
                if (null != res) {
                    properties = getData(res);
                } else {
                    Device dev = resourceManagerRef
                            .getCurrentDeviceInSelection();
                    if (null != dev) {
                        properties = getData(dev);
                    }
                }
                updateViewer(properties);

                cancelBtn.setEnabled(false);
                editBtn.setText("Edit");
                enable_edit = false;
            }
        });

        addManagerListeners();

        // Check whether there is any resource selected already
        Resource resource = resourceManagerRef.getCurrentResourceInSelection();
        properties = getData(resource);
        if (null != properties) {
            updateViewer(properties);
        }
        updateEditControls(resource);
    }

    private void updateEditControls(Object obj) {
        if (!editBtn.isDisposed() && !cancelBtn.isDisposed()) {

            if (editBtn.getText().equals("Save")) {
                editBtn.setText("Edit");
                enable_edit = false;
            }

            if (null == obj) {
                editBtn.setEnabled(false);
            } else {
                editBtn.setEnabled(true);
            }
            cancelBtn.setEnabled(false);
        }
    }

    private List<MetaProperty> getData(Resource resource) {
        if (null != resource) {
            List<MetaProperty> metaPropertyList = resourceManagerRef
                    .getMetaProperties(resource);
            return metaPropertyList;
        } else {
            return null;
        }
    }

    private List<MetaProperty> getData(Device dev) {
        if (null != dev) {
            List<MetaProperty> metaPropertyList = resourceManagerRef
                    .getMetaProperties(dev);
            return metaPropertyList;
        } else {
            return null;
        }
    }

    private void updateViewer(List<MetaProperty> metaPropertyList) {
        if (null != tableViewer) {
            Table tbl = tableViewer.getTable();
            if (null != metaPropertyList) {
                tableViewer.setInput(metaPropertyList.toArray());
                if (!tbl.isDisposed()) {
                    tbl.setLinesVisible(true);
                }
            } else {
                if (!tbl.isDisposed()) {
                    tbl.removeAll();
                    tbl.setLinesVisible(false);
                }
            }
        }
    }

    public void createColumns(TableViewer tableViewer) {
        TableViewerColumn propName = new TableViewerColumn(tableViewer,
                SWT.NONE);
        propName.getColumn().setWidth(columnWidth[0]);
        propName.getColumn().setText(columnHeaders[0]);
        propName.setLabelProvider(new StyledCellLabelProvider() {
            @Override
            public void update(ViewerCell cell) {
                MetaProperty prop = (MetaProperty) cell.getElement();
                cell.setText(prop.getPropName());
                super.update(cell);
            }
        });

        TableViewerColumn propValue = new TableViewerColumn(tableViewer,
                SWT.NONE);
        propValue.getColumn().setWidth(columnWidth[1]);
        propValue.getColumn().setText(columnHeaders[1]);
        propValue.setLabelProvider(new ColumnLabelProvider() {
            @Override
            public String getText(Object element) {
                MetaProperty prop = (MetaProperty) element;
                if (null != prop) {
                    return prop.getPropValue();
                } else {
                    return "";
                }
            }
        });
        propValue.setEditingSupport(new PropValueEditor(tableViewer));
    }

    private void addManagerListeners() {
        UiListenerHandler.getInstance().addResourceSelectionChangedUIListener(
                resourceSelectionChangedListener);
    }

    class PropertycontentProvider implements IStructuredContentProvider {

        @Override
        public void dispose() {
        }

        @Override
        public void inputChanged(Viewer arg0, Object arg1, Object arg2) {
        }

        @Override
        public Object[] getElements(Object element) {
            return (Object[]) element;
        }
    }

    @Override
    public void dispose() {
        // Unregister the listener
        if (null != resourceSelectionChangedListener) {
            UiListenerHandler.getInstance()
                    .removeResourceSelectionChangedUIListener(
                            resourceSelectionChangedListener);
        }
        super.dispose();
    }

    class PropValueEditor extends EditingSupport {

        private final TableViewer viewer;

        public PropValueEditor(TableViewer viewer) {
            super(viewer);
            this.viewer = viewer;
        }

        @Override
        protected boolean canEdit(Object element) {
            return true;
        }

        @Override
        protected CellEditor getCellEditor(Object element) {
            if (!enable_edit) {
                return null;
            }
            // Disabling edit for resource type
            String propName = ((MetaProperty) element).getPropName();
            if (null != propName && propName.equals(Constants.RESOURCE_TYPE)) {
                return null;
            }
            CellEditor editor = new TextCellEditor(viewer.getTable());
            return editor;
        }

        @Override
        protected Object getValue(Object element) {
            return ((MetaProperty) element).getPropValue();
        }

        @Override
        protected void setValue(Object element, Object value) {
            MetaProperty prop = (MetaProperty) element;
            prop.setPropValue(String.valueOf(value));
            viewer.update(element, null);
        }

    }

    @Override
    public void setFocus() {
    }
}