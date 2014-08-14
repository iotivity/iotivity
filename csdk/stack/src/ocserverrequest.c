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

#include <string.h>
#include "ocstack.h"
#include "ocresource.h"
#include "ocstackinternal.h"
#include "ocserverrequest.h"
#include "debug.h"
#include "cJSON.h"

// Module Name
#define MOD_NAME PCF("ocserverrequest")

#define TAG  PCF("OCStackServerRequest")

#define VERIFY_NON_NULL(arg, logLevel, retVal) { if (!(arg)) { OC_LOG((logLevel), \
             TAG, PCF(#arg " is NULL")); return (retVal); } }

extern OCResource *headResource;

OCStackResult ProcessResourceDiscoverReq (OCEntityHandlerRequest *request,
                                 uint8_t filterOn, char *filterValue)
{
    OCResource *resourcePtr = headResource;
    OCResourceType *resourceTypePtr;
    OCResourceInterface *interfacePtr;
    cJSON *ocObj, *pLoadObj, *resArray, *resObj, *rtArray;
    char *jsonStr;
    uint8_t encodeRes = 0;
    OCStackResult ret = OC_STACK_OK;

    OC_LOG_V(INFO, TAG, PCF("Entering ProcessResourceDiscoverReq"));
    ocObj = cJSON_CreateObject();
    cJSON_AddItemToObject (ocObj, OC_RSRVD_OC, pLoadObj = cJSON_CreateObject());
    cJSON_AddItemToObject (pLoadObj, OC_RSRVD_PAYLOAD, resArray = cJSON_CreateArray());

    while (resourcePtr)
    {
        encodeRes = 0;
        if (filterOn == STACK_RES_DISCOVERY_RT_FILTER) {
            resourceTypePtr = resourcePtr->rsrcType;
            while (resourceTypePtr) {
                if (strcmp (resourceTypePtr->resourcetypename, filterValue) == 0) {
                    encodeRes = 1;
                    break;
                }
                resourceTypePtr = resourceTypePtr->next;
            }
        } else if (filterOn == STACK_RES_DISCOVERY_IF_FILTER) {
            interfacePtr = resourcePtr->rsrcInterface;
            while (interfacePtr) {
                if (strcmp (interfacePtr->name, filterValue) == 0) {
                    encodeRes = 1;
                    break;
                }
                interfacePtr = interfacePtr->next;
            }
        } else if (filterOn == STACK_RES_DISCOVERY_NOFILTER) {
            encodeRes = 1;
        } else {
            //TODO: Unsupported query filter
            return OC_STACK_INVALID_QUERY;
        }

        if (encodeRes) {
            cJSON_AddItemToArray (resArray, resObj = cJSON_CreateObject());
            // Add URIs
            cJSON_AddItemToObject (resObj, OC_RSRVD_HREF, cJSON_CreateString(resourcePtr->uri));
            // Add resource types
            cJSON_AddItemToObject (resObj, OC_RSRVD_RESOURCE_TYPE, rtArray = cJSON_CreateArray());
            resourceTypePtr = resourcePtr->rsrcType;
            while (resourceTypePtr) {
                cJSON_AddItemToArray (rtArray,
                                      cJSON_CreateString(resourceTypePtr->resourcetypename));
                resourceTypePtr = resourceTypePtr->next;
            }
            // Add interface types
            cJSON_AddItemToObject (resObj, OC_RSRVD_INTERFACE, rtArray = cJSON_CreateArray());
            interfacePtr = resourcePtr->rsrcInterface;
            while (interfacePtr) {
                cJSON_AddItemToArray (rtArray, cJSON_CreateString(interfacePtr->name));
                interfacePtr = interfacePtr->next;
            }
            // If resource is observable, set observability flag.
            // Resources that are not observable will not have the flag.
            if (resourcePtr->resourceProperties & OC_OBSERVABLE) {
                cJSON_AddItemToObject (resObj, OC_RSRVD_OBSERVABLE,
                                       cJSON_CreateNumber(OC_RESOURCE_OBSERVABLE));
            }
        }
        resourcePtr = resourcePtr->next;
    }
    jsonStr = cJSON_PrintUnformatted (ocObj);
    if (strlen(jsonStr) < request->resJSONPayloadLen)
    {
        strncpy((char *)request->resJSONPayload, jsonStr, request->resJSONPayloadLen);
    }
    else
    {
        ret = OC_STACK_ERROR;
    }
    cJSON_Delete (ocObj);
    free (jsonStr);

    OC_LOG_V(INFO, TAG, PCF("Exiting ProcessResourceDiscoverReq"));
    return ret;
}

OCStackResult ValidateUrlQuery (unsigned char *url, unsigned char *query,
                                uint8_t *filterOn, char **filterValue)
{
    char *filterParam;

    OC_LOG_V(INFO, TAG, PCF("Exiting ValidateUrlQuery"));
    if (!url)
        return OC_STACK_INVALID_URI;

    if (strcmp ((char *)url, GetVirtualResourceUri(OC_WELL_KNOWN_URI)) == 0) {
        *filterOn = STACK_RES_DISCOVERY_NOFILTER;
        if (query && *query) {
            filterParam = strtok ((char *)query, "=");
            *filterValue = strtok (NULL, " ");
            if (!(*filterValue)) {
                return OC_STACK_INVALID_QUERY;
            } else if (strcmp (filterParam, OC_RSRVD_INTERFACE) == 0) {
                // Resource discovery with interface filter
                *filterOn = STACK_RES_DISCOVERY_IF_FILTER;
            } else if (strcmp (filterParam, OC_RSRVD_RESOURCE_TYPE) == 0) {
                // Resource discovery with resource type filter
                *filterOn = STACK_RES_DISCOVERY_RT_FILTER;
            } else {
                // Other filter types not supported
                return OC_STACK_INVALID_QUERY;
            }
        }
    } else {
        // Other URIs not yet supported
        return OC_STACK_INVALID_URI;
    }
    OC_LOG_V(INFO, TAG, PCF("Exiting ValidateUrlQuery"));
    return OC_STACK_OK;
}



