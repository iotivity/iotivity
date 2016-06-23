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

/**
 * SampleResource.cpp
 *
 *  Created on: Nov 25, 2014
 *
 */

#include "SampleResource.h"

#define PRESENCE_INTERVAL 10000

SampleResource::SampleResource(void)
{
    //child constructor

    string value = DEFAULT_POWER_STATE;
    m_representation.setValue("power", value);
    value = DEFAULT_INTENSITY;
    m_representation.setValue("intensity", value);
    value = DEFAULT_MANUFACTURER;
    m_representation.setValue("manufacturer", value);
    value = DEFAULT_REGION;
    m_representation.setValue("region", value);
    value = DEFAULT_FACTORY_RESET_STATE;
    m_representation.setValue("value", value);
    value = DEFAULT_REBOOT_STATE;
    m_representation.setValue("rb", value);
    value = "";
    m_representation.setValue("ActionSet", value);

    m_recursiveDelay = 0;
    m_scheduledDelay = 0;
    m_isCancelCalled = false;
    m_isObserveRegistered = false;
    m_listOfObservers.clear();
    m_resourceList.clear();

    IOTIVITYTEST_LOG(INFO, "Current resource info: %s",
            m_representation.getPayload()->values->str);

}

SampleResource::~SampleResource(void)
{
    // child destructor
}

void SampleResource::handleObserveRequest(QueryParamsMap& queryParamsMap,
        std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    IOTIVITYTEST_LOG(DEBUG, "Inside handleObserveRequest... ");
    // handle observe request

    ObservationInfo observationInfo = request->getObservationInfo();
    if (ObserveAction::ObserveRegister == observationInfo.action)
    {
        cout << "Adding new observer to observer list" << endl;
        cout << "Observe Info:" << observationInfo.obsId << endl;
        m_listOfObservers.push_back(observationInfo.obsId);
        m_isObserveRegistered = true;
        notifyObservers(this);
    }
    else if (ObserveAction::ObserveUnregister == observationInfo.action)
    {
        if (m_listOfObservers.size() > 0){
            cout << "Removing observer from observer list" << endl;
            cout << "Observe Info:" << observationInfo.obsId << endl;
            m_listOfObservers.erase(
                    remove(m_listOfObservers.begin(), m_listOfObservers.end(), observationInfo.obsId),
                    m_listOfObservers.end());
            response->setErrorCode(200);
        }else{
            response->setErrorCode(400);
        }

        OCStackResult result = OCPlatform::sendResponse(response);
        if (result != OC_STACK_OK)
        {
            IOTIVITYTEST_LOG(ERROR, "Unable to cancel observe");
        }
    }

    cout << "\t\trequestFlag : Observer\n";

    if (m_listOfObservers.size() == 0)
    {
        m_isObserveRegistered = false;
    }
}

void SampleResource::handleDeleteRequest(QueryParamsMap& queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceResponse > response)
{
    IOTIVITYTEST_LOG(DEBUG, "Inside handleDeleteRequest... ");
    // handle delete request
    OCRepresentation rep = getRepresentation();

    IOTIVITYTEST_LOG(DEBUG, "Inside handlePutRequest... ");
    IOTIVITYTEST_LOG(INFO, "Incoming JSON Representation is: %s",
            incomingRepresentation.getPayload()->values->str);

    // handle DELETE

    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        IOTIVITYTEST_LOG(ERROR, "Unable to send response for Delete Request");
    }
    this->stopServer();
}

