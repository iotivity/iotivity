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
import oic.simulator.serviceprovider.manager.ResourceManager;
import oic.simulator.serviceprovider.model.AttributeElement;
import oic.simulator.serviceprovider.model.AutomationSettingHelper;
import oic.simulator.serviceprovider.model.CollectionResource;
import oic.simulator.serviceprovider.model.Resource;
import oic.simulator.serviceprovider.model.ResourceRepresentation;
import oic.simulator.serviceprovider.model.SingleResource;
import oic.simulator.serviceprovider.utils.AttributeValueBuilder;
import oic.simulator.serviceprovider.utils.Utility;
import oic.simulator.serviceprovider.view.dialogs.AutomationSettingDialog;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.CheckboxCellEditor;
import org.eclipse.jface.viewers.ComboBoxCellEditor;
import org.eclipse.jface.viewers.EditingSupport;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.oic.simulator.AttributeProperty;
import org.oic.simulator.AttributeValue;
import org.oic.simulator.AttributeValue.TypeInfo;
import org.oic.simulator.AttributeValue.ValueType;
import org.oic.simulator.InvalidArgsException;
import org.oic.simulator.SimulatorResourceAttribute;
import org.oic.simulator.server.SimulatorResource.AutoUpdateType;

/**
 * This class provides editing support to the resources attributes table in the
 * attributes view.
 */
public class AttributeEditingSupport {

    private AttributeValueEditor attValueEditor;
    private AutomationEditor     automationEditor;

    public AttributeValueEditor createAttributeValueEditor(TreeViewer viewer) {
        attValueEditor = new AttributeValueEditor(viewer);
        return attValueEditor;
    }

    public AutomationEditor createAutomationEditor(TreeViewer viewer) {
        automationEditor = new AutomationEditor(viewer);
        return automationEditor;
    }

    class AttributeValueEditor extends EditingSupport {

        private final TreeViewer viewer;
        private CCombo           comboBox;

        public AttributeValueEditor(TreeViewer viewer) {
            super(viewer);
            this.viewer = viewer;
        }

        @Override
        protected boolean canEdit(Object arg0) {
            return true;
        }

        @Override
        protected CellEditor getCellEditor(final Object element) {
            ResourceManager resourceManager = Activator.getDefault()
                    .getResourceManager();

            Resource res = resourceManager.getCurrentResourceInSelection();
            if (null == res) {
                return null;
            }

            // If selected resource is a collection, then editor support is not
            // required.
            if (res instanceof CollectionResource) {
                return null;
            }

            final SimulatorResourceAttribute attribute;
            if (!(element instanceof AttributeElement)) {
                return null;
            }

            AttributeElement attributeElement = ((AttributeElement) element);
            attribute = attributeElement.getSimulatorResourceAttribute();
            if (null == attribute) {
                return null;
            }

            // CellEditor is not required as the automation is in progress.
            if (attributeElement.isAutoUpdateInProgress()) {
                return null;
            }

            final AttributeValue val = attribute.value();
            if (null == val) {
                return null;
            }

            final TypeInfo type = val.typeInfo();
            if (type.mBaseType == ValueType.RESOURCEMODEL) {
                return null;
            }

            AttributeProperty prop = attribute.property();
            if (null == prop) {
                return null;
            }

            if (!resourceManager.isAttHasRangeOrAllowedValues(attribute)) {
                return null;
            }

            String values[] = null;
            List<String> valueSet = resourceManager
                    .getAllValuesOfAttribute(attribute);
            values = convertListToStringArray(valueSet);

            ComboBoxCellEditor comboEditor;
            if (type.mType == ValueType.ARRAY) {
                comboEditor = new ComboBoxCellEditor(viewer.getTree(), values);
            } else {
                comboEditor = new ComboBoxCellEditor(viewer.getTree(), values,
                        SWT.READ_ONLY);
            }
            comboBox = (CCombo) comboEditor.getControl();
            comboBox.addModifyListener(new ModifyListener() {

                @Override
                public void modifyText(ModifyEvent event) {
                    if (type.mType == ValueType.ARRAY) {
                        return;
                    }
                    String oldValue = String.valueOf(Utility
                            .getAttributeValueAsString(val));
                    String newValue = comboBox.getText();
                    if (!oldValue.equals(newValue)) {
                        // Get the AttriuteValue from the string
                        AttributeValue value = AttributeValueBuilder.build(
                                newValue, type.mBaseType);
                        boolean invalid = false;
                        if (null == value) {
                            invalid = true;
                        } else {
                            TypeInfo resTypeInfo = value.typeInfo();
                            if (type.mDepth != resTypeInfo.mDepth
                                    || type.mType != resTypeInfo.mType
                                    || type.mBaseType != resTypeInfo.mBaseType) {
                                invalid = true;
                            }
                        }
                        if (invalid) {
                            MessageBox dialog = new MessageBox(viewer.getTree()
                                    .getShell(), SWT.ICON_ERROR | SWT.OK);
                            dialog.setText("Invalid Value");
                            dialog.setMessage("Given value is invalid");
                            dialog.open();
                        } else {
                            updateAttributeValue(attribute, value);
                            MessageBox dialog = new MessageBox(viewer.getTree()
                                    .getShell(), SWT.ICON_QUESTION | SWT.OK
                                    | SWT.CANCEL);
                            dialog.setText("Confirm action");
                            dialog.setMessage("Do you want to modify the value?");
                            int retval = dialog.open();
                            if (retval != SWT.OK) {
                                value = AttributeValueBuilder.build(oldValue,
                                        type.mBaseType);
                                updateAttributeValue(attribute, value);
                            } else {
                                ResourceManager resourceManager;
                                resourceManager = Activator.getDefault()
                                        .getResourceManager();

                                Resource resource = resourceManager
                                        .getCurrentResourceInSelection();

                                SimulatorResourceAttribute result = getResultantValue(value);

                                resourceManager.attributeValueUpdated(
                                        (SingleResource) resource,
                                        result.name(), result.value());

                            }
                        }
                        viewer.update(element, null);
                        comboBox.setVisible(false);
                    }
                }
            });
            return comboEditor;
        }

