/******************************************************************
*
* Copyright 2017 Samsung Electronics All Rights Reserved.
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

#include "SampleCollection.h"

#define PRESENCE_INTERVAL 10000

SampleCollection::SampleCollection(void)
{
    //child constructor
    m_isCancelCalled = false;
    m_isObserveRegistered = false;
    m_isSecured = true;
    m_ipVer = CT_IP_USE_V4;
    m_collectionName = "";
    m_listOfObservers.clear();
    m_childResourceList.clear();
    m_accessmodifier.clear();
}

SampleCollection::~SampleCollection(void)
{
    // child destructor
}

void SampleCollection::handleObserveRequest(QueryParamsMap &queryParamsMap,
        std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    cout << "Inside handleObserveRequest... " << endl;
    handleGetRequest(queryParamsMap, request, response);
}

void SampleCollection::handleDeleteRequest(QueryParamsMap &queryParamsMap,
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
                    response->setResponseResult(OCEntityHandlerResult::OC_EH_FORBIDDEN);
                    shouldDelete = false;
                    break;
                }
            }
        }
    }

    if (shouldDelete)
    {
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

void SampleCollection::handlePostRequest(QueryParamsMap &queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    OCStackResult result = OC_STACK_ERROR;

    cout << "Inside handlePostRequest... " << endl;
    cout << "THe POST request comprises of the following representation:" << endl;
    p_resourceHelper->printRepresentation(incomingRepresentation);

    vector< OCRepresentation > allChildren = incomingRepresentation.getChildren();
    for(std::vector<OCRepresentation>::iterator it = allChildren.begin(); it != allChildren.end(); ++it)
    {
        p_resourceHelper->printRepresentation(*it);
    }
    allChildren.insert(allChildren.begin(), incomingRepresentation);

    // handle POST request
    string targetValue = "";
    string uriValue = "";
    OCRepresentation rep = getRepresentation();

    bool shouldReturnError = false;
    bool invalidRepUpdate = false;
    string responseInterface = DEFAULT_INTERFACE;

    cout << "Current Resource Representation to send : " << endl;

    if (queryParamsMap.size() > 0)
    {
        for (const auto &eachQuery : queryParamsMap)
        {
            string key = eachQuery.first;
            string queryValue = eachQuery.second;
            if (key.compare(INTERFACE_KEY) == 0)
            {
                responseInterface = key;
                vector< string > interfaceList;
                vector< string > resourceTypeList;
                interfaceList.push_back(LINK_INTERFACE);
                interfaceList.push_back(BATCH_INTERFACE);
                interfaceList.push_back(DEFAULT_INTERFACE);

                resourceTypeList.push_back(GROUP_TYPE_DEFAULT);
                if (queryValue.compare(DEFAULT_INTERFACE) == 0 )
                {
                    cout << "Found baseline query, adding rt & if into response payload" << endl;

                    if (incomingRepresentation.hasAttribute( NAME_KEY))
                    {
                        incomingRepresentation.getValue(NAME_KEY, m_collectionName);
                        rep.setValue(NAME_KEY, m_collectionName);
                        response->setResourceRepresentation(rep, responseInterface);
                    }
                    else
                    {
                        shouldReturnError = true;
                    }
                }
                else if (queryValue.compare(READ_ONLY_INTERFACE) == 0)
                {
                    cout << "Found readonly query, update not supported" << endl;
                    shouldReturnError = true;

                }
                else if (queryValue.compare(LINK_INTERFACE) == 0)
                {
                    cout << "linked list interface query is not supported for update"
                            << endl;
                    shouldReturnError = true;
                }
                else if (queryValue.compare(BATCH_INTERFACE) == 0)
                {
                    cout << "Responding to batch interface query" << endl;
                    OCRepresentation batchRep;
                    OCRepresentation tempRep;
                    vector< OCRepresentation > tempRepList;
                    bool isFirstTime = true;
                    bool isUpdated = false;

                    for(std::vector<OCRepresentation>::iterator it = allChildren.begin(); it != allChildren.end(); ++it)
                    {
                        bool isError = false;
                        OCRepPayloadValue *keyValues = it->getPayload()->values->obj->values;
                        while(keyValues)
                        {
                            vector< shared_ptr< SampleResource > > matchedChildren = getChildResourcesFromRepKey(keyValues->name);
                            for (shared_ptr< SampleResource > child : matchedChildren)
                            {
                                isUpdated = child->updateBatchRepresentation(keyValues->name, *it, isError);
                                if (isError)
                                {
                                    invalidRepUpdate = true;
                                    //break;
                                }
                                if (isUpdated && isFirstTime)
                                {
                                    batchRep.setValue(URI_KEY, it->getUri());
                                    batchRep.setValue(REPRESENTATION_KEY, child->getRepresentation());
                                    isFirstTime = false;
                                }
                                else if (isUpdated && !isFirstTime)
                                {
                                    tempRep.setValue(URI_KEY, it->getUri());
                                    tempRep.setValue(REPRESENTATION_KEY, child->getRepresentation());
                                    tempRepList.push_back(tempRep);
                                }
                            }
                            keyValues = keyValues->next;
                        }
                    }

                    if (tempRepList.size() > 0)
                    {
                        batchRep.setChildren(tempRepList);
                    }

                    if (isFirstTime)
                    {
                        shouldReturnError = true;
                        break;
                    }
                    else
                    {
                        response->setResourceRepresentation(batchRep, BATCH_INTERFACE);
                    }
                }
                else
                {
                    cout << "The interface  used in query is not supported by this resource"<< endl;
                    shouldReturnError = true;
                    break;
                }

                continue;
            }
        }
    }
    else
    {
        cout << "No query found!!" << endl;
        shouldReturnError = true;
    }

    if (shouldReturnError)
    {
        cout << "sending forbidden POST response" << endl;
        response->setResourceRepresentation(OCRepresentation(), responseInterface);
        response->setResponseResult(OCEntityHandlerResult::OC_EH_FORBIDDEN);
    }
    else if (invalidRepUpdate)
    {
        cout << "sending forbidden POST response with payload" << endl;
        response->setResponseResult(OCEntityHandlerResult::OC_EH_FORBIDDEN);
    }
    else
    {
        cout << "sending normal POST response" << endl;
        response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
    }

    try
    {
        result = OCPlatform::sendResponse(response);
    }
    catch (exception& e)
    {
        cout << "Exception occurred while sending response. Exception is: " << e.what()
                << endl;
    }
    if (result != OC_STACK_OK)
    {
        cerr << "Unable to send response for POST Request" << endl;
    }

}

void SampleCollection::handleGetRequest(QueryParamsMap &queryParamsMap,
        std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    cout << "Inside handleGetRequest... " << endl;
    OCStackResult result = OC_STACK_ERROR;
    bool shouldReturnError = false;
    string responseInterface = LINK_INTERFACE;

    OCRepresentation rep = m_representation;
    cout << "Current Resource Representation to send : " << endl;
    p_resourceHelper->printRepresentation(rep);

    OCRepresentation completeRep;
    OCRepresentation epRep;
    vector< OCRepresentation > epLiist;
    //string anchor = ANCHOR_DEFAULT_VALUE + m_di;
    string ep = EP_DEFAULT_VALUE;
    OCRepresentation pRep;
    vector< OCRepresentation > allChildren;
    uint8_t bm = 2;
    CATransportFlags_t flag = CA_DEFAULT_FLAGS;
    if (m_isSecured)
    {
        flag = (CATransportFlags_t)(flag | CA_SECURE);
    }

    if (m_ipVer == CT_IP_USE_V4)
    {
        flag = (CATransportFlags_t)(flag | CA_IPV4);
    }
    else
    {
        flag = (CATransportFlags_t)(flag | CA_IPV6);
    }
    ep = ep + to_string(CAGetAssignedPortNumber(CA_ADAPTER_IP, flag));
    //    pRep.setValue(PORT_KEY, CAGetAssignedPortNumber(CA_ADAPTER_IP, flag));

    epRep.setValue(PRI_KEY, PRI_DEFAULT_VALUE);
    epRep.setValue(EP_KEY, ep);
    epLiist.push_back(epRep);
    pRep.setValue(BITMASK_KEY, bm);
    rep.setValue(URI_KEY, m_childResourceList[0]->getUri());
    rep.setValue(DEVICE_ID_KEY, m_di);
    rep.setValue(RESOURCE_TYPE_KEY, m_childResourceList[0]->getResourceTypes());
    rep.setValue(INTERFACE_KEY, m_childResourceList[0]->getResourceInterfaces());
    rep.setValue(POLICY_KEY, pRep);
    rep.setValue(EPS_KEY, epLiist);

//                addIntoLinksArray(allChildren, g_acSwitchResourceHidden);

    for (unsigned int i = 1; i <  m_childResourceList.size(); i++)
    {
        addIntoLinksArray(allChildren, m_childResourceList[i]);
    }

    rep.setChildren(allChildren);

    cout << "Current Resource Representation to send : " << endl;

    if (queryParamsMap.size() > 0)
    {
        for (const auto &eachQuery : queryParamsMap)
        {
            string key = eachQuery.first;
            string queryValue = eachQuery.second;
            if (key.compare(INTERFACE_KEY) == 0)
            {
                responseInterface = key;

                if (queryValue.compare(DEFAULT_INTERFACE) == 0 || queryValue.compare(READ_ONLY_INTERFACE) == 0)
                {
                    cout << "Found baseline/readonly query, adding rt & if into response payload" << endl;

                    setBaselineResponse(response);

                }
                else if (queryValue.compare(LINK_INTERFACE) == 0)
                {
                    cout << "Responding to linked list interface query" << endl;

                    response->setResourceRepresentation(rep, responseInterface);
                }
                else if (queryValue.compare(BATCH_INTERFACE) == 0)
                {
                    cout << "Responding to batch interface query" << endl;
                    OCRepresentation batchRep;
                    batchRep.setValue(URI_KEY, m_childResourceList[0]->getUri());
                    batchRep.setValue(REPRESENTATION_KEY,
                            m_childResourceList[0]->getRepresentation());

                    OCRepresentation tempRep;
                    vector< OCRepresentation > tempRepList;

                    for (unsigned int i = 1; i <  m_childResourceList.size(); i++)
                    {
                        tempRep.setValue(URI_KEY, m_childResourceList[i]->getUri());
                        tempRep.setValue(REPRESENTATION_KEY, m_childResourceList[i]->getRepresentation());
                        tempRepList.push_back(tempRep);
                    }

                    batchRep.setChildren(tempRepList);
                    response->setResourceRepresentation(batchRep, BATCH_INTERFACE);
                }
                else
                {
                    cout
                            << "The interface  used in query is not supported by this resource"
                            << endl;
                    shouldReturnError = true;
                    break;
                }

                continue;
            }
            else if (key.compare(RESOURCE_TYPE_KEY) == 0)
            {
                vector< string > resourceTypeList;
                vector < shared_ptr< SampleResource > > matchedChildren = getChildResourcesFromType(queryValue);

                if (matchedChildren.size() > 0)
                {
                    vector< OCRepresentation > requiredChild;
                    OCRepresentation requiredResponse;
                    for (shared_ptr< SampleResource > child : matchedChildren)
                    {
                        addIntoLinksArray(requiredChild, child);
                        cout << "The resource type  used in query is " << queryValue << endl;
                    }
                    requiredResponse.setValue(LINKS_KEY, requiredChild);
                    response->setResourceRepresentation(requiredResponse, responseInterface);


                }
                else
                {
                    shouldReturnError = true;
                }
            }

        }
    }
    else if (m_resourceTypeNames.size() > 1)
    {
        cout << "No query found. As collection has mnulti value rt, responding as baseline:" << endl;
        setBaselineResponse(response);
    }
    else
    {
        cout << "No query found!!" << endl;

        response->setResourceRepresentation(rep, responseInterface);
    }

    if (shouldReturnError)
    {
        cout << "sending forbidden GET response" << endl;
        response->setResourceRepresentation(OCRepresentation(), responseInterface);
        response->setResponseResult(OCEntityHandlerResult::OC_EH_FORBIDDEN);
    }
    else
    {
        cout << "sending normal GET response" << endl;
        response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
    }

    try
    {
        result = OCPlatform::sendResponse(response);
    }
    catch (exception& e)
    {
        cout << "Exception occurred while sending response. Exception is: " << e.what()
                << endl;
    }
    if (result != OC_STACK_OK)
    {
        cerr << "Unable to send response for GET Request" << endl;
    }

}

void SampleCollection::setBaselineResponse(shared_ptr<OCResourceResponse> response)
{
    vector< OCRepresentation > allLinks;
    for (unsigned int i = 0; i <  m_childResourceList.size(); i++)
    {
        addIntoLinksArray(allLinks, m_childResourceList[i]);
    }
    OCRepresentation completeRep = m_representation;
    completeRep.setValue(LINKS_KEY, allLinks);
    completeRep.setValue(NAME_KEY, m_collectionName);
    completeRep.setResourceInterfaces(m_resourceInterfaces);
    completeRep.setResourceTypes(m_resourceTypeNames);

    vector<string> allResourceTypes = m_childResourceList[0]->getResourceTypes();
    for (unsigned int i = 1; i <  m_childResourceList.size(); i++)
    {
        vector<string> childResourceTypes = m_childResourceList[i]->getResourceTypes();
        allResourceTypes.insert(allResourceTypes.end(), childResourceTypes.begin(), childResourceTypes.end());
    }
    completeRep.setValue("rts", allResourceTypes);
    completeRep.setValue("rts-m", allResourceTypes);

    response->setResourceRepresentation(completeRep, DEFAULT_INTERFACE);
}

void SampleCollection::handlePutRequest(QueryParamsMap &queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{

}

void SampleCollection::onResourceServerStarted(bool &isRegisteredForPresence, int &presenceInterval)
{
    cout << "Inside onResourceServerStarted... " << endl;
    isRegisteredForPresence = false;
    presenceInterval = PRESENCE_INTERVAL;

}

void SampleCollection::handleInitRequest(QueryParamsMap &queryParamsMap,
        std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    cout << "Inside handleInitRequest... " << endl;

// handle initialization

    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        cerr << "Unable to send response for Init Request" << endl;
    }
}

OCRepresentation SampleCollection::getResourceRepresentation(OCRepresentation &resourceRep)
{
    return resourceRep;
}

void SampleCollection::notifyObservers(void *param)
{
    SampleCollection *resource = (SampleCollection *) param;

    cout << "Inside notifyObservers....." << endl;
    try
    {
        if ((m_isObserveRegistered == true) && (m_listOfObservers.size() > 0))
        {
            cout << "Sending Notification to Observers...." << endl;
            std::shared_ptr< OCResourceResponse > resourceResponse =
            { std::make_shared< OCResourceResponse >() };

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

void SampleCollection::setDI(string di)
{
    m_di = di;
}

void SampleCollection::setName(string name)
{
    m_collectionName = name;
}

void SampleCollection::setSecured(bool isSecured)
{
    m_isSecured = isSecured;
}

void SampleCollection::setIPVer(OCConnectivityType ipVer)
{
    m_ipVer = ipVer;
}

void SampleCollection::addChild(shared_ptr< SampleResource > childResource)
{
    if(!childResource)
    {
        cout << "No resource given to join to group!" << endl;
        return;
    }

    try
    {
        OCStackResult expectedResult = OCPlatform::bindResource(m_resourceHandle, childResource->getResourceHandle());
        if (expectedResult == OC_STACK_OK)
        {
            cout << "Joining to the group completed!!" << endl;
            m_childResourceList.push_back(childResource);
        }
        else
        {
            cout << "Unable to join to group" << endl;
        }
    }
    catch (exception& e)
    {
        cout << "Exception occurred while trying to join to grop, exception is: " << e.what()
                << endl;
    }

}

bool SampleCollection::updateRepresentation(string key, OCRepresentation incomingRep,
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
    if (key.compare(NAME_KEY) == 0)
    {
        string newDeviceName = incomingRep.getValueToString(key);
        setDeviceInfo(newDeviceName);
    }
    if (rep.getAttributeValue(configurationKey, configurationValue))
    {
        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
        setResourceRepresentation(rep);
        response->setResponseResult(OCEntityHandlerResult::OC_EH_OK);
        result = true;

    }
    else
    {
        result = false;
        response->setResponseResult(OCEntityHandlerResult::OC_EH_FORBIDDEN);
    }

    return result;
}

bool SampleCollection::updateRepresentation(string key, OCRepresentation incomingRep)
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
        setResourceRepresentation(rep);
        result = true;

    }
    else
    {
        result = false;
    }

    return result;
}


OCStackResult SampleCollection::addArrayAttribute(string key, OCRepresentation arrayRep)
{
    OCStackResult result = OC_STACK_OK;

    arrayRep.erase(URI_KEY);
    m_representation.setValue(key, arrayRep);

    return result;
}

void SampleCollection::setAsReadOnly(string key)
{
    m_accessmodifier[key] = "R";
}

bool SampleCollection::isReadonly(string key)
{
    bool readOnly = false;
    if (m_accessmodifier[key] == "R")
    {
        readOnly = true;
    }
    return readOnly;
}

void SampleCollection::addIntoLinksArray(vector< OCRepresentation >& childrenList, shared_ptr< SampleResource > resource)
{
    OCRepresentation interimRep;
    OCRepresentation pRep;
    OCRepresentation epRep;
    vector< OCRepresentation > epLiist;
    //string anchor = ANCHOR_DEFAULT_VALUE + m_di;
    string ep = EP_DEFAULT_VALUE;

    uint8_t bm = 2;
    pRep.setValue(BITMASK_KEY, bm);
    CATransportFlags_t flag = CA_DEFAULT_FLAGS;

    if (m_isSecured)
    {
        flag = (CATransportFlags_t)(flag | CA_SECURE);
    }

    if (m_ipVer == CT_IP_USE_V4)
    {
        flag = (CATransportFlags_t)(flag | CA_IPV4);
    }
    else
    {
        flag = (CATransportFlags_t)(flag | CA_IPV6);
    }
    ep = ep + to_string(CAGetAssignedPortNumber(CA_ADAPTER_IP, flag));
//    pRep.setValue(PORT_KEY, CAGetAssignedPortNumber(CA_ADAPTER_IP, flag));

    epRep.setValue(PRI_KEY, PRI_DEFAULT_VALUE);
    epRep.setValue(EP_KEY, ep);
    epLiist.push_back(epRep);

    vector< OCRepresentation > allChildren;
    interimRep.setValue(URI_KEY, resource->getUri());
    //interimRep.setValue(ANCHOR_KEY, anchor);
    interimRep.setValue(RESOURCE_TYPE_KEY, resource->getResourceTypes());
    interimRep.setValue(INTERFACE_KEY, resource->getResourceInterfaces());
    interimRep.setValue(POLICY_KEY, pRep);
    interimRep.setValue(EPS_KEY, epLiist);
    childrenList.push_back(interimRep);
}

vector< shared_ptr< SampleResource > > SampleCollection::getChildResourcesFromType(string resourceType)
{
    vector< shared_ptr< SampleResource > > matchedChildren;
    for (shared_ptr< SampleResource > child : m_childResourceList )
    {
        for (string currentResourceType : child->getResourceTypes())
        {
            if (currentResourceType.compare(resourceType) == 0)
            {
                matchedChildren.push_back(child);
                break;
            }

        }

    }
    return matchedChildren;
}

vector< shared_ptr< SampleResource > > SampleCollection::getChildResourcesFromRepKey(string key)
{
    vector< shared_ptr< SampleResource > > matchedChildren;
    for (shared_ptr< SampleResource > child : m_childResourceList )
    {
        OCRepresentation rep = child->getRepresentation();
        if (rep.hasAttribute(key))
        {
            matchedChildren.push_back(child);
        }

    }
    return matchedChildren;
}
