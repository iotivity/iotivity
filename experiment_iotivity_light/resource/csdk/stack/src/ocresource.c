//******************************************************************
//
// Copyright 2016 Intel Corporation All Rights Reserved.
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

// Defining _POSIX_C_SOURCE macro with 200112L (or greater) as value
// causes header files to expose definitions
// corresponding to the POSIX.1-2001 base
// specification (excluding the XSI extension).
// For POSIX.1-2001 base specification,
// Refer http://pubs.opengroup.org/onlinepubs/009695399/
#define _POSIX_C_SOURCE 200112L
#include <string.h>
#include "octypes.h"
#include "ocresource.h"
#include "ocserverrequest.h"
#include "ocresourcehandler.h"
#include "ocobserve.h"
#include "occollection.h"
#include "ocadaptation.h"
#include "oic_string.h"
#include "ocpayload.h"
#include "ocpayloadint.h"
//#include "secureresourcemanager.h"

/// Module Name
#define TAG PCF("ocresource")

extern OCResource *headResource;
static OCPlatformInfo savedPlatformInfo = { .platformID={0} };
static OCDeviceInfo savedDeviceInfo = { .deviceName={0}};

//-----------------------------------------------------------------------------
// Default resource entity handler function
//-----------------------------------------------------------------------------
OCEntityHandlerResult defaultResourceEHandler(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest * request, void* callbackParam)
{
    //TODO ("Implement me!!!!");
    (void) flag;
    (void) request;
    (void) callbackParam;
    return  OC_EH_OK; // Making sure that the Default EH and the Vendor EH have matching signatures
}

#ifdef WITH_SECURITY
/* This method will retrieve the port at which the secure resource is hosted */
static OCStackResult GetSecurePortInfo(OCDevAddr *endpoint, uint16_t *port)
{
    uint16_t p = 0;

    if (endpoint->adapter == OC_ADAPTER_IP)
    {
        if (endpoint->flags & OC_IP_USE_V6)
        {
            p = caglobals.ip.u6s.port;
        }
        else if (endpoint->flags & OC_IP_USE_V4)
        {
            p = caglobals.ip.u4s.port;
        }
    }

    *port = p;
    return OC_STACK_OK;
}
#endif // WITH_SECURITY

/*
 * Function will extract 0, 1 or 2 filters from query.
 * More than 2 filters or unsupported filters will result in error.
 * If both filters are of the same supported type, the 2nd one will be picked.
 * Resource and device filters in the SAME query are NOT validated
 * and resources will likely not clear filters.
 */
static OCStackResult ExtractFiltersFromQuery(char *query, char *rtFilter, char *ifFilter)
{
    char *key = NULL;
    char *value = NULL;
    char *restOfQuery = NULL;
    int numKeyValuePairsParsed = 0;

    OC_LOG_V(INFO, TAG, "Extracting params from %s", query);

    char *keyValuePair = strtok_r(query, OC_QUERY_SEPARATOR, &restOfQuery);

    while (keyValuePair)
    {
        if (numKeyValuePairsParsed >= 2)
        {
            OC_LOG(ERROR, TAG, PCF("More than 2 queries params in URI."));
            return OC_STACK_INVALID_QUERY;
        }

        key = strtok_r(keyValuePair, OC_KEY_VALUE_DELIMITER, &value);
        if (!key || !value)
        {
            return OC_STACK_INVALID_QUERY;
        }

        if (strcmp(key, OC_RSRVD_INTERFACE) == 0)
        {
            OICStrcpy(ifFilter, URI_QUERY_LENGTH, value);
        }
        else if (strcmp (key, OC_RSRVD_RESOURCE_TYPE) == 0)
        {
            OICStrcpy(rtFilter, URI_QUERY_LENGTH, value);
        }
        else
        {
            OC_LOG_V(ERROR, TAG, "Unsupported query key: %s", key);
            return OC_STACK_INVALID_QUERY;
        }
        ++numKeyValuePairsParsed;

        keyValuePair = strtok_r(NULL, OC_QUERY_SEPARATOR, &restOfQuery);
    }

    OC_LOG_V(INFO, TAG, "Extracted params %s and %s.", *rtFilter, *ifFilter);
    return OC_STACK_OK;
}

