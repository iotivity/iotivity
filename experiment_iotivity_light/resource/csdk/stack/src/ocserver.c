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


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

// Defining _POSIX_C_SOURCE macro with 200112L (or greater) as value
// causes header files to expose definitions
// corresponding to the POSIX.1-2001 base
// specification (excluding the XSI extension).
// For POSIX.1-2001 base specification,
// Refer http://pubs.opengroup.org/onlinepubs/009695399/
#define _POSIX_C_SOURCE 200112L
#include <string.h>
#include <ctype.h>

#include "oic_malloc.h"
#include "oic_string.h"
#include "octypes.h"
#include "ocadaptation.h"
#include "occlientcb.h"
#include "ocserverrequest.h"
#include "ocstackinternal.h"
#include "ocresourcehandler.h"
#include "ocobserve.h"
#include "ocrandom.h"
//#include "secureresourcemanager.h"
#include "ocpayload.h"
#include "ocpayloadint.h"
#include "ocpayloadcbor.h"
#include "ocglobals.h"
#include "ocstack.h"
#include "ocstackextra.h"

#ifdef WITH_ARDUINO
#include "Time.h"
#else
#include <sys/time.h>
#endif

#ifndef ARDUINO
#include <arpa/inet.h>
#endif

#ifndef UINT32_MAX
#define UINT32_MAX   (0xFFFFFFFFUL)
#endif

//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------

OCResource *headResource = NULL;
static OCResource *tailResource = NULL;

#ifdef WITH_PRESENCE
PresenceResource ThePresenceResource = { .initialized = 0 };
#endif

static ServerID sid;

OCDeviceEntityHandler DefaultDeviceHandler = NULL;
void* DefaultDeviceHandlerCallbackParameter = NULL;

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define TAG  PCF("OCServer")

//TODO: we should allow the server to define this
#define MAX_OBSERVE_AGE (0x2FFFFUL)

//-----------------------------------------------------------------------------
// forward references
//-----------------------------------------------------------------------------
OCStackResult initResources();
void deleteAllResources();
OCStackResult deleteResource(OCResource *resource);

#ifdef WITH_RESOURCE_API
void insertResource(OCResource *resource);

OCResource *findResource(OCResource *resource);

static OCStackResult insertResourceType(OCResource *resource, const char *name);

OCResourceText_t *findResourceTypeAtIndex(OCResourceHandle handle, int index);

OCResourceText_t *findResourceInterfaceAtIndex(OCResourceHandle handle, int index);

void deleteResourceElements(OCResource *resource);

void deleteResourceInterface(OCResourceText_t *resourceInterface);

static OCStackResult insertResourceInterface(OCResource *resource, const char *name);
#endif // WITH_RESOURCE_API

#ifdef WITH_PRESENCE
void OCChangeActiveProperty(OCResourceHandle handle, int enable);
#endif

//-----------------------------------------------------------------------------
// Internal functions
//-----------------------------------------------------------------------------
OCStackResult OCInitServer()
{
    if (OCGenerateUuid(sid) != RAND_UUID_OK)
    {
        OC_LOG(FATAL, TAG, PCF("Generate UUID for Server Instance failed!"));
        return OC_STACK_ERROR;
    }
    DefaultDeviceHandler = NULL;
    DefaultDeviceHandlerCallbackParameter = NULL;
    return initResources();
}

void OCStopServer()
{
    deleteAllResources();
    DeleteDeviceInfo();
    DeletePlatformInfo();
}

//-----------------------------------------------------------------------------
// Internal API function
//-----------------------------------------------------------------------------

CAResponseResult_t OCToCAStackResult(OCStackResult ocCode)
{
    CAResponseResult_t ret = CA_INTERNAL_SERVER_ERROR;

    switch(ocCode)
    {
        case OC_STACK_OK:
            ret = CA_SUCCESS;
            break;
        case OC_STACK_RESOURCE_CREATED:
            ret = CA_CREATED;
            break;
        case OC_STACK_RESOURCE_DELETED:
            ret = CA_DELETED;
            break;
        case OC_STACK_INVALID_QUERY:
            ret = CA_BAD_REQ;
            break;
        case OC_STACK_INVALID_OPTION:
            ret = CA_BAD_OPT;
            break;
        case OC_STACK_NO_RESOURCE:
            ret = CA_NOT_FOUND;
            break;
        case OC_STACK_COMM_ERROR:
            ret = CA_RETRANSMIT_TIMEOUT;
            break;
        case OC_STACK_UNAUTHORIZED_REQ:
            ret = CA_UNAUTHORIZED_REQ;
            break;
        default:
            break;
    }
    return ret;
}

