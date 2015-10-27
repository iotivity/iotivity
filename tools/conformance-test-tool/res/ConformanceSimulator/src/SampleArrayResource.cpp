//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 

/**
 * SampleArrayResource.cpp
 *
 *  Created on: Oct 19, 2015
 * @author: Mushfiqul Islam Antu(i.mushfiq@samsung.com)
 *
 */

#include "SampleArrayResource.h"

#define PRESENCE_INTERVAL 10000

SampleArrayResource::SampleArrayResource(void)
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
    double version = DEFAULT_VERSION;
    m_representation.setValue(VERSION_KEY, version);
    bool isCrudnSupported = DEFAULT_CRUDN_SUPPORT;
    m_representation.setValue(CRUDN_SUPPORT_KEY, isCrudnSupported);

    map< string, boost::any > warrantyMap;
    warrantyMap.clear();
    bool isVoid = false;
    int year = 2016;
    int month = 10;
    int day = 19;
    warrantyMap["warranty_void"] = isVoid;
    warrantyMap["valid_year"] = year;
    warrantyMap["valid_month"] = month;
    warrantyMap["valid_day"] = day;
//    m_representation.setValue("warranty_info", warrantyMap);

    vector< pair< string, boost::any > > arrayRep;
    arrayRep.push_back(pair< string, boost::any >("warranty_void", isVoid));
    arrayRep.push_back(pair< string, boost::any >("valid_year", year));
    arrayRep.push_back(pair< string, boost::any >("valid_month", month));
    arrayRep.push_back(pair< string, boost::any >("valid_day", day));
//    m_representation.setValue("Warrant_info", arrayRep);

    vector< pair< string, int > > arrayRep1;
    arrayRep1.push_back(pair< string, int >("warranty_void", isVoid));
    arrayRep1.push_back(pair< string, int >("valid_year", year));
    arrayRep1.push_back(pair< string, int >("valid_month", month));
    arrayRep1.push_back(pair< string, int >("valid_day", day));
//    m_representation.setValue("Warrant_info", arrayRep1);

    vector< int > arrayRep2;
    arrayRep2.push_back(1);
    arrayRep2.push_back(2);
    arrayRep2.push_back(3);
    arrayRep2.push_back(4);
    //m_representation.setValue("Warranty_info", arrayRep2);

    map< string, int > warrantyMap3;
    warrantyMap3.clear();
    warrantyMap3["warranty_void"] = isVoid;
    warrantyMap3["valid_year"] = year;
    warrantyMap3["valid_month"] = month;
    warrantyMap3["valid_day"] = day;
//    m_representation.setValue("warranty_info3", warrantyMap3);
//    m_representation["warranty_info3"] = warrantyMap3;
    vector< map< string, int > > hoho;
    hoho.push_back(warrantyMap3);
    hoho.push_back(warrantyMap3);
    hoho.push_back(warrantyMap3);

    OCRepresentation warrantyMap4;
    warrantyMap4.setValue("warranty_void", isVoid);
    warrantyMap4.setValue("valid_year",year);
    warrantyMap4.setValue("valid_month", month);
    warrantyMap4.setValue("valid_day", day);
    vector< OCRepresentation > warrantyList;
    OCRepresentation warrantyMap5;
    warrantyMap5["warranty_void"] = !isVoid;
    warrantyMap5["valid_year"] = year + 2;
    warrantyMap5["valid_month"] = month + 3;
    warrantyMap5["valid_day"] = day + 4;
    warrantyList.push_back(warrantyMap4);
    warrantyList.push_back(warrantyMap5);
//    m_representation.setValue("warranty_info4", warrantyList);
//    m_representation.setValue("warranty_info3", hoho);

    p_conformanceHelper = ConformanceHelper::getInstance();

    m_recursiveDelay = 0;
    m_scheduledDelay = 0;
    m_isCancelCalled = false;
    m_isObserveRegistered = false;
    m_listOfObservers.clear();
    m_resourceList.clear();

    cout << "Current resource info: " << m_representation.getPayload()->values->str << endl;

}

SampleArrayResource::~SampleArrayResource(void)
{
    // child destructor
}

