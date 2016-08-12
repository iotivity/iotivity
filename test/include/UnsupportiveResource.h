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

#ifndef _COMMON_TESTCASE_UNSUPPORTIVERESOURCE_H_
#define _COMMON_TESTCASE_UNSUPPORTIVERESOURCE_H_

#include "ResourceServer.h"

#define DEFAULT_DUMMY_VALUE "0"

class UnsupportiveResource: public ResourceServer
{
public:
    UnsupportiveResource(void);

    ~UnsupportiveResource(void);

    virtual void onResourceServerStarted(bool& isRegisteredForPresence, int& presenceInterval);

    virtual void handleObserveRequest(QueryParamsMap& queryParamsMap,
            std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handleDeleteRequest(QueryParamsMap& queryParamsMap,
            OCRepresentation incomingRepresentation,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handlePostRequest(QueryParamsMap& queryParamsMap,
            OCRepresentation incomingRepresentation,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handleGetRequest(QueryParamsMap& queryParamsMap,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handlePutRequest(QueryParamsMap& queryParamsMap,
            OCRepresentation incomingRepresentation,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handleInitRequest(QueryParamsMap& queryParamsMap,
            std::shared_ptr< OCResourceResponse > response);

//    virtual AttributeMap getAttributeMap(void);
    virtual OCRepresentation getResourceRepresentation(OCRepresentation& resourceRep);

};

#endif /* COMMON_TESTCASE_UNSUPPORTIVERESOURCE_H_ */
