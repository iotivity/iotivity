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
#include "ocmalloc.h"
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
static cJSON *savedDeviceInfo = NULL;
static const char * VIRTUAL_RSRCS[] =
{
       "/oc/core",
       "/oc/core/d",
       "/oic/p",
       "/oc/core/types/d",
       #ifdef WITH_PRESENCE
       "/oic/ad"
       #endif
};

//-----------------------------------------------------------------------------
// Default resource entity handler function
//-----------------------------------------------------------------------------
OCEntityHandlerResult defaultResourceEHandler(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest * request)
{
    //TODO ("Implement me!!!!");
    // TODO:  remove silence unused param warnings
    (void) flag;
    (void) request;
    return  OC_EH_OK; // Making sure that the Default EH and the Vendor EH have matching signatures
}

/* This method will retrieve the port at which the secure resource is hosted */
static OCStackResult GetSecurePortInfo(CATransportType_t connType, uint16_t *port)
{
    CALocalConnectivity_t* info = NULL;
    uint32_t size = 0;
    OCStackResult ret = OC_STACK_ERROR;

    CAResult_t caResult = CAGetNetworkInformation(&info, &size);
    if ((caResult == CA_STATUS_OK) && info && size)
    {
        while (size--)
        {
            if (info[size].isSecured && info[size].type == connType)
            {
                if (info[size].type == CA_IPV4)
                {
                    *port = info[size].addressInfo.IP.port;
                    ret = OC_STACK_OK;
                    break;
                }
            }
        }
    }

    OCFree(info);
    return ret;
}

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

    cJSON_AddItemToObject (repObj, OC_RSRVD_PLATFORM_ID, cJSON_CreateString(info.platformID));
    cJSON_AddItemToObject (repObj, OC_RSRVD_MFG_NAME, cJSON_CreateString(info.manufacturerName));
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
                       const char *filterValue, char *out, uint16_t *remaining,
                       CATransportType_t connType )
{
    if(!resourcePtr || !out  || !remaining)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCResourceType *resourceTypePtr = NULL;
    OCResourceInterface *interfacePtr = NULL;
    cJSON *resObj = NULL;
    cJSON *propObj = NULL;
    cJSON *rtArray = NULL;
    char *jsonStr = NULL;
    uint8_t encodeRes = 0;
    OCStackResult ret = OC_STACK_OK;
    uint16_t jsonLen = 0;

    OC_LOG(INFO, TAG, PCF("Entering BuildVirtualResourceResponse"));
    resObj = cJSON_CreateObject();

    if (resourcePtr)
    {
        encodeRes = 0;
        if ((filterOn == STACK_RES_DISCOVERY_RT_FILTER) && filterValue)
        {
            resourceTypePtr = resourcePtr->rsrcType;
            while (resourceTypePtr)
            {
                if (strcmp (resourceTypePtr->resourcetypename, filterValue) == 0)
                {
                    encodeRes = 1;
                    break;
                }
                resourceTypePtr = resourceTypePtr->next;
            }
        }
        else if ((filterOn == STACK_RES_DISCOVERY_IF_FILTER) && filterValue)
        {
            interfacePtr = resourcePtr->rsrcInterface;
            while (interfacePtr)
            {
                if (strcmp (interfacePtr->name, filterValue) == 0)
                {
                    encodeRes = 1;
                    break;
                }
                interfacePtr = interfacePtr->next;
            }
        }
        else if (filterOn == STACK_RES_DISCOVERY_NOFILTER)
        {
            encodeRes = 1;
        }
        else
        {
            //TODO: Unsupported query filter
            return OC_STACK_INVALID_QUERY;
        }

        if (encodeRes)
        {
            // Add URIs
            cJSON_AddItemToObject (resObj, OC_RSRVD_HREF, cJSON_CreateString(resourcePtr->uri));

            // Add server instance id
            cJSON_AddItemToObject (resObj,
                                   OC_RSRVD_SERVER_INSTANCE_ID,
                                   cJSON_CreateString(OCGetServerInstanceIDString()));

            cJSON_AddItemToObject (resObj, OC_RSRVD_PROPERTY, propObj = cJSON_CreateObject());
            // Add resource types
            cJSON_AddItemToObject (propObj, OC_RSRVD_RESOURCE_TYPE, rtArray = cJSON_CreateArray());
            resourceTypePtr = resourcePtr->rsrcType;
            while (resourceTypePtr)
            {
                cJSON_AddItemToArray (rtArray,
                                      cJSON_CreateString(resourceTypePtr->resourcetypename));
                resourceTypePtr = resourceTypePtr->next;
            }
            // Add interface types
            cJSON_AddItemToObject (propObj, OC_RSRVD_INTERFACE, rtArray = cJSON_CreateArray());
            interfacePtr = resourcePtr->rsrcInterface;
            while (interfacePtr)
            {
                cJSON_AddItemToArray (rtArray, cJSON_CreateString(interfacePtr->name));
                interfacePtr = interfacePtr->next;
            }
            // If resource is observable, set observability flag.
            // Resources that are not observable will not have the flag.
            if (resourcePtr->resourceProperties & OC_OBSERVABLE)
            {
                cJSON_AddItemToObject (propObj, OC_RSRVD_OBSERVABLE,
                                       cJSON_CreateNumber(OC_RESOURCE_OBSERVABLE));
            }
            // Set secure flag for secure resources
            if (resourcePtr->resourceProperties & OC_SECURE)
            {
                cJSON_AddNumberToObject (propObj, OC_RSRVD_SECURE, OC_RESOURCE_SECURE);
                //Set the IP port also as secure resources are hosted on a different port
                uint16_t port = 0;
                if (GetSecurePortInfo (connType, &port) == OC_STACK_OK)
                {
                    cJSON_AddNumberToObject (propObj, OC_RSRVD_HOSTING_PORT, port);
                }
            }

        }
    }
    jsonStr = cJSON_PrintUnformatted (resObj);

    if(!jsonStr)
    {
        cJSON_Delete(resObj);
        return OC_STACK_NO_MEMORY;
    }

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
    OCFree (jsonStr);

    OC_LOG(INFO, TAG, PCF("Exiting BuildVirtualResourceResponse"));
    return ret;
}

