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

package oic.simulator.serviceprovider.manager;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Vector;

import org.eclipse.swt.widgets.Display;
import org.oic.simulator.AttributeProperty;
import org.oic.simulator.AttributeProperty.Type;
import org.oic.simulator.AttributeValue;
import org.oic.simulator.AttributeValue.TypeInfo;
import org.oic.simulator.AttributeValue.ValueType;
import org.oic.simulator.ILogger.Level;
import org.oic.simulator.PlatformInfo;
import org.oic.simulator.SimulatorException;
import org.oic.simulator.SimulatorManager;
import org.oic.simulator.SimulatorResourceAttribute;
import org.oic.simulator.SimulatorResourceModel;
import org.oic.simulator.server.Observer;
import org.oic.simulator.server.SimulatorCollectionResource;
import org.oic.simulator.server.SimulatorResource;
import org.oic.simulator.server.SimulatorResource.AutoUpdateListener;
import org.oic.simulator.server.SimulatorResource.AutoUpdateType;
import org.oic.simulator.server.SimulatorResource.ObserverListener;
import org.oic.simulator.server.SimulatorResource.ResourceModelChangeListener;
import org.oic.simulator.server.SimulatorSingleResource;

import oic.simulator.serviceprovider.Activator;
import oic.simulator.serviceprovider.model.AttributeElement;
import oic.simulator.serviceprovider.model.CollectionResource;
import oic.simulator.serviceprovider.model.Device;
import oic.simulator.serviceprovider.model.LocalResourceAttribute;
import oic.simulator.serviceprovider.model.MetaProperty;
import oic.simulator.serviceprovider.model.Resource;
import oic.simulator.serviceprovider.model.ResourceType;
import oic.simulator.serviceprovider.model.SRMItem;
import oic.simulator.serviceprovider.model.SingleResource;
import oic.simulator.serviceprovider.utils.Constants;
import oic.simulator.serviceprovider.utils.Utility;

/**
 * This class acts as an interface between the simulator java SDK and the
 * various UI modules. It maintains all the details of resources and provides
 * other UI modules with the information required. It also handles model change,
 * automation, and observer related events from native layer and propagates
 * those events to the registered UI listeners.
 */
public class ResourceManager {

    private Data                           data;

    private Resource                       currentResourceInSelection;

    private Device                         currentDeviceInSelection;

    private ResourceModelChangeListener    resourceModelChangeListener;

    private AutoUpdateListener             automationListener;

    private ObserverListener               observer;

    private NotificationSynchronizerThread synchronizerThread;

    private Thread                         threadHandle;

    private String                         deviceName;
    private PlatformInfo                   platformInfo;

    public ResourceManager() {
        data = new Data();

        // Set the default device and platform information
        deviceName = "IoTivity Simulator";
        try {
            SimulatorManager.setDeviceInfo(deviceName);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(),
                            new Date(),
                            "Error while registering the device info.\n"
                                    + Utility.getSimulatorErrorString(e, null));
        }

