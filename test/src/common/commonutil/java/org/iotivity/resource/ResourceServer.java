/******************************************************************
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 * <p/>
 * <p/>
 * <p/>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p/>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p/>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************/

package org.iotivity.resource;

import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.OcDeviceInfo;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPlatformInfo;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.OcResourceResponse;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.RequestHandlerFlag;
import org.iotivity.base.RequestType;
import org.iotivity.base.ResourceProperty;
import org.iotivity.common.ResourceConstants;
import org.iotivity.common.ResourceHelper;
import org.iotivity.service.testapp.framework.Base;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.EnumSet;
import java.util.List;
import java.util.Map;
import java.util.Vector;

public abstract class ResourceServer {
    static boolean m_isServerConstructed = false;
    private static OcPlatformInfo m_platformInfo;
    private static OcDeviceInfo m_deviceInfo;
    static ResourceHelper m_resourceHelper = ResourceHelper.getInstance();
    OcRepresentation m_representation = new OcRepresentation();
    String m_resourceURI = "";
    String m_resourceTypeName = "";
    Vector<String> m_resourceTypeNames = new Vector<String>();
    String m_resourceInterface = ResourceConstants.DEFAULT_INTERFACE;
    Vector<String> m_resourceInterfaces = new Vector<String>();
    OcResourceHandle m_resourceHandle = null;
    boolean m_isServerRunning = false;
    boolean m_isSlowResource = false;
    boolean m_isPresenceStarted = false;
    EnumSet<ResourceProperty> m_resourceProperty;

    public ResourceServer() {
        m_resourceProperty.add(ResourceProperty.ACTIVE);
        m_resourceTypeNames.clear();
        m_resourceInterfaces.clear();
    }

    public abstract void onResourceServerStarted(
            boolean isRegisteredForPresence, int presenceInterval);

    public abstract void handleGetRequest(Map<String, String> queryParamsMap,
                                          OcResourceRequest request, OcResourceResponse response);

    public abstract void handlePutRequest(Map<String, String> queryParamsMap,
                                          OcRepresentation incomingRepresentation, OcResourceRequest request,
                                          OcResourceResponse response);

    public abstract void handlePostRequest(Map<String, String> queryParamsMap,
                                           OcRepresentation incomingRepresentation, OcResourceRequest request,
                                           OcResourceResponse response);

    public abstract void handleDeleteRequest(Map<String, String> queryParamsMap,
                                             OcRepresentation incomingRepresentation, OcResourceRequest request,
                                             OcResourceResponse response);

    public abstract void handleObserveRequest(
            Map<String, String> queryParamsMap, OcResourceRequest request,
            OcResourceResponse response);

    public static boolean constructServer(PlatformConfig cfg) {
        boolean result = true;
        try {
            OcPlatform.Configure(cfg);
            m_isServerConstructed = true;

            setPlatformInfo(ResourceConstants.PLATFORM_ID,
                    ResourceConstants.MANUFACTURER_NAME,
                    ResourceConstants.MANUFACTURER_URL,
                    ResourceConstants.MODEL_NUMBER,
                    ResourceConstants.DATE_OF_MANUFACTURE,
                    ResourceConstants.PLATFORM_VERSION,
                    ResourceConstants.OPERATING_SYSTEM_VERSION,
                    ResourceConstants.HARDWARE_VERSION,
                    ResourceConstants.FIRMWARE_VERSION,
                    ResourceConstants.SUPPORT_URL,
                    ResourceConstants.SYSTEM_TIME);

            m_deviceInfo = new OcDeviceInfo(ResourceConstants.DEFAULT_DEVICE_NAME,
                    Arrays.asList(new String[]{
                            ResourceConstants.DEVICE_TYPE_DEFAULT,
                            ResourceConstants.DEVICE_TYPE_AC}));

            OcPlatform.registerPlatformInfo(m_platformInfo);

            OcPlatform.registerDeviceInfo(m_deviceInfo);

            System.out.println("Server Created...");
        } catch (OcException e) {
            result = false;
            System.out.println(
                    "Error occurred while creating server, error code: ");
            e.printStackTrace();
        }

        return result;
    }

    public OcRepresentation getRepresentation() {
        return m_representation;
    }

    public OcResourceHandle getResourceHandle() {
        return m_resourceHandle;
    }

