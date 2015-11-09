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

import java.lang.reflect.InvocationTargetException;
import java.net.URL;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

import oic.simulator.serviceprovider.Activator;
import oic.simulator.serviceprovider.manager.UiListenerHandler;
import oic.simulator.serviceprovider.model.AttributeHelper;
import oic.simulator.serviceprovider.model.CollectionResource;
import oic.simulator.serviceprovider.model.LocalResourceAttribute;
import oic.simulator.serviceprovider.model.Resource;
import oic.simulator.serviceprovider.model.ResourceType;
import oic.simulator.serviceprovider.model.SingleResource;
import oic.simulator.serviceprovider.utils.Utility;
import oic.simulator.serviceprovider.view.dialogs.MainPage.ResourceOption;

import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.wizard.IWizardPage;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.jface.wizard.WizardDialog;
import org.oic.simulator.ILogger.Level;
import org.oic.simulator.SimulatorException;

/**
 * This class creates a UI wizard for create resource operation.
 */
public class CreateResourceWizard extends Wizard {

    private MainPage                           mainPage;
    private SimpleResourceBasicDetailsPage     simpleResourceBasicDetailsPage;
    private SimpleResourceAddAttributePage     simpleResourceAddAttributePage;
    private SimpleResourceOtherDetailsPage     simpleResourceOtherDetailsPage;
    private CollectionResourceBasicDetailsPage collectionResourceBasicDetailsPage;
    private AddResourcesToCollectionPage       addResourcesToCollectionPage;
    private LoadRamlPage                       loadRamlPage;
    private UpdatePropertiesPage               updatePropPage;
    private DevicePage                         devicePage;
    private StartStopResourcePage              startStopPage;

    // private SingleResource simResource;
    private String                             status;

    private WizardDialog                       wizDialog;
    private boolean                            dlgForceClosed;

    public CreateResourceWizard() {
        setWindowTitle("Create resources");
        IPath path = new Path("/icons/oic_logo_64x64.png");
        URL find = FileLocator.find(Activator.getDefault().getBundle(), path,
                null);
        setDefaultPageImageDescriptor(ImageDescriptor.createFromURL(find));

        setNeedsProgressMonitor(true);
    }

    @Override
    public void addPages() {
        mainPage = new MainPage();
        simpleResourceBasicDetailsPage = new SimpleResourceBasicDetailsPage();
        simpleResourceAddAttributePage = new SimpleResourceAddAttributePage();
        simpleResourceOtherDetailsPage = new SimpleResourceOtherDetailsPage();
        collectionResourceBasicDetailsPage = new CollectionResourceBasicDetailsPage();
        addResourcesToCollectionPage = new AddResourcesToCollectionPage();
        loadRamlPage = new LoadRamlPage();
        updatePropPage = new UpdatePropertiesPage();
        devicePage = new DevicePage();
        startStopPage = new StartStopResourcePage();

        addPage(mainPage);
        addPage(simpleResourceBasicDetailsPage);
        addPage(simpleResourceAddAttributePage);
        addPage(simpleResourceOtherDetailsPage);
        addPage(collectionResourceBasicDetailsPage);
        if (Activator.getDefault().getResourceManager().isAnyResourceExist())
            addPage(addResourcesToCollectionPage);
        addPage(loadRamlPage);
        addPage(updatePropPage);
        addPage(devicePage);
        addPage(startStopPage);
    }

    public void setWizardDialog(WizardDialog dlg) {
        wizDialog = dlg;
    }

    public WizardDialog getWizardDialog() {
        return wizDialog;
    }

    public boolean isDlgForceClosed() {
        return dlgForceClosed;
    }

    public void setDlgForceClosed(boolean dlgForceClosed) {
        this.dlgForceClosed = dlgForceClosed;
    }

    public String getConfigFilePath() {
        if (null == loadRamlPage) {
            return null;
        }
        return loadRamlPage.getConfigFilePath();
    }

