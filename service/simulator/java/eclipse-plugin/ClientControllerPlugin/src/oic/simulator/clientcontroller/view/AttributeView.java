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

import java.util.Map;

import oic.simulator.clientcontroller.Activator;
import oic.simulator.clientcontroller.listener.IConfigurationUpload;
import oic.simulator.clientcontroller.listener.IGetUIListener;
import oic.simulator.clientcontroller.listener.IObserveUIListener;
import oic.simulator.clientcontroller.listener.IPostUIListener;
import oic.simulator.clientcontroller.listener.IPutUIListener;
import oic.simulator.clientcontroller.listener.IResourceSelectionChangedUIListener;
import oic.simulator.clientcontroller.listener.IVerificationUIListener;
import oic.simulator.clientcontroller.manager.ResourceManager;
import oic.simulator.clientcontroller.remoteresource.AttributeElement;
import oic.simulator.clientcontroller.remoteresource.RemoteResource;
import oic.simulator.clientcontroller.remoteresource.ResourceRepresentation;
import oic.simulator.clientcontroller.utils.Constants;
import oic.simulator.clientcontroller.utils.Utility;
import oic.simulator.clientcontroller.view.dialogs.PostRequestDialog;
import oic.simulator.clientcontroller.view.dialogs.PutRequestDialog;
import oic.simulator.clientcontroller.view.dialogs.VerificationDialog;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.window.Window;
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
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeColumn;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;
import org.oic.simulator.AttributeValue.ValueType;
import org.oic.simulator.SimulatorResourceAttribute;
import org.oic.simulator.client.SimulatorRemoteResource.VerificationType;

/**
 * This class manages and shows the attribute view in the perspective.
 */
public class AttributeView extends ViewPart {

    public static final String                  VIEW_ID        = "oic.simulator.clientcontroller.view.attribute";

    private TreeViewer                          attViewer;

    private Button                              getButton;
    private Button                              putButton;
    private Button                              postButton;
    private Button                              automateButton;
    private Button                              observeResButton;

    private final String[]                      attTblHeaders  = {
            "Attribute Name", "Attribute Value"               };
    private final Integer[]                     attTblColWidth = { 200, 200 };

    private ResourceManager                     resourceManager;

    private IResourceSelectionChangedUIListener resourceSelectionChangedListener;
    private IGetUIListener                      getUIListener;
    private IPutUIListener                      putUIListener;
    private IPostUIListener                     postUIListener;
    private IObserveUIListener                  observeUIListener;
    private IVerificationUIListener             verificationUIListener;
    private IConfigurationUpload                configUploadUIListener;

    private RemoteResource                      resourceInSelection;

