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

#ifndef _NS_TEST_APP_UTILITY_H_
#define _NS_TEST_APP_UTILITY_H_

#include <iomanip>
#include <iostream>
#include <list>
#include <vector>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ocstack.h"
#include "octypes.h"
#include "OCPlatform.h"
#include "oic_string.h"
#include "NSCommon.h"
#include "NSConsumerInterface.h"
#include "NSProviderInterface.h"

#define MAX_BUF 1024

using namespace std;

NSProvider *g_provider = NULL;
NSTopicLL *g_topicLL = NULL;
NSSyncInfo* g_syncInfo;
vector< NSMessage * > g_NotificaitonList;
vector< string > g_ConsumerIDList;

string g_TopicName = "TESTAPP_Topic";

int g_ID = 0, g_TopicID = 0;
int g_ReadFile, g_WriteFile;

bool g_IsExit = false;
bool g_IsTCRunning = false;
bool g_IsConsumerStarted = false;

char g_InputBuf[MAX_BUF];

const char * FIFO_READ_FILE = "/tmp/inputFifo";
const char * FIFO_CONSUMER_FILE = "/tmp/providerFifo";
const char * FIFO_WRITE_FILE = "/tmp/consumerFifo";

typedef enum
{
    PROVIDER_EXIT,
    START_PROVIDER_ACCEPT,
    START_PROVIDER_CONSUMER_ACCEPT,
    STOP_PROVIDER,
    ACCEPT_SUBSCRIPTION,
    REJECT_SUBSCRIPTION,
    SEND_NOTIFICATION,
    SEND_SYNC_INFO,
    ADD_TOPIC,
    DELETE_TOPIC,
    SELECT_TOPIC,
    UNSELECT_TOPIC,
    CONSUMER_TOPICS,
    PROVIDER_TOPICS
} ProviderAppMenu;

typedef enum
{
    CONSUMER_EXIT,
    START_CONSUMER,
    STOP_CONSUMER,
    SUBSCRIBE_CONSUMER,
    RESCAN_PROVIDER,
    CONSUMER_SEND_SYNC_INFO,
    CONSUMER_GET_TOPICS,
    CONSUMER_UPDATE_TOPICS
} ConsumerAppMenu;

string getSyncState(NSSyncType state)
{
    switch (state)
    {
        case NSSyncType::NS_SYNC_READ:
            return "NS_SYNC_READ";

        case NSSyncType::NS_SYNC_UNREAD:
            return "NS_SYNC_UNREAD";

        case NSSyncType::NS_SYNC_DELETED:
            return "NS_SYNC_DELETED";
    }

    return "UNKNOWN_SYNC";
}

string getTopicState(NSTopicState state)
{
    switch (state)
    {
        case NSTopicState::NS_TOPIC_SUBSCRIBED:
            return "NS_TOPIC_SUBSCRIBED";

        case NSTopicState::NS_TOPIC_UNSUBSCRIBED:
            return "NS_TOPIC_UNSUBSCRIBED";
    }

    return "UNKNOWN_TOPIC";
}

#endif /* _NS_TEST_APP_UTILITY_H_ */
