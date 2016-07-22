/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "SampleResource.h"

#define PRESENCE_INTERVAL 10000

SampleResource::SampleResource(void)
{
    //child constructor

    string value = DEFAULT_POWER_STATE;
    m_representation.setValue(POWER_KEY, value);
    int intensity = DEFAULT_INTENSITY;
    m_representation.setValue(INTENSITY_KEY, intensity);
    value = DEFAULT_MANUFACTURER;
    m_representation.setValue(MANUFACTURER_KEY, value);
    value = DEFAULT_REGION;
    m_representation.setValue(REGION_KEY, value);
    value = DEFAULT_FACTORY_RESET_STATE;
    m_representation.setValue(FACTORY_RESET_KEY, value);
    value = DEFAULT_REBOOT_STATE;
    m_representation.setValue(REBOOT_KEY, value);
    value = "";
    m_representation.setValue(ACTIONSET_KEY, value);
    float accuracy = DEFAULT_ACCURACY;
    m_representation.setValue(ACCURACY_KEY, accuracy);
    double version = DEFAULT_VERSION;
    m_representation.setValue(VERSION_KEY, version);
    bool isCrudnSupported = DEFAULT_CRUDN_SUPPORT;
    m_representation.setValue(CRUDN_SUPPORT_KEY, isCrudnSupported);

    m_recursiveDelay = 0;
    m_scheduledDelay = 0;
    m_isCancelCalled = false;
    m_isObserveRegistered = false;
    m_listOfObservers.clear();
    m_resourceList.clear();
    m_accessmodifier.clear();

    cout << "Current resource info: " << m_representation.getPayload()->values->str << endl;

}

SampleResource::~SampleResource(void)
{
    // child destructor
}

void SampleResource::handleObserveRequest(QueryParamsMap &queryParamsMap,
        std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    cout << "Inside handleObserveRequest... " << endl;
    OCStackResult result = OC_STACK_ERROR;

    // handle observe request
    if (this->isObservableResource() == false)
    {
        cout << "Observe not supported!!";
        response->setErrorCode(COAP_RESPONSE_CODE_ERROR);
        response->setResponseResult(OCEntityHandlerResult::OC_EH_FORBIDDEN);
        cerr << "Resource does not support Observe!!" << endl;

        result = OCPlatform::sendResponse(response);
        if (result != OC_STACK_OK)
        {
            cerr << "Unable to send response" << endl;
        }

        return;
    }

    ObservationInfo observationInfo = request->getObservationInfo();
    if (ObserveAction::ObserveRegister == observationInfo.action)
    {
        cout << "Adding new observer to observer list" << endl;
        cout << "Observe Info:" << observationInfo.obsId << endl;
        m_listOfObservers.push_back(observationInfo.obsId);
        m_isObserveRegistered = true;
        cout << "Sending notification from from register observer - for the first time" << endl;

        response->setErrorCode(COAP_RESPONSE_CODE_SUCCESS);
        response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
        result = OCPlatform::sendResponse(response);
        if (result != OC_STACK_OK)
        {
            cerr << "Unable to register observe" << endl;
        }
    }
    else if (ObserveAction::ObserveUnregister == observationInfo.action)
    {
        if (m_listOfObservers.size() > 0)
        {
            cout << "Removing observer from observer list" << endl;
            cout << "Observe Info: " << observationInfo.address << endl;
            m_listOfObservers.erase(
                    remove(m_listOfObservers.begin(), m_listOfObservers.end(),
                            observationInfo.obsId), m_listOfObservers.end());
            response->setErrorCode(COAP_RESPONSE_CODE_SUCCESS);
            response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
        }
        else
        {
            response->setErrorCode(COAP_RESPONSE_CODE_ERROR);
            response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
            cerr << "No observer found!! Unable cancel observe!!" << endl;
        }

        result = OCPlatform::sendResponse(response);
        if (result != OC_STACK_OK)
        {
            cerr << "Unable to send response" << endl;
        }
    }

    cout << "\t\trequestFlag : Observer\n";

    if (m_listOfObservers.size() == 0)
    {
        m_isObserveRegistered = false;
    }
}

