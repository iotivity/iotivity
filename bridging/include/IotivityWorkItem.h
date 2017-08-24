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
#include "ocpayload.h"
#include "experimental/logger.h"
#include <string>

#define LOG "IOTIVITY_WORK_ITEM"

namespace OC
{
    namespace Bridging
    {
        /**
         * Basic workitem to act as a base class so more specific
         * child classes can populate fields and
         */
        class IotivityWorkItem
        {
            public:
                virtual void process() = 0;
                virtual ~IotivityWorkItem() {};

            protected:
                std::string m_uri;
        };

        /**
         * Creates an object used to create Iotivity resources.
         */
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
                : m_resourceType(resourceType)
                , m_interface(interface)
                , m_entityHandler(entityHandler)
                , m_callbackParam(callbackParam)
                , m_resourceProperties(resourceProperties)
                {
                    m_uri = uri;
                }

                virtual void process()
                {
                    OCResourceHandle handle;

                    OCStackResult res = OCCreateResource(&handle, m_resourceType.c_str(),
                                                         m_interface.c_str(),
                                                         m_uri.c_str(),
                                                         m_entityHandler,
                                                         m_callbackParam,
                                                         m_resourceProperties);

                    if (res == OC_STACK_OK)
                    {
                        OIC_LOG_V(INFO, LOG, "Created and saved %s", m_uri.c_str());
                    }
                    else
                    {
                        OIC_LOG_V(ERROR, LOG, "Failed to create %s", m_uri.c_str());
                    }
                }
            private:
                std::string m_resourceType;
                std::string m_interface;
                OCEntityHandler m_entityHandler;
                void *m_callbackParam;
                uint8_t m_resourceProperties;
        };

        /**
         * Creates an object used to create send Iotivity responses.
         */
        class SendResponseItem : public IotivityWorkItem
        {
            public:
                SendResponseItem(std::unique_ptr<OCEntityHandlerResponse> response)
                : m_response(std::move(response))
                {}

                virtual void process()
                {
                    OCDoResponse((m_response).get());
                    OCPayloadDestroy(m_response->payload);
                }

            private:
                std::unique_ptr<OCEntityHandlerResponse> m_response;
        };

        /**
         * Creates an object used to notify observers of Iotivity resources.
         */
        class NotifyObserversItem : public IotivityWorkItem
        {
            public:
                NotifyObserversItem(const std::string &uri)
                {
                    m_uri = uri;
                }

                virtual void process()
                {
                    OCResourceHandle handle = OCGetResourceHandleAtUri(m_uri.c_str());

                    if (!handle)
                    {
                        OIC_LOG_V(ERROR, LOG, "No handle for %s. Not notifying observers.", m_uri.c_str());
                        return;
                    }
                    OCNotifyAllObservers(handle, OC_NA_QOS);
                }

        };

        /**
         * Creates an object used to delete Iotivity resources.
         */
        class DeleteResourceItem : public IotivityWorkItem
        {
            public:
                DeleteResourceItem(const std::string &uri)
                {
                    m_uri = uri;
                }

                virtual void process()
                {
                    OCResourceHandle handle = OCGetResourceHandleAtUri(m_uri.c_str());

                     if (!handle)
                     {
                         OIC_LOG_V(ERROR, LOG, "No handle for %s. Nothing to delete", m_uri.c_str());
                         return;
                     }

                     OCStackResult res = OCDeleteResource(handle);
                     if (res == OC_STACK_OK)
                     {
                         OIC_LOG_V(INFO, LOG, "Deleted %s", m_uri.c_str());
                     }
                     else
                     {
                         OIC_LOG_V(ERROR, LOG, "Failed to delete %s", m_uri.c_str());
                     }
                }

        };
    }
}
#endif // _IOTIVITYWORKITEM_H_