#ifdef WITH_COLLECTIONS
OCStackResult BuildResponseRepresentation(const OCResource *resource,
                                                        OCRepPayload** payload)
{
    OCRepPayload *tempPayload = OCRepPayloadCreate();
    if (!tempPayload)
    {
        return OC_STACK_NO_MEMORY;
    }

    OCRepPayloadSetUri(tempPayload, resource->uri);

#ifdef RT_MAX
    for (int i = 0; i < resource->nTypes; i++)
    {
        OCRepPayloadAddResourceType(tempPayload, resource->rsrcType[i].name);
    }
#else
    const OCResourceText_t *resType = resource->rsrcType;
    for (; resType; resType = resType->next)
    {
        OCRepPayloadAddResourceType(tempPayload, resType->name);
    }
#endif

#ifdef RI_MAX
    for (int i = 0; i < resource->nInterfaces; i++)
    {
        OCRepPayloadAddInterface(tempPayload, resource->rsrcInterface[i].name);
    }
#else
    const OCResourceText_t *resInterface = resource->rsrcInterface;
    for (; resInterface; resInterface = resInterface->next)
    {
        OCRepPayloadAddInterface(tempPayload, resInterface->name);
    }
#endif

#ifdef resourceattributeused
    const OCResourceAttribute *resAttrib = resource->rsrcAttributes;
    for (; resAttrib; resAttrib = resAttrib->next)
    {
        OCRepPayloadSetPropString(tempPayload, resAttrib->attrName,
                                resAttrib->attrValue);
    }
#endif

    if (*payload)
    {
        OCRepPayloadAppend(*payload, tempPayload);
    }
    else
    {
        *payload = tempPayload;
    }

    return OC_STACK_OK;
}
#endif // WITH_COLLECTIONS

#ifdef WITH_RESOURCE_API
uint8_t IsCollectionResource(OCResource *resource)
{
#ifdef WITH_COLLECTIONS
    for (int i = 0; i < MAX_CONTAINED_RESOURCES; i++)
    {
        if (resource->rsrcResources[i])
        {
            return 1;
        }
    }
    return 0;
#else
    (void)resource;
    return 0;
#endif // WITH_COLLECTIONS
}
#endif // WITH_RESOURCE_API

OCStackResult EntityHandlerCodeToOCStackCode(OCEntityHandlerResult ehResult)
{
    static OCStackResult ocresults[] =
    {   OC_STACK_OK,
        OC_STACK_ERROR,
        OC_STACK_RESOURCE_CREATED,
        OC_STACK_RESOURCE_DELETED,
        OC_STACK_SLOW_RESOURCE,
        OC_STACK_RESOURCE_ERROR,
        OC_STACK_NO_RESOURCE
    };

    if ((size_t)ehResult >= sizeof (ocresults) / sizeof (OCStackResult))
    {
        return OC_STACK_ERROR;
    }
    return ocresults[ehResult];   
}

static bool resourceMatchesRTFilter(OCResource *resource, char *rtFilter)
{
#ifdef RT_MAX
    for (int i = 0; i < resource->nTypes; i++)
    {
        if (strcmp(resource->rsrcType[i].name, rtFilter) == 0)
        {
            return true;
        }
    }
#else
    OCResourceText_t *resourceTypePtr = resource->rsrcType;
    for (; resourceTypePtr; resourceTypePtr = resourceTypePtr->next)
    {
        if (strcmp(resourceTypePtr->name, rtFilter) == 0)
        {
            return true;
        }
    }
#endif

    OC_LOG_V(INFO, TAG, PCF("%s does not contain rt=%s."), resource->uri, rtFilter);
    return false;
}

static bool resourceMatchesIFFilter(OCResource *resource, char *ifFilter)
{
#ifdef RI_MAX
    for (int i = 0; i < resource->nInterfaces; i++)
    {
        if (strcmp(resource->rsrcInterface[i].name, ifFilter) == 0)
        {
            return true;
        }
    }
#else
    OCResourceText_t *interfacePtr = resource->rsrcInterface;
    for  ( ; interfacePtr; interfacePtr = interfacePtr->next)
    {
        if (strcmp(interfacePtr->name, ifFilter) == 0)
        {
            return true;
        }
    }
#endif

    OC_LOG_V(INFO, TAG, PCF("%s does not contain if=%s."), resource->uri, ifFilter);
    return false;
}

