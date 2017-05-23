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
#ifndef SMARTHOME_API_CLIENT_SHBASEREMOTEDEVICE_IMPL_H_
#define SMARTHOME_API_CLIENT_SHBASEREMOTEDEVICE_IMPL_H_

/**
 * @file
 *
 * This file contains the declaration of a class and its members related to
 * SHBaseRemoteDevice_Impl.
 */

#include <string>
#include <list>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class SHBaseRemoteResource;

            /**
             * @class SHBaseRemoteDevice_Impl
             *
             * @brief This class contains a set of base functions to control remote device.
             */
            class SHBaseRemoteDevice_Impl
            {
                friend class SHBaseRemoteDevice;

            public:
                ~SHBaseRemoteDevice_Impl();

            protected:
                void setType(std::string deviceType);

                std::string getType();

                std::list<SHBaseRemoteResource*> getResourceWithResourceType(
                        const std::string &resourceType);

                SHBaseRemoteResource* getResourceWithResourceUri(
                        const std::string &resourceUri);

                /**
                 * API to add remote resource to remote device
                 *
                 * @param[in] resource Pointer of remote resource
                 */
                void addResource(SHBaseRemoteResource *resource);

            private:
                SHBaseRemoteDevice_Impl();
                std::list<SHBaseRemoteResource*> m_resources;
                std::string m_deviceType;
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_SHBASEREMOTEDEVICE_IMPL_H_ */
