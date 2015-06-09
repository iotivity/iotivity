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

static const char * VIRTUAL_RSRCS[] =
{
    "/oic/res",
    "/oic/d",
    "/oic/p",
    "/oic/res/types/d",
    #ifdef WITH_PRESENCE
    "/oic/ad"
    #endif
};

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

static OCStackResult ValidateUrlQuery (char *url, char *query,
                                uint8_t *filterOn, char **filterValue)
{
    if(!filterOn || !filterValue)
    {
        return OC_STACK_INVALID_PARAM;
    }

    char *filterParam = NULL;

    OC_LOG(INFO, TAG, PCF("Entering ValidateUrlQuery"));
    if (!url)
    {
        return OC_STACK_INVALID_URI;
    }

    if (strcmp ((char *)url, GetVirtualResourceUri(OC_WELL_KNOWN_URI)) == 0 ||
                strcmp ((char *)url, GetVirtualResourceUri(OC_DEVICE_URI)) == 0 ||
                strcmp((char *)url, GetVirtualResourceUri(OC_PLATFORM_URI)) == 0)
    {
        *filterOn = STACK_RES_DISCOVERY_NOFILTER;
        if (query && *query)
        {
            char* strTokPtr = NULL;
            filterParam = strtok_r((char *)query, "=", &strTokPtr);
            *filterValue = strtok_r(NULL, " ", &strTokPtr);

            if (!(*filterValue) || ! filterParam)
            {
                return OC_STACK_INVALID_QUERY;
            }
            else if (strcmp (filterParam, OC_RSRVD_INTERFACE) == 0)
            {
                // Resource discovery with interface filter
                *filterOn = STACK_RES_DISCOVERY_IF_FILTER;
            }
            else if (strcmp (filterParam, OC_RSRVD_RESOURCE_TYPE) == 0)
            {
                // Resource discovery with resource type filter
                *filterOn = STACK_RES_DISCOVERY_RT_FILTER;
            }
            else if (strcmp (filterParam, OC_RSRVD_DEVICE_ID) == 0)
            {
                //Device ID filter
                *filterOn = STACK_DEVICE_DISCOVERY_DI_FILTER;
            }
            else if (strcmp (filterParam, OC_RSRVD_DEVICE_NAME) == 0)
            {
                //Device Name filter
                *filterOn = STACK_DEVICE_DISCOVERY_DN_FILTER;
            }
            else
            {
                // Other filter types not supported
                return OC_STACK_INVALID_QUERY;
            }
        }
    }
    #ifdef WITH_PRESENCE
    else if (strcmp((char *)url, GetVirtualResourceUri(OC_PRESENCE)) == 0)
    {
        //Nothing needs to be done, except for pass a OC_PRESENCE query through as OC_STACK_OK.
        OC_LOG(INFO, TAG, PCF("OC_PRESENCE Request"));
        *filterOn = STACK_RES_DISCOVERY_NOFILTER;
    }
    #endif
    else
    {
        // Other URIs not yet supported
        return OC_STACK_INVALID_URI;
    }
    OC_LOG(INFO, TAG, PCF("Exiting ValidateUrlQuery"));
    return OC_STACK_OK;
}

OCStackResult
BuildVirtualResourceResponse(const OCResource *resourcePtr, uint8_t filterOn,
                       const char *filterValue, OCDiscoveryPayload* payload,
                       CATransportAdapter_t adapter)
{
    if (!resourcePtr || !payload)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OC_LOG(INFO, TAG, PCF("Entering BuildVirtualResourceResponse"));

    // TODO: Rebase should get rid of this code
    uint16_t port = 0;
    if (resourcePtr->resourceProperties & OC_SECURE &&
            GetSecurePortInfo (adapter, &port) == OC_STACK_OK)
    {
    }
    else
    {
        port = 0;
    }

    if ((filterOn == STACK_RES_DISCOVERY_RT_FILTER) && filterValue)
    {
        OCResourceType* resourceTypePtr = resourcePtr->rsrcType;
        while (resourceTypePtr)
        {
            if (strcmp (resourceTypePtr->resourcetypename, filterValue) == 0)
            {
                OCDiscoveryPayloadAddResource(payload, resourcePtr, port);
                break;
            }
            resourceTypePtr = resourceTypePtr->next;
        }
    }
    else if ((filterOn == STACK_RES_DISCOVERY_IF_FILTER) && filterValue)
    {
        OCResourceInterface* interfacePtr = resourcePtr->rsrcInterface;
        while (interfacePtr)
        {
            if (strcmp (interfacePtr->name, filterValue) == 0)
            {
                OCDiscoveryPayloadAddResource(payload, resourcePtr, port);
                break;
            }
            interfacePtr = interfacePtr->next;
        }
    }
    else if (filterOn == STACK_RES_DISCOVERY_NOFILTER)
    {
        OCDiscoveryPayloadAddResource(payload, resourcePtr, port);
    }
    else
    {
        //TODO: Unsupported query filter
        return OC_STACK_INVALID_QUERY;
    }


    return OC_STACK_OK;
}