OCStackResult BuildVirtualResourceResponseForDevice(uint8_t filterOn, char *filterValue,
                                                    char *out, uint16_t *remaining)
{
    if(!out || !remaining)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ret = OC_STACK_ERROR;

    if (savedDeviceInfo != NULL)
    {
        char *jsonStr = NULL;
        uint16_t jsonLen = 0;
        cJSON *repObj = cJSON_GetObjectItem(savedDeviceInfo, OC_RSRVD_REPRESENTATION);

        OC_LOG(INFO, TAG, PCF("Entering BuildVirtualResourceResponseForDevice"));

        if ((filterOn == STACK_DEVICE_DISCOVERY_DI_FILTER) && filterValue)
        {
            if((cJSON_GetObjectItem(repObj,OC_RSRVD_DEVICE_ID) != NULL) &&
                    strcmp(cJSON_GetObjectItem(repObj,OC_RSRVD_DEVICE_ID)->valuestring, filterValue)
                    == 0)
            {
                ret = OC_STACK_OK;
            }
        }
        else if ((filterOn == STACK_DEVICE_DISCOVERY_DN_FILTER) && filterValue)
        {
            if((cJSON_GetObjectItem(repObj,OC_RSRVD_DEVICE_NAME) != NULL) &&
                    strcmp(cJSON_GetObjectItem(repObj,OC_RSRVD_DEVICE_NAME)->valuestring,
                        filterValue) == 0)
            {
                ret = OC_STACK_OK;
            }
        }
        else if (filterOn == STACK_RES_DISCOVERY_NOFILTER)
        {
            ret = OC_STACK_OK;
        }
        else
        {
            ret = OC_STACK_INVALID_QUERY;
        }

        if (ret == OC_STACK_OK)
        {
            jsonStr = cJSON_PrintUnformatted (savedDeviceInfo);

            if(jsonStr)
            {
                jsonLen = strlen(jsonStr);

                if (jsonLen < *remaining)
                {
                    strncpy(out, jsonStr, (jsonLen + 1));
                    *remaining = *remaining - jsonLen;
                    ret = OC_STACK_OK;
                }
                else
                {
                    ret = OC_STACK_ERROR;
                }

                OCFree(jsonStr);
            }
            else
            {
                ret = OC_STACK_ERROR;
            }
        }
        else
        {
            ret = OC_STACK_INVALID_DEVICE_INFO;
        }
    }
    else
    {
        OC_LOG(ERROR, TAG, PCF("No device info found."));
        //error so that stack won't respond with empty payload
        ret = OC_STACK_INVALID_DEVICE_INFO;
    }

    OC_LOG(INFO, TAG, PCF("Exiting BuildVirtualResourceResponseForDevice"));
    return ret;
}

