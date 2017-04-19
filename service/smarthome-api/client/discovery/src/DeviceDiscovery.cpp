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
#include <DeviceDiscovery.h>
#include <iostream>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            DeviceDiscovery::DeviceDiscovery() :
                    m_delegate(NULL)
            {
            }

            DeviceDiscovery::~DeviceDiscovery()
            {
            }

            void DeviceDiscovery::setDelegate(DeviceDiscoveryDelegate *delegate)
            {
                this->m_delegate = delegate;
            }

            void DeviceDiscovery::findRemoteDevices()
            {
                std::cout << "findRemoteDevices call!" << std::endl;
            }

            void DeviceDiscovery::findRemoteDevices(const DiscoveryQuery &query)
            {
                std::cout << "findRemoteDevices call! query : " << query.getDeviceType()
                        << std::endl;
            }

            void DeviceDiscovery::findAllRemoteDevices()
            {
                std::cout << "findAllRemoteDevices call!" << std::endl;
            }

            void DeviceDiscovery::findAllRemoteDevices(const DiscoveryQuery &query)
            {
                std::cout << "findAllRemoteDevices call! query : " << query.getDeviceType()
                        << std::endl;
            }
        }
    }
}
