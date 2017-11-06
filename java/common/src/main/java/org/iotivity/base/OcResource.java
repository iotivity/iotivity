/*
 *******************************************************************
 *
 * Copyright 2015 Intel Corporation.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base;

import java.util.EnumSet;
import java.util.List;
import java.util.Map;

/**
 * OcResource represents an OC resource. A resource could be a light controller, temperature sensor,
 * smoke detector, etc. A resource comes with a well-defined contract or interface onto which you
 * can perform different operations, such as turning on the light, getting the current temperature
 * or subscribing for event notifications from the smoke detector. A resource can be composed of
 * one or more resources.
 */
public class OcResource {
    public static final String CREATED_URI_KEY = "createduri";

    private OcResource(long nativeHandle) {
        this.mNativeHandle = nativeHandle;
    }

    /**
     * Method to get the attributes of a resource.
     *
     * @param queryParamsMap map which can have the query parameter name and value
     * @param onGetListener  The event handler will be invoked with a map of attribute name and
     *                       values. The event handler will also have the result from this Get
     *                       operation This will have error codes
     * @throws OcException if failure
     */
    public native void get(Map<String, String> queryParamsMap,
                           OnGetListener onGetListener) throws OcException;

    /**
     * Method to get the attributes of a resource.
     *
     * @param queryParamsMap   map which can have the query parameter name and value
     * @param onGetListener    The event handler will be invoked with a map of attribute name and
     *                         values. The event handler will also have the result from this Get
     *                         operation This will have error codes
     * @param qualityOfService the quality of communication
     * @throws OcException if failure
     */
    public void get(Map<String, String> queryParamsMap,
                    OnGetListener onGetListener,
                    QualityOfService qualityOfService) throws OcException {

        if (qualityOfService == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "qualityOfService cannot be null");
        }