void SampleResource::handlePostRequest(QueryParamsMap& queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceResponse > response)
{
    IOTIVITYTEST_LOG(DEBUG, "Inside handlePostRequest... ");
    IOTIVITYTEST_LOG(INFO, "Incoming JSON Representation is: %s",
            incomingRepresentation.getPayload()->values->str);
    printIncomingRepresentation(incomingRepresentation);
    // handle POST request
    response->setErrorCode(400);

    string executeActionSetKey = "DoAction";
    string executeScheduledActionSetKey = "DoScheduledAction";
    string cancelActionSetKey = "CancelAction";
    string getActionSetKey = "GetActionSet";
    string actionSetKey = "ActionSet";
    string regionKey = "region";
    string powerKey = "power";
    string manufacturerKey = "manufacturer";
    string intensityKey = "intensity";
    string targetValue = "";
    string uriKey = "href";
    string uriValue = "";
    OCRepresentation rep = getRepresentation();
    IOTIVITYTEST_LOG(INFO, "Resource representation  is: ");
    printIncomingRepresentation(rep);
    m_pResponse = response;


    if (incomingRepresentation.hasAttribute(executeActionSetKey))
    {
        string actionSet = "";
        rep.getValue(actionSetKey, actionSet);
        IOTIVITYTEST_LOG(INFO, "Actionset name  is: %s", actionSet.c_str());
        if (actionSet.compare("") != 0)
        {
            IOTIVITYTEST_LOG(DEBUG, "Executing ActionSet...");
            string targetValue = "on";
            string targetKey = "power";

            int startPos = actionSet.find_first_of("*") + 1;
            int endPos = actionSet.find_first_of("*", startPos);
            string actionSetInfo = actionSet.substr(startPos, endPos - startPos);
            IOTIVITYTEST_LOG(INFO, actionSetInfo.c_str());
            int delay = stoi(actionSetInfo.substr(0, actionSetInfo.find_first_of(" ")));
            int actionSetType = stoi(actionSetInfo.substr(actionSetInfo.find_first_of(" ") + 1));

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
                response->setErrorCode(204);
                response->setResourceRepresentation(rep, DEFAULT_INTERFACE);

                rep.getValue(targetKey, targetValue);
                if (targetValue.compare("on") == 0)
                {
                    IOTIVITYTEST_LOG(INFO, "Successfully executed ActionSet");
                }
                else
                {
                    IOTIVITYTEST_LOG(WARNING, "Failed to execute ActionSet");
                }

                string updatedConfiguration = targetKey + " = " + targetValue;
                IOTIVITYTEST_LOG(INFO, "Updated Configuration: %s", updatedConfiguration.c_str());
            }

        }

    }
    else if (incomingRepresentation.hasAttribute(executeScheduledActionSetKey))
    {
        string actionSet = "";
        rep.getValue(actionSetKey, actionSet);
        if (actionSet.compare("") != 0)
        {
            IOTIVITYTEST_LOG(DEBUG, "Executing Scheduled ActionSet...");
            string scheduleWithDelay = "";
            incomingRepresentation.getValue(executeScheduledActionSetKey, scheduleWithDelay);
            IOTIVITYTEST_LOG(INFO, scheduleWithDelay.c_str());
            int delaySec = stoi(scheduleWithDelay.substr(scheduleWithDelay.find_last_of("*") + 1));
            IOTIVITYTEST_LOG(INFO, to_string(delaySec).c_str());
            CommonUtil::waitInSecond(delaySec);

            string targetValue = "on";
            string targetKey = "power";
            rep.setValue(targetKey, targetValue);

            m_representation = rep;

            rep.getValue(targetKey, targetValue);
            if (targetValue.compare("on") == 0)
            {
                IOTIVITYTEST_LOG(INFO, "Successfully executed Scheduled ActionSet");
            }
            else
            {
                IOTIVITYTEST_LOG(WARNING, "Failed to execute Scheduled ActionSet");
            }

            string updatedConfiguration = targetKey + " = " + targetValue;
            IOTIVITYTEST_LOG(INFO, "Updated Configuration: %s", updatedConfiguration.c_str());
        }
        response->setErrorCode(204);
        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
    }
    else if (incomingRepresentation.hasAttribute(cancelActionSetKey))
    {
        m_isCancelCalled = true;
        response->setErrorCode(204);
        response->setResourceRepresentation(m_representation, DEFAULT_INTERFACE);

    }
    else if (incomingRepresentation.hasAttribute(getActionSetKey))
    {
        response->setErrorCode(204);
        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
    }
    else if (incomingRepresentation.hasAttribute(uriKey))
    {
        incomingRepresentation.getValue(uriKey, uriValue);

        if (uriValue.compare(m_resourceURI) != 0)
        {
            string initialUri = this->getUri();
            this->createResource(initialUri, incomingRepresentation, response);
        }

    }
    else
    {
        if (incomingRepresentation.hasAttribute(regionKey))
        {
            updateRepresentation(regionKey, incomingRepresentation, response);
        }
        if (incomingRepresentation.hasAttribute(powerKey))
        {
            updateRepresentation(powerKey, incomingRepresentation, response);
        }
        if (incomingRepresentation.hasAttribute(manufacturerKey))
        {
            updateRepresentation(manufacturerKey, incomingRepresentation, response);
        }
        if (incomingRepresentation.hasAttribute(intensityKey))
        {
            updateRepresentation(intensityKey, incomingRepresentation, response);
        }
    }

    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        IOTIVITYTEST_LOG(ERROR, "Unable to send response for POST Request");
    }
}