void SampleArrayResource::handleObserveRequest(QueryParamsMap &queryParamsMap,
        std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    cout << "Inside handleObserveRequest... " << endl;
    // handle observe request

    ObservationInfo observationInfo = request->getObservationInfo();
    if (ObserveAction::ObserveRegister == observationInfo.action)
    {
        cout << "Adding new observer to observer list" << endl;
        cout << "Observe Info:" << observationInfo.obsId << endl;
        m_listOfObservers.push_back(observationInfo.obsId);
        m_isObserveRegistered = true;
        cout << "Sending notification from from register observer - for the first time" << endl;

        response->setErrorCode(COAP_RESPONSE_CODE_SUCCESS);
        OCStackResult result = OCPlatform::sendResponse(response);
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
            cout << "Observe Info: " << observationInfo.obsId << endl;
            m_listOfObservers.erase(
                    remove(m_listOfObservers.begin(), m_listOfObservers.end(),
                            observationInfo.obsId), m_listOfObservers.end());
            response->setErrorCode(COAP_RESPONSE_CODE_SUCCESS);
            response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
        }
        else
        {
            response->setErrorCode(COAP_RESPONSE_CODE_ERROR);
        }

        OCStackResult result = OCPlatform::sendResponse(response);
        if (result != OC_STACK_OK)
        {
            cerr << "Unable to cancel observe" << endl;
        }
    }

    cout << "\t\trequestFlag : Observer\n";

    if (m_listOfObservers.size() == 0)
    {
        m_isObserveRegistered = false;
    }
}

void SampleArrayResource::handleDeleteRequest(QueryParamsMap &queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceResponse > response)
{
    cout << "Inside handleDeleteRequest... " << endl;
    // handle delete request
    OCRepresentation rep = getRepresentation();

    cout << "THe DELETE request comprises of the following representation:" << endl;
    p_conformanceHelper->printRepresentation(incomingRepresentation);

    // handle DELETE

    response->setErrorCode(COAP_RESPONSE_CODE_DELETED);
    response->setResponseResult(OCEntityHandlerResult::OC_EH_RESOURCE_DELETED);

    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        cerr << "Unable to send response for Delete Request" << endl;
    }
    this->stopServer();
}

void SampleArrayResource::handlePostRequest(QueryParamsMap &queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceResponse > response)
{
    cout << "Inside handlePostRequest... " << endl;
    cout << "THe POST request comprises of the following representation:" << endl;
    p_conformanceHelper->printRepresentation(incomingRepresentation);

    // handle POST request
    response->setErrorCode(COAP_RESPONSE_CODE_ERROR);

    string targetValue = "";
    string uriValue = "";
    OCRepresentation rep = getRepresentation();
    m_pResponse = response;

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
            int actionSetType = stoi(actionSetInfo.substr(actionSetInfo.find_first_of(" ") + 1));

            if (actionSetType == 2)
            {
                m_recursiveDelay = delay;
                m_isCancelCalled = false;

                std::thread recursive(&SampleArrayResource::handleRecursiveActionSet, this);
                recursive.detach();

            }
            else if (actionSetType == 1)
            {
                m_scheduledDelay = delay;
                m_isCancelCalled = false;

                std::thread sceduled(&SampleArrayResource::handleScheduledActionSet, this);
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
            int delaySec = stoi(scheduleWithDelay.substr(scheduleWithDelay.find_last_of("*") + 1));
            cout << "Delay time in second is: " << delaySec << endl;
            CommonUtil::waitInSecond(delaySec);

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
        bool isRepUpdated = false;
        if (incomingRepresentation.hasAttribute(REGION_KEY))
        {
            updateRepresentation(REGION_KEY, incomingRepresentation, response);
            isRepUpdated = true;
        }
        if (incomingRepresentation.hasAttribute(POWER_KEY))
        {
            updateRepresentation(POWER_KEY, incomingRepresentation, response);
            isRepUpdated = true;
        }
        if (incomingRepresentation.hasAttribute(MANUFACTURER_KEY))
        {
            updateRepresentation(MANUFACTURER_KEY, incomingRepresentation, response);
            isRepUpdated = true;
        }
        if (incomingRepresentation.hasAttribute(INTENSITY_KEY))
        {
            updateRepresentation(INTENSITY_KEY, incomingRepresentation, response);
            isRepUpdated = true;
        }
        if (incomingRepresentation.hasAttribute(VERSION_KEY))
        {
            updateRepresentation(VERSION_KEY, incomingRepresentation, response);
            isRepUpdated = true;
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
            cout << "Incoming Representation not supported by this resource!!" << endl;
            response->setErrorCode(405);
            response->setResponseResult(OCEntityHandlerResult::OC_EH_ERROR);
        }

    }

    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        cerr << "Unable to send response for POST Request" << endl;
    }
}

