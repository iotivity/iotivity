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
import org.iotivity.base.ObservationInfo;
import org.iotivity.base.ObserveAction;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.OcResourceResponse;
import org.iotivity.base.ResourceProperty;
import org.iotivity.common.ResourceConstants;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

public class SampleResource extends ResourceServer {

    private boolean m_isObserveRegistered = false;
    private Vector<String> m_resourceList = new Vector<String>();
    private Map<String, String> m_accessModifier = new HashMap<String, String>();
    private ArrayList<Byte> m_listOfObservers = new ArrayList<Byte>();

    public SampleResource() {
        super();
        m_listOfObservers.clear();
        m_resourceList.clear();
        m_accessModifier.clear();
    }

    @Override
    public void onResourceServerStarted(boolean isRegisteredForPresence,
                                        int presenceInterval) {
        System.out.println("Inside onResourceServerStarted... ");
        isRegisteredForPresence = false;
    }

    @Override
    public void handleGetRequest(Map<String, String> queryParamsMap,
                                 OcResourceRequest request, OcResourceResponse response) {
        System.out.println("Inside handleGetRequest... ");
        boolean result = false;
        boolean shouldReturnError = false;
        String responseInterface = OcPlatform.DEFAULT_INTERFACE;

        response.setErrorCode(ResourceConstants.COAP_RESPONSE_CODE_RETRIEVED);
        OcRepresentation rep = m_representation;
        System.out.println("Current Resource Representation to send : ");
        m_resourceHelper.printRepresentation(rep);

        if (queryParamsMap.size() > 0) {
            for (Map.Entry<String, String> eachQuery : queryParamsMap
                    .entrySet()) {
                String key = eachQuery.getKey();
                if (key == ResourceConstants.INTERFACE_KEY) {
                    responseInterface = key;
                    String queryValue = eachQuery.getValue();
                    if (queryValue == OcPlatform.DEFAULT_INTERFACE) {
                        System.out.println(
                                "Found baseline query, adding rt & if into response payload");
                        rep.setResourceInterfaces(getResourceInterfaces());
                        rep.setResourceTypes(getResourceTypes());
                        m_resourceHelper.printRepresentation(rep);

                    } else {
                        System.out.println(
                                "Ignoring if query other than baseline");
                    }
                    continue;
                }
                if (rep.hasAttribute(key)) {
                    String attributeValue = rep.getValueString(key);
                    if (eachQuery.getValue()
                            .contains(attributeValue) == false) {
                        shouldReturnError = true;
                        response.setResourceRepresentation(rep,
                                responseInterface);
                        break;
                    }
                }
                if ((key != ResourceConstants.INTERFACE_KEY) && (rep.hasAttribute(key) == false)) {
                    System.out.println("This query is not supported: "
                            + eachQuery.getKey() + "=" + eachQuery.getValue());
                    shouldReturnError = true;
                    break;
                }
            }
        } else {
            System.out.println("No query found!!");
        }

        if (shouldReturnError) {
            System.out.println("sending forbidden GET response");
            response.setResponseResult(EntityHandlerResult.ERROR);
        } else {
            System.out.println("sending normal GET response");
            response.setResourceRepresentation(rep, responseInterface);
            response.setResponseResult(EntityHandlerResult.OK);
        }

        try {
            OcPlatform.sendResponse(response);
            result = true;
        } catch (OcException e) {
            result = false;
            System.out.println(
                    "Exception occurred while sending response. Exception is: ");
            e.printStackTrace();
        }
        if (result == false) {
            System.out.println("Unable to send response for GET Request");
        }

    }

    @Override
    public void handlePutRequest(Map<String, String> queryParamsMap,
                                 OcRepresentation incomingRepresentation, OcResourceRequest request,
                                 OcResourceResponse response) {

        System.out.println("Inside handlePutRequest... ");
        System.out
                .println("The PUT request has the following representation: ");
        m_resourceHelper.printRepresentation(incomingRepresentation);

        // handle PUT request
        response.setErrorCode(ResourceConstants.COAP_RESPONSE_CODE_SUCCESS);

        boolean shouldChange = true;
        boolean result = true;

        if (shouldAllowUpdate(queryParamsMap, response)) {
            response.setErrorCode(ResourceConstants.COAP_RESPONSE_CODE_ERROR);
            response.setResponseResult(EntityHandlerResult.ERROR);
        }

        try {
            OcPlatform.sendResponse(response);
        } catch (OcException e) {
            System.out.println(
                    "Exception occurred while sending response. Exception is: ");
            e.printStackTrace();
        }
        if (result != true) {
            System.out.println("Unable to send response for POST Request");
        }
    }