const char * GetVirtualResourceUri( OCVirtualResources resource)
{
    if (resource < OC_MAX_VIRTUAL_RESOURCES)
    {
        return VIRTUAL_RSRCS[resource];
    }

    return NULL;
}

bool IsVirtualResource(const char* resourceUri)
{
    if(!resourceUri)
    {
        return false;
    }

    for (int i = 0; i < OC_MAX_VIRTUAL_RESOURCES; i++)
    {
        if (strcmp(resourceUri, GetVirtualResourceUri((OCVirtualResources)i)) == 0)
        {
            return true;
        }
    }
    return false;
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
    OC_LOG(INFO, TAG, PCF("Resource not found"));
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

    OC_LOG(INFO, TAG, PCF("Entering DetermineResourceHandling"));

    const OCDevAddr *devAddr = &request->devAddr;

    // Check if virtual resource
    if (IsVirtualResource((const char*)request->resourceUrl))
    {
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

static OCStackResult
HandleVirtualResource(OCServerRequest *request, OCResource *resource)
{
    if (!request || !resource)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult result = OC_STACK_ERROR;
    char *filterValue = NULL;
    uint8_t filterOn = 0;

    result = ValidateUrlQuery (request->resourceUrl,
            request->query, &filterOn,
            &filterValue);

    if (result == OC_STACK_OK)
    {
        if (strcmp ((char *)request->resourceUrl, GetVirtualResourceUri(OC_WELL_KNOWN_URI)) == 0)
        {
            OCDiscoveryPayload* payload = OCDiscoveryPayloadCreate();
            if(!payload)
            {
                return OC_STACK_NO_MEMORY;
            }

            for(;resource != NULL;resource=resource->next)
            {
                if((resource->resourceProperties & OC_ACTIVE)
                        && (resource->resourceProperties & OC_DISCOVERABLE))
                {
                    result = result | BuildVirtualResourceResponse(resource, filterOn,
                            filterValue, payload, (CATransportAdapter_t)request->devAddr.adapter);
                }
            }

            if(result == OC_STACK_OK)
            {
                OCEntityHandlerResponse response = {};

                response.ehResult = OC_EH_OK;
                response.payload = (OCPayload*)payload;
                response.persistentBufferFlag = 0;
                response.requestHandle = (OCRequestHandle) request;
                response.resourceHandle = (OCResourceHandle) resource;

                result = OCDoResponse(&response);
            }

            OCDiscoveryPayloadDestroy(payload);
            return result;
        }
        else if (strcmp ((char *)request->resourceUrl, GetVirtualResourceUri(OC_DEVICE_URI)) == 0)
        {
            OCDevicePayload* payload = OCDevicePayloadCreate(GetVirtualResourceUri(OC_DEVICE_URI),
                        OCGetServerInstanceID(), savedDeviceInfo.deviceName,
                        OC_SPEC_VERSION, OC_DATA_MODEL_VERSION);
            if (!payload)
            {
                return OC_STACK_NO_MEMORY;
            }

            OCEntityHandlerResponse response = {};

            response.ehResult = OC_EH_OK;
            response.payload = (OCPayload*)payload;
            response.persistentBufferFlag = 0;
            response.requestHandle = (OCRequestHandle) request;
            response.resourceHandle = (OCResourceHandle) resource;

            result = OCDoResponse(&response);

            OCDevicePayloadDestroy(payload);
            return result;
        }
        else if (strcmp ((char *)request->resourceUrl, GetVirtualResourceUri(OC_PLATFORM_URI)) == 0)
        {
            OCPlatformPayload* payload = OCPlatformPayloadCreate(
                    GetVirtualResourceUri(OC_PLATFORM_URI),
                    &savedPlatformInfo);
            if (!payload)
            {
                return OC_STACK_NO_MEMORY;
            }
            OCEntityHandlerResponse response = {};

            response.ehResult = OC_EH_OK;
            response.payload = (OCPayload*)payload;
            response.persistentBufferFlag = 0;
            response.requestHandle = (OCRequestHandle) request;
            response.resourceHandle = (OCResourceHandle) resource;

            result = OCDoResponse(&response);

            OCPlatformPayloadDestroy(payload);
            return result;
        }
        #ifdef WITH_PRESENCE
        else
        {
            if(resource->resourceProperties & OC_ACTIVE){
                SendPresenceNotification(resource->rsrcType, OC_PRESENCE_TRIGGER_CHANGE);
            }
        }
        #endif
    }
    result = OC_STACK_OK;
    return result;
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
    else if(ehRequest.obsInfo.action == OC_OBSERVE_REGISTER &&
            !collectionResource)
    {
        OC_LOG(INFO, TAG, PCF("Registering observation requested"));
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
            // The error in observeResult for the request will be
            // used when responding to this request by omitting
            // the observation option/sequence number.
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