        @Override
        protected Object getValue(Object element) {
            int indexOfItem = 0;
            SimulatorResourceAttribute att = null;

            if (element instanceof AttributeElement) {
                att = ((AttributeElement) element)
                        .getSimulatorResourceAttribute();
            }

            if (att == null) {
                return 0;
            }

            String valueString = Utility.getAttributeValueAsString(att.value());
            List<String> valueSet = Activator.getDefault().getResourceManager()
                    .getAllValuesOfAttribute(att);
            if (null != valueSet) {
                indexOfItem = valueSet.indexOf(valueString);
            }
            if (indexOfItem == -1) {
                indexOfItem = 0;
            }
            return indexOfItem;
        }

        @Override
        protected void setValue(Object element, Object value) {
            SimulatorResourceAttribute att = null;

            if (element instanceof AttributeElement) {
                att = ((AttributeElement) element)
                        .getSimulatorResourceAttribute();
            }

            if (att == null) {
                return;
            }

            AttributeValue val = att.value();
            if (null == val) {
                return;
            }
            TypeInfo type = val.typeInfo();
            if (type.mType == ValueType.ARRAY) {
                int index;
                try {
                    index = Integer.parseInt(String.valueOf(value));
                } catch (NumberFormatException nfe) {
                    index = -1;
                }
                if (index == -1) {
                    String oldValue = String.valueOf(Utility
                            .getAttributeValueAsString(val));
                    String newValue = comboBox.getText();
                    if (!oldValue.equals(newValue)) {
                        // Get the AttriuteValue from the string
                        AttributeValue attValue = AttributeValueBuilder.build(
                                newValue, type.mBaseType);
                        boolean invalid = false;
                        if (null == attValue) {
                            invalid = true;
                        } else {
                            TypeInfo resTypeInfo = attValue.typeInfo();
                            if (type.mDepth != resTypeInfo.mDepth
                                    || type.mType != resTypeInfo.mType
                                    || type.mBaseType != resTypeInfo.mBaseType) {
                                invalid = true;
                            }
                        }
                        if (invalid) {
                            MessageBox dialog = new MessageBox(viewer.getTree()
                                    .getShell(), SWT.ICON_ERROR | SWT.OK);
                            dialog.setText("Invalid Value");
                            dialog.setMessage("Given value is invalid");
                            dialog.open();
                        } else {
                            updateAttributeValue(att, attValue);
                            MessageBox dialog = new MessageBox(viewer.getTree()
                                    .getShell(), SWT.ICON_QUESTION | SWT.OK
                                    | SWT.CANCEL);
                            dialog.setText("Confirm action");
                            dialog.setMessage("Do you want to modify the value?");
                            int retval = dialog.open();
                            if (retval != SWT.OK) {
                                attValue = AttributeValueBuilder.build(
                                        oldValue, type.mBaseType);
                                updateAttributeValue(att, attValue);
                            } else {
                                ResourceManager resourceManager;
                                resourceManager = Activator.getDefault()
                                        .getResourceManager();

                                Resource resource = resourceManager
                                        .getCurrentResourceInSelection();

                                SimulatorResourceAttribute result = getResultantValue(attValue);

                                resourceManager.attributeValueUpdated(
                                        (SingleResource) resource,
                                        result.name(), result.value());
                            }
                        }
                    }
                }
            }

            viewer.update(element, null);
        }

        public String[] convertListToStringArray(List<String> valueList) {
            String[] strArr;
            if (null != valueList && valueList.size() > 0) {
                strArr = valueList.toArray(new String[1]);
            } else {
                strArr = new String[1];
            }
            return strArr;
        }

