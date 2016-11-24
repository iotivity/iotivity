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

#include "NSConsumerService.h"
#include <cstring>
#include "NSConsumerInterface.h"
#include "NSMessage.h"
#include "NSCommon.h"
#include "NSConstants.h"
#include "oic_string.h"
#include "oic_malloc.h"

namespace OIC
{
    namespace Service
    {
        void onProviderStateReceived(::NSProvider *provider, ::NSProviderState state)
        {
            NS_LOG(DEBUG, "onNSProviderStateChanged - IN");
            NS_LOG_V(DEBUG, "provider Id : %s", provider->providerId);
            NS_LOG_V(DEBUG, "state : %d", (int)state);

            std::string provId;
            provId.assign(provider->providerId, NS_UTILS_UUID_STRING_SIZE - 1);
            NSProvider *oldProvider = NSConsumerService::getInstance()->getProvider(provId);

            if (oldProvider == nullptr)
            {
                NSProvider *nsProvider = new NSProvider(provider);
                NS_LOG(DEBUG, "Provider with same Id do not exist. updating Data for New Provider");
                auto discoveredCallback = NSConsumerService::getInstance()->getProviderDiscoveredCb();
                nsProvider->setProviderState((NSProviderState)state);
                auto topicLL = NSConsumerGetTopicList(provider->providerId);
                nsProvider->setTopicList(new NSTopicsList(topicLL));
                if (state == NS_DISCOVERED)
                {
                    nsProvider->setProviderSubscribedState(NSProviderSubscribedState::DISCOVERED);
                    if (discoveredCallback != NULL)
                    {
                        NS_LOG(DEBUG, "initiating the Discovered callback : NS_DISCOVERED, policy false");
                        discoveredCallback(nsProvider);
                    }
                }
                else if (state == NS_ALLOW)
                {
                    nsProvider->setProviderSubscribedState(NSProviderSubscribedState::SUBSCRIBED);
                    if (discoveredCallback != NULL)
                    {
                        NS_LOG(DEBUG, "initiating the Discovered callback : NS_ALLOW, policy true");
                        discoveredCallback(nsProvider);
                    }
                }
                NSConsumerService::getInstance()->getAcceptedProviders().push_back(nsProvider);
            }
            else
            {
                NS_LOG(DEBUG, "Provider with same Id exists. updating the old Provider data");
                auto changeCallback = oldProvider->getProviderStateReceivedCb();
                auto prevState = oldProvider->getProviderState();
                oldProvider->setProviderState((NSProviderState)state);
                if (state == NS_ALLOW)
                {
                    oldProvider->setProviderSubscribedState(NSProviderSubscribedState::SUBSCRIBED);
                    if (changeCallback != NULL)
                    {
                        NS_LOG(DEBUG, "initiating the callback for Response : NS_ALLOW");
                        changeCallback((NSProviderState)state);
                    }
                    else
                    {
                        oldProvider->setProviderSubscribedState(NSProviderSubscribedState::SUBSCRIBED);
                        auto discoveredCallback = NSConsumerService::getInstance()->getProviderDiscoveredCb();
                        discoveredCallback(oldProvider);
                        auto changeCallback = oldProvider->getProviderStateReceivedCb();
                        changeCallback(prevState);
                    }
                }
                else if (state == NS_DENY)
                {
                    oldProvider->setProviderSubscribedState(NSProviderSubscribedState::DENY);
                    NSConsumerService::getInstance()->getAcceptedProviders().remove(oldProvider);
                    if (changeCallback != NULL)
                    {
                        NS_LOG(DEBUG, "initiating the callback for Response : NS_DENY");
                        changeCallback((NSProviderState)state);
                    }
                    delete oldProvider;
                }
                else if (state == NS_TOPIC)
                {
                    auto topicLL = NSConsumerGetTopicList(provider->providerId);
                    oldProvider->setTopicList(new NSTopicsList(topicLL));
                    if (changeCallback != NULL)
                    {
                        NS_LOG(DEBUG, "initiating the callback for Response : NS_TOPIC");
                        changeCallback((NSProviderState)state);
                    }
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
                }
                else if (state == NS_STOPPED)
                {
                    oldProvider->setProviderSubscribedState(NSProviderSubscribedState::DENY);
                    NSConsumerService::getInstance()->getAcceptedProviders().remove(oldProvider);
                    NS_LOG(DEBUG, "initiating the State callback : NS_STOPPED");
                    if (changeCallback != NULL)
                    {
                        NS_LOG(DEBUG, "initiating the callback for Response : NS_STOPPED");
                        changeCallback((NSProviderState)state);
                    }
                }
            }
            NS_LOG(DEBUG, "onNSProviderStateChanged - OUT");
        }

