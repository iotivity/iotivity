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

#include "NSConsumerHelper.h"

NSConsumerHelper* NSConsumerHelper::s_nsHelperInstance = NULL;
std::mutex NSConsumerHelper::s_mutex;
NSProvider* NSConsumerHelper::s_pProvider = nullptr;
NSMessage* NSConsumerHelper::s_pNotification = nullptr;
bool NSConsumerHelper::s_isDiscovered;
bool NSConsumerHelper::s_isTopicChanged;
bool NSConsumerHelper::s_isConsumerAllowed;
bool NSConsumerHelper::s_isNotificationPosted;

NSConsumerHelper::NSConsumerHelper()
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

NSConsumerHelper::~NSConsumerHelper()
{
    NSConsumerHelper::s_pProvider = nullptr;
}

NSConsumerHelper* NSConsumerHelper::getInstance(void)
{
    if (s_nsHelperInstance == NULL)
    {
        s_mutex.lock();

        s_nsHelperInstance = new NSConsumerHelper();

        s_mutex.unlock();
    }

    return s_nsHelperInstance;
}

void NSConsumerHelper::onProviderChanged(NSProvider * provider, NSProviderState response)
{
    IOTIVITYTEST_LOG(INFO, "onProviderChanged() called !!");

    NSConsumerHelper::s_pProvider = provider;

    IOTIVITYTEST_LOG(INFO, "Provider changed: %d", response);
    IOTIVITYTEST_LOG(INFO, "Subscribed provider Id : %s", provider->providerId);

    if (response == NS_DISCOVERED)
    {
        s_isDiscovered = true;
        IOTIVITYTEST_LOG(INFO, "notification resource discovered\n");
    }
    else if (response == NS_TOPIC)
    {
        s_isTopicChanged = true;
        IOTIVITYTEST_LOG(INFO, "Provider Topic Updated\n");
    }
    else if (response == NS_ALLOW)
    {
        s_isConsumerAllowed = true;
        IOTIVITYTEST_LOG(INFO, "subscribe allowed\n");
    }
}

void NSConsumerHelper::onNotificationPosted(NSMessage * notification)
{
    IOTIVITYTEST_LOG(INFO, "onNotificationPosted() called !!");
    s_isNotificationPosted = true;
    s_pNotification = notification;
}

void NSConsumerHelper::onNotificationSync(NSSyncInfo * sync)
{
    IOTIVITYTEST_LOG(INFO, "onNotificationSync() called !!");
}

NSConsumerConfig NSConsumerHelper::getConsumerConfig()
{
    NSConsumerConfig config;

    config.changedCb = &NSConsumerHelper::onProviderChanged;
    config.messageCb = &NSConsumerHelper::onNotificationPosted;
    config.syncInfoCb = &NSConsumerHelper::onNotificationSync;

    return config;
}

NSProvider* NSConsumerHelper::getProvider()
{
    NSConsumerHelper::s_pProvider = nullptr;

    IOTIVITYTEST_LOG(INFO, "Getting Provider.....");

    NSStartConsumer(getConsumerConfig());

    waitForProvider(WAIT_TIME_MAX);

    return NSConsumerHelper::s_pProvider;
}

void NSConsumerHelper::waitForProvider(int time)
{
    IOTIVITYTEST_LOG(INFO, "Waiting for provider........");

    while (time--)
    {
        if (NSConsumerHelper::s_pProvider != nullptr)
        {
            return;
        }

        CommonUtil::waitInSecond(WAIT_TIME_MIN);
    }
}

string NSConsumerHelper::getResultString(NSResult result)
{
    switch (result)
    {
        case NSResult::NS_OK:
            return "NS_OK";

        case NSResult::NS_ERROR:
            return "NS_ERROR";

        case NSResult::NS_SUCCESS:
            return "NS_SUCCESS";

        case NSResult::NS_FAIL:
            return "NS_FAIL";

        default:
            return "UNKNOWN";
    }
}

bool NSConsumerHelper::printTopicList(NSTopicLL *topics)
{
    IOTIVITYTEST_LOG(INFO, "Printing topic list");
    bool isTopicFound = false;
    if (topics)
    {
        NSTopicLL *topicIter = topics;
        while (topicIter)
        {
            IOTIVITYTEST_LOG(INFO, "\t Topic Name: %s \t Topic State: %d", topicIter->topicName,
                    topicIter->state);
            if (strcmp(topicIter->topicName, TOPIC_NAME_2) == 0)
            {
                isTopicFound = true;
            }
            topicIter = topicIter->next;
        }
    }

    return isTopicFound;
}

NSMessage* NSConsumerHelper::getNotificationMessage()
{
    return s_pNotification;
}

