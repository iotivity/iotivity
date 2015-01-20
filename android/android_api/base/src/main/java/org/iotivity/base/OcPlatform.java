/*
 * //******************************************************************
 * //
 * // Copyright 2015 Intel Corporation.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base;

import java.util.EnumSet;
import java.util.Iterator;
import java.util.List;

/**
 * Contains the main entrance/functionality of the product. To set a custom configuration, the
 * implementer must make a call to OcPlatform.Configure before the first usage of a function in this
 * class.
 */
public final class OcPlatform {

    static {
        System.loadLibrary("oc_logger");
        System.loadLibrary("octbstack");
        System.loadLibrary("coap");
        System.loadLibrary("oc");
        System.loadLibrary("ocstack-jni");
    }

    public static final String DEFAULT_INTERFACE = "oc.mi.def";
    public static final String LINK_INTERFACE = "oc.mi.ll";
    public static final String BATCH_INTERFACE = "oc.mi.b";
    public static final String GROUP_INTERFACE = "oc.mi.grp";

    private OcPlatform() {
    }

    /**
     * API for setting the configuration of the OcPlatform.
     * Note: Any calls made to this AFTER the first call to OcPlatform.Configure will have no affect
     *
     * @param platformConfig platform configuration
     */
    public static void Configure(PlatformConfig platformConfig) {
        OcPlatform.configure(
                platformConfig.getServiceType().getValue(),
                platformConfig.getModeType().getValue(),
                platformConfig.getIpAddress(),
                platformConfig.getPort(),
                platformConfig.getQualityOfService().getValue()
        );
    }

    private static native void configure(int serviceType,
                                         int modeType,
                                         String ipAddress,
                                         int port,
                                         int qualityOfService);

    /**
     * API for notifying base that resource's attributes have changed.
     *
     * @param ocResourceHandle resource handle of the resource
     * @throws OcException
     */
    public static native void notifyAllObservers(
            OcResourceHandle ocResourceHandle) throws OcException;

    /**
     * API for notifying base that resource's attributes have changed.
     *
     * @param ocResourceHandle resource handle of the resource
     * @param qualityOfService the quality of communication
     * @throws OcException
     */
    public static void notifyAllObservers(
            OcResourceHandle ocResourceHandle,
            QualityOfService qualityOfService) throws OcException {
        OcPlatform.notifyAllObservers1(ocResourceHandle, qualityOfService.getValue());
    }

    private static native void notifyAllObservers1(
            OcResourceHandle ocResourceHandle,
            int qualityOfService) throws OcException;

    /**
     * API for notifying only specific clients that resource's attributes have changed.
     *
     * @param ocResourceHandle    resource handle of the resource
     * @param ocObservationIdList These set of ids are ones which which will be notified upon
     *                            resource change.
     * @param ocResourceResponse  OcResourceResponse object used by app to fill the response for
     *                            this resource change
     * @throws OcException
     */
    public static void notifyListOfObservers(
            OcResourceHandle ocResourceHandle,
            List<Byte> ocObservationIdList,
            OcResourceResponse ocResourceResponse) throws OcException {
        byte[] idArr = new byte[ocObservationIdList.size()];
        Iterator<Byte> it = ocObservationIdList.iterator();
        int i = 0;
        while (it.hasNext()) {
            idArr[i++] = (byte) it.next();
        }

        OcPlatform.notifyListOfObservers2(
                ocResourceHandle,
                idArr,
                ocResourceResponse);
    }

    private static native void notifyListOfObservers2(
            OcResourceHandle ocResourceHandle,
            byte[] ocObservationIdArray,
            OcResourceResponse ocResourceResponse) throws OcException;

    /**
     * API for notifying only specific clients that resource's attributes have changed.
     *
     * @param ocResourceHandle    resource handle of the resource
     * @param ocObservationIdList These set of ids are ones which which will be notified upon
     *                            resource change.
     * @param ocResourceResponse  OcResourceResponse object used by app to fill the response for
     *                            this resource change
     * @param qualityOfService    the quality of communication
     * @throws OcException
     */
    public static void notifyListOfObservers(
            OcResourceHandle ocResourceHandle,
            List<Byte> ocObservationIdList,
            OcResourceResponse ocResourceResponse,
            QualityOfService qualityOfService) throws OcException {
        byte[] idArr = new byte[ocObservationIdList.size()];
        Iterator<Byte> it = ocObservationIdList.iterator();
        int i = 0;
        while (it.hasNext()) {
            idArr[i++] = (byte) it.next();
        }

        OcPlatform.notifyListOfObservers3(
                ocResourceHandle,
                idArr,
                ocResourceResponse,
                qualityOfService.getValue()
        );
    }

