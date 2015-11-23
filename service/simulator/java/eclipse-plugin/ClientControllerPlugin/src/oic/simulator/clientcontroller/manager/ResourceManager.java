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

package oic.simulator.clientcontroller.manager;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Vector;

import oic.simulator.clientcontroller.Activator;
import oic.simulator.clientcontroller.listener.IConfigurationUpload;
import oic.simulator.clientcontroller.listener.IDevicePlatformInfoUIListener;
import oic.simulator.clientcontroller.listener.IFindResourceUIListener;
import oic.simulator.clientcontroller.listener.IGetUIListener;
import oic.simulator.clientcontroller.listener.IObserveUIListener;
import oic.simulator.clientcontroller.listener.IPostUIListener;
import oic.simulator.clientcontroller.listener.IPutUIListener;
import oic.simulator.clientcontroller.listener.IResourceSelectionChangedUIListener;
import oic.simulator.clientcontroller.listener.IVerificationUIListener;
import oic.simulator.clientcontroller.remoteresource.DeviceAndPlatformInfo;
import oic.simulator.clientcontroller.remoteresource.MetaProperty;
import oic.simulator.clientcontroller.remoteresource.RemoteResource;
import oic.simulator.clientcontroller.utils.Constants;
import oic.simulator.clientcontroller.utils.Utility;

import org.oic.simulator.AttributeProperty;
import org.oic.simulator.AttributeProperty.Type;
import org.oic.simulator.AttributeValue;
import org.oic.simulator.AttributeValue.TypeInfo;
import org.oic.simulator.AttributeValue.ValueType;
import org.oic.simulator.DeviceInfo;
import org.oic.simulator.DeviceListener;
import org.oic.simulator.ILogger.Level;
import org.oic.simulator.PlatformInfo;
import org.oic.simulator.PlatformListener;
import org.oic.simulator.SimulatorException;
import org.oic.simulator.SimulatorManager;
import org.oic.simulator.SimulatorResourceAttribute;
import org.oic.simulator.SimulatorResourceModel;
import org.oic.simulator.SimulatorResult;
import org.oic.simulator.client.FindResourceListener;
import org.oic.simulator.client.SimulatorRemoteResource;
import org.oic.simulator.client.SimulatorRemoteResource.GetResponseListener;
import org.oic.simulator.client.SimulatorRemoteResource.ObserveNotificationListener;
import org.oic.simulator.client.SimulatorRemoteResource.PostResponseListener;
import org.oic.simulator.client.SimulatorRemoteResource.PutResponseListener;
import org.oic.simulator.client.SimulatorRemoteResource.VerificationListener;
import org.oic.simulator.client.SimulatorRemoteResource.VerificationType;

/**
 * This class acts as an interface between the simulator java SDK and the
 * various UI modules. It maintains all the details of resources and provides
 * other UI modules with the information required. It also handles responses for
 * find, GET, PUT, POST, Observe and automatic verification operations from
 * native layer and propagates those events to the registered UI listeners.
 */
public class ResourceManager {

    private Set<String>                               lastKnownSearchTypes;

    private RemoteResource                            currentResourceInSelection;

    private FindResourceListener                      findResourceListener;
    private GetResponseListener                       getListener;
    private PutResponseListener                       putListener;
    private PostResponseListener                      postListener;
    private ObserveNotificationListener               observeListener;
    private VerificationListener                      verifyListener;
    private DeviceListener                            deviceListener;
    private PlatformListener                          platformListener;

    private ResponseSynchronizerThread                synchronizerThread;

    private Thread                                    threadHandle;

    private List<IFindResourceUIListener>             findResourceUIListeners;
    private List<IResourceSelectionChangedUIListener> resourceSelectionChangedUIListeners;
    private List<IGetUIListener>                      getUIListeners;
    private List<IPutUIListener>                      putUIListeners;
    private List<IPostUIListener>                     postUIListeners;
    private List<IObserveUIListener>                  observeUIListeners;
    private List<IVerificationUIListener>             verificationUIListeners;
    private List<IConfigurationUpload>                configUploadUIListeners;
    private List<IDevicePlatformInfoUIListener>       devicePlatformInfoUIListeners;

    // Map with Server ID as key and the complete object as the value
    private Map<String, RemoteResource>               resourceMap;
    private List<RemoteResource>                      favoriteResources;
    // Maintaining a list of resource URIs for favorite resources feature.
    private List<String>                              favoriteURIList;

    // Maintaining a list of observed resource URIs.
    private List<String>                              observedResourceURIList;

    private Map<String, DeviceAndPlatformInfo>        hostDeviceAndPlatformMap;

