/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#include "NSProviderHelper.h"

NSProviderHelper* NSProviderHelper::s_nsHelperInstance = NULL;
std::mutex NSProviderHelper::s_mutex;
NSConsumer* NSProviderHelper::s_pConsumer = nullptr;
string NSProviderHelper::s_consumerID = "";
const string NOTIFICATION_INFO = "OCF_NOTIFICATION";
bool NSProviderHelper::s_isOnSubscribeRequest = false;
bool NSProviderHelper::s_isOnProviderSyncInfo = false;

NSProviderHelper::NSProviderHelper()
{
    PlatformConfig config
    { ServiceType::InProc, ModeType::Both, "0.0.0.0", 0, QualityOfService::LowQos };
    OCPlatform::Configure(config);
    IOTIVITYTEST_LOG(INFO, "Platform Configuration Done!!");

    try
    {
        OCPlatform::stopPresence();
        IOTIVITYTEST_LOG(INFO, "Successfully stopPresense() called !!");
    }
    catch (...)
    {
        IOTIVITYTEST_LOG(WARNING, "Can't stop presense..!!!");
    }
}

NSProviderHelper::~NSProviderHelper()
{
    NSProviderHelper::s_pConsumer = nullptr;
}

NSProviderHelper* NSProviderHelper::getInstance(void)
{
    if (s_nsHelperInstance == NULL)
    {
        s_mutex.lock();

        s_nsHelperInstance = new NSProviderHelper();

        s_mutex.unlock();
    }

    return s_nsHelperInstance;
}

void NSProviderHelper::onProviderSyncInfo(NSSyncInfo* syncInfo)
{
    IOTIVITYTEST_LOG(INFO, "onProviderSyncInfoCallback() called !!");
    s_isOnProviderSyncInfo = true;
}

void NSProviderHelper::onSubscribeRequest(NSConsumer* consumer)
{
    IOTIVITYTEST_LOG(INFO, "onSubscribeRequestCallback() called with consumerID %s !!",
            string(consumer->consumerId).c_str());

    NSProviderHelper::s_pConsumer = consumer;
    NSProviderHelper::s_consumerID = consumer->consumerId;
    s_isOnSubscribeRequest = true;
}

NSProviderConfig NSProviderHelper::getProviderConfig(bool policy)
{
    NSProviderConfig config;

    config.subRequestCallback = &NSProviderHelper::onSubscribeRequest;
    config.syncInfoCallback = &NSProviderHelper::onProviderSyncInfo;
    config.subControllability = policy;
    config.userInfo = OICStrdup(NOTIFICATION_INFO.c_str());

    return config;
}

NSConsumer* NSProviderHelper::getConsumer()
{
    NSProviderHelper::s_pConsumer = nullptr;
    NSProviderHelper::s_consumerID = "";

    waitForConsumer(WAIT_TIME_MAX);

    return NSProviderHelper::s_pConsumer;
}

string NSProviderHelper::getConsumerID()
{
    NSProviderHelper::s_pConsumer = nullptr;
    NSProviderHelper::s_consumerID = "";

    waitForConsumer(WAIT_TIME_MAX);

    return NSProviderHelper::s_consumerID;
}

void NSProviderHelper::waitForConsumer(int time)
{
    IOTIVITYTEST_LOG(INFO, "Waiting for consumer........");

    while (time--)
    {
        if (NSProviderHelper::s_pConsumer != nullptr)
        {
            return;
        }

        CommonUtil::waitInSecond(WAIT_TIME_MIN);
    }
}

bool NSProviderHelper::printProviderTopicList(NSTopicLL *topics)
{
    bool isTopicFound = false;
    if (topics)
    {
        NSTopicLL *topicIter = topics;
        while (topicIter)
        {
            IOTIVITYTEST_LOG(INFO, "Topic Name: %s \t Topic State: %d", topicIter->topicName,
                    topicIter->state);
            if (strcmp(topicIter->topicName, TOPIC_NAME_PROVIDER) == 0)
            {
                isTopicFound = true;
            }
            topicIter = topicIter->next;
        }
    }

    return isTopicFound;
}
