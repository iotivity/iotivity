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
#ifndef SMARTHOME_API_SERVER_SHBASERESOURCE_H_
#define SMARTHOME_API_SERVER_SHBASERESOURCE_H_

#include <string>
#include <list>
#include <CommonApi.h>

/**
 * @file
 *
 * This file contains the declaration of classes and its members related to SHBaseResource.
 */

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class SHBaseResource_Impl;
            class PropertyBundle;
            class ResourceQuery;

            /**
             * @class
             *
             * @brief This class contains a set of callback functions handling events and requests
             * for SHBaseResource class.
             */
            class SHBaseResourceDelegate
            {
            public:
                /**
                 * Virtual destructor
                 */
                virtual ~SHBaseResourceDelegate() {}

                /**
                 * API to handle client get requests
                 *
                 * @param[in] requestId Request handle assigned for each incoming request
                 * @param[in] query ResourceQuery to specify request

                 * @return Returns ::SUCCESS if success
                 */
                virtual ResultCode onGet(RequestId requestId, const ResourceQuery& query) = 0;

                /**
                 * API to handle client post requests
                 *
                 * @param[in] requestId Request handle assigned for each incoming request
                 * @param[in] bundle Set of attribute names and values which needs to be
                 * updated.
                 * @param[in] query ResourceQuery to specify request

                 * @return Returns ::SUCCESS if success
                 */
                virtual ResultCode onSet(RequestId requestId, const PropertyBundle& bundle,
                                         const ResourceQuery& query) = 0;
            };

           /**
            * @class
            *
            * @brief This class contains a set of functions to get/set/delete the properties of
            * a resource. Also, some API can support to send response to an incoming request.
            */
            class SHBaseResource
            {
            public:
                /**
                 * API to get URI of this resource
                 *
                 * @return resource Uri
                 */
                std::string getUri() const;

                /**
                 * API to set URI of this resource
                 *
                 * @param[in] Uri Uri of Resource
                 */
                void setUri(const std::string& uri);

                /**
                * API to get the list of resource types
                *
                * @return resource types
                */
                std::list<std::string> getTypes() const;

                /**
                * API to set the list of resource types
                *
                * @param[in] types resource types
                */
                void setTypes(const std::list<std::string>& types);

                /**
                * API to get the list of resource interfaces
                *
                * @return resource interfaces
                */
                std::list<std::string> getInterfaces() const;

                /**
                * API to set the list of resource interfaces
                *
                * @param[in] interfaces resource interfaces
                */
                void setInterfaces(const std::list<std::string>& interfaces);

                SHBaseResource(const std::string& uri, const std::string& type);
                SHBaseResource(const std::string& uri, const std::string& type, 
                               const std::string& interface);
                SHBaseResource(const std::string& uri, const std::list<std::string>& types);
                SHBaseResource(const std::string& uri, const std::list<std::string>& types,
                               const std::list<std::string>& interfaces);
                virtual ~SHBaseResource();

            protected:
                /**
                * API to set the properties of a resource
                *
                * @param[in] bundle resource interfaces
                */
                void setPropertyBundle(const PropertyBundle& bundle);

                /**
                * API to get the properties of a resource
                *
                * @return PropertyBundle the properties of a resource
                */
                const PropertyBundle& getPropertyBundle() const;

                /**
                * API to send response to an incoming request
                *
                * @param[in] requestId Request handle assigned for each incoming request
                * @param[in] bundle the properties of a resource
                * @return true if success.
                */
                bool sendResponse(RequestId requestId, const PropertyBundle& bundle);

                /**
                * API to send error response to an incoming request
                *
                * @param[in] requestId Request handle assigned for each incoming request
                * @param[in] bundle the properties of a resource
                *
                * @return true if success
                */
                bool sendErrorResponse(RequestId requestId, const PropertyBundle& bundle);

                /**
                * API to set delegate of this resource
                *
                * @param[in] delegate Instance that inherit SHBaseResourceDelegate class
                */
                void setDelegate(SHBaseResourceDelegate *delegate);

            private:
                SHBaseResource_Impl *m_resourceImpl;
            };
        }
    }
}

#endif /* SMARTHOME_API_SERVER_SHBASERESOURCE_H_ */
