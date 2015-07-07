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

/*
 * Encodes platform info into json and returns a string on the heap.
 * Caller responsible for freeing the string.
 */
static char* GetJSONStringFromPlatformInfo(OCPlatformInfo info)
{
    cJSON *rootObj = cJSON_CreateObject();

    if (!rootObj)
    {
        return NULL;
    }

    cJSON *repObj = NULL;
    char *jsonEncodedInfo = NULL;

    cJSON_AddItemToObject (rootObj, OC_RSRVD_HREF,
            cJSON_CreateString(GetVirtualResourceUri(OC_PLATFORM_URI)));

    cJSON_AddItemToObject (rootObj, OC_RSRVD_REPRESENTATION, repObj = cJSON_CreateObject());

    if (!repObj)
    {
        return NULL;
    }
    if (info.platformID)
    {
        cJSON_AddItemToObject (repObj, OC_RSRVD_PLATFORM_ID,
                cJSON_CreateString(info.platformID));
    }

    if (info.manufacturerName)
    {
        cJSON_AddItemToObject (repObj, OC_RSRVD_MFG_NAME,
                cJSON_CreateString(info.manufacturerName));
    }

    if (info.manufacturerUrl)
    {
        cJSON_AddItemToObject (repObj, OC_RSRVD_MFG_URL,
                cJSON_CreateString(info.manufacturerUrl));
    }

    if (info.modelNumber)
    {
        cJSON_AddItemToObject (repObj, OC_RSRVD_MODEL_NUM,
                cJSON_CreateString(info.modelNumber));
    }

    if (info.dateOfManufacture)
    {
        cJSON_AddItemToObject (repObj, OC_RSRVD_MFG_DATE,
                cJSON_CreateString(info.dateOfManufacture));
    }

    if (info.platformVersion)
    {
        cJSON_AddItemToObject (repObj, OC_RSRVD_PLATFORM_VERSION,
                cJSON_CreateString(info.platformVersion));
    }

    if (info.operatingSystemVersion)
    {
        cJSON_AddItemToObject (repObj, OC_RSRVD_OS_VERSION,
                cJSON_CreateString(info.operatingSystemVersion));
    }

    if (info.hardwareVersion)
    {
        cJSON_AddItemToObject (repObj, OC_RSRVD_HARDWARE_VERSION,
                cJSON_CreateString(info.hardwareVersion));
    }

    if (info.firmwareVersion)
    {
        cJSON_AddItemToObject (repObj, OC_RSRVD_FIRMWARE_VERSION,
                cJSON_CreateString(info.firmwareVersion));
    }

    if (info.supportUrl)
    {
        cJSON_AddItemToObject (repObj, OC_RSRVD_SUPPORT_URL,
                cJSON_CreateString(info.supportUrl));
    }

    if (info.systemTime)
    {
        cJSON_AddItemToObject (repObj, OC_RSRVD_SYSTEM_TIME,
                cJSON_CreateString(info.systemTime));
    }

    jsonEncodedInfo = cJSON_PrintUnformatted (rootObj);

    cJSON_Delete(rootObj);

    return jsonEncodedInfo;
}

/*
 * Encodes device info into json and returns a string on the heap.
 * Caller responsible for freeing the string.
 */