void SampleResource::handleDeleteRequest(QueryParamsMap &queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    cout << "Inside handleDeleteRequest... " << endl;
    // handle delete request
    OCRepresentation rep = getRepresentation();
    bool shouldDelete = true;

    cout << "THe DELETE request comprises of the following representation:" << endl;
    p_resourceHelper->printRepresentation(incomingRepresentation);

    // handle DELETE
    if (queryParamsMap.size() > 0)
    {
        for (const auto &eachQuery : queryParamsMap)
        {
            string key = eachQuery.first;
            if (key.compare("if") == 0)
            {
                string value = eachQuery.second;
                if (value.compare("oic.if.r") == 0)
                {
                    cout
                            << "Update request received via read-only interface. Read-only interface is not authorized to update resource!!"
                            << endl;
                    response->setErrorCode(COAP_RESPONSE_CODE_RESOURCE_UNAUTHORIZED);
                    response->setResponseResult(OCEntityHandlerResult::OC_EH_FORBIDDEN);
                    shouldDelete = false;
                    break;
                }
            }
        }
    }

    if (shouldDelete)
    {
        response->setErrorCode(COAP_RESPONSE_CODE_DELETED);
        response->setResponseResult(OCEntityHandlerResult::OC_EH_RESOURCE_DELETED);
    }

    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        cerr << "Unable to send response for Delete Request" << endl;
    }
    if (shouldDelete)
    {
        this->stopResource();
    }

}

