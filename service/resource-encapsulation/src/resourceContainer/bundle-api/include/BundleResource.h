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

#ifndef BUNDLERESOURCE_H_
#define BUNDLERESOURCE_H_

#include <list>
#include <string>
#include <map>
#include <vector>

#include "NotificationReceiver.h"
#include "RCSResourceAttributes.h"

namespace OIC
{
    namespace Service
    {

        /**
        * @class    BundleResource
        * @brief    This class represents Basic bundle resource template
        *               to be registered in the container and make resource server
        *
        */
        class BundleResource
        {
            public:

                /**
                * Constructor for BundleResource
                */
                BundleResource();

                /**
                * Virtual destructor for BundleResource
                */
                virtual ~BundleResource();

                /**
                * Return the list of attribute names of the resource
                *
                * @return std::list - return list of the attribute names
                */
                std::list<std::string> getAttributeNames();

                /**
                * Initialize attributes of the resource
                *
                * @return void
                */
                virtual void initAttributes() = 0;

                /**
                * Register notification receiver(resource container) to notify for the changes of attributes
                *
                * @param pNotiReceiver - Notification Receiver to get notification from bundle resource
                *
                * @return void
                */
                void registerObserver(NotificationReceiver *pNotiReceiver);

                /**
                * Return all attributes of the resource
                *
                * @return RCSResourceAttributes - attributes of the resource
                */
                virtual RCSResourceAttributes &getAttributes();

                /**
                * Execute the logic of bundle to set the value of attribute
                *
                * @param key - name of attribute to set
                *
                * @param value - value of attribute to set
                *
                * @return void
                */
                virtual void setAttribute(std::string key, RCSResourceAttributes::Value &&value);

                /**
                * Execute the logic of bundle to get the value of attribute
                *
                * @param key - key of attribute to get
                *
                * @return RCSResourceAttributes::Value - return value of the attribute
                */
                virtual RCSResourceAttributes::Value getAttribute(const std::string &key);


            public:
                std::string m_bundleId;
                std::string m_name, m_uri, m_resourceType, m_address;
                std::map< std::string, std::vector< std::map< std::string, std::string > > > m_mapResourceProperty;

            private:
                NotificationReceiver *m_pNotiReceiver;
                RCSResourceAttributes m_resourceAttributes;

        };
    }
}

#endif
