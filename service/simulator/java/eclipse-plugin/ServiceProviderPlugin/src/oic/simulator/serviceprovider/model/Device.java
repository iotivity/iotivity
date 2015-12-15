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

package oic.simulator.serviceprovider.model;

import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

public class Device {
    private String        deviceName;

    private Set<Resource> childResources;

    public Device() {
        childResources = new HashSet<Resource>();
    }

    public String getDeviceName() {
        return deviceName;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
    }

    public Set<Resource> getChildResources() {
        return childResources;
    }

    public void setChildResources(Set<Resource> childResources) {
        this.childResources = childResources;
    }

    public void addChildResource(Resource resource) {
        if (null == resource) {
            return;
        }
        if (null == childResources) {
            childResources = new HashSet<Resource>();
        }
        childResources.add(resource);
    }

    public void addChildResource(Set<Resource> resources) {
        if (null == resources) {
            return;
        }
        if (null == childResources) {
            childResources = new HashSet<Resource>();
        }
        childResources.addAll(resources);
    }

    public void removeChildResource(Resource resource) {
        if (null == resource || null == childResources) {
            return;
        }
        childResources.remove(resource);
    }

    public void removeChildResource(Set<Resource> resources) {
        if (null == resources || null == childResources) {
            return;
        }
        childResources.removeAll(resources);
    }

    public Set<SingleResource> getSingleTypeChildResources() {
        if (null == childResources || childResources.isEmpty()) {
            return null;
        }
        Set<SingleResource> resultSet = new HashSet<SingleResource>();
        Iterator<Resource> itr = childResources.iterator();
        Resource res;
        while (itr.hasNext()) {
            res = itr.next();
            if (null != res) {
                if (res instanceof SingleResource) {
                    resultSet.add((SingleResource) res);
                }
            }
        }
        return resultSet;
    }

    public Set<CollectionResource> getCollectionTypeChildResources() {
        if (null == childResources || childResources.isEmpty()) {
            return null;
        }
        Set<CollectionResource> resultSet = new HashSet<CollectionResource>();
        Iterator<Resource> itr = childResources.iterator();
        Resource res;
        while (itr.hasNext()) {
            res = itr.next();
            if (null != res) {
                if (res instanceof CollectionResource) {
                    resultSet.add((CollectionResource) res);
                }
            }
        }
        return resultSet;
    }
}