void SampleResource::handleGetRequest(QueryParamsMap& queryParamsMap,
        std::shared_ptr< OCResourceResponse > response)
{
    IOTIVITYTEST_LOG(DEBUG, "Inside handleGetRequest... ");
//handle GET request
    response->setErrorCode(205);
    OCRepresentation rep = m_representation;
    IOTIVITYTEST_LOG(INFO, "Current Resource Representaion in JSON : ",
            rep.getPayload()->values->str);

    response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        IOTIVITYTEST_LOG(ERROR, "Unable to send response for GET Request");
    }
}

void SampleResource::handlePutRequest(QueryParamsMap& queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceResponse > response)
{
    OCRepresentation rep = getRepresentation();

    IOTIVITYTEST_LOG(DEBUG, "Inside handlePutRequest... ");
    IOTIVITYTEST_LOG(INFO, "Incoming JSON Representation is: \n");
    printIncomingRepresentation(incomingRepresentation);

// handle PUT request
    response->setErrorCode(200);

    string targetValue = "";
    string updateKey = "power";
    string resetKey = "value";
    string rebootKey = "rb";
    string regionKey = "region";
    string actionSetKey = "ActionSet";
    string deleteActionSetKey = "DelActionSet";
    string uriKey = "href";
    string uriValue = "";
    OCStackResult result = OC_STACK_OK;

    if (incomingRepresentation.hasAttribute(uriKey))
    {
        incomingRepresentation.getValue(uriKey, uriValue);

        if (uriValue.compare(m_resourceURI) != 0)
        {
            createResource("", incomingRepresentation, response);
        }
        else
        {
            rep = incomingRepresentation;
            notifyObservers(this);

            response->setResourceRepresentation(rep, DEFAULT_INTERFACE);

        }

        result = OCPlatform::sendResponse(response);
        if (result != OC_STACK_OK)
        {
            IOTIVITYTEST_LOG(ERROR, "Unable to send response for PUT Request");
        }

    }
//    else if (incomingRepresentation.hasAttribute(updateKey))
//    {
//        updateRepresentation(updateKey, incomingRepresentation, response);
//    }
    else if (incomingRepresentation.hasAttribute(actionSetKey))
    {
        updateRepresentation(actionSetKey, incomingRepresentation, response);
        result = OCPlatform::sendResponse(response);
        if (result != OC_STACK_OK)
        {
            IOTIVITYTEST_LOG(ERROR, "Unable to send response for PUT Request");
        }
    }
    else if (incomingRepresentation.hasAttribute(deleteActionSetKey))
    {
        incomingRepresentation.getValue(deleteActionSetKey, targetValue);
        IOTIVITYTEST_LOG(DEBUG, "Deleting ActionSet, with target = %s", targetValue.c_str());

        if (targetValue.compare("") != 0)
        {
            IOTIVITYTEST_LOG(DEBUG, "Deleting ActionSet from representation... ");
            string configurationKey = actionSetKey;
            string configurationValue = "";

            IOTIVITYTEST_LOG(INFO, "System JSON Representation: %s",
                    rep.getPayload()->values->str);
            rep.setValue(configurationKey, configurationValue);
            response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
            setResourceRepresentation(rep);
            IOTIVITYTEST_LOG(INFO, "Updated System JSON Representation: %s",
                    rep.getPayload()->values->str);

            m_representation = rep;
            m_representation.getValue(configurationKey, configurationValue);
            string updatedConfiguration = configurationKey + " = " + configurationValue;
            IOTIVITYTEST_LOG(INFO, "Updated Configuration: %s", updatedConfiguration.c_str());

        }

        result = OCPlatform::sendResponse(response);
        if (result != OC_STACK_OK)
        {
            IOTIVITYTEST_LOG(ERROR, "Unable to send response for PUT Request");
        }
    }
    else if (incomingRepresentation.hasAttribute(resetKey))
    {
        incomingRepresentation.getValue(resetKey, targetValue);
        IOTIVITYTEST_LOG(DEBUG, "Inside Factory Reset, target value = %s", targetValue.c_str());

        if (targetValue.compare("true") == 0)
        {
            IOTIVITYTEST_LOG(INFO, "Factory Reset Going on....");

            string configurationKey = resetKey;
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

            configurationKey = regionKey;
            configurationValue = DEFAULT_REGION;
            rep.setValue(configurationKey, configurationValue);

            configurationKey = "reboot";
            configurationValue = DEFAULT_REBOOT_STATE;
            rep.setValue(configurationKey, configurationValue);

            response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
            setResourceRepresentation(rep);

            rep.getValue(configurationKey, configurationValue);
            string updatedConfiguration = configurationKey + " = " + configurationValue;
            IOTIVITYTEST_LOG(INFO, "Configuration after Factory reset: %s",
                    updatedConfiguration.c_str());

        }

        result = OCPlatform::sendResponse(response);
        if (result != OC_STACK_OK)
        {
            IOTIVITYTEST_LOG(ERROR, "Unable to send response for PUT Request");
        }

        targetValue = DEFAULT_FACTORY_RESET_STATE;
        rep.setValue("value", targetValue);

        m_representation = rep;
    }
    else if (incomingRepresentation.hasAttribute(rebootKey))
    {
        incomingRepresentation.getValue(rebootKey, targetValue);
        IOTIVITYTEST_LOG(INFO, "Inside Reboot, target value = %s", targetValue.c_str());

        if (targetValue.compare("true") == 0)
        {
            IOTIVITYTEST_LOG(DEBUG, "Rebooting Resource ");
            rep.setValue(rebootKey, targetValue);
            response->setResourceRepresentation(rep, DEFAULT_INTERFACE);

            m_representation = rep;
            m_representation.getValue(rebootKey, targetValue);
            string updatedConfiguration = rebootKey + " = " + targetValue;

            OCStackResult result = OCPlatform::sendResponse(response);
            if (result != OC_STACK_OK)
            {
                IOTIVITYTEST_LOG(ERROR, "Unable to send response for PUT Request");
            }
            int restartResult = system("sudo reboot -f");
            IOTIVITYTEST_LOG(INFO, "Roboot result = %d", restartResult);
        }
    }
//    else if (incomingRepresentation.hasAttribute(regionKey))
//    {
//        updateRepresentation(regionKey, incomingRepresentation, response);
//    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "Inside default put handle");
        rep = incomingRepresentation;
        notifyObservers(this);
        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
        response->setErrorCode(204);
        m_representation = rep;
        result = OCPlatform::sendResponse(response);
        if (result != OC_STACK_OK)
        {
            IOTIVITYTEST_LOG(ERROR, "Unable to send response for PUT Request");
        }
    }


}

