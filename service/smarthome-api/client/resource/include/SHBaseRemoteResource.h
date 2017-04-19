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
#ifndef SMARTHOME_API_CLIENT_SHBASEREMOTERESOURCE_H_
#define SMARTHOME_API_CLIENT_SHBASEREMOTERESOURCE_H_

#include <list>
#include <CommonApi.h>
#include <SHBaseRemoteResource_Impl.h>

/**
 * @file
 *
 * This file contains the declaration of classes and its members related to
 * SHBaseRemoteResource.
 */

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class PropertyBundle;
            class ResourceQuery;

           /**
            * @class SHBaseRemoteResourceDelegate
            *
            * @brief This class contains a set of base functions for callback of
            * SHBaseRemoteResource class.
            */
            class SHBaseRemoteResourceDelegate
            {
            public:
                /**
                 * Virtual destructor
                 */
                virtual ~SHBaseRemoteResourceDelegate() {}

                /**
                 * API to receive and handle a response for GET request
                 *
                 * @param[in] bundle PropertyBundle which has property keys and values
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onGet(PropertyBundle bundle, ResultCode ret) = 0;

                /**
                 * API to receive and handle a response for POST request
                 *
                 * @param[in] bundle PropertyBundle which has property keys and values
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onSet(PropertyBundle bundle, ResultCode ret) = 0;
            };

           /**
            * @class SHBaseRemoteResource
            *
            * @brief This class contains a set of functions to get/set of resource(server-side)
            * Also, some API can support to get informations about resource.
            */
            class SHBaseRemoteResource : public SHBaseRemoteResource_Impl
            {
            public:
                /**
                 * Virtual destructor
                 */
                virtual ~SHBaseRemoteResource();

                /**
                 * API to get types of this resource
                 *
                 * @return resource types
                 */
                std::list<std::string> getResourceTypes() const;

                /**
                 * API to get Uri of this resource
                 *
                 * @return resource Uri
                 */
                std::string getUri() const;

                /**
                 * API to get the endpoints information of this resource
                 *
                 * @return endpoints information
                 */
                std::list<std::string> getHosts() const;

            protected:
                SHBaseRemoteResource();

                /**
                 * API to set delegate of this resource.
                 *
                 * @param[in] delegate  Instance that inherit SHBaseRemoteResourceDelegate class
                 */
                void setDelegate(const SHBaseRemoteResourceDelegate *delegate);

                /**
                 * API to get the properties of a resource(server-side)
                 * (use GET method)
                 */
                void getPropertyBundle();

                /**
                 * API to get the properties of a resource(server-side)
                 * (use GET method)
                 *
                 * @param[in] query  ResourceQuery to specify request.
                 */
                void getPropertyBundle(const ResourceQuery &query);

                /**
                 * API to set the properties to a resource(server-side)
                 * (use POST method)
                 *
                 * @param[in] bundle Properties to set a resource
                 */
                void setPropertyBundle(const PropertyBundle &bundle);

                /**
                 * API to set the properties to a resource(server-side)
                 * (use POST method)
                 *
                 * @param[in] bundle Properties to set a resource
                 * @param[in] query  ResourceQuery to specify request
                 */
                void setPropertyBundle(const PropertyBundle &bundle, const ResourceQuery &query);
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_SHBASEREMOTERESOURCE_H_ */
