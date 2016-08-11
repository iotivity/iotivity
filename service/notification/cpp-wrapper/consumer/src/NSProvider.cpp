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
#include "NSConsumerInterface.h"

namespace OIC
{
    namespace Service
    {
        ::NSProvider *NSProvider::getNSProvider()
        {
            ::NSProvider *provider = new ::NSProvider;
            OICStrcpy(provider->providerId, m_providerId.length(), m_providerId.c_str());
            return provider;
        }

        NSProvider::NSProvider(::NSProvider *provider)
        {
            m_messageCb = NULL;
            m_syncInfoCb = NULL;
            if (provider != nullptr)
            {
                if ((provider->providerId != nullptr) && strlen(provider->providerId))
                    m_providerId.assign(provider->providerId, strlen(provider->providerId));
            }
        }

        std::string NSProvider::getProviderId() const
        {
            return m_providerId;
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
