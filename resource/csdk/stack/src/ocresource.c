//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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
#include "ocstackconfig.h"
#include "ocstackinternal.h"
#include "ocresource.h"
#include "ocobserve.h"
#include "occollection.h"
#include "occoap.h"
#include "logger.h"
#include "debug.h"
#include "cJSON.h"

/// Module Name
#define TAG PCF("ocresource")
#define VERIFY_NON_NULL(arg, logLevel, retVal) { if (!(arg)) { OC_LOG((logLevel), \
             TAG, PCF(#arg " is NULL")); return (retVal); } }

extern OCResource *headResource;

static const char * VIRTUAL_RSRCS[] = {
       "/oc/core",
       "/oc/core/d",
       "/oc/core/types/d",
       #ifdef WITH_PRESENCE
       "/oc/presence"
       #endif
};

//-----------------------------------------------------------------------------
// Default resource entity handler function
//-----------------------------------------------------------------------------
OCEntityHandlerResult defaultResourceEHandler(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest * request) {
    TODO ("Implement me!!!!");
    // TODO:  remove silence unused param warnings
    (void) flag;
    (void) request;
    return  OC_EH_OK; // Making sure that the Default EH and the Vendor EH have matching signatures
}

static OCStackResult ValidateUrlQuery (unsigned char *url, unsigned char *query,
                                uint8_t *filterOn, char **filterValue)
{
    char *filterParam;

    OC_LOG(INFO, TAG, PCF("Entering ValidateUrlQuery"));
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
    }
    #ifdef WITH_PRESENCE
    else if (strcmp((char *)url, GetVirtualResourceUri(OC_PRESENCE)) == 0) {
        //Nothing needs to be done, except for pass a OC_PRESENCE query through as OC_STACK_OK.
        OC_LOG(INFO, TAG, PCF("OC_PRESENCE Request"));
        *filterOn = STACK_RES_DISCOVERY_NOFILTER;
    }
    #endif
    else {
        // Other URIs not yet supported
        return OC_STACK_INVALID_URI;
    }
    OC_LOG(INFO, TAG, PCF("Exiting ValidateUrlQuery"));
    return OC_STACK_OK;
}

OCStackResult BuildVirtualResourceResponse(OCResource *resourcePtr, uint8_t filterOn,
                                            char *filterValue, char * out, uint16_t *remaining)
{
    OCResourceType *resourceTypePtr;
    OCResourceInterface *interfacePtr;
    cJSON *resObj, *propObj, *rtArray;
    char *jsonStr;
    uint8_t encodeRes = 0;
    OCStackResult ret = OC_STACK_OK;
    uint16_t jsonLen;

    OC_LOG(INFO, TAG, PCF("Entering BuildVirtualResourceResponse"));
    resObj = cJSON_CreateObject();

    if (resourcePtr)
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
            // Add URIs
            cJSON_AddItemToObject (resObj, OC_RSRVD_HREF, cJSON_CreateString(resourcePtr->uri));

            cJSON_AddItemToObject (resObj, "prop", propObj = cJSON_CreateObject());
            // Add resource types
            cJSON_AddItemToObject (propObj, OC_RSRVD_RESOURCE_TYPE, rtArray = cJSON_CreateArray());
            resourceTypePtr = resourcePtr->rsrcType;
            while (resourceTypePtr) {
                cJSON_AddItemToArray (rtArray,
                                      cJSON_CreateString(resourceTypePtr->resourcetypename));
                resourceTypePtr = resourceTypePtr->next;
            }
            // Add interface types
            cJSON_AddItemToObject (propObj, OC_RSRVD_INTERFACE, rtArray = cJSON_CreateArray());
            interfacePtr = resourcePtr->rsrcInterface;
            while (interfacePtr) {
                cJSON_AddItemToArray (rtArray, cJSON_CreateString(interfacePtr->name));
                interfacePtr = interfacePtr->next;
            }
            // If resource is observable, set observability flag.
            // Resources that are not observable will not have the flag.
            if (resourcePtr->resourceProperties & OC_OBSERVABLE) {
                cJSON_AddItemToObject (propObj, OC_RSRVD_OBSERVABLE,
                                       cJSON_CreateNumber(OC_RESOURCE_OBSERVABLE));
            }
            // Set secure flag for secure resources
            if (resourcePtr->resourceProperties & OC_SECURE) {
                uint16_t port;
                cJSON_AddNumberToObject (propObj, OC_RSRVD_SECURE, OC_RESOURCE_SECURE);
                //Set the IP port also as secure resources are hosted on a different port
                if (OCGetResourceEndPointInfo (resourcePtr, &port) == OC_STACK_OK) {
                    cJSON_AddNumberToObject (propObj, OC_RSRVD_HOSTING_PORT, port);
                }
            }

        }
    }
    jsonStr = cJSON_PrintUnformatted (resObj);
    jsonLen = strlen(jsonStr);
    if (jsonLen < *remaining)
    {
        strcpy(out, jsonStr);
        *remaining = *remaining - jsonLen;
    }
    else
    {
        ret = OC_STACK_ERROR;
    }
    cJSON_Delete (resObj);
    free (jsonStr);


    OC_LOG(INFO, TAG, PCF("Exiting BuildVirtualResourceResponse"));
    return ret;
}

