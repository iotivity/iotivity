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
#include "oic_malloc.h"

namespace OIC
{
    namespace Service
    {
        ::NSProvider *NSProvider::getNSProvider()
        {
            ::NSProvider *provider = new ::NSProvider;
            OICStrcpy(provider->providerId, NS_UTILS_UUID_STRING_SIZE, m_providerId.c_str());
            return provider;
        }

        NSProvider::NSProvider(::NSProvider *provider)
        {
            m_stateCb = NULL;
            m_messageCb = NULL;
            m_syncInfoCb = NULL;
            m_state = NSProviderState::DENY;
            m_subscribedState = NSProviderSubscribedState::DENY;

            m_topicList = new NSTopicsList();

            if (provider != nullptr)
            {
                m_providerId.assign(provider->providerId, NS_UTILS_UUID_STRING_SIZE - 1);
            }
        }

        NSProvider::NSProvider(const NSProvider &provider)
        {
            m_providerId = provider.getProviderId();
            m_topicList = new NSTopicsList();
            auto topicsList = provider.getTopicList();
            if (topicsList != nullptr)
            {
                for (auto it : topicsList->getTopicsList())
                {
                    getTopicList()->addTopic(it->getTopicName(), it->getState());
                }
            }
            setListener(provider.getProviderStateReceivedCb(), provider.getMessageReceivedCb(),
                        provider.getSyncInfoReceivedCb());
            setProviderState(provider.getProviderState());
            setProviderSubscribedState(provider.getProviderSubscribedState());
        }

        NSProvider &NSProvider::operator=(const NSProvider &provider)
        {
            this->m_providerId = provider.getProviderId();
            this->m_topicList = new NSTopicsList();
            auto topicsList = provider.getTopicList();
            if (topicsList != nullptr)
            {
                for (auto it : topicsList->getTopicsList())
                {
                    this->getTopicList()->addTopic(it->getTopicName(), it->getState());
                }
            }
            this->setListener(provider.getProviderStateReceivedCb(), provider.getMessageReceivedCb(),
                              provider.getSyncInfoReceivedCb());
            this->setProviderState(provider.getProviderState());
            this->setProviderSubscribedState(provider.getProviderSubscribedState());
            return *this;
        }

        NSProvider::~NSProvider()
        {
            if (m_topicList != nullptr)
            {
                delete m_topicList;
            }
        }

        std::string NSProvider::getProviderId() const
        {
            return m_providerId;
        }

        NSTopicsList *NSProvider::getTopicList() const
        {
            NS_LOG(DEBUG, "getTopicList - IN");
            return m_topicList;
        }

        NSResult NSProvider::updateTopicList(NSTopicsList *topicList)
        {
            NS_LOG(DEBUG, "updateTopicList - IN");
            if (topicList == nullptr)
            {
                return NSResult::ERROR;
            }
            NS_LOG(DEBUG, "Creating TopicLL from TopicList");
            NSTopicLL *topicLL = NULL;
            for (auto it : topicList->getTopicsList())
            {
                NSTopicLL *topic = (NSTopicLL *) OICMalloc(sizeof(NSTopicLL));
                if (topic == nullptr)
                {
                    NS_LOG(ERROR, "new NSTopicLL failed");
                    return NSResult::ERROR;
                }
                topic->topicName = NULL;
                topic->topicName = OICStrdup(it->getTopicName().c_str());
                topic->state = (::NSTopicState)it->getState();
                topic->next = NULL;
                if (topicLL == NULL)
                {
                    topicLL = topic;
                }
                else
                {
                    NSTopicLL *iter = topicLL;
                    NSTopicLL *prev = NULL;
                    while (iter)
                    {
                        prev = iter;
                        iter = (NSTopicLL *) iter->next;
                    }
                    prev->next = topic;
                    topic->next = NULL;
                }
            }
            if (topicLL)
            {
                NSTopicLL *iter = topicLL;
                while (iter)
                {
                    NS_LOG_V(DEBUG, "Topic Name : %s", iter->topicName);
                    NS_LOG_V(DEBUG, "Topic State : %d", (int) iter->state);
                    iter = iter->next;
                }
            }
            NS_LOG_V(DEBUG, "calling Lower layer UpdateTopicList for Provider Id : %s",
                     getProviderId().c_str());
            NSResult result = (NSResult) NSConsumerUpdateTopicList(getProviderId().c_str(), topicLL);

            if (topicLL)
            {
                NSTopicLL *iter = topicLL;
                NSTopicLL *following = NULL;

                while (iter)
                {
                    following = iter->next;
                    if (iter)
                    {
                        NSOICFree(iter->topicName);
                        iter->next = NULL;
                        NSOICFree(iter);
                    }
                    iter = following;
                }
            }
            NS_LOG(DEBUG, "updateTopicList - OUT");
            return result;
        }

        NSProviderState NSProvider::getProviderState() const
        {
            NS_LOG_V(DEBUG, "getProviderState  state : %d", (int)m_state);
            return m_state;
        }

        NSProviderSubscribedState NSProvider::getProviderSubscribedState() const
        {
            NS_LOG_V(DEBUG, "getProviderSubscribedState  state : %d", (int)m_subscribedState);
            return m_subscribedState;
        }

        void NSProvider::subscribe()
        {
            NS_LOG(DEBUG, "Subscribe - IN");
            NSSubscribe(getProviderId().c_str());
            NS_LOG(DEBUG, "Subscribe - OUT");
        }

        bool NSProvider::isSubscribed()
        {
            NS_LOG(DEBUG, "isSubscribed - IN");
            NS_LOG_V(DEBUG, "Subscribed state : %d", (int)getProviderSubscribedState());
            if (getProviderSubscribedState() == NSProviderSubscribedState::SUBSCRIBED)
            {
                return true;
            }
            return false;
        }

        void NSProvider::sendSyncInfo(uint64_t messageId, NSSyncInfo::NSSyncType type)
        {
            NS_LOG(DEBUG, "SendSyncInfo - IN");
            NSConsumerSendSyncInfo(m_providerId.c_str(), messageId, (::NSSyncType)type);
            NS_LOG(DEBUG, "SendSyncInfo - OUT");
            return;
        }

        void NSProvider::setListener(NSProvider::ProviderStateCallback stateHandle,
                                     NSProvider::MessageReceivedCallback messageHandle,
                                     NSProvider::SyncInfoReceivedCallback syncHandle)
        {
            NS_LOG(DEBUG, "setListener - IN");
            m_stateCb = stateHandle;
            m_messageCb = messageHandle;
            m_syncInfoCb = syncHandle;
            NS_LOG(DEBUG, "setListener - OUT");
        }

        NSProvider::ProviderStateCallback NSProvider::getProviderStateReceivedCb() const
        {
            return m_stateCb;
        }

        NSProvider::MessageReceivedCallback NSProvider::getMessageReceivedCb() const
        {
            return m_messageCb;
        }

        NSProvider::SyncInfoReceivedCallback NSProvider::getSyncInfoReceivedCb() const
        {
            return m_syncInfoCb;
        }

        void NSProvider::setTopicList(NSTopicsList *topicsList)
        {
            if (m_topicList != nullptr)
            {
                delete m_topicList;
            }
            m_topicList = topicsList;
        }

        void NSProvider::setProviderState(const NSProviderState &providerState)
        {
            m_state = providerState;
        }

        void NSProvider::setProviderSubscribedState(const NSProviderSubscribedState &subscribedState)
        {
            m_subscribedState = subscribedState;
        }
    }
}
