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

package oic.simulator.clientcontroller.remoteresource;

import oic.simulator.clientcontroller.utils.Constants;

import org.oic.simulator.SimulatorResourceModel;
import org.oic.simulator.client.SimulatorRemoteResource;

/**
 * This class represents a remote resource. It maintains all the necessary
 * information about the resource.
 */
public class RemoteResource {

    private boolean                 observed;

    // Native object references
    private SimulatorRemoteResource remoteResourceRef;
    private SimulatorResourceModel  resourceModelRef;
    private ResourceRepresentation  mResourceRepresentation;
    // private Map<String, RemoteResourceAttribute> resourceAttributesMap;

    private boolean                 configUploaded;

    private boolean                 getAutomtnInProgress;
    private boolean                 putAutomtnInProgress;
    private boolean                 postAutomtnInProgress;

    private int                     getAutomtnId;
    private int                     putAutomtnId;
    private int                     postAutomtnId;

    private boolean                 isFavorite;

    public SimulatorResourceModel getResourceModelRef() {
        return resourceModelRef;
    }

    public void setResourceModelRef(SimulatorResourceModel resourceModel) {
        this.resourceModelRef = resourceModel;
    }

    /*
     * public Map<String, RemoteResourceAttribute> getResourceAttributesMap() {
     * return resourceAttributesMap; }
     * 
     * public void setResourceAttributesMap( Map<String,
     * RemoteResourceAttribute> resourceAttributesMap) {
     * this.resourceAttributesMap = resourceAttributesMap; }
     */
    public int getGetAutomtnId() {
        return getAutomtnId;
    }

    public void setGetAutomtnId(int getAutomtnId) {
        this.getAutomtnId = getAutomtnId;
    }

    public int getPutAutomtnId() {
        return putAutomtnId;
    }

    public void setPutAutomtnId(int putAutomtnId) {
        this.putAutomtnId = putAutomtnId;
    }

    public int getPostAutomtnId() {
        return postAutomtnId;
    }

    public void setPostAutomtnId(int postAutomtnId) {
        this.postAutomtnId = postAutomtnId;
    }

    public boolean isGetAutomtnInProgress() {
        return getAutomtnInProgress;
    }

    public void setGetAutomtnInProgress(boolean getAutomtnInProgress) {
        this.getAutomtnInProgress = getAutomtnInProgress;
    }

    public boolean isPutAutomtnInProgress() {
        return putAutomtnInProgress;
    }

    public void setPutAutomtnInProgress(boolean putAutomtnInProgress) {
        this.putAutomtnInProgress = putAutomtnInProgress;
    }

    public boolean isPostAutomtnInProgress() {
        return postAutomtnInProgress;
    }

    public void setPostAutomtnInProgress(boolean postAutomtnInProgress) {
        this.postAutomtnInProgress = postAutomtnInProgress;
    }

    public boolean isConfigUploaded() {
        return configUploaded;
    }

    public void setConfigUploaded(boolean configUploaded) {
        this.configUploaded = configUploaded;
    }

    public SimulatorRemoteResource getRemoteResourceRef() {
        return remoteResourceRef;
    }

    public void setRemoteResourceRef(SimulatorRemoteResource resource) {
        this.remoteResourceRef = resource;
    }

    public boolean isObserved() {
        return observed;
    }

    public void setObserved(boolean observed) {
        this.observed = observed;
    }

    /*
     * public List<PutPostAttributeModel> getPutPostModel() { Map<String,
     * RemoteResourceAttribute> attMap = getResourceAttributesMap(); if (null ==
     * attMap || attMap.size() < 1) { return null; } List<PutPostAttributeModel>
     * putPostModelList = new ArrayList<PutPostAttributeModel>(); String
     * attName; RemoteResourceAttribute attribute; PutPostAttributeModel
     * putPostModel; Iterator<String> attItr = attMap.keySet().iterator(); while
     * (attItr.hasNext()) { attName = attItr.next(); attribute =
     * attMap.get(attName); putPostModel =
     * PutPostAttributeModel.getModel(attribute); if (null != putPostModel) {
     * putPostModelList.add(putPostModel); } } return putPostModelList; }
     * 
     * public String getAttributeValue(String attName) { RemoteResourceAttribute
     * attribute = resourceAttributesMap.get(attName); if (null == attribute) {
     * return null; } return String.valueOf(attribute.getAttributeValue()); }
     */
    public int getAutomationtype(int autoId) {
        if (getAutomtnId == autoId) {
            return Constants.GET_AUTOMATION_INDEX;
        } else if (putAutomtnId == autoId) {
            return Constants.PUT_AUTOMATION_INDEX;
        } else {// if(postAutomtnId == autoId) {
            return Constants.POST_AUTOMATION_INDEX;
        }
    }

    public void updateAutomationStatus(int autoId, boolean status) {
        if (getAutomtnId == autoId) {
            getAutomtnInProgress = status;
        } else if (putAutomtnId == autoId) {
            putAutomtnInProgress = status;
        } else {// if(postAutomtnId == autoId) {
            postAutomtnInProgress = status;
        }
    }

    public boolean isFavorite() {
        return isFavorite;
    }

    public void setFavorite(boolean isFavorite) {
        this.isFavorite = isFavorite;
    }

    public void setResourceRepresentation(SimulatorResourceModel resModel,
            boolean ramlUploaded) {
        if (mResourceRepresentation == null)
            mResourceRepresentation = new ResourceRepresentation(resModel);
        else
            mResourceRepresentation.update(resModel, ramlUploaded);
    }

    public ResourceRepresentation getResourceRepresentation() {
        return mResourceRepresentation;
    }
}
