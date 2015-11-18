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

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import oic.simulator.serviceprovider.Activator;
import oic.simulator.serviceprovider.listener.IPropertiesChangedUIListener;
import oic.simulator.serviceprovider.listener.IResourceListChangedUIListener;
import oic.simulator.serviceprovider.manager.ResourceManager;
import oic.simulator.serviceprovider.manager.UiListenerHandler;
import oic.simulator.serviceprovider.model.AutomationSettingHelper;
import oic.simulator.serviceprovider.model.CollectionResource;
import oic.simulator.serviceprovider.model.Device;
import oic.simulator.serviceprovider.model.Resource;
import oic.simulator.serviceprovider.model.ResourceType;
import oic.simulator.serviceprovider.model.SingleResource;
import oic.simulator.serviceprovider.utils.Constants;
import oic.simulator.serviceprovider.utils.Utility;
import oic.simulator.serviceprovider.view.dialogs.AddResourceToCollections;
import oic.simulator.serviceprovider.view.dialogs.AddResourceToDevices;
import oic.simulator.serviceprovider.view.dialogs.AddResources;
import oic.simulator.serviceprovider.view.dialogs.AutomationSettingDialog;
import oic.simulator.serviceprovider.view.dialogs.CreateResourceWizard;
import oic.simulator.serviceprovider.view.dialogs.DeleteResourceWizard;
import oic.simulator.serviceprovider.view.dialogs.RemoveResourceFromCollections;
import oic.simulator.serviceprovider.view.dialogs.RemoveResourceFromDevices;
import oic.simulator.serviceprovider.view.dialogs.RemoveResources;
import oic.simulator.serviceprovider.view.dialogs.ResourceWizardDialog;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.window.Window;
import org.eclipse.jface.wizard.WizardDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.events.MenuAdapter;
import org.eclipse.swt.events.MenuEvent;
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
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeItem;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.FilteredTree;
import org.eclipse.ui.dialogs.PatternFilter;
import org.eclipse.ui.part.ViewPart;
import org.oic.simulator.SimulatorException;
import org.oic.simulator.server.SimulatorResource.AutoUpdateType;

/**
 * This class manages and shows the resource manager view in the perspective.
 */
public class ResourceManagerView extends ViewPart {

    public static final String             VIEW_ID = "oic.simulator.serviceprovider.view.resourcemanager";

    private Button                         createButton;
    private Button                         deleteButton;

    private TreeViewer                     singleResTreeViewer;
    private TreeViewer                     collectionResTreeViewer;
    private TreeViewer                     deviceTreeViewer;

    private CTabFolder                     folder;
    private CTabItem                       singleResTab;
    private CTabItem                       collectionResTab;
    private CTabItem                       deviceTab;

    private IResourceListChangedUIListener resourceListChangedListener;

    private IPropertiesChangedUIListener   resourcePropertiesChangedListener;

    private ResourceManager                resourceManager;

