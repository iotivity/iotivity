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
import java.util.Set;

import org.oic.simulator.server.SimulatorResource.AutoUpdateType;

/**
 * This class represents a simulated resource. It maintains all the necessary
 * information about the resource.
 */
public class SingleResource extends Resource {
    private Set<CollectionResource> collectionMembership;

    private boolean                 getAllowed;

    private boolean                 putAllowed;

    private boolean                 postAllowed;

    private int                     automationId;

    private boolean                 resourceAutomationInProgress;

    private boolean                 attributeAutomationInProgress;

    private int                     automationUpdateInterval;

    private AutoUpdateType          automationType;

    public SingleResource() {
        setCollectionMembership(new HashSet<CollectionResource>());
    }

    public Set<CollectionResource> getCollectionMembership() {
        return collectionMembership;
    }

    public void setCollectionMembership(
            Set<CollectionResource> collectionMembership) {
        this.collectionMembership = collectionMembership;
    }

    public void addCollectionMembership(CollectionResource resource) {
        if (null == resource) {
            return;
        }
        if (null == collectionMembership) {
            collectionMembership = new HashSet<CollectionResource>();
        }
        collectionMembership.add(resource);
    }

    public void addCollectionMembership(Set<CollectionResource> resources) {
        if (null == resources) {
            return;
        }
        if (null == collectionMembership) {
            collectionMembership = new HashSet<CollectionResource>();
        }
        collectionMembership.addAll(resources);
    }

    public void removeCollectionMembership(CollectionResource resource) {
        if (null == resource || null == collectionMembership) {
            return;
        }
        collectionMembership.remove(resource);
    }

    public void removeCollectionMembership(Set<CollectionResource> collections) {
        if (null == collections || null == collectionMembership) {
            return;
        }
        collectionMembership.removeAll(collections);
    }

    public boolean isMemberOfAnyCollection() {
        if (null == collectionMembership || collectionMembership.isEmpty()) {
            return false;
        }
        return true;
    }

    public boolean isGetAllowed() {
        return getAllowed;
    }

    public void setGetAllowed(boolean getAllowed) {
        this.getAllowed = getAllowed;
    }

    public boolean isPutAllowed() {
        return putAllowed;
    }

    public void setPutAllowed(boolean putAllowed) {
        this.putAllowed = putAllowed;
    }

    public boolean isPostAllowed() {
        return postAllowed;
    }

    public void setPostAllowed(boolean postAllowed) {
        this.postAllowed = postAllowed;
    }

    public int getAutomationUpdateInterval() {
        return automationUpdateInterval;
    }

    public void setAutomationUpdateInterval(int automationUpdateInterval) {
        this.automationUpdateInterval = automationUpdateInterval;
    }

    public AutoUpdateType getAutomationType() {
        return automationType;
    }

    public void setAutomationType(AutoUpdateType automationType) {
        this.automationType = automationType;
    }

    public int getAutomationId() {
        return automationId;
    }

    public void setAutomationId(int automationId) {
        this.automationId = automationId;
    }

    public boolean isResourceAutomationInProgress() {
        return resourceAutomationInProgress;
    }

    public void setResourceAutomationInProgress(
            boolean resourceAutomationInProgress) {
        this.resourceAutomationInProgress = resourceAutomationInProgress;
    }

    public boolean isAttributeAutomationInProgress() {
        return attributeAutomationInProgress;
    }

    public void setAttributeAutomationInProgress(
            boolean attributeAutomationInProgress) {
        this.attributeAutomationInProgress = attributeAutomationInProgress;
    }

    /*
     * public LocalResourceAttribute getAttribute(String attributeName) { if
     * (null == attributeName || null == resourceAttributesMap ||
     * resourceAttributesMap.size() < 1) { return null; } return
     * resourceAttributesMap.get(attributeName); }
     */
}