        platformInfo = new PlatformInfo();
        platformInfo.setPlatformID("Samsung Platform Identifier");
        platformInfo.setManufacturerName("Samsung");
        platformInfo.setManufacturerUrl("www.samsung.com");
        platformInfo.setModelNumber("Samsung Model Num01");
        platformInfo.setDateOfManufacture("2015-09-10T11:10:30Z");
        platformInfo.setPlatformVersion("PlatformVersion01");
        platformInfo.setOperationSystemVersion("OSVersion01");
        platformInfo.setHardwareVersion("HardwareVersion01");
        platformInfo.setFirmwareVersion("FirwareVersion01");
        platformInfo.setSupportUrl("http://www.samsung.com/support");
        platformInfo.setSystemTime("2015-09-10T11:10:30Z");
        try {
            SimulatorManager.setPlatformInfo(platformInfo);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(),
                            new Date(),
                            "Error while registering the platform info.\n"
                                    + Utility.getSimulatorErrorString(e, null));
        }

        resourceModelChangeListener = new ResourceModelChangeListener() {

            @Override
            public void onResourceModelChanged(final String resourceURI,
                    final SimulatorResourceModel resourceModelN) {
                synchronizerThread.addToQueue(new Runnable() {

                    @Override
                    public void run() {
                        if (null == resourceURI || null == resourceModelN) {
                            return;
                        }

                        Display.getDefault().asyncExec(new Runnable() {
                            @Override
                            public void run() {
                                Resource resource = data
                                        .getResourceByURI(resourceURI);
                                if (null != resource) {
                                    resource.setResourceRepresentation(resourceModelN);
                                }
                            }
                        });
                    }
                });
            }
        };

        automationListener = new AutoUpdateListener() {

            @Override
            public void onUpdateComplete(final String resourceURI,
                    final int automationId) {
                synchronizerThread.addToQueue(new Runnable() {

                    @Override
                    public void run() {
                        SingleResource resource = data
                                .getSingleResourceByURI(resourceURI);
                        if (null == resource) {
                            return;
                        }
                        // Checking whether this notification is for an
                        // attribute or a resource
                        if (resource.isResourceAutomationInProgress()) {
                            changeResourceLevelAutomationStatus(resource, false);
                            // Notify the UI listeners
                            UiListenerHandler.getInstance()
                                    .automationCompleteUINotification(resource,
                                            null);
                        } else if (resource.isAttributeAutomationInProgress()) {
                            // Find the attribute with the given automation id
                            final AttributeElement attribute = getAttributeWithGivenAutomationId(
                                    resource, automationId);
                            if (null != attribute) {
                                // Display.getDefault().asyncExec(new Runnable()
                                // {
                                // @Override
                                // public void run() {
                                attribute.setAutoUpdateState(false);
                                // }
                                // });
                                resource.setAttributeAutomationInProgress(isAnyAttributeInAutomation(resource));
                                // Notify the UI listeners
                                /*
                                 * UiListenerHandler .getInstance()
                                 * .automationCompleteUINotification( resource,
                                 * attribute
                                 * .getSimulatorResourceAttribute().name());
                                 */
                            } else {
                                // TODO: Temporarily reset the attribute
                                // automation status to false for making
                                // resource-level automation work after
                                // attribute-level automations.
                                resource.setAttributeAutomationInProgress(false);
                            }
                        }
                    }
                });
            }
        };

        observer = new ObserverListener() {

            public void onObserverChanged(final String resourceURI,
                    final int status, final Observer observer) {
                new Thread() {
                    @Override
                    public void run() {
                        if (null == resourceURI || null == observer) {
                            return;
                        }
                        Resource resource = data.getResourceByURI(resourceURI);
                        if (null == resource) {
                            return;
                        }
                        // Update the observers information
                        if (status == 0) {
                            resource.addObserverInfo(observer);
                        } else {
                            resource.removeObserverInfo(observer);
                        }
                        // Notify the UI listeners
                        UiListenerHandler.getInstance()
                                .observerListChangedUINotification(resource);
                    }
                }.start();
            }

            @Override
            public void onObserverAdded(String resourceURI, Observer observer) {
                onObserverChanged(resourceURI, 0, observer);
            }

            @Override
            public void onObserverRemoved(String resourceURI, Observer observer) {
                onObserverChanged(resourceURI, 1, observer);
            }
        };

        synchronizerThread = new NotificationSynchronizerThread();
        threadHandle = new Thread(synchronizerThread);
        threadHandle.setName("Simulator service provider event queue");
        threadHandle.start();
    }

    private static class NotificationSynchronizerThread implements Runnable {

        LinkedList<Runnable> notificationQueue = new LinkedList<Runnable>();

        @Override
        public void run() {
            while (!Thread.interrupted()) {
                synchronized (this) {
                    try {
                        while (notificationQueue.isEmpty()) {
                            this.wait();
                            break;
                        }
                    } catch (InterruptedException e) {
                        return;
                    }
                }

                Runnable thread;
                synchronized (this) {
                    thread = notificationQueue.pop();
                }
                try {
                    thread.run();
                } catch (Exception e) {
                    if (e instanceof InterruptedException) {
                        return;
                    }
                    e.printStackTrace();
                }
            }
        }

        public void addToQueue(Runnable event) {
            synchronized (this) {
                notificationQueue.add(event);
                this.notify();
            }
        }
    }

    public String getDeviceName() {
        return deviceName;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
        try {
            SimulatorManager.setDeviceInfo(deviceName);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(),
                            new Date(),
                            "Error while registering the device info.\n"
                                    + Utility.getSimulatorErrorString(e, null));
        }
    }

    public List<MetaProperty> getPlatformInfo() {
        List<MetaProperty> metaProperties = new ArrayList<MetaProperty>();
        metaProperties.add(new MetaProperty(Constants.PLATFORM_ID, platformInfo
                .getPlatformID()));
        metaProperties.add(new MetaProperty(Constants.PLATFORM_MANUFAC_NAME,
                platformInfo.getManufacturerName()));
        metaProperties.add(new MetaProperty(Constants.PLATFORM_MANUFAC_URL,
                platformInfo.getManufacturerUrl()));
        metaProperties.add(new MetaProperty(Constants.PLATFORM_MODEL_NO,
                platformInfo.getModelNumber()));
        metaProperties.add(new MetaProperty(Constants.PLATFORM_DATE_OF_MANUFAC,
                platformInfo.getDateOfManufacture()));
        metaProperties.add(new MetaProperty(Constants.PLATFORM_VERSION,
                platformInfo.getPlatformVersion()));
        metaProperties.add(new MetaProperty(Constants.PLATFORM_OS_VERSION,
                platformInfo.getOperationSystemVersion()));
        metaProperties.add(new MetaProperty(
                Constants.PLATFORM_HARDWARE_VERSION, platformInfo
                        .getHardwareVersion()));
        metaProperties.add(new MetaProperty(
                Constants.PLATFORM_FIRMWARE_VERSION, platformInfo
                        .getFirmwareVersion()));
        metaProperties.add(new MetaProperty(Constants.PLATFORM_SUPPORT_URL,
                platformInfo.getSupportUrl()));
        metaProperties.add(new MetaProperty(Constants.PLATFORM_SYSTEM_TIME,
                platformInfo.getSystemTime()));
        return metaProperties;
    }

    public void setPlatformInfo(List<MetaProperty> metaProperties) {
        if (null == metaProperties || metaProperties.size() < 1) {
            return;
        }
        Iterator<MetaProperty> itr = metaProperties.iterator();
        MetaProperty prop;
        String propName;
        String propValue;
        while (itr.hasNext()) {
            prop = itr.next();
            propName = prop.getPropName();
            propValue = prop.getPropValue();
            if (propName.equals(Constants.PLATFORM_ID)) {
                platformInfo.setPlatformID(propValue);
            } else if (propName.equals(Constants.PLATFORM_MANUFAC_NAME)) {
                platformInfo.setManufacturerName(propValue);
            } else if (propName.equals(Constants.PLATFORM_MANUFAC_URL)) {
                platformInfo.setManufacturerUrl(propValue);
            } else if (propName.equals(Constants.PLATFORM_MODEL_NO)) {
                platformInfo.setModelNumber(propValue);
            } else if (propName.equals(Constants.PLATFORM_DATE_OF_MANUFAC)) {
                platformInfo.setDateOfManufacture(propValue);
            } else if (propName.equals(Constants.PLATFORM_VERSION)) {
                platformInfo.setPlatformVersion(propValue);
            } else if (propName.equals(Constants.PLATFORM_OS_VERSION)) {
                platformInfo.setOperationSystemVersion(propValue);
            } else if (propName.equals(Constants.PLATFORM_HARDWARE_VERSION)) {
                platformInfo.setHardwareVersion(propValue);
            } else if (propName.equals(Constants.PLATFORM_FIRMWARE_VERSION)) {
                platformInfo.setFirmwareVersion(propValue);
            } else if (propName.equals(Constants.PLATFORM_SUPPORT_URL)) {
                platformInfo.setSupportUrl(propValue);
            } else if (propName.equals(Constants.PLATFORM_SYSTEM_TIME)) {
                platformInfo.setSystemTime(propValue);
            }
        }
        try {
            SimulatorManager.setPlatformInfo(platformInfo);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(),
                            new Date(),
                            "Error while registering the platform info.\n"
                                    + Utility.getSimulatorErrorString(e, null));
        }
    }

    public synchronized Resource getCurrentResourceInSelection() {
        return currentResourceInSelection;
    }

    public synchronized void setCurrentResourceInSelection(Resource resource) {
        this.currentResourceInSelection = resource;
    }

    public synchronized Device getCurrentDeviceInSelection() {
        return currentDeviceInSelection;
    }

    public synchronized void setCurrentDeviceInSelection(Device dev) {
        this.currentDeviceInSelection = dev;
    }

    public boolean isResourceExist(String resourceURI) {
        return data.isResourceExist(resourceURI);
    }

    public boolean isAnyResourceExist() {
        return data.isAnyResourceExist();
    }

    public boolean createSingleResource(SingleResource resource)
            throws SimulatorException {
        if (null == resource) {
            return false;
        }
        String resType = (String) resource.getResourceTypes().toArray()[0];
        try {
            // 1. Create the resource.
            SimulatorResource jSimulatorResource = SimulatorManager
                    .createResource(SimulatorResource.Type.SINGLE,
                            resource.getResourceName(),
                            resource.getResourceURI(), resType);
            if (null == jSimulatorResource
                    || !(jSimulatorResource instanceof SimulatorSingleResource)) {
                return false;
            }
            SimulatorSingleResource jSimulatorSingleResource = (SimulatorSingleResource) jSimulatorResource;
            resource.setSimulatorResource(jSimulatorSingleResource);

            // 2. Cancel observable property if requested by user.
            if (!resource.isObservable()) {
                jSimulatorSingleResource.setObservable(false);
            }

            // 3. Set the model change listener.
            jSimulatorSingleResource
                    .setResourceModelChangeListener(resourceModelChangeListener);

            // 4. Set the observer listener if the resource is observable.
            if (resource.isObservable()) {
                jSimulatorSingleResource.setObserverListener(observer);
            }

            // 5. Add attributes.
            Map<String, LocalResourceAttribute> attributes = resource
                    .getResourceAttributes();
            if (null != attributes && !attributes.isEmpty()) {
                Set<String> keySet = attributes.keySet();
                Iterator<String> itr = keySet.iterator();

                String attName;
                LocalResourceAttribute localAtt;
                SimulatorResourceAttribute simResAtt;

                while (itr.hasNext()) {
                    attName = itr.next();
                    localAtt = attributes.get(attName);
                    if (null == localAtt) {
                        continue;
                    }
                    simResAtt = localAtt.getResourceAttributeRef();
                    if (null == simResAtt) {
                        continue;
                    }
                    jSimulatorSingleResource.addAttribute(simResAtt);
                }

                // 6. Get the resource model java object reference.
                resource.setResourceModel(jSimulatorSingleResource
                        .getResourceModel());

                resource.setResourceRepresentation(resource.getResourceModel());
            }

            // 7. Register the resource with the platform.
            jSimulatorSingleResource.start();
            resource.setStarted(true);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
            throw e;
        }

        // 8. Add to local cache.
        data.addResource(resource);

        // 9. Update UI listeners
        UiListenerHandler.getInstance().resourceCreatedUINotification(
                ResourceType.SINGLE);

        return true;
    }

    public boolean createCollectionResource(CollectionResource resource)
            throws SimulatorException {
        if (null == resource) {
            return false;
        }
        String resType = (String) resource.getResourceTypes().toArray()[0];
        try {
            // 1. Create the resource.
            SimulatorResource jSimulatorResource = SimulatorManager
                    .createResource(SimulatorResource.Type.COLLECTION,
                            resource.getResourceName(),
                            resource.getResourceURI(), resType);
            if (null == jSimulatorResource
                    || !(jSimulatorResource instanceof SimulatorCollectionResource)) {
                return false;
            }
            SimulatorCollectionResource jSimulatorCollectionResource = (SimulatorCollectionResource) jSimulatorResource;
            resource.setSimulatorResource(jSimulatorCollectionResource);

            // 2. Cancel observable property if requested by user.
            if (!resource.isObservable()) {
                jSimulatorCollectionResource.setObservable(false);
            }

            // 3. Set the observer listener if the resource is observable.
            if (resource.isObservable()) {
                jSimulatorCollectionResource.setObserverListener(observer);
            }

            // 4. Set the model change listener.
            jSimulatorCollectionResource
                    .setResourceModelChangeListener(resourceModelChangeListener);

            // set resource model
            resource.setResourceRepresentation(jSimulatorCollectionResource
                    .getResourceModel());;

            // 5. Register the resource with the platform.
            jSimulatorCollectionResource.start();
            resource.setStarted(true);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
            throw e;
        }

        // 6. Add to local cache.
        data.addResource(resource);

        // 7. Update UI listeners
        UiListenerHandler.getInstance().resourceCreatedUINotification(
                ResourceType.COLLECTION);

        return true;
    }

    public Resource createResourceByRAML(String configFilePath)
            throws SimulatorException {
        Resource resource = null;
        try {
            // 1. Create the resource
            SimulatorResource jSimulatorResource = SimulatorManager
                    .createResource(configFilePath);
            if (null == jSimulatorResource) {
                return null;
            }
            if (jSimulatorResource instanceof SimulatorSingleResource) {
                resource = new SingleResource();
            } else {
                resource = new CollectionResource();
            }
            resource.setSimulatorResource(jSimulatorResource);

            // 2. Fetch and locally store the resource name and uri.
            String uri = jSimulatorResource.getURI();
            if (null == uri || uri.trim().isEmpty()) {
                return null;
            }
            resource.setResourceURI(uri.trim());

            String name = jSimulatorResource.getName();
            if (null == name || name.trim().isEmpty()) {
                return null;
            }
            resource.setResourceName(name.trim());
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
            throw e;
        }
        return resource;
    }

    /**
     * This method can set/change the resource uri and name of an already
     * created resource which is not yet registered with the platform. This
     * method registers the model change and observer listeners, registers the
     * resource, fetches the resource attributes, updates the local cache and
     * notifies the UI listeners.
     */
    public boolean completeSingleResourceCreationByRAML(Resource resource,
            String uri, String name, boolean multiInstance)
            throws SimulatorException {
        if (null == resource || !(resource instanceof SingleResource)) {
            return false;
        }
        try {
            SingleResource singleRes = (SingleResource) resource;

            SimulatorSingleResource jSimulatorSingleResource = (SimulatorSingleResource) resource
                    .getSimulatorResource();
            if (null == jSimulatorSingleResource) {
                return false;
            }

            // 1. Update resource URI and Name if they are changed.
            String newUri = uri.trim();
            String newName = name.trim();

            if (multiInstance) {
                singleRes.setResourceURI(newUri);
                singleRes.setResourceName(newName);
            } else {
                if (!singleRes.getResourceURI().equals(newUri)) {
                    jSimulatorSingleResource.setURI(newUri);
                    singleRes.setResourceURI(newUri);
                }
                if (!singleRes.getResourceName().equals(newName)) {
                    jSimulatorSingleResource.setName(newName);
                    singleRes.setResourceName(newName);
                }
            }

            // 2. Set the model change listener.
            jSimulatorSingleResource
                    .setResourceModelChangeListener(resourceModelChangeListener);

            // 3. Set the observer listener if the resource is observable.
            if (jSimulatorSingleResource.isObservable()) {
                jSimulatorSingleResource.setObserverListener(observer);
                singleRes.setObservable(true);
            }

            // 4. Fetch the resource model.
            SimulatorResourceModel jResModel = jSimulatorSingleResource
                    .getResourceModel();
            if (null == jResModel) {
                return false;
            }
            singleRes.setResourceModel(jResModel);

            // 5. Fetch the basic details of the resource.
            singleRes.addResourceType(jSimulatorSingleResource
                    .getResourceType());
            singleRes
                    .setResourceInterfaces(Utility
                            .convertVectorToSet(jSimulatorSingleResource
                                    .getInterface()));

            // 6. Register the resource with the platform.
            jSimulatorSingleResource.start();
            singleRes.setStarted(true);

            singleRes.setResourceRepresentation(jResModel);

            // 7. Fetch the resource attributes.
            Map<String, LocalResourceAttribute> resourceAttributeMap;
            resourceAttributeMap = fetchResourceAttributesFromModel(jResModel);
            if (null != resourceAttributeMap) {
                singleRes.setResourceAttributes(resourceAttributeMap);
            }

            // 8. Add to local cache.
            data.addResource(singleRes);

            // 9. Update UI listeners for single instance creation
            if (!multiInstance)
                UiListenerHandler.getInstance().resourceCreatedUINotification(
                        ResourceType.SINGLE);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
            throw e;
        }
        return true;
    }

    /**
     * This method can set/change the resource uri and name of an already
     * created resource which is not yet registered with the platform. This
     * method registers the model change and observer listeners, registers the
     * resource, fetches the resource attributes, updates the local cache and
     * notifies the UI listeners.
     */
    public boolean completeCollectionResourceCreationByRAML(Resource resource,
            String uri, String name) throws SimulatorException {
        if (null == resource || !(resource instanceof CollectionResource)) {
            return false;
        }
        try {
            CollectionResource collectionRes = (CollectionResource) resource;

            SimulatorCollectionResource jSimulatorCollectionResource = (SimulatorCollectionResource) resource
                    .getSimulatorResource();
            if (null == jSimulatorCollectionResource) {
                return false;
            }

            // 1. Update resource URI and Name if they are changed.
            String newUri = uri.trim();
            String newName = name.trim();

            if (!collectionRes.getResourceURI().equals(newUri)) {
                jSimulatorCollectionResource.setURI(newUri);
                collectionRes.setResourceURI(newUri);
            }
            if (!collectionRes.getResourceName().equals(newName)) {
                jSimulatorCollectionResource.setName(newName);
                collectionRes.setResourceName(newName);
            }

            // 2. Set the model change listener.
            jSimulatorCollectionResource
                    .setResourceModelChangeListener(resourceModelChangeListener);

            // 3. Fetch the resource model.
            SimulatorResourceModel jResModel = jSimulatorCollectionResource
                    .getResourceModel();
            if (null == jResModel) {
                return false;
            }
            collectionRes.setResourceModel(jResModel);

            // 4. Fetch the basic details of the resource.
            collectionRes.addResourceType(jSimulatorCollectionResource
                    .getResourceType());
            collectionRes.setResourceInterfaces(Utility
                    .convertVectorToSet(jSimulatorCollectionResource
                            .getInterface()));

            // 5. Set the observer listener if the resource is observable.
            if (jSimulatorCollectionResource.isObservable()) {
                jSimulatorCollectionResource.setObserverListener(observer);
                collectionRes.setObservable(true);
            }

            // 6. Register the resource with the platform.
            jSimulatorCollectionResource.start();
            collectionRes.setStarted(true);

            collectionRes.setResourceRepresentation(jResModel);

            // 7. Fetch the resource attributes.
            Map<String, LocalResourceAttribute> resourceAttributeMap;
            resourceAttributeMap = fetchResourceAttributesFromModel(jResModel);
            if (null != resourceAttributeMap) {
                collectionRes.setResourceAttributes(resourceAttributeMap);
            }

            // 6. Add to local cache.
            data.addResource(collectionRes);

            // 7. Update UI listeners for single instance creation
            UiListenerHandler.getInstance().resourceCreatedUINotification(
                    ResourceType.COLLECTION);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
            throw e;
        }
        return true;
    }

    public int createSingleResourceMultiInstances(String configFile, int count)
            throws SimulatorException {
        int createCount = 0;
        try {
            Vector<SimulatorResource> jSimulatorResources = SimulatorManager
                    .createResource(configFile, count);
            if (null == jSimulatorResources || jSimulatorResources.size() < 1) {
                return 0;
            }
            SimulatorSingleResource jResource;
            SingleResource resource;
            boolean result;
            for (SimulatorResource jSimulatorResource : jSimulatorResources) {
                jResource = (SimulatorSingleResource) jSimulatorResource;
                resource = new SingleResource();
                resource.setSimulatorResource(jResource);
                try {
                    result = completeSingleResourceCreationByRAML(resource,
                            jResource.getURI(), jResource.getName(), true);
                    if (result) {
                        createCount++;
                    }
                } catch (SimulatorException eInner) {
                    Activator
                            .getDefault()
                            .getLogManager()
                            .log(Level.ERROR.ordinal(),
                                    new Date(),
                                    Utility.getSimulatorErrorString(eInner,
                                            null));
                }
            }
            if (createCount > 0) {
                UiListenerHandler.getInstance().resourceCreatedUINotification(
                        ResourceType.SINGLE);
            }
        } catch (SimulatorException eOuter) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(eOuter, null));
            throw eOuter;
        }
        return createCount;
    }

    public void createDevice(String deviceName, Set<Resource> childs) {
        // 1. Create device
        Device dev = new Device();
        dev.setDeviceName(deviceName);
        data.addDevice(dev);

        // 2. Add children to device
        if (null != childs && !childs.isEmpty())
            addResourceToDevice(dev, childs);

        // 3. Update ui listeners
        UiListenerHandler.getInstance().resourceListUpdateUINotification(
                ResourceType.DEVICE);
    }

    private Map<String, LocalResourceAttribute> fetchResourceAttributesFromModel(
            SimulatorResourceModel jResModel) throws SimulatorException {
        Map<String, LocalResourceAttribute> resourceAttributeMap = null;
        if (null != jResModel) {
            Map<String, SimulatorResourceAttribute> jAttributeMap;
            jAttributeMap = jResModel.getAttributes();
            if (null != jAttributeMap) {
                resourceAttributeMap = new HashMap<String, LocalResourceAttribute>();
                Iterator<String> itr = jAttributeMap.keySet().iterator();
                String attName;
                SimulatorResourceAttribute jResAtt;
                LocalResourceAttribute localAtt;
                while (itr.hasNext()) {
                    attName = itr.next();
                    if (null != attName) {
                        jResAtt = jAttributeMap.get(attName);
                        if (null != jResAtt) {
                            localAtt = new LocalResourceAttribute();

                            localAtt.setResourceAttributeRef(jResAtt);

                            // Initially disabling the automation
                            localAtt.setAutomationInProgress(false);

                            // Assigning the default automation interval
                            localAtt.setAutomationUpdateInterval(Constants.DEFAULT_AUTOMATION_INTERVAL);

                            // Setting the default automation type
                            localAtt.setAutomationType(Constants.DEFAULT_AUTOMATION_TYPE);

                            resourceAttributeMap.put(attName, localAtt);
                        }
                    }
                }
            }
        }
        return resourceAttributeMap;

    }

    // This method gives all known possible values of the attribute in string
    // format. It takes allowed values or range of values whichever is available
    private List<String> getValueList(SimulatorResourceAttribute attributeN) {
        AttributeProperty attProp = attributeN.property();
        if (null == attProp) {
            return null;
        }
        List<String> valueList = new ArrayList<String>();
        Type valuesType = attProp.type();
        if (valuesType == Type.VALUESET) {
            Object[] allowedValues = attProp.valueSet();
            if (null != allowedValues && allowedValues.length > 0) {
                for (Object value : allowedValues) {
                    if (null != value) {
                        valueList.add(String.valueOf(((AttributeValue) value)
                                .get()));
                    }
                }
            }
        } else if (valuesType == Type.RANGE) {
            double minD = attProp.min();
            double maxD = attProp.max();
            for (double value = minD; value <= maxD; value++) {
                valueList.add(String.valueOf(value));
            }
        }
        Object attValue = attributeN.value().get();
        if (valueList.size() < 1 && null != attValue) {
            valueList.add(String.valueOf(attValue));
        }
        return valueList;
    }

    public List<Resource> getResourceList() {
        List<Resource> resourceList = data.getResources();
        if (null == resourceList) {
            return null;
        }
        // Sort the list
        Collections.sort(resourceList, Utility.resourceComparator);

        return resourceList;
    }

    public List<SingleResource> getSingleResourceList() {
        List<SingleResource> resourceList = data.getSingleResources();
        if (null == resourceList) {
            return null;
        }
        // Sort the list
        Collections.sort(resourceList, Utility.singleResourceComparator);

        return resourceList;
    }

    public List<CollectionResource> getCollectionResourceList() {
        List<CollectionResource> resourceList = data.getCollectionResources();
        if (null == resourceList) {
            return null;
        }
        // Sort the list
        Collections.sort(resourceList, Utility.collectionResourceComparator);

        return resourceList;
    }

    public List<Device> getDeviceList() {
        List<Device> deviceList = data.getDevices();
        if (null == deviceList) {
            return null;
        }
        // Sort the list
        Collections.sort(deviceList, Utility.deviceComparator);
        return deviceList;
    }

    // Returns the number of resources which are added properly to the
    // collection.
    public int addResourceToCollection(CollectionResource collectionParent,
            Set<Resource> childs) {
        if (null == collectionParent || null == childs || childs.isEmpty()) {
            return -1;
        }
        Iterator<Resource> itr = childs.iterator();
        Resource res;
        int count = childs.size();
        while (itr.hasNext()) {
            res = itr.next();
            try {
                addResourceToCollection(collectionParent, res);
            } catch (SimulatorException e) {
                count--;
            }
        }
        return count;
    }

    public void addResourceToCollection(CollectionResource collectionParent,
            Resource child) throws SimulatorException {
        if (null == collectionParent || null == child) {
            return;
        }
        try {
            // 1. Add child to collection
            collectionParent.addChildResource(child);

            // 2. Add a reference to the collection in the child
            if (child instanceof SingleResource) {
                ((SingleResource) child)
                        .addCollectionMembership(collectionParent);
            } else {
                ((CollectionResource) child).addMembership(collectionParent);
            }
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
            throw e;
        }
    }

    public int addResourceToCollection(Set<CollectionResource> collections,
            Resource child) {
        if (null == collections || collections.isEmpty() || null == child) {
            return -1;
        }
        Iterator<CollectionResource> itr = collections.iterator();
        CollectionResource res;
        int count = collections.size();
        while (itr.hasNext()) {
            res = itr.next();
            try {
                addResourceToCollection(res, child);
            } catch (SimulatorException e) {
                count--;
            }
        }
        return count;
    }

    public void addResourceToDevice(Device dev, Set<Resource> childs) {
        // 1. Add children to the device.
        dev.addChildResource(childs);

        // 2. Add a reference to the device in all children.
        Iterator<Resource> itr = childs.iterator();
        Resource res;
        while (itr.hasNext()) {
            res = itr.next();
            if (res instanceof SingleResource) {
                ((SingleResource) res).addDeviceMembership(dev);
            } else {
                ((CollectionResource) res).addDeviceMembership(dev);
            }
        }
    }

    public void addResourceToDevice(Device dev, Resource child) {
        // 1. Add child to the device.
        dev.addChildResource(child);

        // 2. Add a reference to the device in the child.
        if (child instanceof SingleResource) {
            ((SingleResource) child).addDeviceMembership(dev);
        } else {
            ((CollectionResource) child).addDeviceMembership(dev);
        }
    }

    public void addResourceToDevice(Set<Device> devices, Resource child) {
        // 1. Add device reference in child.
        if (child instanceof SingleResource)
            ((SingleResource) child).addDeviceMembership(devices);
        else
            ((CollectionResource) child).addDeviceMembership(devices);

        // 2. Add a reference to the child in all devices.
        Iterator<Device> itr = devices.iterator();
        Device dev;
        while (itr.hasNext()) {
            dev = itr.next();
            dev.addChildResource(child);
        }
    }

    public int removeResourceFromCollection(
            Set<CollectionResource> collections, Resource resource) {
        // 1. Remove the reference of resource from all the collections.
        Iterator<CollectionResource> itr = collections.iterator();
        CollectionResource colRes;
        int count = collections.size();
        while (itr.hasNext()) {
            colRes = itr.next();
            try {
                removeResourceFromCollection(colRes, resource);
            } catch (SimulatorException e) {
                count--;
            }
        }
        return count;

    }

    public void removeResourceFromDevice(Set<Device> devices, Resource resource) {
        // 1. Remove the reference of resource from all the devices.
        Iterator<Device> itr = devices.iterator();
        Device dev;
        while (itr.hasNext()) {
            dev = itr.next();
            dev.removeChildResource(resource);
        }

        // 2. Remove the reference of devices from the resource.
        resource.removeDeviceMembership(devices);
    }

    // Returns the count of resources removed from the collection
    public int removeResourcesFromCollection(CollectionResource colRes,
            Set<Resource> resources) {
        Iterator<Resource> itr = resources.iterator();
        Resource res;
        int count = resources.size();
        while (itr.hasNext()) {
            res = itr.next();
            try {
                removeResourceFromCollection(colRes, res);
            } catch (SimulatorException e) {
                count--;
            }
        }
        return count;
    }

    public void removeResourcesFromDevice(Device dev, Set<Resource> resources) {
        Iterator<Resource> itr = resources.iterator();
        Resource res;
        while (itr.hasNext()) {
            res = itr.next();
            res.removeDeviceMembership(dev);
        }
        dev.removeChildResource(resources);
    }

    public void removeResourceFromCollection(CollectionResource parent,
            Resource child) throws SimulatorException {
        try {
            // 1. Remove the child from the parent
            parent.removeChildResource(child);

            // 2. Remove the reference to parent from child
            if (child instanceof SingleResource) {
                ((SingleResource) child).removeCollectionMembership(parent);
            } else {
                ((CollectionResource) child).removeMembership(parent);
            }
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
            throw e;
        }
    }

    public void removeResourceFromDevice(Device parent, Resource child) {
        // 1. Remove the reference to parent from child
        child.removeDeviceMembership(parent);

        // 2. Remove the child from the parent
        parent.removeChildResource(child);
    }

    public void removeSingleResources(Set<SingleResource> resources)
            throws SimulatorException {
        if (null == resources) {
            return;
        }
        Iterator<SingleResource> itr = resources.iterator();
        while (itr.hasNext()) {
            removeResource(itr.next());
        }
    }

    public void removeCollectionResources(Set<CollectionResource> resources)
            throws SimulatorException {
        if (null == resources) {
            return;
        }
        Iterator<CollectionResource> itr = resources.iterator();
        while (itr.hasNext()) {
            removeResource(itr.next());
        }
    }

    public void removeResource(Resource res) throws SimulatorException {
        // 1. Unregister the resource from the platform.
        SimulatorResource simRes = res.getSimulatorResource();
        try {
            simRes.stop();
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
            throw e;
        }

        Set<CollectionResource> collectionMembership;
        Set<Device> deviceMembership;

        if (res instanceof SingleResource) {
            collectionMembership = ((SingleResource) res)
                    .getCollectionMembership();
            deviceMembership = ((SingleResource) res).getDeviceMembership();
        } else {
            collectionMembership = ((CollectionResource) res).getMembership();
            deviceMembership = ((CollectionResource) res).getDeviceMembership();
        }

        // 2. Delete from the collections to which this resource is a member.
        if (null != collectionMembership && !collectionMembership.isEmpty()) {
            removeResourceFromCollection(collectionMembership, res);
        }

        // 3. Delete from the devices to which this resource is a member.
        if (null != deviceMembership && !deviceMembership.isEmpty()) {
            removeResourceFromDevice(deviceMembership, res);
        }

        // 4. Delete this resource
        data.deleteResource(res);
    }

    public void removeDevice(Device dev) {
        Set<Resource> childs = dev.getChildResources();
        if (null != childs && !childs.isEmpty()) {
            // 1. Remove the reference from all the children.
            Iterator<Resource> itr = childs.iterator();
            Resource res;
            while (itr.hasNext()) {
                res = itr.next();
                res.removeDeviceMembership(dev);
            }
        }
        // 2. Delete the device.
        data.deleteDevice(dev);
    }

    public boolean isUriUnique(List<MetaProperty> properties) {
        if (null == properties) {
            return false;
        }
        MetaProperty prop;
        Iterator<MetaProperty> itr = properties.iterator();
        while (itr.hasNext()) {
            prop = itr.next();
            if (prop.getPropName().equals(Constants.RESOURCE_URI)) {
                String uri = prop.getPropValue();
                return !data.isResourceExist(uri);
            }
        }
        return false;
    }

    public List<CollectionResource> getCollectionsForAddingToSingleResource(
            SingleResource resource) {
        List<CollectionResource> collectionResources = data
                .getCollectionResources();
        if (null == collectionResources || collectionResources.isEmpty()) {
            return null;
        }

        Set<CollectionResource> collectionMembership;
        collectionMembership = resource.getCollectionMembership();
        if (null == collectionMembership || collectionMembership.isEmpty()) {
            return collectionResources;
        }

        if (collectionMembership.size() == collectionResources.size()) {
            return null;
        }

        collectionResources.removeAll(collectionMembership);

        // Sort the list
        Collections.sort(collectionResources,
                Utility.collectionResourceComparator);

        return collectionResources;
    }

    public List<SingleResource> getSingleTypeResourcesForAddingToCollectionResource(
            CollectionResource colRes) {
        List<SingleResource> singleResources = data.getSingleResources();
        if (null == singleResources || singleResources.isEmpty()) {
            return null;
        }

        Set<SingleResource> childs;
        childs = colRes.getSingleTypeChildResources();
        if (null == childs || childs.isEmpty()) {
            return singleResources;
        }

        if (childs.size() == singleResources.size()) {
            return null;
        }

        singleResources.removeAll(childs);

        // Sort the list
        Collections.sort(singleResources, Utility.singleResourceComparator);

        return singleResources;
    }

    public List<SingleResource> getSingleTypeResourcesForAddingToDevice(
            Device dev) {
        List<SingleResource> singleResources = data.getSingleResources();
        if (null == singleResources || singleResources.isEmpty()) {
            return null;
        }

        Set<SingleResource> childs;
        childs = dev.getSingleTypeChildResources();
        if (null == childs || childs.isEmpty()) {
            return singleResources;
        }

        if (childs.size() == singleResources.size()) {
            return null;
        }

        singleResources.removeAll(childs);

        // Sort the list
        Collections.sort(singleResources, Utility.singleResourceComparator);

        return singleResources;
    }

    public List<CollectionResource> getCollectionTypeResourcesForAddingToCollectionResource(
            CollectionResource colRes) {
        List<CollectionResource> collectionResources = data
                .getCollectionResources();
        if (null == collectionResources || collectionResources.isEmpty()) {
            return null;
        }

        // Remove the colRes from the list
        collectionResources.remove(colRes);

        Set<CollectionResource> childs;
        childs = colRes.getCollectionTypeChildResources();
        if (null == childs || childs.isEmpty()) {
            return collectionResources;
        }

        if (childs.size() == collectionResources.size()) {
            return null;
        }

        collectionResources.removeAll(childs);

        // Sort the list
        Collections.sort(collectionResources,
                Utility.collectionResourceComparator);

        return collectionResources;
    }

    public List<CollectionResource> getCollectionTypeResourcesForAddingToDevice(
            Device dev) {
        List<CollectionResource> collectionResources = data
                .getCollectionResources();
        if (null == collectionResources || collectionResources.isEmpty()) {
            return null;
        }

        Set<CollectionResource> childs;
        childs = dev.getCollectionTypeChildResources();
        if (null == childs || childs.isEmpty()) {
            return collectionResources;
        }

        if (childs.size() == collectionResources.size()) {
            return null;
        }

        collectionResources.removeAll(childs);

        // Sort the list
        Collections.sort(collectionResources,
                Utility.collectionResourceComparator);

        return collectionResources;
    }

    public List<Device> getDevicesForAddingToResource(Resource resource) {
        List<Device> devices = data.getDevices();
        if (null == devices || devices.isEmpty()) {
            return null;
        }

        Set<Device> deviceMembership;
        if (resource instanceof SingleResource) {
            deviceMembership = ((SingleResource) resource)
                    .getDeviceMembership();
        } else {
            deviceMembership = ((CollectionResource) resource)
                    .getDeviceMembership();
        }
        if (null == deviceMembership || deviceMembership.isEmpty()) {
            return devices;
        }

        if (devices.size() == deviceMembership.size()) {
            return null;
        }

        devices.removeAll(deviceMembership);

        // Sort the list
        Collections.sort(devices, Utility.deviceComparator);

        return devices;
    }

    public List<CollectionResource> getResourceReferences(
            SingleResource resource) {
        List<CollectionResource> resources = Utility
                .getCollectionResourceListFromSet(resource
                        .getCollectionMembership());
        if (null == resources || resources.isEmpty()) {
            return null;
        }

        Collections.sort(resources, Utility.collectionResourceComparator);

        return resources;
    }

    public List<Device> getDeviceReferences(Resource resource) {
        Set<Device> deviceMembership;
        if (resource instanceof SingleResource) {
            deviceMembership = ((SingleResource) resource)
                    .getDeviceMembership();
        } else {
            deviceMembership = ((CollectionResource) resource)
                    .getDeviceMembership();
        }

        List<Device> devices = Utility.getDeviceListFromSet(deviceMembership);
        if (null == devices || devices.isEmpty()) {
            return null;
        }

        Collections.sort(devices, Utility.deviceComparator);

        return devices;
    }

    public List<SingleResource> getSingleTypeChilds(CollectionResource colRes) {
        Set<SingleResource> childs = colRes.getSingleTypeChildResources();
        return Utility.getSingleResourceListFromSet(childs);
    }

    public List<SingleResource> getSingleTypeChilds(Device dev) {
        Set<SingleResource> childs = dev.getSingleTypeChildResources();
        return Utility.getSingleResourceListFromSet(childs);
    }

    public List<CollectionResource> getCollectionTypeChilds(
            CollectionResource colRes) {
        Set<CollectionResource> childs = colRes
                .getCollectionTypeChildResources();
        return Utility.getCollectionResourceListFromSet(childs);
    }

    public List<CollectionResource> getCollectionTypeChilds(Device dev) {
        Set<CollectionResource> childs = dev.getCollectionTypeChildResources();
        return Utility.getCollectionResourceListFromSet(childs);
    }

    public void resourceSelectionChanged(final Resource selectedResource) {
        new Thread() {
            @Override
            public void run() {

                setCurrentDeviceInSelection(null);

                if (null != selectedResource) {
                    setCurrentResourceInSelection(selectedResource);
                } else {
                    setCurrentResourceInSelection(null);
                }
                // Notify all observers for resource selection change event
                UiListenerHandler.getInstance()
                        .resourceSelectionChangedUINotification(
                                selectedResource);
            }
        }.start();
    }

    public void deviceSelectionChanged(final Device selectedDevice) {
        new Thread() {
            @Override
            public void run() {

                setCurrentResourceInSelection(null);

                if (null != selectedDevice) {
                    setCurrentDeviceInSelection(selectedDevice);
                } else {
                    setCurrentDeviceInSelection(null);
                }
                // Notify all observers for resource selection change event
                UiListenerHandler.getInstance()
                        .deviceSelectionChangedUINotification(selectedDevice);
            }
        }.start();
    }

    public List<MetaProperty> getMetaProperties(Resource resource) {
        if (null != resource) {
            String propName;
            String propValue;

            List<MetaProperty> metaPropertyList = new ArrayList<MetaProperty>();

            for (int index = 0; index < Constants.META_PROPERTY_COUNT; index++) {
                propName = Constants.META_PROPERTIES[index];
                if (propName.equals(Constants.RESOURCE_NAME)) {
                    propValue = resource.getResourceName();
                } else if (propName.equals(Constants.RESOURCE_URI)) {
                    propValue = resource.getResourceURI();
                } else if (propName.equals(Constants.RESOURCE_TYPE)) {
                    propValue = resource.getResourceTypes().toString();
                } else {
                    propValue = null;
                }
                if (null != propValue) {
                    metaPropertyList.add(new MetaProperty(propName, propValue));
                }
            }
            return metaPropertyList;
        }
        return null;
    }

    public List<MetaProperty> getMetaProperties(Device dev) {
        if (null != dev) {
            List<MetaProperty> metaPropertyList = new ArrayList<MetaProperty>();
            metaPropertyList.add(new MetaProperty(Constants.DEVICE_NAME, dev
                    .getDeviceName()));
            return metaPropertyList;
        }
        return null;
    }

    public boolean startResource(Resource resource) throws SimulatorException {
        if (null == resource) {
            return false;
        }
        SimulatorResource server = resource.getSimulatorResource();
        if (null == server) {
            return false;
        }
        try {
            server.start();
            resource.setStarted(true);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(),
                            new Date(),
                            "There is an error while starting the resource.\n"
                                    + Utility.getSimulatorErrorString(e, null));
            throw e;
        }
        return true;
    }

    public boolean stopResource(Resource resource) throws SimulatorException {
        if (null == resource) {
            return false;
        }
        SimulatorResource server = resource.getSimulatorResource();
        if (null == server) {
            return false;
        }
        try {
            server.stop();
            resource.setStarted(false);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(),
                            new Date(),
                            "There is an error while stopping the resource.\n"
                                    + Utility.getSimulatorErrorString(e, null));
            throw e;
        }
        return true;
    }

    public boolean changeResourceName(Resource resource, String newName)
            throws SimulatorException {
        if (null == resource || null == newName) {
            return false;
        }

        if (!stopResource(resource)) {
            return false;
        }

        SimulatorResource server = resource.getSimulatorResource();
        try {
            server.setName(newName);
            resource.setResourceName(newName);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(),
                            new Date(),
                            "There is an error while changing the resource name.\n"
                                    + Utility.getSimulatorErrorString(e, null));
            throw e;
        }

        if (!startResource(resource)) {
            return false;
        }

        return true;
    }

    public boolean changeDeviceName(Device dev, String newName) {
        if (null == dev || null == newName) {
            return false;
        }
        data.changeDeviceName(dev, dev.getDeviceName(), newName);
        return true;
    }

    public boolean changeResourceURI(Resource resource, String newURI)
            throws SimulatorException {
        if (null == resource || null == newURI) {
            return false;
        }

        if (!stopResource(resource)) {
            return false;
        }

        String curURI = resource.getResourceURI();
        SimulatorResource server = resource.getSimulatorResource();
        try {
            server.setURI(newURI);
            data.changeResourceURI(resource, curURI, newURI);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(),
                            new Date(),
                            "There is an error while changing the resource URI.\n"
                                    + Utility.getSimulatorErrorString(e, null));
            throw e;
        }

        if (!startResource(resource)) {
            return false;
        }
        return true;
    }

    public boolean updateResourceProperties(Resource resource,
            List<MetaProperty> properties, boolean uriChanged,
            boolean nameChanged) throws SimulatorException {
        if (null == resource || null == properties) {
            return false;
        }

        // Updating the properties
        Iterator<MetaProperty> itr = properties.iterator();
        MetaProperty property;
        String propName;
        String propValue;
        String resName = null;
        String resURI = null;
        while (itr.hasNext()) {
            property = itr.next();
            if (null == property) {
                continue;
            }
            propName = property.getPropName();
            propValue = property.getPropValue();
            if (propName.equals(Constants.RESOURCE_NAME)) {
                resName = propValue;
            } else if (propName.equals(Constants.RESOURCE_URI)) {
                resURI = propValue;
            }
        }

        if (nameChanged) {
            if (!changeResourceName(resource, resName)) {
                return false;
            }

            // Notify UI Listeners
            UiListenerHandler.getInstance().propertiesChangedUINotification(
                    Resource.class);
        }

        if (uriChanged) {
            if (!changeResourceURI(resource, resURI)) {
                return false;
            }
        }

        return true;
    }

    public boolean updateDeviceProperties(Device dev,
            List<MetaProperty> properties) {
        if (null == dev || null == properties) {
            return false;
        }

        // Updating the properties
        Iterator<MetaProperty> itr = properties.iterator();
        MetaProperty property;
        String propName;
        String propValue;
        String devName = null;
        while (itr.hasNext()) {
            property = itr.next();
            if (null == property) {
                continue;
            }
            propName = property.getPropName();
            propValue = property.getPropValue();
            if (propName.equals(Constants.DEVICE_NAME)) {
                devName = propValue;
            }
        }

        if (!changeDeviceName(dev, devName)) {
            return false;
        }

        // Notify UI Listeners
        UiListenerHandler.getInstance().propertiesChangedUINotification(
                Device.class);

        return true;
    }

    public List<LocalResourceAttribute> getAttributes(Resource resource) {
        List<LocalResourceAttribute> attList = null;
        if (null != resource) {
            Map<String, LocalResourceAttribute> attMap = resource
                    .getResourceAttributes();
            if (null != attMap && attMap.size() > 0) {
                attList = new ArrayList<LocalResourceAttribute>();
                Set<String> attNameSet = attMap.keySet();
                String attName;
                LocalResourceAttribute attribute;
                Iterator<String> attNameItr = attNameSet.iterator();
                while (attNameItr.hasNext()) {
                    attName = attNameItr.next();
                    attribute = attMap.get(attName);
                    if (null != attribute) {
                        attList.add(attribute);
                    }
                }
            }
        }
        return attList;
    }

    public List<SimulatorResourceAttribute> getAttributes(
            SimulatorResourceModel model) {
        List<SimulatorResourceAttribute> attList = null;
        if (null != model) {
            Map<String, SimulatorResourceAttribute> attMap = model
                    .getAttributes();
            if (null != attMap && attMap.size() > 0) {
                attList = new ArrayList<SimulatorResourceAttribute>();
                Set<String> attNameSet = attMap.keySet();
                String attName;
                SimulatorResourceAttribute attribute;
                Iterator<String> attNameItr = attNameSet.iterator();
                while (attNameItr.hasNext()) {
                    attName = attNameItr.next();
                    attribute = attMap.get(attName);
                    if (null != attribute) {
                        attList.add(attribute);
                    }
                }
            }
        }
        return attList;
    }

    public List<SRMItem> getIndexedAttributes(SimulatorResourceModel[] model) {
        List<SRMItem> indexedAttList = null;
        if (null != model && model.length > 0) {
            indexedAttList = new ArrayList<SRMItem>();
            int i = 0;
            for (SimulatorResourceModel m : model) {
                indexedAttList.add(new SRMItem(i++, m));
            }
        }
        return indexedAttList;
    }

    public void attributeValueUpdated(SingleResource resource,
            String attributeName, AttributeValue value) {
        if (null != resource && null != attributeName && null != value) {
            SimulatorSingleResource simRes = (SimulatorSingleResource) resource
                    .getSimulatorResource();
            if (null != simRes) {
                try {
                    simRes.updateAttribute(attributeName, value);
                } catch (SimulatorException e) {
                    Activator
                            .getDefault()
                            .getLogManager()
                            .log(Level.ERROR.ordinal(), new Date(),
                                    Utility.getSimulatorErrorString(e, null));
                }
            }
        }
    }

    // TODO: This method should get the status from the native layer.
    public boolean isResourceStarted(Resource resource) {
        if (null == resource) {
            return false;
        }
        return resource.isStarted();
    }

    public boolean isPropertyValueInvalid(Resource resource,
            List<MetaProperty> properties, String propName) {
        if (null == resource || null == properties || null == propName) {
            return false;
        }
        boolean invalid = false;
        MetaProperty prop;
        Iterator<MetaProperty> itr = properties.iterator();
        while (itr.hasNext()) {
            prop = itr.next();
            if (prop.getPropName().equals(propName)) {
                String value = prop.getPropValue();
                if (null == value || value.trim().isEmpty()) {
                    invalid = true;
                }
            }
        }
        return invalid;
    }

    public boolean isPropertyValueInvalid(Device dev,
            List<MetaProperty> properties, String propName) {
        if (null == dev || null == properties || null == propName) {
            return false;
        }
        boolean invalid = false;
        MetaProperty prop;
        Iterator<MetaProperty> itr = properties.iterator();
        while (itr.hasNext()) {
            prop = itr.next();
            if (prop.getPropName().equals(propName)) {
                String value = prop.getPropValue();
                if (null == value || value.trim().isEmpty()) {
                    invalid = true;
                }
            }
        }
        return invalid;
    }

    public boolean isPropValueChanged(Resource resource,
            List<MetaProperty> properties, String propName) {
        if (null == resource || null == properties || null == propName) {
            return false;
        }
        boolean changed = false;
        MetaProperty prop;
        String oldValue;
        Iterator<MetaProperty> itr = properties.iterator();
        while (itr.hasNext()) {
            prop = itr.next();
            if (prop.getPropName().equals(propName)) {
                oldValue = getPropertyValueFromResource(resource, propName);
                if (null != oldValue && !prop.getPropValue().equals(oldValue)) {
                    changed = true;
                }
                break;
            }
        }
        return changed;
    }

    public boolean isPropValueChanged(Device dev,
            List<MetaProperty> properties, String propName) {
        if (null == dev || null == properties || null == propName) {
            return false;
        }
        boolean changed = false;
        MetaProperty prop;
        String oldValue;
        Iterator<MetaProperty> itr = properties.iterator();
        while (itr.hasNext()) {
            prop = itr.next();
            if (prop.getPropName().equals(propName)) {
                oldValue = dev.getDeviceName();
                if (null != oldValue && !prop.getPropValue().equals(oldValue)) {
                    changed = true;
                }
                break;
            }
        }
        return changed;
    }

    private String getPropertyValueFromResource(Resource resource,
            String propName) {
        if (null == resource || null == propName) {
            return null;
        }
        if (propName.equals(Constants.RESOURCE_URI)) {
            return resource.getResourceURI();
        } else if (propName.equals(Constants.RESOURCE_NAME)) {
            return resource.getResourceName();
        } else if (propName.equals(Constants.RESOURCE_TYPE)) {
            return resource.getResourceTypes().toString();
        } else {
            return null;
        }
    }

    public boolean isURIChanged(Resource resource, List<MetaProperty> properties) {
        if (null == resource || null == properties) {
            return false;
        }
        boolean changed = false;
        MetaProperty prop;
        Iterator<MetaProperty> itr = properties.iterator();
        while (itr.hasNext()) {
            prop = itr.next();
            if (prop.getPropName().equals(Constants.RESOURCE_URI)) {
                if (!prop.getPropValue().equals(resource.getResourceURI())) {
                    changed = true;
                }
                break;
            }
        }
        return changed;
    }

    public boolean startResource(SingleResource resource) {
        if (null == resource || resource.isStarted()) {
            return false;
        }
        boolean result;
        SimulatorResource server = resource.getSimulatorResource();
        if (null == server) {
            result = false;
        } else {
            try {
                server.start();
                resource.setStarted(true);
                result = true;
            } catch (SimulatorException e) {
                Activator
                        .getDefault()
                        .getLogManager()
                        .log(Level.ERROR.ordinal(), new Date(),
                                Utility.getSimulatorErrorString(e, null));
                result = false;
            }
        }
        return result;
    }

    public boolean stopResource(SingleResource resource) {
        if (null == resource || !resource.isStarted()) {
            return false;
        }
        boolean result;
        SimulatorResource server = resource.getSimulatorResource();
        if (null == server) {
            result = false;
        } else {
            try {
                server.stop();
                resource.setStarted(false);
                result = true;
            } catch (SimulatorException e) {
                Activator
                        .getDefault()
                        .getLogManager()
                        .log(Level.ERROR.ordinal(), new Date(),
                                Utility.getSimulatorErrorString(e, null));
                result = false;
            }
        }
        return result;
    }

    public boolean isAttHasRangeOrAllowedValues(SimulatorResourceAttribute att) {
        if (null == att) {
            return false;
        }
        AttributeProperty prop = att.property();
        if (null == prop) {
            return false;
        }
        Type attProp = prop.type();
        if (attProp == Type.UNKNOWN) {
            return false;
        }
        return true;
    }

    public int startAutomation(SingleResource resource,
            AttributeElement attribute, AutoUpdateType autoType,
            int autoUpdateInterval) {
        int autoId = -1;
        if (null != resource && null != attribute) {
            SimulatorSingleResource server = (SimulatorSingleResource) resource
                    .getSimulatorResource();
            if (null != server) {
                String attrName = attribute.getSimulatorResourceAttribute()
                        .name();
                try {
                    autoId = server.startAttributeUpdation(attrName, autoType,
                            autoUpdateInterval, automationListener);
                } catch (SimulatorException e) {
                    Activator
                            .getDefault()
                            .getLogManager()
                            .log(Level.ERROR.ordinal(),
                                    new Date(),
                                    "[" + e.getClass().getSimpleName() + "]"
                                            + e.code().toString() + "-"
                                            + e.message());
                    return -1;
                }
                if (-1 != autoId) {
                    attribute.setAutoUpdateId(autoId);
                    attribute.setAutoUpdateType(autoType);
                    attribute.setAutoUpdateInterval(autoUpdateInterval);
                    attribute.setAutoUpdateState(true);
                    resource.setAttributeAutomationInProgress(true);
                }
            }
        }
        return autoId;
    }

    public void stopAutomation(SingleResource resource, AttributeElement att,
            int autoId) {
        if (null != resource) {
            SimulatorSingleResource server = (SimulatorSingleResource) resource
                    .getSimulatorResource();
            if (null != server) {
                try {
                    server.stopUpdation(autoId);
                } catch (SimulatorException e) {
                    Activator
                            .getDefault()
                            .getLogManager()
                            .log(Level.ERROR.ordinal(),
                                    new Date(),
                                    "[" + e.getClass().getSimpleName() + "]"
                                            + e.code().toString() + "-"
                                            + e.message());
                    return;
                }
                // Change the automation status
                att.setAutoUpdateState(false);
                resource.setAttributeAutomationInProgress(isAnyAttributeInAutomation(resource));
            }
        }
    }

    public boolean startResourceAutomationUIRequest(AutoUpdateType autoType,
            int autoUpdateInterval, final SingleResource resource) {
        if (null == resource) {
            return false;
        }
        boolean status = false;
        changeResourceLevelAutomationStatus(resource, true);
        // Invoke the native automation method
        SimulatorSingleResource resourceServer = (SimulatorSingleResource) resource
                .getSimulatorResource();
        if (null != resourceServer) {
            int autoId = -1;
            try {
                autoId = resourceServer.startResourceUpdation(autoType,
                        autoUpdateInterval, automationListener);
            } catch (SimulatorException e) {
                Activator
                        .getDefault()
                        .getLogManager()
                        .log(Level.ERROR.ordinal(), new Date(),
                                Utility.getSimulatorErrorString(e, null));
                autoId = -1;
            }
            if (-1 == autoId) {
                // Automation request failed and hence status is being
                // rolled back
                changeResourceLevelAutomationStatus(resource, false);
            } else {
                // Automation request accepted.
                resource.setAutomationId(autoId);

                // Notify the UI listeners in a different thread.
                Thread notifyThread = new Thread() {
                    public void run() {
                        UiListenerHandler.getInstance()
                                .resourceAutomationStartedUINotification(
                                        resource);
                    };
                };
                notifyThread.setPriority(Thread.MAX_PRIORITY);
                notifyThread.start();

                status = true;
            }
        }
        return status;
    }

    public boolean stopResourceAutomationUIRequest(final SingleResource resource) {
        if (null == resource) {
            return false;
        }
        final int autoId = resource.getAutomationId();
        if (-1 == autoId) {
            return false;
        }
        SimulatorSingleResource resourceServer = (SimulatorSingleResource) resource
                .getSimulatorResource();
        if (null == resourceServer) {
            return false;
        }
        // Call native method
        try {
            resourceServer.stopUpdation(autoId);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
            return false;
        }

        // Notify the UI Listeners. Invoke the automation complete callback.
        Thread stopThread = new Thread() {
            public void run() {
                automationListener.onUpdateComplete(resource.getResourceURI(),
                        autoId);
            }
        };
        stopThread.start();
        return true;
    }

    private boolean isAnyAttributeInAutomation(SingleResource resource) {
        if (null == resource || null == resource.getResourceRepresentation()) {
            return false;
        }

        Map<String, AttributeElement> attributes = resource
                .getResourceRepresentation().getAttributes();
        if (null == attributes || 0 == attributes.size())
            return false;

        for (Map.Entry<String, AttributeElement> entry : attributes.entrySet()) {
            if (entry.getValue().isAutoUpdateInProgress())
                return true;
        }

        return false;
    }

    // Changes the automation state of the resource and its attributes
    private void changeResourceLevelAutomationStatus(SingleResource resource,
            boolean status) {

        if (null == resource || null == resource.getResourceRepresentation()) {
            return;
        }

        Map<String, AttributeElement> attributes = resource
                .getResourceRepresentation().getAttributes();
        if (null == attributes || 0 == attributes.size())
            return;

        for (Map.Entry<String, AttributeElement> entry : attributes.entrySet()) {
            entry.getValue().setAutoUpdateState(status);
        }

        resource.setResourceAutomationInProgress(status);
    }

    private AttributeElement getAttributeWithGivenAutomationId(
            SingleResource resource, int automationId) {
        if (null == resource || null == resource.getResourceRepresentation()) {
            return null;
        }

        Map<String, AttributeElement> attributes = resource
                .getResourceRepresentation().getAttributes();
        if (null == attributes || 0 == attributes.size())
            return null;

        for (Map.Entry<String, AttributeElement> entry : attributes.entrySet()) {
            if (automationId == entry.getValue().getAutoUpdateId())
                return entry.getValue();
        }

        return null;
    }

    public boolean isResourceAutomationStarted(SingleResource resource) {
        boolean status = false;
        if (null != resource) {
            status = resource.isResourceAutomationInProgress();
        }
        return status;
    }

    public boolean isAttributeAutomationStarted(SingleResource resource) {
        if (null == resource) {
            return false;
        }
        return resource.isAttributeAutomationInProgress();
    }

    public LocalResourceAttribute getAttributeByResourceURI(
            SingleResource resource, String attName) {
        if (null == resource || null == attName) {
            return null;
        }
        Map<String, LocalResourceAttribute> attMap = resource
                .getResourceAttributes();
        if (null == attMap) {
            return null;
        }
        return attMap.get(attName);
    }

    public void notifyObserverRequest(Resource resource, int observerId) {
        if (null == resource) {
            return;
        }
        SimulatorResource simulatorResource = resource.getSimulatorResource();
        if (null == simulatorResource) {
            return;
        }
        try {
            simulatorResource.notifyObserver(observerId);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
        }
    }

    public void shutdown() {
        threadHandle.interrupt();
    }

    public List<String> getAllValuesOfAttribute(SimulatorResourceAttribute att) {
        if (null == att) {
            return null;
        }

        AttributeValue val = att.value();
        if (null == val) {
            return null;
        }

        TypeInfo type = val.typeInfo();

        AttributeProperty prop = att.property();
        if (null == prop) {
            return null;
        }

        List<String> values = new ArrayList<String>();

        Type valuesType = prop.type();

        if (valuesType == Type.UNKNOWN) {
            // Adding the default value
            values.add(Utility.getAttributeValueAsString(val));
            return values;
        }

        if (type.mType != ValueType.RESOURCEMODEL) {
            if (type.mType == ValueType.ARRAY) {
                if (type.mDepth == 1) {
                    AttributeProperty childProp = prop.getChildProperty();
                    if (null != childProp) {
                        valuesType = childProp.type();
                        if (valuesType == Type.RANGE) {
                            List<String> list = getRangeForPrimitiveNonArrayAttributes(
                                    childProp, type.mBaseType);
                            if (null != list) {
                                values.addAll(list);
                            }
                        } else if (valuesType == Type.VALUESET) {
                            List<String> list = getAllowedValuesForPrimitiveNonArrayAttributes(
                                    childProp.valueSet(), type.mBaseType);
                            if (null != list) {
                                values.addAll(list);
                            }
                        }
                    }
                }
            } else {
                if (valuesType == Type.RANGE) {
                    List<String> list = getRangeForPrimitiveNonArrayAttributes(
                            prop, type.mType);
                    if (null != list) {
                        values.addAll(list);
                    }
                } else if (valuesType == Type.VALUESET) {
                    List<String> list = getAllowedValuesForPrimitiveNonArrayAttributes(
                            prop.valueSet(), type.mType);
                    if (null != list) {
                        values.addAll(list);
                    }
                }
            }
        }

        return values;
    }

    public List<String> getRangeForPrimitiveNonArrayAttributes(
            AttributeProperty prop, ValueType type) {
        if (null == prop) {
            return null;
        }

        if (type == ValueType.ARRAY || type == ValueType.RESOURCEMODEL) {
            return null;
        }

        List<String> values = new ArrayList<String>();
        switch (type) {
            case INTEGER:
                int min = (int) prop.min();
                int max = (int) prop.max();
                for (int iVal = min; iVal <= max; iVal++) {
                    values.add(String.valueOf(iVal));
                }
                break;
            case DOUBLE:
                double minD = (double) prop.min();
                double maxD = (double) prop.max();
                for (double iVal = minD; iVal <= maxD; iVal = iVal + 1.0) {
                    values.add(String.valueOf(iVal));
                }
                break;
            default:
        }
        return values;
    }

    public List<String> getAllowedValuesForPrimitiveNonArrayAttributes(
            AttributeValue[] attValues, ValueType type) {
        if (null == attValues || attValues.length < 1) {
            return null;
        }

        if (type == ValueType.ARRAY || type == ValueType.RESOURCEMODEL) {
            return null;
        }

        Object obj;
        List<String> values = new ArrayList<String>();
        for (AttributeValue val : attValues) {
            if (null == val) {
                continue;
            }
            obj = val.get();
            if (null == obj) {
                continue;
            }
            values.add(String.valueOf(obj));
        }
        return values;
    }
}