OCEntityHandlerResult
BuildObsJSONResponse(OCResource *resource, OCEntityHandlerRequest *ehRequest)
{
    OCEntityHandlerResult ret = OC_EH_ERROR;
    unsigned char* saveJSONPayLoadPtr = ehRequest->resJSONPayload;

    if (ehRequest->resJSONPayloadLen > OC_JSON_PREFIX_LEN)
    {
        strcpy((char*)ehRequest->resJSONPayload, OC_JSON_PREFIX);
        ehRequest->resJSONPayloadLen -= OC_JSON_PREFIX_LEN;
        ehRequest->resJSONPayload += OC_JSON_PREFIX_LEN;
    }

    ret = resource->entityHandler(OC_REQUEST_FLAG, ehRequest);

    ehRequest->resJSONPayloadLen = ehRequest->resJSONPayloadLen -
            strlen((char*)ehRequest->resJSONPayload);
    ehRequest->resJSONPayload += strlen((char*)ehRequest->resJSONPayload);

    if (ehRequest->resJSONPayloadLen > OC_JSON_SUFFIX_LEN)
    {
        strcpy((char*)ehRequest->resJSONPayload, OC_JSON_SUFFIX);
    }
    ehRequest->resJSONPayload = saveJSONPayLoadPtr;
    return ret;
}


TODO ("Does it make sense to make this method as inline")
const char * GetVirtualResourceUri( OCVirtualResources resource)
{
    if (resource < OC_MAX_VIRTUAL_RESOURCES)
    {
        return VIRTUAL_RSRCS[resource];
    }

    return NULL;
}

uint8_t IsVirtualResource(const char* resourceUri)
{
    for (int i = 0; i < OC_MAX_VIRTUAL_RESOURCES; i++)
    {
        if (strcmp(resourceUri, GetVirtualResourceUri((OCVirtualResources)i)) == 0)
        {
            return 1;
        }
    }
    return 0;
}

uint8_t IsCollectionResource (OCResource *resource)
{
    for (int i = 0; i < MAX_CONTAINED_RESOURCES; i++)
    {
        if (resource->rsrcResources[i])
        {
            return 1;
        }
    }
    return 0;
}

OCResource *FindResourceByUri(const char* resourceUri)
{
    OCResource * pointer = headResource;
    while (pointer) {
        if (strcmp(resourceUri, pointer->uri) == 0) {
            return pointer;
        }
        pointer = pointer->next;
    }
    OC_LOG(INFO, TAG, PCF("Resource not found"));
    return NULL;
}


OCStackResult DetermineResourceHandling (OCRequest *request,
                                         ResourceHandling *handling,
                                         OCResource **resource)
{

    OC_LOG(INFO, TAG, PCF("Entering DetermineResourceHandling"));

    // Check if virtual resource
    if (IsVirtualResource((const char*)request->resourceUrl))
    {
        *handling = OC_RESOURCE_VIRTUAL;
        *resource = headResource;
        return OC_STACK_OK;
    }
    if (NULL == request->resourceUrl || (strlen((const char*)(request->resourceUrl)) == 0))
    {
        // Resource URL not specified
        *handling = OC_RESOURCE_NOT_SPECIFIED;
        return OC_STACK_OK;
    }
    else
    {
        OCResource *resourcePtr = NULL;
        resourcePtr = FindResourceByUri((const char*)request->resourceUrl);
        *resource = resourcePtr;
        if (!resourcePtr)
        {
            if(defaultDeviceHandler)
            {
                *handling = OC_RESOURCE_DEFAULT_DEVICE_ENTITYHANDLER;
                return OC_STACK_OK;
            }

            // Resource does not exist
            // and default device handler does not exist
            return OC_STACK_NO_RESOURCE;
        }

        // secure resource will entertain only authorized requests
        if ((resourcePtr->resourceProperties & OC_SECURE) && (request->secure == 0))
        {
            OC_LOG(INFO, TAG, PCF("Un-authorized request. Ignore it!"));
            return OC_STACK_RESOURCE_ERROR;
        }

        if (IsCollectionResource (resourcePtr))
        {
            // Collection resource
            if (resourcePtr->entityHandler != defaultResourceEHandler)
            {
                *handling = OC_RESOURCE_COLLECTION_WITH_ENTITYHANDLER;
                return OC_STACK_OK;
            } else {
                *handling = OC_RESOURCE_COLLECTION_DEFAULT_ENTITYHANDLER;
                return OC_STACK_OK;
            }
        } else {
            // Resource not a collection
            if (resourcePtr->entityHandler != defaultResourceEHandler)
            {
                *handling = OC_RESOURCE_NOT_COLLECTION_WITH_ENTITYHANDLER;
                return OC_STACK_OK;
            } else {
                *handling = OC_RESOURCE_NOT_COLLECTION_DEFAULT_ENTITYHANDLER;
                return OC_STACK_OK;
            }
        }
    }
}

