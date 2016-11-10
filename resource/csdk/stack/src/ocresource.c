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

// Defining _POSIX_C_SOURCE macro with 200112L (or greater) as value
// causes header files to expose definitions
// corresponding to the POSIX.1-2001 base
// specification (excluding the XSI extension).
// For POSIX.1-2001 base specification,
// Refer http://pubs.opengroup.org/onlinepubs/009695399/
#define _POSIX_C_SOURCE 200112L
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "iotivity_config.h"
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include "ocresource.h"
#include "ocresourcehandler.h"
#include "ocobserve.h"
#include "occollection.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "logger.h"
#include "cJSON.h"
#include "ocpayload.h"
#include "secureresourcemanager.h"
#include "cacommon.h"
#include "cainterface.h"
#include "ocpayload.h"
#include "platform_features.h"

#ifdef ROUTING_GATEWAY
#include "routingmanager.h"
#endif

#ifdef RD_SERVER
#include "rd_database.h"
#endif

/// Module Name
#define TAG "OIC_RI_RESOURCE"

#define VERIFY_SUCCESS(op, successCode) { if (op != successCode) \
            {OIC_LOG_V(FATAL, TAG, "%s failed!!", #op); goto exit;} }

extern OCResource *headResource;
static OCPlatformInfo savedPlatformInfo = {0};
static OCDeviceInfo savedDeviceInfo = {0};

/**
 * Prepares a Payload for response.
 */
static OCStackResult BuildVirtualResourceResponse(const OCResource *resourcePtr,
                                                  OCDiscoveryPayload* payload,
                                                  OCDevAddr *endpoint);

//-----------------------------------------------------------------------------
// Default resource entity handler function
//-----------------------------------------------------------------------------
OCEntityHandlerResult defaultResourceEHandler(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest * request, void* callbackParam)
{
    //TODO ("Implement me!!!!");
    // TODO:  remove silence unused param warnings
    (void) flag;
    (void) request;
    (void) callbackParam;
    return  OC_EH_OK; // Making sure that the Default EH and the Vendor EH have matching signatures
}

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

#ifdef TCP_ADAPTER
/* This method will retrieve the tcp port */
static OCStackResult GetTCPPortInfo(OCDevAddr *endpoint, uint16_t *port)
{
    uint16_t p = 0;

    if (endpoint->adapter == OC_ADAPTER_IP)
    {
        if (endpoint->flags & OC_IP_USE_V4)
        {
            p = caglobals.tcp.ipv4.port;
        }
        else if (endpoint->flags & OC_IP_USE_V6)
        {
            p = caglobals.tcp.ipv6.port;
        }
    }

    *port = p;
    return OC_STACK_OK;
}
#endif

/*
 * Function will extract 0, 1 or 2 filters from query.
 * More than 2 filters or unsupported filters will result in error.
 * If both filters are of the same supported type, the 2nd one will be picked.
 * Resource and device filters in the SAME query are NOT validated
 * and resources will likely not clear filters.
 */
static OCStackResult ExtractFiltersFromQuery(char *query, char **filterOne, char **filterTwo)
{
    char *key = NULL;
    char *value = NULL;
    char *queryDup = NULL;
    char *restOfQuery = NULL;
    char *keyValuePair = NULL;
    int numKeyValuePairsParsed = 0;

    *filterOne = NULL;
    *filterTwo = NULL;

    queryDup = OICStrdup(query);
    if (NULL == queryDup)
    {
        OIC_LOG_V(ERROR, TAG, "Creating duplicate string failed!");
        return OC_STACK_NO_MEMORY;
    }

    OIC_LOG_V(INFO, TAG, "Extracting params from %s", queryDup);

    OCStackResult eCode = OC_STACK_INVALID_QUERY;
    if (strnlen(queryDup, MAX_QUERY_LENGTH) >= MAX_QUERY_LENGTH)
    {
        OIC_LOG(ERROR, TAG, "Query exceeds maximum length.");
        goto exit;
    }

    keyValuePair = strtok_r (queryDup, OC_QUERY_SEPARATOR, &restOfQuery);

    while(keyValuePair)
    {
        if (numKeyValuePairsParsed >= 2)
        {
            OIC_LOG(ERROR, TAG, "More than 2 queries params in URI.");
            goto exit;
        }

        key = strtok_r(keyValuePair, OC_KEY_VALUE_DELIMITER, &value);

        if (!key || !value)
        {
            goto exit;
        }
        else if (strncasecmp(key, OC_RSRVD_INTERFACE, sizeof(OC_RSRVD_INTERFACE) - 1) == 0)
        {
            *filterOne = value;     // if
        }
        else if (strncasecmp(key, OC_RSRVD_RESOURCE_TYPE, sizeof(OC_RSRVD_INTERFACE) - 1) == 0)
        {
            *filterTwo = value;     // rt
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "Unsupported query key: %s", key);
            goto exit;
        }
        ++numKeyValuePairsParsed;

        keyValuePair = strtok_r(NULL, OC_QUERY_SEPARATOR, &restOfQuery);
    }

    if (*filterOne)
    {
        *filterOne = OICStrdup(*filterOne);
        if (NULL == *filterOne)
        {
            OIC_LOG_V(ERROR, TAG, "Creating duplicate string failed!");
            eCode = OC_STACK_NO_MEMORY;
            goto exit;
        }
    }

    if (*filterTwo)
    {
        *filterTwo = OICStrdup(*filterTwo);
        if (NULL == *filterTwo)
        {
            OIC_LOG_V(ERROR, TAG, "Creating duplicate string failed!");
            OICFree(*filterOne);
            eCode = OC_STACK_NO_MEMORY;
            goto exit;
        }
    }

    OICFree(queryDup);
    OIC_LOG_V(INFO, TAG, "Extracted params if: %s and rt: %s.", *filterOne, *filterTwo);
    return OC_STACK_OK;

exit:
    *filterOne = NULL;
    *filterTwo = NULL;
    OICFree(queryDup);
    return eCode;
}

static OCVirtualResources GetTypeOfVirtualURI(const char *uriInRequest)
{
    if (strcmp(uriInRequest, OC_RSRVD_WELL_KNOWN_URI) == 0)
    {
        return OC_WELL_KNOWN_URI;
    }
    else if (strcmp(uriInRequest, OC_RSRVD_DEVICE_URI) == 0)
    {
        return OC_DEVICE_URI;
    }
    else if (strcmp(uriInRequest, OC_RSRVD_PLATFORM_URI) == 0)
    {
        return OC_PLATFORM_URI;
    }
    else if (strcmp(uriInRequest, OC_RSRVD_RESOURCE_TYPES_URI) == 0)
    {
        return OC_RESOURCE_TYPES_URI;
    }
#ifdef ROUTING_GATEWAY
    else if (0 == strcmp(uriInRequest, OC_RSRVD_GATEWAY_URI))
    {
        return OC_GATEWAY_URI;
    }
#endif
#ifdef WITH_PRESENCE
    else if (strcmp(uriInRequest, OC_RSRVD_PRESENCE_URI) == 0)
    {
        return OC_PRESENCE;
    }
#endif //WITH_PRESENCE

#ifdef MQ_BROKER
    else if (0 == strcmp(uriInRequest, OC_RSRVD_WELL_KNOWN_MQ_URI))
    {
        return OC_MQ_BROKER_URI;
    }
#endif //MQ_BROKER

#ifdef TCP_ADAPTER
    else if (strcmp(uriInRequest, OC_RSRVD_KEEPALIVE_URI) == 0)
    {
        return OC_KEEPALIVE_RESOURCE_URI;
    }
#endif

    return OC_UNKNOWN_URI;
}

static OCStackResult getQueryParamsForFiltering (OCVirtualResources uri, char *query,
                                            char **filterOne, char **filterTwo)
{
    if(!filterOne || !filterTwo)
    {
        return OC_STACK_INVALID_PARAM;
    }

    *filterOne = NULL;
    *filterTwo = NULL;

    #ifdef WITH_PRESENCE
    if (uri == OC_PRESENCE)
    {
        //Nothing needs to be done, except for pass a OC_PRESENCE query through as OC_STACK_OK.
        OIC_LOG(INFO, TAG, "OC_PRESENCE Request for virtual resource.");
        return OC_STACK_OK;
    }
    #endif

    OCStackResult result = OC_STACK_OK;

    if (query && *query)
    {
        result = ExtractFiltersFromQuery(query, filterOne, filterTwo);
    }

    return result;
}

OCStackResult BuildResponseRepresentation(const OCResource *resourcePtr,
                    OCRepPayload** payload)
{
    OCRepPayload *tempPayload = OCRepPayloadCreate();

    if (!resourcePtr)
    {
        OCRepPayloadDestroy(tempPayload);
        return OC_STACK_INVALID_PARAM;
    }

    if(!tempPayload)
    {
        return OC_STACK_NO_MEMORY;
    }

    OCRepPayloadSetUri(tempPayload, resourcePtr->uri);

    OCResourceType *resType = resourcePtr->rsrcType;
    while(resType)
    {
        OCRepPayloadAddResourceType(tempPayload, resType->resourcetypename);
        resType = resType->next;
    }

    OCResourceInterface *resInterface = resourcePtr->rsrcInterface;
    while(resInterface)
    {
        OCRepPayloadAddInterface(tempPayload, resInterface->name);
        resInterface = resInterface->next;
    }

    OCAttribute *resAttrib = resourcePtr->rsrcAttributes;
    while(resAttrib)
    {
        OCRepPayloadSetPropString(tempPayload, resAttrib->attrName,
                                resAttrib->attrValue);
        resAttrib = resAttrib->next;
    }

    if(!*payload)
    {
        *payload = tempPayload;
    }
    else
    {
        OCRepPayloadAppend(*payload, tempPayload);
    }

    return OC_STACK_OK;
}

OCStackResult BuildVirtualResourceResponse(const OCResource *resourcePtr,
                        OCDiscoveryPayload *payload, OCDevAddr *devAddr)
{
    if (!resourcePtr || !payload)
    {
        return OC_STACK_INVALID_PARAM;
    }
    uint16_t securePort = 0;
    if (resourcePtr->resourceProperties & OC_SECURE)
    {
       if (GetSecurePortInfo(devAddr, &securePort) != OC_STACK_OK)
       {
           securePort = 0;
       }
    }

#ifdef TCP_ADAPTER
    uint16_t tcpPort = 0;
    if (GetTCPPortInfo(devAddr, &tcpPort) != OC_STACK_OK)
    {
        tcpPort = 0;
    }
    OCDiscoveryPayloadAddResource(payload, resourcePtr, securePort, tcpPort);
#else
    OCDiscoveryPayloadAddResource(payload, resourcePtr, securePort);
#endif

    return OC_STACK_OK;
}

uint8_t IsCollectionResource (OCResource *resource)
{
    if(!resource)
    {
        return 0;
    }

    if(resource->rsrcChildResourcesHead != NULL)
    {
        return 1;
    }

    return 0;
}

OCResource *FindResourceByUri(const char* resourceUri)
{
    if(!resourceUri)
    {
        return NULL;
    }

    OCResource * pointer = headResource;
    while (pointer)
    {
        if (strcmp(resourceUri, pointer->uri) == 0)
        {
            return pointer;
        }
        pointer = pointer->next;
    }
    OIC_LOG_V(INFO, TAG, "Resource %s not found", resourceUri);
    return NULL;
}


OCStackResult DetermineResourceHandling (const OCServerRequest *request,
                                         ResourceHandling *handling,
                                         OCResource **resource)
{
    if(!request || !handling || !resource)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OIC_LOG_V(INFO, TAG, "DetermineResourceHandling for %s", request->resourceUrl);

    // Check if virtual resource
    if (GetTypeOfVirtualURI(request->resourceUrl) != OC_UNKNOWN_URI)
    {
        OIC_LOG_V (INFO, TAG, "%s is virtual", request->resourceUrl);
        *handling = OC_RESOURCE_VIRTUAL;
        *resource = headResource;
        return OC_STACK_OK;
    }
    if (strlen((const char*)(request->resourceUrl)) == 0)
    {
        // Resource URL not specified
        *handling = OC_RESOURCE_NOT_SPECIFIED;
        return OC_STACK_NO_RESOURCE;
    }
    else
    {
        OCResource *resourcePtr = FindResourceByUri((const char*)request->resourceUrl);
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
            *handling = OC_RESOURCE_NOT_SPECIFIED;
            return OC_STACK_NO_RESOURCE;
        }

        if (IsCollectionResource (resourcePtr))
        {
            // Collection resource
            if (resourcePtr->entityHandler != defaultResourceEHandler)
            {
                *handling = OC_RESOURCE_COLLECTION_WITH_ENTITYHANDLER;
                return OC_STACK_OK;
            }
            else
            {
                *handling = OC_RESOURCE_COLLECTION_DEFAULT_ENTITYHANDLER;
                return OC_STACK_OK;
            }
        }
        else
        {
            // Resource not a collection
            if (resourcePtr->entityHandler != defaultResourceEHandler)
            {
                *handling = OC_RESOURCE_NOT_COLLECTION_WITH_ENTITYHANDLER;
                return OC_STACK_OK;
            }
            else
            {
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
        case OC_EH_CONTENT:
        case OC_EH_VALID:
            result = OC_STACK_OK;
            break;
        case OC_EH_SLOW:
            result = OC_STACK_SLOW_RESOURCE;
            break;
        case OC_EH_ERROR:
            result = OC_STACK_ERROR;
            break;
        case OC_EH_FORBIDDEN:
            result = OC_STACK_FORBIDDEN_REQ;
            break;
        case OC_EH_INTERNAL_SERVER_ERROR:
            result = OC_STACK_INTERNAL_SERVER_ERROR;
            break;
        case OC_EH_RESOURCE_CREATED:
            result = OC_STACK_RESOURCE_CREATED;
            break;
        case OC_EH_RESOURCE_DELETED:
            result = OC_STACK_RESOURCE_DELETED;
            break;
        case OC_EH_CHANGED:
            result = OC_STACK_RESOURCE_CHANGED;
            break;
        case OC_EH_RESOURCE_NOT_FOUND:
            result = OC_STACK_NO_RESOURCE;
            break;
        default:
            result = OC_STACK_ERROR;
    }

    return result;
}

static bool resourceMatchesRTFilter(OCResource *resource, char *resourceTypeFilter)
{
    if (!resource)
    {
        return false;
    }

    // Null or empty is analogous to no filter.
    if (resourceTypeFilter == NULL || *resourceTypeFilter == 0)
    {
        return true;
    }

    OCResourceType *resourceTypePtr = resource->rsrcType;

    while (resourceTypePtr)
    {
        if (strcmp (resourceTypePtr->resourcetypename, resourceTypeFilter) == 0)
        {
            return true;
        }
        resourceTypePtr = resourceTypePtr->next;
    }

    OIC_LOG_V(INFO, TAG, "%s does not contain rt=%s.", resource->uri, resourceTypeFilter);
    return false;
}

static bool resourceMatchesIFFilter(OCResource *resource, char *interfaceFilter)
{
    if (!resource)
    {
        return false;
    }

    // Null or empty is analogous to no filter.
    if (interfaceFilter == NULL || *interfaceFilter == 0)
    {
        return true;
    }

    OCResourceInterface *interfacePtr = resource->rsrcInterface;

    while (interfacePtr)
    {
        if (strcmp (interfacePtr->name, interfaceFilter) == 0 ||
            strcmp (OC_RSRVD_INTERFACE_LL, interfaceFilter) == 0 ||
            strcmp (OC_RSRVD_INTERFACE_DEFAULT, interfaceFilter) == 0)
        {
            return true;
        }
        interfacePtr = interfacePtr->next;
    }

    OIC_LOG_V(INFO, TAG, "%s does not contain if=%s.", resource->uri, interfaceFilter);
    return false;
}

/*
 * If the filters are null, they will be assumed to NOT be present
 * and the resource will not be matched against them.
 * Function will return true if all non null AND non empty filters passed in find a match.
 */
static bool includeThisResourceInResponse(OCResource *resource,
                                          char *interfaceFilter,
                                          char *resourceTypeFilter)
{
    if (!resource)
    {
        OIC_LOG(ERROR, TAG, "Invalid resource");
        return false;
    }

    if (resource->resourceProperties & OC_EXPLICIT_DISCOVERABLE)
    {
        /*
         * At least one valid filter should be available to
         * include the resource in discovery response
         */
        if (!(resourceTypeFilter && *resourceTypeFilter))
        {
            OIC_LOG_V(INFO, TAG, "%s no query string for EXPLICIT_DISCOVERABLE \
                resource", resource->uri);
            return false;
        }
    }
    else if ( !(resource->resourceProperties & OC_ACTIVE) ||
         !(resource->resourceProperties & OC_DISCOVERABLE))
    {
        OIC_LOG_V(INFO, TAG, "%s not ACTIVE or DISCOVERABLE", resource->uri);
        return false;
    }

    return resourceMatchesIFFilter(resource, interfaceFilter) &&
           resourceMatchesRTFilter(resource, resourceTypeFilter);

}

OCStackResult SendNonPersistantDiscoveryResponse(OCServerRequest *request, OCResource *resource,
                                OCPayload *discoveryPayload, OCEntityHandlerResult ehResult)
{
    OCEntityHandlerResponse response = {0};

    response.ehResult = ehResult;
    response.payload = discoveryPayload;
    response.persistentBufferFlag = 0;
    response.requestHandle = (OCRequestHandle) request;
    response.resourceHandle = (OCResourceHandle) resource;

    return OCDoResponse(&response);
}

/**
 * Find resource at the resource directory server. This resource is not local resource but a
 * remote resource.
 *
 * @param resource The resource to check the matching resource URI.
 * @param interfaceQuery The interface query parameter.
 * @param resourceTypeQuery The resourceType query parameter.
 * @param discPayload The payload that will be added with the resource information if found at RD.
 *
 * @return ::OC_STACK_OK if the resource is found else ::OC_STACK_NO_RESOURCE.
 * In case if build is not with flag RD_SERVER, it returns ::OC_STACK_NO_RESOURCE.
 */
static OCStackResult findResourceAtRD(const OCResource* resource, const char *interfaceQuery,
    const char *resourceTypeQuery, OCDiscoveryPayload *discPayload)
{
#ifdef RD_SERVER
    if (strcmp(resource->uri, OC_RSRVD_RD_URI) == 0)
    {
        if (OC_STACK_OK == OCRDDatabaseCheckResources(interfaceQuery, resourceTypeQuery, discPayload))
        {
            return OC_STACK_OK;
        }
    }
#else
    OC_UNUSED(resource);
    OC_UNUSED(interfaceQuery);
    OC_UNUSED(resourceTypeQuery);
    OC_UNUSED(discPayload);
#endif
    return OC_STACK_NO_RESOURCE;
}

/**
 * Creates a discovery payload and add device id information. This information is included in all
 * /oic/res response.
 *
 * @param payload  payload that will have memory alllocated and device id information added.
 *
 * @return The memory allocated of the payload. If already allocated  it will return same payload. If
 * failed allocated memory then NULL.
 */
static OCDiscoveryPayload* discoveryPayloadCreateAndAddDeviceId(OCPayload *payload)
{
    if (payload)
    {
        OIC_LOG_V(ERROR, TAG, "Discovery payload is already allocated.");
        return ((OCDiscoveryPayload *)payload);
    }

    payload = (OCPayload *)OCDiscoveryPayloadCreate();
    VERIFY_PARAM_NON_NULL(TAG, payload, "Failed creating Discovery Payload.");

    {
        OCDiscoveryPayload *discPayload = (OCDiscoveryPayload *)payload;
        discPayload->sid = (char *)OICCalloc(1, UUID_STRING_SIZE);
        VERIFY_PARAM_NON_NULL(TAG, discPayload->sid, "Failed adding device id to discovery payload.");

        const char* uid = OCGetServerInstanceIDString();
        if (uid)
        {
            memcpy(discPayload->sid, uid, UUID_STRING_SIZE);
        }

        return discPayload;
    }
exit:
    OCPayloadDestroy(payload);
    return NULL;
}

/**
 * Add the common properties to the payload, they are only included in case of oic.if.baseline response.
 *
 * @param discPayload payload that will have the baseline information included.
 *
 * @return ::OC_STACK_OK if successful in adding all the information. ::OC_STACK_NO_MEMORY if failed
 * allocating the memory for the baseline information.
 */
static OCStackResult addDiscoveryBaselineCommonProperties(OCDiscoveryPayload *discPayload)
{
    discPayload->uri = OICStrdup(OC_RSRVD_WELL_KNOWN_URI);
    VERIFY_PARAM_NON_NULL(TAG, discPayload->uri, "Failed adding href to discovery payload.");

    if (savedDeviceInfo.deviceName)
    {
        discPayload->name = OICStrdup(savedDeviceInfo.deviceName);
        VERIFY_PARAM_NON_NULL(TAG, discPayload->name, "Failed adding name to discovery payload.");
    }

    discPayload->type = (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
    VERIFY_PARAM_NON_NULL(TAG, discPayload->type, "Failed adding rt to discovery payload.");
    discPayload->type->value = OICStrdup(OC_RSRVD_RESOURCE_TYPE_RES);
    VERIFY_PARAM_NON_NULL(TAG, discPayload->type, "Failed adding rt value to discovery payload.");

    OCResourcePayloadAddStringLL(&discPayload->iface, OC_RSRVD_INTERFACE_LL);
    OCResourcePayloadAddStringLL(&discPayload->iface, OC_RSRVD_INTERFACE_DEFAULT);
    VERIFY_PARAM_NON_NULL(TAG, discPayload->iface, "Failed adding if to discovery payload.");

    return OC_STACK_OK;

exit:
    return OC_STACK_NO_MEMORY;
}

static OCStackResult HandleVirtualResource (OCServerRequest *request, OCResource* resource)
{
    if (!request || !resource)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult discoveryResult = OC_STACK_ERROR;
    OCPayload* payload = NULL;
    char *interfaceQuery = NULL;
    char *resourceTypeQuery = NULL;
    char *dataModelVersions = NULL;

    OIC_LOG(INFO, TAG, "Entering HandleVirtualResource");

    OCVirtualResources virtualUriInRequest = GetTypeOfVirtualURI (request->resourceUrl);

    // Step 1: Generate the response to discovery request
    if (virtualUriInRequest == OC_WELL_KNOWN_URI
#ifdef MQ_BROKER
            || virtualUriInRequest == OC_MQ_BROKER_URI
#endif
            )
    {
        if (request->method == OC_REST_PUT || request->method == OC_REST_POST ||
            request->method == OC_REST_DELETE)
        {
            OIC_LOG_V(ERROR, TAG, "Resource : %s not permitted for method: %d",
                request->resourceUrl, request->method);
            discoveryResult = OC_STACK_UNAUTHORIZED_REQ;
            goto exit;
        }

        discoveryResult = getQueryParamsForFiltering (virtualUriInRequest, request->query,
                &interfaceQuery, &resourceTypeQuery);
        VERIFY_SUCCESS(discoveryResult, OC_STACK_OK);
        if (!interfaceQuery && !resourceTypeQuery)
        {
            // If no query is sent, default interface is used i.e. oic.if.ll.
            interfaceQuery = OICStrdup(OC_RSRVD_INTERFACE_LL);
        }

        bool baselineQuery = false;
        if (0 != strcmp(interfaceQuery, OC_RSRVD_INTERFACE_LL))
        {
            baselineQuery = true;
        }
        if (interfaceQuery)
        {
            OCDiscoveryPayload *discPayload = discoveryPayloadCreateAndAddDeviceId(payload);
            VERIFY_PARAM_NON_NULL(TAG, discPayload, "Failed creating discovery payload.");

            if (baselineQuery)
            {
                discoveryResult = addDiscoveryBaselineCommonProperties(discPayload);
                VERIFY_SUCCESS(discoveryResult, OC_STACK_OK);
            }
            OCResourceProperty prop = OC_DISCOVERABLE;
#ifdef MQ_BROKER
            prop = (OC_MQ_BROKER_URI == virtualUriInRequest) ? OC_MQ_BROKER : prop;
#endif
            for (; resource && discoveryResult == OC_STACK_OK; resource = resource->next)
            {
                discoveryResult = findResourceAtRD(resource, interfaceQuery, resourceTypeQuery,
                    discPayload);
                if (OC_STACK_NO_RESOURCE == discoveryResult)
                {
                    if ((!baselineQuery && (resource->resourceProperties & prop)) ||
                        (baselineQuery && (includeThisResourceInResponse(resource, interfaceQuery,
                                                                         resourceTypeQuery))))
                    {
                        discoveryResult = BuildVirtualResourceResponse(resource, discPayload,
                            &request->devAddr);
                    }
                }
            }
            if (discPayload->resources == NULL)
            {
                discoveryResult = OC_STACK_NO_RESOURCE;
            }
        }
    }
    else if (virtualUriInRequest == OC_DEVICE_URI)
    {
        if (request->method == OC_REST_PUT || request->method == OC_REST_POST ||
            request->method == OC_REST_DELETE)
        {
            OIC_LOG_V(ERROR, TAG, "Resource : %s not permitted for method: %d",
                request->resourceUrl, request->method);
            discoveryResult = OC_STACK_UNAUTHORIZED_REQ;
            goto exit;
        }

        const char* deviceId = OCGetServerInstanceIDString();
        VERIFY_PARAM_NON_NULL(TAG, deviceId, "Failed retrieving device id.");
        discoveryResult = OC_STACK_NO_MEMORY;
        dataModelVersions = OCCreateString(savedDeviceInfo.dataModelVersions);
        VERIFY_PARAM_NON_NULL(TAG, dataModelVersions, "Failed adding data model version.");
        payload = (OCPayload*) OCDevicePayloadCreate(deviceId, savedDeviceInfo.deviceName,
            savedDeviceInfo.types, savedDeviceInfo.specVersion, dataModelVersions);
        VERIFY_PARAM_NON_NULL(TAG, payload, "Failed adding device payload.");
    }
    else if (virtualUriInRequest == OC_PLATFORM_URI)
    {
        if (request->method == OC_REST_PUT || request->method == OC_REST_POST ||
            request->method == OC_REST_DELETE)
        {
            OIC_LOG_V(ERROR, TAG, "Resource : %s not permitted for method: %d",
                request->resourceUrl, request->method);
            discoveryResult = OC_STACK_UNAUTHORIZED_REQ;
            goto exit;
        }

        discoveryResult = OC_STACK_NO_MEMORY;
        payload = (OCPayload *)OCPlatformPayloadCreate(&savedPlatformInfo);
        VERIFY_PARAM_NON_NULL(TAG, payload, "Failed adding platform payload.");
   }
#ifdef ROUTING_GATEWAY
    else if (OC_GATEWAY_URI == virtualUriInRequest)
    {
        // Received request for a gateway
        OIC_LOG(INFO, TAG, "Request is for Gateway Virtual Request");
        discoveryResult = RMHandleGatewayRequest(request, resource);
    }
#endif
#ifdef TCP_ADAPTER
    else if (OC_KEEPALIVE_RESOURCE_URI == virtualUriInRequest)
    {
        // Received request for a keepalive
        OIC_LOG(INFO, TAG, "Request is for KeepAlive Request");
        discoveryResult = HandleKeepAliveRequest(request, resource);
    }
#endif
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

#ifdef WITH_PRESENCE
    if ((virtualUriInRequest == OC_PRESENCE) &&
        (resource->resourceProperties & OC_ACTIVE))
    {
        // Need to send ACK when the request is CON.
        if (request->qos == OC_HIGH_QOS)
        {
            CAEndpoint_t endpoint = { .adapter = CA_DEFAULT_ADAPTER };
            CopyDevAddrToEndpoint(&request->devAddr, &endpoint);
            SendDirectStackResponse(&endpoint, request->coapID, CA_EMPTY, CA_MSG_ACKNOWLEDGE,
                                    0, NULL, NULL, 0, NULL, CA_RESPONSE_FOR_RES);
        }
        FindAndDeleteServerRequest(request);

        // Presence uses observer notification api to respond via SendPresenceNotification.
        SendPresenceNotification(resource->rsrcType, OC_PRESENCE_TRIGGER_CHANGE);
    }
    else
#endif
#if ROUTING_GATEWAY
    // Gateway uses the RMHandleGatewayRequest to respond to the request.
    if (OC_GATEWAY_URI != virtualUriInRequest)
#endif
    {
#if TCP_ADAPTER
        // KeepAlive uses the HandleKeepAliveRequest to respond to the request.
        if (OC_KEEPALIVE_RESOURCE_URI != virtualUriInRequest)
#endif
        {
            if(discoveryResult == OC_STACK_OK)
            {
                SendNonPersistantDiscoveryResponse(request, resource, payload, OC_EH_OK);
            }
            else if(((request->devAddr.flags &  OC_MULTICAST) == false) &&
                (request->devAddr.adapter != OC_ADAPTER_RFCOMM_BTEDR) &&
                (request->devAddr.adapter != OC_ADAPTER_GATT_BTLE))
            {
                OIC_LOG_V(ERROR, TAG, "Sending a (%d) error to (%d) discovery request",
                    discoveryResult, virtualUriInRequest);
                SendNonPersistantDiscoveryResponse(request, resource, NULL,
                    (discoveryResult == OC_STACK_NO_RESOURCE) ?
                            OC_EH_RESOURCE_NOT_FOUND : OC_EH_ERROR);
            }
            else
            {
                // Ignoring the discovery request as per RFC 7252, Section #8.2
                OIC_LOG(INFO, TAG, "Silently ignoring the request since no useful data to send.");
                // the request should be removed.
                // since it never remove and causes a big memory waste.
                FindAndDeleteServerRequest(request);
            }
        }
    }

exit:
    if (interfaceQuery)
    {
        OICFree(interfaceQuery);
    }

    if (resourceTypeQuery)
    {
        OICFree(resourceTypeQuery);
    }
    OCPayloadDestroy(payload);
    if (dataModelVersions)
    {
        OICFree(dataModelVersions);
    }
    return discoveryResult;
}

static OCStackResult
HandleDefaultDeviceEntityHandler (OCServerRequest *request)
{
    if(!request)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult result = OC_STACK_OK;
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OCEntityHandlerRequest ehRequest = {0};

    OIC_LOG(INFO, TAG, "Entering HandleResourceWithDefaultDeviceEntityHandler");
    result = FormOCEntityHandlerRequest(&ehRequest,
                                        (OCRequestHandle) request,
                                        request->method,
                                        &request->devAddr,
                                        (OCResourceHandle) NULL, request->query,
                                        PAYLOAD_TYPE_REPRESENTATION,
                                        request->payload,
                                        request->payloadSize,
                                        request->numRcvdVendorSpecificHeaderOptions,
                                        request->rcvdVendorSpecificHeaderOptions,
                                        (OCObserveAction)request->observationOption,
                                        (OCObservationId)0,
                                        request->coapID);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    // At this point we know for sure that defaultDeviceHandler exists
    ehResult = defaultDeviceHandler(OC_REQUEST_FLAG, &ehRequest,
                                  (char*) request->resourceUrl, defaultDeviceHandlerCallbackParameter);
    if(ehResult == OC_EH_SLOW)
    {
        OIC_LOG(INFO, TAG, "This is a slow resource");
        request->slowFlag = 1;
    }
    else if(ehResult == OC_EH_ERROR)
    {
        FindAndDeleteServerRequest(request);
    }
    result = EntityHandlerCodeToOCStackCode(ehResult);
exit:
    OCPayloadDestroy(ehRequest.payload);
    return result;
}

static OCStackResult
HandleResourceWithEntityHandler (OCServerRequest *request,
                                 OCResource *resource,
                                 uint8_t collectionResource)
{
    OC_UNUSED(collectionResource);

    if(!request || ! resource)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult result = OC_STACK_ERROR;
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OCEntityHandlerFlag ehFlag = OC_REQUEST_FLAG;
    ResourceObserver *resObs = NULL;

    OCEntityHandlerRequest ehRequest = {0};

    OIC_LOG(INFO, TAG, "Entering HandleResourceWithEntityHandler");
    OCPayloadType type = PAYLOAD_TYPE_REPRESENTATION;
    // check the security resource
    if (request && request->resourceUrl && SRMIsSecurityResourceURI(request->resourceUrl))
    {
        type = PAYLOAD_TYPE_SECURITY;

    }

    result = FormOCEntityHandlerRequest(&ehRequest,
                                        (OCRequestHandle)request,
                                        request->method,
                                        &request->devAddr,
                                        (OCResourceHandle)resource,
                                        request->query,
                                        type,
                                        request->payload,
                                        request->payloadSize,
                                        request->numRcvdVendorSpecificHeaderOptions,
                                        request->rcvdVendorSpecificHeaderOptions,
                                        (OCObserveAction)request->observationOption,
                                        0,
                                        request->coapID);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    if(ehRequest.obsInfo.action == OC_OBSERVE_NO_OPTION)
    {
        OIC_LOG(INFO, TAG, "No observation requested");
        ehFlag = OC_REQUEST_FLAG;
    }
    else if(ehRequest.obsInfo.action == OC_OBSERVE_REGISTER)
    {
        OIC_LOG(INFO, TAG, "Observation registration requested");

        ResourceObserver *obs = GetObserverUsingToken (request->requestToken,
                                    request->tokenLength);

        if (obs)
        {
            OIC_LOG (INFO, TAG, "Observer with this token already present");
            OIC_LOG (INFO, TAG, "Possibly re-transmitted CON OBS request");
            OIC_LOG (INFO, TAG, "Not adding observer. Not responding to client");
            OIC_LOG (INFO, TAG, "The first request for this token is already ACKED.");

            // server requests are usually free'd when the response is sent out
            // for the request in ocserverrequest.c : HandleSingleResponse()
            // Since we are making an early return and not responding, the server request
            // needs to be deleted.
            FindAndDeleteServerRequest (request);
            return OC_STACK_OK;
        }

        result = GenerateObserverId(&ehRequest.obsInfo.obsId);
        VERIFY_SUCCESS(result, OC_STACK_OK);

        result = AddObserver ((const char*)(request->resourceUrl),
                (const char *)(request->query),
                ehRequest.obsInfo.obsId, request->requestToken, request->tokenLength,
                resource, request->qos, request->acceptFormat,
                &request->devAddr);

        if(result == OC_STACK_OK)
        {
            OIC_LOG(INFO, TAG, "Added observer successfully");
            request->observeResult = OC_STACK_OK;
            ehFlag = (OCEntityHandlerFlag)(OC_REQUEST_FLAG | OC_OBSERVE_FLAG);
        }
        else if (result == OC_STACK_RESOURCE_ERROR)
        {
            OIC_LOG(INFO, TAG, "The Resource is not active, discoverable or observable");
            request->observeResult = OC_STACK_ERROR;
            ehFlag = OC_REQUEST_FLAG;
        }
        else
        {
            // The error in observeResult for the request will be used when responding to this
            // request by omitting the observation option/sequence number.
            request->observeResult = OC_STACK_ERROR;
            OIC_LOG(ERROR, TAG, "Observer Addition failed");
            ehFlag = OC_REQUEST_FLAG;
            FindAndDeleteServerRequest(request);
            goto exit;
        }

    }
    else if(ehRequest.obsInfo.action == OC_OBSERVE_DEREGISTER)
    {
        OIC_LOG(INFO, TAG, "Deregistering observation requested");

        resObs = GetObserverUsingToken (request->requestToken, request->tokenLength);

        if (NULL == resObs)
        {
            // Stack does not contain this observation request
            // Either token is incorrect or observation list is corrupted
            result = OC_STACK_ERROR;
            goto exit;
        }
        ehRequest.obsInfo.obsId = resObs->observeId;
        ehFlag = (OCEntityHandlerFlag)(ehFlag | OC_OBSERVE_FLAG);

        result = DeleteObserverUsingToken (request->requestToken, request->tokenLength);

        if(result == OC_STACK_OK)
        {
            OIC_LOG(INFO, TAG, "Removed observer successfully");
            request->observeResult = OC_STACK_OK;
            // There should be no observe option header for de-registration response.
            // Set as an invalid value here so we can detect it later and remove the field in response.
            request->observationOption = MAX_SEQUENCE_NUMBER + 1;
        }
        else
        {
            request->observeResult = OC_STACK_ERROR;
            OIC_LOG(ERROR, TAG, "Observer Removal failed");
            FindAndDeleteServerRequest(request);
            goto exit;
        }
    }
    else
    {
        result = OC_STACK_ERROR;
        goto exit;
    }

    ehResult = resource->entityHandler(ehFlag, &ehRequest, resource->entityHandlerCallbackParam);
    if(ehResult == OC_EH_SLOW)
    {
        OIC_LOG(INFO, TAG, "This is a slow resource");
        request->slowFlag = 1;
    }
    else if(ehResult == OC_EH_ERROR)
    {
        FindAndDeleteServerRequest(request);
    }
    result = EntityHandlerCodeToOCStackCode(ehResult);
exit:
    OCPayloadDestroy(ehRequest.payload);
    return result;
}

static OCStackResult
HandleCollectionResourceDefaultEntityHandler (OCServerRequest *request,
                                              OCResource *resource)
{
    if(!request || !resource)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult result = OC_STACK_ERROR;
    OCEntityHandlerRequest ehRequest = {0};

    result = FormOCEntityHandlerRequest(&ehRequest,
                                        (OCRequestHandle)request,
                                        request->method,
                                        &request->devAddr,
                                        (OCResourceHandle)resource,
                                        request->query,
                                        PAYLOAD_TYPE_REPRESENTATION,
                                        request->payload,
                                        request->payloadSize,
                                        request->numRcvdVendorSpecificHeaderOptions,
                                        request->rcvdVendorSpecificHeaderOptions,
                                        (OCObserveAction)request->observationOption,
                                        (OCObservationId)0,
                                        request->coapID);
    if(result == OC_STACK_OK)
    {
        result = DefaultCollectionEntityHandler (OC_REQUEST_FLAG, &ehRequest);
    }

    OCPayloadDestroy(ehRequest.payload);
    return result;
}

OCStackResult
ProcessRequest(ResourceHandling resHandling, OCResource *resource, OCServerRequest *request)
{
    OCStackResult ret = OC_STACK_OK;

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
            OIC_LOG(INFO, TAG, "OC_RESOURCE_NOT_COLLECTION_DEFAULT_ENTITYHANDLER");
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
        case OC_RESOURCE_NOT_SPECIFIED:
        {
            ret = OC_STACK_NO_RESOURCE;
            break;
        }
        default:
        {
            OIC_LOG(INFO, TAG, "Invalid Resource Determination");
            return OC_STACK_ERROR;
        }
    }
    return ret;
}

void DeletePlatformInfo()
{
    OIC_LOG(INFO, TAG, "Deleting platform info.");

    OICFree(savedPlatformInfo.platformID);
    savedPlatformInfo.platformID = NULL;

    OICFree(savedPlatformInfo.manufacturerName);
    savedPlatformInfo.manufacturerName = NULL;

    OICFree(savedPlatformInfo.manufacturerUrl);
    savedPlatformInfo.manufacturerUrl = NULL;

    OICFree(savedPlatformInfo.modelNumber);
    savedPlatformInfo.modelNumber = NULL;

    OICFree(savedPlatformInfo.dateOfManufacture);
    savedPlatformInfo.dateOfManufacture = NULL;

    OICFree(savedPlatformInfo.platformVersion);
    savedPlatformInfo.platformVersion = NULL;

    OICFree(savedPlatformInfo.operatingSystemVersion);
    savedPlatformInfo.operatingSystemVersion = NULL;

    OICFree(savedPlatformInfo.hardwareVersion);
    savedPlatformInfo.hardwareVersion = NULL;

    OICFree(savedPlatformInfo.firmwareVersion);
    savedPlatformInfo.firmwareVersion = NULL;

    OICFree(savedPlatformInfo.supportUrl);
    savedPlatformInfo.supportUrl = NULL;

    OICFree(savedPlatformInfo.systemTime);
    savedPlatformInfo.systemTime = NULL;
}

static OCStackResult DeepCopyPlatFormInfo(OCPlatformInfo info)
{
    savedPlatformInfo.platformID = OICStrdup(info.platformID);
    savedPlatformInfo.manufacturerName = OICStrdup(info.manufacturerName);
    savedPlatformInfo.manufacturerUrl = OICStrdup(info.manufacturerUrl);
    savedPlatformInfo.modelNumber = OICStrdup(info.modelNumber);
    savedPlatformInfo.dateOfManufacture = OICStrdup(info.dateOfManufacture);
    savedPlatformInfo.platformVersion = OICStrdup(info.platformVersion);
    savedPlatformInfo.operatingSystemVersion = OICStrdup(info.operatingSystemVersion);
    savedPlatformInfo.hardwareVersion = OICStrdup(info.hardwareVersion);
    savedPlatformInfo.firmwareVersion = OICStrdup(info.firmwareVersion);
    savedPlatformInfo.supportUrl = OICStrdup(info.supportUrl);
    savedPlatformInfo.systemTime = OICStrdup(info.systemTime);

    if ((!savedPlatformInfo.platformID && info.platformID)||
        (!savedPlatformInfo.manufacturerName && info.manufacturerName)||
        (!savedPlatformInfo.manufacturerUrl && info.manufacturerUrl)||
        (!savedPlatformInfo.modelNumber && info.modelNumber)||
        (!savedPlatformInfo.dateOfManufacture && info.dateOfManufacture)||
        (!savedPlatformInfo.platformVersion && info.platformVersion)||
        (!savedPlatformInfo.operatingSystemVersion && info.operatingSystemVersion)||
        (!savedPlatformInfo.hardwareVersion && info.hardwareVersion)||
        (!savedPlatformInfo.firmwareVersion && info.firmwareVersion)||
        (!savedPlatformInfo.supportUrl && info.supportUrl)||
        (!savedPlatformInfo.systemTime && info.systemTime))
    {
        DeletePlatformInfo();
        return OC_STACK_INVALID_PARAM;
    }

    return OC_STACK_OK;

}

OCStackResult SavePlatformInfo(OCPlatformInfo info)
{
    DeletePlatformInfo();

    OCStackResult res = DeepCopyPlatFormInfo(info);

    if (res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to save platform info. errno(%d)", res);
    }
    else
    {
        OIC_LOG(INFO, TAG, "Platform info saved.");
    }

    return res;
}

void DeleteDeviceInfo()
{
    OIC_LOG(INFO, TAG, "Deleting device info.");

    OICFree(savedDeviceInfo.deviceName);
    OCFreeOCStringLL(savedDeviceInfo.types);
    OICFree(savedDeviceInfo.specVersion);
    OCFreeOCStringLL(savedDeviceInfo.dataModelVersions);
    savedDeviceInfo.deviceName = NULL;
    savedDeviceInfo.specVersion = NULL;
    savedDeviceInfo.dataModelVersions = NULL;
}

static OCStackResult DeepCopyDeviceInfo(OCDeviceInfo info)
{
    savedDeviceInfo.deviceName = OICStrdup(info.deviceName);

    if(!savedDeviceInfo.deviceName && info.deviceName)
    {
        DeleteDeviceInfo();
        return OC_STACK_NO_MEMORY;
    }

    if (info.types)
    {
        savedDeviceInfo.types = CloneOCStringLL(info.types);
        OCStringLL *type = info.types;
        bool found = false;
        while (type)
        {
            if (type && type->value && 0 == strcmp(type->value, OC_RSRVD_RESOURCE_TYPE_DEVICE))
            {
                found = true;
            }
            type = type->next;
        }
        if (!found)
        {
            // Append the oic.wk.d at the start of rt link parameter value.
            OCStringLL *dest = (OCStringLL*)OICCalloc (1, sizeof (OCStringLL));
            if (!dest)
            {
                DeleteDeviceInfo();
                return OC_STACK_NO_MEMORY;
            }
            dest->value = OICStrdup (OC_RSRVD_RESOURCE_TYPE_DEVICE);
            if (!dest->value)
            {
                DeleteDeviceInfo();
                return OC_STACK_NO_MEMORY;
            }
            dest->next = savedDeviceInfo.types;
            savedDeviceInfo.types = dest;
        }
        if(!savedDeviceInfo.types && info.types)
        {
            DeleteDeviceInfo();
            return OC_STACK_NO_MEMORY;
        }
    }

    if (info.specVersion)
    {
        savedDeviceInfo.specVersion = OICStrdup(info.specVersion);
        if(!savedDeviceInfo.specVersion && info.specVersion)
        {
            DeleteDeviceInfo();
            return OC_STACK_NO_MEMORY;
        }
    }
    else
    {
        savedDeviceInfo.specVersion = OICStrdup(OC_SPEC_VERSION);
        if(!savedDeviceInfo.specVersion && OC_SPEC_VERSION)
        {
            DeleteDeviceInfo();
            return OC_STACK_NO_MEMORY;
        }
    }

    if (info.dataModelVersions)
    {
        savedDeviceInfo.dataModelVersions = CloneOCStringLL(info.dataModelVersions);
        if(!savedDeviceInfo.dataModelVersions && info.dataModelVersions)
        {
            DeleteDeviceInfo();
            return OC_STACK_NO_MEMORY;
        }
    }
    else
    {
        savedDeviceInfo.dataModelVersions = (OCStringLL *)OICCalloc(1,sizeof(OCStringLL));
        if (!savedDeviceInfo.dataModelVersions)
        {
            return OC_STACK_NO_MEMORY;
        }
        savedDeviceInfo.dataModelVersions->value = OICStrdup(OC_DATA_MODEL_VERSION);
        if(!savedDeviceInfo.dataModelVersions->value && OC_DATA_MODEL_VERSION)
        {
            DeleteDeviceInfo();
            return OC_STACK_NO_MEMORY;
        }
    }

    return OC_STACK_OK;
}

OCStackResult SaveDeviceInfo(OCDeviceInfo info)
{
    OCStackResult res = OC_STACK_OK;

    DeleteDeviceInfo();

    res = DeepCopyDeviceInfo(info);

    VERIFY_SUCCESS(res, OC_STACK_OK);

    if (OCGetServerInstanceIDString() == NULL)
    {
        OIC_LOG(INFO, TAG, "Device ID generation failed");
        res =  OC_STACK_ERROR;
        goto exit;
    }

    OIC_LOG(INFO, TAG, "Device initialized successfully.");
    return OC_STACK_OK;

exit:
    DeleteDeviceInfo();
    return res;
}