void SampleResource::handlePostRequest(QueryParamsMap &queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    OCStackResult result = OC_STACK_ERROR;
    bool isRepUpdated = false;
    bool isSameAttributeValue = false;
    bool isAttributeReadOnly = false;
    bool shouldChange = true;
    cout << "Inside handlePostRequest... " << endl;
    cout << "THe POST request comprises of the following representation:" << endl;
    p_resourceHelper->printRepresentation(incomingRepresentation);

    // handle POST request
    response->setErrorCode(COAP_RESPONSE_CODE_ERROR);

    string targetValue = "";
    string uriValue = "";
    OCRepresentation rep = getRepresentation();
    m_pResponse = response;

    if (queryParamsMap.size() > 0)
    {
        for (const auto &eachQuery : queryParamsMap)
        {
            string key = eachQuery.first;
            if (key.compare(INTERFACE_KEY) == 0)
            {
                string value = eachQuery.second;
                if ((value.compare("oic.if.r") == 0) || (value.compare("oic.if.s") == 0))
                {
                    cout << "Update request received via interface: " << value
                            << " . This interface is not authorized to update resource!!" << endl;
                    response->setErrorCode(COAP_RESPONSE_CODE_RESOURCE_UNAUTHORIZED);
                    response->setResponseResult(OCEntityHandlerResult::OC_EH_FORBIDDEN);
                    shouldChange = false;
                    break;
                }
            }

        }
    }

    if (shouldChange)
    {
        if (incomingRepresentation.hasAttribute(EXECUTE_ACTIONSET_KEY))
        {
            string actionSet = "";
            rep.getValue(ACTIONSET_KEY, actionSet);
            if (actionSet.compare("") != 0)
            {
                cout << "Executing ActionSet..." << endl;
                string targetValue = "on";
                string targetKey = "power";

                int startPos = actionSet.find_first_of("*") + 1;
                int endPos = actionSet.find_first_of("*", startPos);
                string actionSetInfo = actionSet.substr(startPos, endPos - startPos);
                cout << actionSetInfo << endl;
                int delay = stoi(actionSetInfo.substr(0, actionSetInfo.find_first_of(" ")));
                int actionSetType = stoi(
                        actionSetInfo.substr(actionSetInfo.find_first_of(" ") + 1));

                if (actionSetType == 2)
                {
                    m_recursiveDelay = delay;
                    m_isCancelCalled = false;

                    std::thread recursive(&SampleResource::handleRecursiveActionSet, this);
                    recursive.detach();

                }
                else if (actionSetType == 1)
                {
                    m_scheduledDelay = delay;
                    m_isCancelCalled = false;

                    std::thread sceduled(&SampleResource::handleScheduledActionSet, this);
                    sceduled.detach();

                }
                else
                {
                    rep.setValue(targetKey, targetValue);
                    m_representation = rep;
                    response->setErrorCode(COAP_RESPONSE_CODE_UPDATED);
                    response->setResourceRepresentation(rep, DEFAULT_INTERFACE);

                    rep.getValue(targetKey, targetValue);
                    if (targetValue.compare("on") == 0)
                    {
                        cout << "Successfully executed ActionSet" << endl;
                    }
                    else
                    {
                        cerr << "Failed to execute ActionSet" << endl;
                    }

                    string updatedConfiguration = targetKey + " = " + targetValue;
                    cout << "Updated Configuration: " << updatedConfiguration << endl;
                }

            }

        }
        else if (incomingRepresentation.hasAttribute(EXECUTE_SCHEDULED_ACTIONSET_KEY))
        {
            string actionSet = "";
            rep.getValue(ACTIONSET_KEY, actionSet);
            if (actionSet.compare("") != 0)
            {
                cout << "Executing Scheduled ActionSet..." << endl;
                string scheduleWithDelay = "";
                incomingRepresentation.getValue(EXECUTE_SCHEDULED_ACTIONSET_KEY, scheduleWithDelay);
                cout << scheduleWithDelay << endl;
                int delaySec = stoi(
                        scheduleWithDelay.substr(scheduleWithDelay.find_last_of("*") + 1));
                cout << "Delay time in second is: " << delaySec << endl;
                p_resourceHelper->waitInSecond(delaySec);

                string targetValue = "on";
                string targetKey = "power";
                rep.setValue(targetKey, targetValue);

                m_representation = rep;

                rep.getValue(targetKey, targetValue);
                if (targetValue.compare("on") == 0)
                {
                    cout << "Successfully executed Scheduled ActionSet" << endl;
                }
                else
                {
                    cerr << "Failed to execute Scheduled ActionSet" << endl;
                }

                string updatedConfiguration = targetKey + " = " + targetValue;
                cout << "Updated Configuration: " << updatedConfiguration << endl;
            }
            response->setErrorCode(COAP_RESPONSE_CODE_UPDATED);
            response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
            response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
        }
        else if (incomingRepresentation.hasAttribute(CANCEL_ACTIONSET_KEY))
        {
            m_isCancelCalled = true;
            response->setErrorCode(COAP_RESPONSE_CODE_UPDATED);
            response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
            response->setResourceRepresentation(m_representation, DEFAULT_INTERFACE);

        }
        else if (incomingRepresentation.hasAttribute(GET_ACTIONSET_KEY))
        {
            response->setErrorCode(COAP_RESPONSE_CODE_UPDATED);
            response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
            response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
        }
        else if (incomingRepresentation.getUri().compare("") != 0)
        {
            incomingRepresentation.getValue(URI_KEY, uriValue);

            if (uriValue.compare(m_resourceURI) != 0)
            {
                string initialUri = this->getUri();
                this->createResource(initialUri, incomingRepresentation, response);
            }

        }
        else
        {

            for (OCRepresentation::iterator repIter = incomingRepresentation.begin();
                    repIter != incomingRepresentation.end(); repIter++)
            {
                string key = repIter->attrname();

                int resourceTypeSize = incomingRepresentation.getResourceTypes().size();
                int interfaceListSize = incomingRepresentation.getResourceInterfaces().size();

                if (key.compare(RESOURCE_TYPE_KEY) == 0 || key.compare(INTERFACE_KEY) == 0
                        || resourceTypeSize > 0 || interfaceListSize > 0)
                {
                    isRepUpdated = false;
                    break;
                }

                if (m_representation.hasAttribute(key) && (isReadonly(key) == false))
                {
                    string repValueString = "";
                    string incomingValueString = "";
                    repValueString = m_representation.getValueToString(key);
                    incomingValueString = incomingRepresentation.getValueToString(key);

                        updateRepresentation(key, incomingRepresentation, response);
                        isSameAttributeValue = false;
                        isRepUpdated = true;

                }
                else if (m_representation.hasAttribute(key) && (isReadonly(key) == true))
                {
                    isAttributeReadOnly = true;
                    isRepUpdated = false;
                    break;
                }
            }

            if (isRepUpdated == true)
            {
                cout << "Resource representation is updated!! Sending Notification to observers"
                        << endl;
                notifyObservers(this);
                response->setErrorCode(COAP_RESPONSE_CODE_UPDATED);
                response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
            }
            else
            {
                if (isAttributeReadOnly)
                {
                    cout << "Update is not possible, Attribute is read-only." << endl;
                }
                else if (isSameAttributeValue)
                {
                    cout
                            << "Incoming representation value is same as resources representation value. No need to update!!"
                            << endl;
                }
                else
                {
                    cout << "Incoming Representation not supported by this resource!!" << endl;
                }

                response->setErrorCode(COAP_RESPONSE_CODE_ERROR);
                response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
            }

        }
    }

    try
    {
        result = OCPlatform::sendResponse(response);
    }
    catch (exception& e)
    {
        cout << "Exception occurred while sending response. Exception is: " << e.what() << endl;
    }
    if (result != OC_STACK_OK)
    {
        cerr << "Unable to send response for POST Request" << endl;
    }
}