OCStackResult EntityHandlerCodeToOCStackCode(OCEntityHandlerResult ehResult)
{
    OCStackResult result;

    switch (ehResult)
    {
        case OC_EH_OK:
            result = OC_STACK_OK;
            break;
        case OC_EH_ERROR:
            result = OC_STACK_ERROR;
            break;
        case OC_EH_FORBIDDEN:
            result = OC_STACK_RESOURCE_ERROR;
            break;
        case OC_EH_RESOURCE_CREATED:
            result = OC_STACK_RESOURCE_CREATED;
            break;
        case OC_EH_RESOURCE_DELETED:
            result = OC_STACK_NO_RESOURCE;
            break;
        default:
            result = OC_STACK_ERROR;
    }

    return result;
}

static OCStackResult
HandleVirtualResource (OCRequest *request, OCResource* resource)
{
    OCStackResult result = OC_STACK_ERROR;
    char *filterValue = NULL;
    uint8_t filterOn = 0;
    uint16_t remaining = 0;
    unsigned char *buffer = NULL;

    OC_LOG(INFO, TAG, PCF("Entering HandleVirtualResource"));

    result = ValidateUrlQuery (request->resourceUrl,
            request->entityHandlerRequest->query, &filterOn,
            &filterValue);

    if (result == OC_STACK_OK)
    {
        if (strcmp ((char *)request->resourceUrl, GetVirtualResourceUri(OC_WELL_KNOWN_URI)) == 0)
        {
            remaining = request->entityHandlerRequest->resJSONPayloadLen;
            buffer = request->entityHandlerRequest->resJSONPayload;
            while(resource)
            {
                if((resource->resourceProperties & OC_ACTIVE)
                        && (resource->resourceProperties & OC_DISCOVERABLE))
                {
                    // if there is data on the buffer, we have already added a response,
                    // so we need to add a comma before we do anything
                    if(buffer != request->entityHandlerRequest->resJSONPayload
                        && remaining >= (sizeof(OC_JSON_SEPARATOR)+1))
                    {
                        *buffer = OC_JSON_SEPARATOR;
                        buffer++;
                        remaining--;
                    }

                    result = BuildVirtualResourceResponse(resource, filterOn, filterValue,
                            (char*)buffer, &remaining);
                    if (result != OC_STACK_OK)
                    {
                        // if this failed, we need to remove the comma added above.
                        if(buffer != request->entityHandlerRequest->resJSONPayload)
                        {
                            buffer--;
                            *buffer = '\0';
                            remaining++;
                        }

                        break;
                    }
                    buffer += strlen((char*)buffer);
                }
                resource = resource->next;
            }
        }
        #ifdef WITH_PRESENCE
        else
        {
            if(resource->resourceProperties & OC_ACTIVE){
                SendPresenceNotification(resource->rsrcType, OC_LOW_QOS);
            }
            result = OC_STACK_PRESENCE_DO_NOT_HANDLE;
        }
        #endif
    }

    if (result == OC_STACK_OK)
    {
        request->entityHandlerRequest->resJSONPayloadLen = remaining;
        request->entityHandlerRequest->resJSONPayload = buffer;
    }

    return result;
}

static OCStackResult
HandleDefaultDeviceEntityHandler (OCRequest *request)
{
    OCStackResult result = OC_STACK_OK;
    OCEntityHandlerResult ehResult;
    OCEntityHandlerRequest *ehRequest = request->entityHandlerRequest;

    // At this point we know for sure that defaultDeviceHandler exists
    ehResult = defaultDeviceHandler(OC_REQUEST_FLAG, ehRequest,
                                  (char*) request->resourceUrl);

    result = EntityHandlerCodeToOCStackCode(ehResult);

    ehRequest->resJSONPayloadLen = ehRequest->resJSONPayloadLen -
                                    strlen((char*)ehRequest->resJSONPayload);
    ehRequest->resJSONPayload += strlen((char*)ehRequest->resJSONPayload);

    return result;
}