void SampleResource::onResourceServerStarted(bool& isRegisteredForPresence, int& presenceInterval)
{
    IOTIVITYTEST_LOG(DEBUG, "Inside onResourceServerStarted... ");
    isRegisteredForPresence = true;
    presenceInterval = PRESENCE_INTERVAL;

// perform additional task
}

void SampleResource::handleInitRequest(QueryParamsMap& queryParamsMap,
        std::shared_ptr< OCResourceResponse > response)
{
    IOTIVITYTEST_LOG(DEBUG, "Inside handleInitRequest... ");

// handle initialization
    response->setErrorCode(200);

    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        IOTIVITYTEST_LOG(ERROR, "Unable to send response for Init Request");
    }
}

OCRepresentation SampleResource::getResourceRepresentation(OCRepresentation & resourceRep)
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
        CommonUtil::waitInSecond(m_recursiveDelay);
        rep.setValue(targetKey, targetValue);
        m_representation = rep;
        string toShow = "Recursive ActionSet Executed!! Current Value : " + targetKey + " = "
                + targetValue;
        IOTIVITYTEST_LOG(INFO, toShow.c_str());
//        m_pResponse->setResourceRepresentation(m_representation, DEFAULT_INTERFACE);
//        OCPlatform::sendResponse(m_pResponse);
    }

}

