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
#include "ocstack.h"

#define TAG "NotiConsumerWrapperExample"
using namespace std;
using namespace OIC::Service;

bool isExit = false;
std::string REMOTE_SERVER_ADDRESS;

void onNotificationPostedCb(OIC::Service::NSMessage *notification)
{
    std::cout << "id : " << notification->getMessageId() << std::endl;
    std::cout << "title : " << notification->getTitle() << std::endl;
    std::cout << "content : " <<  notification->getContentText() << std::endl;
    std::cout << "source : " <<  notification->getSourceName() << std::endl;
}

void onNotificationSyncCb(OIC::Service::NSSyncInfo *sync)
{
    std::cout << "Sync ID : " <<  sync->getMessageId() << std::endl;
    std::cout << "Sync STATE : " << (int) sync->getState() << std::endl;
}

void onDiscoverNotificationCb(OIC::Service::NSProvider *provider)
{
    std::cout << "notification resource discovered" << std::endl;
    provider->subscribe();
    std::cout << "startSubscribing" << std::endl;
}

void onSubscriptionAcceptedCb(OIC::Service::NSProvider *provider)
{
    std::cout << "Subscription accepted" << std::endl;
    std::cout << "subscribed provider Id : " << provider->getProviderId() << std::endl;
    provider->setListener(onNotificationPostedCb, onNotificationSyncCb);
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
    pthread_t OCThread;

    std::cout << "start Iotivity" << std::endl;
    if (OCInit1(OC_CLIENT, OC_DEFAULT_FLAGS, OC_DEFAULT_FLAGS) != OC_STACK_OK)
    {
        std::cout << "OCInit fail" << std::endl;
        return 0;
    }

    NSConsumerService::ConsumerConfig cfg;
    cfg.m_discoverCb = onDiscoverNotificationCb;
    cfg.m_acceptedCb = onSubscriptionAcceptedCb;

    pthread_create(&OCThread, NULL, OCProcessThread, NULL);

    std::cout << "Start notification consumer service" << std::endl;
    while (!isExit)
    {
        int num;

        std::cout << "1. Start Consumer" << std::endl;
        std::cout << "2. Stop Consumer" << std::endl;
        std::cout << "3. Enable  NS Consumer RemoteService" << std::endl;
        std::cout << "5. Exit" << std::endl;

        std::cout << "Input: " << std::endl;
        std::cin >> num;
        switch (num)
        {
            case 1:
                std::cout << "1. Start the Notification Consumer" << std::endl;
                NSConsumerService::getInstance()->Start(cfg);
                break;
            case 2:
                std::cout << "2. Stop the Notification Consumer" << std::endl;
                NSConsumerService::getInstance()->Stop();
                break;
            case 3:
                {
                    std::cout << "3. Enable NS Consumer RemoteService" << std::endl;
                    std::cout << "Input the Server Address :";
                    std::cin >> REMOTE_SERVER_ADDRESS;
                    NSConsumerService::getInstance()->EnableRemoteService(REMOTE_SERVER_ADDRESS);
                    break;
                }
            case 5:
                std::cout << "5. Exit" << std::endl;
                isExit = true;
                break;
            default:
                break;
        }
    }

    return 0;
}
