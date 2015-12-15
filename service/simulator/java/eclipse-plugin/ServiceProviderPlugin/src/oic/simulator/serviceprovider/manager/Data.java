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
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import oic.simulator.serviceprovider.model.CollectionResource;
import oic.simulator.serviceprovider.model.Device;
import oic.simulator.serviceprovider.model.Resource;
import oic.simulator.serviceprovider.model.SingleResource;

public class Data {
    private Map<String, SingleResource>     singleResourceMap;

    private Map<String, CollectionResource> collectionResourceMap;

    private Map<String, Device>             deviceMap;

    // Holds resource type as key and a resource URI set as values.
    // Helps in performing operations based on resource type.
    private Map<String, Set<String>>        resTypeToResUriMap;

    public Data() {
        singleResourceMap = new HashMap<String, SingleResource>();
        collectionResourceMap = new HashMap<String, CollectionResource>();
        deviceMap = new HashMap<String, Device>();
        resTypeToResUriMap = new HashMap<String, Set<String>>();
    }

    public synchronized void addResource(Resource resource) {
        if (null == resource) {
            return;
        }
        if (resource instanceof SingleResource) {
            singleResourceMap.put(resource.getResourceURI(),
                    (SingleResource) resource);
        } else {
            collectionResourceMap.put(resource.getResourceURI(),
                    (CollectionResource) resource);
        }
        addToTypeAndUriMap(resource);
    }

    public synchronized void addToTypeAndUriMap(Resource resource) {
        if (null == resource) {
            return;
        }
        Set<String> resTypes = resource.getResourceTypes();
        if (null == resTypes || resTypes.isEmpty()) {
            return;
        }
        String uri = resource.getResourceURI();
        if (null == uri || uri.isEmpty()) {
            return;
        }
        Iterator<String> itr = resTypes.iterator();
        String rType;
        Set<String> newTypeSet;
        while (itr.hasNext()) {
            rType = itr.next();
            newTypeSet = resTypeToResUriMap.get(rType);
            if (null == newTypeSet) {
                newTypeSet = new HashSet<String>();
                resTypeToResUriMap.put(rType, newTypeSet);
            }
            newTypeSet.add(uri);
        }
    }

    public void addDevice(Device dev) {
        if (null == dev) {
            return;
        }
        synchronized (deviceMap) {
            deviceMap.put(dev.getDeviceName(), dev);
        }
    }

    public synchronized void deleteResource(Resource resource) {
        if (null == resource) {
            return;
        }
        if (resource instanceof SingleResource) {
            singleResourceMap.remove(resource.getResourceURI());
        } else {
            collectionResourceMap.remove(resource.getResourceURI());
        }
        removeFromTypeAndUriMap(resource);
    }

    public synchronized void deleteDevice(Device dev) {
        if (null == dev) {
            return;
        }
        deviceMap.remove(dev.getDeviceName());
    }

    public synchronized void removeFromTypeAndUriMap(Resource resource) {
        if (null == resource) {
            return;
        }
        Set<String> resTypes = resource.getResourceTypes();
        if (null == resTypes || resTypes.isEmpty()) {
            return;
        }
        String uri = resource.getResourceURI();
        if (null == uri || uri.isEmpty()) {
            return;
        }
        Iterator<String> itr = resTypes.iterator();
        String rType;
        Set<String> newTypeSet;
        while (itr.hasNext()) {
            rType = itr.next();
            newTypeSet = resTypeToResUriMap.get(rType);
            if (null != newTypeSet) {
                newTypeSet.remove(uri);
            }
            if (null == newTypeSet || newTypeSet.isEmpty()) {
                resTypeToResUriMap.remove(rType);
            }
        }
    }

    public List<SingleResource> getSingleResources() {
        List<SingleResource> resources;
        synchronized (singleResourceMap) {
            if (singleResourceMap.isEmpty()) {
                return null;
            }
            resources = new ArrayList<SingleResource>();
            Set<String> uriSet = singleResourceMap.keySet();
            Iterator<String> itr = uriSet.iterator();
            while (itr.hasNext()) {
                resources.add(singleResourceMap.get(itr.next()));
            }
        }
        return resources;
    }

