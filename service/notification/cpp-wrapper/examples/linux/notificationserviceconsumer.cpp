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
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include <iostream>

#include <unistd.h>
#include "NSConsumerService.h"
#include "NSMessage.h"
#include "NSProvider.h"
#include "NSTopicsList.h"
#include "ocstack.h"

#define TAG "NotiConsumerWrapperExample"
using namespace std;
using namespace OIC::Service;

bool isExit = false;
std::string REMOTE_SERVER_ADDRESS;
std::string mainProvider;

void onNotificationPostedCb(OIC::Service::NSMessage *notification)
{
    std::cout << "id : " << notification->getMessageId() << std::endl;
    std::cout << "title : " << notification->getTitle() << std::endl;
    std::cout << "content : " <<  notification->getContentText() << std::endl;
    std::cout << "source : " <<  notification->getSourceName() << std::endl;
    std::cout << "topic : " <<  notification->getTopic() << std::endl;

    auto provider = NSConsumerService::getInstance()->getProvider(notification->getProviderId());
    if (provider != nullptr)
    {
        provider->sendSyncInfo(notification->getMessageId(),
                               OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ);
    }
}

void onNotificationSyncCb(OIC::Service::NSSyncInfo *sync)
{
    std::cout << "Sync ID : " <<  sync->getMessageId() << std::endl;
    std::cout << "Sync STATE : " << (int) sync->getState() << std::endl;
}

void onProviderStateChangedCb(OIC::Service::NSProviderState state)
{
    std::cout << "onProviderStateChangedCb" << std::endl;
    if (state == OIC::Service::NSProviderState::ALLOW)
    {
        std::cout << "Provider Subscription Accepted" << std::endl;
    }
    else if (state == OIC::Service::NSProviderState::DENY)
    {
        std::cout << "Provider Subscription Denied" << std::endl;
    }
    else if (state == OIC::Service::NSProviderState::TOPIC)
    {
        OIC::Service::NSProvider *provider = NSConsumerService::getInstance()->getProvider(mainProvider);
        if (provider != nullptr)
        {
            auto topicList = provider->getTopicList();
            if (topicList != nullptr)
            {
                for (auto it : topicList->getTopicsList())
                {
                    std::cout << "Topic Name: " << it->getTopicName() << std::endl;
                    std::cout << "Topic state: " << (int) it->getState() << std::endl;
                }
            }
        }
    }
    else if (state == OIC::Service::NSProviderState::STOPPED)
    {
        std::cout << "Provider Stopped" << std::endl;
    }
}

void onDiscoverNotificationCb(OIC::Service::NSProvider *provider)
{
    std::cout << "notification resource discovered" << std::endl;
    std::cout << "SetListeners for callbacks" << std::endl;
    provider->setListener(onProviderStateChangedCb, onNotificationPostedCb, onNotificationSyncCb);
    if (!provider->isSubscribed())
    {
        std::cout << "startSubscribing" << std::endl;
        provider->subscribe();
    }
    if (mainProvider.empty())
    {
        mainProvider = provider->getProviderId();
    }
}

void *OCProcessThread(void *ptr)
{
    (void) ptr;

    while (!isExit)
    {
        usleep(2000);
        if (OCProcess() != OC_STACK_OK)
        {
            OCStop();
            break;
        }
    }

    return NULL;
}

int main(void)
{
    pthread_t OCThread = NULL;

    std::cout << "start Iotivity" << std::endl;
    if (OCInit1(OC_CLIENT, OC_DEFAULT_FLAGS, OC_DEFAULT_FLAGS) != OC_STACK_OK)
    {
        std::cout << "OCInit fail" << std::endl;
        return 0;
    }

    pthread_create(&OCThread, NULL, OCProcessThread, NULL);

    std::cout << "Start notification consumer service" << std::endl;
    while (!isExit)
    {
        int num = 0;

        std::cout << "1. Start Consumer" << std::endl;
        std::cout << "2. Stop Consumer" << std::endl;
        std::cout << "3. GetTopicList" << std::endl;
        std::cout << "4. UpdateTopicList" << std::endl;
#ifdef WITH_CLOUD
        std::cout << "5. Enable  NS Consumer RemoteService" << std::endl;
#endif
        std::cout << "6. Exit" << std::endl;

        std::cout << "Input: " << std::endl;
        std::cin >> num;
        switch (num)
        {
            case 1:
                std::cout << "1. Start the Notification Consumer" << std::endl;
                NSConsumerService::getInstance()->start(onDiscoverNotificationCb);
                break;
            case 2:
                std::cout << "2. Stop the Notification Consumer" << std::endl;
                NSConsumerService::getInstance()->stop();
                break;
            case 3:
                {
                    std::cout <<  "GetTopicList" << std::endl;
                    OIC::Service::NSProvider *provider = NSConsumerService::getInstance()->getProvider(mainProvider);
                    if (provider != nullptr)
                    {
                        auto topicList = provider->getTopicList();
                        if (topicList != nullptr)
                        {
                            for (auto it : topicList->getTopicsList())
                            {
                                std::cout << "Topic Name: " << it->getTopicName() << std::endl;
                                std::cout << "Topic state: " << (int) it->getState() << std::endl;
                            }
                        }
                    }
                }
                break;
            case 4:
                {
                    std::cout <<  "UpdateTopicList" << std::endl;
                    OIC::Service::NSProvider *provider = NSConsumerService::getInstance()->getProvider(mainProvider);
                    if (provider != nullptr)
                    {
                        NSTopicsList *topicList = new NSTopicsList();
                        topicList->addTopic("OCF_TOPIC1", NSTopic::NSTopicState::SUBSCRIBED);
                        topicList->addTopic("OCF_TOPIC2", NSTopic::NSTopicState::SUBSCRIBED);
                        topicList->addTopic("OCF_TOPIC3", NSTopic::NSTopicState::UNSUBSCRIBED);

                        provider->updateTopicList(topicList);
                    }
                }
                break;
#ifdef WITH_CLOUD
            case 5:
                {
                    std::cout << "5. Enable NS Consumer RemoteService" << std::endl;
                    std::cout << "Input the Server Address :";
                    std::cin >> REMOTE_SERVER_ADDRESS;
                    NSConsumerService::getInstance()->enableRemoteService(REMOTE_SERVER_ADDRESS);
                    break;
                }
#endif
            case 6:
                std::cout << "6. Exit" << std::endl;
                isExit = true;
                break;
            default:
                break;
        }
    }

    return 0;
}