    @Override
    public void handlePostRequest(Map<String, String> queryParamsMap,
                                  OcRepresentation incomingRepresentation, OcResourceRequest request,
                                  OcResourceResponse response) {
        boolean isRepUpdated = false;
        boolean isSameAttributeValue = false;
        boolean isAttributeReadOnly = false;
        boolean shouldChange = true;
        System.out.println("Inside handlePostRequest... ");
        System.out.println(
                "The POST request comprises of the following representation:");
        m_resourceHelper.printRepresentation(incomingRepresentation);

        // handle POST request
        response.setErrorCode(ResourceConstants.COAP_RESPONSE_CODE_ERROR);

        String uriValue = "";

        if (shouldAllowUpdate(queryParamsMap, response)) {
            if (incomingRepresentation.getUri() != "") {
                try {
                    uriValue = incomingRepresentation
                            .getValue(ResourceConstants.URI_KEY);
                } catch (OcException e) {
                    e.printStackTrace();
                }

                if (uriValue != m_resourceURI) {
                    String initialUri = getUri();
                    createResource(initialUri, incomingRepresentation,
                            response);
                }

            } else {

                m_representation = incomingRepresentation;
                isRepUpdated = true;

                if (isRepUpdated == true) {
                    System.out.println(
                            "Resource representation is updated!! Sending Notification to observers");
                    notifyObservers(this);
                    response.setErrorCode(
                            ResourceConstants.COAP_RESPONSE_CODE_UPDATED);
                    response.setResponseResult(EntityHandlerResult.OK);
                } else {
                    if (isAttributeReadOnly) {
                        System.out.println(
                                "Update is not possible, Attribute is read-only.");
                    } else if (isSameAttributeValue) {
                        System.out.println(
                                "Incoming representation value is same as resources representation value. No need to update!!");
                    } else {
                        System.out.println(
                                "Incoming Representation not supported by this resource!!");
                    }

                    response.setErrorCode(
                            ResourceConstants.COAP_RESPONSE_CODE_ERROR);
                    response.setResponseResult(EntityHandlerResult.ERROR);
                }
            }
        }

        try {
            OcPlatform.sendResponse(response);
        } catch (OcException e) {
            System.out.println("Unable to send response for POST Request");
            System.out.println(
                    "Exception occurred while sending response. Exception is: ");
            e.printStackTrace();
        }
    }

    @Override
    public void handleDeleteRequest(Map<String, String> queryParamsMap,
                                    OcRepresentation incomingRepresentation, OcResourceRequest request,
                                    OcResourceResponse response) {
        System.out.println("Inside handleDeleteRequest... ");
        boolean shouldDelete = true;

        System.out.println(
                "THe DELETE request comprises of the following representation:");
        m_resourceHelper.printRepresentation(incomingRepresentation);

        // handle DELETE

        if (shouldAllowUpdate(queryParamsMap, response)) {
            response.setErrorCode(ResourceConstants.COAP_RESPONSE_CODE_DELETED);
            response.setResponseResult(EntityHandlerResult.RESOURCE_DELETED);
        }

        try {
            OcPlatform.sendResponse(response);
        } catch (OcException e) {
            System.out.println("Unable to send response for Delete Request");
            e.printStackTrace();
        }

        if (shouldDelete) {
            stopResource();
        }
    }

