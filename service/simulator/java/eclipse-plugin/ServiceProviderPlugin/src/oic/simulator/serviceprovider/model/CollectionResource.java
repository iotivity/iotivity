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

import org.oic.simulator.SimulatorException;
import org.oic.simulator.server.SimulatorCollectionResource;
import org.oic.simulator.server.SimulatorResource;

public class CollectionResource extends Resource {
    private Set<Resource>           childResources;
    private Set<String>             supportedResourceTypes;

    private Set<CollectionResource> membership;

    public CollectionResource() {
        childResources = new HashSet<Resource>();
        supportedResourceTypes = new HashSet<String>();
        membership = new HashSet<CollectionResource>();
        setDeviceMembership(new HashSet<Device>());
    }

    public Set<String> getSupportedResourceTypes() {
        return supportedResourceTypes;
    }

    public void setSupportedResourceTypes(Set<String> supportedResourceTypes) {
        this.supportedResourceTypes = supportedResourceTypes;
    }

    public void addSupportedResourceType(String rType) {
        if (null == supportedResourceTypes) {
            supportedResourceTypes = new HashSet<String>();
        }
        supportedResourceTypes.add(rType);
    }

    public void removeSupportedResourceType(String rType) {
        if (null != supportedResourceTypes) {
            supportedResourceTypes.remove(rType);
        }
    }

    public Set<Resource> getChildResources() {
        return childResources;
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

    public void setChildResources(Set<Resource> childResources) {
        this.childResources = childResources;
    }

    public void addChildResource(Resource resource) throws SimulatorException {
        if (null == resource) {
            return;
        }
        if (null == childResources) {
            childResources = new HashSet<Resource>();
        }

        // Native call to add child resource
        SimulatorResource simRes;
        simRes = resource.getSimulatorResource();
        ((SimulatorCollectionResource) simulatorResource)
                .addChildResource(simRes);

        childResources.add(resource);
    }

    public void addChildResource(Set<Resource> resources)
            throws SimulatorException {
        if (null == resources || resources.isEmpty()) {
            return;
        }
        if (null == childResources) {
            childResources = new HashSet<Resource>();
        }

        // Native call to add child resource
        SimulatorResource simRes;
        Resource res;
        Iterator<Resource> itr = resources.iterator();
        while (itr.hasNext()) {
            res = itr.next();
            simRes = res.getSimulatorResource();
            ((SimulatorCollectionResource) simulatorResource)
                    .addChildResource(simRes);

            childResources.add(res);
        }
    }

    public void removeChildResource(Resource resource)
            throws SimulatorException {
        if (null == resource || null == childResources) {
            return;
        }

        // Native call to add child resource
        SimulatorResource simRes;
        simRes = resource.getSimulatorResource();
        ((SimulatorCollectionResource) simulatorResource)
                .removeChildResource(simRes);

        childResources.remove(resource);
    }

    public void removeChildResource(Set<Resource> resources)
            throws SimulatorException {
        if (null == resources || null == childResources) {
            return;
        }

        // Native call to add child resource
        SimulatorResource simRes;
        Resource res;
        Iterator<Resource> itr = resources.iterator();
        while (itr.hasNext()) {
            res = itr.next();
            simRes = res.getSimulatorResource();
            ((SimulatorCollectionResource) simulatorResource)
                    .removeChildResource(simRes);

            childResources.remove(res);
        }
    }

    public Set<CollectionResource> getMembership() {
        return membership;
    }

    public void setMembership(Set<CollectionResource> membership) {
        this.membership = membership;
    }

    public void addMembership(CollectionResource resource) {
        if (null == resource) {
            return;
        }
        if (null == membership) {
            membership = new HashSet<CollectionResource>();
        }
        membership.add(resource);
    }

    public void addMembership(Set<CollectionResource> resources) {
        if (null == resources) {
            return;
        }
        if (null == membership) {
            membership = new HashSet<CollectionResource>();
        }
        membership.addAll(resources);
    }

    public void removeMembership(CollectionResource resource) {
        if (null == resource || null == membership) {
            return;
        }
        membership.remove(resource);
    }

    public void removeMembership(Set<CollectionResource> collections) {
        if (null == collections || null == membership) {
            return;
        }
        membership.removeAll(collections);
    }
}