static char* GetJSONStringFromDeviceInfo(OCDeviceInfo info)
{
    cJSON *rootObj = cJSON_CreateObject();

    if (!rootObj)
    {
        return NULL;
    }

    cJSON *repObj = NULL;
    char *jsonEncodedInfo = NULL;

    cJSON_AddItemToObject (rootObj, OC_RSRVD_HREF,
            cJSON_CreateString(GetVirtualResourceUri(OC_DEVICE_URI)));

    cJSON_AddItemToObject (rootObj, OC_RSRVD_REPRESENTATION, repObj = cJSON_CreateObject());

    if (!repObj)
    {
        return NULL;
    }

    cJSON_AddItemToObject (repObj, OC_RSRVD_DEVICE_ID,
                    cJSON_CreateString(OCGetServerInstanceIDString()));

    if (info.deviceName)
    {
        cJSON_AddItemToObject (repObj, OC_RSRVD_DEVICE_NAME,
                        cJSON_CreateString(info.deviceName));
    }

    cJSON_AddItemToObject (repObj, OC_RSRVD_SPEC_VERSION,
                        cJSON_CreateString(OC_SPEC_VERSION));

    cJSON_AddItemToObject (repObj, OC_RSRVD_DATA_MODEL_VERSION,
                        cJSON_CreateString(OC_DATA_MODEL_VERSION));

    jsonEncodedInfo = cJSON_PrintUnformatted (rootObj);

    cJSON_Delete(rootObj);

    return jsonEncodedInfo;
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

    OC_LOG_V(INFO, TAG, PCF("Received query %s for param extraction"), query);

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

static OCVirtualResources GetTypeOfVirtualURI(char *uriInRequest)
{
    if (strcmp (uriInRequest, GetVirtualResourceUri(OC_WELL_KNOWN_URI)) == 0)
    {
        return OC_WELL_KNOWN_URI;
    }
    else if (strcmp (uriInRequest, GetVirtualResourceUri(OC_DEVICE_URI)) == 0)
    {
        return OC_DEVICE_URI;
    }
    else if (strcmp (uriInRequest, GetVirtualResourceUri(OC_PLATFORM_URI)) == 0)
    {
        return OC_PLATFORM_URI;
    }

    #ifdef WITH_PRESENCE
    else
    {
        return OC_PRESENCE;
    }
    #endif
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

OCStackResult copyStringIfEnoughSpace(char* dest, const char *src, uint16_t *remaining)
{
    if (!dest || !src || !remaining)
    {
        return OC_STACK_INVALID_PARAM;
    }

    size_t srcLen = strlen(src);

    if (srcLen < *remaining)
    {
        OICStrcpy(dest, *remaining, src);
        *remaining = *remaining - srcLen;
        return OC_STACK_OK;
    }
    else
    {
        return OC_STACK_ERROR;
    }
}

OCStackResult BuildVirtualResourceResponse(const OCResource *resourcePtr, char *out,
                        uint16_t *remaining, CATransportAdapter_t adapter )
{
    if(!resourcePtr || !out  || !remaining)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCResourceType *resourceTypePtr = NULL;
    OCResourceInterface *interfacePtr = NULL;
    cJSON *resObj = NULL;
    cJSON *policyObj = NULL;
    cJSON *rtArray = NULL;
    char *jsonStr = NULL;
    OCStackResult ret = OC_STACK_OK;

    OC_LOG_V(INFO, TAG, PCF("Encoding resource %s"), resourcePtr->uri);

    resObj = cJSON_CreateObject();

    if (resourcePtr)
    {
        // URI
        cJSON_AddItemToObject (resObj, OC_RSRVD_HREF, cJSON_CreateString(resourcePtr->uri));

        // resource types
        cJSON_AddItemToObject (resObj, OC_RSRVD_RESOURCE_TYPE, rtArray = cJSON_CreateArray());
        resourceTypePtr = resourcePtr->rsrcType;
        while (resourceTypePtr)
        {
            cJSON_AddItemToArray (rtArray,
                                  cJSON_CreateString(resourceTypePtr->resourcetypename));
            resourceTypePtr = resourceTypePtr->next;
        }
        // interfaces
        cJSON_AddItemToObject (resObj, OC_RSRVD_INTERFACE, rtArray = cJSON_CreateArray());
        interfacePtr = resourcePtr->rsrcInterface;
        while (interfacePtr)
        {
            cJSON_AddItemToArray (rtArray, cJSON_CreateString(interfacePtr->name));
            interfacePtr = interfacePtr->next;
        }

        // Policy
        cJSON_AddItemToObject (resObj, OC_RSRVD_POLICY, policyObj = cJSON_CreateObject());

        if (policyObj)
        {
            cJSON_AddNumberToObject (policyObj, OC_RSRVD_BITMAP,
                             resourcePtr->resourceProperties & (OC_OBSERVABLE|OC_DISCOVERABLE));
            // Set secure flag for secure resources
            if (resourcePtr->resourceProperties & OC_SECURE)
            {
                cJSON_AddNumberToObject(policyObj, OC_RSRVD_SECURE, OC_RESOURCE_SECURE);
                //Set the IP port also as secure resources are hosted on a different port
                uint16_t port = 0;
                if (GetSecurePortInfo(adapter, &port) == OC_STACK_OK)
                {
                    cJSON_AddNumberToObject(policyObj, OC_RSRVD_HOSTING_PORT, port);
                }
            }
        }
        else
        {
            cJSON_Delete(resObj);
            return OC_STACK_NO_MEMORY;
        }
    }
    jsonStr = cJSON_PrintUnformatted (resObj);

    if(!jsonStr)
    {
        cJSON_Delete(resObj);
        return OC_STACK_NO_MEMORY;
    }

    ret = copyStringIfEnoughSpace(out, jsonStr, remaining);

    cJSON_Delete (resObj);
    OICFree (jsonStr);

    return ret;
}

OCStackResult BuildVirtualResourceResponseForDevice(char *out, uint16_t *remaining)
{
    if(!out || !remaining)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ret = OC_STACK_ERROR;
    char *jsonStr = NULL;

    jsonStr = GetJSONStringFromDeviceInfo(savedDeviceInfo);

    if(jsonStr)
    {
        ret = copyStringIfEnoughSpace(out, jsonStr, remaining);
        OICFree(jsonStr);
    }
    else
    {
        OC_LOG(ERROR, TAG, PCF("Error encoding save device info."));
        ret = OC_STACK_ERROR;
    }
    return ret;
}

OCStackResult BuildVirtualResourceResponseForPlatform(char *out, uint16_t *remaining)
{
    if(!out || !remaining)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ret = OC_STACK_ERROR;
    char *jsonStr = NULL;

    jsonStr = GetJSONStringFromPlatformInfo(savedPlatformInfo);

    if(jsonStr)
    {
        ret = copyStringIfEnoughSpace(out, jsonStr, remaining);
        OICFree(jsonStr);
    }
    else
    {
        OC_LOG(ERROR, TAG, PCF("Error encoding save platform info."));
        ret = OC_STACK_ERROR;
    }

    return ret;
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
                                char *discoveryPayload)
{
    OCEntityHandlerResponse response = {};

    response.ehResult = OC_EH_OK;
    response.payload = discoveryPayload;
    response.payloadSize = strlen((const char *)discoveryPayload) + 1;
    response.persistentBufferFlag = 0;
    response.requestHandle = (OCRequestHandle) request;
    response.resourceHandle = (OCResourceHandle) resource;

    return OCDoResponse(&response);
}

static OCStackResult appendDeviceIDAndOpenLinksArray (char *out, uint16_t *remaining)
{
    if (!out || !remaining)
    {
        return OC_STACK_INVALID_PARAM;
    }

    const char *deviceID = OCGetServerInstanceIDString();

    char deviceIDJSONField[MAX_RESPONSE_LENGTH] = {};

    // Format is : {"di":"UUID","links":[
    sprintf (deviceIDJSONField, "{\"%s\":\"%s\"%s\"%s\":[", OC_RSRVD_DEVICE_ID, deviceID,
                            OC_JSON_SEPARATOR_STR, OC_RSRVD_LINKS);

    uint16_t lenDeviceIDJSON = strlen(deviceIDJSONField);

    if (lenDeviceIDJSON > *remaining)
    {
        return OC_STACK_ERROR;
    }
    else
    {
        OICStrcat(out, *remaining, deviceIDJSONField);
        *remaining -= lenDeviceIDJSON;
    }

    return OC_STACK_OK;
}

static OCStackResult HandleVirtualResource (OCServerRequest *request, OCResource* resource)
{
    if (!request || !resource)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult discoveryResult = OC_STACK_ERROR;
    char *filterOne = NULL;
    char *filterTwo = NULL;
    uint16_t remaining  = 0;

    char discoveryBuffer[MAX_RESPONSE_LENGTH] = {};

    OC_LOG(INFO, TAG, PCF("Entering HandleVirtualResource"));

    OCVirtualResources virtualUriInRequest = GetTypeOfVirtualURI (request->resourceUrl);

    remaining = MAX_RESPONSE_LENGTH - sizeof ('\0');

    if (virtualUriInRequest == OC_WELL_KNOWN_URI)
    {
        discoveryResult = getQueryParamsForFiltering (virtualUriInRequest, request->query,
                                            &filterOne, &filterTwo);
        if (discoveryResult != OC_STACK_OK)
        {
            OC_LOG_V(ERROR, TAG, "Error (%d) validating query.\n", discoveryResult);
            return discoveryResult;
        }
        char *ptrIntoBuffer = discoveryBuffer;

        discoveryResult = appendDeviceIDAndOpenLinksArray(ptrIntoBuffer, &remaining);

        while(resource && discoveryResult == OC_STACK_OK)
        {
            if(includeThisResourceInResponse(resource, filterOne, filterTwo))
            {
                ptrIntoBuffer += strlen(ptrIntoBuffer);

                discoveryResult = BuildVirtualResourceResponse(resource, ptrIntoBuffer,
                                        &remaining, (CATransportAdapter_t)request->devAddr.adapter);

                if (discoveryResult != OC_STACK_OK)
                {
                    OC_LOG_V(INFO, TAG, "Error (%d) encoding %s", discoveryResult, resource->uri);
                    break;
                }

                ptrIntoBuffer += strlen(ptrIntoBuffer);
                if (remaining > sizeof(OC_JSON_SEPARATOR))
                {
                    *ptrIntoBuffer = OC_JSON_SEPARATOR;
                    ++ptrIntoBuffer;
                    --remaining;
                }
                else
                {
                    OC_LOG(INFO, TAG, PCF("Out of space in buffer"));
                    break;
                }

            }
            resource = resource->next;
        }

        size_t lenDiscoveryBuff = strlen (discoveryBuffer);

        // Traling separator replaced with closing of "links" array.
        // discoveryBuffer guaranteed to be null terminated as "remaining"
        // accounts for '\0' when initialized.
        if (lenDiscoveryBuff > 0 && discoveryBuffer[lenDiscoveryBuff - 1] == OC_JSON_SEPARATOR)
        {
            discoveryBuffer[lenDiscoveryBuff - 1] = ']';
        }
        else
        {
            // If No trailing separator then close links array anyway.
            discoveryBuffer[lenDiscoveryBuff] = ']';
        }
        --remaining;
        ptrIntoBuffer += strlen(ptrIntoBuffer);

        // Close array element brace.
        if (remaining > sizeof ('}'))
        {
            *ptrIntoBuffer = '}';
            --remaining;
        }
        else
        {
            discoveryResult = OC_STACK_ERROR;
        }
    }

    else if (virtualUriInRequest == OC_DEVICE_URI)
    {
        discoveryResult = BuildVirtualResourceResponseForDevice(discoveryBuffer, &remaining);
    }
    else if (virtualUriInRequest == OC_PLATFORM_URI)
    {
        discoveryResult = BuildVirtualResourceResponseForPlatform(discoveryBuffer, &remaining);
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
                                                        discoveryBuffer);
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
            request->reqJSONPayload, request->numRcvdVendorSpecificHeaderOptions,
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
            request->reqJSONPayload, request->numRcvdVendorSpecificHeaderOptions,
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
            request->reqJSONPayload, request->numRcvdVendorSpecificHeaderOptions,
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
