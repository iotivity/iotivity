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

package oic.simulator.clientcontroller.view;

import java.util.List;

import oic.simulator.clientcontroller.Activator;
import oic.simulator.clientcontroller.manager.ResourceManager;
import oic.simulator.clientcontroller.remoteresource.AttributeElement;
import oic.simulator.clientcontroller.remoteresource.RemoteResource;
import oic.simulator.clientcontroller.utils.AttributeValueBuilder;
import oic.simulator.clientcontroller.utils.Utility;
import oic.simulator.clientcontroller.view.dialogs.PostRequestDialog;

import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.CheckboxCellEditor;
import org.eclipse.jface.viewers.ComboBoxCellEditor;
import org.eclipse.jface.viewers.EditingSupport;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.oic.simulator.AttributeValue;
import org.oic.simulator.AttributeValue.TypeInfo;
import org.oic.simulator.AttributeValue.ValueType;
import org.oic.simulator.InvalidArgsException;
import org.oic.simulator.SimulatorResourceAttribute;

/**
 * This class provides editing support to the resources attributes table in the
 * attributes view.
 */
public class AttributeEditingSupport {

    private AttributeValueEditor attValueEditor;
    private PostRequestEditor    postReqEditor;

    public AttributeValueEditor createAttributeValueEditor(TreeViewer viewer,
            TitleAreaDialog dialog) {
        attValueEditor = new AttributeValueEditor(viewer, dialog);
        return attValueEditor;
    }

    public PostRequestEditor createAutomationEditor(TreeViewer viewer) {
        postReqEditor = new PostRequestEditor(viewer);
        return postReqEditor;
    }

    class AttributeValueEditor extends EditingSupport {

        private final TreeViewer viewer;
        private CCombo           comboBox;
        private TitleAreaDialog  dialog;

        public AttributeValueEditor(TreeViewer viewer, TitleAreaDialog dialog) {
            super(viewer);
            this.viewer = viewer;
            this.dialog = dialog;
        }

        @Override
        protected boolean canEdit(Object arg0) {
            return true;
        }

        @Override
        protected CellEditor getCellEditor(final Object element) {
            ResourceManager resourceManager = Activator.getDefault()
                    .getResourceManager();

            RemoteResource res = resourceManager
                    .getCurrentResourceInSelection();
            if (null == res) {
                return null;
            }

            final SimulatorResourceAttribute attribute;
            if (!(element instanceof AttributeElement)) {
                return null;
            }

            final AttributeElement attributeElement = ((AttributeElement) element);
            attribute = attributeElement.getSimulatorResourceAttribute();
            if (null == attribute) {
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

            String values[] = null;
            List<String> valueSet = resourceManager
                    .getAllValuesOfAttribute(attribute);
            values = convertListToStringArray(valueSet);

            ComboBoxCellEditor comboEditor;
            comboEditor = new ComboBoxCellEditor(viewer.getTree(), values);
            comboBox = (CCombo) comboEditor.getControl();
            comboBox.addModifyListener(new ModifyListener() {

                @Override
                public void modifyText(ModifyEvent event) {
                    String newValue = comboBox.getText();

                    if (null != newValue && !newValue.isEmpty()) {
                        attributeElement.setPostState(true);
                    } else {
                        attributeElement.setPostState(false);
                    }

                    if (dialog instanceof PostRequestDialog) {
                        viewer.update(attributeElement, null);
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

            String oldValue = String.valueOf(Utility
                    .getAttributeValueAsString(val));
            String newValue = comboBox.getText();
            if (!oldValue.equals(newValue)) {
                // Get the AttriuteValue from the string
                AttributeValue attValue = AttributeValueBuilder.build(newValue,
                        type.mBaseType);
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
                }
            }

            viewer.update(element, null);
        }

        public String[] convertListToStringArray(List<String> values) {
            String[] strArr;
            if (null != values && values.size() > 0) {
                strArr = values.toArray(new String[1]);
            } else {
                strArr = new String[1];
            }
            return strArr;
        }

        public void updateAttributeValue(SimulatorResourceAttribute att,
                AttributeValue value) {
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

            TreeItem parent = item.getParentItem();
            if (null != parent) {
                while (parent.getParentItem() != null) {
                    parent = parent.getParentItem();
                }
                Object data = parent.getData();
                ((AttributeElement) data).setPostState(true);
            }

            if (item.getData() instanceof AttributeElement) {
                AttributeElement attributeElement = (AttributeElement) item
                        .getData();
                attributeElement.getSimulatorResourceAttribute()
                        .setValue(value);

                parent = item.getParentItem();
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
    }

    class PostRequestEditor extends EditingSupport {

        private final TreeViewer viewer;

        public PostRequestEditor(TreeViewer viewer) {
            super(viewer);
            this.viewer = viewer;
        }

        @Override
        protected boolean canEdit(Object arg0) {
            return true;
        }

        @Override
        protected CellEditor getCellEditor(Object element) {
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
                    || type.mBaseType == ValueType.RESOURCEMODEL) {
                return null;
            }

            return new CheckboxCellEditor(null, SWT.CHECK | SWT.READ_ONLY);
        }

        @Override
        protected Object getValue(Object element) {
            if (element instanceof AttributeElement) {
                return ((AttributeElement) element).getPostState();
            }

            return false;
        }

        @Override
        protected void setValue(Object element, Object value) {
            if (!(element instanceof AttributeElement)) {
                return;
            }
            boolean status = (Boolean) value;
            ((AttributeElement) element).setPostState(status);
            viewer.update(element, null);
        }
    }
}
