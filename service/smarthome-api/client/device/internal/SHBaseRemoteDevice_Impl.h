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

#include <string>
#include <list>
#include "octypes.h"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class SHBaseRemoteResource;

            class SHBaseRemoteDevice_Impl
            {
            friend class SHBaseRemoteDeviceBuilder;
            friend class SHBaseRemoteDevice;
            public:
                virtual ~SHBaseRemoteDevice_Impl();

            protected:

                void setTypes(std::list<std::string> deviceTypes);

                void setDeviceId(std::string deviceId);

                std::list<std::string> getTypes() const;

                std::string getDeviceId() const;

                bool hasDeviceType(const std::string deviceType) const;

                std::list<SHBaseRemoteResource*> getResourceWithResourceType(
                        const std::string &resourceType);

                const SHBaseRemoteResource* getResourceWithResourceUri(
                        const std::string &resourceUri);

                std::list<SHBaseRemoteResource*> getAllResources();

            private:
                SHBaseRemoteDevice_Impl();
                std::list<SHBaseRemoteResource*> m_resources;
                std::list<std::string> m_deviceTypes;
                std::string m_deviceId;

                void addResource(SHBaseRemoteResource *resource);
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_SHBASEREMOTEDEVICE_IMPL_H_ */