    public ResourceManager() {
        resourceMap = new HashMap<String, RemoteResource>();
        favoriteResources = new ArrayList<RemoteResource>();
        favoriteURIList = new ArrayList<String>();
        observedResourceURIList = new ArrayList<String>();
        hostDeviceAndPlatformMap = new HashMap<String, DeviceAndPlatformInfo>();

        findResourceUIListeners = new ArrayList<IFindResourceUIListener>();
        resourceSelectionChangedUIListeners = new ArrayList<IResourceSelectionChangedUIListener>();
        getUIListeners = new ArrayList<IGetUIListener>();
        putUIListeners = new ArrayList<IPutUIListener>();
        postUIListeners = new ArrayList<IPostUIListener>();
        observeUIListeners = new ArrayList<IObserveUIListener>();
        verificationUIListeners = new ArrayList<IVerificationUIListener>();
        configUploadUIListeners = new ArrayList<IConfigurationUpload>();
        devicePlatformInfoUIListeners = new ArrayList<IDevicePlatformInfoUIListener>();

        findResourceListener = new FindResourceListener() {

            @Override
            public void onResourceFound(final SimulatorRemoteResource resourceN) {
                synchronizerThread.addToQueue(new Runnable() {
                    @Override
                    public void run() {
                        if (null == resourceN) {
                            return;
                        }

                        // If id is not available, then it cannot be added to
                        // the local map as null value should not be allowed as
                        // key.
                        String uid = resourceN.getId();
                        if (null == uid) {
                            return;
                        }

                        // If resource already exist, then ignore it.
                        boolean exist = isUidExist(uid);
                        if (exist) {
                            return;
                        }

                        // Fetch the resource data
                        // RemoteResource resource =
                        // fetchResourceDetails(resourceN);
                        RemoteResource resource = new RemoteResource();
                        resource.setRemoteResourceRef(resourceN);

                        String uri = resourceN.getURI();
                        if (null != uri && uri.trim().length() > 0) {
                            // Add resource to favorite list if it was in
                            // favorites list during find/refresh operation.
                            if (favoriteURIList.contains(uri)) {
                                addResourcetoFavorites(resource);
                            }
                            // Add resource to observed resources list if it was
                            // in observe list during find/refresh operation.
                            if (observedResourceURIList.contains(uri)) {
                                sendObserveRequest(resource);
                            }
                        } else {
                            Activator
                                    .getDefault()
                                    .getLogManager()
                                    .log(Level.INFO.ordinal(), new Date(),
                                            "Found a resource without URI. Ignoring it.");
                            return;
                        }

                        // Add the resource in local data structure
                        addResourceDetails(resource);

                        // Notify the UI listener
                        newResourceFoundNotification(resource);

                        Activator
                                .getDefault()
                                .getLogManager()
                                .log(Level.INFO.ordinal(),
                                        new Date(),
                                        "Resource Found [" + resourceN.getURI()
                                                + "].");

                        // Send an initial GET request to get the resource
                        // attributes
                        try {
                            resourceN.get(null, getListener);
                        } catch (SimulatorException e) {
                            Activator
                                    .getDefault()
                                    .getLogManager()
                                    .log(Level.ERROR.ordinal(),
                                            new Date(),
                                            Utility.getSimulatorErrorString(e,
                                                    null));
                        }

                        // Get the device information
                        if (!isDeviceInfoExist(resourceN.getHost())) {
                            try {
                                SimulatorManager.findDevices(resource
                                        .getRemoteResourceRef().getHost(),
                                        deviceListener);
                            } catch (SimulatorException e) {
                                Activator
                                        .getDefault()
                                        .getLogManager()
                                        .log(Level.ERROR.ordinal(),
                                                new Date(),
                                                Utility.getSimulatorErrorString(
                                                        e, null));
                            }
                        }

                        // Get the platform information
                        if (!isPlatformInfoExist(resourceN.getHost())) {
                            try {
                                SimulatorManager.getPlatformInformation(
                                        resource.getRemoteResourceRef()
                                                .getHost(), platformListener);
                            } catch (SimulatorException e) {
                                Activator
                                        .getDefault()
                                        .getLogManager()
                                        .log(Level.ERROR.ordinal(),
                                                new Date(),
                                                Utility.getSimulatorErrorString(
                                                        e, null));
                            }
                        }
                    }
                });
            }
        };

        // TODO: Listeners for device and platform information.
        deviceListener = new DeviceListener() {

            @Override
            public void onDeviceFound(final String host,
                    final DeviceInfo deviceInfo) {
                if (null == deviceInfo || null == host) {
                    return;
                }
                synchronizerThread.addToQueue(new Runnable() {
                    @Override
                    public void run() {
                        synchronized (hostDeviceAndPlatformMap) {
                            DeviceAndPlatformInfo info = hostDeviceAndPlatformMap
                                    .get(host);
                            if (null == info) {
                                info = new DeviceAndPlatformInfo();
                                info.setHost(host);
                                hostDeviceAndPlatformMap.put(host, info);
                            }
                            info.setDeviceInfo(deviceInfo);
                        }

                        // Notify UI listeners
                        deviceInfoReceivedNotification();
                    }
                });
            }
        };

        platformListener = new PlatformListener() {

            @Override
            public void onPlatformFound(final String host,
                    final PlatformInfo platformInfo) {
                if (null == platformInfo || null == host) {
                    return;
                }
                synchronizerThread.addToQueue(new Runnable() {
                    @Override
                    public void run() {
                        synchronized (hostDeviceAndPlatformMap) {
                            DeviceAndPlatformInfo info = hostDeviceAndPlatformMap
                                    .get(host);
                            if (null == info) {
                                info = new DeviceAndPlatformInfo();
                                info.setHost(host);
                                hostDeviceAndPlatformMap.put(host, info);
                            }
                            info.setPlatformInfo(platformInfo);
                        }

                        // Notify UI listeners
                        platformInfoReceivedNotification();
                    }
                });
            }
        };

        getListener = new GetResponseListener() {
            @Override
            public void onGetResponse(final String uid,
                    final SimulatorResult result,
                    final SimulatorResourceModel resourceModelN) {
                System.out.println(result);
                if (result != SimulatorResult.SIMULATOR_OK) {
                    Activator
                            .getDefault()
                            .getLogManager()
                            .log(Level.ERROR.ordinal(),
                                    new Date(),
                                    "["
                                            + result.toString()
                                            + "] Received error response for GET request.");
                    return;
                }
                synchronizerThread.addToQueue(new Runnable() {
                    @Override
                    public void run() {
                        // Handling the response which includes retrieving the
                        // attributes and updating the local model.
                        RemoteResource resource = handleResponse(uid,
                                resourceModelN);
                        if (null != resource) {
                            // Notify the UI listeners
                            getCompleteNotification(resource);
                        }
                    }
                });
            }
        };

        putListener = new PutResponseListener() {

            @Override
            public void onPutResponse(final String uid,
                    final SimulatorResult result,
                    final SimulatorResourceModel resourceModelN) {
                if (result != SimulatorResult.SIMULATOR_OK) {
                    Activator
                            .getDefault()
                            .getLogManager()
                            .log(Level.ERROR.ordinal(),
                                    new Date(),
                                    "["
                                            + result.toString()
                                            + "] Received error response for PUT request.");
                    return;
                }
                synchronizerThread.addToQueue(new Thread() {
                    @Override
                    public void run() {
                        // Handling the response which includes retrieving the
                        // attributes and updating the local model.
                        RemoteResource resource = handleResponse(uid,
                                resourceModelN);
                        if (null != resource) {
                            // Notify the UI listeners
                            putCompleteNotification(resource);
                        }
                    }
                });
            }
        };

        postListener = new PostResponseListener() {
            @Override
            public void onPostResponse(final String uid,
                    final SimulatorResult result,
                    final SimulatorResourceModel resourceModelN) {
                if (result != SimulatorResult.SIMULATOR_OK) {
                    Activator
                            .getDefault()
                            .getLogManager()
                            .log(Level.ERROR.ordinal(),
                                    new Date(),
                                    "["
                                            + result.toString()
                                            + "] Received error response for POST request.");
                    return;
                }
                synchronizerThread.addToQueue(new Runnable() {
                    @Override
                    public void run() {
                        // Handling the response which includes retrieving the
                        // attributes and updating the local model.
                        RemoteResource resource = handleResponse(uid,
                                resourceModelN);
                        if (null != resource) {
                            // Notify the UI listeners
                            postCompleteNotification(resource);
                        }
                    }
                });
            }
        };

        observeListener = new ObserveNotificationListener() {

            @Override
            public void onObserveNotification(final String uid,
                    final SimulatorResourceModel resourceModelN, final int seq) {
                synchronizerThread.addToQueue(new Runnable() {
                    @Override
                    public void run() {
                        // Handling the response which includes retrieving the
                        // attributes and updating the local model.
                        RemoteResource resource = handleResponse(uid,
                                resourceModelN);
                        if (null != resource) {
                            // Notify the UI listeners
                            observeCompleteNotification(resource);
                        }
                    }
                });
            }
        };

        verifyListener = new VerificationListener() {

            @Override
            public void onVerificationStarted(final String uid, final int autoId) {
                synchronizerThread.addToQueue(new Runnable() {
                    @Override
                    public void run() {
                        RemoteResource resource = getResource(uid);
                        if (null == resource) {
                            return;
                        }
                        // Update the automation status.
                        resource.updateAutomationStatus(autoId, true);

                        int autoType = resource.getAutomationtype(autoId);

                        // Notify the listeners.
                        verificationStartedNotification(resource, autoType);
                    }
                });
            }

            @Override
            public void onVerificationCompleted(final String uid,
                    final int autoId) {
                synchronizerThread.addToQueue(new Runnable() {
                    @Override
                    public void run() {
                        RemoteResource resource = getResource(uid);
                        if (null == resource) {
                            return;
                        }
                        // Update the automation status.
                        resource.updateAutomationStatus(autoId, false);

                        int autoType = resource.getAutomationtype(autoId);

                        // Notify the listeners.
                        verificationCompletedNotification(resource, autoType);
                    }
                });
            }

            @Override
            public void onVerificationAborted(final String uid, final int autoId) {
                synchronizerThread.addToQueue(new Runnable() {
                    @Override
                    public void run() {
                        RemoteResource resource = getResource(uid);
                        if (null == resource) {
                            return;
                        }
                        // Update the automation status.
                        resource.updateAutomationStatus(autoId, false);

                        int autoType = resource.getAutomationtype(autoId);

                        // Notify the listeners.
                        verificationAbortedNotification(resource, autoType);
                    }
                });
            }
        };

        synchronizerThread = new ResponseSynchronizerThread();
        threadHandle = new Thread(synchronizerThread);
        threadHandle.setName("Simulator Client Controller Event Queue");
        threadHandle.start();
    }

