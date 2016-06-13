//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "NSUtil.h"

OCEntityHandlerRequest *NSCopyOCEntityHandlerRequest(OCEntityHandlerRequest *entityHandlerRequest)
{
    NS_LOG(DEBUG, "NSCopyOCEntityHandlerRequest - IN");

    OCEntityHandlerRequest *copyOfRequest =
            (OCEntityHandlerRequest *)OICMalloc(sizeof(OCEntityHandlerRequest));

    if (copyOfRequest)
    {
        // Do shallow copy
        memcpy(copyOfRequest, entityHandlerRequest, sizeof(OCEntityHandlerRequest));


        if (copyOfRequest->query)
        {
            copyOfRequest->query = OICStrdup(entityHandlerRequest->query);
            if(!copyOfRequest->query)
            {
                NS_LOG(ERROR, "Copy failed due to allocation failure");
                OICFree(copyOfRequest);
                return NULL;
            }
        }

        if (entityHandlerRequest->payload)
        {
            copyOfRequest->payload = (OCPayload *)
                    (OCRepPayloadClone ((OCRepPayload*) entityHandlerRequest->payload));
        }

        // Ignore vendor specific header options for example
        copyOfRequest->numRcvdVendorSpecificHeaderOptions = 0;
        copyOfRequest->rcvdVendorSpecificHeaderOptions = NULL;
    }

    if (copyOfRequest)
    {
        NS_LOG(DEBUG, "Copied client request");
    }
    else
    {
        NS_LOG(DEBUG, "Error copying client request");
    }

    NS_LOG(DEBUG, "NSCopyOCEntityHandlerRequest - OUT");

    return copyOfRequest;
}

NSResult NSFreeOCEntityHandlerRequest(OCEntityHandlerRequest * entityHandlerRequest)
{
    NS_LOG(DEBUG, "NSFreeOCEntityHandlerRequest - IN");

    OICFree(entityHandlerRequest->query);
    OCPayloadDestroy(entityHandlerRequest->payload);
    OICFree(entityHandlerRequest);

    NS_LOG(DEBUG, "NSFreeOCEntityHandlerRequest - OUT");

    return NS_OK;
}

NSResult NSFreeMessage(NSMessage * obj)
{
    if (!obj)
    {
        return NS_ERROR;
    }

    if (obj->messageId)
    {
        obj->messageId = 0;
    }

    if (obj->title)
    {
        OICFree(obj->title);
        obj->title = NULL;
    }

    if (obj->contentText)
    {
        OICFree(obj->contentText);
        obj->contentText = NULL;
    }

    if (obj->sourceName)
    {
        OICFree(obj->sourceName);
        obj->sourceName = NULL;
    }

    OICFree(obj);

    return NS_OK;
}

NSMessage * NSDuplicateMessage(NSMessage * copyMsg)
{
    NSMessage * newMsg = NULL;

    if(copyMsg == NULL)
    {
        NS_LOG(ERROR, "Copy Msg is NULL");
        return NULL;
    }

    newMsg = (NSMessage *)OICMalloc(sizeof(NSMessage));
    newMsg->contentText = NULL;
    newMsg->messageId = 0;
    newMsg->sourceName = NULL;
    newMsg->title = NULL;

    if (copyMsg->messageId)
    {
        newMsg->messageId = copyMsg->messageId;
    }

    if (copyMsg->title)
    {
        newMsg->title = OICStrdup(copyMsg->title);
    }

    if (copyMsg->contentText)
    {
        newMsg->contentText = OICStrdup(copyMsg->contentText);
    }

    if (copyMsg->sourceName)
    {
       newMsg->sourceName = OICStrdup(copyMsg->sourceName);
    }

    return newMsg;
}

NSResult NSFreeSync(NSSyncInfo * obj)
{
    if (!obj)
    {
        return NS_ERROR;
    }

    if (obj->messageId)
    {
        obj->messageId = 0;
    }

    if (obj->providerId)
    {
        NS_LOG_V(DEBUG, "obj->mSourceid = %s", obj->providerId);
        OICFree(obj->providerId);
        obj->providerId = NULL;
    }

    OICFree(obj);

    return NS_OK;
}

