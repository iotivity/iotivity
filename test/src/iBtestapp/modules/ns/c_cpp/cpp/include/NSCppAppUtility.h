/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#ifndef _NS_CPP_APP_UTILITY_H_
#define _NS_CPP_APP_UTILITY_H_

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include "NSCommon.h"
#include "NSProviderService.h"
#include "NSConsumerService.h"
#include "NSAcceptedConsumers.h"
#include "NSUtils.h"
#include "NSSyncInfo.h"
#include "NSTopicsList.h"
#include "OCPlatform.h"
#include "octypes.h"
#include "pthread.h"
#include "oic_string.h"
#include "oic_malloc.h"
#include "ocstack.h"

using namespace std;
using namespace OIC::Service;

#define TOPIC_1 "TESTAPP_Topic_1"
#define TOPIC_2 "TESTAPP_Topic_2"
#define TOPIC_3 "TESTAPP_Topic_3"
#define TOPIC_4 "TESTAPP_Topic_4"

typedef enum
{
    PROVIDER_EXIT,
    START_PROVIDER = 101,
    START_PROVIDER_AUTO_ACCEPT,
    STOP_PROVIDER,
    ACCEPT_SUBSCRIPTION,
    REJECT_SUBSCRIPTION,
    SEND_NOTIFICATION,
    PROVIDER_SEND_SYNC_INFO,
    ADD_TOPIC,
    DELETE_TOPIC,
    SELECT_TOPIC,
    UNSELECT_TOPIC,
    CONSUMER_TOPICS,
    PROVIDER_TOPICS,
    ACCEPTED_CONSUMER_LIST,
    PROVIDER_ENABLE_REMOTE_SERVICE,
    PROVIDER_DISABLE_REMOTE_SERVICE
} ProviderCppAppMenu;

typedef enum
{
    CONSUMER_EXIT,
    START_CONSUMER = 201,
    STOP_CONSUMER,
    CONSUMER_SUBSCRIBE,
    RESCAN_PROVIDER,
    SEND_SYNCINFO,
    CONSUMER_GET_TOPICS,
    CONSUMER_UPDATE_TOPICS,
    CONSUMER_UNSUBSCRIBE,
    CONSUMER_ENABLE_REMOTE_SERVICE
} ConsumerCppAppMenu;

#endif /* __NS_CPP_APP_UTILITY_H__ */