    public AttributeView() {
        resourceManager = Activator.getDefault().getResourceManager();

        resourceSelectionChangedListener = new IResourceSelectionChangedUIListener() {

            @Override
            public void onResourceSelectionChange(final RemoteResource resource) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        resourceInSelection = resource;

                        // Set visibility of manual and automation controls
                        setVisibility((resource == null) ? false : true);

                        // Update the attribute table
                        if (null != attViewer
                                && !attViewer.getControl().isDisposed()) {
                            updateViewer(resource);
                        }

                        // Update the observe status
                        updateObserve(resource);
                    }
                });
            }
        };

        getUIListener = new IGetUIListener() {

            @Override
            public void onGetCompleted(final RemoteResource resource) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (null == resource) {
                            return;
                        }
                        // Update the attribute table
                        if (resourceInSelection != resource) {
                            return;
                        }
                        updateViewer(resource);

                        // Update the observe status
                        updateObserve(resource);
                    }
                });
            }
        };

        putUIListener = new IPutUIListener() {

            @Override
            public void onPutCompleted(final RemoteResource resource) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (null == resource) {
                            return;
                        }
                        // Update the attribute table
                        if (resourceInSelection != resource) {
                            return;
                        }
                        updateViewer(resource);

                        // Update the observe status
                        updateObserve(resource);
                    }
                });
            }
        };

        postUIListener = new IPostUIListener() {

            @Override
            public void onPostCompleted(final RemoteResource resource) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (null == resource) {
                            return;
                        }
                        // Update the attribute table
                        if (resourceInSelection != resource) {
                            return;
                        }
                        updateViewer(resource);

                        // Update the observe status
                        updateObserve(resource);
                    }
                });
            }
        };

        observeUIListener = new IObserveUIListener() {

            @Override
            public void onObserveCompleted(final RemoteResource resource) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (null == resource) {
                            return;
                        }
                        // Update the attribute table
                        if (resourceInSelection != resource) {
                            return;
                        }
                        updateViewer(resource);

                        // Update the observe status
                        updateObserve(resource);
                    }
                });
            }
        };

        verificationUIListener = new IVerificationUIListener() {

            @Override
            public void onVerificationStarted(final RemoteResource resource,
                    final int autoType) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        // changeReqBtnVisibility(autoType, false);
                    }
                });
            }

            @Override
            public void onVerificationCompleted(final RemoteResource resource,
                    final int autoType) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        // changeReqBtnVisibility(autoType, true);
                    }
                });
            }

            @Override
            public void onVerificationAborted(final RemoteResource resource,
                    final int autoType) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        // changeReqBtnVisibility(autoType, true);
                    }
                });
            }
        };

        configUploadUIListener = new IConfigurationUpload() {

            @Override
            public void onConfigurationUploaded(final RemoteResource resource) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (null == resource) {
                            return;
                        }
                        if (resourceInSelection != resource) {
                            return;
                        }
                        if (!automateButton.isDisposed()) {
                            automateButton.setEnabled(true);
                        }
                    }
                });
            }
        };
    }

    private void changeReqBtnVisibility(int reqType, boolean visibility) {
        if (reqType == Constants.GET_AUTOMATION_INDEX) {
            if (!getButton.isDisposed()) {
                getButton.setEnabled(visibility);
            }
        } else if (reqType == Constants.PUT_AUTOMATION_INDEX) {
            if (!putButton.isDisposed()) {
                putButton.setEnabled(visibility);
            }
        } else {// if(reqType == Constants.POST_AUTOMATION_INDEX) {
            if (!postButton.isDisposed()) {
                postButton.setEnabled(visibility);
            }
        }
    }

    private void updateViewer(RemoteResource resource) {
        if (null == attViewer) {
            return;
        }
        Tree tree = attViewer.getTree();
        if (null != resource) {
            attViewer.setInput(resource.getResourceRepresentation());
            if (!tree.isDisposed()) {
                tree.setLinesVisible(true);
            }
        } else {
            if (!tree.isDisposed()) {
                tree.removeAll();
                tree.setLinesVisible(false);
            }
        }
    }

    private void updateObserve(RemoteResource resource) {
        if (null == resource || observeResButton.isDisposed()) {
            return;
        }
        boolean observed = resource.isObserved();
        if (observed) {
            observeResButton.setText(Constants.STOP_OBSERVE);
        } else {
            observeResButton.setText(Constants.OBSERVE);
        }
    }

    @Override
    public void createPartControl(Composite parent) {
        Color color = Display.getDefault().getSystemColor(SWT.COLOR_WHITE);

        parent.setLayout(new GridLayout());
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        parent.setLayoutData(gd);

        Group attGroup = new Group(parent, SWT.NONE);
        attGroup.setLayout(new GridLayout());
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessVerticalSpace = true;
        gd.verticalAlignment = SWT.FILL;
        gd.horizontalSpan = 2;
        // gd.heightHint = 175;
        attGroup.setLayoutData(gd);
        attGroup.setText("Attributes");
        attGroup.setBackground(color);

        setupAttributeTable(attGroup);

        setupRequestControls(parent);

        setUIListeners();

        addManagerListeners();

        setVisibility(false);

        // Updating the data in the UI as per the resource in selection.
        if (null != attViewer && !attViewer.getControl().isDisposed()) {
            updateViewer(resourceManager.getCurrentResourceInSelection());
        }
    }

    private void setupRequestControls(Composite parent) {
        GridData gd;
        Color color = Display.getDefault().getSystemColor(SWT.COLOR_WHITE);
        Composite opsComp = new Composite(parent, SWT.NONE);
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        opsComp.setLayoutData(gd);
        GridLayout grid = new GridLayout(5, false);
        opsComp.setLayout(grid);
        opsComp.setBackground(color);

        getButton = new Button(opsComp, SWT.PUSH);
        getButton.setText("GET");
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        gd.widthHint = 50;
        getButton.setLayoutData(gd);

        putButton = new Button(opsComp, SWT.PUSH);
        putButton.setText("PUT");
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        gd.widthHint = 50;
        putButton.setLayoutData(gd);

        postButton = new Button(opsComp, SWT.PUSH);
        postButton.setText("POST");
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        gd.widthHint = 50;
        postButton.setLayoutData(gd);

        observeResButton = new Button(opsComp, SWT.PUSH);
        observeResButton.setText(Constants.OBSERVE);
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        observeResButton.setLayoutData(gd);

        automateButton = new Button(opsComp, SWT.PUSH);
        automateButton.setText("Automation");
        gd = new GridData();
        gd.grabExcessHorizontalSpace = true;
        gd.horizontalAlignment = SWT.FILL;
        automateButton.setLayoutData(gd);
    }

    private void setupAttributeTable(Group attGroup) {
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
    }

    public void createAttributeColumns(TreeViewer viewer) {
        Tree tree = viewer.getTree();

        TreeColumn attName = new TreeColumn(tree, SWT.NONE);
        attName.setWidth(attTblColWidth[0]);
        attName.setText(attTblHeaders[0]);

        TreeColumn attValue = new TreeColumn(tree, SWT.NONE);
        attValue.setWidth(attTblColWidth[1]);
        attValue.setText(attTblHeaders[1]);
    }

    private void setUIListeners() {

        getButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                if (resourceInSelection.isGetAutomtnInProgress()) {
                    MessageDialog
                            .openInformation(Display.getDefault()
                                    .getActiveShell(), "GET Request",
                                    "GET Automation is in progress. Please wait or stop the automation.");
                } else {
                    resourceManager.sendGetRequest(resourceInSelection);
                }
            }
        });

        putButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                PlatformUI.getWorkbench().getDisplay().syncExec(new Thread() {
                    @Override
                    public void run() {
                        if (resourceInSelection.isPutAutomtnInProgress()) {
                            MessageDialog
                                    .openInformation(Display.getDefault()
                                            .getActiveShell(), "PUT Request",
                                            "PUT Automation is in progress. Please wait or stop the automation.");
                            return;
                        }
                        if (resourceInSelection.getResourceRepresentation()
                                .getAttributes().size() < 1) {
                            MessageDialog
                                    .openInformation(Display.getDefault()
                                            .getActiveShell(), "PUT Request",
                                            "No attributes exist in the resource model.");
                            return;
                        }
                        PutRequestDialog putDialog = new PutRequestDialog(
                                Display.getDefault().getActiveShell());
                        if (putDialog.open() == Window.OK) {
                            // Call the native PUT method
                            resourceManager.sendPutRequest(resourceInSelection,
                                    putDialog.getUpdatedRepresentation()
                                            .getModel());
                        }
                    }
                });
            }
        });

        postButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                PlatformUI.getWorkbench().getDisplay().syncExec(new Thread() {
                    @Override
                    public void run() {
                        if (resourceInSelection.isPostAutomtnInProgress()) {
                            MessageDialog
                                    .openInformation(Display.getDefault()
                                            .getActiveShell(), "POST Request",
                                            "POST Automation is in progress. Please wait or stop the automation.");
                            return;
                        }
                        if (resourceInSelection.getResourceRepresentation()
                                .getAttributes().size() < 1) {
                            MessageDialog
                                    .openInformation(Display.getDefault()
                                            .getActiveShell(), "POST Request",
                                            "No attributes exist in the resource model.");
                            return;
                        }

                        PostRequestDialog postDialog = new PostRequestDialog(
                                Display.getDefault().getActiveShell());
                        if (postDialog.open() == Window.OK) {
                            // Call the native POST method
                            ResourceRepresentation representation = postDialog
                                    .getUpdatedRepresentation();
                            resourceManager.sendPostRequest(
                                    resourceInSelection,
                                    representation.getSelectedModel());
                        }
                    }
                });
            }
        });

        observeResButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                boolean result;
                if (observeResButton.getText().equals(Constants.OBSERVE)) {
                    result = resourceManager
                            .sendObserveRequest(resourceInSelection);
                    if (result) {
                        observeResButton.setText(Constants.STOP_OBSERVE);
                    } else {
                        MessageDialog.openError(Display.getDefault()
                                .getActiveShell(), "Observe failed",
                                "Failed to observe the resource. Try again.");
                    }
                } else {
                    result = resourceManager.sendCancelObserveRequest(
                            resourceInSelection, true);
                    if (result) {
                        observeResButton.setText(Constants.OBSERVE);
                    } else {
                        MessageDialog
                                .openError(Display.getDefault()
                                        .getActiveShell(),
                                        "Cancel Observe failed",
                                        "Failed to stop observing the resource. Try again.");
                    }
                }
            }
        });

        automateButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                PlatformUI.getWorkbench().getDisplay().syncExec(new Thread() {
                    @Override
                    public void run() {
                        RemoteResource resource = resourceManager
                                .getCurrentResourceInSelection();
                        if (null == resource) {
                            return;
                        }
                        Map<String, Boolean> autoStatus = resourceManager
                                .getAutomationStatus(resource);
                        if (null == autoStatus) {
                            return;
                        }

                        int startCount = 0;
                        int stopCount = 0;
                        boolean startGet, startPut, startPost;
                        boolean stopGet, stopPut, stopPost;
                        startGet = startPut = startPost = false;
                        stopGet = stopPut = stopPost = false;
                        String status = null;
                        String startMsg = "Verification will be started for: ";
                        String stopMsg = "Verification will be stopped for: ";
                        VerificationDialog ad = new VerificationDialog(Display
                                .getDefault().getActiveShell(), autoStatus);
                        if (ad.open() == Window.OK) {
                            Map<String, Boolean> oldStatus = resourceManager
                                    .getAutomationStatus(resource);
                            if (null == oldStatus || oldStatus.size() < 1) {
                                status = "Failed to perform the requested operation.";
                            } else {
                                // GET
                                if (oldStatus.get(Constants.GET) != autoStatus
                                        .get(Constants.GET)) {
                                    if (autoStatus.get(Constants.GET)) {
                                        startMsg += Constants.GET;
                                        startCount++;
                                        startGet = true;
                                    } else {
                                        stopMsg += Constants.GET;
                                        stopCount++;
                                        stopGet = true;
                                    }
                                }
                                // PUT
                                if (oldStatus.get(Constants.PUT) != autoStatus
                                        .get(Constants.PUT)) {
                                    if (autoStatus.get(Constants.PUT)) {
                                        if (startCount == 1) {
                                            startMsg += ", ";
                                        }
                                        startMsg += Constants.PUT;
                                        startCount++;
                                        startPut = true;
                                    } else {
                                        if (stopCount == 1) {
                                            stopMsg += ", ";
                                        }
                                        stopMsg += Constants.PUT;
                                        stopCount++;
                                        stopPut = true;
                                    }

                                }
                                // POST
                                if (oldStatus.get(Constants.POST) != autoStatus
                                        .get(Constants.POST)) {
                                    if (autoStatus.get(Constants.POST)) {
                                        if (startCount > 0) {
                                            startMsg += ", ";
                                        }
                                        startMsg += Constants.POST;
                                        startCount++;
                                        startPost = true;
                                    } else {
                                        if (stopCount > 0) {
                                            stopMsg += ", ";
                                        }
                                        stopMsg += Constants.POST;
                                        stopCount++;
                                        stopPost = true;
                                    }
                                }
                                if (startCount > 0) {
                                    status = startMsg + ".";
                                }
                                if (stopCount > 0) {
                                    if (startCount <= 0) {
                                        status = stopMsg;
                                    } else {
                                        status += "\n" + stopMsg + ".";
                                    }
                                }
                            }
                            if (startCount == 0 && stopCount == 0) {
                                MessageDialog.openInformation(Display
                                        .getDefault().getActiveShell(),
                                        "Verification", "No New Changes.");
                            } else {
                                boolean answer = MessageDialog.openQuestion(
                                        Display.getDefault().getActiveShell(),
                                        "Verification", status
                                                + "\nDo you want to proceed?");
                                if (answer) {
                                    if (startGet || stopGet)
                                        automate(VerificationType.GET,
                                                autoStatus.get(Constants.GET));
                                    if (startPut || stopPut)
                                        automate(VerificationType.PUT,
                                                autoStatus.get(Constants.PUT));
                                    if (startPost || stopPost)
                                        automate(VerificationType.POST,
                                                autoStatus.get(Constants.POST));
                                }
                            }
                        }
                    }
                });
            }
        });
    }

    private void automate(VerificationType type, boolean start) {
        if (start) {
            resourceManager.startAutomationRequest(type, resourceInSelection);
        } else {
            resourceManager.stopAutomationRequest(type, resourceInSelection);
        }
    }

    private void addManagerListeners() {
        resourceManager
                .addResourceSelectionChangedUIListener(resourceSelectionChangedListener);
        resourceManager.addGetUIListener(getUIListener);
        resourceManager.addPutUIListener(putUIListener);
        resourceManager.addPostUIListener(postUIListener);
        resourceManager.addObserveUIListener(observeUIListener);
        resourceManager.addVerificationUIListener(verificationUIListener);
        resourceManager.addConfigUploadUIListener(configUploadUIListener);
    }

    private void setVisibility(boolean visibility) {
        if (!getButton.isDisposed())
            getButton.setEnabled(visibility);
        if (!putButton.isDisposed())
            putButton.setEnabled(visibility);
        if (!postButton.isDisposed())
            postButton.setEnabled(visibility);
        if (!automateButton.isDisposed()) {
            if (visibility && null != resourceInSelection
                    && resourceInSelection.isConfigUploaded()) {
                automateButton.setEnabled(true);
            } else {
                automateButton.setEnabled(false);
            }
        }
        if (!observeResButton.isDisposed())
            observeResButton.setEnabled(visibility);
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
                }
            }
            return null;
        }
    }

    @Override
    public void dispose() {
        // Unregister the selection listener
        if (null != resourceSelectionChangedListener) {
            resourceManager
                    .removeResourceSelectionChangedUIListener(resourceSelectionChangedListener);
        }

        // Unregister the GET listener
        if (null != getUIListener) {
            resourceManager.removeGetUIListener(getUIListener);
        }

        super.dispose();
    }

    @Override
    public void setFocus() {
    }

}