//This function will be called back by CA layer when a request is received
void HandleCARequests(CAMessageInfo_t *messageInfo)
{
    OCRequestInfo_t *requestInfo = (OCRequestInfo_t *)messageInfo;
    OCStackResult result = OC_STACK_ERROR;

    if (!caglobals.server)
    {
        return;
    }

    OC_LOG_V(INFO, TAG, PCF("request URI: %s?%s"), messageInfo->info.resourcePath, messageInfo->info.resourceQuery);

    // reorganize and rename data for later usage
    // split up query into before and after ?
    requestInfo->qos = messageInfo->info.coapType == OC_MSG_CONFIRM ? OC_HIGH_QOS : OC_LOW_QOS;
    requestInfo->signature = REQUEST_SIGNATURE;
    requestInfo->notificationFlag = 1;
    requestInfo->ehResponseHandler = HandleSingleResponse;

    result = ProcessRequest(requestInfo);
    if (result != OC_STACK_OK)
    {
        OC_LOG_V(ERROR, TAG, PCF("HandleStackRequests process failed: %d"), result);

        messageInfo->info.coapCode = OCToCAStackResult(result);
        OCSendMessage(messageInfo);
    }
    else if (result == OC_STACK_SLOW_RESOURCE)
    {
        // Send ACK to client as precursor to slow response
        SendDirectStackResponse(messageInfo, OC_MSG_ACKNOWLEDGE);
    }
    return;
}

OCStackResult OCStartPresence(const uint32_t ttl)
{
#ifdef WITH_PRESENCE
    if (!ThePresenceResource.initialized)
    {
        ThePresenceResource.initialized = 1;
        OICStrcpy(ThePresenceResource.resource.uri, sizeof (ThePresenceResource.resource.uri),
                                                        OC_RSRVD_PRESENCE_URI);
        CAGenerateToken(&ThePresenceResource.token, USE_TOKEN_LEN);
#ifdef RT_MAX
        OICStrcpy(ThePresenceResource.resource.rsrcType[0].name,
                            MAX_NAME_LENGTH, OC_RSRVD_RESOURCE_TYPE_PRESENCE);
        ThePresenceResource.resource.nTypes = 1;
#else
        static OCResourceText_t presenceType = { NULL, OC_RSRVD_RESOURCE_TYPE_PRESENCE};
        ThePresenceResource.resource.rsrcType = &presenceType;
#endif // RT_MAX
#ifdef RI_MAX
        OICStrcpy(ThePresenceResource.resource.rsrcInterface[0].name,
                            MAX_NAME_LENGTH, "core.r");
        ThePresenceResource.resource.nInterfaces = 1;
#else
        static OCResourceText_t presenceInterface = { NULL, "core.r" };
        ThePresenceResource.resource.rsrcInterface = &presenceInterface;
#endif // RI_MAX
        ThePresenceResource.resource.resourceProperties = OC_DISCOVERABLE|OC_ACTIVE;
    }

    ThePresenceResource.active = 1;
    ThePresenceResource.resource.sequenceNum = OCGetRandom(); // yes, each time

    if (OC_MAX_PRESENCE_TTL_SECONDS < ttl)
    {
        ThePresenceResource.presenceTTL = OC_MAX_PRESENCE_TTL_SECONDS;
        OC_LOG(INFO, TAG, PCF("Setting Presence TTL to max value"));
    }
    else if (0 == ttl)
    {
        ThePresenceResource.presenceTTL = OC_DEFAULT_PRESENCE_TTL_SECONDS;
        OC_LOG(INFO, TAG, PCF("Setting Presence TTL to default value"));
    }
    else
    {
        ThePresenceResource.presenceTTL = ttl;
    }
    OC_LOG_V(DEBUG, TAG, "Presence TTL is %lu seconds", ThePresenceResource.presenceTTL);

    NotifyPresenceChange(&ThePresenceResource.resource, OC_PRESENCE_TRIGGER_CREATE);
    return OC_STACK_OK;
#else
    (void)ttl;
    return OC_STACK_NOTIMPL;
#endif // WITH_PRESENCE
}

OCStackResult OCStopPresence()
{
#ifdef WITH_PRESENCE
    ThePresenceResource.resource.sequenceNum = OCGetRandom();
    ThePresenceResource.active = 0;

    return SendStopNotification();
#else
    return OC_STACK_NOTIMPL;
#endif // WITH_PRESENCE
}

OCStackResult OCSetDefaultDeviceEntityHandler(OCDeviceEntityHandler entityHandler,
                                            void* callbackParameter)
{
    DefaultDeviceHandler = entityHandler;
    DefaultDeviceHandlerCallbackParameter = callbackParameter;

    return OC_STACK_OK;
}

OCStackResult OCSetPlatformInfo(const OCPlatformInfo *platformInfo)
{
    if (!platformInfo)
    {
        OC_LOG(ERROR, TAG, PCF("Null or empty platform info."));
        return OC_STACK_INVALID_PARAM;
    }
    SavePlatformInfo(platformInfo);
    return OC_STACK_OK;
}

OCStackResult OCSetDeviceInfo(const OCDeviceInfo *deviceInfo)
{
    if (!deviceInfo || deviceInfo->deviceName[0] == '\0')
    {
        OC_LOG(ERROR, TAG, PCF("Null or empty device name."));
        return OC_STACK_INVALID_PARAM;
    }

    SaveDeviceInfo(deviceInfo);
    return OC_STACK_OK;
}