/*
 * If the filters are null, they will be assumed to NOT be present
 * and the resource will not be matched against them.
 * Function will return true if all non null AND non empty filters passed in find a match.
 */
bool IncludeThisResource(OCResource *resource, char *rtFilter, char *ifFilter)
{
    if (!(resource->resourceProperties & OC_ACTIVE) ||
                            !(resource->resourceProperties & OC_DISCOVERABLE))
    {
        OC_LOG_V(INFO, TAG, PCF("%s not ACTIVE and DISCOVERABLE"), resource->uri);
        return false;
    }

    return resourceMatchesIFFilter(resource, ifFilter) &&
           resourceMatchesRTFilter(resource, rtFilter);
}

static OCEntityHandlerResult PrepareDiscoveryResponse(uint8_t *buf,
                                        size_t *size, OCRequestInfo_t *request)
{
    OCStackResult result = OC_STACK_OK;
    char rtFilter[URI_QUERY_LENGTH] = { '\0' };
    char ifFilter[URI_QUERY_LENGTH] = { '\0' };

    CAInfo_t *info = &(((CAMessageInfo_t *)request)->info);
    bool filter = info->resourceQuery[0];  // no filters
    if (filter)
    {
        result = ExtractFiltersFromQuery(info->resourceQuery, rtFilter, ifFilter);
        if (result != OC_STACK_OK)
        {
            return OC_EH_ERROR;
        }
    }

    OCResource *resourceList[100];
    int count = 0;

    OCResource *resource = headResource;
    for ( ;resource && result == OC_STACK_OK; resource = resource->next)
    {
        if (!filter || IncludeThisResource(resource, rtFilter, ifFilter))
        {
            resourceList[count++] = resource;
        }
    }

    result = OCBuildDiscoveryPayload(buf, size, resourceList, count);
    if (result != OC_STACK_OK)
    {
        return OC_EH_ERROR;
    }

    if (!count)
    {
        return OC_EH_RESOURCE_NOT_FOUND;
    }

    return OC_EH_OK;
}

static OCStackResult HandleVirtualResource(OCRequestInfo_t *request)
{
    OCStackResult result = OC_STACK_OK;
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OCEntityHandlerResponse response = {0};

    // prepare for encoding.  Buffer was allocated by caller.
    CAMessageInfo_t *message = (CAMessageInfo_t *)request;
    uint8_t *buf = message->encodedPayload;
    size_t *size = &(message->encodedSize);
    *size = MAX_ENCODED_PAYLOAD_LENGTH;         // ready for encoding

    // Step 1: Generate the response to discovery request
    if (!strcmp(message->info.resourcePath, OC_RSRVD_WELL_KNOWN_URI))
    {
        ehResult = PrepareDiscoveryResponse(buf, size, request);
    }
    else
    {
        if (!strcmp(message->info.resourcePath, OC_RSRVD_DEVICE_URI))
        {
            result = OCBuildDevicePayload(buf, size, &savedDeviceInfo);
        }
        else if (!strcmp(message->info.resourcePath, OC_RSRVD_PLATFORM_URI))
        {
            result = OCBuildPlatformPayload(buf, size, &savedPlatformInfo);
        }
        #ifdef WITH_PRESENCE
        else if (!strcmp(message->info.resourcePath, OC_RSRVD_PRESENCE_URI))
        {
            result = OCBuildPresencePayload(buf, size,
                                       ThePresenceResource.resource.sequenceNum,
                                       ThePresenceResource.presenceTTL,
                                       OC_PRESENCE_TRIGGER_CHANGE,
                                       OC_RSRVD_PRESENCE_URI);
        }
        #endif
        else
        {
            return OC_STACK_NO_RESOURCE;
        }
        if (result == OC_STACK_OK)
        {
            ehResult = OC_EH_OK;
        }
    }

    /**
     * Step 2: Send the discovery response
     *
     * Iotivity should respond to discovery requests in below manner:
     * 1)If query filter matching fails and discovery request is multicast,
     *   it should NOT send any response.
     * 2)If query filter matching fails and discovery request is unicast,
     *   it should send an error(RESOURCE_NOT_FOUND - 404) response.
     * 3)If Server does not have any 'DISCOVERABLE' resources and discovery
     *   request is multicast, it should NOT send any response.
     * 4)If Server does not have any 'DISCOVERABLE' resources and discovery
     *   request is unicast, it should send an error(RESOURCE_NOT_FOUND - 404) response.
     */

    if (ehResult != OC_EH_OK && (request->messageInfo.devAddr.flags & OC_MULTICAST))
    {
        // Multicast discovery errors are ignored as per RFC 7252, Section #8.2
        return OC_STACK_OK;
    }

    response.ehResult = ehResult;
    response.requestHandle = (OCRequestHandle)request;

    return HandleSingleResponse(&response); // can't return OC_STACK_NO_RESOURCE
}