    public int getResourceCount() {
        if (null == loadRamlPage) {
            return 0;
        }
        return loadRamlPage.getResourceCount();
    }

    @Override
    public boolean canFinish() {
        System.out.println("canFinish()");
        IWizardPage curPage = this.getContainer().getCurrentPage();
        System.out.println(curPage.getName());
        if ((curPage == updatePropPage && ((mainPage.getResourceOption() == ResourceOption.SIMPLE_FROM_RAML) || !Activator
                .getDefault().getResourceManager().isAnyResourceExist()))
                || (curPage == collectionResourceBasicDetailsPage
                        && !Activator.getDefault().getResourceManager()
                                .isAnyResourceExist() && collectionResourceBasicDetailsPage
                            .isSelectionDone())
                || curPage == addResourcesToCollectionPage
                || curPage == simpleResourceAddAttributePage
                || (curPage == loadRamlPage && loadRamlPage.isSelectionDone() && loadRamlPage
                        .isMultiResourceCreation()) || curPage == devicePage) {
            return true;
        }
        return false;
    }

    @Override
    public boolean performFinish() {
        System.out.println("performFinish()");
        final IWizardPage curPage = this.getContainer().getCurrentPage();
        System.out.println(curPage.getClass().getSimpleName());
        if (curPage == simpleResourceAddAttributePage) { // Handling Simple
            // Resource Creation
            // without RAML
            try {
                getContainer().run(true, true, new IRunnableWithProgress() {

                    @Override
                    public void run(IProgressMonitor monitor)
                            throws InvocationTargetException,
                            InterruptedException {
                        try {
                            monitor.beginTask(
                                    "Single Resource Creation Without RAML", 2);
                            monitor.worked(1);
                            createSingleResourceWithoutRAML();
                            monitor.worked(1);
                        } finally {
                            monitor.done();
                        }
                    }
                });
            } catch (InvocationTargetException e) {
                Activator.getDefault().getLogManager()
                        .log(Level.ERROR.ordinal(), new Date(), e.getMessage());
                e.printStackTrace();
            } catch (InterruptedException e) {
                Activator.getDefault().getLogManager()
                        .log(Level.ERROR.ordinal(), new Date(), e.getMessage());
                e.printStackTrace();
            }
        } else if (curPage == loadRamlPage) { // Handling multi-instance
            // creation of simple resource
            // with RAML
            try {
                getContainer().run(true, true, new IRunnableWithProgress() {

                    @Override
                    public void run(IProgressMonitor monitor)
                            throws InvocationTargetException,
                            InterruptedException {
                        try {
                            monitor.beginTask(
                                    "Single Resource Creation(multi-instance) With RAML",
                                    3);
                            monitor.worked(1);
                            createMultiInstanceSingleResourceWithoutRAML();
                            monitor.worked(2);
                        } finally {
                            monitor.done();
                        }
                    }
                });
            } catch (InvocationTargetException e) {
                Activator.getDefault().getLogManager()
                        .log(Level.ERROR.ordinal(), new Date(), e.getMessage());
                e.printStackTrace();
            } catch (InterruptedException e) {
                Activator.getDefault().getLogManager()
                        .log(Level.ERROR.ordinal(), new Date(), e.getMessage());
                e.printStackTrace();
            }
        } else if (curPage == updatePropPage) { // Handling the single instance
            // creation of simple resource
            // with RAML
            // String resURI = updatePropPage.getResURI();
            // Checking whether the uri is used by any other resource.
            if (Activator.getDefault().getResourceManager()
                    .isResourceExist(updatePropPage.getResURI())) {
                MessageDialog
                        .openError(getShell(), "Resource URI in use",
                                "Entered resource URI is in use. Please try a different one.");
                // TODO: Instead of MessageDialog, errors may be shown on wizard
                // itself.
                return false;
            }
            // Resource resource = loadRamlPage.getResource();
            // resource.setResourceName(updatePropPage.getResName());
            try {
                getContainer().run(true, true, new IRunnableWithProgress() {

                    @Override
                    public void run(IProgressMonitor monitor)
                            throws InvocationTargetException,
                            InterruptedException {
                        try {
                            monitor.beginTask("Completing Resource Creation", 2);
                            monitor.worked(1);
                            completeResourceCreationWithRAML();
                            monitor.worked(1);
                        } finally {
                            monitor.done();
                        }
                    }
                });
            } catch (InvocationTargetException e) {
                Activator.getDefault().getLogManager()
                        .log(Level.ERROR.ordinal(), new Date(), e.getMessage());
                e.printStackTrace();
            } catch (InterruptedException e) {
                Activator.getDefault().getLogManager()
                        .log(Level.ERROR.ordinal(), new Date(), e.getMessage());
                e.printStackTrace();
            }
        } else if (curPage == collectionResourceBasicDetailsPage
                || curPage == addResourcesToCollectionPage) { // Handling
            // Collection
            // Resource
            // Creation
            // without RAML
            if (mainPage.getResourceOption() == ResourceOption.COLLECTION_FROM_RAML) {
                Resource res = loadRamlPage.getResource();
                if (null != res && res instanceof CollectionResource) {
                    Set<Resource> selectedResources = addResourcesToCollectionPage
                            .getSelectedResourceList();
                    if (!selectedResources.isEmpty()) {
                        int addedCount = Activator
                                .getDefault()
                                .getResourceManager()
                                .addResourceToCollection(
                                        (CollectionResource) res,
                                        selectedResources);
                        if (addedCount > 0) {

                            status = "[" + addedCount + "/"
                                    + selectedResources.size()
                                    + "] resources added to the collection";

                            UiListenerHandler.getInstance()
                                    .resourceListUpdateUINotification(
                                            ResourceType.COLLECTION);
                        } else {
                            status = "Failed to add resources to the collection";
                        }
                    }
                }
            } else {
                // Checking whether the uri is used by any other resource.
                if (Activator
                        .getDefault()
                        .getResourceManager()
                        .isResourceExist(
                                collectionResourceBasicDetailsPage.getResURI())) {
                    MessageDialog
                            .openError(getShell(), "Resource URI in use",
                                    "Entered resource URI is in use. Please try a different one.");
                    // TODO: Instead of MessageDialog, errors may be shown on
                    // wizard
                    // itself.
                    return false;
                }
                try {
                    getContainer().run(true, true, new IRunnableWithProgress() {

                        @Override
                        public void run(IProgressMonitor monitor)
                                throws InvocationTargetException,
                                InterruptedException {
                            try {
                                monitor.beginTask(
                                        "Collection Resource Creation Without RAML",
                                        3);
                                monitor.worked(1);
                                createCollectionResourceWithoutRAML((curPage == addResourcesToCollectionPage) ? true
                                        : false);
                                monitor.worked(1);
                                if (curPage == addResourcesToCollectionPage) {
                                    UiListenerHandler.getInstance()
                                            .resourceListUpdateUINotification(
                                                    ResourceType.COLLECTION);
                                }
                                monitor.worked(1);
                            } finally {
                                monitor.done();
                            }
                        }
                    });
                } catch (InvocationTargetException e) {
                    Activator
                            .getDefault()
                            .getLogManager()
                            .log(Level.ERROR.ordinal(), new Date(),
                                    e.getMessage());
                    e.printStackTrace();
                } catch (InterruptedException e) {
                    Activator
                            .getDefault()
                            .getLogManager()
                            .log(Level.ERROR.ordinal(), new Date(),
                                    e.getMessage());
                    e.printStackTrace();
                }
            }
        } else if (curPage == devicePage) {
            final String devName = devicePage.getDeviceName();
            final Set<Resource> resourceList = devicePage
                    .getSelectedResourceList();
            try {
                getContainer().run(true, true, new IRunnableWithProgress() {

                    @Override
                    public void run(IProgressMonitor monitor)
                            throws InvocationTargetException,
                            InterruptedException {
                        try {
                            monitor.beginTask("Device Creation", 3);
                            monitor.worked(1);
                            createDevice(devName, resourceList);
                            monitor.worked(1);
                            UiListenerHandler.getInstance()
                                    .resourceListUpdateUINotification(
                                            ResourceType.DEVICE);
                            status = "Device Created.";
                            monitor.worked(1);
                        } finally {
                            monitor.done();
                        }
                    }
                });
            } catch (InvocationTargetException e) {
                Activator.getDefault().getLogManager()
                        .log(Level.ERROR.ordinal(), new Date(), e.getMessage());
                e.printStackTrace();
            } catch (InterruptedException e) {
                Activator.getDefault().getLogManager()
                        .log(Level.ERROR.ordinal(), new Date(), e.getMessage());
                e.printStackTrace();
            }
        }
        return true;
    }

