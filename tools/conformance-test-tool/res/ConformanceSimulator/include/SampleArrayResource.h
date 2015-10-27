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

/*
 * SampleArrayResource.h
 *
 *  Created on: Oct 19, 2015
 *      Author: i.mushfiq
 */

#ifndef SAMPLE_ARRAY_RESOURCE_H_
#define SAMPLE_ARRAY_RESOURCE_H_

#include "ResourceServer.h"
#include "ConformanceHelper.h"
#include "boost/any.hpp"

using namespace std;
namespace PH = std::placeholders;

class SampleArrayResource: public ResourceServer
{
private:
    int m_recursiveDelay;
    int m_scheduledDelay;bool m_isCancelCalled;bool m_isObserveRegistered;
    ObservationIds m_listOfObservers;
    shared_ptr< OCResourceResponse > m_pResponse;
    vector< string > m_resourceList;
    ConformanceHelper *p_conformanceHelper;
//    template <typename T>
//    map<string, T> warrantyMap;

public:
    SampleArrayResource(void);

    ~SampleArrayResource(void);

    virtual void onResourceServerStarted(bool &isRegisteredForPresence, int &presenceInterval);

    virtual void handleObserveRequest(QueryParamsMap &queryParamsMap,
            std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handleDeleteRequest(QueryParamsMap &queryParamsMap,
            OCRepresentation incomingRepresentation,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handlePostRequest(QueryParamsMap &queryParamsMap,
            OCRepresentation incomingRepresentation,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handleGetRequest(QueryParamsMap &queryParamsMap,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handlePutRequest(QueryParamsMap &queryParamsMap,
            OCRepresentation incomingRepresentation,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handleInitRequest(QueryParamsMap &queryParamsMap,
            std::shared_ptr< OCResourceResponse > response);

//    virtual AttributeMap getAttributeMap(void);
    virtual OCRepresentation getResourceRepresentation(OCRepresentation &resourceRep);

private:
    void handleRecursiveActionSet();

    void handleScheduledActionSet();

    void notifyObservers(void *param);

    bool updateRepresentation(string key, OCRepresentation incomingRep,
            shared_ptr< OCResourceResponse > response);
    void createResource(string initialUri, OCRepresentation incomingRepresentation,
            shared_ptr< OCResourceResponse > response);
};

#endif /* COMMON_TESTCASE_SAMPLERESOURCE_H_ */
