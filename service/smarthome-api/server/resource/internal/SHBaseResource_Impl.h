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
            class SHBaseResourceDelegate;

            /**
            * @class SHBaseResource_Impl
            *
            * @brief This class contains a set of functions to implement SHBaseResoource class.
            */
            class SHBaseResource_Impl
            {
            friend class SHBaseResource;

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

                bool sendResponse(int requestId, const PropertyBundle& bundle);

                bool sendErrorResponse(int requestId, const PropertyBundle& bundle);

                void setDelegate(SHBaseResourceDelegate *delegate);

            private:
                SHBaseResource_Impl(const std::string& uri);
                SHBaseResource_Impl(const std::string& uri,
                                    const std::list<std::string>& types,
                                    const std::list<std::string>& interfaces);
                std::string m_resourceUri;
                std::list<std::string> m_resourceType;
                std::list<std::string> m_resourceInterface;

                SHBaseResourceDelegate *m_delegate;
                PropertyBundle m_propertyBundle;
            };
        }
    }
}
#endif /* SMARTHOME_API_SERVER_SHBASERESOURCE_IMPL_H_ */