    private static native void notifyListOfObservers3(
            OcResourceHandle ocResourceHandle,
            byte[] ocObservationIdArray,
            OcResourceResponse ocResourceResponse,
            int qualityOfService) throws OcException;

    /**
     * API for Service and Resource Discovery. NOTE: This API applies to client side only
     *
     * @param host                    Host IP Address of a service to direct resource discovery query.
     *                                If empty, performs multicast resource discovery query
     * @param resourceUri             name of the resource. If null or empty, performs search for all
     *                                resource names
     * @param onResourceFoundListener Handles events, success states and failure states.
     * @throws OcException
     */
    public static native void findResource(
            String host,
            String resourceUri,
            OnResourceFoundListener onResourceFoundListener) throws OcException;


    /**
     * API for Service and Resource Discovery. NOTE: This API applies to client side only
     *
     * @param host                    Host IP Address of a service to direct resource discovery query.
     *                                If empty, performs multicast resource discovery query
     * @param resourceUri             name of the resource. If null or empty, performs search for all
     *                                resource names
     * @param onResourceFoundListener Handles events, success states and failure states.
     * @param qualityOfService        the quality of communication
     * @throws OcException
     */
    public static void findResource(
            String host,
            String resourceUri,
            OnResourceFoundListener onResourceFoundListener,
            QualityOfService qualityOfService) throws OcException {
        OcPlatform.findResource1(host,
                resourceUri,
                onResourceFoundListener,
                qualityOfService.getValue()
        );
    }

    private static native void findResource1(
            String host,
            String resourceUri,
            OnResourceFoundListener onResourceFoundListener,
            int qualityOfService) throws OcException;

    /**
     * API for Device Discovery
     *
     * @param host                  Host IP Address. If null or empty, Multicast is performed.
     * @param deviceUri             Uri containing address to the virtual device
     * @param onDeviceFoundListener Handles events, success states and failure states.
     * @throws OcException
     */
    public static native void getDeviceInfo(
            String host,
            String deviceUri,
            OnDeviceFoundListener onDeviceFoundListener) throws OcException;

    /**
     * API for Device Discovery
     *
     * @param host                  Host IP Address. If null or empty, Multicast is performed.
     * @param deviceUri             Uri containing address to the virtual device
     * @param onDeviceFoundListener Handles events, success states and failure states.
     * @param qualityOfService      the quality of communication
     * @throws OcException
     */
    public static void getDeviceInfo(
            String host,
            String deviceUri,
            OnDeviceFoundListener onDeviceFoundListener,
            QualityOfService qualityOfService) throws OcException {
        OcPlatform.getDeviceInfo1(
                host,
                deviceUri,
                onDeviceFoundListener,
                qualityOfService.getValue()
        );
    }

    private static native void getDeviceInfo1(
            String host,
            String deviceUri,
            OnDeviceFoundListener onDeviceFoundListener,
            int qualityOfService) throws OcException;

    /**
     * This API registers a resource with the server NOTE: This API applies to server side only.
     *
     * @param ocResource The instance of OcResource with all data filled
     * @return resource handle
     * @throws OcException
     */
    public static native OcResourceHandle registerResource(
            OcResource ocResource) throws OcException;

    /**
     * This API registers a resource with the server NOTE: This API applies to server side only.
     *
     * @param resourceUri         The URI of the resource. Example: "a/light"
     * @param resourceTypeName    The resource type. Example: "light"
     * @param resourceInterface   The resource interface (whether it is collection etc).
     * @param entityHandler       entity handler.
     * @param resourcePropertySet indicates the property of the resource
     * @return resource handle
     * @throws OcException
     */
    public static OcResourceHandle registerResource(
            String resourceUri,
            String resourceTypeName,
            String resourceInterface,
            EntityHandler entityHandler,
            EnumSet<ResourceProperty> resourcePropertySet) throws OcException {
        int resProperty = 0;

        for (ResourceProperty prop : ResourceProperty.values()) {
            if (resourcePropertySet.contains(prop))
                resProperty |= prop.getValue();
        }

        return OcPlatform.registerResource1(resourceUri,
                resourceTypeName,
                resourceInterface,
                entityHandler,
                resProperty);
    }

    private static native OcResourceHandle registerResource1(
            String resourceUri,
            String resourceTypeName,
            String resourceInterface,
            EntityHandler entityHandler,
            int resourceProperty) throws OcException;