void SampleArrayResource::handleGetRequest(QueryParamsMap &queryParamsMap,
        std::shared_ptr< OCResourceResponse > response)
{
    cout << "Inside handleGetRequest... " << endl;
//handle GET request
    response->setErrorCode(COAP_RESPONSE_CODE_RETRIEVED);
    OCRepresentation rep = m_representation;
    cout << "Current Resource Representation to send : " << endl;
    p_conformanceHelper->printRepresentation(rep);

    response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
    response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        cerr << "Unable to send response for GET Request" << endl;
    }
}

void SampleArrayResource::handlePutRequest(QueryParamsMap &queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceResponse > response)
{
    OCRepresentation rep = getRepresentation();

    cout << "Inside handlePutRequest... " << endl;
    cout << "The PUT request has the following representation: " << endl;
    p_conformanceHelper->printRepresentation(incomingRepresentation);

// handle PUT request
    response->setErrorCode(COAP_RESPONSE_CODE_SUCCESS);

    string targetValue = "";
    string updateKey = "power";
    string uriValue = "";
    OCStackResult result = OC_STACK_OK;

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

    m_representation = rep;
}

void SampleArrayResource::onResourceServerStarted(bool &isRegisteredForPresence,
        int &presenceInterval)
{
    cout << "Inside onResourceServerStarted... " << endl;
    isRegisteredForPresence = true;
    presenceInterval = PRESENCE_INTERVAL;

}

void SampleArrayResource::handleInitRequest(QueryParamsMap &queryParamsMap,
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

OCRepresentation SampleArrayResource::getResourceRepresentation(OCRepresentation &resourceRep)
{
    return resourceRep;
}

void SampleArrayResource::handleRecursiveActionSet()
{
    OCRepresentation rep = getRepresentation();
    string targetValue = "on";
    string targetKey = "power";

    while (!m_isCancelCalled)
    {
        CommonUtil::waitInSecond(m_recursiveDelay);
        rep.setValue(targetKey, targetValue);
        m_representation = rep;
        string toShow = "Recursive ActionSet Executed!! Current Value : " + targetKey + " = "
                + targetValue;
        cout << toShow << endl;

    }

}

void SampleArrayResource::handleScheduledActionSet()
{
    OCRepresentation rep = getRepresentation();
    string targetValue = "on";
    string targetKey = "power";

    while (m_scheduledDelay > 0 && m_isCancelCalled == false)
    {
        CommonUtil::waitInSecond(1);
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

void SampleArrayResource::notifyObservers(void *param)
{
    SampleArrayResource *resource = (SampleArrayResource *) param;

    cout << "Inside notifyObservers....." << endl;
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
            m_isObserveRegistered = false;
        }
    }
    else
    {
        cout << "No observers available to notify!!" << endl;
    }

}

bool SampleArrayResource::updateRepresentation(string key, OCRepresentation incomingRep,
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

void SampleArrayResource::createResource(string initialUri, OCRepresentation incomingRepresentation,
        shared_ptr< OCResourceResponse > response)
{
    cout << "Creating new resource!!" << endl;
    string uriValue = incomingRepresentation.getUri();
    string resourceUri = initialUri + uriValue;
    OCStackResult result = OC_STACK_OK;
    cout << "The CREATE request consists of the following representation:" << endl;
    p_conformanceHelper->printRepresentation(incomingRepresentation);

    if (resourceUri.compare(m_resourceURI) != 0)
    {
        SampleArrayResource *newResource;

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
            if (resourceType.compare(NON_DISCOVERABLE_RESOURCE_TYPE) == 0)
            {
                isDiscoverable = false;
            }
            cout << "Creating resource using resource type = " << resourceType
                    << ", and resource uri = " << resourceUri << endl;
            try
            {
                newResource = new SampleArrayResource();
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
                result = newResource->startServer(resourceProperty);

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
