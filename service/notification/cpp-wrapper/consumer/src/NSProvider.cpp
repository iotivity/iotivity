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

#include "NSProvider.h"
#include <cstring>
#include "NSConsumerInterface.h"
#include "NSConstants.h"
#include "NSCommon.h"
#include "oic_string.h"

namespace OIC
{
    namespace Service
    {
        ::NSProvider *NSProvider::getNSProvider()
        {
            ::NSProvider *provider = new ::NSProvider;
            OICStrcpy(provider->providerId, NS_UTILS_UUID_STRING_SIZE, m_providerId.c_str());

            provider->topicLL = NULL;

            if (m_topicList != nullptr)
            {
                for (auto it : m_topicList->getTopicsList())
                {
                    ::NSTopicLL *topic = new ::NSTopicLL;
                    OICStrcpy(topic->topicName, it->getTopicName().length(),
                              it->getTopicName().c_str());
                    topic->state = (::NSTopicState)it->getState();
                    topic->next = NULL;
                    if (provider->topicLL == NULL)
                        provider->topicLL = topic;
                    else
                    {
                        topic->next = provider->topicLL;
                        provider->topicLL = topic;
                    }
                }
            }
            return provider;
        }

        NSProvider::NSProvider(::NSProvider *provider)
        {
            m_messageCb = NULL;
            m_syncInfoCb = NULL;
            if (provider != nullptr)
            {
                m_providerId = provider->providerId;
                if (provider->topicLL != nullptr)
                    m_topicList = new NSTopicsList(provider->topicLL);
                else
                    m_topicList = new NSTopicsList();
            }
        }

        NSProvider::~NSProvider()
        {
            if (m_topicList != nullptr)
                delete m_topicList;
        }

        std::string NSProvider::getProviderId() const
        {
            return m_providerId;
        }

        NSTopicsList *NSProvider::getTopicList() const
        {
            return m_topicList;
        }

        void NSProvider::subscribe()
        {
            NS_LOG(DEBUG, "subscribe - IN");
            NSSubscribe(getNSProvider());
            NS_LOG(DEBUG, "subscribe - OUT");
        }

        void NSProvider::unSubscribe()
        {
            NS_LOG(DEBUG, "unSubscribe - IN");
            NSUnsubscribe(getNSProvider());
            NS_LOG(DEBUG, "unSubscribe - OUT");
        }

        void NSProvider::SendSyncInfo(uint64_t messageId, NSSyncInfo::NSSyncType type)
        {
            NS_LOG(DEBUG, "SendSyncInfo - IN");
            NSConsumerSendSyncInfo(m_providerId.c_str(), messageId, (::NSSyncType)type);
            NS_LOG(DEBUG, "SendSyncInfo - OUT");
            return;
        }

        void NSProvider::setListener(NSProvider::MessageReceivedCallback messageHandle,
                                     NSProvider::SyncInfoReceivedCallback syncHandle)
        {
            m_messageCb = messageHandle;
            m_syncInfoCb = syncHandle;
        }

        NSResult NSProvider::selectInterestTopics(NSTopicsList *topicList)
        {
            NS_LOG(DEBUG, "selectInterestTopics - IN");
            NSProvider *provider = new NSProvider(getProviderId(), topicList);
            NSResult result = (NSResult) NSConsumerSelectInterestTopics(
                                  provider->getNSProvider());
            delete provider;
            NS_LOG(DEBUG, "selectInterestTopics - OUT");
            return result;
        }

        NSProvider::MessageReceivedCallback NSProvider::getMessageReceivedCb()
        {
            return m_messageCb;
        }

        NSProvider::SyncInfoReceivedCallback NSProvider::getSyncInfoReceivedCb()
        {
            return m_syncInfoCb;
        }
    }
}