    public List<CollectionResource> getCollectionResources() {
        List<CollectionResource> resources;
        synchronized (collectionResourceMap) {
            if (collectionResourceMap.isEmpty()) {
                return null;
            }
            resources = new ArrayList<CollectionResource>();
            Set<String> uriSet = collectionResourceMap.keySet();
            Iterator<String> itr = uriSet.iterator();
            while (itr.hasNext()) {
                resources.add(collectionResourceMap.get(itr.next()));
            }
        }
        return resources;
    }

    public List<Resource> getResources() {
        if (singleResourceMap.isEmpty() && collectionResourceMap.isEmpty()) {
            return null;
        }
        List<Resource> resourceList = new ArrayList<Resource>();
        synchronized (singleResourceMap) {
            Set<String> uriSet = singleResourceMap.keySet();
            Iterator<String> itr = uriSet.iterator();
            while (itr.hasNext()) {
                resourceList.add(singleResourceMap.get(itr.next()));
            }
        }
        synchronized (collectionResourceMap) {
            Set<String> uriSet = collectionResourceMap.keySet();
            Iterator<String> itr = uriSet.iterator();
            while (itr.hasNext()) {
                resourceList.add(collectionResourceMap.get(itr.next()));
            }
        }
        return resourceList;
    }

    public List<Device> getDevices() {
        List<Device> devices;
        synchronized (deviceMap) {
            if (deviceMap.isEmpty()) {
                return null;
            }
            devices = new ArrayList<Device>();
            Set<String> uriSet = deviceMap.keySet();
            Iterator<String> itr = uriSet.iterator();
            while (itr.hasNext()) {
                devices.add(deviceMap.get(itr.next()));
            }
        }
        return devices;
    }

    public synchronized boolean isResourceExist(String resourceURI) {
        if (null == resourceURI || resourceURI.isEmpty()) {
            return false;
        }
        if ((null != singleResourceMap && singleResourceMap
                .containsKey(resourceURI))
                || (null != collectionResourceMap && collectionResourceMap
                        .containsKey(resourceURI))) {
            return true;
        }
        return false;
    }

    public synchronized boolean isAnyResourceExist() {
        return ((null != singleResourceMap && !singleResourceMap.isEmpty()) || (null != collectionResourceMap && !collectionResourceMap
                .isEmpty()));
    }

    public void changeResourceURI(Resource resource, String curURI,
            String newURI) {
        if (null == resource || null == curURI || null == newURI) {
            return;
        }
        if (resource instanceof SingleResource) {
            synchronized (singleResourceMap) {
                singleResourceMap.remove(curURI);
                singleResourceMap.put(newURI, (SingleResource) resource);
            }
        } else {
            synchronized (collectionResourceMap) {
                collectionResourceMap.remove(curURI);
                collectionResourceMap
                        .put(newURI, (CollectionResource) resource);
            }
        }
        resource.setResourceURI(newURI);
    }

    public void changeDeviceName(Device dev, String curName, String newName) {
        if (null == dev || null == curName || null == newName) {
            return;
        }
        synchronized (deviceMap) {
            deviceMap.remove(curName);
            deviceMap.put(newName, dev);
        }
        dev.setDeviceName(newName);
    }

    public SingleResource getSingleResourceByURI(String resourceURI) {
        if (null == resourceURI) {
            return null;
        }
        SingleResource res;
        synchronized (singleResourceMap) {
            res = singleResourceMap.get(resourceURI);
        }
        return res;
    }

    public CollectionResource getCollectionResourceByURI(String resourceURI) {
        if (null == resourceURI) {
            return null;
        }
        CollectionResource res;
        synchronized (collectionResourceMap) {
            res = collectionResourceMap.get(resourceURI);
        }
        return res;
    }

    public Resource getResourceByURI(String resourceURI) {
        Resource res = getSingleResourceByURI(resourceURI);
        if (null == res) {
            res = getCollectionResourceByURI(resourceURI);
        }
        return res;
    }

}