    private RemoteResource handleResponse(String uid,
            SimulatorResourceModel resourceModelN) {
        if (null == uid || null == resourceModelN) {
            return null;
        }

        // Update the local model
        RemoteResource resource;
        resource = getResource(uid);
        if (null == resource) {
            return null;
        }

        // if(!resource.isConfigUploaded() || null ==
        // resource.getResourceModelRef())
        SimulatorResourceModel resourceModel = resource.getResourceModelRef();
        if (null == resourceModel) {
            resource.setResourceModelRef(resourceModelN);
        } else {
            resourceModel.update(resourceModelN);
        }

        resource.setResourceRepresentation(resourceModelN, false);

        return resource;
    }

    public synchronized boolean isDeviceInfoExist(String host) {
        DeviceAndPlatformInfo info = hostDeviceAndPlatformMap.get(host);
        if (null == info) {
            return false;
        }
        if (null == info.getDeviceInfo()) {
            return false;
        }
        return true;
    }

    public synchronized boolean isPlatformInfoExist(String host) {
        DeviceAndPlatformInfo info = hostDeviceAndPlatformMap.get(host);
        if (null == info) {
            return false;
        }
        if (null == info.getPlatformInfo()) {
            return false;
        }
        return true;
    }

    private static class ResponseSynchronizerThread implements Runnable {

        LinkedList<Runnable> responseQueue = new LinkedList<Runnable>();

        @Override
        public void run() {
            while (!Thread.interrupted()) {
                synchronized (this) {
                    try {
                        while (responseQueue.isEmpty()) {
                            this.wait();
                            break;
                        }
                    } catch (InterruptedException e) {
                        return;
                    }
                }

                Runnable thread;
                synchronized (this) {
                    thread = responseQueue.pop();
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
                responseQueue.add(event);
                this.notify();
            }
        }
    }

    public void addResourceSelectionChangedUIListener(
            IResourceSelectionChangedUIListener resourceSelectionChangedUIListener) {
        synchronized (resourceSelectionChangedUIListeners) {
            resourceSelectionChangedUIListeners
                    .add(resourceSelectionChangedUIListener);
        }
    }

    public void addGetUIListener(IGetUIListener getUIListener) {
        synchronized (getUIListeners) {
            getUIListeners.add(getUIListener);
        }
    }

    public void addPutUIListener(IPutUIListener putUIListener) {
        synchronized (putUIListeners) {
            putUIListeners.add(putUIListener);
        }
    }

    public void addPostUIListener(IPostUIListener postUIListener) {
        synchronized (postUIListeners) {
            postUIListeners.add(postUIListener);
        }
    }

    public void addObserveUIListener(IObserveUIListener observeUIListener) {
        synchronized (observeUIListeners) {
            observeUIListeners.add(observeUIListener);
        }
    }

    public void addVerificationUIListener(
            IVerificationUIListener verificationUIListener) {
        synchronized (verificationUIListeners) {
            verificationUIListeners.add(verificationUIListener);
        }
    }