void SampleResource::handleScheduledActionSet()
{
    OCRepresentation rep = getRepresentation();
    string targetValue = "on";
    string targetKey = "power";

    while (m_scheduledDelay > 0 && m_isCancelCalled == false)
    {
        CommonUtil::waitInSecond(1);
        IOTIVITYTEST_LOG(DEBUG, "Waiting for scheduled time to come");
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
    IOTIVITYTEST_LOG(INFO, toShow.c_str());
//    m_pResponse->setResourceRepresentation(m_representation, DEFAULT_INTERFACE);
//    OCPlatform::sendResponse(m_pResponse);

}

void SampleResource::notifyObservers(void *param)
{
    SampleResource* resource = (SampleResource*) param;

    cout << "Inside notifyObservers....." << endl;
    if (m_isObserveRegistered)
    {
        cout << "Sending Notification to Observers...." << endl;
        std::shared_ptr< OCResourceResponse > resourceResponse =
        { std::make_shared< OCResourceResponse >() };

        resourceResponse->setErrorCode(200);
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
    else{
        cout << "No observers available to notify!!" << endl;
    }

}

bool SampleResource::updateRepresentation(string key, OCRepresentation incomingRep,
        shared_ptr< OCResourceResponse > response)
{
    bool result = false;
    OCRepresentation rep = getRepresentation();

    string targetValue = "";
    incomingRep.getValue(key, targetValue);
    IOTIVITYTEST_LOG(DEBUG, "Target Value = %s", targetValue.c_str());

    if (targetValue.compare("") != 0)
    {
        IOTIVITYTEST_LOG(DEBUG, "Updating Representation... ");
        string configurationKey = key;
        string configurationValue = "";
        rep.setValue(configurationKey, targetValue);
        rep.getValue(configurationKey, configurationValue);
        if (targetValue.compare(configurationValue) == 0)
        {
            response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
            setResourceRepresentation(rep);
            response->setErrorCode(204);
            m_representation = rep;
            m_representation.getValue(configurationKey, configurationValue);

            string updatedConfiguration = configurationKey + " = " + configurationValue;
            IOTIVITYTEST_LOG(INFO, "Updated Representation: ");
            printIncomingRepresentation(getRepresentation());
            result = true;
            notifyObservers(this);
        }
        else
        {
            result = false;
            response->setErrorCode(400);
        }

    }

    return result;
}

void SampleResource::createResource(string initialUri, OCRepresentation incomingRepresentation,
        shared_ptr< OCResourceResponse > response)
{
    string uriKey = "href";
    string uriValue = "";
    OCStackResult result = OC_STACK_OK;
    incomingRepresentation.getValue(uriKey, uriValue);

    if (uriValue.compare(m_resourceURI) != 0)
    {
        SampleResource* newResource;
        string resourceUri = initialUri + uriValue;
        string resourceType = "";
        string resourceTypeKey = "rt";
        string discoverableKey = "dis";
        bool isDiscoverable = true;

        incomingRepresentation.getValue(resourceTypeKey, resourceType);

        OCRepresentation blankRep;
        if (resourceType.compare("") == 0)
        {
            response->setErrorCode(400);
            response->setResourceRepresentation(blankRep, DEFAULT_INTERFACE);
        }
        else
        {
            if (incomingRepresentation.hasAttribute(discoverableKey)){
                string discoverable = "";
                incomingRepresentation.getValue(discoverableKey, discoverable);
                if(discoverable.compare("0") == 0){
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
                if (isDiscoverable){
                    resourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE;
                }else{
                    resourceProperty = OC_ACTIVE;
                }

                result = (OCStackResult) ((int) result + (int) newResource->startServer(resourceProperty));
                cout << "resource started!!" << endl;

            }
            catch (const exception& e)
            {
                cout << e.what() << endl;
            }

            if (result != OC_STACK_OK)
            {
                response->setErrorCode(400);
                response->setResourceRepresentation(blankRep, DEFAULT_INTERFACE);
            }
            else
            {
                response->setErrorCode(201);
                response->setResourceRepresentation(newResource->getRepresentation(),
                        DEFAULT_INTERFACE);
                m_resourceList.push_back(resourceUri);
            }

        }
    }
}

void SampleResource::printIncomingRepresentation(OCRepresentation rep)
{
    OCRepPayload *incomingPayload = rep.getPayload();
    cout << "The representation has uri: " << incomingPayload->uri << endl;

    while (incomingPayload)
    {
        OCRepPayloadValue *repValue = incomingPayload->values;
        cout << "The representation has following value: " << endl;
        while (repValue)
        {
            string value = "";
            if (repValue->type == OCRepPayloadPropType::OCREP_PROP_INT)
            {
                value = std::to_string(repValue->i);
            }
            if (repValue->type == OCRepPayloadPropType::OCREP_PROP_DOUBLE)
            {
                value = std::to_string(repValue->d);
            }
            if (repValue->type == OCRepPayloadPropType::OCREP_PROP_BOOL)
            {
                value = repValue->b ? "true" : "false";
            }
            if (repValue->type == OCRepPayloadPropType::OCREP_PROP_STRING)
            {
                value = std::string(repValue->str);
            }
            cout << "\t\t\t" << repValue->name << " : " << value << endl;
            repValue = repValue->next;
        }
        incomingPayload = incomingPayload->next;
    }
}

