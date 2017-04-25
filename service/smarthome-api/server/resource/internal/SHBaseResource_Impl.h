/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
#ifndef SMARTHOME_API_SERVER_SHBASERESOURCE_IMPL_H_
#define SMARTHOME_API_SERVER_SHBASERESOURCE_IMPL_H_

#include <list>
#include <string>
#include <PropertyBundle.h>
#include <CommonApi.h>
#include "octypes.h"

using namespace std;

/**
 * @file
 *
 * This file contains the declaration of classes and its members related to
 * SHBaseResource_Impl.
 */

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class EntityHandlerContext;
            class SHBaseResourceDelegate;
            class ResourceQuery;

            /**
             * @class SHBaseResource_Impl
             *
             * @brief This class contains a set of functions to implement SHBaseResoource class.
             */
            class SHBaseResource_Impl
            {
                friend class SHBaseResource;
                friend class EntityHandlerWrapper;
                friend void sendAutoResponse(ResultCode resultCode,
                        const OCEntityHandlerRequest* entityHandlerRequest,
                        const SHBaseResource_Impl* ctx);
            public:
                virtual ~SHBaseResource_Impl();

            protected:
                std::string getUri() const;

                void setUri(const std::string& uri);

                std::list<std::string> getTypes() const;

                void setTypes(const std::list<std::string>& types);

                std::list<std::string> getInterfaces() const;

                void setInterfaces(const std::list<std::string>& interfaces);

                void setPropertyBundle(const PropertyBundle& bundle);

                const PropertyBundle& getPropertyBundle() const;

                bool deletePropertyBundle();

                /**
                 * API to send response to an incoming request.
                 *
                 * @param[in] requestId    Request handle assigned for each incoming request.
                 * @param[in] bundle       the properties of a resource.
                 *
                 * @return bool true if success.
                 */
                bool sendResponse(RequestId requestId, const PropertyBundle& bundle);

                /**
                 * API to send error response to an incoming request.
                 *
                 * @param[in] requestId    Request handle assigned for each incoming request.
                 * @param[in] bundle       the properties of a resource.
                 *
                 * @return bool true if success.
                 */
                bool sendErrorResponse(RequestId requestId, const PropertyBundle& bundle);

                /**
                 * API to set delegate of this resource.
                 *
                 * @param[in] delegate    Instance that inherit SHBaseResourceDelegate class.
                 */
                void setDelegate(SHBaseResourceDelegate *delegate);

            private:
                SHBaseResource_Impl(const std::string& uri, const std::string& type);
                SHBaseResource_Impl(const std::string& uri, const std::string& type,
                        const std::string& interface);
                SHBaseResource_Impl(const std::string& uri, const std::list<std::string>& types);
                SHBaseResource_Impl(const std::string& uri, const std::list<std::string>& types,
                        const std::list<std::string>& interfaces);

                SHBaseResource_Impl(const std::string& uri, const std::string& type,
                        const uint8_t properties);
                SHBaseResource_Impl(const std::string& uri, const std::string& type,
                        const std::string& interface, const uint8_t properties);
                SHBaseResource_Impl(const std::string& uri, const std::list<std::string>& types,
                        uint8_t properties);
                SHBaseResource_Impl(const std::string& uri, const std::list<std::string>& types,
                        const std::list<std::string>& interfaces, const uint8_t properties);

                ResultCode handleGetRequest(const OCEntityHandlerRequest* entityHandlerRequest,
                        const SHBaseResource_Impl* resourceContext) const;
                ResultCode handleSetRequest(const OCEntityHandlerRequest* entityHandlerRequest,
                        const SHBaseResource_Impl* resourceContext) const;

                void registerResource();
                void deregisterResource();
                void bindTypesToResource(std::list<std::string> types);
                void bindInterfacesToResource(std::list<std::string> interfaces);
                void notifyAllObservers();
                bool isChangedPropertyBundle(const PropertyBundle& bundle);

                OCResourceHandle m_resourceHandle;
                std::string m_resourceUri;
                std::list<std::string> m_resourceType;
                std::list<std::string> m_resourceInterface;
                uint8_t m_resourceProperty;

                EntityHandlerContext *m_context;
                SHBaseResourceDelegate *m_delegate;
                PropertyBundle m_propertyBundle;
            };
        }
    }
}
#endif /* SMARTHOME_API_SERVER_SHBASERESOURCE_IMPL_H_ */
