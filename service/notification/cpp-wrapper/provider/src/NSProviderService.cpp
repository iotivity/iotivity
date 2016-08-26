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


#include "NSProviderService.h"
#include <cstring>
#include "NSCommon.h"
#include "NSProviderInterface.h"
#include "NSConsumer.h"
#include "NSSyncInfo.h"
#include "NSConstants.h"
#include "oic_string.h"
#include "oic_malloc.h"

namespace OIC
{
    namespace Service
    {
        void onConsumerSubscribedCallback(::NSConsumer *consumer)
        {
            NS_LOG(DEBUG, "onConsumerSubscribedCallback - IN");
            NSConsumer *nsConsumer = new NSConsumer(consumer);
            NSProviderService::getInstance()->getAcceptedConsumers().push_back(nsConsumer);
            if (NSProviderService::getInstance()->getProviderConfig().m_subscribeRequestCb != NULL)
            {
                NS_LOG(DEBUG, "initiating the callback");
                NSProviderService::getInstance()->getProviderConfig().m_subscribeRequestCb(nsConsumer);
            }
            NS_LOG(DEBUG, "onConsumerSubscribedCallback - OUT");
        }

        void onMessageSynchronizedCallback(::NSSyncInfo *syncInfo)
        {
            NS_LOG(DEBUG, "onMessageSynchronizedCallback - IN");
            NSSyncInfo *nsSyncInfo = new NSSyncInfo(syncInfo);
            if (NSProviderService::getInstance()->getProviderConfig().m_syncInfoCb != NULL)
            {
                NS_LOG(DEBUG, "initiating the callback");
                NSProviderService::getInstance()->getProviderConfig().m_syncInfoCb(nsSyncInfo);
            }
            delete nsSyncInfo;
            NS_LOG(DEBUG, "onMessageSynchronizedCallback - OUT");
        }

        ::NSMessage *NSProviderService::getNSMessage(NSMessage *msg)
        {
            ::NSMessage *nsMsg = new ::NSMessage;
            nsMsg->messageId = msg->getMessageId();
            OICStrcpy(nsMsg->providerId, msg->getProviderId().length(), msg->getProviderId().c_str());
            nsMsg->sourceName = OICStrdup(msg->getSourceName().c_str());
            nsMsg->type = (::NSMessageType) msg->getType();
            nsMsg->dateTime = OICStrdup(msg->getTime().c_str());
            nsMsg->ttl = msg->getTTL();
            nsMsg->title = OICStrdup(msg->getTitle().c_str());
            nsMsg->contentText = OICStrdup(msg->getContentText().c_str());
            nsMsg->topic = OICStrdup(msg->getTopic().c_str());

            nsMsg->mediaContents = new ::NSMediaContents;
            if (msg->getMediaContents() != nullptr)
                nsMsg->mediaContents->iconImage = OICStrdup(msg->getMediaContents()->getIconImage().c_str());
            else
                nsMsg->mediaContents->iconImage = nullptr;
            return nsMsg;
        }

        NSProviderService::~NSProviderService()
        {
            for (auto it : getAcceptedConsumers())
            {
                delete it;
            }
            getAcceptedConsumers().clear();
        }

        NSProviderService *NSProviderService::getInstance()
        {
            static NSProviderService s_instance;
            return &s_instance;
        }

        NSResult NSProviderService::Start(NSProviderService::ProviderConfig config)
        {
            NS_LOG(DEBUG, "Start - IN");

            m_config = config;
            NSProviderConfig nsConfig;
            nsConfig.subRequestCallback = onConsumerSubscribedCallback;
            nsConfig.syncInfoCallback = onMessageSynchronizedCallback;
            nsConfig.policy = config.policy;
            nsConfig.userInfo = OICStrdup(config.userInfo.c_str());

            NSResult result = (NSResult) NSStartProvider(nsConfig);
            NS_LOG(DEBUG, "Start - OUT");
            return result;
        }

        NSResult NSProviderService::Stop()
        {
            NS_LOG(DEBUG, "Stop - IN");
            NSResult result = (NSResult) NSStopProvider();
            NS_LOG(DEBUG, "Stop - OUT");
            return result;
        }

