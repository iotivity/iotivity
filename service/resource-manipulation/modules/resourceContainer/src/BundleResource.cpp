//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "BundleResource.h"
#include "Configuration.h"
#include <list>
#include <string.h>

namespace OIC
{
    namespace Service
    {
        BundleResource::BundleResource()
        {

        }

        BundleResource::~BundleResource()
        {

        }

        void BundleResource::registerObserver(NotificationReceiver *pNotiReceiver)
        {
            m_pNotiReceiver = pNotiReceiver;
        }

        void BundleResource::setAttribute(string attributeName, string value)
        {
            this->m_mapAttributes[attributeName] = value;

            if (m_pNotiReceiver != NULL && !m_uri.empty())
            {
                m_pNotiReceiver->onNotificationReceived(m_uri);
            }

        }

        std::string BundleResource::getAttribute(string attributeName)
        {
            return this->m_mapAttributes[attributeName];
        }

        std::list< string > BundleResource::getAttributeNames()
        {
            std::list< string > ret;
            for (map< string, string >::iterator it = m_mapAttributes.begin();
                    it != m_mapAttributes.end(); ++it)
            {
                ret.push_back(it->first);
            }
            return ret;
        }

    }
}