OCStackResult OCCreateResource(OCResourceHandle *handle,
                               const char *resourceTypeName,
                               const char *resourceInterfaceName,
                               const char *uri,
                               OCEntityHandler entityHandler,
                               void *callbackParam,
                               uint8_t resourceProperties)
{
#ifdef WITH_RESOURCE_API
    OCResource *resource = NULL;
    OCStackResult result = OC_STACK_ERROR;

    if (!resourceInterfaceName || strlen(resourceInterfaceName) == 0)
    {
        resourceInterfaceName = OC_RSRVD_INTERFACE_DEFAULT;
    }

    // Make sure resourceProperties bitmask has allowed properties specified
    if (resourceProperties > OC_VALID_RESOURCE_PROPERTY)
    {
        OC_LOG(ERROR, TAG, PCF("Invalid property"));
        return OC_STACK_INVALID_PARAM;
    }

    for (resource = headResource; resource; resource = resource->next)
    {
        if (strcmp(uri, resource->uri) == 0)
        {
            OC_LOG_V(ERROR, TAG, "Resource %s already exists", uri);
            return OC_STACK_INVALID_PARAM;
        }
    }
    // Create the pointer and insert it into the resource list
    resource = (OCResource *)OCCalloc(MAT_resource);
    resource->sequenceNum = OC_OFFSET_SEQUENCE_NUMBER;

    insertResource(resource);

    // Set the uri
    OICStrcpy(resource->uri, sizeof (resource->uri), uri);

    resource->resourceProperties = (OCResourceProperty)(resourceProperties|OC_ACTIVE);

    result = insertResourceType(resource, resourceTypeName);
    if (result != OC_STACK_OK)
    {
        goto exit;
    }
    result = insertResourceInterface(resource, resourceInterfaceName);
    if (result != OC_STACK_OK)
    {
        goto exit;
    }

    if (entityHandler)
    {
        resource->entityHandler = entityHandler;
        resource->ehCallbackParam = callbackParam;
    }
    else
    {
        resource->entityHandler = defaultResourceEHandler;
        resource->ehCallbackParam = NULL;
    }

    *handle = resource;

    #ifdef WITH_PRESENCE
    NotifyPresenceChange(resource, OC_PRESENCE_TRIGGER_CREATE);
    #endif

    return OC_STACK_OK;

exit:
    if (result != OC_STACK_OK)
    {
        deleteResource(resource);
    }
    return result;
#else
    (void)handle;
    (void)resourceTypeName;
    (void)resourceInterfaceName;
    (void)uri;
    (void)entityHandler;
    (void)callbackParam;
    (void)resourceProperties;
    return OC_STACK_NOTIMPL;
#endif // WITH_RESOURCE_API
}

