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

#include "NSConsumer.h"
#include <cstring>
#include "NSProviderInterface.h"
#include "NSConstants.h"
#include "oic_string.h"
#include "oic_malloc.h"

namespace OIC
{
    namespace Service
    {
        ::NSConsumer *NSConsumer::getNSConsumer()
        {
            ::NSConsumer *nsCon = new ::NSConsumer;
            OICStrcpy(nsCon->consumerId, NS_UTILS_UUID_STRING_SIZE, m_consumerId.c_str());
            return nsCon;
        }

        NSConsumer::NSConsumer(::NSConsumer *consumer)
        {
            if (consumer != nullptr)
            {
                m_consumerId.assign(consumer->consumerId, NS_UTILS_UUID_STRING_SIZE);
            }
        }

        std::string NSConsumer::getConsumerId() const
        {
            return m_consumerId;
        }

        int NSConsumer::acceptSubscription(NSConsumer *consumer, bool accepted)
        {
            NS_LOG(DEBUG, "acceptSubscription - IN");
            if (consumer != nullptr)
                return NSAcceptSubscription(consumer->getNSConsumer(), accepted);
            NS_LOG(DEBUG, "acceptSubscription - OUT");
            return NS_ERROR;
        }

        NSResult NSConsumer::selectTopic(const std::string &topicName)
        {
            NS_LOG(DEBUG, "selectTopic - IN");
            NSResult result = (NSResult) NSProviderSelectTopic(OICStrdup(getConsumerId().c_str()),
                              OICStrdup(topicName.c_str()));
            NS_LOG(DEBUG, "selectTopic - OUT");
            return result;
        }

        NSResult NSConsumer::unselectTopic(const std::string &topicName)
        {
            NS_LOG(DEBUG, "unselectTopic - IN");
            NSResult result = (NSResult) NSProviderUnselectTopic(OICStrdup(getConsumerId().c_str()),
                              OICStrdup(topicName.c_str()));
            NS_LOG(DEBUG, "unselectTopic - OUT");
            return result;
        }

        NSTopicsList *NSConsumer::getConsumerTopics()
        {
            NS_LOG(DEBUG, "getConsumerTopics - IN");
            ::NSTopicLL *topics = NSProviderGetConsumerTopics(OICStrdup(getConsumerId().c_str()));

            NSTopicsList *nsTopics = new NSTopicsList(topics);
            NS_LOG(DEBUG, "getConsumerTopics - OUT");
            return nsTopics;
        }
    }
}
