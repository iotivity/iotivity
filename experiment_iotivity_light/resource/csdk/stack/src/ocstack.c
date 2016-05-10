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
#include <assert.h>

#include "oclogger.h"
#include "occlientcb.h"
#include "ocserverrequest.h"
#include "ocstackinternal.h"
#include "ocresourcehandler.h"
#include "ocobserve.h"
#include "ocrandom.h"
#include "oic_malloc.h"
#include "oic_string.h"
//#include "secureresourcemanager.h"
#include "ocpayload.h"
#include "ocpayloadcbor.h"
#include "ocadaptation.h"
#include "ocglobals.h"
#include "ocstack.h"

#ifdef WITH_ARDUINO
#include "Time.h"
#else
#include <sys/time.h>
#endif
//#include "coap_time.h"
//#include "utlist.h"
//#include "pdu.h"

#ifndef ARDUINO
#include <arpa/inet.h>
#endif

#ifndef UINT32_MAX
#define UINT32_MAX   (0xFFFFFFFFUL)
#endif

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------
typedef enum
{
    OC_STACK_UNINITIALIZED = 0,
    OC_STACK_INITIALIZED,
    OC_STACK_UNINIT_IN_PROGRESS
} OCStackState;

static OCStackState stackState = OC_STACK_UNINITIALIZED;

static OCMode myStackMode;

CAGlobals_t caglobals = { 0 };

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define TAG  PCF("OCStack")

//TODO: we should allow the server to define this
#define MAX_OBSERVE_AGE (0x2FFFFUL)

//-----------------------------------------------------------------------------
// Internal functions
//-----------------------------------------------------------------------------

static OCStackResult InitializeMemoryPools();

static void OCToCATransportFlags(OCDevAddr *devAddr)
{
    OCTransportFlags flags = devAddr->flags;

    // supply default behavior.
    if ((flags & (OC_IP_USE_V6|OC_IP_USE_V4)) == 0)
    {
        flags = (OCTransportFlags)(flags|OC_IP_USE_V6|OC_IP_USE_V4);
    }
    if ((flags & OC_MASK_SCOPE) == 0)
    {
        flags = (OCTransportFlags)(flags|OC_SCOPE_LINK);
    }
    devAddr->flags = flags;
}

#ifdef WITH_PRESENCE
const char *convertTriggerEnumToString(OCPresenceTrigger trigger)
{
    if (trigger == OC_PRESENCE_TRIGGER_CREATE)
    {
        return OC_RSRVD_TRIGGER_CREATE;
    }
    else if (trigger == OC_PRESENCE_TRIGGER_CHANGE)
    {
        return OC_RSRVD_TRIGGER_CHANGE;
    }
    else
    {
        return OC_RSRVD_TRIGGER_DELETE;
    }
}

OCPresenceTrigger convertTriggerStringToEnum(const char * triggerStr)
{
    if(!triggerStr)
    {
        return OC_PRESENCE_TRIGGER_CREATE;
    }
    else if(strcmp(triggerStr, OC_RSRVD_TRIGGER_CREATE) == 0)
    {
        return OC_PRESENCE_TRIGGER_CREATE;
    }
    else if(strcmp(triggerStr, OC_RSRVD_TRIGGER_CHANGE) == 0)
    {
        return OC_PRESENCE_TRIGGER_CHANGE;
    }
    else
    {
        return OC_PRESENCE_TRIGGER_DELETE;
    }
}
#endif // WITH_PRESENCE

/*
 * This function handles error response from CA
 * code shall be added to handle the errors
 */
void HandleCAErrorResponse(const CAMessageInfo_t *errorInfo)
{
    (void)errorInfo;
}

/*
 * This function sends out Direct Stack Responses. These are responses that are not coming
 * from the application entity handler. These responses have no payload and are usually ACKs,
 * RESETs or some error conditions that were caught by the stack.
 */
OCStackResult SendDirectStackResponse(const CAMessageInfo_t *message,
                                      const OCMessageType_t coapType)
{
    CAMessageInfo_t respInfo = { .type = OC_RESPONSE_DATA };

    respInfo.info.coapCode = CA_EMPTY;
    respInfo.info.coapType = coapType;
    respInfo.info.messageId = message->info.messageId;
    respInfo.devAddr = message->devAddr;

    OCSendMessage(&respInfo);
    return OC_STACK_OK;
}

#ifdef RA_ADAPTER
OCStackResult OCSetRAInfo(const OCRAInfo_t *raInfo)
{
    if (!raInfo           ||
        !raInfo->username ||
        !raInfo->hostname ||
        !raInfo->xmpp_domain)
    {

        return OC_STACK_INVALID_PARAM;
    }
    OCStackResult result = CAResultToOCResult(CASetRAInfo((const CARAInfo_t *) raInfo));
    gRASetInfo = (result == OC_STACK_OK)? true : false;

    return result;
}
#endif

OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
{
    (void) ipAddr;
    (void) port;
    return OCInit1(mode, OC_DEFAULT_FLAGS, OC_DEFAULT_FLAGS);
}

OCStackResult OCInit1(OCMode mode, OCTransportFlags serverFlags, OCTransportFlags clientFlags)
{
    if(stackState == OC_STACK_INITIALIZED)
    {
        OC_LOG(INFO, TAG, PCF("Subsequent calls to OCInit() without calling \
                OCStop() between them are ignored."));
        return OC_STACK_OK;
    }

#ifdef RA_ADAPTER
    if(!gRASetInfo)
    {
        OC_LOG(ERROR, TAG, PCF("Need to call OCSetRAInfo before calling OCInit"));
        return OC_STACK_ERROR;
    }
#endif

    OC_LOG(INFO, TAG, PCF("Entering OCInit"));

    // Validate mode
    if (!((mode == OC_CLIENT) || (mode == OC_SERVER) || (mode == OC_CLIENT_SERVER)))
    {
        OC_LOG(ERROR, TAG, PCF("Invalid mode"));
        return OC_STACK_ERROR;
    }
    myStackMode = mode;

    if (mode == OC_CLIENT || mode == OC_CLIENT_SERVER)
    {
        caglobals.client = true;
    }
    if (mode == OC_SERVER || mode == OC_CLIENT_SERVER)
    {
        caglobals.server = true;
    }

    caglobals.serverFlags = serverFlags;
    if (!(caglobals.serverFlags & OC_IPFAMILY_MASK))
    {
        caglobals.serverFlags = (OCTransportFlags)(caglobals.serverFlags|OC_IP_USE_V6|OC_IP_USE_V4);
    }
    caglobals.clientFlags = clientFlags;
    if (!(caglobals.clientFlags & OC_IPFAMILY_MASK))
    {
        caglobals.clientFlags = (OCTransportFlags)(caglobals.clientFlags|OC_IP_USE_V6|OC_IP_USE_V4);
    }

    OCSeedRandom();

    if (InitializeMemoryPools() != OC_STACK_OK)
        goto exit;

    if (CAInitialize() != OC_STACK_OK)
        goto exit;
    OC_LOG(ERROR, TAG, PCF("network initialized"));

#ifdef WITH_SECURITY
	SRMRegisterHandler(HandleCARequests, HandleCAResponses, HandleCAErrorResponse);
#endif // WITH_SECURITY

    if (caglobals.server && OC_STACK_OK != OCInitServer())
        goto exit;
    OC_LOG(ERROR, TAG, PCF("server initialized"));
#ifdef WITH_CLIENT
    if (caglobals.client && OC_STACK_OK != OCInitClient())
        goto exit;
#endif
    OC_LOG(ERROR, TAG, PCF("client initialized"));
#ifdef WITH_SECURITY
    if (OC_STACK_OK != SRMInitPolicyEngine())
        goto exit;
#endif // WITH_SECURITY

    stackState = OC_STACK_INITIALIZED;
    return OC_STACK_OK;

exit:
    OC_LOG(ERROR, TAG, PCF("initialization failed"));
#ifdef WITH_CLIENT
    OCStopClient();
#endif
    OCStopServer();
    CATerminate();
    return OC_STACK_UNINITIALIZED;
}

OCStackResult OCStop()
{
    OC_LOG(INFO, TAG, PCF("Entering OCStop"));

    if (stackState == OC_STACK_UNINIT_IN_PROGRESS)
    {
        OC_LOG(DEBUG, TAG, PCF("Stack already stopping, exiting"));
        return OC_STACK_OK;
    }
    else if (stackState != OC_STACK_INITIALIZED)
    {
        OC_LOG(ERROR, TAG, PCF("Stack not initialized"));
        return OC_STACK_ERROR;
    }

    stackState = OC_STACK_UNINIT_IN_PROGRESS;

#ifdef WITH_CLIENT
    OCStopClient();
#endif
    OCStopServer();

    CATerminate();
#ifdef WITH_RESOURCE_API
    // Remove all observers
    DeleteObserverList();
#endif
#ifdef WITH_CLIENT
    // Remove all the client callbacks
    DeleteClientCBList();
#endif
#ifdef WITH_SECURITY
	// De-init the SRM Policy Engine
    // TODO after BeachHead delivery: consolidate into single SRMDeInit()
    SRMDeInitPolicyEngine();
#endif // WITH_SECURITY

    stackState = OC_STACK_UNINITIALIZED;
    return OC_STACK_OK;
}

/**
 * @brief   Register Persistent storage callback.
 * @param   persistentStorageHandler [IN] Pointers to open, read, write, close & unlink handlers.
 * @return
 *     OC_STACK_OK    - No errors; Success
 *     OC_STACK_INVALID_PARAM - Invalid parameter
 */
OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
{
#ifdef WITH_SECURITY
    if (!persistentStorageHandler)
    {
        OC_LOG(ERROR, TAG, PCF("The persistent storage handler is invalid"));
        return OC_STACK_INVALID_PARAM;
    }
    else
    {
        if (!persistentStorageHandler->open ||
            !persistentStorageHandler->close ||
            !persistentStorageHandler->read ||
            !persistentStorageHandler->unlink ||
            !persistentStorageHandler->write)
        {
            OC_LOG(ERROR, TAG, PCF("The persistent storage handler is invalid"));
            return OC_STACK_INVALID_PARAM;
        }
    }
    return SRMRegisterPersistentStorageHandler(persistentStorageHandler);
#else
    (void)persistentStorageHandler;
    return OC_STACK_OK;
#endif // WITH_SECURITY
}

OCStackResult OCProcess()
{
    #ifdef WITH_CLIENT
    #ifdef WITH_PRESENCE
    OCProcessPresence();
    #endif
    #endif
    CAHandleRequestResponse();

    return OC_STACK_OK;
}

void HandleCAResponses(const CAMessageInfo_t* responseInfo)
{
    bool handledbyclient = false;
    bool handledbyserver = false;

#ifdef WITH_CLIENT
    if (caglobals.client)
    {
        handledbyclient = HandleCAResponsesClient(responseInfo);
    }
#endif
    if (caglobals.server)
    {
        handledbyserver = HandleCAResponsesServer(responseInfo);
    }

    if (!handledbyclient && !handledbyserver)
    {
        OC_LOG(INFO, TAG, PCF("response not claimed by client or server"));
        if (responseInfo->info.coapCode == CA_EMPTY)
        {
            OC_LOG(INFO, TAG, PCF("Receiving CA_EMPTY in the ocstack"));
        }
        else
        {
            OC_LOG(INFO, TAG, PCF("Received a message without callbacks. Sending RESET"));
            SendDirectStackResponse(responseInfo, OC_MSG_RESET);
        }
    }
}

void OCSendMessage(CAMessageInfo_t *messageInfo)
{
    OCToCATransportFlags(&(messageInfo->devAddr));
    CASendPDU(messageInfo);
}

void OCHandleIncoming(CAMessageInfo_t *messageInfo)
{
    switch (messageInfo->type)
    {
#ifdef WITH_SECURITY
    case OC_REQUEST_DATA:
        SRMRequestHandler(messageInfo);
        break;
    case OC_RESPONSE_DATA:
        SRMResponseHandler(messageInfo);
        break;
    case OC_ERROR_DATA:
        SRMErrorHandler(messageInfo);
        break;
#else // not WITH_SECURITY
    case OC_REQUEST_DATA:
        HandleCARequests(messageInfo);
        break;
    case OC_RESPONSE_DATA:
        HandleCAResponses(messageInfo);
        break;
    case OC_ERROR_DATA:
        HandleCAErrorResponse(messageInfo);
        break;
#endif // WITH_SECURITY
    }
}

/*
 * memory pools handling
 */
#ifdef WITH_DEBUG
#define Z 0,0,0,0,0
#else
#define Z 0,0,0,0
#endif
static MAEntry_t allocTable[] = 
{
    { MAT_message, MAS_message, MAD_message, MAB_message, MAX_message, Z },
    { MAT_payload, MAS_payload, MAD_payload, MAB_payload, MAX_payload, Z },
    { MAT_resource, MAS_resource, MAD_resource, MAB_resource, MAX_resource, Z },
    { MAT_observer, MAS_observer, MAD_observer, MAB_observer, MAX_observer, Z },
    { MAT_collection, MAS_collection, MAD_collection, MAB_collection, MAX_collection, Z },
    { MAT_entity, MAS_entity, MAD_entity, MAB_entity, MAX_entity, Z },
    { MAT_PDUs, MAS_PDUs, MAD_PDUs, MAB_PDUs, MAX_PDUs, Z }
};
#undef Z

static int currentMemoryPoolSize = 0;
static int maximumMemoryPoolSize = 0;

static void finishMemoryEntry(int type)
{
    MAEntry_t *entry = allocTable + type;

    int share = entry->share;
    if (share == MAT_none)
    {
        return;
    }
    if (allocTable[share].size < entry->size)
    {
        allocTable[share].size = entry->size;
    }
    allocTable[share].minAllocs += entry->minAllocs;
    allocTable[share].maxAllocs += entry->maxAllocs;
}

