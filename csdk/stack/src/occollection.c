//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

#define _POSIX_C_SOURCE 200112L
#include <string.h>
#include "ocstack.h"
#include "ocstackinternal.h"
#include "ocresource.h"
#include "logger.h"
#include "debug.h"
#include "cJSON.h"
/// Module Name
#define TAG PCF("occollection")

#define NUM_PARAM_IN_QUERY  2

static OCStackResult
ValidateQuery (const unsigned char *query, OCResourceHandle *resource,
                             OCStackIfTypes *ifParam, char **rtParam)
{
    uint8_t numFields = 0, numParam;

    //TODO: Query and URL validation is being done for virtual resource case
    // using ValidateUrlQuery function. We should be able to merge it with this
    // function.
    OC_LOG_V(INFO, TAG, PCF("Entering ValidateQuery"));

    if (!query)
        return OC_STACK_ERROR;

    if (!(*query))
    {
        // Query string is empty
        OC_LOG_V(INFO, TAG, PCF("Empty query string, use default IF and RT"));
        *ifParam = STACK_IF_DEFAULT;
        *rtParam = (char *) OCGetResourceTypeName (resource, 0);
        return OC_STACK_OK;
    }

    // Break the query string to validate it and determine IF and RT parameters
    // Validate there are atmost 2 parameters in string and that one is 'if' and other 'rt'
    char *endStr, *ifPtr = NULL, *rtPtr = NULL;
    char *token = strtok_r ((char *)query, "&", &endStr);

    // External loop breaks query string into fields using the & separator
    while (token != NULL)
    {
        numFields++;
        char *endToken;
        char *innerToken = strtok_r (token, "=", &endToken);
        numParam = 0;
        // Internal loop parses the field to extract values (parameters) assigned to each field
        while (innerToken != NULL)
        {

            numParam++;
            if (strcmp (innerToken, OC_RSRVD_INTERFACE) == 0)
            {
                // Determine the value of IF parameter
                innerToken = strtok_r (NULL, "=", &endToken);
                ifPtr = innerToken;
            } else if (strcmp (innerToken, OC_RSRVD_RESOURCE_TYPE) == 0) {
                // Determine the value of RT parameter
                innerToken = strtok_r (NULL, "=", &endToken);
                rtPtr = innerToken;
            } else {
                innerToken = strtok_r (NULL, "=", &endToken);
            }
        }
        if (numParam != 2)
        {
            // Query parameter should be of the form if=<string>. String should not have & or =
            return OC_STACK_INVALID_QUERY;
        }
        token = strtok_r (NULL, "&", &endStr);
    }
    if (numFields > NUM_PARAM_IN_QUERY)
    {
        // M1 release supports one IF value, one RT value and no other params
        return OC_STACK_INVALID_QUERY;
    }

    if (!ifPtr)
    {
        // IF not specified in query, use default IF
        *ifParam = STACK_IF_DEFAULT;
    }
    else
    {
        if (strcmp (ifPtr, OC_RSRVD_DEFAULT) == 0)
            *ifParam = STACK_IF_DEFAULT;
        else if (strcmp (ifPtr, OC_RSRVD_INTERFACE_LL) == 0)
            *ifParam = STACK_IF_LL;
        else if (strcmp (ifPtr, OC_RSRVD_INTERFACE_BATCH) == 0)
            *ifParam = STACK_IF_BATCH;
        else
        {
            return OC_STACK_ERROR;
        }
    }

    if (!rtPtr)
    {
        // RT not specified in query. Use the first resource type for the resource as default.
        *rtParam = (char *) OCGetResourceTypeName (resource, 0);
    }
    else
    {
        *rtParam = rtPtr;
    }
    OC_LOG_V(INFO, TAG, "Query params: IF = %d, RT = %s\n", *ifParam, *rtParam);

    // TODO: Validate that the resource supports specified IF param
    // TODO: Validate that the resource supports specified RT param
    return OC_STACK_OK;
}
static OCStackResult BuildRootResourceJSON(OCResource *resource, OCEntityHandlerRequest *ehRequest)
{
    OCStackResult ret = OC_STACK_ERROR;
    cJSON *resObj;
    char *jsonStr;
    uint16_t jsonLen;

    OC_LOG_V(INFO, TAG, PCF("Entering BuildRootResourceJSON\n"));
    resObj = cJSON_CreateObject();
    if (resource)
    {
        cJSON_AddItemToObject (resObj, OC_RSRVD_HREF, cJSON_CreateString(resource->uri));
    }
    jsonStr = cJSON_PrintUnformatted (resObj);
    jsonLen = strlen(jsonStr);
    if (jsonLen < ehRequest->resJSONPayloadLen)
    {
        strcpy((char*)ehRequest->resJSONPayload, jsonStr);
        ehRequest->resJSONPayloadLen -= jsonLen;
        ehRequest->resJSONPayload += jsonLen;
        ret = OC_STACK_OK;
    }

    if (ehRequest->resJSONPayloadLen >= (sizeof(OC_JSON_SEPARATOR) + 1))
    {
        *ehRequest->resJSONPayload = OC_JSON_SEPARATOR;
        ehRequest->resJSONPayload++;
        ehRequest->resJSONPayloadLen--;
        ret = OC_STACK_OK;
    }

    cJSON_Delete (resObj);
    free (jsonStr);

    return ret;
}


