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

#ifndef _NS_CPP_UTILITY_H_
#define _NS_CPP_UTILITY_H_

#include <algorithm>
#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <mutex>
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "ocstack.h"
#include "octypes.h"
#include "oic_string.h"
#include "gtest_custom.h"
#include "gtest/gtest.h"
#include "OCPlatform.h"
#include "CommonUtil.h"
#include "CommonTestUtil.h"
#include "IotivityTest_Logger.h"
#include "OCPlatform.h"
#include "NSConsumerService.h"
#include "NSProvider.h"
#include "NSProviderService.h"
#include "NSConsumer.h"

#include "NSMessage.h"
#include "NSTopic.h"
#include "NSUtils.h"

#define MAX_BUF 51200
#define WAIT_TIME_MIN 1
#define WAIT_TIME_DEFAULT 2
#define WAIT_TIME_MAX 10

using namespace OC;
using namespace std;
using namespace OIC::Service;
namespace PH = std::placeholders;

const string TEST_TOPIC_1 = "TEST_TOPIC_1";
const string EMPTY_STRING = "";
const string SOURCE_NAME = "source1";
const OIC::Service::NSMessage::NSMessageType MESSAGE_TYPE =
        OIC::Service::NSMessage::NSMessageType::NS_MESSAGE_INFO;
const string TIME_TO_SET = "time1";
const uint64_t TTL_TO_SET = 0;
const string TITLE_TO_SET = "title1";
const string CONTENTTEXT_TO_SET = "contenttext1";
const string ICONIMAGE_TO_SET = "iconimage1";
const string TOPIC_NAME_1 = "topic1";
const OIC::Service::NSTopic::NSTopicState TOPIC_STATE_UNSUBSCRIBED =
        OIC::Service::NSTopic::NSTopicState::UNSUBSCRIBED;
const int PROVIDER_ACCEPT_SUCCESS=100;


class NSCppUtility
{
public:
    static string getSyncType(OIC::Service::NSSyncInfo::NSSyncType);
    static void printSyncInfo(OIC::Service::NSSyncInfo);
    static void printMessage(OIC::Service::NSMessage);
    static string getResultString(OIC::Service::NSResult result) ;
    static OIC::Service::NSTopic getDesiredTopic(shared_ptr<OIC::Service::NSTopicsList> topicList, string topicID);

};

#endif /* _NS_CPP_UTILITY_H_ */