    @Override
    public void handleObserveRequest(Map<String, String> queryParamsMap,
                                     OcResourceRequest request, OcResourceResponse response) {
        System.out.println("Inside handleObserveRequest... ");

        // handle observe request
        if (isObservableResource() == false) {
            System.out.println("Observe not supported!!");
            response.setErrorCode(ResourceConstants.COAP_RESPONSE_CODE_ERROR);
            response.setResponseResult(EntityHandlerResult.FORBIDDEN);
            System.out.println("Resource does not support Observe!!");

            try {
                OcPlatform.sendResponse(response);
            } catch (OcException e) {
                System.out.println("Unable to send response");
            }

            return;
        }

        ObservationInfo observationInfo = request.getObservationInfo();
        if (ObserveAction.REGISTER == observationInfo.getObserveAction()) {
            System.out.println("Adding new observer to observer list");
            System.out.println(
                    "Observe Info:" + observationInfo.getOcObservationId());
            m_listOfObservers.add(observationInfo.getOcObservationId());
            m_isObserveRegistered = true;
            System.out.println(
                    "Sending notification from from register observer - for the first time");

            response.setErrorCode(ResourceConstants.COAP_RESPONSE_CODE_SUCCESS);
            response.setResponseResult(EntityHandlerResult.OK);
            try {
                OcPlatform.sendResponse(response);
            } catch (OcException e) {
                System.out.println("Unable to register observe");
                e.printStackTrace();
            }
        } else if (ObserveAction.UNREGISTER == observationInfo
                .getObserveAction()) {
            if (m_listOfObservers.size() > 0) {
                System.out.println("Removing observer from observer list");
                System.out
                        .println("Observe Info: " + observationInfo.toString());
                m_listOfObservers.remove(observationInfo.getOcObservationId());
                response.setErrorCode(
                        ResourceConstants.COAP_RESPONSE_CODE_SUCCESS);
                response.setResponseResult(EntityHandlerResult.OK);
            } else {
                response.setErrorCode(
                        ResourceConstants.COAP_RESPONSE_CODE_ERROR);
                response.setResponseResult(EntityHandlerResult.ERROR);
                System.out
                        .println("No observer found!! Unable cancel observe!!");
            }

            try {
                OcPlatform.sendResponse(response);
            } catch (OcException e) {
                System.out.println("Unable to send response");
                e.printStackTrace();
            }
        }

        System.out.println("\t\trequestFlag : Observer\n");

        if (m_listOfObservers.size() == 0) {
            m_isObserveRegistered = false;
        }
    }

    public void notifyObservers(SampleResource resource) {

        System.out.println("Inside notifyObservers.....");
        try {
            if ((m_isObserveRegistered == true)
                    && (m_listOfObservers.size() > 0)) {
                System.out.println("Sending Notification to Observers....");
                OcResourceResponse resourceResponse = new OcResourceResponse();

                resourceResponse.setErrorCode(
                        ResourceConstants.COAP_RESPONSE_CODE_RETRIEVED);
                resourceResponse.setResponseResult(EntityHandlerResult.OK);
                resourceResponse.setResourceRepresentation(
                        resource.getRepresentation(),
                        OcPlatform.DEFAULT_INTERFACE);

                if (m_listOfObservers.size() > 0) {
                    System.out.println(
                            "No More observers, stopping notifications");
                    m_listOfObservers.clear();
                    m_isObserveRegistered = false;
                } else {
                    OcPlatform.notifyListOfObservers(
                            resource.getResourceHandle(),
                            resource.m_listOfObservers, resourceResponse);
                }
            } else {
                System.out.println("No observers available to notify!!");
            }
        } catch (OcException e) {
            System.out
                    .println("Unable to notify Observers, exception occurred.");
            e.printStackTrace();
        }
    }

