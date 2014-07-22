//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "ocstack.h"
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

OCStackResult processResourceDiscoverReq (const char *request, char *response,
                                 uint8_t filterOn, char *filterValue)
{
    OCResource *resourcePtr = headResource;
    OCResourceType *resourceTypePtr;
    OCResourceInterface *interfacePtr;
    cJSON *ocObj, *pLoadObj, *resArray, *resObj, *rtArray;
    char *jsonStr;
    uint8_t encodeRes = 0;

    ocObj = cJSON_CreateObject();
    cJSON_AddItemToObject (ocObj, OC_RSRVD_OC, pLoadObj = cJSON_CreateObject());
    cJSON_AddItemToObject (pLoadObj, OC_RSRVD_PAYLOAD, resArray = cJSON_CreateArray());

    while (resourcePtr != NULL)
    {
        encodeRes = 0;
        if (filterOn == STACK_RES_DISCOVERY_RT_FILTER) {
            resourceTypePtr = resourcePtr->rsrcType;
            while (resourceTypePtr != NULL) {
                if (strcmp (resourceTypePtr->resourcetypename, filterValue) == 0) {
                    encodeRes = 1;
                    break;
                }
                resourceTypePtr = resourceTypePtr->next;
            }
        } else if (filterOn == STACK_RES_DISCOVERY_IF_FILTER) {
            interfacePtr = resourcePtr->rsrcInterface;
            while (interfacePtr != NULL) {
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
            while (resourceTypePtr != NULL) {
                cJSON_AddItemToArray (rtArray,
                                      cJSON_CreateString(resourceTypePtr->resourcetypename));
                resourceTypePtr = resourceTypePtr->next;
            }
            // Add interface types
            cJSON_AddItemToObject (resObj, OC_RSRVD_INTERFACE, rtArray = cJSON_CreateArray());
            interfacePtr = resourcePtr->rsrcInterface;
            while (interfacePtr != NULL) {
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
    memcpy (response, jsonStr, strlen(jsonStr));
    cJSON_Delete (ocObj);
    free (jsonStr);

    return OC_STACK_OK;
}

OCStackResult validateUrlQuery (char *url, char *query, uint8_t *filterOn, char **filterValue)
{
    char *filterParam;

    if (NULL == url)
        return OC_STACK_INVALID_URI;

    if (strcmp (url, OC_RESOURCE_DISCOVERY_URI) == 0) {
        *filterOn = STACK_RES_DISCOVERY_NOFILTER;
        if (*query != NULL) {
            filterParam = strtok (query, "=");
            *filterValue = strtok (NULL, " ");
            if (*filterValue == NULL) {
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
    return OC_STACK_OK;
}