static OCStackResult HandleDefaultDeviceEntityHandler(OCRequestInfo_t *request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OCEntityHandlerRequest ehRequest = {0};

    FormOCEntityHandlerRequest(&ehRequest, request, NULL);

    ehResult = DefaultDeviceHandler(OC_REQUEST_FLAG, &ehRequest,
                                  (char*)request->messageInfo.info.resourcePath,
                                  DefaultDeviceHandlerCallbackParameter);
    return EntityHandlerCodeToOCStackCode(ehResult);
}

#ifdef WITH_RESOURCE_API
static OCStackResult HandleResourceWithEntityHandler(OCRequestInfo_t *request,
                                                     OCResource *resource,
                                                     uint8_t collectionResource)
{
    OCStackResult result = OC_STACK_ERROR;
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OCEntityHandlerFlag ehFlag = OC_REQUEST_FLAG;
    ResourceObserver *resObs = NULL;

    OCEntityHandlerRequest ehReqStatic = {0};
    OCEntityHandlerRequest *ehRequest = &ehReqStatic;
    if (resource->resourceProperties & OC_SLOW)
    {
        ehRequest = (OCEntityHandlerRequest *)OCCalloc(MAT_entity);
    }

    OC_LOG(INFO, TAG, PCF("Entering HandleResourceWithEntityHandler"));

    FormOCEntityHandlerRequest(ehRequest, request, resource);

    OCObserveAction action = ehRequest->obsInfo.action;

    if (action == OC_OBSERVE_NO_OPTION)
    {
        OC_LOG(INFO, TAG, PCF("No observation requested"));
        ehFlag = OC_REQUEST_FLAG;
    }
    else if (action == OC_OBSERVE_REGISTER && !collectionResource)
    {
        OC_LOG(INFO, TAG, PCF("Observation registration requested"));

        result = GenerateObserverId(&ehRequest->obsInfo.obsId);
        if (result != OC_STACK_OK)
        {
            goto exit;
        }

        CAMessageInfo_t *message = (CAMessageInfo_t *)request;
        result = AddObserver((const char*)(message->info.resourcePath),
                (const char *)(message->info.resourceQuery),
                ehRequest->obsInfo.obsId, &(message->info.token),
                resource, request->qos,
                &(message->devAddr));

        if (result == OC_STACK_OK)
        {
            OC_LOG(INFO, TAG, PCF("Added observer successfully"));
            request->observeResult = OC_STACK_OK;
            ehFlag = (OCEntityHandlerFlag)(OC_REQUEST_FLAG | OC_OBSERVE_FLAG);
        }
        else
        {
            result = OC_STACK_OK;

            // The error in observeResult for the request will be used when responding to this
            // request by omitting the observation option/sequence number.
            request->observeResult = OC_STACK_ERROR;
            OC_LOG(ERROR, TAG, PCF("Observer Addition failed"));
            ehFlag = OC_REQUEST_FLAG;
        }

    }
    else if (action == OC_OBSERVE_DEREGISTER && !collectionResource)
    {
        OC_LOG(INFO, TAG, PCF("Deregistering observation requested"));

        resObs = GetObserverUsingToken(&(request->messageInfo.info.token));
        if (NULL == resObs)
        {
            // Stack does not contain this observation request
            // Either token is incorrect or observation list is corrupted
            goto exit;
        }
        ehRequest->obsInfo.obsId = resObs->observeId;
        ehFlag = (OCEntityHandlerFlag)(ehFlag | OC_OBSERVE_FLAG);

        request->observeResult = DeleteObserverUsingToken(&(request->messageInfo.info.token));
    }
    else
    {
        return OC_STACK_ERROR;
    }

    ehResult = resource->entityHandler(ehFlag, ehRequest, resource->ehCallbackParam);
exit:
    if (resource->resourceProperties & OC_SLOW)
    {
        if (ehResult == OC_EH_SLOW)
        {
            OC_LOG(INFO, TAG, PCF("This is a slow resource"));
            request->slowFlag = 1;
        } else {
            OCFree(ehRequest, MAT_entity);
        }
    }
    if (ehResult == OC_EH_ERROR)
    {
        OCFree(request, MAT_message);
    }

    return EntityHandlerCodeToOCStackCode(ehResult);
}