void SampleResource::handleGetRequest(QueryParamsMap &queryParamsMap,
        std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    cout << "Inside handleGetRequest... " << endl;
    OCStackResult result = OC_STACK_ERROR;
    bool shouldReturnError = false;
    string responseInterface = DEFAULT_INTERFACE;

    response->setErrorCode(COAP_RESPONSE_CODE_RETRIEVED);
    OCRepresentation rep = m_representation;
    cout << "Current Resource Representation to send : " << endl;
    p_resourceHelper->printRepresentation(rep);

    if (queryParamsMap.size() > 0)
    {
        for (const auto &eachQuery : queryParamsMap)
        {
            string key = eachQuery.first;
            if (key.compare("if") == 0)
            {
                responseInterface = key;
                string queryValue = eachQuery.second;
                if (queryValue.compare(DEFAULT_INTERFACE) == 0)
                {
                    cout << "Found baseline query, adding rt & if into response payload" << endl;
                    rep.setResourceInterfaces(getResourceInterfaces());
                    rep.setResourceTypes(getResourceTypes());
                    p_resourceHelper->printRepresentation(rep);

                }
                else
                {
                    cout << "Ignoring if query other than baseline" << endl;
                }
                continue;
            }
            if (rep.hasAttribute(key))
            {
                string attributeValue = rep.getValueToString(key);
                if (eachQuery.second.find(attributeValue) == string::npos)
                {
                    shouldReturnError = true;
                    response->setResourceRepresentation(rep, responseInterface);
                    break;
                }
            }
            if ((key.compare("if") != 0) && (rep.hasAttribute(key) == false))
            {
                cout << "This query is not supported: " << eachQuery.first << "=" << eachQuery.second << endl;
                shouldReturnError = true;
                break;
            }
        }
    }
    else
    {
        cout << "No query found!!" << endl;
    }

    if (shouldReturnError)
    {
        cout << "sending forbidden GET response" << endl;
        response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
    }
    else
    {
        cout << "sending normal GET response" << endl;
        response->setResourceRepresentation(rep, responseInterface);
        response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
    }

    try
    {
        result = OCPlatform::sendResponse(response);
    }
    catch (exception& e)
    {
        cout << "Exception occurred while sending response. Exception is: " << e.what() << endl;
    }
    if (result != OC_STACK_OK)
    {
        cerr << "Unable to send response for GET Request" << endl;
    }
}

