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

NSResult NSFreeMessage(NSMessage * obj)
{
    if (!obj)
    {
        return NS_ERROR;
    }

    if (obj->mId)
    {
        OICFree(obj->mId);
        obj->mId = NULL;
    }

    if (obj->mTitle)
    {
        OICFree(obj->mTitle);
        obj->mTitle = NULL;
    }

    if (obj->mContentText)
    {
        OICFree(obj->mContentText);
        obj->mContentText = NULL;
    }

    if (obj->mSource)
    {
        OICFree(obj->mSource);
        obj->mSource = NULL;
    }

    OICFree(obj);

    return NS_OK;
}

NSMessage * NSDuplicateMessage(NSMessage * copyMsg)
{
    if(copyMsg == NULL)
    {
        NS_LOG(ERROR, "Copy Msg is NULL");
        return NULL;
    }

    NSMessage * newMsg = (NSMessage *)OICMalloc(sizeof(NSMessage));

    if (!copyMsg->mId)
    {
        newMsg->mId = OICStrdup(copyMsg->mId);
    }

    if (!copyMsg->mTitle)
    {
        newMsg->mTitle = OICStrdup(copyMsg->mTitle);
    }

    if (!copyMsg->mContentText)
    {
        newMsg->mContentText = OICStrdup(copyMsg->mContentText);
    }

    if (!copyMsg->mSource)
    {
        newMsg->mSource = OICStrdup(copyMsg->mSource);
    }

    return newMsg;
}

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


