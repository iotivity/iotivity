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
#include <stdlib.h>
#include <cstdint>
#include "NSCommon.h"
#include "NSProviderService.h"
#include "NSUtils.h"
#include "NSTopicsList.h"
#include "logger.h"
#include "octypes.h"
#include "pthread.h"
#include "oic_string.h"
#include "oic_malloc.h"
#include "ocstack.h"

#define TAG "NotiProviderWrapperExample"
using namespace std;
using namespace OIC::Service;
std::string mainConsumer;
uint64_t mainMessageId;

extern char *strdup(const char *s);

bool isExit = false;

int id = 0;
std::string REMOTE_SERVER_ADDRESS;

void *OCProcessThread(void *ptr)
{
    (void) ptr;
    while (!isExit)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            std::cout << "OCStack process error" << std::endl;
            return NULL;
        }
    }

    return NULL;
}

void subscribeRequestCallback(OIC::Service::NSConsumer *consumer)
{
    std::cout << "consumer requested to subscribe" << std::endl;

    std::cout << "Consumer Device ID: " << consumer->getConsumerId() << std::endl;
    if (mainConsumer.empty())
    {
        mainConsumer = consumer->getConsumerId();
    }
    consumer->acceptSubscription(true);
}

void syncCallback(OIC::Service::NSSyncInfo *sync)
{
    std::cout << "synnc requested" << std::endl;

    std::cout << "Sync State: " << (int) sync->getState() << std::endl;
}

