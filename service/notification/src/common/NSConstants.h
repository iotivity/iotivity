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

#ifndef _NS_CONSTANTS_H_
#define _NS_CONSTANTS_H_

#define NS_TAG "IOT_NOTI"

// SCHEDULE //
#define THREAD_COUNT 4

// NOTIOBJ // 
#define NOTIOBJ_TITLE_KEY "title"
#define NOTIOBJ_ID_KEY "id"
#define NOTOOBJ_CONTENT_KEY "contentText"

#define DISCOVERY_TAG "NS_PROVIDER_DISCOVERY"
#define SUBSCRIPTION_TAG "NS_PROVIDER_SUBSCRIPTION"
#define INTERFACE_TAG "NS_PROVIDER_INTERFACE"
#define NOTIFICATION_TAG "NS_PROVIDER_NOTIFICATION"
#define SCHEDULER_TAG "NS_PROVIDER_SCHEDULER"
#define LISTENER_TAG "NS_PROVIDER_LISTENER"
#define RESOURCE_TAG "NS_PROVIDER_RESOURCE"

typedef enum eConnectionState
{
    DISCONNECTED = 0,
    CONNECTED = 1,

} NSConnectionState;

typedef enum eSchedulerType
{
    RESPONSE_SCHEDULER = 0,
    DISCOVERY_SCHEDULER = 1,
    SUBSCRIPTION_SCHEDULER = 2,
    NOTIFICATION_SCHEDULER = 3,
} NSSchedulerType;


typedef enum eTaskType
{
    TASK_REGISTER_RESOURCE = 1000,

    TASK_START_PRESENCE = 2000,
    TASK_STOP_PRESENCE = 2001,

    TASK_RECV_SUBSCRIPTION = 3000,
    TASK_RECV_UNSUBSCRIPTION = 3001,
    TASK_SEND_POLICY = 3002,
    TASK_SEND_ALLOW = 3003,
    TASK_SEND_DENY = 3004,
    TASK_SYNC_SUBSCRIPTION = 3005,

    TASK_SEND_NOTIFICATION = 4000,
    TASK_SEND_PENDING_NOTI = 4001,

    TASK_RECV_READ = 5000,
    TASK_RECV_DISMISS = 5001,
    TASK_SEND_READ = 5002,
    TASK_SEND_DISMISS = 5003,

    TASK_CONSUMER_REQ_DISCOVER = 8001,
    TASK_CONSUMER_REQ_SUBSCRIBE = 8002,
    TASK_CONSUMER_REQ_SUBSCRIBE_CANCEL = 8003,
    TASK_CONSUMER_RECV_NOTIFICATION = 8101,

    TASK_EVENT_CONNECTED = 9000,
    TASK_EVENT_DISCONNECTED = 9001,

    TASK_CB_SUBSCRIPTION = 10000,
    TASK_CB_SYNC = 10001,

} NSTaskType;

typedef enum eCache
{
    NS_CONSUMER_BLACKLIST = 0,
    NS_CONSUMER_WHITELIST = 1,

} NSCache;

#endif /* _NS_CONSTANTS_H_ */