    /**
     * Register Device Info
     *
     * @param ocDeviceInfo object containing all the device specific information
     * @throws OcException
     */
    public static void registerDeviceInfo(
            OcDeviceInfo ocDeviceInfo) throws OcException {

        OcPlatform.registerDeviceInfo(
                ocDeviceInfo.getDeviceName(),
                ocDeviceInfo.getHostName(),
                ocDeviceInfo.getDeviceUuid(),
                ocDeviceInfo.getContentType(),
                ocDeviceInfo.getVersion(),
                ocDeviceInfo.getManufacturerName(),
                ocDeviceInfo.getManufacturerUrl(),
                ocDeviceInfo.getModelNumber(),
                ocDeviceInfo.getDateOfManufacture(),
                ocDeviceInfo.getPlatformVersion(),
                ocDeviceInfo.getFirmwareVersion(),
                ocDeviceInfo.getSupportUrl()
        );
    }

    private static native void registerDeviceInfo(
            String deviceName,
            String hostName,
            String deviceUUID,
            String contentType,
            String version,
            String manufacturerName,
            String manufacturerUrl,
            String modelNumber,
            String dateOfManufacture,
            String platformVersion,
            String firmwareVersion,
            String supportUrl) throws OcException;

    /**
     * This API unregisters a resource with the server NOTE: This API applies to server side only.
     *
     * @param ocResourceHandle This is the resource handle which we which to unregister from the
     *                         server
     * @throws OcException
     */
    public static native void unregisterResource(
            OcResourceHandle ocResourceHandle) throws OcException;

    /**
     * Add a resource to a collection resource
     *
     * @param ocResourceCollectionHandle handle to the collection resource
     * @param ocResourceHandle           handle to resource to be added to the collection resource
     * @throws OcException
     */
    public static native void bindResource(
            OcResourceHandle ocResourceCollectionHandle,
            OcResourceHandle ocResourceHandle) throws OcException;

    /**
     * Add multiple resources to a collection resource.
     *
     * @param ocResourceCollectionHandle handle to the collection resource
     * @param ocResourceHandleList       reference to list of resource handles to be added to the
     *                                   collection resource
     * @throws OcException
     */
    public static void bindResources(
            OcResourceHandle ocResourceCollectionHandle,
            List<OcResourceHandle> ocResourceHandleList) throws OcException {
        OcPlatform.bindResources(
                ocResourceCollectionHandle,
                ocResourceHandleList.toArray(
                        new OcResourceHandle[ocResourceHandleList.size()])
        );
    }

    private static native void bindResources(
            OcResourceHandle ocResourceCollectionHandle,
            OcResourceHandle[] ocResourceHandleArray) throws OcException;

    /**
     * Unbind a resource from a collection resource.
     *
     * @param ocResourceCollectionHandle handle to the collection resource
     * @param ocResourceHandle           resource handle to be unbound from the collection resource
     * @throws OcException
     */
    public static native void unbindResource(
            OcResourceHandle ocResourceCollectionHandle,
            OcResourceHandle ocResourceHandle) throws OcException;

    /**
     * Unbind resources from a collection resource.
     *
     * @param ocResourceCollectionHandle Handle to the collection resource
     * @param ocResourceHandleList       List of resource handles to be unbound from the collection
     *                                   resource
     * @throws OcException
     */
    public static void unbindResources(
            OcResourceHandle ocResourceCollectionHandle,
            List<OcResourceHandle> ocResourceHandleList) throws OcException {
        OcPlatform.unbindResources(
                ocResourceCollectionHandle,
                ocResourceHandleList.toArray(
                        new OcResourceHandle[ocResourceHandleList.size()])
        );
    }

    private static native void unbindResources(
            OcResourceHandle ocResourceCollectionHandle,
            OcResourceHandle[] ocResourceHandleArray) throws OcException;

    /**
     * Binds a type to a particular resource
     *
     * @param ocResourceHandle handle to the resource
     * @param resourceTypeName new typename to bind to the resource
     * @throws OcException
     */
    public static native void bindTypeToResource(
            OcResourceHandle ocResourceHandle,
            String resourceTypeName) throws OcException;

    /**
     * Binds an interface to a particular resource
     *
     * @param ocResourceHandle      handle to the resource
     * @param resourceInterfaceName new interface to bind to the resource
     * @throws OcException
     */
    public static native void bindInterfaceToResource(
            OcResourceHandle ocResourceHandle,
            String resourceInterfaceName) throws OcException;

    /**
     * Start Presence announcements.
     *
     * @param ttl time to live in seconds
     * @throws OcException
     */
    public static native void startPresence(int ttl) throws OcException;