#ifdef WITH_COLLECTIONS
static OCStackResult HandleCollectionResourceDefaultEntityHandler(
                                OCRequestInfo_t *request, OCResource *resource)
{
    OCStackResult result = OC_STACK_ERROR;
    OCEntityHandlerRequest ehRequest = {0};

    result = FormOCEntityHandlerRequest(&ehRequest, request, resource);
    if (result == OC_STACK_OK)
    {
        result = DefaultCollectionEntityHandler (OC_REQUEST_FLAG, &ehRequest);
    }

    OCPayloadDestroy(ehRequest.payload);
    return result;
}
#endif // WITH_COLLECTIONS

OCResource *FindResourceByUri(const char* resourceUri)
{
    OCResource *pointer = headResource;
    while (pointer)
    {
        if (strcmp(resourceUri, pointer->uri) == 0)
        {
            return pointer;
        }
        pointer = pointer->next;
    }
    OC_LOG_V(INFO, TAG, "Resource %s not found", resourceUri);
    return NULL;
}
#endif // WITH_RESOURCE_API

OCStackResult ProcessRequest(OCRequestInfo_t *request)
{
    // This puts the encoded payload buffer on the stack instead of the heap.
    // This allows us to hold onto the request structure.
    // If the request structure is kept around (slow response, observer, etc.),
    // the pointer to this buffer must be NULLed.
    CAMessageInfo_t *message = (CAMessageInfo_t *)request;
    uint8_t encodedPayload[MAX_ENCODED_PAYLOAD_LENGTH];
    message->encodedPayload = encodedPayload;
    message->encodedSize = 0;   // set non-zero as part of encoding

    OCStackResult result = HandleVirtualResource(request);
    if (result != OC_STACK_NO_RESOURCE)
    {
        return result;
    }

#ifdef WITH_RESOURCE_API
    OCResource *resource = FindResourceByUri(message->info.resourcePath);
    if (!resource)
    {
        if (DefaultDeviceHandler)
        {
            return HandleDefaultDeviceEntityHandler(request);
        }
        return OC_STACK_NO_RESOURCE;
    }

#ifdef WITH_COLLECTIONS
    if (IsCollectionResource(resource))
    {
        if (resource->entityHandler != defaultResourceEHandler)
        {
            return HandleResourceWithEntityHandler(request, resource, 1);
        }
        return HandleCollectionResourceDefaultEntityHandler(request, resource);
    }
#endif // WITH_COLLECTIONS

    if (resource->entityHandler != defaultResourceEHandler)
    {
        return HandleResourceWithEntityHandler(request, resource, 0);
    }

    OC_LOG(INFO, TAG, "default entity handler, but not collection");
    return OC_STACK_ERROR;
#else
    if (!DefaultDeviceHandler)
    {
        return OC_STACK_NO_RESOURCE;
    }
    return HandleDefaultDeviceEntityHandler(request);
#endif // WITH_RESOURCE_API
}

void DeletePlatformInfo()
{
    memset(&savedPlatformInfo, 0, sizeof (savedPlatformInfo));
}

void SavePlatformInfo(const OCPlatformInfo *info)
{
    savedPlatformInfo = *info;
}

void DeleteDeviceInfo()
{
    memset(&savedDeviceInfo, 0, sizeof (savedDeviceInfo));
}

void SaveDeviceInfo(const OCDeviceInfo *info)
{
    savedDeviceInfo = *info;
}