void SampleResource::handlePutRequest(QueryParamsMap &queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    OCRepresentation rep = getRepresentation();

    cout << "Inside handlePutRequest... " << endl;
    cout << "The PUT request has the following representation: " << endl;
    p_resourceHelper->printRepresentation(incomingRepresentation);

// handle PUT request
    response->setErrorCode(COAP_RESPONSE_CODE_SUCCESS);

    string targetValue = "";
    string updateKey = "power";
    string uriValue = "";
    bool shouldChange = true;
    OCStackResult result = OC_STACK_OK;

    if (queryParamsMap.size() > 0)
    {
        for (const auto &eachQuery : queryParamsMap)
        {
            string key = eachQuery.first;
            if (key.compare("if") == 0)
            {
                string value = eachQuery.second;
                if ((value.compare("oic.if.r") == 0) || (value.compare("oic.if.s") == 0))
                {
                    cout << "Update request received via interface: " << value
                            << " . This interface is not authorized to update resource!!" << endl;
                    response->setErrorCode(COAP_RESPONSE_CODE_RESOURCE_UNAUTHORIZED);
                    response->setResponseResult(OCEntityHandlerResult::OC_EH_FORBIDDEN);
                    shouldChange = false;
                    break;
                }
            }

        }
    }

    if (shouldChange)
    {
        response->setErrorCode(COAP_RESPONSE_CODE_ERROR);
        response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
    }

    try
    {
        result = OCPlatform::sendResponse(response);
    }
    catch (exception& e)
    {
        cout << "Exception occurred while sending response. Exception is: " << e.what() << endl;
    }
    if (result != OC_STACK_OK)
    {
        cerr << "Unable to send response for POST Request" << endl;
    }
}

void SampleResource::onResourceServerStarted(bool &isRegisteredForPresence, int &presenceInterval)
{
    cout << "Inside onResourceServerStarted... " << endl;
    isRegisteredForPresence = false;
    presenceInterval = PRESENCE_INTERVAL;

}

void SampleResource::handleInitRequest(QueryParamsMap &queryParamsMap,
        std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    cout << "Inside handleInitRequest... " << endl;

// handle initialization
    response->setErrorCode(COAP_RESPONSE_CODE_SUCCESS);

    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        cerr << "Unable to send response for Init Request" << endl;
    }
}

OCRepresentation SampleResource::getResourceRepresentation(OCRepresentation &resourceRep)
{
    return resourceRep;
}

void SampleResource::handleRecursiveActionSet()
{
    OCRepresentation rep = getRepresentation();
    string targetValue = "on";
    string targetKey = "power";

    while (!m_isCancelCalled)
    {
        p_resourceHelper->waitInSecond(m_recursiveDelay);
        rep.setValue(targetKey, targetValue);
        m_representation = rep;
        string toShow = "Recursive ActionSet Executed!! Current Value : " + targetKey + " = "
                + targetValue;
        cout << toShow << endl;

    }

}

void SampleResource::handleScheduledActionSet()
{
    OCRepresentation rep = getRepresentation();
    string targetValue = "on";
    string targetKey = "power";

    while (m_scheduledDelay > 0 && m_isCancelCalled == false)
    {
        p_resourceHelper->waitInSecond(1);
        m_scheduledDelay--;
    }

    if (m_isCancelCalled)
    {
        return;
    }
    rep.setValue(targetKey, targetValue);
    m_representation = rep;
    string toShow = "Scheduled ActionSet Executed!! Current Value : " + targetKey + " = "
            + targetValue;
    cout << toShow << endl;

}