NSSyncInfo* NSDuplicateSync(NSSyncInfo * copyMsg)
{
    NSSyncInfo * newMsg = NULL;

    if(copyMsg == NULL)
    {
        NS_LOG(ERROR, "Copy Msg is NULL");
        return NULL;
    }

    newMsg = (NSSyncInfo *)OICMalloc(sizeof(NSSyncInfo));
    newMsg->messageId = 0;
    newMsg->providerId = NULL;
    newMsg->state = -1;

    if (copyMsg->messageId)
    {
        newMsg->messageId = copyMsg->messageId;
    }

    if (copyMsg->providerId)
    {
        newMsg->providerId = OICStrdup(copyMsg->providerId);
    }

    newMsg->state = copyMsg->state;

    return newMsg;
}

NSResult NSFreeConsumer(NSConsumer * obj)
{
    if (!obj)
    {
        return NS_ERROR;
    }

    if (obj->mAddress)
    {
        OICFree(obj->mAddress);
        obj->mAddress = NULL;
    }

    if (obj->mDeviceId)
    {
        OICFree(obj->mDeviceId);
        obj->mDeviceId = NULL;
    }

    OICFree(obj);

    return NS_OK;
}

NSConsumer* NSDuplicateConsumer(NSConsumer * copyMsg)
{
    NSConsumer * newMsg = NULL;

    if(copyMsg == NULL)
    {
        NS_LOG(ERROR, "Copy Msg is NULL");
        return NULL;
    }

    newMsg = (NSConsumer *)OICMalloc(sizeof(NSConsumer));
    newMsg->mAddress = NULL;
    newMsg->mDeviceId = NULL;

    if (copyMsg->mAddress)
    {
        newMsg->mAddress = OICStrdup(copyMsg->mAddress);
    }

    if (copyMsg->mDeviceId)
    {
        newMsg->mDeviceId = OICStrdup(copyMsg->mDeviceId);
    }

    return newMsg;
}

void NSDuplicateSetPropertyString(OCRepPayload** msgPayload, const char * name,
        const char * copyString)
{
    if(copyString)
    {
        OCRepPayloadSetPropString(*msgPayload, name, copyString);
    }
    else
    {
        OCRepPayloadSetNull(*msgPayload, name);
    }
}


NSSyncInfo * NSGetSyncInfo(OCPayload * payload)
{
    NS_LOG(DEBUG, "NSBuildOICNotificationSync - IN");

    if(!payload)
    {
        return NULL;
    }
    NSSyncInfo * retSync = (NSSyncInfo *)OICMalloc(sizeof(NSSyncInfo));
    if (!retSync)
    {
        return NULL;
    }

    retSync->messageId = 0;
    retSync->state = NS_SYNC_READ;

    OCRepPayload * repPayload = (OCRepPayload *)payload;
    if (!OCRepPayloadGetPropInt(repPayload, NS_ATTRIBUTE_MESSAGE_ID, (int64_t *)&retSync->messageId))
    {
        OICFree(retSync);
        return NULL;
    }
    if (!OCRepPayloadGetPropString(repPayload, NS_ATTRIBUTE_PROVIDER_ID, &retSync->providerId))
    {
        OICFree(retSync);
        return NULL;
    }
    int64_t state;
    if (!OCRepPayloadGetPropInt(repPayload, NS_ATTRIBUTE_STATE, &state))
    {
        OICFree(retSync);
        return NULL;
    }

    retSync->state = (NSSyncType) state;

    NS_LOG_V(DEBUG, "Sync ID : %ld", retSync->messageId);
    NS_LOG_V(DEBUG, "Sync State : %d", (int) retSync->state);

    NS_LOG(DEBUG, "NSBuildOICNotificationSync - OUT");

    return retSync;
}