        void onNSMessageReceived(::NSMessage *message)
        {
            NS_LOG(DEBUG, "onNSMessageReceived - IN");
            NS_LOG_V(DEBUG, "message->providerId : %s", message->providerId);

            NSMessage *nsMessage = new NSMessage(message);

            NS_LOG_V(DEBUG, "getAcceptedProviders Size : %d", (int)
                     NSConsumerService::getInstance()->getAcceptedProviders().size());
            for (auto it : NSConsumerService::getInstance()->getAcceptedProviders())
            {
                if (it->getProviderId() == nsMessage->getProviderId())
                {
                    NS_LOG(DEBUG, "Found Provider with given ID");
                    auto callback = it->getMessageReceivedCb();
                    if (callback != NULL)
                    {
                        NS_LOG(DEBUG, "initiating the callback for messageReceived");
                        callback(nsMessage);
                    }
                    break;
                }
            }
            delete nsMessage;
            NS_LOG(DEBUG, "onNSMessageReceived - OUT");
        }

        void onNSSyncInfoReceived(::NSSyncInfo *syncInfo)
        {
            NS_LOG(DEBUG, "onNSSyncInfoReceived - IN");
            NSSyncInfo *nsSyncInfo = new NSSyncInfo(syncInfo);
            for (auto it : NSConsumerService::getInstance()->getAcceptedProviders())
            {
                if (it->getProviderId() == nsSyncInfo->getProviderId())
                {
                    NS_LOG(DEBUG, "Found Provider with given ID");
                    auto callback = it->getSyncInfoReceivedCb();
                    if (callback != NULL)
                    {
                        NS_LOG(DEBUG, "initiating the callback for SyncInfoReceived");
                        callback(nsSyncInfo);
                    }
                    break;
                }
            }
            delete nsSyncInfo;
            NS_LOG(DEBUG, "onNSSyncInfoReceived - OUT");
        }

        NSConsumerService::NSConsumerService()
        {
            m_providerDiscoveredCb = NULL;
        }

        NSConsumerService::~NSConsumerService()
        {
            for (auto it : getAcceptedProviders())
            {
                delete it;
            }
            getAcceptedProviders().clear();
        }

        NSConsumerService *NSConsumerService::getInstance()
        {
            static  NSConsumerService s_instance;
            return &s_instance;
        }

        void NSConsumerService::start(NSConsumerService::ProviderDiscoveredCallback providerDiscovered)
        {
            NS_LOG(DEBUG, "start - IN");
            m_providerDiscoveredCb = providerDiscovered;
            NSConsumerConfig nsConfig;
            nsConfig.changedCb = onProviderStateReceived;
            nsConfig.messageCb = onNSMessageReceived;
            nsConfig.syncInfoCb = onNSSyncInfoReceived;

            NSStartConsumer(nsConfig);
            NS_LOG(DEBUG, "start - OUT");
            return;
        }

        void NSConsumerService::stop()
        {
            NS_LOG(DEBUG, "stop - IN");
            NSStopConsumer();
            for (auto it : getAcceptedProviders())
            {
                delete it;
            }
            getAcceptedProviders().clear();
            NS_LOG(DEBUG, "stop - OUT");
            return;
        }

        NSResult NSConsumerService::enableRemoteService(const std::string &serverAddress)
        {
            NS_LOG(DEBUG, "enableRemoteService - IN");
            NS_LOG_V(DEBUG, "Server Address : %s", serverAddress.c_str());
            NSResult result = NSResult::ERROR;
#ifdef WITH_CLOUD
            result = (NSResult) NSConsumerEnableRemoteService(OICStrdup(serverAddress.c_str()));
#else
            NS_LOG(ERROR, "Remote Services feature is not enabled in the Build");
            (void) serverAddress;
#endif
            NS_LOG(DEBUG, "enableRemoteService - OUT");
            return result;
        }

        void NSConsumerService::rescanProvider()
        {
            NS_LOG(DEBUG, "rescanProvider - IN");
            NSRescanProvider();
            NS_LOG(DEBUG, "rescanProvider - OUT");
            return;
        }

        NSConsumerService::ProviderDiscoveredCallback NSConsumerService::getProviderDiscoveredCb()
        {
            return m_providerDiscoveredCb;
        }

        NSProvider *NSConsumerService::getProvider(const std::string &id)
        {
            for (auto it : getAcceptedProviders())
            {
                if (it->getProviderId() == id)
                {
                    NS_LOG(DEBUG, "getProvider : Found Provider with given ID");
                    return it;
                }
            }
            NS_LOG(DEBUG, "getProvider : Not Found Provider with given ID");
            return NULL;
        }

        std::list<NSProvider *> &NSConsumerService::getAcceptedProviders()
        {
            return m_acceptedProviders;
        }
    }
}
