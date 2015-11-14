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
import oic.simulator.serviceprovider.model.MetaProperty;

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
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.part.ViewPart;

/**
 * This class is used to set the platform and device information. The
 * information is only one per stack and hence only one instance can be
 * stored/used at a time.
 */
public class DevicePlatformInfoView extends ViewPart {

    public static final String VIEW_ID       = "oic.simulator.serviceprovider.view.devplatinfo";

    private final String[]     columnHeaders = { "Property", "Value" };

    private final Integer[]    columnWidth   = { 150, 150 };

    private List<MetaProperty> metaProperties;

    private boolean            enable_edit;

    private Text               deviceNameTxt;
    private Button             devEditBtn;
    private Button             devCancelBtn;
    private Button             platEditBtn;
    private Button             platCancelBtn;

    private TableViewer        platformTblViewer;

    private CTabFolder         folder;
    private CTabItem           devicePropTab;
    private CTabItem           platformPropTab;

    @Override
    public void createPartControl(Composite parent) {
        parent.setLayout(new GridLayout());
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        parent.setLayoutData(gd);

        // Create a Tab Folder.
        folder = new CTabFolder(parent, SWT.BORDER);
        gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        folder.setLayoutData(gd);
        folder.setSimple(false);
        folder.setUnselectedCloseVisible(false);
        folder.setUnselectedImageVisible(false);

        createDevicePropertiesTab();

        createPlatformPropertiesTab();

        folder.setSelection(devicePropTab);

        metaProperties = getPlatformPropData();

        updateViewer(metaProperties);
    }

    private void createDevicePropertiesTab() {
        devicePropTab = new CTabItem(folder, SWT.NULL);
        devicePropTab.setText("Device");

        // Adding the group to the folder.
        Group propGroup = new Group(folder, SWT.NONE);

        Color color = Display.getDefault().getSystemColor(SWT.COLOR_WHITE);
        propGroup.setBackground(color);

        propGroup.setLayout(new GridLayout(2, false));
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        gd.horizontalIndent = 5;
        gd.verticalIndent = 5;
        propGroup.setLayoutData(gd);

        Label devNameLbl = new Label(propGroup, SWT.NULL);
        devNameLbl.setText("Device Name");
        gd = new GridData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalSpan = 2;
        devNameLbl.setLayoutData(gd);

        deviceNameTxt = new Text(propGroup, SWT.BORDER);
        deviceNameTxt.setText(Activator.getDefault().getResourceManager()
                .getDeviceName());
        gd = new GridData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalSpan = 2;
        deviceNameTxt.setLayoutData(gd);
        deviceNameTxt.setEnabled(false);

        devEditBtn = new Button(propGroup, SWT.PUSH);
        devEditBtn.setText("Edit");
        gd = new GridData();
        gd.widthHint = 50;
        devEditBtn.setLayoutData(gd);
        devEditBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                if (devEditBtn.getText().equals("Edit")) {
                    deviceNameTxt.setEnabled(true);
                    devCancelBtn.setEnabled(true);

                    deviceNameTxt.selectAll();
                    deviceNameTxt.setFocus();
                    devEditBtn.setText("Save");
                } else {
                    String devName = deviceNameTxt.getText();
                    if (null == devName || devName.trim().length() < 1) {
                        MessageDialog.openError(Display.getDefault()
                                .getActiveShell(), "Invalid Device Name",
                                "Device name is Invalid!!");
                        deviceNameTxt.setFocus();
                        return;
                    }
                    Activator.getDefault().getResourceManager()
                            .setDeviceName(devName);
                    deviceNameTxt.setEnabled(false);
                    devCancelBtn.setEnabled(false);
                    devEditBtn.setText("Edit");
                }
            }
        });

        devCancelBtn = new Button(propGroup, SWT.PUSH);
        devCancelBtn.setText("Cancel");
        devCancelBtn.setEnabled(false);
        gd = new GridData();
        gd.widthHint = 70;
        devCancelBtn.setLayoutData(gd);
        devCancelBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                deviceNameTxt.setText(Activator.getDefault()
                        .getResourceManager().getDeviceName());
                deviceNameTxt.setEnabled(false);
                devCancelBtn.setEnabled(false);
                devEditBtn.setText("Edit");
            }
        });

        devicePropTab.setControl(propGroup);
    }

    private void createPlatformPropertiesTab() {
        platformPropTab = new CTabItem(folder, SWT.NULL);
        platformPropTab.setText("Platform");

        // Adding the group to the folder.
        Group propGroup = new Group(folder, SWT.NONE);

        Color color = Display.getDefault().getSystemColor(SWT.COLOR_WHITE);
        propGroup.setBackground(color);

        propGroup.setLayout(new GridLayout(2, false));
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        propGroup.setLayoutData(gd);

        platformTblViewer = new TableViewer(propGroup, SWT.SINGLE
                | SWT.H_SCROLL | SWT.V_SCROLL | SWT.FULL_SELECTION | SWT.BORDER);

        createColumns(platformTblViewer);

        // Make lines and header visible
        final Table table = platformTblViewer.getTable();
        table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        table.setHeaderVisible(true);
        table.setLinesVisible(true);
        gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        gd.horizontalSpan = 2;
        table.setLayoutData(gd);

        platformTblViewer.setContentProvider(new PropertycontentProvider());

        platEditBtn = new Button(propGroup, SWT.PUSH);
        platEditBtn.setText("Edit");
        gd = new GridData();
        gd.widthHint = 50;
        platEditBtn.setLayoutData(gd);
        platEditBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                if (platEditBtn.getText().equals("Edit")) {
                    platCancelBtn.setEnabled(true);
                    platEditBtn.setText("Save");
                    enable_edit = true;
                } else {
                    Activator.getDefault().getResourceManager()
                            .setPlatformInfo(metaProperties);
                    platCancelBtn.setEnabled(false);
                    platEditBtn.setText("Edit");
                    enable_edit = false;
                }
            }
        });

        platCancelBtn = new Button(propGroup, SWT.PUSH);
        platCancelBtn.setText("Cancel");
        platCancelBtn.setEnabled(false);
        gd = new GridData();
        gd.widthHint = 70;
        platCancelBtn.setLayoutData(gd);
        platCancelBtn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                metaProperties = getPlatformPropData();
                updateViewer(metaProperties);

                platCancelBtn.setEnabled(false);
                platEditBtn.setText("Edit");
                enable_edit = false;
            }
        });

        platformPropTab.setControl(propGroup);
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
        propValue.setEditingSupport(new PropValueEditor(platformTblViewer));
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

    private List<MetaProperty> getPlatformPropData() {
        List<MetaProperty> metaPropertyList = Activator.getDefault()
                .getResourceManager().getPlatformInfo();
        return metaPropertyList;
    }

    private void updateViewer(List<MetaProperty> metaPropertyList) {
        if (null != platformTblViewer) {
            Table tbl = platformTblViewer.getTable();
            if (null != metaPropertyList) {
                platformTblViewer.setInput(metaPropertyList.toArray());
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