        NSResult NSProviderService::EnableRemoteService(const std::string &serverAddress)
        {
            NS_LOG(DEBUG, "EnableRemoteService - IN");
            NSResult result = NSResult::ERROR;
#ifdef WITH_CLOUD
            result = (NSResult) NSProviderEnableRemoteService(OICStrdup(serverAddress.c_str()));
#else
            NS_LOG(ERROR, "Remote Services feature is not enabled in the Build");
#endif
            NS_LOG(DEBUG, "EnableRemoteService - OUT");
            return result;
        }

        NSResult NSProviderService::DisableRemoteService(const std::string &serverAddress)
        {
            NS_LOG(DEBUG, "DisableRemoteService - IN");
            NSResult result = NSResult::ERROR;
#ifdef WITH_CLOUD
            result = (NSResult) NSProviderDisableRemoteService(OICStrdup(serverAddress.c_str()));
#else
            NS_LOG(ERROR, "Remote Services feature is not enabled in the Build");
#endif
            NS_LOG(DEBUG, "DisableRemoteService - OUT");
            return result;
        }

        NSResult NSProviderService::SendMessage(NSMessage *msg)
        {
            NS_LOG(DEBUG, "SendMessage - IN");
            NSResult result = NSResult::ERROR;
            if (msg != nullptr)
            {
                ::NSMessage *nsMsg = getNSMessage(msg);
                result = (NSResult) NSSendMessage(nsMsg);
                delete nsMsg->mediaContents;
                delete nsMsg;
            }
            else
                NS_LOG(DEBUG, "Empty Message");
            NS_LOG(DEBUG, "SendMessage - OUT");
            return result;
        }

        void NSProviderService::SendSyncInfo(uint64_t messageId,
                                             NSSyncInfo::NSSyncType type)
        {
            NS_LOG(DEBUG, "SendSyncInfo - IN");
            NSProviderSendSyncInfo(messageId, (NSSyncType)type);
            NS_LOG(DEBUG, "SendSyncInfo - OUT");
            return;
        }

        NSMessage *NSProviderService::CreateMessage()
        {
            NS_LOG(DEBUG, "CreateMessage - IN");

            ::NSMessage *message = NSCreateMessage();
            NSMessage *nsMessage = new NSMessage(message);

            NS_LOG_V(DEBUG, "Message ID : %lld", nsMessage->getMessageId());
            NS_LOG_V(DEBUG, "Provider ID : %s", nsMessage->getProviderId().c_str());
            NS_LOG(DEBUG, "CreateMessage - OUT");

            return nsMessage;
        }

        NSConsumer *NSProviderService::getConsumer(const std::string &id)
        {
            for (auto it : getAcceptedConsumers())
            {
                if (it->getConsumerId() == id)
                {
                    NS_LOG(DEBUG, "getConsumer : Found Consumer with given ID");
                    return it;
                }
            }
            NS_LOG(DEBUG, "getConsumer : Not Found Consumer with given ID");
            return NULL;
        }

        NSResult NSProviderService::AddTopic(const std::string &topicName)
        {
            NS_LOG(DEBUG, "AddTopic - IN");
            NSResult result = (NSResult) NSProviderAddTopic(OICStrdup(topicName.c_str()));
            NS_LOG(DEBUG, "AddTopic - OUT");
            return result;
        }

        NSResult NSProviderService::DeleteTopic(const std::string &topicName)
        {
            NS_LOG(DEBUG, "DeleteTopic - IN");
            NSResult result = (NSResult) NSProviderDeleteTopic(OICStrdup(topicName.c_str()));
            NS_LOG(DEBUG, "DeleteTopic - OUT");
            return result;
        }

        NSTopicsList *NSProviderService::GetTopics()
        {
            NS_LOG(DEBUG, "GetTopics - IN");
            ::NSTopicLL *topics = NSProviderGetTopics();

            NSTopicsList *nsTopics = new NSTopicsList(topics);
            NS_LOG(DEBUG, "GetTopics - OUT");
            return nsTopics;
        }

        NSProviderService::ProviderConfig NSProviderService::getProviderConfig()
        {
            return m_config;
        }

        std::list<NSConsumer *>& NSProviderService::getAcceptedConsumers()
        {
            return m_acceptedConsumers;
        }
    }
}