static OCStackResult fillMemoryEntry(int type)
{
    MAEntry_t *entry = allocTable + type;

    if (entry->share != MAT_none)
    {
        return OC_STACK_OK;
    }
    int minAllocs = entry->minAllocs;
    int maxAllocs = entry->maxAllocs;

    if (maxAllocs > 0)
    {
        entry->availableBuffers = OICCalloc(maxAllocs, sizeof (void *));
        if (!entry->availableBuffers)
        {
            return OC_STACK_NO_MEMORY;
        }
        #ifdef WITH_DEBUG
        entry->allocatedBuffers = OICCalloc(maxAllocs, sizeof (void *));
        if (!entry->allocatedBuffers)
        {
            return OC_STACK_NO_MEMORY;
        }
        #endif
        for (int i = 0; i < minAllocs; i++)
        {
            entry->availableBuffers[i] = OICMalloc(entry->size);
            if (!entry->availableBuffers[i])
            {
                return OC_STACK_NO_MEMORY;
            }
            #ifdef WITH_DEBUG
            entry->allocatedBuffers[i] = entry->availableBuffers[i];
            #endif
        }
        currentMemoryPoolSize += minAllocs * entry->size;
        maximumMemoryPoolSize += entry->maxAllocs * entry->size;
    }

    entry->nBuffers = entry->minAllocs;
    entry->nAvail = entry->nBuffers;
    return OC_STACK_OK;
}

static OCStackResult InitializeMemoryPools()
{
    for (int i = 0; i < MAT_count; i++)
    {
        finishMemoryEntry(i);
    }
    for (int i = 0; i < MAT_count; i++)
    {
        OCStackResult res = fillMemoryEntry(i);
        if (res != OC_STACK_OK)
        {
            return res;
        }
    }

    OC_LOG_V(INFO, TAG, PCF("MAD_message: %d"), MAD_message);
    OC_LOG_V(INFO, TAG, PCF("MAD_payload: %d"), MAD_payload);
    OC_LOG_V(INFO, TAG, PCF("MAD_resource: %d"), MAD_resource);
    OC_LOG_V(INFO, TAG, PCF("MAD_observer: %d"), MAD_observer);
    OC_LOG_V(INFO, TAG, PCF("MAD_collection: %d"), MAD_collection);
    OC_LOG_V(INFO, TAG, PCF("MAD_entity: %d"), MAD_entity);
    OC_LOG_V(INFO, TAG, PCF("MAD_PDUs: %d"), MAD_PDUs);

    OC_LOG_V(INFO, TAG, PCF("memory pool size: initial %d, maximum %d"), currentMemoryPoolSize, maximumMemoryPoolSize);

    return OC_STACK_OK;
}

void *OCMalloc(MAType_t type)
{
    void *buf;
    MAEntry_t *entry = allocTable + type;

    for (int i = 0; i < entry->maxAllocs; i++)
    {
        buf = entry->availableBuffers[i];
        if (buf)
        {
            entry->availableBuffers[i] = NULL;
            entry->nAvail--;
            return buf;
        }
    }
    if (entry->nBuffers < entry->maxAllocs)
    {
        buf = OICMalloc(entry->size);
        if (buf)
        {
            #ifdef WITH_DEBUG
            entry->allocatedBuffers[entry->nBuffers] = buf;
            #endif
            entry->nBuffers++;
            currentMemoryPoolSize += entry->size;
            OC_LOG_V(INFO, TAG, PCF("memory pool %d: %d buffers"), entry->type, entry->nBuffers);
            return buf;
        }
    }
    OC_LOG(ERROR, TAG, PCF("MEMORY ALLOCATION OVERRUN FAILURE"));
    return NULL;        // needs robust response to allocation failure
}

void *OCCalloc(MAType_t type)
{
    void *buf = OCMalloc(type);
    if (buf)
    {
        MAEntry_t *entry = allocTable + type;
        memset(buf, '\0', entry->size);
    }
    return buf;
}

void OCFree(void *buf, MAType_t type)
{
    if (!buf)
    {
        return;
    }
    MAEntry_t *entry;
    #ifdef WITH_DEBUG
    if (type < 0 || type >= MAT_count)
    {
        OC_LOG(ERROR, TAG, PCF("MEMORY FREE CALL FAILURE"));
        return;
    }
    entry = allocTable + type;
    bool found = false;
    for (int i = 0; i < entry->maxAllocs; i++)
    {
        if (buf == entry->allocatedBuffers[i])
        {
            found = true;
            break;
        }
    }
    if (!found)
    {
        OC_LOG(ERROR, TAG, PCF("MEMORY FREE FIND FAILURE"));
        assert(false);
    }
    #endif
    entry = allocTable + type;
    for (int i = 0; i < entry->maxAllocs; i++)
    {
        if (!entry->availableBuffers[i])
        {
            entry->availableBuffers[i] = buf;
            entry->nAvail++;
            return;
        }
    }
    OC_LOG(ERROR, TAG, PCF("MEMORY FREE USAGE FAILURE"));
    // drop buf on the floor.  needs robust response to free failure.
}