    public void addConfigUploadUIListener(IConfigurationUpload configListener) {
        synchronized (configUploadUIListeners) {
            configUploadUIListeners.add(configListener);
        }
    }

    public void addDevicePlatformInfoUIListener(
            IDevicePlatformInfoUIListener deviceUIListener) {
        synchronized (devicePlatformInfoUIListeners) {
            devicePlatformInfoUIListeners.add(deviceUIListener);
        }
    }

    public void removeDevicePlatformInfoUIListener(
            IDevicePlatformInfoUIListener platformUIListener) {
        synchronized (devicePlatformInfoUIListeners) {
            devicePlatformInfoUIListeners.remove(platformUIListener);
        }
    }

    public void removeResourceSelectionChangedUIListener(
            IResourceSelectionChangedUIListener listener) {
        synchronized (resourceSelectionChangedUIListeners) {
            if (null != listener
                    && resourceSelectionChangedUIListeners.size() > 0) {
                resourceSelectionChangedUIListeners.remove(listener);
            }
        }
    }

    public void removeGetUIListener(IGetUIListener getUIListener) {
        synchronized (getUIListeners) {
            getUIListeners.remove(getUIListener);
        }
    }

    public void removePutUIListener(IPutUIListener putUIListener) {
        synchronized (putUIListeners) {
            putUIListeners.remove(putUIListener);
        }
    }

    public void removePostUIListener(IPostUIListener postUIListener) {
        synchronized (postUIListeners) {
            postUIListeners.remove(postUIListener);
        }
    }

    public void removeObserveUIListener(IObserveUIListener observeUIListener) {
        synchronized (observeUIListeners) {
            observeUIListeners.remove(observeUIListener);
        }
    }

    public void removeVerificationUIListener(
            IVerificationUIListener verificationUIListener) {
        synchronized (verificationUIListeners) {
            verificationUIListeners.remove(verificationUIListener);
        }
    }

    public void removeConfigUploadUIListener(IConfigurationUpload configListener) {
        synchronized (configUploadUIListeners) {
            configUploadUIListeners.remove(configListener);
        }
    }

    public void addResourcetoFavorites(RemoteResource resource) {
        if (null == resource) {
            return;
        }
        resource.setFavorite(true);
        synchronized (favoriteResources) {
            favoriteResources.add(resource);
        }
    }

    public void removeResourceFromFavorites(RemoteResource resource) {
        if (null == resource) {
            return;
        }
        resource.setFavorite(false);
        synchronized (favoriteResources) {
            favoriteResources.remove(resource);
        }
    }

    public void addResourceURItoFavorites(RemoteResource resource) {
        if (null == resource) {
            return;
        }
        synchronized (favoriteURIList) {
            favoriteURIList.add(resource.getRemoteResourceRef().getURI());
        }
    }

    public void removeResourceURIFromFavorites(RemoteResource resource) {
        if (null == resource) {
            return;
        }
        synchronized (favoriteURIList) {
            favoriteURIList.remove(resource.getRemoteResourceRef().getURI());
        }
    }

    public void addObservedResourceURI(String resourceURI) {
        synchronized (observedResourceURIList) {
            if (!observedResourceURIList.contains(resourceURI))
                observedResourceURIList.add(resourceURI);
        }
    }

    public void removeObservedResourceURI(String resourceURI) {
        synchronized (observedResourceURIList) {
            observedResourceURIList.remove(resourceURI);
        }
    }

    public boolean isResourceObserved(String resourceURI) {
        boolean observed = false;
        synchronized (observedResourceURIList) {
            observed = observedResourceURIList.contains(resourceURI);
        }
        return observed;
    }

    public synchronized RemoteResource getCurrentResourceInSelection() {
        return currentResourceInSelection;
    }

    public synchronized void setCurrentResourceInSelection(
            RemoteResource resource) {
        this.currentResourceInSelection = resource;
    }

    private void addResourceDetails(RemoteResource remoteResource) {
        if (null != remoteResource) {
            synchronized (resourceMap) {
                resourceMap.put(remoteResource.getRemoteResourceRef().getId(),
                        remoteResource);
            }
        }
    }

    public void addFindresourceUIListener(IFindResourceUIListener listener) {
        if (null == listener) {
            return;
        }
        synchronized (findResourceUIListeners) {
            findResourceUIListeners.add(listener);
        }
    }

    public void removeFindresourceUIListener(IFindResourceUIListener listener) {
        if (null == listener) {
            return;
        }
        synchronized (findResourceUIListeners) {
            findResourceUIListeners.remove(listener);
        }
    }

    private boolean isUidExist(String uid) {
        boolean exist;
        synchronized (resourceMap) {
            exist = resourceMap.containsKey(uid);
        }
        return exist;
    }

    private RemoteResource getResource(String uid) {
        if (null == uid) {
            return null;
        }
        RemoteResource resource;
        synchronized (resourceMap) {
            resource = resourceMap.get(uid);
        }
        return resource;
    }

