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
#include <SHBaseRemoteDevice.h>
#include <SHBaseRemoteDevice_Impl.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            SHBaseRemoteDevice::SHBaseRemoteDevice()
            {
                m_remoteDeviceImpl = new SHBaseRemoteDevice_Impl;
            }

            SHBaseRemoteDevice::~SHBaseRemoteDevice()
            {
                delete m_remoteDeviceImpl;
            }

            std::string SHBaseRemoteDevice::getType()
            {
                return m_remoteDeviceImpl->getType();
            }

            std::list<SHBaseRemoteResource*> SHBaseRemoteDevice::getResourceWithResourceType(
                    const std::string &resourceType)
            {
                return m_remoteDeviceImpl->getResourceWithResourceType(resourceType);
            }

            SHBaseRemoteResource* SHBaseRemoteDevice::getResourceWithResourceUri(
                    const std::string &resourceUri)
            {
                return m_remoteDeviceImpl->getResourceWithResourceUri(resourceUri);
            }
        }
    }
}