static OCStackResult
BuildCollectionJSONResponse(OCResource *resource, OCEntityHandlerRequest *ehRequest,
                       uint8_t filterOn, char *filterValue)
{
    OCStackResult ret = OC_STACK_ERROR;
    ret = BuildRootResourceJSON(resource, ehRequest);

    unsigned char* buffer = ehRequest->resJSONPayload;
    uint16_t remaining = ehRequest->resJSONPayloadLen;
    if (ret == OC_STACK_OK)
    {
        for  (int i = 0; i < MAX_CONTAINED_RESOURCES; i++)
        {
            OCResource* temp = resource->rsrcResources[i];
            if (temp)
            {
                //TODO ("Proper Error handling");
                ret = BuildDiscoveryResponse(temp, filterOn, filterValue, (char*)buffer, &remaining);
                if (ret != OC_STACK_OK)
                {
                    break;
                }

                buffer += strlen((char*)buffer);
                if (resource->rsrcResources[i+1] && remaining > sizeof(OC_JSON_SEPARATOR))
                {
                    *buffer = OC_JSON_SEPARATOR;
                    buffer++;
                    remaining--;
                }
            }
            else
            {
                break;
            }
        }
        ehRequest->resJSONPayload = buffer;
        ehRequest->resJSONPayloadLen = remaining;
    }

    return ret;
}


static OCStackResult
BuildCollectionBatchJSONResponse(OCEntityHandlerFlag flag,
                OCResource *resource, OCEntityHandlerRequest *ehRequest)
{
    OCStackResult stackRet = OC_STACK_ERROR;
    OCEntityHandlerResult ehRet = OC_EH_ERROR;

    stackRet = BuildRootResourceJSON(resource, ehRequest);
    if (stackRet == OC_STACK_OK)
    {
        OCResourceHandle origResourceHandle = ehRequest->resource;

        for  (int i = 0; i < MAX_CONTAINED_RESOURCES; i++)
        {
            OCResource* temp = resource->rsrcResources[i];
            if (temp)
            {
                //TODO ("Proper Error handling");

                ehRequest->resource = (OCResourceHandle) temp;

                ehRet = temp->entityHandler(OC_REQUEST_FLAG, ehRequest);

                if(ehRet == OC_EH_OK)
                {
                    unsigned char* buffer = ehRequest->resJSONPayload;
                    ehRequest->resJSONPayloadLen = ehRequest->resJSONPayloadLen - strlen((char*)buffer);

                    buffer += strlen((char*)buffer);
                    ehRequest->resJSONPayload = buffer;
                    if ( resource->rsrcResources[i+1] && ehRequest->resJSONPayloadLen > sizeof(OC_JSON_SEPARATOR) )
                    {
                        * buffer = OC_JSON_SEPARATOR;
                        buffer++;
                        ehRequest->resJSONPayload = buffer;
                        ehRequest->resJSONPayloadLen = ehRequest->resJSONPayloadLen - 1;
                    }
                }
                else
                {
                    stackRet = OC_STACK_ERROR;
                    break;
                }
            }
            else
            {
               break;
            }
        }

        ehRequest->resource = origResourceHandle;
    }
    return stackRet;
}


OCStackResult DefaultCollectionEntityHandler (OCEntityHandlerFlag flag,
                                              OCEntityHandlerRequest *ehRequest)
{
    OCStackResult result;
    OCStackIfTypes ifQueryParam;
    char *rtQueryParam;

    OC_LOG(INFO, TAG, "DefaultCollectionEntityHandler\n");

    if (flag != OC_REQUEST_FLAG)
        return OC_STACK_ERROR;

    result = ValidateQuery ((const unsigned char *)ehRequest->query,
                            &ehRequest->resource, &ifQueryParam, &rtQueryParam);

    if (result != OC_STACK_OK)
        return result;

    if ((ehRequest->method != OC_REST_GET) &&
        (ehRequest->method != OC_REST_PUT))
        return OC_STACK_ERROR;

    if (ehRequest->method == OC_REST_GET)
    {
        switch (ifQueryParam)
        {
            case STACK_IF_DEFAULT:
                // Get attributes of collection resource and properties of contined resource
                // M1 release does not support attributes for collection resource, so the GET
                // operation is same as the GET on LL interface.

                OC_LOG(INFO, TAG, "STACK_IF_DEFAULT\n");
                return BuildCollectionJSONResponse( (OCResource *)ehRequest->resource,
                             ehRequest, STACK_RES_DISCOVERY_NOFILTER, NULL);

            case STACK_IF_LL:
                OC_LOG(INFO, TAG, "STACK_IF_LL\n");
                return BuildCollectionJSONResponse( (OCResource *)ehRequest->resource,
                             ehRequest, STACK_RES_DISCOVERY_NOFILTER, NULL);

            case STACK_IF_BATCH:
                OC_LOG(INFO, TAG, "STACK_IF_BATCH\n");
                return BuildCollectionBatchJSONResponse(flag, (OCResource *)ehRequest->resource, ehRequest);

            default:
                return OC_STACK_ERROR;
        }
    } else if (ehRequest->method == OC_REST_PUT) {
        switch (ifQueryParam)
        {
            case STACK_IF_DEFAULT:
                // M1 release does not support PUT on default interface
                return OC_STACK_ERROR;

            case STACK_IF_LL:
                // LL interface only supports GET
                return OC_STACK_ERROR;

            case STACK_IF_BATCH:
                return BuildCollectionBatchJSONResponse(flag, (OCResource *)ehRequest->resource, ehRequest);

            default:
                return OC_STACK_ERROR;
        }
    }
    return result;
}