    void createResource(String initialUri,
                        OcRepresentation incomingRepresentation,
                        OcResourceResponse response) {
        System.out.println("Creating new resource!!");
        String uriValue = incomingRepresentation.getUri();
        String resourceUri = initialUri + uriValue;
        boolean result = true;
        System.out.println(
                "The CREATE request consists of the following representation:");
        m_resourceHelper.printRepresentation(incomingRepresentation);

        if (resourceUri != m_resourceURI) {
            SampleResource newResource;

            String resourceType = "";
            resourceType = incomingRepresentation.getResourceTypes().get(0);

            String discoverableKey = "dis";
            boolean isDiscoverable = true;
            OcRepresentation blankRep = new OcRepresentation();
            if (resourceType == "") {
                System.out.println(
                        "No resource type received!! Unable to create resource.");
                response.setErrorCode(
                        ResourceConstants.COAP_RESPONSE_CODE_ERROR);
                response.setResponseResult(EntityHandlerResult.ERROR);
                response.setResourceRepresentation(blankRep,
                        OcPlatform.DEFAULT_INTERFACE);
            } else {
                if (incomingRepresentation.hasAttribute(discoverableKey)) {
                    System.out.println("Inside has attribute dis");
                    try {
                        isDiscoverable = incomingRepresentation
                                .getValue(discoverableKey);
                    } catch (OcException e) {
                        e.printStackTrace();
                    }
                }

                System.out.println("Creating resource using resource type = "
                        + resourceType + ", and resource uri = " + resourceUri);
                newResource = new SampleResource();
                System.out.println("constructor called!!");
                newResource.setResourceProperties(resourceUri, resourceType,
                        OcPlatform.DEFAULT_INTERFACE);
                System.out.println("resource property set!!");

                EnumSet<ResourceProperty> resourceProperty = EnumSet
                        .of(ResourceProperty.ACTIVE);
                if (isDiscoverable) {
                    resourceProperty.add(ResourceProperty.ACTIVE);
                    resourceProperty.add(ResourceProperty.DISCOVERABLE);
                    resourceProperty.add(ResourceProperty.OBSERVABLE);
                } else {
                    resourceProperty.add(ResourceProperty.ACTIVE);
                }

                newResource.setResourceRepresentation(incomingRepresentation);
                result = newResource.startResource(resourceProperty);

                if (result != true) {
                    System.out.println("unable to start resource!!");
                    response.setErrorCode(
                            ResourceConstants.COAP_RESPONSE_CODE_ERROR);
                    if (result == false) {
                        response.setResponseResult(
                                EntityHandlerResult.FORBIDDEN);
                    } else {
                        response.setResponseResult(EntityHandlerResult.ERROR);
                    }

                    response.setResourceRepresentation(blankRep,
                            OcPlatform.DEFAULT_INTERFACE);
                } else {
                    System.out.println("resource started!!");
                    response.setErrorCode(
                            ResourceConstants.COAP_RESPONSE_CODE_CREATED);
                    response.setResponseResult(
                            EntityHandlerResult.RESOURCE_CREATED);
                    response.setResourceRepresentation(
                            newResource.getRepresentation(),
                            OcPlatform.DEFAULT_INTERFACE);
                    m_resourceList.add(resourceUri);
                }
            }
        }
    }

    boolean addArrayAttribute(String key, OcRepresentation arrayRep) {
        boolean result = true;

        arrayRep.remove(ResourceConstants.URI_KEY);
        try {
            m_representation.setValue(key, arrayRep);
        } catch (OcException e) {
            e.printStackTrace();
        }

        return result;
    }

    public void setAsReadOnly(String key) {
        m_accessModifier.put(key, "R");
    }

    public boolean isReadonly(String key) {
        boolean readOnly = false;
        if (m_accessModifier.get(key) == "R") {
            readOnly = true;
        }
        return readOnly;
    }

