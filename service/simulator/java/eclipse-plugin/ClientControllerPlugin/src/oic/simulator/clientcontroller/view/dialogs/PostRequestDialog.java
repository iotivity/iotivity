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

package oic.simulator.clientcontroller.view.dialogs;

import oic.simulator.clientcontroller.Activator;
import oic.simulator.clientcontroller.remoteresource.AttributeElement;
import oic.simulator.clientcontroller.remoteresource.RemoteResource;
import oic.simulator.clientcontroller.remoteresource.ResourceRepresentation;
import oic.simulator.clientcontroller.utils.Constants;
import oic.simulator.clientcontroller.utils.Utility;
import oic.simulator.clientcontroller.view.AttributeEditingSupport;

import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.TreeViewerColumn;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeColumn;
import org.oic.simulator.AttributeValue.ValueType;
import org.oic.simulator.SimulatorResourceAttribute;

/**
 * This dialog is used for generating a POST request.
 */
public class PostRequestDialog extends TitleAreaDialog {

    private TreeViewer              attViewer;

    private AttributeEditingSupport attributeEditor;

    private ResourceRepresentation  updatedRepresentation;

    private final String[]          attTblHeaders  = { "Name", "Value",
            "Select"                              };
    private final Integer[]         attTblColWidth = { 200, 200, 50 };

    public PostRequestDialog(Shell parentShell) {
        super(parentShell);
    }

    @Override
    public void create() {
        super.create();
        setTitle("Generate POST Request");
        setMessage("Dialog which takes input and generates a post request.");
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        Composite compLayout = (Composite) super.createDialogArea(parent);
        Composite container = new Composite(compLayout, SWT.NONE);
        container.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        GridLayout layout = new GridLayout(1, false);
        layout.verticalSpacing = 10;
        layout.marginTop = 10;
        container.setLayout(layout);

        createTreeViewer(container);

        RemoteResource resource = Activator.getDefault().getResourceManager()
                .getCurrentResourceInSelection();

        updatedRepresentation = new ResourceRepresentation(
                resource.getResourceModelRef());

        attViewer.setInput(updatedRepresentation);

        return compLayout;
    }

    private void createTreeViewer(Composite parent) {
        Tree addressTree = new Tree(parent, SWT.SINGLE | SWT.BORDER
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
                .createAttributeValueEditor(attViewer, this));

        TreeColumn updateColumn = new TreeColumn(tree, SWT.NONE);
        updateColumn.setWidth(attTblColWidth[2]);
        updateColumn.setText(attTblHeaders[2]);
        TreeViewerColumn updateVwrCol = new TreeViewerColumn(attViewer,
                updateColumn);
        updateVwrCol.setEditingSupport(attributeEditor
                .createAutomationEditor(attViewer));
    }

    class AttributeContentProvider implements ITreeContentProvider {

        @Override
        public void dispose() {
        }

        @Override
        public void inputChanged(Viewer viewer, Object oldAttribute,
                Object newAttribute) {
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
                return ((ResourceRepresentation) resourceModel).getAttributes()
                        .values().toArray();
            }

            return new Object[0];
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

                    AttributeElement attrElement = (AttributeElement) element;
                    if (attrElement.isPostSupported()) {
                        if (attrElement.getPostState()) {
                            return Activator.getDefault().getImageRegistry()
                                    .get(Constants.CHECKED);
                        } else {
                            return Activator.getDefault().getImageRegistry()
                                    .get(Constants.UNCHECKED);
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
                        return "";
                    }
                }
            }
            return null;
        }
    }

    @Override
    protected boolean isResizable() {
        return true;
    }

    @Override
    public boolean isHelpAvailable() {
        return false;
    }

    @Override
    protected Button createButton(Composite parent, int id, String label,
            boolean defaultButton) {
        if (id == IDialogConstants.OK_ID) {
            label = "POST";
        }
        return super.createButton(parent, id, label, defaultButton);
    }

    public ResourceRepresentation getUpdatedRepresentation() {
        return updatedRepresentation;
    }
}