OCStackResult OCBindResource(
        OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
{
#ifdef WITH_RESOURCE_API
#ifdef WITH_COLLECTIONS
    OCResource *resource = NULL;
    uint8_t i = 0;

    VERIFY_NON_NULL(collectionHandle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(resourceHandle, ERROR, OC_STACK_ERROR);

    if (collectionHandle == resourceHandle)
    {
        OC_LOG(ERROR, TAG, PCF("Added handle equals collection handle"));
        return OC_STACK_INVALID_PARAM;
    }

    resource = findResource((OCResource *)collectionHandle);
    if (!resource)
    {
        OC_LOG(ERROR, TAG, PCF("Collection handle not found"));
        return OC_STACK_INVALID_PARAM;
    }

    for (i = 0; i < MAX_CONTAINED_RESOURCES; i++)
    {
        if (resource->rsrcResources[i])
        {
            continue;   // slot used
        }
        resource->rsrcResources[i] = (OCResource *)resourceHandle;

        #ifdef WITH_PRESENCE
        NotifyPresenceChange(resource, OC_PRESENCE_TRIGGER_CHANGE);
        NotifyPresenceChange(resource->rsrcResources[i], OC_PRESENCE_TRIGGER_CHANGE);
        #endif
        return OC_STACK_OK;
    }

    // Unable to add resourceHandle, so return error
    return OC_STACK_ERROR;
#else
    (void)collectionHandle;
    (void)resourceHandle;
    return OC_STACK_NOTIMPL;
#endif // WITH_COLLECTIONS
#else
    (void)collectionHandle;
    (void)resourceHandle;
    return OC_STACK_NOTIMPL;
#endif // WITH_RESOURCE_API
}

OCStackResult OCUnBindResource(
        OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
{
#ifdef WITH_RESOURCE_API
#ifdef WITH_COLLECTIONS
    OCResource *resource = NULL;
    uint8_t i = 0;

    OC_LOG(INFO, TAG, PCF("Entering OCUnBindResource"));

    // Validate parameters
    VERIFY_NON_NULL(collectionHandle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(resourceHandle, ERROR, OC_STACK_ERROR);
    // Container cannot contain itself
    if (collectionHandle == resourceHandle)
    {
        OC_LOG(ERROR, TAG, PCF("removing handle equals collection handle"));
        return OC_STACK_INVALID_PARAM;
    }

    // Use the handle to find the resource in the resource linked list
    resource = findResource((OCResource *) collectionHandle);
    if (!resource)
    {
        OC_LOG(ERROR, TAG, PCF("Collection handle not found"));
        return OC_STACK_INVALID_PARAM;
    }

    // Look for an open slot to add add the child resource.
    // If found, add it and return success
    for (i = 0; i < MAX_CONTAINED_RESOURCES; i++)
    {
        if (resourceHandle != resource->rsrcResources[i])
        {
            continue;
        }

        resource->rsrcResources[i] = (OCResource *) NULL;
        OC_LOG(INFO, TAG, PCF("resource unbound"));

        // Send notification when resource is unbounded successfully.
        #ifdef WITH_PRESENCE
        NotifyPresenceChange(resource, OC_PRESENCE_TRIGGER_CHANGE);
        #endif
        return OC_STACK_OK;
    }

    OC_LOG(INFO, TAG, PCF("resource not found in collection"));

    // Unable to add resourceHandle, so return error
    return OC_STACK_ERROR;
#else
    (void)collectionHandle;
    (void)resourceHandle;
    return OC_STACK_NOTIMPL;
#endif // WITH_COLLECTIONS
#else
    (void)collectionHandle;
    (void)resourceHandle;
    return OC_STACK_NOTIMPL;
#endif // WITH_RESOURCE_API
}

OCStackResult OCBindResourceTypeToResource(OCResourceHandle handle,
                                                const char *resourceTypeName)
{
#ifdef WITH_RESOURCE_API
    OCStackResult result = OC_STACK_ERROR;
    OCResource *resource = NULL;

    resource = findResource((OCResource *) handle);
    if (!resource)
    {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_ERROR;
    }

    result = insertResourceType(resource, resourceTypeName);

    #ifdef WITH_PRESENCE
    NotifyPresenceChange(resource, OC_PRESENCE_TRIGGER_CREATE);
    #endif

    return result;
#else
    (void)handle;
    (void)resourceTypeName;
    return OC_STACK_NOTIMPL;
#endif // WITH_RESOURCE_API
}

OCStackResult OCBindResourceInterfaceToResource(OCResourceHandle handle,
                                            const char *resourceInterfaceName)
{
#ifdef WITH_RESOURCE_API
    OCStackResult result = OC_STACK_ERROR;
    OCResource *resource = NULL;

    resource = findResource((OCResource *) handle);
    if (!resource)
    {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_ERROR;
    }

    result = insertResourceInterface(resource, resourceInterfaceName);

    #ifdef WITH_PRESENCE
    NotifyPresenceChange(resource, OC_PRESENCE_TRIGGER_CHANGE);
    #endif

    return result;
#else
    (void)handle;
    (void)resourceInterfaceName;
    return OC_STACK_NOTIMPL;
#endif // WITH_RESOURCE_API
}

OCStackResult OCGetNumberOfResources(uint8_t *numResources)
{
#ifdef WITH_RESOURCE_API
    VERIFY_NON_NULL(numResources, ERROR, OC_STACK_INVALID_PARAM);
    *numResources = 0;

    for (OCResource *pointer = headResource; pointer; pointer = pointer->next)
    {
        (*numResources)++;
    }
    return OC_STACK_OK;
#else
    (void)numResources;
    return OC_STACK_NOTIMPL;
#endif // WITH_RESOURCE_API
}

OCResourceHandle OCGetResourceHandle(uint8_t index)
{
#ifdef WITH_RESOURCE_API
    OCResource *pointer = headResource;

    for (uint8_t i = 0; i < index && pointer; ++i)
    {
        pointer = pointer->next;
    }
    return (OCResourceHandle)pointer;
#else
    (void)index;
    return NULL;
#endif // WITH_RESOURCE_API
}

OCStackResult OCDeleteResource(OCResourceHandle handle)
{
#ifdef WITH_RESOURCE_API
    if (!handle)
    {
        OC_LOG(ERROR, TAG, PCF("Invalid handle for deletion"));
        return OC_STACK_INVALID_PARAM;
    }

    OCResource *resource = findResource((OCResource *)handle);
    if (resource == NULL)
    {
        OC_LOG(ERROR, TAG, PCF("Resource not found"));
        return OC_STACK_NO_RESOURCE;
    }
    return deleteResource(resource);
#else
    (void)handle;
    return OC_STACK_NOTIMPL;
#endif // WITH_RESOURCE_API
}

#ifndef WITH_RESOURCE_API
OCStackResult OCAddResources(OCResource *resource)
{
    if (resource && headResource)
    {
        return OC_STACK_INVALID_PARAM;
    }
    headResource = resource;
    return OC_STACK_OK;
}

void OCNotifyPresenceChange(const OCResource *resource, OCPresenceTrigger trigger)
{
#ifdef WITH_PRESENCE
    OCNotifyPresenceChange(resource, trigger);
#else
    (void)resource;
    (void)trigger;
#endif // WITH_PRESENCE
}
#endif // not WITH_RESOURCE_API

const char *OCGetResourceUri(OCResourceHandle handle)
{
#ifdef WITH_RESOURCE_API
    OCResource *resource = findResource((OCResource *)handle);
    if (resource)
    {
        return resource->uri;
    }
    return (const char *) NULL;
#else
    (void)handle;
    return NULL;
#endif // WITH_RESOURCE_API
}

OCResourceProperty OCGetResourceProperties(OCResourceHandle handle)
{
#ifdef WITH_RESOURCE_API
    OCResource *resource = findResource((OCResource *)handle);
    if (resource)
    {
        return resource->resourceProperties;
    }
    return (OCResourceProperty)-1;
#else
    (void)handle;
    return (OCResourceProperty)-1;
#endif // WITH_RESOURCE_API
}

OCStackResult OCGetNumberOfResourceTypes(OCResourceHandle handle,
                                                    uint8_t *numResourceTypes)
{
#ifdef WITH_RESOURCE_API
    VERIFY_NON_NULL(numResourceTypes, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);

    *numResourceTypes = 0;

    OCResource *resource = findResource((OCResource *)handle);
    if (!resource)
    {
        return OC_STACK_OK;
    }
#ifdef RT_MAX
    *numResourceTypes = resource->nTypes;
#else
    for (OCResourceText_t *ptr = resource->rsrcType; ptr; ptr = ptr->next)
    {
        (*numResourceTypes)++;
    }
#endif // RT_MAX
    return OC_STACK_OK;
#else
    (void)handle;
    (void)numResourceTypes;
    return OC_STACK_NOTIMPL;
#endif // WITH_RESOURCE_API
}

const char *OCGetResourceTypeName(OCResourceHandle handle, uint8_t index)
{
#ifdef WITH_RESOURCE_API
    OCResourceText_t *resourceType = findResourceTypeAtIndex(handle, index);
    if (resourceType)
    {
        return resourceType->name;
    }
    return NULL;
#else
    (void)handle;
    (void)index;
    return NULL;
#endif // WITH_RESOURCE_API
}

OCStackResult OCGetNumberOfResourceInterfaces(OCResourceHandle handle,
                                                uint8_t *numResourceInterfaces)
{
#ifdef WITH_RESOURCE_API
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(numResourceInterfaces, ERROR, OC_STACK_INVALID_PARAM);

    *numResourceInterfaces = 0;

    OCResource *resource = findResource((OCResource *) handle);
    if (!resource)
    {
        return OC_STACK_OK;
    }

#ifdef RI_MAX
    *numResourceInterfaces = resource->nInterfaces;
#else
    for (OCResourceText_t *ri = resource->rsrcInterface; ri; ri = ri->next)
    {
        (*numResourceInterfaces)++;
    }
#endif // RI_MAX
    return OC_STACK_OK;
#else
    (void)handle;
    (void)numResourceInterfaces;
    return OC_STACK_NOTIMPL;
#endif // WITH_RESOURCE_API
}

const char *OCGetResourceInterfaceName(OCResourceHandle handle, uint8_t index)
{
#ifdef WITH_RESOURCE_API
    OCResourceText_t *resourceInterface = findResourceInterfaceAtIndex(handle, index);
    if (resourceInterface)
    {
        return resourceInterface->name;
    }
    return NULL;
#else
    (void)handle;
    (void)index;
    return NULL;
#endif // WITH_RESOURCE_API
}

OCResourceHandle OCGetResourceHandleFromCollection(OCResourceHandle collectionHandle,
        uint8_t index)
{
#ifdef WITH_RESOURCE_API
#ifdef WITH_COLLECTIONS
    OCResource *resource = NULL;

    if (index >= MAX_CONTAINED_RESOURCES)
    {
        return NULL;
    }

    resource = findResource((OCResource *) collectionHandle);
    if (!resource)
    {
        return NULL;
    }

    return resource->rsrcResources[index];
#else
    (void)collectionHandle;
    (void)index;
    return NULL;
#endif // WITH_COLLECTIONS
#else
    (void)collectionHandle;
    (void)index;
    return NULL;
#endif // WITH_RESOURCE_API
}

#ifdef WITH_PRESENCE
static OCStackResult SendPresenceNotification(OCPresenceTrigger trigger,
                                                const char *resourceTypeName)
{
    uint8_t payloadBuffer[MAX_ENCODED_PAYLOAD_LENGTH];
    size_t payloadSize = sizeof (payloadBuffer);
    OCStackResult result;
    OCRequestInfo_t request;
    memset(&request, 0, sizeof (request));

    char query[1] = { '\0' };
    OCDevAddr devAddr = { OC_DEFAULT_ADAPTER };

    FillServerRequest(&request,
                      OC_REST_GET,
                      ThePresenceResource.resource.sequenceNum,
                      OC_LOW_QOS,
                      query,
                      ThePresenceResource.token,
                      OC_RSRVD_PRESENCE_URI,
                      &devAddr);
    OCEntityHandlerResponse ehResponse = {0};

    result = OCBuildPresencePayload(payloadBuffer, &payloadSize,
                                    ThePresenceResource.resource.sequenceNum,
                                    ThePresenceResource.presenceTTL,
                                    trigger, resourceTypeName);
    if (result != OC_STACK_OK)
    {
        return result;
    }

    CAMessageInfo_t *messageInfo = (CAMessageInfo_t *)&request;
    messageInfo->encodedPayload = payloadBuffer;
    messageInfo->encodedSize = payloadSize;
    ehResponse.ehResult = OC_EH_OK;
    ehResponse.requestHandle = (OCRequestHandle)&request;
    ehResponse.resourceHandle = (OCResourceHandle)&ThePresenceResource.resource;
    OICStrcpy(ehResponse.resourceUri, sizeof (ehResponse.resourceUri),
                                                        OC_RSRVD_PRESENCE_URI);

    return HandleSingleResponse(&ehResponse);
}

void NotifyPresenceChange(const OCResource *resource, OCPresenceTrigger trigger)
{
    if (!ThePresenceResource.active)
    {
        return;
    }
    ThePresenceResource.resource.sequenceNum = OCGetRandom();
    SendPresenceNotification(trigger,
                             #ifdef RT_MAX
                             resource->rsrcType[0].name);
                             #else
                             resource->rsrcType->name);
                             #endif
}

OCStackResult SendStopNotification()
{
    return SendPresenceNotification(OC_PRESENCE_TRIGGER_DELETE, NULL);
}
#endif // WITH_PRESENCE

OCStackResult OCNotifyAllObservers(OCResourceHandle handle, OCQualityOfService qos)
{
#ifdef WITH_RESOURCE_API
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_ERROR);

    // Verify that the resource exists
    OCResource *resource = findResource((OCResource *)handle);
    if (!resource)
    {
        return OC_STACK_NO_RESOURCE;
    }
    return OCNotifyAllObservers1(resource, qos);
#else
    (void)handle;
    (void)qos;
    return OC_STACK_NOTIMPL;
#endif // WITH_RESOURCE_API
}

void incrementSequenceNumber(OCResource *resource)
{
    (resource->sequenceNum)++;
    if (resource->sequenceNum == MAX_SEQUENCE_NUMBER)
    {
        resource->sequenceNum = OC_OFFSET_SEQUENCE_NUMBER + 1;
    }
}

OCStackResult OCNotifyAllObservers1(OCResource *resource, OCQualityOfService qos)
{
#ifdef WITH_RESOURCE_API
    //only increment in the case of regular observing (not presence)
    incrementSequenceNumber(resource);
    return SendAllObserverNotification(resource, qos);
#else
    (void)resource;
    (void)qos;
    return OC_STACK_ERROR;
#endif // WITH_RESOURCE_API
}

OCStackResult OCNotifyListOfObservers(OCResourceHandle handle,
                                      OCObservationId *obsIdList,
                                      uint8_t numberOfIds,
                                      const OCRepPayload *payload,
                                      OCQualityOfService qos)
{
#ifdef WITH_RESOURCE_API
    VERIFY_NON_NULL(handle, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(obsIdList, ERROR, OC_STACK_ERROR);
    VERIFY_NON_NULL(payload, ERROR, OC_STACK_ERROR);

    OCResource *resource = findResource((OCResource *)handle);
    if (!resource)
    {
        return OC_STACK_NO_RESOURCE;
    }
    return OCNotifyListOfObservers1(resource, obsIdList, numberOfIds,
                                                               payload, qos);
#else
    (void)handle;
    (void)obsIdList;
    (void)numberOfIds;
    (void)payload;
    (void)qos;
    return OC_STACK_NOTIMPL;
#endif // WITH_RESOURCE_API
}

OCStackResult OCNotifyListOfObservers1(OCResource *resource,
                                      OCObservationId *obsIdList,
                                      uint8_t numberOfIds,
                                      const OCRepPayload *payload,
                                      OCQualityOfService qos)
{
#ifdef WITH_RESOURCE_API
    incrementSequenceNumber(resource);

    //TODO: we should allow the server to define this
    uint32_t maxAge = MAX_OBSERVE_AGE;

    return SendListObserverNotification(resource, obsIdList, numberOfIds,
                                        payload, maxAge, qos);
#else
    (void)resource;
    (void)obsIdList;
    (void)numberOfIds;
    (void)payload;
    (void)qos;
    return OC_STACK_NOTIMPL;
#endif // WITH_RESOURCE_API
}

OCStackResult OCDoResponse(OCEntityHandlerResponse *ehResponse)
{
    VERIFY_NON_NULL(ehResponse, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NON_NULL(ehResponse->requestHandle, ERROR, OC_STACK_INVALID_PARAM);

    // Normal response
    OCRequestInfo_t *serverRequest = (OCRequestInfo_t *)ehResponse->requestHandle;
    if (serverRequest->signature != REQUEST_SIGNATURE)
    {
        return OC_STACK_ERROR;
    }
    // Usually HandleSingleResponse
    return serverRequest->ehResponseHandler(ehResponse);
}

//-----------------------------------------------------------------------------
// Private internal function definitions
//-----------------------------------------------------------------------------
#ifdef WITH_PRESENCE
void OCChangeActiveProperty(OCResourceHandle handle, int enable)
{
    OCResource *resource = (OCResource *)handle;
    if (enable)
    {
        resource->resourceProperties |= OC_ACTIVE;
    }
    else
    {
        resource->resourceProperties &= ~OC_ACTIVE;
    }
}
#endif

OCStackResult initResources()
{
    OCStackResult result = OC_STACK_OK;

    headResource = NULL;
    tailResource = NULL;
    // Init Virtual Resources

#ifdef WITH_SECURITY
    if (result == OC_STACK_OK)
    {
        result = SRMInitSecureResources();
    }
#endif

    return result;
}

void insertResource(OCResource *resource)
{
    if (!headResource)
    {
        headResource = resource;
        tailResource = resource;
    }
    else
    {
        tailResource->next = resource;
        tailResource = resource;
    }
    resource->next = NULL;
}

OCResource *findResource(OCResource *resource)
{
    for (OCResource *pointer = headResource; pointer; pointer = pointer->next)
    {
        if (pointer == resource)
        {
            return resource;
        }
    }
    return NULL;
}

void deleteAllResources()
{
#ifdef WITH_SECURITY
    SRMDeInitSecureResources();
#endif

#ifdef WITH_RESOURCE_API
    for (OCResource *temp, *pointer = headResource; pointer; pointer = temp)
    {
        temp = pointer->next;
        deleteResource(pointer);
    }
#endif // WITH_RESOURCE_API

#ifdef WITH_PRESENCE
    // Ensure that the last resource to be deleted is the presence resource. This allows for all
    // presence notification attributed to their deletion to be processed.
    ThePresenceResource.active = 0;
#endif // WITH_PRESENCE

}

OCStackResult deleteResource(OCResource *resource)
{
    OCResource *prev = NULL;
    OCResource *temp;

    for (temp = headResource; temp; prev = temp, temp = temp->next)
    {
        if (temp == resource)
        {
            break;
        }
    }
    if (temp != resource)
    {
        return OC_STACK_ERROR;
    }

    // Invalidate all Resource Properties.
    resource->resourceProperties = (OCResourceProperty)0;
    OCNotifyAllObservers((OCResourceHandle)resource, OC_HIGH_QOS);

    #ifdef WITH_PRESENCE
    NotifyPresenceChange(resource, OC_PRESENCE_TRIGGER_DELETE);
    #endif

#ifdef WITH_RESOURCE_API
    if (temp == headResource && temp == tailResource)
    {
        headResource = NULL;
        tailResource = NULL;
    }
    else if (temp == headResource)
    {
        headResource = temp->next;
    }
    else if (temp == tailResource)
    {
        tailResource = prev;
        tailResource->next = NULL;
    }
    else
    {
        prev->next = temp->next;
    }

    deleteResourceElements(temp);
    OCFree(temp, MAT_resource);
#else
    (void)prev;
#endif // WITH_RESOURCE_API
    return OC_STACK_OK;
}

#ifdef WITH_RESOURCE_API
void deleteResourceElements(OCResource *resource)
{
    (void)resource;
#ifndef RT_MAX
    for (OCResourceText_t *n1, *p1 = resource->rsrcType; p1; p1 = n1)
    {
        n1 = p1->next;
        OICFree(p1);
    }
#endif

#ifndef RI_MAX
    for (OCResourceText_t *n2, *p2 = resource->rsrcInterface; p2; p2 = n2)
    {
        n2 = p2->next;
        OICFree(p2);
    }
#endif
}

static OCStackResult insertResourceType(OCResource *resource, const char *name)
{
    if (strlen(name) > MAX_RESOURCE_TYPE_SIZE)
    {
        return OC_STACK_INVALID_PARAM;
    }
#ifdef RT_MAX
    for (int i = 0; i < resource->nTypes; i++)
    {
        if (!strcmp(name, resource->rsrcType[i].name))
        {
            return OC_STACK_ERROR;
        }
    }
    if (resource->nTypes < RT_MAX)
    {
        OICStrcpy(resource->rsrcType[resource->nTypes++].name, MAX_NAME_LENGTH, name);
    }
#else
    OCResourceText_t *previous = NULL;
    OCResourceText_t *added;

    for (OCResourceText_t *ptr = resource->rsrcType; ptr; ptr = ptr->next)
    {
        if (!strcmp(name, ptr->name))
        {
            return OC_STACK_ERROR;
        }
        previous = ptr;
    }

    added = (OCResourceText_t *)OICCalloc(1, sizeof (OCResourceText_t));
    OICStrcpy(added->name, sizeof (added->name), name);

    if (resource->rsrcType)
    {
        previous->next = added;
    }
    else
    {
        resource->rsrcType = added;
    }
    OC_LOG_V(INFO, TAG, "Added type %s to %s", name, resource->uri);
#endif // RT_MAX

    return OC_STACK_OK;
}

OCResourceText_t *findResourceTypeAtIndex(OCResourceHandle handle, int index)
{
    OCResource *resource = findResource((OCResource *)handle);
    if (!resource)
    {
        return NULL;
    }

#ifdef RT_MAX
    return index < resource->nTypes ? resource->rsrcType + index : NULL;
#else
    OCResourceText_t *ptr = resource->rsrcType;
    for (uint8_t i = 0; i < index && ptr; ++i)
    {
        ptr = ptr->next;
    }
    return ptr;
#endif // RT_MAX
}

/*
 * Insert a new interface into interface linked list only if not already present.
 * If alredy present, 2nd arg is free'd.
 * Default interface will always be first if present.
 */
static OCStackResult insertResourceInterface(OCResource *resource, const char *name)
{
    if (strlen(name) > MAX_RESOURCE_IFACE_SIZE)
    {
        return OC_STACK_INVALID_PARAM;
    }
#ifdef RI_MAX
    for (int i = 0; i < resource->nInterfaces; i++)
    {
        if (!strcmp(name, resource->rsrcInterface[i].name))
        {
            return OC_STACK_ERROR;
        }
    }

    bool isDefault = strcmp(name, OC_RSRVD_INTERFACE_DEFAULT) == 0;

    if (resource->nTypes < RI_MAX)
    {
        int where = resource->nInterfaces++;
        if (isDefault)
        {
            memmove(resource->rsrcInterface + 1, resource->rsrcInterface,
                            resource->nInterfaces * sizeof (OCResourceText_t));
            where = 0;
        }
        OICStrcpy(resource->rsrcInterface[where].name, MAX_NAME_LENGTH, name);
    }
#else
    OCResourceText_t *previous = NULL;
    OCResourceText_t *added;

    for (OCResourceText_t *ptr = resource->rsrcInterface; ptr; ptr = ptr->next)
    {
        if (strcmp(name, ptr->name) == 0)
        {
            return OC_STACK_ERROR;
        }
        previous = ptr;
    }

    bool isDefault = strcmp(name, OC_RSRVD_INTERFACE_DEFAULT) == 0;

    added = (OCResourceText_t *)OICCalloc(1, sizeof (OCResourceText_t));
    OICStrcpy(added->name, sizeof (added->name), name);

    if (resource->rsrcInterface)
    {
        if (isDefault)
        {
            added->next = resource->rsrcInterface;
            resource->rsrcInterface = added;
        }
        else
        {
            previous->next = added;
        }
    }
    else
    {
        resource->rsrcInterface = added;
    }
#endif
    return OC_STACK_OK;
}

OCResourceText_t *findResourceInterfaceAtIndex(OCResourceHandle handle, int index)
{
    OCResource *resource = findResource((OCResource *)handle);
    if (!resource)
    {
        return NULL;
    }

#ifdef RI_MAX
    return index < resource->nInterfaces ? resource->rsrcInterface + index : NULL;
#else
    // Iterate through the list
    OCResourceText_t *ptr = resource->rsrcInterface;
    for (uint8_t i = 0; i < index && ptr; ++i)
    {
        ptr = ptr->next;
    }
    return ptr;
#endif // RI_MAX
}
#endif // WITH_RESOURCE_API

const uint8_t* OCGetServerInstanceID(void)
{
    return sid;
}

#ifdef WITH_RESOURCE_API
void OCEntityFeedback(ResourceObserver *observer, const OCToken_t *token)
{
    OCEntityHandlerRequest ehRequest = {0};

    ehRequest.method = OC_REST_NOMETHOD;
    ehRequest.devAddr = observer->devAddr;
    ehRequest.obsInfo.action = OC_OBSERVE_DEREGISTER;
    ehRequest.obsInfo.obsId = observer->observeId;

    observer->resource->entityHandler(OC_OBSERVE_FLAG, &ehRequest,
                                      observer->resource->ehCallbackParam);
    
    (void)DeleteObserverUsingToken(token);
}
#endif // WITH_RESOURCE_API

bool HandleCAResponsesServer(const CAMessageInfo_t* responseInfo)
{
#ifdef WITH_RESOURCE_API
    int type = responseInfo->info.coapType;
    const OCToken_t *token = &(responseInfo->info.token);

    ResourceObserver *observer = GetObserverUsingToken(token);
    if (!observer)
    {
        return false;   // not meant for server
    }

    switch (responseInfo->info.coapCode)
    {
    case CA_EMPTY:
        if (type == OC_MSG_RESET)
        {
            OC_LOG(INFO, TAG, PCF("This is a RESET"));
            OCEntityFeedback(observer, token);
        }
        else if (type == OC_MSG_ACKNOWLEDGE)
        {
            OC_LOG(INFO, TAG, PCF("This is a pure ACK"));
            observer->forceHighQos = 0;
            observer->failedCommCount = 0;
        }
        break;
    case CA_RETRANSMIT_TIMEOUT:
        OC_LOG(INFO, TAG, PCF("Receiving Time Out for an observer"));
        if (observer->failedCommCount >= MAX_OBSERVER_FAILED_COMM)
        {
            OCEntityFeedback(observer, token);
        }
        else
        {
            observer->failedCommCount++;
        }
        observer->forceHighQos = 1;
        OC_LOG_V(DEBUG, TAG, "Failed count for this observer is %d",observer->failedCommCount);
        break;
    default:
        break;
    }
#else
    (void)responseInfo;
#endif // WITH_RESOURCE_API

    return true;    // meant for server
}
