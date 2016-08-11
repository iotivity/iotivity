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

namespace OIC
{
    namespace Service
    {
        void onConsumerSubscribedCallback(::NSConsumer *consumer)
        {
            NS_LOG(DEBUG, "onConsumerSubscribedCallback - IN");
            NSConsumer *nsConsumer = new NSConsumer(consumer);
            if (NSProviderService::getInstance()->getProviderConfig().m_subscribeRequestCb != NULL)
                NSProviderService::getInstance()->getProviderConfig().m_subscribeRequestCb(nsConsumer);
            delete nsConsumer;
            NS_LOG(DEBUG, "onConsumerSubscribedCallback - OUT");
        }

        void onMessageSynchronizedCallback(::NSSyncInfo *syncInfo)
        {
            NS_LOG(DEBUG, "onMessageSynchronizedCallback - IN");
            NSSyncInfo *nsSyncInfo = new NSSyncInfo(syncInfo);
            if (NSProviderService::getInstance()->getProviderConfig().m_syncInfoCb != NULL)
                NSProviderService::getInstance()->getProviderConfig().m_syncInfoCb(nsSyncInfo);
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

            nsMsg->mediaContents = new ::NSMediaContents;
            if (msg->getMediaContents() != nullptr)
                nsMsg->mediaContents->iconImage = OICStrdup(msg->getMediaContents()->getIconImage().c_str());
            else
                nsMsg->mediaContents->iconImage = nullptr;
            return nsMsg;
        }

        NSProviderService *NSProviderService::getInstance()
        {
            static NSProviderService s_instance;
            return &s_instance;
        }

        NSResult NSProviderService::Start(NSProviderService::NSAccessPolicy policy,
                                          NSProviderService::ProviderConfig config)
        {
            NS_LOG(DEBUG, "Start - IN");

            m_config = config;
            NSResult result = NSStartProvider((::NSAccessPolicy)policy, onConsumerSubscribedCallback,
                                              onMessageSynchronizedCallback);
            NS_LOG(DEBUG, "Start - OUT");
            return result;
        }

        NSResult NSProviderService::Stop()
        {
            NS_LOG(DEBUG, "Stop - IN");
            NSResult result = NSStopProvider();
            NS_LOG(DEBUG, "Stop - OUT");
            return result;
        }

        NSResult NSProviderService::EnableRemoteService(const std::string &serverAddress)
        {
            NS_LOG(DEBUG, "EnableRemoteService - IN");
            NSResult result = NSProviderEnableRemoteService(OICStrdup(serverAddress.c_str()));
            NS_LOG(DEBUG, "EnableRemoteService - OUT");
            return result;
        }

        NSResult NSProviderService::DisableRemoteService(const std::string &serverAddress)
        {
            NS_LOG(DEBUG, "DisableRemoteService - IN");
            NSResult result = NSProviderDisableRemoteService(OICStrdup(serverAddress.c_str()));
            NS_LOG(DEBUG, "DisableRemoteService - OUT");
            return result;
        }

        NSResult NSProviderService::SendMessage(NSMessage *msg)
        {
            NS_LOG(DEBUG, "SendMessage - IN");
            NSResult result = NS_ERROR;
            if (msg != nullptr)
                result = NSSendMessage(getNSMessage(msg));
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

        NSProviderService::ProviderConfig NSProviderService::getProviderConfig()
        {
            return m_config;
        }
    }
}