    public MainPage getMainPage() {
        return mainPage;
    }

    public SimpleResourceBasicDetailsPage getSimpleResourceBasicDetailsPage() {
        return simpleResourceBasicDetailsPage;
    }

    public SimpleResourceAddAttributePage getSimpleResourceAddAttributePage() {
        return simpleResourceAddAttributePage;
    }

    public CollectionResourceBasicDetailsPage getCollectionResourceBasicDetailsPage() {
        return collectionResourceBasicDetailsPage;
    }

    public SimpleResourceOtherDetailsPage getSimpleResourceOtherDetailsPage() {
        return simpleResourceOtherDetailsPage;
    }

    public AddResourcesToCollectionPage getAddResourcesToCollectionPage() {
        return addResourcesToCollectionPage;
    }

    public LoadRamlPage getLoadRamlPage() {
        return loadRamlPage;
    }

    public UpdatePropertiesPage getUpdatePropPage() {
        return updatePropPage;
    }

    public DevicePage getDevicePage() {
        return devicePage;
    }

    public StartStopResourcePage getStartStopPage() {
        return startStopPage;
    }

    /*
     * public SingleResource getSimResource() { return simResource; }
     * 
     * public void setSimResource(SingleResource simResource) { this.simResource
     * = simResource; }
     */