    public boolean setResourceProperties(String resourceUri,
                                         String resourceTypeName, String resourceInterface) {
        m_resourceURI = resourceUri;
        m_resourceTypeName = resourceTypeName;
        m_resourceInterface = resourceInterface;

        boolean result = true;

        if ((m_resourceURI == "") || (m_resourceTypeName == "")
                || (m_resourceInterface == "")) {
            result = false;
        }

        return result;
    }

    public boolean startResource(EnumSet<ResourceProperty> resourceProperty) {
        m_resourceProperty.addAll(resourceProperty);
        return startResource();
    }

    public boolean startResource() {
        boolean result = true;

        if (isObservableResource()) {
            System.out.println("This resource is Observable");
        }
        if (isDiscoverableResource()) {
            System.out.println("This resource is Discoverable");
        }
        if (isSecuredResource()) {
            System.out.println("This resource is Secured");
        }

        m_resourceTypeNames = new Vector<String>(
                Arrays.asList(m_resourceTypeName.split(" ")));
        m_resourceTypeName = m_resourceTypeNames.get(0);

        m_resourceInterfaces = new Vector<String>(
                Arrays.asList(m_resourceInterface.split(" ")));
        m_resourceInterface = m_resourceInterfaces.get(0);

        OcPlatform.EntityHandler entityHandler = new OcPlatform.EntityHandler() {

            public EntityHandlerResult handleEntity(
                    final OcResourceRequest request) {
                EntityHandlerResult result = EntityHandlerResult.OK;

                System.out.println("\tIn Server entity handler:");

                if (request != null) {
                    if (m_isSlowResource) {
                        Thread thread = new Thread(new Runnable() {

                            public void run() {
                                handleSlowResponse(request);
                            }

                        });

                        thread.start();

                        result = EntityHandlerResult.SLOW;
                    } else {
                        handleResponse(request);
                        result = EntityHandlerResult.OK;
                    }
                } else {
                    System.out.println("Request invalid");
                }

                return result;

            }
        };

        try {
            // This will internally create and register the resource.
            m_resourceHandle = OcPlatform.registerResource(m_resourceURI,
                    m_resourceTypeName, m_resourceInterface, entityHandler,
                    m_resourceProperty);

            if (m_resourceTypeNames.size() > 1) {
                for (int i = 1; i < m_resourceTypeNames.size(); i++) {
                    OcPlatform.bindTypeToResource(m_resourceHandle,
                            m_resourceTypeNames.get(i));
                }
            }

            if (m_resourceInterfaces.size() > 1) {
                for (int i = 1; i < m_resourceInterfaces.size(); i++) {
                    OcPlatform.bindInterfaceToResource(m_resourceHandle,
                            m_resourceInterfaces.get(i));
                }
            }

        } catch (OcException e) {
            System.out.println("Problem occured during creating resource");
            e.printStackTrace();
        }

        if (m_resourceHandle == null) {
            System.out.println("Resource failed to start");
            result = false;
        } else {
            m_isServerRunning = true;
            System.out.println("Resource Started");

            boolean isRegisteredForPresence = false;
            int presenceInterval = 0;
            onResourceServerStarted(isRegisteredForPresence, presenceInterval);

            if (isRegisteredForPresence) {
                try {
                    OcPlatform.startPresence(presenceInterval);
                    m_isPresenceStarted = true;
                } catch (OcException e) {
                    m_isPresenceStarted = false;
                    System.out.println("Unable to start presence");
                    e.printStackTrace();
                }
            }

            m_representation.setUri(m_resourceURI);

        }

        return result;
    }

    public boolean stopResource() {
        boolean result = true;

        // check whether server is running
        if (m_isServerRunning == false) {
            result = false;
            System.out.println(
                    "Resource server is not started; Please start it before stopping ");

            return result;
        }
        if (m_isPresenceStarted) {
            try {
                // stop presence
                OcPlatform.stopPresence();
                m_isPresenceStarted = false;
            } catch (OcException e) {
                System.out.println(
                        "Unable to stop presence, error is" + e.getMessage());
            }
        }

        try {
            // stop server
            OcPlatform.unregisterResource(m_resourceHandle);
            m_isServerRunning = false;
        } catch (OcException e) {
            System.out.println("Unable to stop server, exception occurred: "
                    + e.getMessage());
            e.printStackTrace();
            result = false;
        }

        return result;
    }

    public void setResourceRepresentation(OcRepresentation ocRepresentation) {
        m_representation = ocRepresentation;
    }

    public String getUri() {
        return m_resourceURI;
    }

