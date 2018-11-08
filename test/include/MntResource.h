/******************************************************************
*
* Copyright 2018 Open Connectivity Foundation All Rights Reserved.
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

#ifndef COMMON_TESTCASE_MNTRESOURCE_H_
#define COMMON_TESTCASE_MNTRESOURCE_H_

#include "SampleResource.h"

using namespace std;
namespace PH = std::placeholders;

class MntResource : public SampleResource
{
protected:
    int numberOf503ErrorResponse;
    bool expect503ErrorRespond;

public:
    MntResource(void);

    MntResource(std::string resourceUri,
            std::string resourceTypeName,
            std::string resourceInterface,
            uint8_t resourceProperty = OC_ACTIVE);

    MntResource(std::string resourceUri,
            std::string resourceTypeName,
            std::string resourceInterface,
            uint8_t resourceProperty,
            OCRepresentation resourceRepresentation);

    virtual ~MntResource(void);

    virtual void handleGetRequest(QueryParamsMap &queryParamsMap,
            std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response);

    virtual void handlePostRequest(QueryParamsMap &queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response);
};

#endif /* COMMON_TESTCASE_MNTRESOURCE_H_ */