    public ResourceManagerView() {

        resourceManager = Activator.getDefault().getResourceManager();

        resourceListChangedListener = new IResourceListChangedUIListener() {

            @Override
            public void onResourceCreation(final ResourceType type) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (type == ResourceType.SINGLE) {
                            if (null != singleResTreeViewer) {
                                singleResTreeViewer.refresh();
                            }
                        } else if (type == ResourceType.COLLECTION) {
                            if (null != collectionResTreeViewer) {
                                collectionResTreeViewer.refresh();
                            }
                        } else {
                            if (null != deviceTreeViewer) {
                                deviceTreeViewer.refresh();
                            }
                        }

                        // Trigger the visibility of delete button
                        changeDeleteVisibility();
                    }
                });
            }

            @Override
            public void onResourceDeletion(final ResourceType type) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (null != singleResTreeViewer
                                && null != collectionResTreeViewer
                                && null != deviceTreeViewer) {
                            switch (type) {
                                case SINGLE:
                                    singleResTreeViewer.refresh();
                                case COLLECTION:
                                    collectionResTreeViewer.refresh();
                                default:
                                    deviceTreeViewer.refresh();
                            }
                        }
                        // Trigger the visibility of delete button
                        changeDeleteVisibility();
                    }
                });
            }

            @Override
            public void onResourceListUpdate(final ResourceType type) {
                Display.getDefault().asyncExec(new Runnable() {

                    @Override
                    public void run() {
                        if (null != singleResTreeViewer
                                && null != collectionResTreeViewer
                                && null != deviceTreeViewer) {
                            switch (type) {
                                case SINGLE:
                                    singleResTreeViewer.refresh();
                                case COLLECTION:
                                    collectionResTreeViewer.refresh();
                                default:
                                    deviceTreeViewer.refresh();
                            }
                        }
                    }
                });
            }
        };

        resourcePropertiesChangedListener = new IPropertiesChangedUIListener() {

            @Override
            public void onResourcePropertyChange() {
                Display.getDefault().asyncExec(new Runnable() {
                    @Override
                    public void run() {
                        if (null != singleResTreeViewer
                                && null != collectionResTreeViewer
                                && null != deviceTreeViewer) {
                            singleResTreeViewer.refresh();
                            collectionResTreeViewer.refresh();
                            deviceTreeViewer.refresh();
                        }
                    }
                });
            }

            @Override
            public void onDevicePropertyChange() {
                Display.getDefault().asyncExec(new Runnable() {
                    @Override
                    public void run() {
                        if (null != deviceTreeViewer) {
                            deviceTreeViewer.refresh();
                        }
                    }
                });
            }
        };
    }

    public void changeDeleteVisibility() {
        boolean visibility = resourceManager.isAnyResourceExist();
        if (null != deleteButton && !deleteButton.isDisposed()) {
            deleteButton.setEnabled(visibility);
        }
    }

    @Override
    public void createPartControl(Composite parent) {
        Composite compContent = new Composite(parent, SWT.NONE);
        GridLayout baseLayout = new GridLayout(1, false);
        compContent.setLayout(baseLayout);

        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        compContent.setLayoutData(gd);

        Composite buttonComp = new Composite(compContent, SWT.NONE);
        buttonComp.setLayout(new GridLayout(2, false));

        gd = new GridData();
        gd.horizontalAlignment = SWT.FILL;
        gd.grabExcessHorizontalSpace = true;

        buttonComp.setLayoutData(gd);

        createButton = new Button(buttonComp, SWT.PUSH);
        createButton.setText("Create");
        createButton.setToolTipText("Create Simulator Resource(s)");

        gd = new GridData();
        gd.widthHint = 90;
        createButton.setLayoutData(gd);

        deleteButton = new Button(buttonComp, SWT.PUSH);
        deleteButton.setText("Delete");
        deleteButton.setToolTipText("Delete Simulator Resource(s)");

        gd = new GridData();
        gd.widthHint = 90;
        deleteButton.setLayoutData(gd);

        // Create a Tab Folder.
        folder = new CTabFolder(compContent, SWT.BORDER);
        gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        folder.setLayoutData(gd);
        folder.setSimple(false);
        folder.setUnselectedCloseVisible(false);
        folder.setUnselectedImageVisible(false);
        folder.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                // Tab is switched.
                singleResTreeViewer.setSelection(null);
                collectionResTreeViewer.setSelection(null);
                deviceTreeViewer.setSelection(null);
                resourceManager.resourceSelectionChanged(null);
            }
        });

        createSimpleResourcesArea();

        createCollectionResourcesArea();

        createDeviceArea();

        folder.setSelection(singleResTab);

        addUIListeners();

        addManagerListeners();

        // If there is at least one resource exist, then enable the delete
        // resource button
        changeDeleteVisibility();
    }

    private void createSimpleResourcesArea() {
        singleResTab = new CTabItem(folder, SWT.NULL);
        singleResTab.setText("Simple Resources");

        // Create a group to show all the discovered resources.
        // Adding the group to the folder.
        Group resourceGroup = new Group(folder, SWT.NONE);
        // resourceGroup.setText("Discovered Resources");

        Color color = Display.getDefault().getSystemColor(SWT.COLOR_WHITE);
        resourceGroup.setBackground(color);

        resourceGroup.setLayout(new GridLayout(1, false));
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        resourceGroup.setLayoutData(gd);

        PatternFilter filter = new PatternFilter();
        FilteredTree filteredTree = new FilteredTree(resourceGroup,
                SWT.H_SCROLL | SWT.V_SCROLL | SWT.SINGLE, filter, true);
        singleResTreeViewer = filteredTree.getViewer();
        singleResTreeViewer.getTree().setLayoutData(
                new GridData(SWT.FILL, SWT.FILL, true, true));
        singleResTreeViewer
                .setContentProvider(new SingleResourceContentProvider());
        singleResTreeViewer.setLabelProvider(new TreeLabelProvider());
        singleResTreeViewer.setInput(new Object());

        singleResTreeViewer
                .addSelectionChangedListener(new ISelectionChangedListener() {
                    @Override
                    public void selectionChanged(SelectionChangedEvent e) {
                        IStructuredSelection selection = (IStructuredSelection) e
                                .getSelection();
                        if (null == selection) {
                            return;
                        }

                        Resource res = null;
                        int size = selection.size();
                        if (size == 1) {
                            res = (Resource) selection.getFirstElement();
                        }
                        resourceManager.resourceSelectionChanged(res);
                    }
                });

        // TODO: Add menu items
        addMenuToSimpleResources();

        singleResTab.setControl(resourceGroup);
    }

    private void addMenuToSimpleResources() {
        if (null != singleResTreeViewer) {
            final Tree resourceTreeHead = singleResTreeViewer.getTree();
            if (null != resourceTreeHead) {
                // Below code creates menu entries and shows them on right
                // clicking a resource
                final Menu menu = new Menu(resourceTreeHead);
                resourceTreeHead.setMenu(menu);
                menu.addMenuListener(new MenuAdapter() {
                    @Override
                    public void menuShown(MenuEvent e) {
                        // Clear existing menu items
                        MenuItem[] items = menu.getItems();
                        for (int index = 0; index < items.length; index++) {
                            items[index].dispose();
                        }

                        IStructuredSelection selection = ((IStructuredSelection) singleResTreeViewer
                                .getSelection());
                        final SingleResource resource = (SingleResource) selection
                                .getFirstElement();
                        if (null == resource) {
                            return;
                        }

                        addAutomationMenu(menu, resource);

                        // Menu to add resource to one or more collections.
                        MenuItem addToCollection = new MenuItem(menu, SWT.NONE);
                        addToCollection.setText("Add to Collection");
                        addToCollection
                                .addSelectionListener(new SelectionAdapter() {
                                    @Override
                                    public void widgetSelected(SelectionEvent e) {
                                        List<CollectionResource> possibleCollections;
                                        possibleCollections = resourceManager
                                                .getCollectionsForAddingToSingleResource(resource);
                                        if (null == possibleCollections
                                                || possibleCollections
                                                        .isEmpty()) {
                                            MessageDialog
                                                    .openError(
                                                            Display.getDefault()
                                                                    .getActiveShell(),
                                                            "No possible candidates",
                                                            "There are no possible collections to which the resource can be added.");
                                            return;
                                        }

                                        AddResourceToCollections addToCollectionsDlg = new AddResourceToCollections(
                                                Display.getDefault()
                                                        .getActiveShell(),
                                                possibleCollections);
                                        if (addToCollectionsDlg.open() != Window.OK) {
                                            return;
                                        }

                                        Set<CollectionResource> resultSet = addToCollectionsDlg
                                                .getSelectedResourceList();

                                        int addedCount = resourceManager
                                                .addResourceToCollection(
                                                        resultSet, resource);
                                        if (addedCount <= 0) {
                                            MessageDialog
                                                    .openInformation(Display
                                                            .getDefault()
                                                            .getActiveShell(),
                                                            "Operation failed",
                                                            "Failed to add resources to the collection.");
                                            return;
                                        }

                                        collectionResTreeViewer.refresh();

                                        deviceTreeViewer.refresh();

                                        MessageDialog.openInformation(
                                                Display.getDefault()
                                                        .getActiveShell(),
                                                "Added",
                                                "["
                                                        + addedCount
                                                        + "/"
                                                        + resultSet.size()
                                                        + "] added to selected collections");
                                    }
                                });

                        // Menu to remove resource from one or more collections
                        // to which it is a member.
                        MenuItem removeFromCollection = new MenuItem(menu,
                                SWT.NONE);
                        removeFromCollection.setText("Remove from Collection");
                        removeFromCollection
                                .addSelectionListener(new SelectionAdapter() {
                                    @Override
                                    public void widgetSelected(SelectionEvent e) {
                                        List<CollectionResource> possibleCollectionstoRemoveFrom;
                                        possibleCollectionstoRemoveFrom = resourceManager
                                                .getResourceReferences(resource);
                                        if (null == possibleCollectionstoRemoveFrom
                                                || possibleCollectionstoRemoveFrom
                                                        .isEmpty()) {
                                            MessageDialog
                                                    .openError(
                                                            Display.getDefault()
                                                                    .getActiveShell(),
                                                            "No possible candidates",
                                                            "Resource has not been added to any collections.");
                                            return;
                                        }

                                        RemoveResourceFromCollections removeFromCollectionsDlg = new RemoveResourceFromCollections(
                                                Display.getDefault()
                                                        .getActiveShell(),
                                                possibleCollectionstoRemoveFrom);
                                        if (removeFromCollectionsDlg.open() != Window.OK) {
                                            return;
                                        }

                                        Set<CollectionResource> resultSet = removeFromCollectionsDlg
                                                .getSelectedResourceList();

                                        int removeCount = resourceManager
                                                .removeResourceFromCollection(
                                                        resultSet, resource);
                                        if (removeCount <= 0) {
                                            MessageDialog
                                                    .openInformation(Display
                                                            .getDefault()
                                                            .getActiveShell(),
                                                            "Operation failed",
                                                            "Failed to remove resources from the collection.");
                                            return;
                                        }

                                        collectionResTreeViewer.refresh();

                                        deviceTreeViewer.refresh();

                                        MessageDialog.openInformation(
                                                Display.getDefault()
                                                        .getActiveShell(),
                                                "Removed",
                                                "["
                                                        + removeCount
                                                        + "/"
                                                        + resultSet.size()
                                                        + "] resource removed from selected collections");
                                    }
                                });

                        // Menu to add resource to one or more devices.
                        MenuItem addToDevice = new MenuItem(menu, SWT.NONE);
                        addToDevice.setText("Add to Device");
                        addToDevice
                                .addSelectionListener(new SelectionAdapter() {
                                    @Override
                                    public void widgetSelected(SelectionEvent e) {
                                        List<Device> possibleDevices;
                                        possibleDevices = resourceManager
                                                .getDevicesForAddingToResource(resource);
                                        if (null == possibleDevices
                                                || possibleDevices.isEmpty()) {
                                            MessageDialog
                                                    .openError(
                                                            Display.getDefault()
                                                                    .getActiveShell(),
                                                            "No possible candidates",
                                                            "There are no possible devices to which the resource can be added.");
                                            return;
                                        }

                                        AddResourceToDevices addToDevicesDlg = new AddResourceToDevices(
                                                Display.getDefault()
                                                        .getActiveShell(),
                                                possibleDevices);
                                        if (addToDevicesDlg.open() != Window.OK) {
                                            return;
                                        }

                                        Set<Device> resultSet = addToDevicesDlg
                                                .getSelectedDeviceList();

                                        resourceManager.addResourceToDevice(
                                                resultSet, resource);

                                        deviceTreeViewer.refresh();

                                        MessageDialog
                                                .openInformation(Display
                                                        .getDefault()
                                                        .getActiveShell(),
                                                        "Added",
                                                        "Resource added to selected devices.");
                                    }
                                });

                        // Menu to remove resource from one or more device to
                        // which it is a member.
                        MenuItem removeFromDevice = new MenuItem(menu, SWT.NONE);
                        removeFromDevice.setText("Remove from Device");
                        removeFromDevice
                                .addSelectionListener(new SelectionAdapter() {
                                    @Override
                                    public void widgetSelected(SelectionEvent e) {
                                        List<Device> possibleDevices;
                                        possibleDevices = resourceManager
                                                .getDeviceReferences(resource);
                                        if (null == possibleDevices
                                                || possibleDevices.isEmpty()) {
                                            MessageDialog
                                                    .openError(
                                                            Display.getDefault()
                                                                    .getActiveShell(),
                                                            "No possible candidates",
                                                            "Resource has not been added to any devices.");
                                            return;
                                        }

                                        RemoveResourceFromDevices removeFromDevicesDlg = new RemoveResourceFromDevices(
                                                Display.getDefault()
                                                        .getActiveShell(),
                                                possibleDevices);
                                        if (removeFromDevicesDlg.open() != Window.OK) {
                                            return;
                                        }

                                        Set<Device> resultSet = removeFromDevicesDlg
                                                .getSelectedDeviceList();

                                        resourceManager
                                                .removeResourceFromDevice(
                                                        resultSet, resource);

                                        deviceTreeViewer.refresh();

                                        MessageDialog
                                                .openInformation(Display
                                                        .getDefault()
                                                        .getActiveShell(),
                                                        "Removed",
                                                        "Resource removed from selected devices");
                                    }
                                });

                        // Menu to remove resource from one or more device to
                        // which it is a member.
                        MenuItem deleteResource = new MenuItem(menu, SWT.NONE);
                        deleteResource.setText("Delete");
                        deleteResource
                                .addSelectionListener(new SelectionAdapter() {
                                    @Override
                                    public void widgetSelected(SelectionEvent e) {
                                        try {
                                            resourceManager
                                                    .removeResource(resource);
                                            singleResTreeViewer.refresh();
                                            // if(refreshCollectionTree)
                                            collectionResTreeViewer.refresh();
                                            // if(refreshDeviceTree)
                                            deviceTreeViewer.refresh();
                                            MessageDialog.openInformation(
                                                    Display.getDefault()
                                                            .getActiveShell(),
                                                    "Deleted",
                                                    "Resource deleted.");
                                        } catch (SimulatorException e1) {
                                            MessageDialog
                                                    .openInformation(Display
                                                            .getDefault()
                                                            .getActiveShell(),
                                                            "Deletion Failed",
                                                            "Failed to delete the resource.");
                                        }
                                        changeDeleteVisibility();
                                    }
                                });
                    }
                });
            }
        }
    }

    private void createCollectionResourcesArea() {
        collectionResTab = new CTabItem(folder, SWT.NULL);
        collectionResTab.setText("Collection Resources");

        // Create a group to show all the discovered resources.
        // Adding the group to the folder.
        Group resourceGroup = new Group(folder, SWT.NONE);
        // resourceGroup.setText("Discovered Resources");

        Color color = Display.getDefault().getSystemColor(SWT.COLOR_WHITE);
        resourceGroup.setBackground(color);

        resourceGroup.setLayout(new GridLayout(1, false));
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        resourceGroup.setLayoutData(gd);

        PatternFilter filter = new PatternFilter();
        FilteredTree filteredTree = new FilteredTree(resourceGroup,
                SWT.H_SCROLL | SWT.V_SCROLL | SWT.SINGLE, filter, true);
        collectionResTreeViewer = filteredTree.getViewer();
        collectionResTreeViewer.getTree().setLayoutData(
                new GridData(SWT.FILL, SWT.FILL, true, true));
        collectionResTreeViewer
                .setContentProvider(new CollectionResourceContentProvider());
        collectionResTreeViewer.setLabelProvider(new TreeLabelProvider());
        collectionResTreeViewer.setInput(new Object());

        collectionResTreeViewer
                .addSelectionChangedListener(new ISelectionChangedListener() {
                    @Override
                    public void selectionChanged(SelectionChangedEvent e) {
                        IStructuredSelection selection = (IStructuredSelection) e
                                .getSelection();
                        if (null == selection) {
                            return;
                        }

                        Resource res = null;
                        int size = selection.size();
                        if (size == 1) {
                            res = (Resource) selection.getFirstElement();
                        }
                        resourceManager.resourceSelectionChanged(res);
                    }
                });

        // TODO: Add menu items
        addMenuToCollectionResources();

        collectionResTab.setControl(resourceGroup);
    }

    private void addMenuToCollectionResources() {
        if (null != collectionResTreeViewer) {
            final Tree resourceTreeHead = collectionResTreeViewer.getTree();
            if (null != resourceTreeHead) {
                // Below code creates menu entries and shows them on right
                // clicking a resource
                final Menu menu = new Menu(resourceTreeHead);
                resourceTreeHead.setMenu(menu);
                menu.addMenuListener(new MenuAdapter() {
                    @Override
                    public void menuShown(MenuEvent e) {
                        // Clear existing menu items
                        MenuItem[] items = menu.getItems();
                        for (int index = 0; index < items.length; index++) {
                            items[index].dispose();
                        }

                        IStructuredSelection selection = ((IStructuredSelection) collectionResTreeViewer
                                .getSelection());
                        final Resource resource = (Resource) selection
                                .getFirstElement();
                        TreeItem[] treeItems = resourceTreeHead.getSelection();
                        if (null == resource || null == treeItems) {
                            return;
                        }

                        final TreeItem parent = treeItems[0].getParentItem();
                        if (null == parent) {
                            System.out.println("No parent exist");
                            // Selected item is a top-level collection
                            addMenuToTopLevelCollectionResources(menu,
                                    (CollectionResource) resource);
                        } else {
                            if (resource instanceof SingleResource)
                                addAutomationMenu(menu,
                                        (SingleResource) resource);

                            MenuItem remFromParent = new MenuItem(menu,
                                    SWT.NONE);
                            remFromParent.setText("Remove from Parent");
                            remFromParent
                                    .addSelectionListener(new SelectionAdapter() {
                                        @Override
                                        public void widgetSelected(
                                                SelectionEvent e) {
                                            CollectionResource parentCollection = (CollectionResource) parent
                                                    .getData();
                                            String status;
                                            try {
                                                resourceManager
                                                        .removeResourceFromCollection(
                                                                parentCollection,
                                                                resource);
                                                collectionResTreeViewer
                                                        .refresh();
                                                deviceTreeViewer.refresh();

                                                status = "Resource removed from its parent";
                                            } catch (SimulatorException e1) {
                                                status = "Failed to remove the resource from its parent";
                                            }
                                            MessageDialog.openInformation(
                                                    Display.getDefault()
                                                            .getActiveShell(),
                                                    "Status", status);
                                        }
                                    });

                            if (resource instanceof SingleResource) {
                                // Menu Items for single resource will be added
                                // here.
                            }
                        }
                    }
                });
            }
        }
    }

    private void addMenuToTopLevelCollectionResources(final Menu menu,
            final CollectionResource colRes) {
        // Menu to add one or more resources to the collection.
        MenuItem addResources = new MenuItem(menu, SWT.CASCADE);
        addResources.setText("Add Resources");

        Menu addSubMenu = new Menu(menu);
        addResources.setMenu(addSubMenu);

        MenuItem simpleResources = new MenuItem(addSubMenu, SWT.NONE);
        simpleResources.setText("Simple");
        simpleResources.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                List<SingleResource> possibleResources;
                possibleResources = resourceManager
                        .getSingleTypeResourcesForAddingToCollectionResource(colRes);
                if (null == possibleResources || possibleResources.isEmpty()) {
                    MessageDialog
                            .openError(Display.getDefault().getActiveShell(),
                                    "No possible candidates",
                                    "There are no possible simple resources that can be added.");
                    return;
                }

                List<Resource> resources = Utility
                        .convertSingleTypeResourceListToBaseType(possibleResources);

                AddResources addToCollectionsDlg = new AddResources(Display
                        .getDefault().getActiveShell(), resources);
                if (addToCollectionsDlg.open() != Window.OK) {
                    return;
                }

                Set<Resource> resultSet = addToCollectionsDlg
                        .getSelectedResourceList();

                int addedCount = resourceManager.addResourceToCollection(
                        colRes, resultSet);
                if (addedCount <= 0) {
                    MessageDialog.openInformation(Display.getDefault()
                            .getActiveShell(), "Operation failed",
                            "Failed to add resources to the collection.");
                    return;
                }

                collectionResTreeViewer.refresh();

                deviceTreeViewer.refresh();

                MessageDialog.openInformation(Display.getDefault()
                        .getActiveShell(), "Added", "[" + addedCount + "/"
                        + resultSet.size()
                        + "] resources added to the collection");
            }
        });

        MenuItem collectionResources = new MenuItem(addSubMenu, SWT.NONE);
        collectionResources.setText("Collection");
        collectionResources.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                List<CollectionResource> possibleResources;
                possibleResources = resourceManager
                        .getCollectionTypeResourcesForAddingToCollectionResource(colRes);
                if (null == possibleResources || possibleResources.isEmpty()) {
                    MessageDialog
                            .openError(Display.getDefault().getActiveShell(),
                                    "No possible candidates",
                                    "There are no possible collections that can be added.");
                    return;
                }

                List<Resource> resources = Utility
                        .convertCollectionTypeResourceListToBaseType(possibleResources);

                AddResources addToCollectionsDlg = new AddResources(Display
                        .getDefault().getActiveShell(), resources);
                if (addToCollectionsDlg.open() != Window.OK) {
                    return;
                }

                Set<Resource> resultSet = addToCollectionsDlg
                        .getSelectedResourceList();

                int addedCount = resourceManager.addResourceToCollection(
                        colRes, resultSet);
                if (addedCount <= 0) {
                    MessageDialog.openInformation(Display.getDefault()
                            .getActiveShell(), "Operation failed",
                            "Failed to add resources to the collection.");
                    return;
                }

                collectionResTreeViewer.refresh();

                deviceTreeViewer.refresh();

                MessageDialog.openInformation(Display.getDefault()
                        .getActiveShell(), "Added", "[" + addedCount + "/"
                        + resultSet.size()
                        + "] resources added to the collection");
            }
        });

        // Menu to remove one or more resources from the collection.
        MenuItem removeResources = new MenuItem(menu, SWT.CASCADE);
        removeResources.setText("Remove Resources");

        Menu removeSubMenu = new Menu(menu);
        removeResources.setMenu(removeSubMenu);

        simpleResources = new MenuItem(removeSubMenu, SWT.NONE);
        simpleResources.setText("Simple");
        simpleResources.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                List<SingleResource> possibleResources;
                possibleResources = resourceManager.getSingleTypeChilds(colRes);
                if (null == possibleResources || possibleResources.isEmpty()) {
                    MessageDialog
                            .openError(Display.getDefault().getActiveShell(),
                                    "No possible candidates",
                                    "There are no possible simple resources which can be removed.");
                    return;
                }

                List<Resource> resources = Utility
                        .convertSingleTypeResourceListToBaseType(possibleResources);

                RemoveResources removeFromCollectionsDlg = new RemoveResources(
                        Display.getDefault().getActiveShell(), resources);
                if (removeFromCollectionsDlg.open() != Window.OK) {
                    return;
                }

                Set<Resource> resultSet = removeFromCollectionsDlg
                        .getSelectedResourceList();

                int removeCount = resourceManager
                        .removeResourcesFromCollection(colRes, resultSet);
                if (removeCount <= 0) {
                    MessageDialog.openInformation(Display.getDefault()
                            .getActiveShell(), "Operation failed",
                            "Failed to remove resources from the collection.");
                    return;
                }

                collectionResTreeViewer.refresh();

                deviceTreeViewer.refresh();

                MessageDialog.openInformation(Display.getDefault()
                        .getActiveShell(), "Removed", "[" + removeCount + "/"
                        + resultSet.size()
                        + "] resources removed from the collection");
            }
        });

        collectionResources = new MenuItem(removeSubMenu, SWT.NONE);
        collectionResources.setText("Collection");
        collectionResources.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                List<CollectionResource> possibleResources;
                possibleResources = resourceManager
                        .getCollectionTypeChilds(colRes);
                if (null == possibleResources || possibleResources.isEmpty()) {
                    MessageDialog
                            .openError(Display.getDefault().getActiveShell(),
                                    "No possible candidates",
                                    "There are no possible collections which can be removed.");
                    return;
                }

                List<Resource> resources = Utility
                        .convertCollectionTypeResourceListToBaseType(possibleResources);

                RemoveResources removeFromCollectionsDlg = new RemoveResources(
                        Display.getDefault().getActiveShell(), resources);
                if (removeFromCollectionsDlg.open() != Window.OK) {
                    return;
                }

                Set<Resource> resultSet = removeFromCollectionsDlg
                        .getSelectedResourceList();

                int removeCount = resourceManager
                        .removeResourcesFromCollection(colRes, resultSet);
                if (removeCount <= 0) {
                    MessageDialog.openInformation(Display.getDefault()
                            .getActiveShell(), "Operation failed",
                            "Failed to remove resources from the collection.");
                    return;
                }

                collectionResTreeViewer.refresh();

                deviceTreeViewer.refresh();

                MessageDialog.openInformation(Display.getDefault()
                        .getActiveShell(), "Removed", "[" + removeCount + "/"
                        + resultSet.size()
                        + "] resources removed from the collection");
            }
        });

        // Menu to add resource to one or more devices.
        MenuItem addToDevice = new MenuItem(menu, SWT.NONE);
        addToDevice.setText("Add to Device");
        addToDevice.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                List<Device> possibleDevices;
                possibleDevices = resourceManager
                        .getDevicesForAddingToResource(colRes);
                if (null == possibleDevices || possibleDevices.isEmpty()) {
                    MessageDialog
                            .openError(Display.getDefault().getActiveShell(),
                                    "No possible candidates",
                                    "There are no possible devices to which the resource can be added.");
                    return;
                }

                AddResourceToDevices addToDevicesDlg = new AddResourceToDevices(
                        Display.getDefault().getActiveShell(), possibleDevices);
                if (addToDevicesDlg.open() != Window.OK) {
                    return;
                }

                Set<Device> resultSet = addToDevicesDlg.getSelectedDeviceList();

                resourceManager.addResourceToDevice(resultSet, colRes);

                deviceTreeViewer.refresh();

                MessageDialog.openInformation(Display.getDefault()
                        .getActiveShell(), "Added",
                        "Resource added to selected devices.");
            }
        });

        // Menu to remove resource from one or more device to which it is a
        // member.
        MenuItem removeFromDevice = new MenuItem(menu, SWT.NONE);
        removeFromDevice.setText("Remove from Device");
        removeFromDevice.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                List<Device> possibleDevices;
                possibleDevices = resourceManager.getDeviceReferences(colRes);
                if (null == possibleDevices || possibleDevices.isEmpty()) {
                    MessageDialog.openError(Display.getDefault()
                            .getActiveShell(), "No possible candidates",
                            "Resource has not been added to any devices.");
                    return;
                }

                RemoveResourceFromDevices removeFromDevicesDlg = new RemoveResourceFromDevices(
                        Display.getDefault().getActiveShell(), possibleDevices);
                if (removeFromDevicesDlg.open() != Window.OK) {
                    return;
                }

                Set<Device> resultSet = removeFromDevicesDlg
                        .getSelectedDeviceList();

                resourceManager.removeResourceFromDevice(resultSet, colRes);

                deviceTreeViewer.refresh();

                MessageDialog.openInformation(Display.getDefault()
                        .getActiveShell(), "Removed",
                        "Resource removed from selected devices");
            }
        });

        // Menu to remove resource from one or more device to which it is a
        // member.
        MenuItem deleteResource = new MenuItem(menu, SWT.NONE);
        deleteResource.setText("Delete");
        deleteResource.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                /*
                 * boolean refreshCollectionTree = false; boolean
                 * refreshDeviceTree = false;
                 * if(resource.isMemberOfAnyCollection()) {
                 * refreshCollectionTree = true; }
                 * if(resource.isMemberOfAnyDevice()) { refreshDeviceTree =
                 * true; }
                 */
                try {
                    resourceManager.removeResource(colRes);
                    // if(refreshCollectionTree)
                    collectionResTreeViewer.refresh();
                    // if(refreshDeviceTree)
                    deviceTreeViewer.refresh();
                    MessageDialog.openInformation(Display.getDefault()
                            .getActiveShell(), "Deleted", "Resource deleted.");
                } catch (SimulatorException e1) {
                    MessageDialog.openInformation(Display.getDefault()
                            .getActiveShell(), "Deletion Failed",
                            "Failed to delete the resource.");
                }

                changeDeleteVisibility();
            }
        });

    }

    private void createDeviceArea() {
        deviceTab = new CTabItem(folder, SWT.NULL);
        deviceTab.setText("Devices");

        // Create a group to show all the discovered resources.
        // Adding the group to the folder.
        Group resourceGroup = new Group(folder, SWT.NONE);
        // resourceGroup.setText("Discovered Resources");

        Color color = Display.getDefault().getSystemColor(SWT.COLOR_WHITE);
        resourceGroup.setBackground(color);

        resourceGroup.setLayout(new GridLayout(1, false));
        GridData gd = new GridData(SWT.FILL, SWT.FILL, true, true);
        resourceGroup.setLayoutData(gd);

        PatternFilter filter = new PatternFilter();
        FilteredTree filteredTree = new FilteredTree(resourceGroup,
                SWT.H_SCROLL | SWT.V_SCROLL | SWT.SINGLE, filter, true);
        deviceTreeViewer = filteredTree.getViewer();
        deviceTreeViewer.getTree().setLayoutData(
                new GridData(SWT.FILL, SWT.FILL, true, true));
        deviceTreeViewer.setContentProvider(new DeviceContentProvider());
        deviceTreeViewer.setLabelProvider(new TreeLabelProvider());
        deviceTreeViewer.setInput(new Object());

        deviceTreeViewer
                .addSelectionChangedListener(new ISelectionChangedListener() {
                    @Override
                    public void selectionChanged(SelectionChangedEvent e) {
                        IStructuredSelection selection = (IStructuredSelection) e
                                .getSelection();
                        if (null == selection) {
                            return;
                        }

                        int size = selection.size();
                        if (size == 1) {
                            Object obj = selection.getFirstElement();
                            if (obj instanceof Device) {
                                resourceManager
                                        .deviceSelectionChanged((Device) obj);
                            } else {
                                resourceManager
                                        .resourceSelectionChanged((Resource) obj);
                            }
                        } else {
                            resourceManager.deviceSelectionChanged(null);
                        }
                    }
                });

        // TODO: Add menu items
        addMenuToDevices();

        deviceTab.setControl(resourceGroup);
    }

    private void addMenuToDevices() {
        if (null != deviceTreeViewer) {
            final Tree resourceTreeHead = deviceTreeViewer.getTree();
            if (null != resourceTreeHead) {
                // Below code creates menu entries and shows them on right
                // clicking a resource
                final Menu menu = new Menu(resourceTreeHead);
                resourceTreeHead.setMenu(menu);
                menu.addMenuListener(new MenuAdapter() {
                    @Override
                    public void menuShown(MenuEvent e) {
                        // Clear existing menu items
                        MenuItem[] items = menu.getItems();
                        for (int index = 0; index < items.length; index++) {
                            items[index].dispose();
                        }

                        IStructuredSelection selection = ((IStructuredSelection) deviceTreeViewer
                                .getSelection());
                        final Object element = selection.getFirstElement();
                        if (element instanceof Device) {
                            addMenuToDevices(menu, (Device) element);
                        } else {
                            final Resource resource = (Resource) element;
                            TreeItem[] treeItems = resourceTreeHead
                                    .getSelection();
                            if (null == resource || null == treeItems) {
                                return;
                            }

                            if (resource instanceof SingleResource)
                                addAutomationMenu(menu,
                                        (SingleResource) resource);

                            final TreeItem parent = treeItems[0]
                                    .getParentItem();
                            MenuItem remFromParent = new MenuItem(menu,
                                    SWT.NONE);
                            remFromParent.setText("Remove from Parent");
                            remFromParent
                                    .addSelectionListener(new SelectionAdapter() {
                                        @Override
                                        public void widgetSelected(
                                                SelectionEvent e) {
                                            String status = "Resource removed from its parent.";
                                            Object obj = parent.getData();
                                            if (obj instanceof Device) {
                                                resourceManager
                                                        .removeResourceFromDevice(
                                                                (Device) obj,
                                                                resource);
                                            } else {
                                                try {
                                                    resourceManager
                                                            .removeResourceFromCollection(
                                                                    (CollectionResource) obj,
                                                                    resource);
                                                    collectionResTreeViewer
                                                            .refresh();
                                                    status = "Resource removed from its parent";
                                                } catch (SimulatorException e1) {
                                                    status = "Failed to remove the resource from its parent";
                                                }
                                            }

                                            deviceTreeViewer.refresh();

                                            MessageDialog.openInformation(
                                                    Display.getDefault()
                                                            .getActiveShell(),
                                                    "Status", status);
                                        }
                                    });

                            if (resource instanceof SingleResource) {
                                // Menu Items for single resource will be added
                                // here.
                            }
                        }
                    }
                });
            }
        }
    }

    private void addMenuToDevices(Menu menu, final Device dev) {
        // Menu to add one or more resources to the collection.
        MenuItem addResources = new MenuItem(menu, SWT.CASCADE);
        addResources.setText("Add Resources");

        Menu addSubMenu = new Menu(menu);
        addResources.setMenu(addSubMenu);

        MenuItem simpleResources = new MenuItem(addSubMenu, SWT.NONE);
        simpleResources.setText("Simple");
        simpleResources.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                List<SingleResource> possibleResources;
                possibleResources = resourceManager
                        .getSingleTypeResourcesForAddingToDevice(dev);
                if (null == possibleResources || possibleResources.isEmpty()) {
                    MessageDialog
                            .openError(Display.getDefault().getActiveShell(),
                                    "No possible candidates",
                                    "There are no possible simple resources that can be added.");
                    return;
                }

                List<Resource> resources = Utility
                        .convertSingleTypeResourceListToBaseType(possibleResources);

                AddResources addDlg = new AddResources(Display.getDefault()
                        .getActiveShell(), resources);
                if (addDlg.open() != Window.OK) {
                    return;
                }

                Set<Resource> resultSet = addDlg.getSelectedResourceList();

                resourceManager.addResourceToDevice(dev, resultSet);

                deviceTreeViewer.refresh();

                MessageDialog.openInformation(Display.getDefault()
                        .getActiveShell(), "Added",
                        "Single Resources added to the device");
            }
        });

        MenuItem collectionResources = new MenuItem(addSubMenu, SWT.NONE);
        collectionResources.setText("Collection");
        collectionResources.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                List<CollectionResource> possibleResources;
                possibleResources = resourceManager
                        .getCollectionTypeResourcesForAddingToDevice(dev);
                if (null == possibleResources || possibleResources.isEmpty()) {
                    MessageDialog
                            .openError(Display.getDefault().getActiveShell(),
                                    "No possible candidates",
                                    "There are no possible collections that can be added.");
                    return;
                }

                List<Resource> resources = Utility
                        .convertCollectionTypeResourceListToBaseType(possibleResources);

                AddResources addDlg = new AddResources(Display.getDefault()
                        .getActiveShell(), resources);
                if (addDlg.open() != Window.OK) {
                    return;
                }

                Set<Resource> resultSet = addDlg.getSelectedResourceList();

                resourceManager.addResourceToDevice(dev, resultSet);

                deviceTreeViewer.refresh();

                MessageDialog.openInformation(Display.getDefault()
                        .getActiveShell(), "Added",
                        "Collection Resources added to the device");
            }
        });

        // Menu to remove one or more resources from the collection.
        MenuItem removeResources = new MenuItem(menu, SWT.CASCADE);
        removeResources.setText("Remove Resources");

        Menu removeSubMenu = new Menu(menu);
        removeResources.setMenu(removeSubMenu);

        simpleResources = new MenuItem(removeSubMenu, SWT.NONE);
        simpleResources.setText("Simple");
        simpleResources.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                List<SingleResource> possibleResources;
                possibleResources = resourceManager.getSingleTypeChilds(dev);
                if (null == possibleResources || possibleResources.isEmpty()) {
                    MessageDialog
                            .openError(Display.getDefault().getActiveShell(),
                                    "No possible candidates",
                                    "There are no possible simple resources which can be removed.");
                    return;
                }

                List<Resource> resources = Utility
                        .convertSingleTypeResourceListToBaseType(possibleResources);

                RemoveResources removeDlg = new RemoveResources(Display
                        .getDefault().getActiveShell(), resources);
                if (removeDlg.open() != Window.OK) {
                    return;
                }

                Set<Resource> resultSet = removeDlg.getSelectedResourceList();

                resourceManager.removeResourcesFromDevice(dev, resultSet);

                collectionResTreeViewer.refresh();

                deviceTreeViewer.refresh();

                MessageDialog.openInformation(Display.getDefault()
                        .getActiveShell(), "Removed",
                        "Single Resources removed from the device");
            }
        });

        collectionResources = new MenuItem(removeSubMenu, SWT.NONE);
        collectionResources.setText("Collection");
        collectionResources.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                List<CollectionResource> possibleResources;
                possibleResources = resourceManager
                        .getCollectionTypeChilds(dev);
                if (null == possibleResources || possibleResources.isEmpty()) {
                    MessageDialog
                            .openError(Display.getDefault().getActiveShell(),
                                    "No possible candidates",
                                    "There are no possible collections which can be removed.");
                    return;
                }

                List<Resource> resources = Utility
                        .convertCollectionTypeResourceListToBaseType(possibleResources);

                RemoveResources removeDlg = new RemoveResources(Display
                        .getDefault().getActiveShell(), resources);
                if (removeDlg.open() != Window.OK) {
                    return;
                }

                Set<Resource> resultSet = removeDlg.getSelectedResourceList();

                resourceManager.removeResourcesFromDevice(dev, resultSet);

                collectionResTreeViewer.refresh();

                deviceTreeViewer.refresh();

                MessageDialog.openInformation(Display.getDefault()
                        .getActiveShell(), "Removed",
                        "Collection Resources removed from the device");
            }
        });

        // Menu to remove the device.
        MenuItem deleteDevice = new MenuItem(menu, SWT.NONE);
        deleteDevice.setText("Delete");
        deleteDevice.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                resourceManager.removeDevice(dev);
                deviceTreeViewer.refresh();
            }
        });
    }

    private void addAutomationMenu(final Menu menu,
            final SingleResource selectedResource) {
        MenuItem startItem = new MenuItem(menu, SWT.NONE);
        startItem.setText(Constants.START_RESOURCE_AUTOMATION);
        startItem.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                // Block starting resource level
                // automation if any attribute level
                // automation is in progress for the
                // selected resource
                boolean started = resourceManager
                        .isAttributeAutomationStarted(selectedResource);
                if (started) {
                    MessageDialog
                            .openInformation(
                                    Display.getDefault().getActiveShell(),
                                    "Attribute automation is in progress",
                                    "Attribute level automation for this resource is already in progress!!!\nPlease stop all "
                                            + "running attribute level automations to start resource level automation.");
                } else {

                    // Start the automation
                    // Fetch the settings data
                    List<AutomationSettingHelper> automationSettings;
                    automationSettings = AutomationSettingHelper
                            .getAutomationSettings(null);

                    // Open the settings dialog
                    AutomationSettingDialog dialog = new AutomationSettingDialog(
                            Activator.getDefault().getWorkbench()
                                    .getActiveWorkbenchWindow().getShell(),
                            automationSettings);
                    dialog.create();
                    if (dialog.open() == Window.OK) {
                        String automationType = dialog.getAutomationType();
                        String updateFreq = dialog.getUpdateFrequency();

                        AutoUpdateType autoType = AutoUpdateType
                                .valueOf(automationType);
                        int updFreq = Utility
                                .getUpdateIntervalFromString(updateFreq);
                        boolean status = resourceManager
                                .startResourceAutomationUIRequest(autoType,
                                        updFreq, selectedResource);
                        String statusMsg = status ? "Automation started successfully!!!"
                                : "Automation request failed!!!";
                        MessageDialog.openInformation(Display.getDefault()
                                .getActiveShell(), "Automation Status",
                                statusMsg);
                    }
                }
            }
        });

        MenuItem stopItem = new MenuItem(menu, SWT.NONE);
        stopItem.setText(Constants.STOP_RESOURCE_AUTOMATION);
        stopItem.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                boolean status = resourceManager
                        .stopResourceAutomationUIRequest(selectedResource);
                String statusMsg = status ? "Automation stopped!!!"
                        : "Automation stop failed.";
                MessageDialog.openInformation(Display.getDefault()
                        .getActiveShell(), "Automation Status", statusMsg);
            }
        });

        // Set the initial visibility of menu items
        boolean status = resourceManager
                .isResourceAutomationStarted(selectedResource);
        startItem.setEnabled(!status);
        stopItem.setEnabled(status);
    }

    private void addUIListeners() {

        createButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                PlatformUI.getWorkbench().getDisplay().syncExec(new Runnable() {

                    @Override
                    public void run() {
                        CreateResourceWizard createWizard = new CreateResourceWizard();
                        ResourceWizardDialog wizardDialog = new ResourceWizardDialog(
                                PlatformUI.getWorkbench().getDisplay()
                                        .getActiveShell(), createWizard);
                        createWizard.setWizardDialog(wizardDialog);
                        int open = wizardDialog.open();
                        if (open == WizardDialog.OK
                                || createWizard.isDlgForceClosed()) {
                            MessageDialog.openInformation(Display.getDefault()
                                    .getActiveShell(),
                                    "Resource Creation Status", createWizard
                                            .getStatus());
                        }
                    }
                });
            }
        });

        deleteButton.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent e) {
                PlatformUI.getWorkbench().getDisplay().syncExec(new Runnable() {

                    @Override
                    public void run() {
                        boolean exist = resourceManager.isAnyResourceExist();
                        if (!exist) {
                            return;
                        }
                        DeleteResourceWizard deleteWizard = new DeleteResourceWizard();
                        ResourceWizardDialog wizardDialog = new ResourceWizardDialog(
                                PlatformUI.getWorkbench().getDisplay()
                                        .getActiveShell(), deleteWizard);
                        int open = wizardDialog.open();
                        if (open == Window.OK) {
                            singleResTreeViewer.refresh();
                            collectionResTreeViewer.refresh();
                            deviceTreeViewer.refresh();
                            MessageDialog.openInformation(Display.getDefault()
                                    .getActiveShell(),
                                    "Resource Deletion Status", deleteWizard
                                            .getStatus());

                            changeDeleteVisibility();
                        }
                    }
                });
            }
        });
    }

    public void addManagerListeners() {
        UiListenerHandler.getInstance().addResourceListChangedUIListener(
                resourceListChangedListener);
        UiListenerHandler.getInstance().addResourcePropertiesChangedUIListener(
                resourcePropertiesChangedListener);
    }

    @Override
    public void setFocus() {
    }

    @Override
    public void dispose() {
        // Unregister the listener
        if (null != resourceListChangedListener) {
            UiListenerHandler.getInstance()
                    .removeResourceListChangedUIListener(
                            resourceListChangedListener);
            resourceManager.resourceSelectionChanged(null);
        }
        if (null != resourcePropertiesChangedListener) {
            UiListenerHandler.getInstance()
                    .removeResourcePropertiesChangedUIListener(
                            resourcePropertiesChangedListener);
        }
        super.dispose();
    }
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
        List<SingleResource> resList;
        resList = Activator.getDefault().getResourceManager()
                .getSingleResourceList();
        if (null == resList) {
            resList = new ArrayList<SingleResource>();
        }
        return resList.toArray();
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
        if (parent instanceof CollectionResource) {
            CollectionResource cRes = (CollectionResource) parent;
            Set<Resource> childs = cRes.getChildResources();
            if (null == childs) {
                return null;
            }
            return childs.toArray();
        }
        return null;
    }

    @Override
    public Object[] getElements(Object parent) {
        List<CollectionResource> resList;
        resList = Activator.getDefault().getResourceManager()
                .getCollectionResourceList();
        if (null == resList) {
            resList = new ArrayList<CollectionResource>();
        }
        return resList.toArray();
    }

    @Override
    public Object getParent(Object child) {
        return null;
    }

    @Override
    public boolean hasChildren(Object parent) {
        if (parent instanceof CollectionResource) {
            CollectionResource cRes = (CollectionResource) parent;
            Set<Resource> children = cRes.getChildResources();
            if (null != children && children.size() > 0) {
                return true;
            }
        }
        return false;
    }
}