    public void setStatus(String status) {
        this.status = status;
    }

    public String getStatus() {
        return status;
    }

    /*
     * public SingleResource getCreatedResource() { return simResource; }
     */

    private void createSingleResourceWithoutRAML() {
        SingleResource resource = new SingleResource();
        // Basic resource details
        resource.setResourceURI(simpleResourceBasicDetailsPage.getResURI());
        resource.setResourceName(simpleResourceBasicDetailsPage.getResName());
        resource.setResourceTypes(simpleResourceBasicDetailsPage.getResTypes());
        resource.setObservable(simpleResourceBasicDetailsPage.isObservable());
        // resource.setStarted(simpleResourceBasicDetailsPage.isStart());

        // Resource Attributes
        Map<String, LocalResourceAttribute> attributes = new HashMap<String, LocalResourceAttribute>();
        Set<AttributeHelper> attributeSet = simpleResourceAddAttributePage
                .getAttributes();
        if (null != attributeSet && !attributeSet.isEmpty()) {
            Iterator<AttributeHelper> itr = attributeSet.iterator();
            AttributeHelper attHelper;
            LocalResourceAttribute localResAtt;
            // List<String> attValues;
            while (itr.hasNext()) {
                attHelper = itr.next();
                if (null != attHelper) {
                    localResAtt = attHelper.convertToLocalResourceAttribute();

                    // Set the attribute value list.
                    /*
                     * attValues =
                     * Activator.getDefault().getResourceManager().getValueList
                     * (localResAtt); localResAtt.setAttValues(attValues);
                     */
                    attributes.put(
                            localResAtt.getResourceAttributeRef().name(),
                            localResAtt);
                }
            }
            resource.setResourceAttributes(attributes);
        }

        // Request types
        resource.setGetAllowed(simpleResourceOtherDetailsPage.isGetChecked());
        resource.setPutAllowed(simpleResourceOtherDetailsPage.isPutChecked());
        resource.setPostAllowed(simpleResourceOtherDetailsPage.isPostChecked());

        // Call method of ResourceManager and update the response in the status
        // string.
        try {
            boolean result = Activator.getDefault().getResourceManager()
                    .createSingleResource(resource);
            if (result) {
                status = "Resource created.";
                resource.printResourceInfo();
            } else {
                status = "Failed to create resource.";
            }
        } catch (SimulatorException e) {
            status = "Failed to create resource.\n"
                    + Utility.getSimulatorErrorString(e, null);
        }
    }