void SampleResource::notifyObservers(void *param)
{
    SampleResource *resource = (SampleResource *) param;

    cout << "Inside notifyObservers....." << endl;
    try
    {
        if ((m_isObserveRegistered == true) && (m_listOfObservers.size() > 0))
        {
            cout << "Sending Notification to Observers...." << endl;
            std::shared_ptr< OCResourceResponse > resourceResponse =
            { std::make_shared< OCResourceResponse >() };

            resourceResponse->setErrorCode(COAP_RESPONSE_CODE_RETRIEVED);
            resourceResponse->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
            resourceResponse->setResourceRepresentation(resource->getRepresentation(),
                    DEFAULT_INTERFACE);

            OCStackResult result = OCPlatform::notifyListOfObservers(resource->getResourceHandle(),
                    resource->m_listOfObservers, resourceResponse);

            if (OC_STACK_NO_OBSERVERS == result)
            {
                cout << "No More observers, stopping notifications" << endl;
                m_listOfObservers.clear();
                m_isObserveRegistered = false;
            }
        }
        else
        {
            cout << "No observers available to notify!!" << endl;
        }
    }
    catch (const exception& e)
    {
        cerr << e.what() << endl;
    }

}

bool SampleResource::updateRepresentation(string key, OCRepresentation incomingRep,
        shared_ptr< OCResourceResponse > response)
{
    bool result = false;
    OCRepresentation rep = getRepresentation();

    string targetValue = "";
    AttributeValue incomingValue;
    incomingRep.getAttributeValue(key, incomingValue);

    cout << "Updating Representation... " << endl;
    string configurationKey = key;
    AttributeValue configurationValue;
    rep.setValue(configurationKey, incomingValue);
    if (rep.getAttributeValue(configurationKey, configurationValue))
    {
        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
        setResourceRepresentation(rep);
        response->setErrorCode(COAP_RESPONSE_CODE_UPDATED);
        response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
        result = true;

    }
    else
    {
        result = false;
        response->setErrorCode(COAP_RESPONSE_CODE_ERROR);
        response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
    }

    return result;
}

void SampleResource::createResource(string initialUri, OCRepresentation incomingRepresentation,
        shared_ptr< OCResourceResponse > response)
{
    cout << "Creating new resource!!" << endl;
    string uriValue = incomingRepresentation.getUri();
    string resourceUri = initialUri + uriValue;
    OCStackResult result = OC_STACK_OK;
    cout << "The CREATE request consists of the following representation:" << endl;
    p_resourceHelper->printRepresentation(incomingRepresentation);

    if (resourceUri.compare(m_resourceURI) != 0)
    {
        SampleResource *newResource;

        string resourceType = "";
        try
        {
            resourceType = incomingRepresentation.getResourceTypes().at(0);
        }
        catch (const exception &e)
        {
            cerr << e.what() << endl;
        }

        string discoverableKey = "dis";
        bool isDiscoverable = true;
        OCRepresentation blankRep;
        if (resourceType.compare("") == 0)
        {
            cerr << "No resource type received!! Unable to create resource." << endl;
            response->setErrorCode(COAP_RESPONSE_CODE_ERROR);
            response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
            response->setResourceRepresentation(blankRep, DEFAULT_INTERFACE);
        }
        else
        {
            if (incomingRepresentation.hasAttribute(discoverableKey))
            {
                cout << "Inside has attribute dis" << endl;
                string discoverable = "";
                incomingRepresentation.getValue(discoverableKey, isDiscoverable);
                if (discoverable.compare("0") == 0)
                {
                    isDiscoverable = false;
                }
            }

            cout << "Creating resource using resource type = " << resourceType
                    << ", and resource uri = " << resourceUri << endl;
            try
            {
                newResource = new SampleResource();
                cout << "constructor called!!" << endl;
                result = (OCStackResult) ((int) result
                        + (int) newResource->setResourceProperties(resourceUri, resourceType,
                                DEFAULT_INTERFACE));
                cout << "resource property set!!" << endl;

                uint8_t resourceProperty;
                if (isDiscoverable)
                {
                    resourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE;
                }
                else
                {
                    resourceProperty = OC_ACTIVE;
                }

                newResource->setResourceRepresentation(incomingRepresentation);
                result = newResource->startResource(resourceProperty);

                if (result != OC_STACK_OK)
                {
                    cout << "unable to start resource!!" << endl;
                    response->setErrorCode(COAP_RESPONSE_CODE_ERROR);
                    if (result == OC_STACK_INVALID_URI)
                    {
                        response->setResponseResult(OCEntityHandlerResult::OC_EH_FORBIDDEN);
                    }
                    else
                    {
                        response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                    }

                    response->setResourceRepresentation(blankRep, DEFAULT_INTERFACE);
                }
                else
                {
                    cout << "resource started!!" << endl;
                    response->setErrorCode(COAP_RESPONSE_CODE_CREATED);
                    response->setResponseResult(OCEntityHandlerResult::OC_EH_RESOURCE_CREATED);
                    response->setResourceRepresentation(newResource->getRepresentation(),
                            DEFAULT_INTERFACE);
                    m_resourceList.push_back(resourceUri);
                }
            }
            catch (const exception &e)
            {
                cout << "unable to start resource!!" << endl;
                cerr << e.what() << endl;
                response->setErrorCode(COAP_RESPONSE_CODE_ERROR);
                response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
                response->setResourceRepresentation(blankRep, DEFAULT_INTERFACE);
            }

        }
    }
}