        public void updateAttributeValue(SimulatorResourceAttribute att,
                AttributeValue value) {
            att.setValue(value);

            IStructuredSelection selection = (IStructuredSelection) viewer
                    .getSelection();
            if (null == selection) {
                return;
            }

            Object obj = selection.getFirstElement();
            if (null == obj) {
                return;
            }

            Tree t = viewer.getTree();
            TreeItem item = t.getSelection()[0];
            if (null == item) {
                return;
            }

            if (item.getData() instanceof AttributeElement) {
                AttributeElement attributeElement = (AttributeElement) item
                        .getData();
                attributeElement.getSimulatorResourceAttribute()
                        .setValue(value);

                TreeItem parent = item.getParentItem();
                if (null != parent) {
                    Object data = parent.getData();
                    try {
                        ((AttributeElement) data).deepSetChildValue(att);
                    } catch (InvalidArgsException e) {
                        e.printStackTrace();
                    }
                }
            }
        }

        public SimulatorResourceAttribute getResultantValue(
                AttributeValue newValue) {
            IStructuredSelection selection = (IStructuredSelection) viewer
                    .getSelection();
            if (null == selection) {
                return null;
            }

            Object obj = selection.getFirstElement();
            if (null == obj) {
                return null;
            }

            Tree t = viewer.getTree();
            TreeItem item = t.getSelection()[0];
            if (null == item) {
                return null;
            }

            SimulatorResourceAttribute result = null;
            TreeItem parent = item.getParentItem();
            if (null == parent) {
                Object data = item.getData();
                result = ((AttributeElement) data)
                        .getSimulatorResourceAttribute();
            } else {
                while (parent.getParentItem() != null) {
                    parent = parent.getParentItem();
                }

                // Parent will point to the top-level attribute of type
                Object data = parent.getData();
                result = ((AttributeElement) data)
                        .getSimulatorResourceAttribute();
            }

            return result;
        }
    }

    class AutomationEditor extends EditingSupport {

        private final TreeViewer viewer;

        public AutomationEditor(TreeViewer viewer) {
            super(viewer);
            this.viewer = viewer;
        }

        @Override
        protected boolean canEdit(Object arg0) {
            return true;
        }

        @Override
        protected CellEditor getCellEditor(Object element) {
            // CellEditor is not required as the automation is in progress.
            ResourceManager resourceManager = Activator.getDefault()
                    .getResourceManager();
            Resource resource = resourceManager.getCurrentResourceInSelection();

            if (null == resource) {
                return null;
            }

            if (resource instanceof CollectionResource) {
                return null;
            }
            if (((SingleResource) resource).isResourceAutomationInProgress()) {
                return null;
            }

            SimulatorResourceAttribute att = null;
            if (element instanceof AttributeElement) {
                att = ((AttributeElement) element)
                        .getSimulatorResourceAttribute();
            }

            if (null == att) {
                return null;
            }

            AttributeValue val = att.value();
            if (null == val) {
                return null;
            }

            TypeInfo type = val.typeInfo();

            if (type.mType == ValueType.RESOURCEMODEL
                    || type.mType == ValueType.ARRAY) {
                return null;
            }

            Object parent = ((AttributeElement) element).getParent();
            if (null != parent && !(parent instanceof ResourceRepresentation)) {
                return null;
            }

            if (((AttributeElement) element).isReadOnly()) {
                return null;
            }

            return new CheckboxCellEditor(null, SWT.CHECK | SWT.READ_ONLY);
        }

        @Override
        protected Object getValue(Object element) {
            if (element instanceof AttributeElement) {
                return ((AttributeElement) element).isAutoUpdateInProgress();
            }

            return false;
        }

        @Override
        protected void setValue(Object element, Object value) {
            if (!(element instanceof AttributeElement)) {
                return;
            }

            ResourceManager resourceManager = Activator.getDefault()
                    .getResourceManager();
            // As automation depends on the current resource in selection, its
            // presence is being checked.
            Resource resource = resourceManager.getCurrentResourceInSelection();
            if (null == resource) {
                return;
            }

            AttributeElement att = (AttributeElement) element;
            boolean checked = (Boolean) value;
            if (checked) {
                // Start the automation
                // Fetch the settings data
                List<AutomationSettingHelper> automationSettings;
                automationSettings = AutomationSettingHelper
                        .getAutomationSettings(att);

                // Open the settings dialog
                AutomationSettingDialog dialog = new AutomationSettingDialog(
                        viewer.getTree().getShell(), automationSettings);
                dialog.create();
                if (dialog.open() == Window.OK) {
                    String automationType = dialog.getAutomationType();
                    String updateFreq = dialog.getUpdateFrequency();

                    AutoUpdateType autoType = AutoUpdateType
                            .valueOf(automationType);
                    int updFreq = Utility
                            .getUpdateIntervalFromString(updateFreq);
                    int autoId = resourceManager.startAutomation(
                            (SingleResource) resource, att, autoType, updFreq);
                    if (autoId == -1) {
                        MessageDialog.openInformation(Display.getDefault()
                                .getActiveShell(), "Automation Status",
                                "Automation start failed!!");
                    } else {
                        // viewer.update(element, null);
                    }
                }
            } else {
                // Stop the automation
                resourceManager.stopAutomation((SingleResource) resource, att,
                        att.getAutoUpdateId());
                MessageDialog.openInformation(Display.getDefault()
                        .getActiveShell(), "Automation Status",
                        "Automation stopped.");
                // viewer.update(element, null);
            }
        }
    }
}