    private void completeResourceCreationWithRAML() {
        try {
            boolean result;
            Resource res = loadRamlPage.getResource();
            if (res instanceof SingleResource) {
                result = Activator
                        .getDefault()
                        .getResourceManager()
                        .completeSingleResourceCreationByRAML(res,
                                updatePropPage.getResURI(),
                                updatePropPage.getResName(), false);
            } else {
                result = Activator
                        .getDefault()
                        .getResourceManager()
                        .completeCollectionResourceCreationByRAML(res,
                                updatePropPage.getResURI(),
                                updatePropPage.getResName());
            }

            if (result) {
                status = "Resource created.";
                loadRamlPage.getResource().printResourceInfo();
            } else {
                status = "Failed to create resource.";
            }
        } catch (SimulatorException e) {
            status = "Failed to create resource.\n"
                    + Utility.getSimulatorErrorString(e, null);
        }
    }

    private void createMultiInstanceSingleResourceWithoutRAML() {
        try {
            int toCreateCount = loadRamlPage.getResourceCount();
            int resCreatedCount = Activator
                    .getDefault()
                    .getResourceManager()
                    .createSingleResourceMultiInstances(
                            loadRamlPage.getConfigFilePath(), toCreateCount);
            if (resCreatedCount > 0) {
                status = "[" + resCreatedCount + " out of " + toCreateCount
                        + "]";
                status += ((resCreatedCount == 1) ? "resource" : "resources")
                        + " created successfully.";
            } else {
                status = "Failed to create resources.";
            }
        } catch (SimulatorException e) {
            status = "Failed to create resource.\n"
                    + Utility.getSimulatorErrorString(e, null);
        }
    }

    private void createCollectionResourceWithoutRAML(boolean childResExist) {
        CollectionResource resource = new CollectionResource();
        // Basic resource details
        resource.setResourceURI(collectionResourceBasicDetailsPage.getResURI());
        resource.setResourceName(collectionResourceBasicDetailsPage
                .getResName());
        resource.setResourceTypes(collectionResourceBasicDetailsPage
                .getResTypes());
        resource.setObservable(collectionResourceBasicDetailsPage
                .isObservable());
        // resource.setStarted(simpleResourceBasicDetailsPage.isStart());

        // Call method of ResourceManager and update the response in the status
        // string.
        try {
            boolean result = Activator.getDefault().getResourceManager()
                    .createCollectionResource(resource);
            if (result) {
                status = "Resource created.";
                // Adding child resources.
                if (childResExist) {
                    Set<Resource> selectedResources = addResourcesToCollectionPage
                            .getSelectedResourceList();
                    if (!selectedResources.isEmpty()) {
                        int addedCount = Activator
                                .getDefault()
                                .getResourceManager()
                                .addResourceToCollection(resource,
                                        selectedResources);
                        if (addedCount > 0) {
                            status = "[" + addedCount + "/"
                                    + selectedResources.size()
                                    + "] resources added to the collection";
                        } else {
                            status = "Failed to add resources to the collection.";
                        }
                    }
                }
            } else {
                status = "Failed to create resource.";
            }
        } catch (SimulatorException e) {
            status = "Failed to create resource.\n"
                    + Utility.getSimulatorErrorString(e, null);
        }
    }

    private void createDevice(String deviceName, Set<Resource> childs) {
        Activator.getDefault().getResourceManager()
                .createDevice(deviceName, childs);
    }

}