        this.get1(queryParamsMap, onGetListener, qualityOfService.getValue());
    }

    private native void get1(Map<String, String> queryParamsMap,
                             OnGetListener onGetListener,
                             int qualityOfService) throws OcException;

    /**
     * Method to get the attributes of a resource.
     *
     * @param resourceType      resourceType of the resource to operate on
     * @param resourceInterface interface type of the resource to operate on
     * @param queryParamsMap    map which can have the query parameter name and value
     * @param onGetListener     The event handler will be invoked with a map of attribute name and
     *                          values. The event handler will also have the result from this Get
     *                          operation This will have error codes
     * @throws OcException if failure
     */
    public void get(String resourceType,
                    String resourceInterface,
                    Map<String, String> queryParamsMap,
                    OnGetListener onGetListener) throws OcException {
        this.get2(
                resourceType,
                resourceInterface,
                queryParamsMap,
                onGetListener);
    }

    private native void get2(String resourceType,
                             String resourceInterface,
                             Map<String, String> queryParamsMap,
                             OnGetListener onGetListener) throws OcException;

    /**
     * Method to get the attributes of a resource.
     *
     * @param resourceType      resourceType of the resource to operate on
     * @param resourceInterface interface type of the resource to operate on
     * @param queryParamsMap    map which can have the query parameter name and value
     * @param onGetListener     The event handler will be invoked with a map of attribute name and
     *                          values. The event handler will also have the result from this Get
     *                          operation This will have error codes
     * @param qualityOfService  the quality of communication
     * @throws OcException if failure
     */
    public void get(String resourceType,
                    String resourceInterface,
                    Map<String, String> queryParamsMap,
                    OnGetListener onGetListener,
                    QualityOfService qualityOfService) throws OcException {

        if (qualityOfService == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "qualityOfService cannot be null");
        }

        this.get3(
                resourceType,
                resourceInterface,
                queryParamsMap,
                onGetListener,
                qualityOfService.getValue());
    }

    private native void get3(String resourceType,
                             String resourceInterface,
                             Map<String, String> queryParamsMap,
                             OnGetListener onGetListener,
                             int qualityOfService) throws OcException;

    /**
     * Method to set the representation of a resource (via PUT)
     *
     * @param representation representation of the resource
     * @param queryParamsMap Map which can have the query parameter name and value
     * @param onPutListener  event handler The event handler will be invoked with a map of attribute
     *                       name and values.
     * @throws OcException if failure
     */
    public native void put(OcRepresentation representation,
                           Map<String, String> queryParamsMap,
                           OnPutListener onPutListener) throws OcException;

    /**
     * Method to set the representation of a resource (via PUT)
     *
     * @param ocRepresentation representation of the resource
     * @param queryParamsMap   Map which can have the query parameter name and value
     * @param onPutListener    event handler The event handler will be invoked with a map of
     *                         attribute name and values.
     * @param qualityOfService the quality of communication
     * @throws OcException if failure
     */
    public void put(OcRepresentation ocRepresentation,
                    Map<String, String> queryParamsMap,
                    OnPutListener onPutListener,
                    QualityOfService qualityOfService) throws OcException {

        if (qualityOfService == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "qualityOfService cannot be null");
        }

        this.put1(
                ocRepresentation,
                queryParamsMap,
                onPutListener,
                qualityOfService.getValue());
    }

    private native void put1(OcRepresentation ocRepresentation,
                             Map<String, String> queryParamsMap,
                             OnPutListener onPutListener,
                             int qualityOfService) throws OcException;

    /**
     * Method to set the representation of a resource (via PUT)
     *
     * @param resourceType      resource type of the resource to operate on
     * @param resourceInterface interface type of the resource to operate on
     * @param ocRepresentation  representation of the resource
     * @param queryParamsMap    Map which can have the query parameter name and value
     * @param onPutListener     event handler The event handler will be invoked with a map of
     *                          attribute name and values.
     * @throws OcException if failure
     */
    public void put(String resourceType,
                    String resourceInterface,
                    OcRepresentation ocRepresentation,
                    Map<String, String> queryParamsMap,
                    OnPutListener onPutListener) throws OcException {
        this.put2(
                resourceType,
                resourceInterface,
                ocRepresentation,
                queryParamsMap,
                onPutListener);
    }

    private native void put2(String resourceType,
                             String resourceInterface,
                             OcRepresentation ocRepresentation,
                             Map<String, String> queryParamsMap,
                             OnPutListener onPutListener) throws OcException;

    /**
     * Method to set the representation of a resource (via PUT)
     *
     * @param resourceType      resource type of the resource to operate on
     * @param resourceInterface interface type of the resource to operate on
     * @param ocRepresentation  representation of the resource
     * @param queryParamsMap    Map which can have the query parameter name and value
     * @param onPutListener     event handler The event handler will be invoked with a map of
     *                          attribute name and values.
     * @param qualityOfService  the quality of communication
     * @throws OcException if failure
     */
    public void put(String resourceType,
                    String resourceInterface,
                    OcRepresentation ocRepresentation,
                    Map<String, String> queryParamsMap,
                    OnPutListener onPutListener,
                    QualityOfService qualityOfService) throws OcException {

        if (qualityOfService == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "qualityOfService cannot be null");
        }

        this.put3(
                resourceType,
                resourceInterface,
                ocRepresentation,
                queryParamsMap,
                onPutListener,
                qualityOfService.getValue());
    }

    private native void put3(String resourceType,
                             String resourceInterface,
                             OcRepresentation ocRepresentation,
                             Map<String, String> queryParamsMap,
                             OnPutListener onPutListener,
                             int qualityOfService) throws OcException;

    /**
     * Method to POST on a resource
     *
     * @param ocRepresentation representation of the resource
     * @param queryParamsMap   Map which can have the query parameter name and value
     * @param onPostListener   event handler The event handler will be invoked with a map of
     *                         attribute name and values.
     * @throws OcException if failure
     */
    public native void post(OcRepresentation ocRepresentation,
                            Map<String, String> queryParamsMap,
                            OnPostListener onPostListener) throws OcException;

    /**
     * Method to POST on a resource
     *
     * @param ocRepresentation representation of the resource
     * @param queryParamsMap   Map which can have the query parameter name and value
     * @param onPostListener   event handler The event handler will be invoked with a map of
     *                         attribute name and values.
     * @param qualityOfService the quality of communication
     * @throws OcException if failure
     */
    public void post(OcRepresentation ocRepresentation,
                     Map<String, String> queryParamsMap,
                     OnPostListener onPostListener,
                     QualityOfService qualityOfService) throws OcException {

        if (qualityOfService == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "qualityOfService cannot be null");
        }

        this.post1(
                ocRepresentation,
                queryParamsMap,
                onPostListener,
                qualityOfService.getValue());
    }

    private native void post1(OcRepresentation ocRepresentation,
                              Map<String, String> queryParamsMap,
                              OnPostListener onPostListener,
                              int qualityOfService) throws OcException;

    /**
     * Method to POST on a resource
     *
     * @param resourceType      resource type of the resource to operate on
     * @param resourceInterface interface type of the resource to operate on
     * @param ocRepresentation  representation of the resource
     * @param queryParamsMap    Map which can have the query parameter name and value
     * @param onPostListener    event handler The event handler will be invoked with a map of
     *                          attribute name and values.
     * @throws OcException if failure
     */
    public void post(String resourceType,
                     String resourceInterface,
                     OcRepresentation ocRepresentation,
                     Map<String, String> queryParamsMap,
                     OnPostListener onPostListener) throws OcException {
        this.post2(
                resourceType,
                resourceInterface,
                ocRepresentation,
                queryParamsMap,
                onPostListener);
    }

    private native void post2(String resourceType,
                              String resourceInterface,
                              OcRepresentation ocRepresentation,
                              Map<String, String> queryParamsMap,
                              OnPostListener onPostListener) throws OcException;

    /**
     * Method to POST on a resource
     *
     * @param resourceType      resource type of the resource to operate on
     * @param resourceInterface interface type of the resource to operate on
     * @param ocRepresentation  representation of the resource
     * @param queryParamsMap    Map which can have the query parameter name and value
     * @param onPostListener    event handler The event handler will be invoked with a map of
     *                          attribute name and values.
     * @param qualityOfService  the quality of communication
     * @throws OcException Failed to POST.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public void post(String resourceType,
                     String resourceInterface,
                     OcRepresentation ocRepresentation,
                     Map<String, String> queryParamsMap,
                     OnPostListener onPostListener,
                     QualityOfService qualityOfService) throws OcException {

        if (qualityOfService == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "qualityOfService cannot be null");
        }

        this.post3(
                resourceType,
                resourceInterface,
                ocRepresentation,
                queryParamsMap,
                onPostListener,
                qualityOfService.getValue());
    }

    private native void post3(String resourceType,
                              String resourceInterface,
                              OcRepresentation ocRepresentation,
                              Map<String, String> queryParamsMap,
                              OnPostListener onPostListener,
                              int qualityOfService) throws OcException;

    /**
     * Method to perform DELETE operation
     *
     * @param onDeleteListener event handler The event handler will have headerOptionList
     *
     * @throws OcException Failed to perform DELETE operation.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public native void deleteResource(OnDeleteListener onDeleteListener) throws OcException;

    /**
     * Method to perform DELETE operation
     *
     * @param onDeleteListener event handler The event handler will have headerOptionList
     * @param qualityOfService the quality of communication
     *
     * @throws OcException Failed to perform DELETE operation.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public void deleteResource(OnDeleteListener onDeleteListener,
                               QualityOfService qualityOfService) throws OcException {

        if (qualityOfService == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "qualityOfService cannot be null");
        }

        this.deleteResource1(onDeleteListener,
                qualityOfService.getValue());
    }

    private native void deleteResource1(OnDeleteListener onDeleteListener,
                                        int qualityOfService) throws OcException;

    /**
     * Method to set observation on the resource
     *
     * @param observeType       allows the client to specify how it wants to observe
     * @param queryParamsMap    map which can have the query parameter name and value
     * @param onObserveListener event handler The handler method will be invoked with a map
     *                          of attribute name and values.
     * @throws OcException Failed to set observation.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public void observe(ObserveType observeType,
                        Map<String, String> queryParamsMap,
                        OnObserveListener onObserveListener) throws OcException {

        if (observeType == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "observeType cannot be null");
        }

        this.observe(
                observeType.getValue(),
                queryParamsMap,
                onObserveListener);
    }

    private synchronized native void observe(int observeType,
                                             Map<String, String> queryParamsMap,
                                             OnObserveListener onObserveListener) throws OcException;

    /**
     * Method to set observation on the resource
     *
     * @param observeType       allows the client to specify how it wants to observe
     * @param queryParamsMap    map which can have the query parameter name and value
     * @param onObserveListener event handler The handler method will be invoked with a map
     *                          of attribute name and values.
     * @param qualityOfService  the quality of communication
     * @throws OcException Failed to set observation.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public void observe(ObserveType observeType,
                        Map<String, String> queryParamsMap,
                        OnObserveListener onObserveListener,
                        QualityOfService qualityOfService) throws OcException {

        if (observeType == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "observeType cannot be null");
        }

        if (qualityOfService == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "qualityOfService cannot be null");
        }

        this.observe1(
                observeType.getValue(),
                queryParamsMap,
                onObserveListener,
                qualityOfService.getValue());
    }

    private synchronized native void observe1(int observeType,
                                              Map<String, String> queryParamsMap,
                                              OnObserveListener onObserveListener,
                                              int qualityOfService) throws OcException;

    /**
     * Method to cancel the observation on the resource
     *
     * @throws OcException Failed to cancel observation.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public void cancelObserve() throws OcException{
        this.cancelObserve(OcPlatform.getPlatformQualityOfService());
    }

    /**
     * Method to cancel the observation on the resource
     *
     * @param qualityOfService the quality of communication
     * @throws OcException Failed to cancel observation.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public void cancelObserve(QualityOfService qualityOfService) throws OcException {

        if (qualityOfService == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "qualityOfService cannot be null");
        }

        this.cancelObserve1(qualityOfService.getValue());
    }

    private native void cancelObserve1(int qualityOfService) throws OcException;

    /**
     * Method to set header options
     *
     * @param headerOptionList List OcHeaderOption where header information(header optionID and
     *                         optionData is passed
     * @throws OcException Failed to set header options.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public void setHeaderOptions(List<OcHeaderOption> headerOptionList) throws OcException {

        if (headerOptionList == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "headerOptionList cannot be null");
        }

        this.setHeaderOptions(headerOptionList.toArray(
                        new OcHeaderOption[headerOptionList.size()])
        );
    }

    private native void setHeaderOptions(OcHeaderOption[] headerOptionList);

    /**
     * Method to unset header options
     */
    public native void unsetHeaderOptions();

    /**
     * Method to get the host address of this resource
     *
     * @return host address NOTE: This might or might not be exposed in future due to
     * security concerns
     */
    public native String getHost();

    /**
     * Method to get the endpoints information of this resource
     *
     * @return List of endpoints information
     */
    public native List<String> getAllHosts();

    /**
     * Method to get the URI for this resource
     *
     * @return resource URI
     */
    public native String getUri();

    /**
     * Method to get the connectivity type of this resource
     *
     * @return EnumSet OcConnectivityType connectivity type set
     */
    public EnumSet<OcConnectivityType> getConnectivityTypeSet() {
        return OcConnectivityType.convertToEnumSet(
                this.getConnectivityTypeN()
        );
    }

    private native int getConnectivityTypeN();

    /**
     * Method to provide ability to check if this resource is observable or not
     *
     * @return true indicates resource is observable; false indicates resource is not observable
     */
    public native boolean isObservable();

    /**
     * Function to change host of this reource
     * @param host host address to change to
     * @return New host Address.
     *         Not observable.
     */
    public native String setHost(String host);

    /**
     * Method to get the list of resource types
     *
     * @return List of resource types
     */
    public native List<String> getResourceTypes();

    /**
     * Method to get the list of resource interfaces
     *
     * @return List of resource interface
     */
    public native List<String> getResourceInterfaces();

    /**
     * Method to get a unique identifier for this resource across network interfaces.  This will
     * be guaranteed unique for every resource-per-server independent of how this was discovered.
     *
     * @return OcResourceIdentifier object, which can be used for all comparison and hashing
     */
    public native OcResourceIdentifier getUniqueIdentifier();

    /**
     * Method to get a string representation of the resource's server ID.
     * <p>
     * This is unique per- server independent on how it was discovered.
     * </p>
     *
     * @return server ID
     */
    public native String getServerId();

    /**
     * An OnGetListener can be registered via the resource get call.
     * Event listeners are notified asynchronously
     */
    public interface OnGetListener {
        public void onGetCompleted(List<OcHeaderOption> headerOptionList,
                                   OcRepresentation ocRepresentation);

        public void onGetFailed(Throwable ex);
    }

    /**
     * An OnPutListener can be registered via the resource put call.
     * Event listeners are notified asynchronously
     */
    public interface OnPutListener {
        public void onPutCompleted(List<OcHeaderOption> headerOptionList,
                                   OcRepresentation ocRepresentation);

        public void onPutFailed(Throwable ex);
    }

    /**
     * An OnPostListener can be registered via the resource post call.
     * Event listeners are notified asynchronously
     */
    public interface OnPostListener {
        public void onPostCompleted(List<OcHeaderOption> headerOptionList,
                                    OcRepresentation ocRepresentation);

        public void onPostFailed(Throwable ex);
    }

    /**
     * An OnDeleteListener can be registered via the resource delete call.
     * Event listeners are notified asynchronously
     */
    public interface OnDeleteListener {
        public void onDeleteCompleted(List<OcHeaderOption> headerOptionList);

        public void onDeleteFailed(Throwable ex);
    }

    /**
     * An OnObserveListener can be registered via the resource observe call.
     * Event listeners are notified asynchronously
     */
    public interface OnObserveListener {
        /**
         * To Register.
         */
        public static final int REGISTER = 0;
        /**
         * To Deregister.
         */
        public static final int DEREGISTER = 1;
        /**
         * Others.
         */
        public static final int NO_OPTION = 2;

        /**
         * Maximum sequence number.
         * A successful deregister will have a sequence number
         * of MAX_SEQUENCE_NUMBER + 1.
         */
        public static final int MAX_SEQUENCE_NUMBER = 0xffffff;

        public void onObserveCompleted(List<OcHeaderOption> headerOptionList,
                                       OcRepresentation ocRepresentation,
                                       int sequenceNumber);

        public void onObserveFailed(Throwable ex);
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();

        dispose();
    }

    private native void dispose();

    private long mNativeHandle;

    /**
     * Method to discovery Topics
     *
     * @param queryParamsMap    map which can have the query parameter name and value
     * @param onTopicFoundListener    event handler The handler method will be invoked with a map
     *                                of attribute name and values.
     * @param qualityOfService the quality of communication.
     * @throws OcException Failed to discover Topic.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public void discoveryMQTopics(Map<String, String> queryParamsMap,
                                  OnMQTopicFoundListener onTopicFoundListener,
                                  QualityOfService qualityOfService) throws OcException {

        if (qualityOfService == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "qualityOfService cannot be null");
        }

        this.discoveryMQTopicsImpl(queryParamsMap, onTopicFoundListener,
                                   qualityOfService.getValue());
    }

    private synchronized native void discoveryMQTopicsImpl(
            Map<String, String> queryParamsMap,
            OnMQTopicFoundListener onTopicFoundListener,
            int qualityOfService) throws OcException;

    /**
     * Method to create Topic into MQ Broker.
     *
     * @param ocRepresentation  representation of the MQ Broker.
     * @param uri               new MQ Topic uri which want to create.
     * @param queryParamsMap    map which can have the query parameter name and value.
     * @param onTopicCreatedListener    event handler The handler method will be invoked with a map
     *                                  of attribute name and values.
     * @param qualityOfService the quality of communication.
     * @throws OcException Failed to create Topic.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public void createMQTopic(OcRepresentation ocRepresentation,
                              String uri,
                              Map<String, String> queryParamsMap,
                              OnMQTopicCreatedListener onTopicCreatedListener,
                              QualityOfService qualityOfService) throws OcException {

        if (qualityOfService == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "qualityOfService cannot be null");
        }

        this.createMQTopicImpl(ocRepresentation, uri, queryParamsMap,
                               onTopicCreatedListener, qualityOfService.getValue());
    }

    private synchronized native void createMQTopicImpl(
            OcRepresentation ocRepresentation,
            String uri,
            Map<String, String> queryParamsMap,
            OnMQTopicCreatedListener onTopicCreatedListener,
            int qualityOfService) throws OcException;

    /**
     * Method to set subscribe on the Topic.
     *
     * @param queryParamsMap    map which can have the query parameter name and value.
     * @param onObserveListener event handler The handler method will be invoked with a map
     *                          of attribute name and values.
     * @param qualityOfService the quality of communication.
     * @throws OcException Failed to subscribe on the Topic.
     *                     Use OcException.GetErrorCode() for more details.
     * 
     */
    public void subscribeMQTopic(Map<String, String> queryParamsMap,
                                 OnObserveListener onObserveListener,
                                 QualityOfService qualityOfService) throws OcException {

        if (qualityOfService == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "qualityOfService cannot be null");
        }

        this.subscribeMQTopicImpl(queryParamsMap,
                                  onObserveListener,
                                  qualityOfService.getValue());
    }

    private synchronized native void subscribeMQTopicImpl(Map<String, String> queryParamsMap,
            OnObserveListener onObserveListener,
            int qualityOfService) throws OcException;

    /**
     * Method to cancel the observation on the Topic.
     *
     * @param qualityOfService the quality of communication.
     * @throws OcException Failed to cancel MQTopic subscription.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public void unsubscribeMQTopic(QualityOfService qualityOfService) throws OcException{

        if (qualityOfService == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "qualityOfService cannot be null");
        }

        this.unsubscribeMQTopicImpl(qualityOfService.getValue());
    }

    private native void unsubscribeMQTopicImpl(
            int qualityOfService) throws OcException;

    /**
     * Method to requestMQPublish on a Topic
     *
     * @param queryParamsMap   Map which can have the query parameter name and value
     * @param onPostListener   event handler The event handler will be invoked with a map of
     *                         attribute name and values.
     * @param qualityOfService the quality of communication.
     * @throws OcException Request failure.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public void requestMQPublish(Map<String, String> queryParamsMap,
                     OnPostListener onPostListener,
                     QualityOfService qualityOfService) throws OcException {

        if (qualityOfService == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "qualityOfService cannot be null");
        }

        this.requestMQPublishImpl(queryParamsMap,
                                  onPostListener,
                                  qualityOfService.getValue());
    }

    private native void requestMQPublishImpl(Map<String, String> queryParamsMap,
                              OnPostListener onPostListener,
                              int qualityOfService) throws OcException;

    /**
     * Method to publishMQTopic on a Topic
     *
     * @param ocRepresentation representation of the resource
     * @param queryParamsMap   Map which can have the query parameter name and value
     * @param onPostListener   event handler The event handler will be invoked with a map of
     *                         attribute name and values.
     * @param qualityOfService the quality of communication.
     * @throws OcException Failed to publish MQTopic.
     *                     Use OcException.GetErrorCode() for more details.
     */
    public void publishMQTopic(OcRepresentation ocRepresentation,
                     Map<String, String> queryParamsMap,
                     OnPostListener onPostListener,
                     QualityOfService qualityOfService) throws OcException {

        if (qualityOfService == null) {
            throw new OcException(ErrorCode.INVALID_PARAM, "qualityOfService cannot be null");
        }

        this.publishMQTopicImpl(ocRepresentation,
                                queryParamsMap,
                                onPostListener,
                                qualityOfService.getValue());
    }

    private native void publishMQTopicImpl(OcRepresentation ocRepresentation,
                              Map<String, String> queryParamsMap,
                              OnPostListener onPostListener,
                              int qualityOfService) throws OcException;

    /**
     * An OnMQTopicFoundListener can be registered via the OcResource.discoveryMQTopics call.
     * Event listeners are notified asynchronously
     */
    public interface OnMQTopicFoundListener {
        public void onTopicDiscoveried(OcResource resource);
        public void onDiscoveryTopicFailed(Throwable ex, String uri);
    }

    /**
     * An OnMQTopicCreatedListener can be registered via the OcResource.createMQTopic call.
     * Event listeners are notified asynchronously
     */
    public interface OnMQTopicCreatedListener {
        public void onTopicResourceCreated(OcResource resource);
        public void onCreateTopicFailed(Throwable ex, String uri);
    }

    /**
     * An OnMQTopicSubscribeListener can be registered via the OcResource.subscribeMQTopic call.
     * Event listeners are notified asynchronously
     */
    public interface OnMQTopicSubscribeListener {
        /**
         * To Subscriber.
         */
        public static final int SUBSCRIBER = 0;
        /**
         * To Unrubscriber.
         */
        public static final int UNSUBSCRIBER = 1;
        /**
         * Others.
         */
        public static final int NO_OPTION = 2;
        public void onSubScribeCompleted(List<OcHeaderOption> headerOptionList,
                OcRepresentation ocRepresentation,
                int sequenceNumber);
        public void onUnsubScribeCompleted(OcRepresentation ocRepresentation,
                int sequenceNumber);
        public void onSubScribeFailed(Throwable ex);
    }
}