    public static boolean setDeviceInfo(String deviceName, String deviceType) {
        boolean result = true;
        List<String> deviceTypes = new ArrayList<String>();
        deviceTypes.add(deviceType);
        m_deviceInfo = new OcDeviceInfo(deviceName, deviceTypes);
        try {
            OcPlatform.registerDeviceInfo(m_deviceInfo);
        } catch (OcException e) {
            Base.showOutPut(e.getMessage());
            result = false;
        }

        return result;
    }

    public static boolean setPlatformInfo(String platformID,
                                          String manufacturerName, String manufacturerUrl, String modelNumber,
                                          String dateOfManufacture, String platformVersion,
                                          String operatingSystemVersion, String hardwareVersion,
                                          String firmwareVersion, String supportUrl, String systemTime) {
        boolean isPlatformSet = true;
        m_platformInfo = new OcPlatformInfo(platformID, manufacturerName,
                manufacturerUrl, modelNumber, dateOfManufacture,
                platformVersion, operatingSystemVersion, hardwareVersion,
                firmwareVersion, supportUrl, systemTime);
        if (m_platformInfo == null) {
            isPlatformSet = false;
        }
        return isPlatformSet;
    }

    public void setAsSlowResource() {
        m_isSlowResource = true;
    }

    public void setAsNormalResource() {
        m_isSlowResource = false;
    }

    void handleResponse(OcResourceRequest request) {
        OcResourceResponse response = new OcResourceResponse();
        response.setRequestHandle(request.getRequestHandle());
        response.setResourceHandle(request.getResourceHandle());

        // Get the request type and request flag
        RequestType requestType = request.getRequestType();
        EnumSet<RequestHandlerFlag> requestFlag = request
                .getRequestHandlerFlagSet();

        if (requestFlag.contains(RequestHandlerFlag.REQUEST)) {
            System.out.println("\t\trequestFlag : Request");
            System.out.println("\t\t\trequestType : " + requestType);

            // If the request type is GET
            if (requestType == RequestType.GET) {
                // Check for query params (if any)
                Map<String, String> queryParamsMap = request
                        .getQueryParameters();

                handleGetRequest(queryParamsMap, request, response);

            } else if (requestType == RequestType.PUT) {
                OcRepresentation incomingRepresentation = request
                        .getResourceRepresentation();

                // Check for query params (if any)
                Map<String, String> queryParamsMap = request
                        .getQueryParameters();

                handlePutRequest(queryParamsMap, incomingRepresentation,
                        request, response);
            } else if (requestType == RequestType.POST) {
                // POST request operations
                OcRepresentation incomingRepresentation = request
                        .getResourceRepresentation();

                // Check for query params (if any)
                Map<String, String> queryParamsMap = request
                        .getQueryParameters();

                handlePostRequest(queryParamsMap, incomingRepresentation,
                        request, response);
            } else if (requestType == RequestType.DELETE) {
                // DELETE request operations
                OcRepresentation incomingRepresentation = request
                        .getResourceRepresentation();
                // Check for query params (if any)
                Map<String, String> queryParamsMap = request
                        .getQueryParameters();

                handleDeleteRequest(queryParamsMap, incomingRepresentation,
                        request, response);
            }
        } else if (requestFlag.contains(RequestHandlerFlag.OBSERVER)) {
            // Check for query params (if any)
            Map<String, String> queryParamsMap = request.getQueryParameters();
            handleObserveRequest(queryParamsMap, request, response);
        }
    }

    void handleSlowResponse(OcResourceRequest request) {
        System.out.println("Acting as Slow Resource....");
        m_resourceHelper.waitInSecond(ResourceConstants.CALLBACK_WAIT_MAX);
        System.out.println("Slow working period is over");

        handleResponse(request);
    }

    public boolean isObservableResource() {
        return m_resourceProperty.contains(ResourceProperty.OBSERVABLE);
    }

    public boolean isDiscoverableResource() {
        return m_resourceProperty.contains(ResourceProperty.DISCOVERABLE);
    }

    public Vector<String> getResourceInterfaces() {
        return m_resourceInterfaces;
    }

    public Vector<String> getResourceTypes() {
        return m_resourceTypeNames;
    }

    public void setAsSecuredResource() {
        m_resourceProperty.add(ResourceProperty.SECURE);
    }

    public void setAsDiscoverableResource() {
        m_resourceProperty.add(ResourceProperty.DISCOVERABLE);
    }

    public void setAsObservableResource() {
        m_resourceProperty.add(ResourceProperty.OBSERVABLE);
    }

    public boolean isSecuredResource() {
        return m_resourceProperty.contains(ResourceProperty.SECURE);
    }
}