class DeviceContentProvider implements ITreeContentProvider {

    @Override
    public void dispose() {
    }

    @Override
    public void inputChanged(Viewer arg0, Object arg1, Object arg2) {
    }

    @Override
    public Object[] getChildren(Object parent) {
        if (parent instanceof Device) {
            Device dev = (Device) parent;
            Set<Resource> children = dev.getChildResources();
            if (null != children && children.size() > 0) {
                return children.toArray();
            }
        } else if (parent instanceof CollectionResource) {
            CollectionResource res = (CollectionResource) parent;
            Set<Resource> children = res.getChildResources();
            if (null != children && children.size() > 0) {
                return children.toArray();
            }
        }
        return null;
    }

    @Override
    public Object[] getElements(Object parent) {
        List<Device> deviceList;
        deviceList = Activator.getDefault().getResourceManager()
                .getDeviceList();
        if (null == deviceList) {
            deviceList = new ArrayList<Device>();
        }
        return deviceList.toArray();
    }

    @Override
    public Object getParent(Object child) {
        return null;
    }

    @Override
    public boolean hasChildren(Object parent) {
        if (parent instanceof Device) {
            Device dev = (Device) parent;
            Set<Resource> children = dev.getChildResources();
            if (null != children && children.size() > 0) {
                return true;
            }
        } else if (parent instanceof CollectionResource) {
            CollectionResource res = (CollectionResource) parent;
            Set<Resource> children = res.getChildResources();
            if (null != children && children.size() > 0) {
                return true;
            }
        }
        return false;
    }
}

class TreeLabelProvider extends LabelProvider {
    @Override
    public String getText(Object element) {
        if (element instanceof Resource) {
            Resource res = (Resource) element;
            return res.getResourceName();
        } else {
            Device dev = (Device) element;
            return dev.getDeviceName();
        }
    }

    @Override
    public Image getImage(Object element) {
        if (element instanceof SingleResource) {
            return Activator.getDefault().getImageRegistry()
                    .get(Constants.SINGLE_RESOURCE);
        } else if (element instanceof CollectionResource) {
            return Activator.getDefault().getImageRegistry()
                    .get(Constants.COLLECTION_RESOURCE);
        } else {
            return Activator.getDefault().getImageRegistry()
                    .get(Constants.DEVICE);
        }
    }
}