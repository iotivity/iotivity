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

#include "NSTopicsList.h"
#include "oic_malloc.h"

namespace OIC
{
    namespace Service
    {
        NSTopicsList::NSTopicsList(::NSTopicLL *topics)
        {
            ::NSTopicLL *topicsNode = nullptr;
            topicsNode = topics;

            while (topicsNode != nullptr)
            {
                addTopic(topicsNode->topicName, (NSTopic::NSTopicState)topicsNode->state);
                topicsNode = topicsNode->next;
            }

        }
        NSTopicsList::NSTopicsList(const NSTopicsList &topicsList)
        {
            for (auto it : topicsList.getTopicsList())
            {
                addTopic(it->getTopicName(), it->getState());
            }
        }

        NSTopicsList &NSTopicsList::operator=(const NSTopicsList &topicsList)
        {
            for (auto it : topicsList.getTopicsList())
            {
                this->addTopic(it->getTopicName(), it->getState());
            }
            return *this;
        }

        NSTopicsList::~NSTopicsList()
        {
            for (auto it : getTopicsList())
            {
                delete it;
            }
            getTopicsList().clear();
        }

        void NSTopicsList::addTopic(const std::string &topicName, NSTopic::NSTopicState state)
        {
            m_topicsList.push_back(new NSTopic(topicName, state));
        }

        void NSTopicsList::removeTopic(const std::string &topicName)
        {
            for (auto it : getTopicsList())
            {
                if (it->getTopicName().compare(topicName) == 0)
                {
                    m_topicsList.remove(it);
                }
            }
        }

        std::list<NSTopic *> NSTopicsList::getTopicsList() const
        {
            return m_topicsList;
        }
    }
}