OCStackResult BuildVirtualResourceResponseForPlatform(char *out, uint16_t *remaining)
{
    OCStackResult ret = OC_STACK_OK;

    char *jsonStr = GetJSONStringFromPlatformInfo(savedPlatformInfo);

    if(jsonStr)
    {
        size_t jsonLen = strlen(jsonStr);

        if (jsonLen < *remaining)
        {
            strncpy(out, jsonStr, (jsonLen + 1));
            *remaining = *remaining - jsonLen;
            ret = OC_STACK_OK;
        }
        else
        {
            OC_LOG_V(ERROR, TAG, PCF("Platform info string too big. len: %u"), jsonLen);
            ret = OC_STACK_ERROR;
        }
        OCFree(jsonStr);
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
        if ((resourcePtr->resourceProperties & OC_SECURE) && (request->secured == 0))
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
HandleVirtualResource (OCServerRequest *request, OCResource* resource)
{
    if(!request || !resource)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult result = OC_STACK_ERROR;
    char *filterValue = NULL;
    uint8_t filterOn = 0;
    uint16_t remaining = 0;
    char * ptr = NULL;
    uint8_t firstLoopDone = 0;
    char discoveryResBuf[MAX_RESPONSE_LENGTH] = {};

    OC_LOG(INFO, TAG, PCF("Entering HandleVirtualResource"));

    result = ValidateUrlQuery (request->resourceUrl,
            request->query, &filterOn,
            &filterValue);

    if (result == OC_STACK_OK)
    {
        if (strcmp ((char *)request->resourceUrl, GetVirtualResourceUri(OC_WELL_KNOWN_URI)) == 0)
        {
            ptr = discoveryResBuf;
            remaining = MAX_RESPONSE_LENGTH;

            // Check if valid resource and enough space in buffer for atleast
            // the null character.
            while(resource && (remaining > 1))
            {
                if((resource->resourceProperties & OC_ACTIVE)
                        && (resource->resourceProperties & OC_DISCOVERABLE))
                {
                    // if there is data on the buffer, we have already added a response,
                    // so we need to add a comma before we do anything
                    if(firstLoopDone
                            && remaining >= (sizeof(OC_JSON_SEPARATOR)+1))
                    {
                        *ptr = OC_JSON_SEPARATOR;
                        ptr++;
                        remaining--;
                    }
                    firstLoopDone = 1;
                    result = BuildVirtualResourceResponse(resource, filterOn, filterValue,
                            (char*)ptr, &remaining, request->connectivityType );

                    if (result != OC_STACK_OK)
                    {
                        // if this failed, we need to remove the comma added above.
                        if(!firstLoopDone)
                        {
                            ptr--;
                            *ptr = '\0';
                            remaining++;
                        }
                        break;
                    }
                    ptr += strlen((char *)ptr);
                }
                resource = resource->next;
            }

            if(strlen((const char *)discoveryResBuf) > 0)
            {
                OCEntityHandlerResponse response = {};

                response.ehResult = OC_EH_OK;
                response.payload = discoveryResBuf;
                response.payloadSize = strlen((const char *)discoveryResBuf) + 1;
                response.persistentBufferFlag = 0;
                response.requestHandle = (OCRequestHandle) request;
                response.resourceHandle = (OCResourceHandle) resource;

                result = OCDoResponse(&response);
            }
        }
        else if (strcmp ((char *)request->resourceUrl, GetVirtualResourceUri(OC_DEVICE_URI)) == 0)
        {
            remaining = MAX_RESPONSE_LENGTH;
            ptr = discoveryResBuf;

            result = BuildVirtualResourceResponseForDevice(filterOn, filterValue,
                    (char*)ptr, &remaining);

            if(result == OC_STACK_OK)
            {
                ptr += strlen((char*)ptr);
            }

            if(remaining < MAX_RESPONSE_LENGTH)
            {
                OCEntityHandlerResponse response = {0};

                response.ehResult = OC_EH_OK;
                response.payload = discoveryResBuf;
                response.payloadSize = strlen((const char *)discoveryResBuf) + 1;
                response.persistentBufferFlag = 0;
                response.requestHandle = (OCRequestHandle) request;
                response.resourceHandle = (OCResourceHandle) resource;

                result = OCDoResponse(&response);
            }
        }
        else if (strcmp ((char *)request->resourceUrl, GetVirtualResourceUri(OC_PLATFORM_URI)) == 0)
        {
            remaining = MAX_RESPONSE_LENGTH;
            ptr = discoveryResBuf;

            result = BuildVirtualResourceResponseForPlatform((char*)ptr, &remaining);

            if(result == OC_STACK_OK)
            {
                ptr += strlen((char*)ptr);
            }

            if(remaining < MAX_RESPONSE_LENGTH)
            {
                OCEntityHandlerResponse response = {0};

                response.ehResult = OC_EH_OK;
                response.payload = discoveryResBuf;
                response.payloadSize = strlen((const char *)discoveryResBuf) + 1;
                response.persistentBufferFlag = 0;
                response.requestHandle = (OCRequestHandle) request;
                response.resourceHandle = (OCResourceHandle) resource;

                result = OCDoResponse(&response);
            }
        }
        #ifdef WITH_PRESENCE
        else
        {
            if(resource->resourceProperties & OC_ACTIVE){
                SendPresenceNotification(NULL);
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
            request->reqJSONPayload, request->numRcvdVendorSpecificHeaderOptions,
            request->rcvdVendorSpecificHeaderOptions,
            (OCObserveAction)request->observationOption, (OCObservationId)0);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    // At this point we know for sure that defaultDeviceHandler exists
    ehResult = defaultDeviceHandler(OC_REQUEST_FLAG, &ehRequest,
                                  (char*) request->resourceUrl);
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
                &request->addressInfo, request->connectivityType);

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

    ehResult = resource->entityHandler(ehFlag, &ehRequest);
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

void DeleteDeviceInfo()
{
    if(savedDeviceInfo)
    {
        cJSON_Delete(savedDeviceInfo);
    }
}

void DeletePlatformInfo()
{
    OC_LOG(INFO, TAG, PCF("Deleting platform info."));

    OCFree(savedPlatformInfo.platformID);
    savedPlatformInfo.platformID = NULL;

    OCFree(savedPlatformInfo.manufacturerName);
    savedPlatformInfo.manufacturerName = NULL;

    OCFree(savedPlatformInfo.manufacturerUrl);
    savedPlatformInfo.manufacturerUrl = NULL;

    OCFree(savedPlatformInfo.modelNumber);
    savedPlatformInfo.modelNumber = NULL;

    OCFree(savedPlatformInfo.dateOfManufacture);
    savedPlatformInfo.dateOfManufacture = NULL;

    OCFree(savedPlatformInfo.platformVersion);
    savedPlatformInfo.platformVersion = NULL;

    OCFree(savedPlatformInfo.operatingSystemVersion);
    savedPlatformInfo.operatingSystemVersion = NULL;

    OCFree(savedPlatformInfo.hardwareVersion);
    savedPlatformInfo.hardwareVersion = NULL;

    OCFree(savedPlatformInfo.firmwareVersion);
    savedPlatformInfo.firmwareVersion = NULL;

    OCFree(savedPlatformInfo.supportUrl);
    savedPlatformInfo.supportUrl = NULL;

    OCFree(savedPlatformInfo.systemTime);
    savedPlatformInfo.systemTime = NULL;
}

static OCStackResult CloneStringIfNonNull(char **dest, char *src)
{
    if (src)
    {
        *dest = (char*) OCMalloc(strlen(src) + 1);
        if (!*dest)
        {
            return OC_STACK_NO_MEMORY;
        }
        strcpy(*dest, src);
    }
    else
    {
        *dest = NULL;
    }
    return OC_STACK_OK;
}
static OCStackResult DeepCopyPlatFormInfo(OCPlatformInfo info)
{
    DeletePlatformInfo();

    OCStackResult ret = OC_STACK_OK;
    ret = CloneStringIfNonNull(&(savedPlatformInfo.platformID), info.platformID);
    VERIFY_SUCCESS(ret, OC_STACK_OK);

    ret = CloneStringIfNonNull(&(savedPlatformInfo.manufacturerName), info.manufacturerName);
    VERIFY_SUCCESS(ret, OC_STACK_OK);

    ret = CloneStringIfNonNull(&(savedPlatformInfo.manufacturerUrl), info.manufacturerUrl);
    VERIFY_SUCCESS(ret, OC_STACK_OK);

    ret = CloneStringIfNonNull(&(savedPlatformInfo.modelNumber), info.modelNumber);
    VERIFY_SUCCESS(ret, OC_STACK_OK);

    ret = CloneStringIfNonNull(&(savedPlatformInfo.dateOfManufacture), info.dateOfManufacture);
    VERIFY_SUCCESS(ret, OC_STACK_OK);

    ret = CloneStringIfNonNull(&(savedPlatformInfo.platformVersion), info.platformVersion);
    VERIFY_SUCCESS(ret, OC_STACK_OK);

    ret = CloneStringIfNonNull(&(savedPlatformInfo.operatingSystemVersion), info.operatingSystemVersion);
    VERIFY_SUCCESS(ret, OC_STACK_OK);

    ret = CloneStringIfNonNull(&(savedPlatformInfo.hardwareVersion), info.hardwareVersion);
    VERIFY_SUCCESS(ret, OC_STACK_OK);

    ret = CloneStringIfNonNull(&(savedPlatformInfo.firmwareVersion), info.firmwareVersion);
    VERIFY_SUCCESS(ret, OC_STACK_OK);

    ret = CloneStringIfNonNull(&(savedPlatformInfo.supportUrl), info.supportUrl);
    VERIFY_SUCCESS(ret, OC_STACK_OK);

    ret = CloneStringIfNonNull(&(savedPlatformInfo.systemTime), info.systemTime);
    VERIFY_SUCCESS(ret, OC_STACK_OK);

    return OC_STACK_OK;

    exit:
        DeletePlatformInfo();
        return ret;

}

OCStackResult SavePlatformInfo(OCPlatformInfo info)
{
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
