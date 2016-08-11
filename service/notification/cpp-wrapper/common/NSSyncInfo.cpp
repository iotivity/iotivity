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

#include "NSSyncInfo.h"

namespace OIC
{
    namespace Service
    {
        NSSyncInfo::NSSyncInfo(::NSSyncInfo *syncInfo)
        {
            if (syncInfo != nullptr)
            {
                m_messageId = syncInfo->messageId;

                if ((syncInfo->providerId != nullptr) && strlen(syncInfo->providerId))
                    m_providerId.assign(syncInfo->providerId, strlen(syncInfo->providerId));

                m_state = (NSSyncType) syncInfo->state;
            }
        }

        uint64_t NSSyncInfo::getMessageId() const
        {
            return m_messageId;
        }

        std::string NSSyncInfo::getProviderId() const
        {
            return m_providerId;
        }

        NSSyncInfo::NSSyncType NSSyncInfo::getState() const
        {
            return m_state;
        }
    }
}
