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

#include "NSProviderResource.h"

const char* NSType = "oic.r.notification";
const char* NSMessageType = "oic.r.notification.message";
const char* NSSyncType = "oic.r.notification.sync";

const char* NSInterface = "oic.if.baseline";
const char* NSMessgeInterface = "oic.if.baseline.message";
const char* NSSyncInterface = "oic.if.baseline.sync";

const char* NSUri = "/notification";
const char* NSMessageUri = "/notification/message";
const char* NSSyncUri = "/notification/sync";

/* Structure to represent notification resources */
typedef struct
{
    OCResourceHandle handle;
    int accepter;
    char* message_uri;
    char* sync_uri;
} NSNotificationResource;

typedef struct
{
    OCResourceHandle handle;
    char* id;
    char* title;
    char* body;
} NSMessageResource;

typedef struct
{
    OCResourceHandle handle;
    char* id;
    char* state;
    NSDevice device;
} NSSyncResource;

NSNotificationResource NotificationResource;
NSMessageResource NotificationMessageResource;
NSSyncResource NotificationSyncResource;

NSResult NSCreateResource(char *uri)
{
    if (!uri)
    {
        OIC_LOG(ERROR, RESOURCE_TAG, "Resource URI cannot be NULL");
        return NS_ERROR;
    }

    if (strcmp(uri, NSUri) == 0)
    {

        NotificationResource.accepter = 0;
        NotificationResource.message_uri = NSMessageUri;
        NotificationResource.sync_uri = NSSyncUri;
        NotificationResource.handle = NULL;

        if (OCCreateResource(&NotificationResource.handle, NSType, NSInterface, NSUri,
                NSEntityHandlerNotificationCb, NULL, OC_DISCOVERABLE) != OC_STACK_OK)
        {
            OIC_LOG(ERROR, RESOURCE_TAG, PCF("Fail to Create Notification Resource"));
            return NS_ERROR;
        }

        printf("NotificationResource.handle = %u\n", NotificationResource.handle);


    }
    else if (strcmp(uri, NSMessageUri) == 0)
    {

        NotificationMessageResource.id = NULL;
        NotificationMessageResource.title = NULL;
        NotificationMessageResource.body = NULL;
        NotificationMessageResource.handle = NULL;

        if (OCCreateResource(&NotificationMessageResource.handle, NSMessageType, NSInterface,
                NSMessageUri, NSEntityHandlerMessageCb, NULL, OC_OBSERVABLE) != OC_STACK_OK)
        {
            OIC_LOG(ERROR, RESOURCE_TAG, PCF("Fail to Create Notification Message Resource"));
            return NS_ERROR;
        }

        printf("NotificationMessageResource.handle = %u\n", NotificationMessageResource.handle);


    }
    else if (strcmp(uri, NSSyncUri) == 0)
    {
        NotificationSyncResource.id = NULL;
        NotificationSyncResource.state = NULL;
        NotificationSyncResource.device;
        NotificationSyncResource.handle = NULL;

        if (OCCreateResource(&(NotificationSyncResource.handle), NSSyncType, NSInterface, NSSyncUri,
                NSEntityHandlerSyncCb, NULL, OC_OBSERVABLE) != OC_STACK_OK)
        {
            OIC_LOG(ERROR, RESOURCE_TAG, PCF("Fail to Create Notification Sync Resource"));
            return NS_ERROR;
        }

        printf("NotificationSyncResource.handle = %u\n", NotificationSyncResource.handle);

    }
    else
    {
        OIC_LOG(ERROR, RESOURCE_TAG, PCF("Fail to create resource with invalid URI"));
        return NS_ERROR;
    }

    return NS_OK;
}

NSResult NSRegisterResource()
{
    OIC_LOG(INFO, RESOURCE_TAG, "NSRegisterResource");

    if (NSCreateResource(NSSyncUri) != NS_OK)
    {
        OIC_LOG(ERROR, RESOURCE_TAG, PCF("Fail to register Sync Resource"));
        return NS_ERROR;
    }

    if (NSCreateResource(NSMessageUri) != NS_OK)
    {
        OIC_LOG(ERROR, RESOURCE_TAG, PCF("Fail to register Message Resource"));
        return NS_ERROR;
    }

    if (NSCreateResource(NSUri) != NS_OK)
    {
        OIC_LOG(ERROR, RESOURCE_TAG, PCF("Fail to register Notification Resource"));
        return NS_ERROR;
    }

    return NS_OK;
}

NSResult NSPutNotificationResource(int accepter, OCResourceHandle * handle)
{
    NotificationResource.accepter = accepter;
    NotificationResource.message_uri = NSMessageUri;
    NotificationResource.sync_uri = NSSyncUri;

    *handle = NotificationResource.handle;

    return NS_OK;
}

NSResult NSPutMessageResource(NSMessage *msg, OCResourceHandle * handle)
{
    OIC_LOG(INFO, RESOURCE_TAG, "Put notification message to Resource");

    if(msg != NULL)
    {
        printf("msg is not null\n");
        NotificationMessageResource.id = strdup(msg->mId);
        NotificationMessageResource.title = strdup(msg->mTitle);
        NotificationMessageResource.body = strdup(msg->mContentText);
    }
    else
    {
        printf("msg is null\n");
    }

    *handle = NotificationMessageResource.handle;

    return NS_OK;
}

NSResult NSPutSyncResource(NSSync *sync, OCResourceHandle * handle)
{
    OIC_LOG(INFO, RESOURCE_TAG, "Put notification sync to Resource");

    *handle = NotificationSyncResource.handle;

    return NS_OK;
}

const char* NSGetNotificationUri()
{
    return NSUri;
}

const char* NSGetNotificationMessageUri()
{
    return NSMessageUri;
}

const char* NSGetNotificationSyncUri()
{
    return NSSyncUri;
}

NSResult NSCopyString(char** targetString, const char* sourceString)
{
    if (sourceString)
    {
        *targetString = (char *) malloc(strlen(sourceString) + 1);

        if (*targetString)
        {
            strncpy(*targetString, sourceString, (strlen(sourceString) + 1));
            return NS_SUCCESS;
        }
    }

    return NS_FAIL;
}
