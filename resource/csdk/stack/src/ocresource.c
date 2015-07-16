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
#include "ocresource.h"
#include <string.h>
#include "ocresourcehandler.h"
#include "ocobserve.h"
#include "occollection.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "logger.h"
#include "cJSON.h"
#include "ocpayload.h"

#include "cacommon.h"
#include "cainterface.h"


/// Module Name
#define TAG PCF("ocresource")
#define VERIFY_SUCCESS(op, successCode) { if (op != successCode) \
            {OC_LOG_V(FATAL, TAG, "%s failed!!", #op); goto exit;} }

#define VERIFY_NON_NULL(arg, logLevel, retVal) { if (!(arg)) { OC_LOG((logLevel), \
             TAG, PCF(#arg " is NULL")); return (retVal); } }

extern OCResource *headResource;
static OCPlatformInfo savedPlatformInfo = {};
static OCDeviceInfo savedDeviceInfo = {};

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
static OCStackResult GetSecurePortInfo(CATransportAdapter_t connType, uint16_t *port)
{
    CAEndpoint_t* info = NULL;
    uint32_t size = 0;
    OCStackResult ret = OC_STACK_ERROR;

    CAResult_t caResult = CAGetNetworkInformation(&info, &size);
    if ((caResult == CA_STATUS_OK) && info && size)
    {
        while (size--)
        {
            if ((info[size].flags & CA_SECURE) && info[size].adapter == connType)
            {
                if (info[size].adapter == CA_ADAPTER_IP)
                {
                    *port = info[size].port;
                    ret = OC_STACK_OK;
                    break;
                }
            }
        }
    }

    OICFree(info);
    return ret;
}

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
    char *restOfQuery = NULL;
    int numKeyValuePairsParsed = 0;

    *filterOne = NULL;
    *filterTwo = NULL;

    OC_LOG_V(INFO, TAG, "Extracting params from %s", query);

    char *keyValuePair = strtok_r (query, OC_QUERY_SEPARATOR, &restOfQuery);

    while(keyValuePair)
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
        else if (strcmp (key, OC_RSRVD_INTERFACE) == 0)
        {
            *filterOne = value;     // if
        }
        else if (strcmp (key, OC_RSRVD_RESOURCE_TYPE) == 0)
        {
            *filterTwo = value;     // rt
        }
        else
        {
            OC_LOG_V(ERROR, TAG, "Unsupported query key: %s", key);
            return OC_STACK_INVALID_QUERY;
        }
        ++numKeyValuePairsParsed;

        keyValuePair = strtok_r(NULL, OC_QUERY_SEPARATOR, &restOfQuery);
    }

    OC_LOG_V(INFO, TAG, "Extracted params %s and %s.", *filterOne, *filterTwo);
    return OC_STACK_OK;
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
#ifdef WITH_PRESENCE
    else if (strcmp(uriInRequest, OC_RSRVD_PRESENCE_URI) == 0)
    {
        return OC_PRESENCE;
    }
#endif //WITH_PRESENCE
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
        OC_LOG(INFO, TAG, PCF("OC_PRESENCE Request for virtual resource."));
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

OCStackResult BuildVirtualResourceResponse(const OCResource *resourcePtr,
                        OCDiscoveryPayload* payload, CATransportAdapter_t adapter )
{
    if (!resourcePtr || !payload)
    {
        return OC_STACK_INVALID_PARAM;
    }
    uint16_t port = 0;
    if (resourcePtr->resourceProperties & OC_SECURE)
    {
       if(GetSecurePortInfo (adapter, &port) != OC_STACK_OK)
       {
           port = 0;
       }
    }

    OCDiscoveryPayloadAddResource(payload, resourcePtr, port);
    return OC_STACK_OK;
}


uint8_t IsCollectionResource (OCResource *resource)
{
    if(!resource)
    {
        return 0;
    }

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
    OC_LOG_V(INFO, TAG, "Resource %s not found", resourceUri);
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

    OC_LOG_V(INFO, TAG, "DetermineResourceHandling for %s", request->resourceUrl);

    const OCDevAddr *devAddr = &request->devAddr;

    // Check if virtual resource
    if (GetTypeOfVirtualURI(request->resourceUrl) != OC_UNKNOWN_URI)
    {
        OC_LOG_V (INFO, TAG, "%s is virtual", request->resourceUrl);
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
            *handling = OC_RESOURCE_NOT_SPECIFIED;
            return OC_STACK_NO_RESOURCE;
        }

        // secure resource will entertain only authorized requests
        if ((resourcePtr->resourceProperties & OC_SECURE) && ((devAddr->flags & OC_FLAG_SECURE) == 0))
        {
            OC_LOG(ERROR, TAG, PCF("Un-authorized request. Ignoring"));
            return OC_STACK_RESOURCE_ERROR;
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
            result = OC_STACK_OK;
            break;
        case OC_EH_SLOW:
            result = OC_STACK_SLOW_RESOURCE;
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
            result = OC_STACK_RESOURCE_DELETED;
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

    OC_LOG_V(INFO, TAG, PCF("%s does not contain rt=%s."), resource->uri, resourceTypeFilter);
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
        if (strcmp (interfacePtr->name, interfaceFilter) == 0)
        {
            return true;
        }
        interfacePtr = interfacePtr->next;
    }

    OC_LOG_V(INFO, TAG, PCF("%s does not contain if=%s."), resource->uri, interfaceFilter);
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
        OC_LOG(ERROR, TAG, PCF("Invalid resource"));
        return false;
    }

    if ( !(resource->resourceProperties & OC_ACTIVE) ||
         !(resource->resourceProperties & OC_DISCOVERABLE))
    {
        OC_LOG_V(INFO, TAG, PCF("%s not ACTIVE or DISCOVERABLE"), resource->uri);
        return false;
    }

    return resourceMatchesIFFilter(resource, interfaceFilter) &&
           resourceMatchesRTFilter(resource, resourceTypeFilter);

}

OCStackResult SendNonPersistantDiscoveryResponse(OCServerRequest *request, OCResource *resource,
                                OCPayload *discoveryPayload)
{
    OCEntityHandlerResponse response = {};

    response.ehResult = OC_EH_OK;
    response.payload = discoveryPayload;
    response.persistentBufferFlag = 0;
    response.requestHandle = (OCRequestHandle) request;
    response.resourceHandle = (OCResourceHandle) resource;

    return OCDoResponse(&response);
}

static OCStackResult HandleVirtualResource (OCServerRequest *request, OCResource* resource)
{
    if (!request || !resource)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult discoveryResult = OC_STACK_ERROR;
    OCPayload* payload = NULL;
    char *filterOne = NULL;
    char *filterTwo = NULL;

    OC_LOG(INFO, TAG, PCF("Entering HandleVirtualResource"));

    OCVirtualResources virtualUriInRequest = GetTypeOfVirtualURI (request->resourceUrl);


    if (virtualUriInRequest == OC_WELL_KNOWN_URI)
    {
        discoveryResult = getQueryParamsForFiltering (virtualUriInRequest, request->query,
                                                            &filterOne, &filterTwo);
        if (discoveryResult != OC_STACK_OK)
        {
            OC_LOG_V(ERROR, TAG, "Error (%d) validating query.\n", discoveryResult);
            return discoveryResult;
        }
        payload = (OCPayload*)OCDiscoveryPayloadCreate();

        if(!payload)
        {
            return OC_STACK_NO_MEMORY;
        }


        for(;resource && discoveryResult == OC_STACK_OK; resource = resource->next)
        {
            if(includeThisResourceInResponse(resource, filterOne, filterTwo))
            {
                discoveryResult = BuildVirtualResourceResponse(resource,
                    (OCDiscoveryPayload*)payload,
                    (CATransportAdapter_t)request->devAddr.adapter);
            }
        }
    }
    else if (virtualUriInRequest == OC_DEVICE_URI)
    {
            payload = (OCPayload*)OCDevicePayloadCreate(OC_RSRVD_DEVICE_URI,
                        OCGetServerInstanceID(), savedDeviceInfo.deviceName,
                        OC_SPEC_VERSION, OC_DATA_MODEL_VERSION);
            if (!payload)
            {
                discoveryResult = OC_STACK_NO_MEMORY;
            }
    }
    else if (virtualUriInRequest == OC_PLATFORM_URI)
    {
            OCPlatformPayload* payload = OCPlatformPayloadCreate(
                    OC_RSRVD_PLATFORM_URI,
                    &savedPlatformInfo);
            if (!payload)
            {
                discoveryResult = OC_STACK_NO_MEMORY;
            }
    }

    #ifdef WITH_PRESENCE
    else
    {
        if(resource->resourceProperties & OC_ACTIVE)
        {
            discoveryResult = SendPresenceNotification(resource->rsrcType,
                                                OC_PRESENCE_TRIGGER_CHANGE);
        }
    }
    #endif

    // Presence uses observer notification api to respond via SendPresenceNotification.
    if (virtualUriInRequest != OC_PRESENCE)
    {
        if(discoveryResult == OC_STACK_OK)
        {
            discoveryResult = SendNonPersistantDiscoveryResponse(request, resource,
                                                        payload);
            OCPayloadDestroy(payload);
        }
        else
        {
            OC_LOG_V(ERROR, TAG, "Error (%d) building (%d)  discovery response. "\
                        "Not responding to request.", discoveryResult, virtualUriInRequest);
        }
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
    OCEntityHandlerRequest ehRequest = {};

    OC_LOG(INFO, TAG, PCF("Entering HandleResourceWithDefaultDeviceEntityHandler"));
    result = FormOCEntityHandlerRequest(&ehRequest, (OCRequestHandle) request,
            request->method, (OCResourceHandle) NULL, request->query,
            request->payload, request->payloadSize,
            request->numRcvdVendorSpecificHeaderOptions,
            request->rcvdVendorSpecificHeaderOptions,
            (OCObserveAction)request->observationOption, (OCObservationId)0);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    // At this point we know for sure that defaultDeviceHandler exists
    ehResult = defaultDeviceHandler(OC_REQUEST_FLAG, &ehRequest,
                                  (char*) request->resourceUrl, defaultDeviceHandlerCallbackParameter);
    if(ehResult == OC_EH_SLOW)
    {
        OC_LOG(INFO, TAG, PCF("This is a slow resource"));
        request->slowFlag = 1;
    }
    else if(ehResult == OC_EH_ERROR)
    {
        FindAndDeleteServerRequest(request);
    }
    result = EntityHandlerCodeToOCStackCode(ehResult);
exit:
    return result;
}

static OCStackResult
HandleResourceWithEntityHandler (OCServerRequest *request,
                                 OCResource *resource,
                                 uint8_t collectionResource)
{
    if(!request || ! resource)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult result = OC_STACK_ERROR;
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OCEntityHandlerFlag ehFlag = OC_REQUEST_FLAG;
    ResourceObserver *resObs = NULL;

    OCEntityHandlerRequest ehRequest = {};

    OC_LOG(INFO, TAG, PCF("Entering HandleResourceWithEntityHandler"));

    result = FormOCEntityHandlerRequest(&ehRequest, (OCRequestHandle) request,
            request->method, (OCResourceHandle) resource, request->query,
            request->payload, request->payloadSize, request->numRcvdVendorSpecificHeaderOptions,
            request->rcvdVendorSpecificHeaderOptions,
            (OCObserveAction)request->observationOption, 0);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    if(ehRequest.obsInfo.action == OC_OBSERVE_NO_OPTION)
    {
        OC_LOG(INFO, TAG, PCF("No observation requested"));
        ehFlag = OC_REQUEST_FLAG;
    }
    else if(ehRequest.obsInfo.action == OC_OBSERVE_REGISTER && !collectionResource)
    {
        OC_LOG(INFO, TAG, PCF("Observation registration requested"));

        result = GenerateObserverId(&ehRequest.obsInfo.obsId);
        VERIFY_SUCCESS(result, OC_STACK_OK);

        result = AddObserver ((const char*)(request->resourceUrl),
                (const char *)(request->query),
                ehRequest.obsInfo.obsId, request->requestToken, request->tokenLength,
                resource, request->qos,
                &request->devAddr);

        if(result == OC_STACK_OK)
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
    else if(ehRequest.obsInfo.action == OC_OBSERVE_DEREGISTER &&
            !collectionResource)
    {
        OC_LOG(INFO, TAG, PCF("Deregistering observation requested"));

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
            OC_LOG(INFO, TAG, PCF("Removed observer successfully"));
            request->observeResult = OC_STACK_OK;
        }
        else
        {
            result = OC_STACK_OK;
            request->observeResult = OC_STACK_ERROR;
            OC_LOG(ERROR, TAG, PCF("Observer Removal failed"));
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
        OC_LOG(INFO, TAG, PCF("This is a slow resource"));
        request->slowFlag = 1;
    }
    else if(ehResult == OC_EH_ERROR)
    {
        FindAndDeleteServerRequest(request);
    }
    result = EntityHandlerCodeToOCStackCode(ehResult);
exit:
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
    OCEntityHandlerRequest ehRequest = {};

    result = FormOCEntityHandlerRequest(&ehRequest, (OCRequestHandle) request,
            request->method, (OCResourceHandle) resource, request->query,
            request->payload, request->payloadSize, request->numRcvdVendorSpecificHeaderOptions,
            request->rcvdVendorSpecificHeaderOptions,
            (OCObserveAction)request->observationOption, (OCObservationId) 0);
    if(result != OC_STACK_OK)
    {
        return result;
    }

    return (DefaultCollectionEntityHandler (OC_REQUEST_FLAG, &ehRequest));
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
        case OC_RESOURCE_NOT_SPECIFIED:
        {
            ret = OC_STACK_NO_RESOURCE;
            break;
        }
        default:
        {
            OC_LOG(INFO, TAG, PCF("Invalid Resource Determination"));
            return OC_STACK_ERROR;
        }
    }
    return ret;
}

void DeletePlatformInfo()
{
    OC_LOG(INFO, TAG, PCF("Deleting platform info."));

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
        OC_LOG_V(ERROR, TAG, PCF("Failed to save platform info. errno(%d)"), res);
    }
    else
    {
        OC_LOG(ERROR, TAG, PCF("Platform info saved."));
    }

    return res;
}

void DeleteDeviceInfo()
{
    OC_LOG(INFO, TAG, PCF("Deleting device info."));

    OICFree(savedDeviceInfo.deviceName);
    savedDeviceInfo.deviceName = NULL;
}

static OCStackResult DeepCopyDeviceInfo(OCDeviceInfo info)
{
    savedDeviceInfo.deviceName = OICStrdup(info.deviceName);

    if(!savedDeviceInfo.deviceName && info.deviceName)
    {
        DeleteDeviceInfo();
        return OC_STACK_NO_MEMORY;
    }

    return OC_STACK_OK;
}

OCStackResult SaveDeviceInfo(OCDeviceInfo info)
{
    OCStackResult res = OC_STACK_OK;

    DeleteDeviceInfo();

    res = DeepCopyDeviceInfo(info);

    VERIFY_SUCCESS(res, OC_STACK_OK);

    if(OCGetServerInstanceID() == NULL)
    {
        OC_LOG(INFO, TAG, PCF("Device ID generation failed"));
        res =  OC_STACK_ERROR;
        goto exit;
    }

    OC_LOG(INFO, TAG, PCF("Device initialized successfully."));
    return OC_STACK_OK;

    exit:
        DeleteDeviceInfo();
        return res;

}