    void supportCreateAndOthersForPUT(Map<String, String> queryParamsMap,
                                      OcRepresentation incomingRepresentation, OcResourceRequest request,
                                      OcResourceResponse response) {
        String uriValue = "";
        String targetValue = "";
        OcRepresentation rep = getRepresentation();
        if (incomingRepresentation.getUri() != "") {
            try {
                uriValue = incomingRepresentation
                        .getValue(ResourceConstants.URI_KEY);
            } catch (OcException e) {
                System.out.println("Unable to get value from representation");
                e.printStackTrace();
            }

            if (uriValue != m_resourceURI) {

                createResource("", incomingRepresentation, response);
            } else {
                rep = incomingRepresentation;
                System.out.println(
                        "Sending notification from complete create - PUT");
                notifyObservers(this);
                rep.setUri("");
                response.setResourceRepresentation(rep,
                        OcPlatform.DEFAULT_INTERFACE);
                response.setResponseResult(EntityHandlerResult.OK);
                response.setErrorCode(
                        ResourceConstants.COAP_RESPONSE_CODE_UPDATED);

            }

            try {
                OcPlatform.sendResponse(response);
            } catch (OcException e) {
                System.out.println("Unable to send response for PUT Request");
            }

        } else if (incomingRepresentation
                .hasAttribute(ResourceConstants.FACTORY_RESET_KEY)) {
            targetValue = incomingRepresentation
                    .getValueString(ResourceConstants.FACTORY_RESET_KEY);
            System.out.println(
                    "Inside Factory Reset, target value = " + targetValue);

            if (targetValue == "true") {
                System.out.println("Factory Reset Going on....");

                String configurationKey = ResourceConstants.FACTORY_RESET_KEY;
                String configurationValue = targetValue;

                try {
                    rep.setValue(configurationKey, configurationValue);

                    configurationKey = "power";
                    configurationValue = ResourceConstants.DEFAULT_POWER_STATE;
                    rep.setValue(configurationKey, configurationValue);

                    configurationKey = "intensity";
                    rep.setValue(configurationKey,
                            ResourceConstants.DEFAULT_INTENSITY);

                    configurationKey = "manufacturer";
                    configurationValue = ResourceConstants.DEFAULT_MANUFACTURER;
                    rep.setValue(configurationKey, configurationValue);

                    configurationKey = ResourceConstants.REGION_KEY;
                    configurationValue = ResourceConstants.DEFAULT_REGION;
                    rep.setValue(configurationKey, configurationValue);

                    configurationKey = ResourceConstants.REBOOT_KEY;
                    configurationValue = ResourceConstants.DEFAULT_REBOOT_STATE;
                    rep.setValue(configurationKey, configurationValue);

                    response.setResourceRepresentation(rep,
                            OcPlatform.DEFAULT_INTERFACE);
                    setResourceRepresentation(rep);
                } catch (OcException e) {
                    System.out.println(
                            "Unable to retrieve/update representation ");
                    e.printStackTrace();
                }

                try {
                    configurationValue = rep.getValue(configurationKey);
                } catch (OcException e) {
                    e.printStackTrace();
                }
                String updatedConfiguration = configurationKey + " = "
                        + configurationValue;
                System.out.println("Configuration after Factory reset: "
                        + updatedConfiguration);
            }

            try {
                OcPlatform.sendResponse(response);
            } catch (OcException e) {
                System.out.println("Unable to send response for PUT Request");
            }

            targetValue = ResourceConstants.DEFAULT_FACTORY_RESET_STATE;
            try {
                rep.setValue("value", targetValue);
            } catch (OcException e) {
                e.printStackTrace();
            }
        } else if (incomingRepresentation
                .hasAttribute(ResourceConstants.REBOOT_KEY)) {
            targetValue = incomingRepresentation
                    .getValueString(ResourceConstants.REBOOT_KEY);
            System.out.println("Inside Reboot, target value = " + targetValue);

            if (targetValue == "true") {
                System.out.println("Rebooting Resource ");
                try {
                    rep.setValue(ResourceConstants.REBOOT_KEY, targetValue);
                } catch (OcException e) {
                    e.printStackTrace();
                }
                response.setResourceRepresentation(rep,
                        OcPlatform.DEFAULT_INTERFACE);

                targetValue = rep.getValueString(ResourceConstants.REBOOT_KEY);
                String updatedConfiguration = ResourceConstants.REBOOT_KEY;
                updatedConfiguration = updatedConfiguration + " = "
                        + targetValue;

                try {
                    OcPlatform.sendResponse(response);
                } catch (OcException e) {
                    System.out
                            .println("Unable to send response for PUT Request");
                }
            }
        } else {
            System.out.println("Inside default put handle");
            rep = incomingRepresentation;
            System.out
                    .println("Sending notification from complete update - PUT");
            notifyObservers(this);
            response.setResourceRepresentation(rep,
                    OcPlatform.DEFAULT_INTERFACE);
            response.setErrorCode(ResourceConstants.COAP_RESPONSE_CODE_UPDATED);
            response.setResponseResult(EntityHandlerResult.OK);
            try {
                OcPlatform.sendResponse(response);
            } catch (OcException e) {
                System.out.println("Unable to send response for PUT Request");
            }
        }
    }

    boolean shouldAllowUpdate(Map<String, String> queryParamsMap, OcResourceResponse response) {
        boolean shouldChange = true;
        if (queryParamsMap.size() > 0) {
            for (Map.Entry<String, String> eachQuery : queryParamsMap
                    .entrySet()) {
                String key = eachQuery.getKey();
                if (key == ResourceConstants.INTERFACE_KEY) {
                    String value = eachQuery.getValue();
                    if ((value == ResourceConstants.READ_ONLY_INTERFACE) || (value == ResourceConstants.SENSOR_INTERFACE)) {
                        System.out.println(
                                "Update/Delete request received via interface: "
                                        + value
                                        + " . This interface is not authorized to update/delete resource!!");
                        response.setErrorCode(
                                ResourceConstants.COAP_RESPONSE_CODE_RESOURCE_UNAUTHORIZED);
                        response.setResponseResult(
                                EntityHandlerResult.FORBIDDEN);
                        shouldChange = false;
                        break;
                    }
                }
            }
        }
        return shouldChange;
    }
}
