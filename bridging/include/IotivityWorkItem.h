//******************************************************************
//
// Copyright 2017 Intel Mobile Communications GmbH All Rights Reserved.
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
//

#ifndef _IOTIVITYWORKITEM_H_
#define _IOTIVITYWORKITEM_H_

#include "ocstack.h"
#include "octypes.h"
#include <string>

namespace OC
{
    namespace Bridging
    {
        enum class WorkItemType
        {
            CREATE, DELETE, RESPOND, NOTIFY
        };

        class IotivityWorkItem
        {
        public:
            std::string m_uri, m_resourceType, m_interface;

            OCEntityHandler m_entityHandler;
            void *m_callbackParam;
            uint8_t m_resourceProperties;
            std::unique_ptr<OCEntityHandlerResponse> m_response;

            WorkItemType m_type;

            virtual ~IotivityWorkItem()
            {};
        };


        class CreateResourceItem : public IotivityWorkItem
        {
        public:
            CreateResourceItem(
                    std::string uri,
                    std::string resourceType,
                    std::string interface,
                    OCEntityHandler entityHandler,
                    void *callbackParam,
                    uint8_t resourceProperties
            )
            {
                m_type = WorkItemType::CREATE;
                m_uri = uri;
                m_resourceType = resourceType;
                m_interface = interface;
                m_entityHandler = entityHandler;
                m_callbackParam = callbackParam;
                m_resourceProperties = resourceProperties;
            }

            ~CreateResourceItem()
            {};
        };

        class SendResponseItem : public IotivityWorkItem
        {
        public:
            SendResponseItem(std::unique_ptr<OCEntityHandlerResponse> response)
            {
                m_type = WorkItemType::RESPOND;
                m_response = std::move(response);
            }

            ~SendResponseItem()
            {};
        };

        class NotifyObserversItem : public IotivityWorkItem
        {
        public:
            NotifyObserversItem(const std::string &uri)
            {
                m_uri = uri;
                m_type = WorkItemType::NOTIFY;
            }

            ~NotifyObserversItem()
            {};
        };

        class DeleteResourceItem : public IotivityWorkItem
        {
        public:
            DeleteResourceItem(const std::string &uri)
            {
                m_uri = uri;
                m_type = WorkItemType::DELETE;
            }

            ~DeleteResourceItem()
            {};
        };
    }
}
#endif // _IOTIVITYWORKITEM_H_