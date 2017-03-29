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
#ifndef SMARTHOME_API_CLIENT_SHBASEREMOTERESOURCE_IMPL_H_
#define SMARTHOME_API_CLIENT_SHBASEREMOTERESOURCE_IMPL_H_

#include <list>
#include <string>

#include <ClientCallbackWrapper.h>
#include "octypes.h"

/**
 * @file
 *
 * This file contains the declaration of classes and its members related to
 * SHBaseRemoteResource_Impl.
 */

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class PropertyBundle;
            class ResourceQuery;
            class SHBaseRemoteResourceDelegate;

            /**
             * @class SHBaseRemoteResource_Impl
             *
             * @brief This class contains a set of functions to implement
             * SHBaseRemoteResoource class.
             */
            class SHBaseRemoteResource_Impl
            {
                friend class SHBaseRemoteResource;
                friend class ClientCallbackWrapper::CallbackHelper;

            public:
                ~SHBaseRemoteResource_Impl();

                std::list<std::string> getResourceTypes() const;

                std::string getUri() const;

                std::list<std::string> getHosts() const;

            protected:
                void setDelegate(const SHBaseRemoteResourceDelegate *delegate);

                void getPropertyBundle();

                void getPropertyBundle(const ResourceQuery &query);

                void setPropertyBundle(const PropertyBundle &bundle);

                void setPropertyBundle(const PropertyBundle &bundle, const ResourceQuery &query);

            private:
                SHBaseRemoteResource_Impl();
                const SHBaseRemoteResourceDelegate *m_delegate;
                std::string resourceType;
                std::string resourceUri;
                std::list<std::string> hosts;
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_SHBASEREMOTERESOURCE_IMPL_H_ */