    /**
     * Stop Presence announcements.
     *
     * @throws OcException
     */
    public static native void stopPresence() throws OcException;

    /**
     * Subscribes to a server's presence change events. By making this subscription, every time a
     * server adds/removes/alters a resource, starts or is intentionally stopped
     *
     * @param host               The IP address/addressable name of the server to subscribe to
     * @param onPresenceListener listener that will receive notifications/subscription events
     * @return a handle object that can be used to identify this subscription request. It can be
     * used to unsubscribe from these events in the future
     * @throws OcException
     */
    public static native OcPresenceHandle subscribePresence(
            String host,
            OnPresenceListener onPresenceListener) throws OcException;

    /**
     * Subscribes to a server's presence change events. By making this subscription, every time a
     * server adds/removes/alters a resource, starts or is intentionally stopped
     *
     * @param host               The IP address/addressable name of the server to subscribe to
     * @param resourceType       a resource type specified as a filter for subscription events.
     * @param onPresenceListener listener that will receive notifications/subscription events
     * @return a handle object that can be used to identify this subscription request. It can be
     * used to unsubscribe from these events in the future
     * @throws OcException
     */
    public static OcPresenceHandle subscribePresence(
            String host,
            String resourceType,
            OnPresenceListener onPresenceListener) throws OcException {

        return OcPlatform.subscribePresence1(
                host,
                resourceType,
                onPresenceListener);
    }

    private static native OcPresenceHandle subscribePresence1(
            String host,
            String resourceType,
            OnPresenceListener onPresenceListener) throws OcException;

    /**
     * Unsubscribes from a previously subscribed server's presence events. Note that you may for
     * a short time still receive events from the server since it may take time for the
     * unsubscribe to take effect.
     *
     * @param ocPresenceHandle the handle object provided by the subscribePresence call that
     *                         identifies this subscription
     * @throws OcException
     */
    public static native void unsubscribePresence(
            OcPresenceHandle ocPresenceHandle) throws OcException;

    /**
     * Creates a resource proxy object so that get/put/observe functionality can be used without
     * discovering the object in advance. Note that the consumer of this method needs to provide
     * all of the details required to correctly contact and observe the object. If the consumer
     * lacks any of this information, they should discover the resource object normally.
     * Additionally, you can only create this object if OcPlatform was initialized to be a Client
     * or Client/Server.
     *
     * @param host             a string containing a resolvable host address of the server holding
     *                         the resource
     * @param uri              the rest of the resource's URI that will permit messages to be
     *                         properly routed.
     *                         Example: /a/light
     * @param isObservable     a boolean containing whether the resource supports observation
     * @param resourceTypeList a collection of resource types implemented by the resource
     * @param interfaceList    a collection of interfaces that the resource supports/implements
     * @return new resource object
     * @throws OcException
     */
    public static OcResource constructResourceObject(
            String host,
            String uri,
            boolean isObservable,
            List<String> resourceTypeList,
            List<String> interfaceList) throws OcException {
        return OcPlatform.constructResourceObject(
                host,
                uri,
                isObservable,
                resourceTypeList.toArray(new String[resourceTypeList.size()]),
                interfaceList.toArray(new String[interfaceList.size()])
        );
    }

    private static native OcResource constructResourceObject(
            String host,
            String uri,
            boolean isObservable,
            String[] resourceTypes,
            String[] interfaces) throws OcException;

    /**
     * Allows application entity handler to send response to an incoming request.
     *
     * @param ocResourceResponse resource response
     * @throws OcException
     */
    public static native void sendResponse(OcResourceResponse ocResourceResponse) throws OcException;

    /**
     * An OnResourceFoundListener can be registered via the OcPlatform.findResource call.
     * Event listeners are notified asynchronously
     */
    public interface OnResourceFoundListener {
        public void onResourceFound(OcResource resource);
    }

    /**
     * An OnDeviceFoundListener can be registered via the OcPlatform.getDeviceInfo call.
     * Event listeners are notified asynchronously
     */
    public interface OnDeviceFoundListener {
        public void onDeviceFound(OcRepresentation ocRepresentation);
    }

    /**
     * An OnPresenceListener can be registered via the OcPlatform.subscribePresence call.
     * Event listeners are notified asynchronously
     */
    public interface OnPresenceListener {
        public void onPresence(OcPresenceStatus ocPresenceStatus, int nonce, String hostAddress);
    }

    /**
     * An EntityHandler can be registered via the OcPlatform.registerResource call.
     * Event listeners are notified asynchronously
     */
    public interface EntityHandler {
        public EntityHandlerResult handleEntity(OcResourceRequest ocResourceRequest);
    }
}