OCStackResult SampleResource::addArrayAttribute(string key, OCRepresentation arrayRep)
{
    OCStackResult result = OC_STACK_OK;

    arrayRep.erase(URI_KEY);
    m_representation.setValue(key, arrayRep);

    return result;
}

void SampleResource::setAsReadOnly(string key)
{
    m_accessmodifier[key] = "R";
}

bool SampleResource::isReadonly(string key)
{
    bool readOnly = false;
    if (m_accessmodifier[key] == "R")
    {
        readOnly = true;
    }
    return readOnly;
}

void SampleResource::supportCreateAndOthersForPUT(QueryParamsMap &queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    string uriValue = "";
    string targetValue = "";
    OCStackResult result = OC_STACK_OK;
    OCRepresentation rep = getRepresentation();
    if (incomingRepresentation.getUri().compare("") != 0)
    {
        incomingRepresentation.getValue(URI_KEY, uriValue);

        if (uriValue.compare(m_resourceURI) != 0)
        {

            createResource("", incomingRepresentation, response);
        }
        else
        {
            rep = incomingRepresentation;
            cout << "Sending notification from complete create - PUT" << endl;
            notifyObservers(this);
            rep.setUri("");
            response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
            response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
            response->setErrorCode(COAP_RESPONSE_CODE_UPDATED);

        }

        result = OCPlatform::sendResponse(response);
        if (result != OC_STACK_OK)
        {
            cerr << "Unable to send response for PUT Request" << endl;
        }

    }
    else if (incomingRepresentation.hasAttribute(ACTIONSET_KEY))
    {
        updateRepresentation(ACTIONSET_KEY, incomingRepresentation, response);
        cout << "Sending notification from add actionset - PUT" << endl;
        notifyObservers(this);
    }
    else if (incomingRepresentation.hasAttribute(DELETE_ACTIONSET_KEY))
    {
        incomingRepresentation.getValue(DELETE_ACTIONSET_KEY, targetValue);
        cout << "Deleting ActionSet, with target = " << targetValue << endl;

        if (targetValue.compare("") != 0)
        {
            cout << "Deleting ActionSet from representation... " << endl;
            string configurationKey = ACTIONSET_KEY;
            string configurationValue = "";

            cout << "System JSON Representation: " << rep.getPayload()->values->str << endl;
            rep.setValue(configurationKey, configurationValue);
            response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
            setResourceRepresentation(rep);
            cout << "Updated System JSON Representation: " << rep.getPayload()->values->str << endl;
            ;
            rep.getValue(configurationKey, configurationValue);
            string updatedConfiguration = configurationKey + " = " + configurationValue;
            cout << "Updated Configuration: " << updatedConfiguration << endl;
            ;

        }

        result = OCPlatform::sendResponse(response);
        if (result != OC_STACK_OK)
        {
            cerr << "Unable to send response for PUT Request" << endl;
        }
    }
    else if (incomingRepresentation.hasAttribute(FACTORY_RESET_KEY))
    {
        incomingRepresentation.getValue(FACTORY_RESET_KEY, targetValue);
        cout << "Inside Factory Reset, target value = " << targetValue << endl;

        if (targetValue.compare("true") == 0)
        {
            cout << "Factory Reset Going on...." << endl;

            string configurationKey = FACTORY_RESET_KEY;
            string configurationValue = targetValue;
            rep.setValue(configurationKey, configurationValue);

            configurationKey = "power";
            configurationValue = DEFAULT_POWER_STATE;
            rep.setValue(configurationKey, configurationValue);

            configurationKey = "intensity";
            configurationValue = DEFAULT_INTENSITY;
            rep.setValue(configurationKey, configurationValue);

            configurationKey = "manufacturer";
            configurationValue = DEFAULT_MANUFACTURER;
            rep.setValue(configurationKey, configurationValue);

            configurationKey = REGION_KEY;
            configurationValue = DEFAULT_REGION;
            rep.setValue(configurationKey, configurationValue);

            configurationKey = "reboot";
            configurationValue = DEFAULT_REBOOT_STATE;
            rep.setValue(configurationKey, configurationValue);

            response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
            setResourceRepresentation(rep);

            rep.getValue(configurationKey, configurationValue);
            string updatedConfiguration = configurationKey + " = " + configurationValue;
            cout << "Configuration after Factory reset: " << updatedConfiguration << endl;

        }

        result = OCPlatform::sendResponse(response);
        if (result != OC_STACK_OK)
        {
            cerr << "Unable to send response for PUT Request" << endl;
        }

        targetValue = DEFAULT_FACTORY_RESET_STATE;
        rep.setValue("value", targetValue);
    }
    else if (incomingRepresentation.hasAttribute(REBOOT_KEY))
    {
        incomingRepresentation.getValue(REBOOT_KEY, targetValue);
        cout << "Inside Reboot, target value = " << targetValue << endl;

        if (targetValue.compare("true") == 0)
        {
            cout << "Rebooting Resource " << endl;
            rep.setValue(REBOOT_KEY, targetValue);
            response->setResourceRepresentation(rep, DEFAULT_INTERFACE);

            rep.getValue(REBOOT_KEY, targetValue);
            string updatedConfiguration = REBOOT_KEY;
            updatedConfiguration = updatedConfiguration + " = " + targetValue;

            OCStackResult result = OCPlatform::sendResponse(response);
            if (result != OC_STACK_OK)
            {
                cerr << "Unable to send response for PUT Request" << endl;
            }
            int restartResult = system("sudo reboot -f");
            cout << "Roboot result = " << restartResult << endl;
        }
    }
    else
    {
        cout << "Inside default put handle" << endl;
        rep = incomingRepresentation;
        cout << "Sending notification from complete update - PUT" << endl;
        notifyObservers(this);
        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
        response->setErrorCode(COAP_RESPONSE_CODE_UPDATED);
        response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
        result = OCPlatform::sendResponse(response);
        if (result != OC_STACK_OK)
        {
            cerr << "Unable to send response for PUT Request" << endl;
        }
    }
}
