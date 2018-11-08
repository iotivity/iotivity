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

#ifndef COMMON_TESTCASE_SAMPLECOLLECTION_H_
#define COMMON_TESTCASE_SAMPLECOLLECTION_H_

#include "cautilinterface.h"
#include "cacommon.h"

#include "ResourceHelper.h"
#include "ResourceServer.h"
#include "SampleResource.h"

using namespace std;
namespace PH = std::placeholders;

class SampleCollection: public ResourceServer
{
private:
    bool m_isCancelCalled;
    bool m_isObserveRegistered;
    bool m_isSecured;

    string m_di;
    OCConnectivityType m_ipVer;
    string m_collectionName;

    ObservationIds m_listOfObservers;
    shared_ptr< OCResourceResponse > m_pResponse;
    vector< shared_ptr< SampleResource >  > m_childResourceList;
    map< string, string > m_accessmodifier;

public:
    SampleCollection(void);

    ~SampleCollection(void);

    void setAsReadOnly(string key);

    bool isReadonly(string key);

    virtual void onResourceServerStarted(bool &isRegisteredForPresence, int &presenceInterval);

    virtual void handleObserveRequest(QueryParamsMap &queryParamsMap,
            std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handleDeleteRequest(QueryParamsMap &queryParamsMap,
            OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handlePostRequest(QueryParamsMap &queryParamsMap,
            OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handleGetRequest(QueryParamsMap &queryParamsMap,
            std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handlePutRequest(QueryParamsMap &queryParamsMap,
            OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handleInitRequest(QueryParamsMap &queryParamsMap,
            std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response);

    virtual OCRepresentation getResourceRepresentation(OCRepresentation &resourceRep);

    void notifyObservers(void *param);
    bool updateRepresentation(string key, OCRepresentation incomingRep);

    void setDI(string di);
    void setName(string name);
    void setIPVer(OCConnectivityType ipVer);
    void setSecured(bool isSecured);

    void addChild(shared_ptr< SampleResource > childResource);


private:
    void setBaselineResponse(shared_ptr<OCResourceResponse> response);

    void addIntoLinksArray(vector< OCRepresentation >& childrenList, shared_ptr< SampleResource > resource);

    OCStackResult addArrayAttribute(string, OCRepresentation);

    bool updateRepresentation(string key, OCRepresentation incomingRep, shared_ptr< OCResourceResponse > response);

    vector< shared_ptr< SampleResource > > getChildResourcesFromType(string resourceType);
    vector< shared_ptr< SampleResource > > getChildResourcesFromRepKey(string key);

};

#endif /* COMMON_TESTCASE_SampleCollection_H_ */