int main()
{
    int num = 0;
    pthread_t processThread = NULL;

    std::cout << "start Iotivity" << std::endl;


    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        std::cout << "OCStack init error" << std::endl;
        return 0;
    }

    pthread_create(&processThread, NULL, OCProcessThread, NULL);

    while (!isExit)
    {
        std::cout << "1. Start the Notification Provider(Accepter: Provider)" << std::endl;
        std::cout << "2. Start the Notification Provider(Accepter: Consumer)" << std::endl;
        std::cout << "3. SendMessage " << std::endl;
        std::cout << "4. SendSyncInfo" << std::endl;

        std::cout << "5. RegisterTopic" << std::endl;
        std::cout << "6. UnregisterTopic" << std::endl;
        std::cout << "7. SetTopic" << std::endl;
        std::cout << "8. UnsetTopic" << std::endl;
        std::cout << "9. GetConsumerTopicList" << std::endl;
        std::cout << "10. GetRegisteredTopicList" << std::endl;
#ifdef WITH_CLOUD
        std::cout << "11. Enable NS Provider RemoteService" << std::endl;
        std::cout << "12. Disable NS Provider RemoteService" << std::endl;
#endif
        std::cout << "13. Stop the Notification Provider" << std::endl;
        std::cout << "0. Exit()" << std::endl;

        std::cout << "input : ";

        std::cin >> num;

        switch (num)
        {
            case 1:
                {
                    std::cout << "Start (Accepter: Provider)" << std::endl;
                    NSProviderService::ProviderConfig cfg;
                    cfg.m_subscribeRequestCb = subscribeRequestCallback;
                    cfg.m_syncInfoCb = syncCallback;
                    cfg.subControllability = true;

                    NSProviderService::getInstance()->start(cfg);
                    break;
                }
            case 2:
                {
                    std::cout << "Start (Accepter: Consumer)" << std::endl;
                    NSProviderService::ProviderConfig cfg;
                    cfg.m_subscribeRequestCb = subscribeRequestCallback;
                    cfg.m_syncInfoCb = syncCallback;
                    cfg.subControllability = false;

                    NSProviderService::getInstance()->start(cfg);
                    break;
                }
            case 3:
                {
                    std::cout << "SendMessage" << std::endl;

                    std::string dummy;
                    std::string title;
                    std::string body;
                    std::string topic;

                    std::cout << "id : " << ++id << std::endl;
                    std::cout << "title : ";

                    std::getline(std::cin, dummy);
                    std::getline(std::cin, title);

                    std::cout << "body : ";
                    std::getline(std::cin, body);

                    std::cout << "topic : ";
                    std::getline(std::cin, topic);

                    std::cout << "app - mTitle : " << title << std::endl;
                    std::cout << "app - mContentText : " << body << std::endl;
                    std::cout << "app - mTopic : " << topic << std::endl;

                    OIC::Service::NSMessage *msg = NSProviderService::getInstance()->createMessage();

                    msg->setType(OIC::Service::NSMessage::NSMessageType::NS_MESSAGE_INFO);
                    msg->setTitle(title.c_str());
                    msg->setContentText(body.c_str());
                    msg->setSourceName("OCF");
                    msg->setTopic(topic);
                    mainMessageId = msg->getMessageId();
                    std::cout << "ProviderID for Message : " << msg->getProviderId() << std::endl;

                    NSProviderService::getInstance()->sendMessage(msg);

                    break;
                }
            case 4:
                {
                    std::cout <<  "SendSyncInfo" << std::endl;
                    NSProviderService::getInstance()->sendSyncInfo(mainMessageId,
                            OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ);
                    break;
                }

            case 5:
                std::cout <<  "RegisterTopic" << std::endl;
                NSProviderService::getInstance()->registerTopic("OCF_TOPIC1");
                NSProviderService::getInstance()->registerTopic("OCF_TOPIC2");
                NSProviderService::getInstance()->registerTopic("OCF_TOPIC3");
                NSProviderService::getInstance()->registerTopic("OCF_TOPIC4");
                break;

            case 6:
                std::cout <<  "UnregisterTopic" << std::endl;
                NSProviderService::getInstance()->unregisterTopic("OCF_TOPIC2");
                break;

            case 7:
                {
                    std::cout <<  "SetTopic" << std::endl;
                    OIC::Service::NSConsumer *consumer = NSProviderService::getInstance()->getConsumer(mainConsumer);
                    if (consumer != nullptr)
                    {
                        consumer->setTopic("OCF_TOPIC1");
                        consumer->setTopic("OCF_TOPIC2");
                        consumer->setTopic("OCF_TOPIC3");
                        std::cout <<  "SelectTopic completed" << std::endl;
                    }
                    break;
                }
            case 8:
                {
                    std::cout <<  "UnsetTopic" << std::endl;
                    OIC::Service::NSConsumer *consumer = NSProviderService::getInstance()->getConsumer(mainConsumer);
                    if (consumer != nullptr)
                    {
                        consumer->unsetTopic("OCF_TOPIC1");
                        std::cout <<  "UnSelectTopic completed" << std::endl;
                    }
                    break;
                }
                break;

            case 9:
                {
                    std::cout <<  "GetConsumerTopicList" << std::endl;
                    OIC::Service::NSConsumer *consumer = NSProviderService::getInstance()->getConsumer(mainConsumer);
                    if (consumer != nullptr)
                    {
                        auto nsTopics = consumer->getConsumerTopicList();
                        if (nsTopics != nullptr)
                        {
                            for (auto it : nsTopics->getTopicsList())
                            {

                                std::cout << it->getTopicName() << std::endl;
                                std::cout << (int) it->getState() << std::endl;
                            }
                            delete nsTopics;
                        }
                        std::cout <<  "GetConsumerTopicList completed" << std::endl;
                    }
                }
                break;

            case 10:
                {
                    std::cout <<  "GetRegisteredTopicList" << std::endl;
                    auto nsTopics = NSProviderService::getInstance()->getRegisteredTopicList();
                    for (auto it : nsTopics->getTopicsList())
                    {

                        std::cout << it->getTopicName() << std::endl;
                        std::cout << (int) it->getState() << std::endl;
                    }
                    delete nsTopics;
                }
                break;
#ifdef WITH_CLOUD
            case 11:
                {
                    std::cout << "11. Enable NS Provider RemoteService" << std::endl;
                    std::cout << "Input the Server Address :";
                    std::cin >> REMOTE_SERVER_ADDRESS;
                    NSProviderService::getInstance()->enableRemoteService(REMOTE_SERVER_ADDRESS);
                    break;
                }
            case 12:
                {
                    std::cout << "12. Disable NS Provider RemoteService" << std::endl;
                    std::cout << "Input the Server Address :";
                    NSProviderService::getInstance()->disableRemoteService(REMOTE_SERVER_ADDRESS);
                    break;
                }
#endif
            case 13:
                NSProviderService::getInstance()->stop();
                break;
            case 0:
                NSProviderService::getInstance()->stop();
                isExit = true;
                break;
            default:
                std::cout << "Under Construction" << std::endl;
                break;
        }

        std::cout << std::endl;
    }

    return 0;
}