    private void newResourceFoundNotification(RemoteResource resource) {
        synchronized (findResourceUIListeners) {
            if (findResourceUIListeners.size() > 0) {
                IFindResourceUIListener listener;
                Iterator<IFindResourceUIListener> listenerItr = findResourceUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onNewResourceFound(resource);
                    }
                }
            }
        }
    }

    private void resourceSelectionChangedUINotification(RemoteResource resource) {
        synchronized (resourceSelectionChangedUIListeners) {
            if (resourceSelectionChangedUIListeners.size() > 0) {
                IResourceSelectionChangedUIListener listener;
                Iterator<IResourceSelectionChangedUIListener> listenerItr = resourceSelectionChangedUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onResourceSelectionChange(resource);
                    }
                }
            }
        }
    }

    private void getCompleteNotification(RemoteResource resource) {
        synchronized (getUIListeners) {
            if (getUIListeners.size() > 0) {
                IGetUIListener listener;
                Iterator<IGetUIListener> listenerItr = getUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onGetCompleted(resource);
                    }
                }
            }
        }
    }

    private void putCompleteNotification(RemoteResource resource) {
        synchronized (putUIListeners) {
            if (putUIListeners.size() > 0) {
                IPutUIListener listener;
                Iterator<IPutUIListener> listenerItr = putUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onPutCompleted(resource);
                    }
                }
            }
        }
    }

    private void postCompleteNotification(RemoteResource resource) {
        synchronized (postUIListeners) {
            if (postUIListeners.size() > 0) {
                IPostUIListener listener;
                Iterator<IPostUIListener> listenerItr = postUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onPostCompleted(resource);
                    }
                }
            }
        }
    }

    private void observeCompleteNotification(RemoteResource resource) {
        synchronized (observeUIListeners) {
            if (observeUIListeners.size() > 0) {
                IObserveUIListener listener;
                Iterator<IObserveUIListener> listenerItr = observeUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onObserveCompleted(resource);
                    }
                }
            }
        }
    }

    private void verificationStartedNotification(RemoteResource resource,
            int autoType) {
        synchronized (verificationUIListeners) {
            if (verificationUIListeners.size() > 0) {
                IVerificationUIListener listener;
                Iterator<IVerificationUIListener> listenerItr = verificationUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onVerificationStarted(resource, autoType);
                    }
                }
            }
        }
    }

    private void verificationAbortedNotification(RemoteResource resource,
            int autoType) {
        synchronized (verificationUIListeners) {
            if (verificationUIListeners.size() > 0) {
                IVerificationUIListener listener;
                Iterator<IVerificationUIListener> listenerItr = verificationUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onVerificationAborted(resource, autoType);
                    }
                }
            }
        }
    }

    private void verificationCompletedNotification(RemoteResource resource,
            int autoType) {
        synchronized (verificationUIListeners) {
            if (verificationUIListeners.size() > 0) {
                IVerificationUIListener listener;
                Iterator<IVerificationUIListener> listenerItr = verificationUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onVerificationCompleted(resource, autoType);
                    }
                }
            }
        }
    }

    private void configUploadedNotification(RemoteResource resource) {
        synchronized (configUploadUIListeners) {
            if (configUploadUIListeners.size() > 0) {
                IConfigurationUpload listener;
                Iterator<IConfigurationUpload> listenerItr = configUploadUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onConfigurationUploaded(resource);
                    }
                }
            }
        }
    }

    private void deviceInfoReceivedNotification() {
        synchronized (devicePlatformInfoUIListeners) {
            if (devicePlatformInfoUIListeners.size() > 0) {
                IDevicePlatformInfoUIListener listener;
                Iterator<IDevicePlatformInfoUIListener> listenerItr = devicePlatformInfoUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onDeviceInfoFound();
                    }
                }
            }
        }
    }

    private void platformInfoReceivedNotification() {
        synchronized (devicePlatformInfoUIListeners) {
            if (devicePlatformInfoUIListeners.size() > 0) {
                IDevicePlatformInfoUIListener listener;
                Iterator<IDevicePlatformInfoUIListener> listenerItr = devicePlatformInfoUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onPlatformInfoFound();
                    }
                }
            }
        }
    }

    public synchronized Set<String> getLastKnownSearchTypes() {
        return lastKnownSearchTypes;
    }

    public synchronized void setLastKnownSearchTypes(
            Set<String> lastKnownSearchTypes) {
        this.lastKnownSearchTypes = lastKnownSearchTypes;
    }

    public boolean findResourceRequest(Set<String> searchTypes) {
        boolean result = false;
        if (null == searchTypes || searchTypes.size() < 1) {
            try {
                SimulatorManager.findResource(findResourceListener);
                result = true;
            } catch (SimulatorException e) {
                Activator
                        .getDefault()
                        .getLogManager()
                        .log(Level.ERROR.ordinal(), new Date(),
                                Utility.getSimulatorErrorString(e, null));
            }
        } else {
            Iterator<String> searchItr = searchTypes.iterator();
            String rType;
            while (searchItr.hasNext()) {
                rType = searchItr.next();
                try {
                    SimulatorManager.findResource(rType, findResourceListener);
                    result = true;
                } catch (SimulatorException e) {
                    Activator
                            .getDefault()
                            .getLogManager()
                            .log(Level.ERROR.ordinal(), new Date(),
                                    Utility.getSimulatorErrorString(e, null));
                }
            }
        }
        return result;
    }

    public void deleteResources(final Set<String> searchTypes) {
        synchronized (resourceMap) {
            if (null == resourceMap && resourceMap.isEmpty()) {
                return;
            }
        }
        new Thread() {
            public void run() {
                if (null == searchTypes || searchTypes.size() < 1) {
                    synchronized (resourceMap) {
                        // Stop observing all the resources
                        Iterator<String> itr = resourceMap.keySet().iterator();
                        while (itr.hasNext()) {
                            sendCancelObserveRequest(
                                    resourceMap.get(itr.next()), false);
                        }
                        // Delete all cached details of resources
                        resourceMap.clear();
                        favoriteResources.clear();

                        // Clearing the device and platform information
                        hostDeviceAndPlatformMap.clear();
                    }
                    // Change the current resource in selection
                    setCurrentResourceInSelection(null);
                    resourceSelectionChangedUINotification(null);
                } else {
                    Iterator<String> typeItr = searchTypes.iterator();
                    String resType;
                    while (typeItr.hasNext()) {
                        resType = typeItr.next();
                        deleteResourcesByType(resType);

                        // Change the current resource in selection
                        updateCurrentResourceInSelection(searchTypes);
                    }
                }
            }
        }.start();
    }

    private void updateCurrentResourceInSelection(Set<String> searchTypes) {
        if (null == searchTypes || searchTypes.size() < 1) {
            return;
        }
        RemoteResource resourceInSelection = getCurrentResourceInSelection();
        if (null == resourceInSelection) {
            return;
        }
        List<String> typesOfSelection = resourceInSelection
                .getRemoteResourceRef().getResourceTypes();
        if (null == typesOfSelection || typesOfSelection.size() < 1) {
            return;
        }
        Iterator<String> itr = typesOfSelection.iterator();
        String type;
        while (itr.hasNext()) {
            type = itr.next();
            if (searchTypes.contains(type)) {
                setCurrentResourceInSelection(null);
                resourceSelectionChangedUINotification(null);
                break;
            }
        }
    }

    private void deleteResourcesByType(String resourceType) {
        if (null == resourceType) {
            return;
        }
        synchronized (resourceMap) {
            Set<String> keySet = resourceMap.keySet();
            if (null == keySet) {
                return;
            }
            Iterator<String> keyItr = keySet.iterator();
            String uId;
            RemoteResource resource;
            boolean exist;
            List<String> types;
            while (keyItr.hasNext()) {
                uId = keyItr.next();
                resource = resourceMap.get(uId);
                if (null == resource) {
                    continue;
                }
                types = resource.getRemoteResourceRef().getResourceTypes();
                if (null != types) {
                    exist = types.contains(resourceType);
                    if (exist) {
                        // Cancel observing the resource.
                        sendCancelObserveRequest(resource, false);
                        // Remove the resource from favorites list.
                        removeResourceFromFavorites(resource);
                        // Remove the resource
                        keyItr.remove();
                        // Remove the device and platform information
                        synchronized (hostDeviceAndPlatformMap) {
                            hostDeviceAndPlatformMap.remove(resource
                                    .getRemoteResourceRef().getHost());
                        }
                    }
                }
            }
        }
    }

    public void resourceSelectionChanged(final RemoteResource resource) {
        new Thread() {
            @Override
            public void run() {
                setCurrentResourceInSelection(resource);
                // Notify all observers for resource selection change event
                resourceSelectionChangedUINotification(resource);
            }
        }.start();
    }

    public List<MetaProperty> getDefaultProperties(RemoteResource resource) {
        if (null != resource) {
            String propName;
            String propValue;

            List<MetaProperty> metaPropertyList = new ArrayList<MetaProperty>();

            for (int index = 0; index < Constants.META_PROPERTY_COUNT; index++) {
                propName = Constants.META_PROPERTIES[index];
                if (propName.equals(Constants.RESOURCE_URI)) {
                    propValue = resource.getRemoteResourceRef().getURI();
                } else if (propName.equals(Constants.CONNECTIVITY_TYPE)) {
                    propValue = resource.getRemoteResourceRef()
                            .getConnectivityType().toString();
                } else if (propName.equals(Constants.ADDRESS)) {
                    propValue = resource.getRemoteResourceRef().getHost();
                } else if (propName.equals(Constants.OBSERVABLE)) {
                    propValue = Utility.getObservableInString(resource
                            .getRemoteResourceRef().isObservable());
                } else if (propName.equals(Constants.RESOURCE_TYPES)) {
                    Vector<String> types = resource.getRemoteResourceRef()
                            .getResourceTypes();
                    if (null != types) {
                        propValue = types.toString();
                    } else {
                        propValue = Constants.NOT_AVAILABLE;
                    }
                } else if (propName.equals(Constants.RESOURCE_INTERFACES)) {
                    Vector<String> interfaces = resource.getRemoteResourceRef()
                            .getResourceInterfaces();
                    if (null != interfaces) {
                        propValue = interfaces.toString();
                    } else {
                        propValue = Constants.NOT_AVAILABLE;
                    }
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

    public List<MetaProperty> getDeviceProperties() {
        if (null == currentResourceInSelection) {
            return null;
        }

        SimulatorRemoteResource remoteResource = currentResourceInSelection
                .getRemoteResourceRef();
        if (null == remoteResource) {
            return null;
        }

        String host = remoteResource.getHost();
        if (null == host) {
            return null;
        }

        if (!isDeviceInfoExist(host)) {
            // Device Information
            try {
                SimulatorManager.findDevices(host, deviceListener);
            } catch (SimulatorException e) {
                Activator
                        .getDefault()
                        .getLogManager()
                        .log(Level.ERROR.ordinal(), new Date(),
                                Utility.getSimulatorErrorString(e, null));
            }
            return null;
        }

        List<MetaProperty> metaProperties = new ArrayList<MetaProperty>();
        synchronized (hostDeviceAndPlatformMap) {
            DeviceInfo devInfo = hostDeviceAndPlatformMap.get(host)
                    .getDeviceInfo();
            metaProperties.add(new MetaProperty(Constants.DEVICE_ID, devInfo
                    .getID()));
            metaProperties.add(new MetaProperty(Constants.DEVICE_NAME, devInfo
                    .getName()));
            metaProperties.add(new MetaProperty(Constants.DEVICE_SPEC_VERSION,
                    devInfo.getSpecVersion()));
            metaProperties.add(new MetaProperty(Constants.DEVICE_DMV_VERSION,
                    devInfo.getDataModelVersion()));
        }

        /*
         * metaProperties.add(new MetaProperty(Constants.DEVICE_ID, ""));
         * metaProperties.add(new MetaProperty(Constants.DEVICE_NAME, ""));
         * metaProperties.add(new MetaProperty(Constants.DEVICE_SPEC_VERSION,
         * "")); metaProperties.add(new
         * MetaProperty(Constants.DEVICE_DMV_VERSION, ""));
         */
        return metaProperties;
    }

    public List<MetaProperty> getPlatformProperties() {
        if (null == currentResourceInSelection) {
            return null;
        }

        SimulatorRemoteResource remoteResource = currentResourceInSelection
                .getRemoteResourceRef();
        if (null == remoteResource) {
            return null;
        }

        String host = remoteResource.getHost();
        if (null == host) {
            return null;
        }

        if (!isPlatformInfoExist(host)) {
            // Platform Information
            try {
                SimulatorManager.getPlatformInformation(host, platformListener);
            } catch (SimulatorException e) {
                Activator
                        .getDefault()
                        .getLogManager()
                        .log(Level.ERROR.ordinal(), new Date(),
                                Utility.getSimulatorErrorString(e, null));
            }
            return null;
        }

        List<MetaProperty> metaProperties = new ArrayList<MetaProperty>();
        synchronized (hostDeviceAndPlatformMap) {
            PlatformInfo platInfo = hostDeviceAndPlatformMap.get(host)
                    .getPlatformInfo();
            metaProperties.add(new MetaProperty(Constants.PLATFORM_ID, platInfo
                    .getPlatformID()));
            metaProperties.add(new MetaProperty(
                    Constants.PLATFORM_MANUFAC_NAME, platInfo
                            .getManufacturerName()));
            metaProperties.add(new MetaProperty(Constants.PLATFORM_MANUFAC_URL,
                    platInfo.getManufacturerUrl()));
            metaProperties.add(new MetaProperty(Constants.PLATFORM_MODEL_NO,
                    platInfo.getModelNumber()));
            metaProperties.add(new MetaProperty(
                    Constants.PLATFORM_DATE_OF_MANUFAC, platInfo
                            .getDateOfManufacture()));
            metaProperties.add(new MetaProperty(Constants.PLATFORM_VERSION,
                    platInfo.getPlatformVersion()));
            metaProperties.add(new MetaProperty(Constants.PLATFORM_OS_VERSION,
                    platInfo.getOperationSystemVersion()));
            metaProperties.add(new MetaProperty(
                    Constants.PLATFORM_HARDWARE_VERSION, platInfo
                            .getHardwareVersion()));
            metaProperties.add(new MetaProperty(
                    Constants.PLATFORM_FIRMWARE_VERSION, platInfo
                            .getFirmwareVersion()));
            metaProperties.add(new MetaProperty(Constants.PLATFORM_SUPPORT_URL,
                    platInfo.getSupportUrl()));
            metaProperties.add(new MetaProperty(Constants.PLATFORM_SYSTEM_TIME,
                    platInfo.getSystemTime()));
        }
        /*
         * metaProperties.add(new MetaProperty(Constants.PLATFORM_ID, ""));
         * metaProperties .add(new MetaProperty(Constants.PLATFORM_MANUFAC_NAME,
         * "")); metaProperties .add(new
         * MetaProperty(Constants.PLATFORM_MANUFAC_URL, ""));
         * metaProperties.add(new MetaProperty(Constants.PLATFORM_MODEL_NO,
         * "")); metaProperties.add(new
         * MetaProperty(Constants.PLATFORM_DATE_OF_MANUFAC, ""));
         * metaProperties.add(new MetaProperty(Constants.PLATFORM_VERSION, ""));
         * metaProperties.add(new MetaProperty(Constants.PLATFORM_OS_VERSION,
         * "")); metaProperties.add(new MetaProperty(
         * Constants.PLATFORM_HARDWARE_VERSION, "")); metaProperties.add(new
         * MetaProperty( Constants.PLATFORM_FIRMWARE_VERSION, ""));
         * metaProperties .add(new MetaProperty(Constants.PLATFORM_SUPPORT_URL,
         * "")); metaProperties .add(new
         * MetaProperty(Constants.PLATFORM_SYSTEM_TIME, ""));
         */
        return metaProperties;
    }

    public Map<String, Boolean> getAutomationStatus(RemoteResource resource) {
        if (null == resource) {
            return null;
        }
        Map<String, Boolean> autoStatus = new HashMap<String, Boolean>();
        autoStatus.put(Constants.GET, resource.isGetAutomtnInProgress());
        autoStatus.put(Constants.PUT, resource.isPutAutomtnInProgress());
        autoStatus.put(Constants.POST, resource.isPostAutomtnInProgress());
        return autoStatus;
    }

    public List<RemoteResource> getResourceList() {
        List<RemoteResource> resourceList = new ArrayList<RemoteResource>();
        synchronized (resourceMap) {
            Set<String> idSet = resourceMap.keySet();
            Iterator<String> idItr = idSet.iterator();
            RemoteResource resource;
            while (idItr.hasNext()) {
                resource = resourceMap.get(idItr.next());
                if (null != resource) {
                    resourceList.add(resource);
                }
            }
        }
        // Sort the list
        Collections.sort(resourceList, new Comparator<RemoteResource>() {
            public int compare(RemoteResource res1, RemoteResource res2) {
                String s1 = res1.getRemoteResourceRef().getURI();
                String s2 = res2.getRemoteResourceRef().getURI();

                String s1Part = s1.replaceAll("\\d", "");
                String s2Part = s2.replaceAll("\\d", "");

                if (s1Part.equalsIgnoreCase(s2Part)) {
                    return extractInt(s1) - extractInt(s2);
                }
                return s1.compareTo(s2);
            }

            int extractInt(String s) {
                String num = s.replaceAll("\\D", "");
                // return 0 if no digits found
                return num.isEmpty() ? 0 : Integer.parseInt(num);
            }
        });

        return resourceList;
    }

    public List<RemoteResource> getFavResourceList() {
        List<RemoteResource> resourceList;
        synchronized (favoriteResources) {
            resourceList = new ArrayList<RemoteResource>(favoriteResources);
        }
        return resourceList;
    }

    public List<String> getAllValuesOfAttribute(SimulatorResourceAttribute att) {
        if (null == att) {
            return null;
        }

        AttributeValue val = att.value();
        if (null == val) {
            return null;
        }

        List<String> values = new ArrayList<String>();

        TypeInfo type = val.typeInfo();

        AttributeProperty prop = att.property();
        if (null == prop || prop.type().ordinal() == Type.UNKNOWN.ordinal()) {
            values.add(Utility.getAttributeValueAsString(val));
            return values;
        }

        Type valuesType = prop.type();

        if (type.mType != ValueType.RESOURCEMODEL) {
            if (type.mType == ValueType.ARRAY) {
                if (type.mDepth == 1) {
                    AttributeProperty childProp = prop.getChildProperty();
                    if (null != childProp) {
                        valuesType = childProp.type();
                        if (valuesType.ordinal() == Type.RANGE.ordinal()) {
                            List<String> list = getRangeForPrimitiveNonArrayAttributes(
                                    childProp, type.mBaseType);
                            if (null != list) {
                                values.addAll(list);
                            }
                        } else if (valuesType.ordinal() == Type.VALUESET
                                .ordinal()) {
                            List<String> list = getAllowedValuesForPrimitiveNonArrayAttributes(
                                    childProp.valueSet(), type.mBaseType);
                            if (null != list) {
                                values.addAll(list);
                            }
                        }
                    }
                }
            } else {
                if (valuesType.ordinal() == Type.RANGE.ordinal()) {
                    List<String> list = getRangeForPrimitiveNonArrayAttributes(
                            prop, type.mType);
                    if (null != list) {
                        values.addAll(list);
                    }
                } else if (valuesType.ordinal() == Type.VALUESET.ordinal()) {
                    List<String> list = getAllowedValuesForPrimitiveNonArrayAttributes(
                            prop.valueSet(), type.mType);
                    if (null != list) {
                        values.addAll(list);
                    }
                }
            }
        }

        if (values.isEmpty()) {
            values.add(Utility.getAttributeValueAsString(val));
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

    /*
     * public String getAttributeValue(RemoteResource res, String attName) { if
     * (null == res || null == attName) { return null; } return
     * res.getAttributeValue(attName); }
     */
    public void sendGetRequest(RemoteResource resource) {
        if (null == resource) {
            return;
        }
        SimulatorRemoteResource resourceN = resource.getRemoteResourceRef();
        if (null == resourceN) {
            return;
        }
        try {
            resourceN.get(null, getListener);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
        }
    }

    public void sendPutRequest(RemoteResource resource,
            SimulatorResourceModel model) {
        if (null == resource || null == model) {
            return;
        }
        SimulatorRemoteResource resourceN = resource.getRemoteResourceRef();
        if (null == resourceN) {
            return;
        }
        try {
            resourceN.put(null, model, putListener);
        } catch (Exception e) {
            String addlInfo;
            addlInfo = "Invalid Attribute Value. Cannot send PUT request.";
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, addlInfo));
        }
    }

    public void sendPostRequest(RemoteResource resource,
            SimulatorResourceModel model) {
        if (null == resource || null == model) {
            return;
        }
        SimulatorRemoteResource resourceN = resource.getRemoteResourceRef();
        if (null == resourceN) {
            return;
        }
        try {
            resourceN.post(null, model, postListener);
        } catch (Exception e) {
            String addlInfo;
            addlInfo = "Invalid Attribute Value. Cannot send POST request.";
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, addlInfo));
        }
    }

    public boolean sendObserveRequest(RemoteResource resource) {
        if (null == resource) {
            return false;
        }
        SimulatorRemoteResource resourceN = resource.getRemoteResourceRef();
        if (null == resourceN) {
            return false;
        }
        try {
            resourceN.startObserve(null, observeListener);
            resource.setObserved(true);
            // Add observed resource URI to show the proper status after every
            // find/refresh operations.
            addObservedResourceURI(resource.getRemoteResourceRef().getURI());
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
            return false;
        }
        return true;
    }

    public boolean sendCancelObserveRequest(RemoteResource resource,
            boolean removeEntry) {
        if (null == resource || !resource.isObserved()) {
            return false;
        }
        SimulatorRemoteResource resourceN = resource.getRemoteResourceRef();
        if (null == resourceN) {
            return false;
        }
        try {
            resourceN.stopObserve();
            resource.setObserved(false);
            // Remove observed resource URI to show the proper status after
            // every find/refresh operations.
            if (removeEntry)
                removeObservedResourceURI(resource.getRemoteResourceRef()
                        .getURI());
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
            return false;
        }
        return true;
    }

    public void startAutomationRequest(VerificationType reqType,
            RemoteResource resource) {
        if (null == resource) {
            return;
        }
        SimulatorRemoteResource resourceN = resource.getRemoteResourceRef();
        if (null == resourceN) {
            return;
        }
        if (null == reqType) {
            return;
        }
        int autoId;
        try {
            autoId = resourceN.startVerification(reqType, verifyListener);
            if (autoId != -1) {
                if (reqType == VerificationType.GET) {
                    // resource.setGetAutomtnInProgress(true);
                    resource.setGetAutomtnId(autoId);
                } else if (reqType == VerificationType.PUT) {
                    // resource.setPutAutomtnInProgress(true);
                    resource.setPutAutomtnId(autoId);
                } else {// if(reqType == Constants.POST_AUTOMATION_INDEX) {
                    // resource.setPostAutomtnInProgress(true);
                    resource.setPostAutomtnId(autoId);
                }
            }
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
        }
    }

    public void stopAutomationRequest(VerificationType reqType,
            RemoteResource resource) {
        if (null == resource) {
            return;
        }
        SimulatorRemoteResource resourceN = resource.getRemoteResourceRef();
        if (null == resourceN) {
            return;
        }
        int autoId;
        if (reqType == VerificationType.GET) {
            resource.setGetAutomtnInProgress(false);
            autoId = resource.getGetAutomtnId();
        } else if (reqType == VerificationType.PUT) {
            resource.setPutAutomtnInProgress(false);
            autoId = resource.getPutAutomtnId();
        } else {// if(reqType == Constants.POST_AUTOMATION_INDEX) {
            resource.setPostAutomtnInProgress(false);
            autoId = resource.getPostAutomtnId();
        }
        try {
            resourceN.stopVerification(autoId);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
        }
    }

    public boolean setConfigFilePath(RemoteResource resource,
            String configFilePath) throws SimulatorException {
        if (null == resource) {
            return false;
        }
        SimulatorRemoteResource resourceN = resource.getRemoteResourceRef();
        if (null == resourceN) {
            return false;
        }
        try {
            SimulatorResourceModel configuredResourceModel;
            configuredResourceModel = resourceN.setConfigInfo(configFilePath);
            if (null == configuredResourceModel) {
                return false;
            }

            // Store the resource model in the local cache
            SimulatorResourceModel resourceModel = resource
                    .getResourceModelRef();
            if (null != resourceModel) {
                configuredResourceModel.update(resourceModel);
            }
            resource.setResourceModelRef(configuredResourceModel);
        } catch (SimulatorException e) {
            Activator
                    .getDefault()
                    .getLogManager()
                    .log(Level.ERROR.ordinal(), new Date(),
                            Utility.getSimulatorErrorString(e, null));
            throw e;
        }
        // Update the status
        resource.setConfigUploaded(true);

        // Notify the UI listeners
        configUploadedNotification(resource);

        return true;
    }

    public void shutdown() {
    }
}
