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
#include "oic_string.h"
#include "NSConsumer.h"
#include "NSSyncInfo.h"
#include "NSConstants.h"

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

        Result NSProviderService::Start(NSProviderService::ProviderConfig config)
        {
            NS_LOG(DEBUG, "Start - IN");

            m_config = config;
            NSProviderConfig nsConfig;
            nsConfig.subRequestCallback = onConsumerSubscribedCallback;
            nsConfig.syncInfoCallback = onMessageSynchronizedCallback;
            nsConfig.policy = config.policy;
            nsConfig.userInfo = OICStrdup(config.userInfo.c_str());

            Result result = (Result) NSStartProvider(nsConfig);
            NS_LOG(DEBUG, "Start - OUT");
            return result;
        }

        Result NSProviderService::Stop()
        {
            NS_LOG(DEBUG, "Stop - IN");
            Result result = (Result) NSStopProvider();
            NS_LOG(DEBUG, "Stop - OUT");
            return result;
        }

        Result NSProviderService::EnableRemoteService(const std::string &serverAddress)
        {
            NS_LOG(DEBUG, "EnableRemoteService - IN");
            Result result = Result::ERROR;
#ifdef WITH_CLOUD
            result = (Result) NSProviderEnableRemoteService(OICStrdup(serverAddress.c_str()));
#else
            NS_LOG(ERROR, "Remote Services feature is not enabled in the Build");
#endif
            NS_LOG(DEBUG, "EnableRemoteService - OUT");
            return result;
        }

        Result NSProviderService::DisableRemoteService(const std::string &serverAddress)
        {
            NS_LOG(DEBUG, "DisableRemoteService - IN");
            Result result = Result::ERROR;
#ifdef WITH_CLOUD
            result = (Result) NSProviderDisableRemoteService(OICStrdup(serverAddress.c_str()));
#else
            NS_LOG(ERROR, "Remote Services feature is not enabled in the Build");
#endif
            NS_LOG(DEBUG, "DisableRemoteService - OUT");
            return result;
        }

        Result NSProviderService::SendMessage(NSMessage *msg)
        {
            NS_LOG(DEBUG, "SendMessage - IN");
            Result result = Result::ERROR;
            if (msg != nullptr)
                result = (Result) NSSendMessage(getNSMessage(msg));
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

        NSProviderService::ProviderConfig NSProviderService::getProviderConfig()
        {
            return m_config;
        }
    }
}