static OCStackResult
HandleResourceWithEntityHandler (OCRequest *request,
                                 OCResource *resource,
                                 uint8_t collectionResource)
{
    OCStackResult result = OC_STACK_OK;
    OCEntityHandlerResult ehResult = OC_EH_OK;

    OCEntityHandlerRequest *ehRequest = request->entityHandlerRequest;

    OC_LOG(INFO, TAG, PCF("Entering HandleResourceWithEntityHandler"));

    ehRequest->resource = (OCResourceHandle)resource;

    // status code from entity handler is ignored unless observe call
    if (request->observe == NULL)
    {
        ehResult = resource->entityHandler(OC_REQUEST_FLAG, ehRequest);
        result = EntityHandlerCodeToOCStackCode(ehResult);
    }
    else
    {
        // If an observation register/deregister is included handle separately
        if (!collectionResource)
        {
            result = ProcessObserveRequest (resource, request);
        }
        else
        {
            // Observation on collection resources not currently supported
            result = OC_STACK_ERROR;
        }
    }

    if (result == OC_STACK_OK || OC_STACK_RESOURCE_CREATED)
    {
        ehRequest->resJSONPayloadLen = ehRequest->resJSONPayloadLen -
            strlen((char*)ehRequest->resJSONPayload);
        ehRequest->resJSONPayload += strlen((char*)ehRequest->resJSONPayload);
    }

    return result;
}


static OCStackResult
HandleCollectionResourceDefaultEntityHandler (OCRequest *request,
                                              OCResource *resource)
{
    request->entityHandlerRequest->resource = (OCResourceHandle)resource;
    return (DefaultCollectionEntityHandler (OC_REQUEST_FLAG, request->entityHandlerRequest));
}


OCStackResult
BuildJSONResponse(ResourceHandling resHandling, OCResource *resource, OCRequest *request)
{
    OCStackResult ret = OC_STACK_OK;

    // save the response payload pointer, this pointer will be moved as
    // different entity handlers will be called
    unsigned char* saveJSONPayLoadPtr = request->entityHandlerRequest->resJSONPayload;
    unsigned char* buffer = saveJSONPayLoadPtr;
    uint16_t remaining = request->entityHandlerRequest->resJSONPayloadLen;

    // add suffix in payload
    if (remaining > OC_JSON_PREFIX_LEN)
    {
        strcpy((char*)buffer, OC_JSON_PREFIX);
        remaining -= OC_JSON_PREFIX_LEN;
        buffer += OC_JSON_PREFIX_LEN;
    }

    // move the entity handler payload pointer and update
    // remaining valid bytes to fill data
    request->entityHandlerRequest->resJSONPayload = buffer;
    request->entityHandlerRequest->resJSONPayloadLen = remaining;

    switch (resHandling)
    {
        case OC_RESOURCE_VIRTUAL:
            {
                ret = HandleVirtualResource (request, resource);
                break;
            }

        case OC_RESOURCE_DEFAULT_DEVICE_ENTITYHANDLER:
            {
                ret = HandleDefaultDeviceEntityHandler(request);
                break;
            }
        case OC_RESOURCE_NOT_COLLECTION_DEFAULT_ENTITYHANDLER:
            {
                OC_LOG(INFO, TAG, PCF("OC_RESOURCE_NOT_COLLECTION_DEFAULT_ENTITYHANDLER"));
                return OC_STACK_ERROR;
            }

        case OC_RESOURCE_NOT_COLLECTION_WITH_ENTITYHANDLER:
            {
                ret = HandleResourceWithEntityHandler (request, resource, 0);
                break;
            }
        case OC_RESOURCE_COLLECTION_WITH_ENTITYHANDLER:
            {
                ret = HandleResourceWithEntityHandler (request, resource, 1);
                break;
            }

        case OC_RESOURCE_COLLECTION_DEFAULT_ENTITYHANDLER:
            {
                ret = HandleCollectionResourceDefaultEntityHandler (request, resource);
                break;
            }

        default:
            {
                OC_LOG(INFO, TAG, PCF("Invalid Resource Determination"));
                return OC_STACK_ERROR;
            }
    }

    remaining = request->entityHandlerRequest->resJSONPayloadLen;

    if (remaining > OC_JSON_SUFFIX_LEN)
    {
        strcpy((char*)request->entityHandlerRequest->resJSONPayload, OC_JSON_SUFFIX);
    }

    // update payload pointer with it's original location and original length
    request->entityHandlerRequest->resJSONPayload = saveJSONPayLoadPtr;
    request->entityHandlerRequest->resJSONPayloadLen = MAX_RESPONSE_LENGTH;

    return ret;
}


