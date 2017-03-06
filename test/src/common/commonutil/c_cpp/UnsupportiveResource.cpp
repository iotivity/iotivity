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
 * UnsupportiveResource.cpp
 *
 *  Created on: Nov 25, 2014
 *
 */

#include "UnsupportiveResource.h"

#define PRESENCE_INTERVAL 10000

UnsupportiveResource::UnsupportiveResource(void)
{
    //child constructor

    string value = DEFAULT_DUMMY_VALUE;
    m_representation.setValue("dummy", value);

    printf("Current resource info: %s", m_representation.getPayload()->values->str);
}

UnsupportiveResource::~UnsupportiveResource(void)
{
    // child destructor
}

void UnsupportiveResource::handleObserveRequest(QueryParamsMap& queryParamsMap,
std::shared_ptr< OCResourceRequest > request,
std::shared_ptr< OCResourceResponse > response)
{
    printf("Inside handleObserveRequest... ");
    // handle observe request
    response->setErrorCode(200);
    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        printf("Unable to send response for Observe Request");
    }
}

void UnsupportiveResource::handleDeleteRequest(QueryParamsMap &queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    printf("Inside handleDeleteRequest... ");
    // handle delete request
    response->setErrorCode(200);
    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        printf("Unable to send response for Delete Request");
    }
}

void UnsupportiveResource::handlePostRequest(QueryParamsMap &queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    printf("Inside handlePostRequest... ");
    // handle POST request
    response->setErrorCode(200);
    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        printf("Unable to send response for POST Request");
    }
}

void UnsupportiveResource::handleGetRequest(QueryParamsMap &queryParamsMap,
std::shared_ptr< OCResourceRequest > request,
std::shared_ptr< OCResourceResponse > response)
{
    printf("Inside handleGetRequest... ");
    //handle GET request
    response->setErrorCode(200);
    OCRepresentation rep = getRepresentation();
    printf("Found representation is: %s", rep.getPayload()->values->str);

    response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        printf("Unable to send response for GET Request");
    }
}

void UnsupportiveResource::handlePutRequest(QueryParamsMap &queryParamsMap,
        OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
        std::shared_ptr< OCResourceResponse > response)
{
    OCRepresentation rep = getRepresentation();

    printf("Inside handlePutRequest... ");
    printf("Incoming JSON Representation is: %s", incomingRepresentation.getPayload()->values->str);
    // handle PUT request
    response->setErrorCode(200);
    response->setResourceRepresentation(rep, DEFAULT_INTERFACE);

    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        printf("Unable to send response for PUT Request");
    }
}

void UnsupportiveResource::onResourceServerStarted(bool& isRegisteredForPresence,
        int& presenceInterval)
{
    printf("Inside onResourceServerStarted... ");
    isRegisteredForPresence = true;
    presenceInterval = PRESENCE_INTERVAL;

    // perform additional task
}

void UnsupportiveResource::handleInitRequest(QueryParamsMap &queryParamsMap,
std::shared_ptr< OCResourceRequest > request,
std::shared_ptr< OCResourceResponse > response)
{
    printf("Inside handleInitRequest... ");
    // handle initialization
    response->setErrorCode(200);

    OCStackResult result = OCPlatform::sendResponse(response);
    if (result != OC_STACK_OK)
    {
        printf("Unable to send response for Init Request");
    }
}

OCRepresentation UnsupportiveResource::getResourceRepresentation(OCRepresentation& resourceRep)
{
    return resourceRep;
}

