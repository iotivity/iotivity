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
#include <SHBaseRemoteDevice_Impl.h>
#include <SHBaseRemoteResource.h>
#include <algorithm>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            SHBaseRemoteDevice_Impl::SHBaseRemoteDevice_Impl()
            {
            }

            SHBaseRemoteDevice_Impl::~SHBaseRemoteDevice_Impl()
            {
                std::list<SHBaseRemoteResource*>::iterator iter;
                for(iter = m_resources.begin(); iter != m_resources.end(); iter++)
                {
                    SHBaseRemoteResource* resource = *iter;
                    if (resource)
                    {
                        delete resource;
                    }
                }
            }

            std::list<std::string> SHBaseRemoteDevice_Impl::getTypes() const
            {
                return m_deviceTypes;
            }

            std::string SHBaseRemoteDevice_Impl::getDeviceId() const
            {
                return m_deviceId;
            }

            void SHBaseRemoteDevice_Impl::setTypes(std::list<std::string> deviceTypes)
            {
                m_deviceTypes = deviceTypes;
            }

            void SHBaseRemoteDevice_Impl::setDeviceId(std::string deviceId)
            {
                m_deviceId = deviceId;
            }

            bool SHBaseRemoteDevice_Impl::hasDeviceType(const std::string deviceType) const
            {
                std::list<std::string>::const_iterator iter =
                    std::find(m_deviceTypes.begin(), m_deviceTypes.end(), deviceType);

                return (iter != m_deviceTypes.end()) ? true : false;
            }

            std::list<SHBaseRemoteResource*> SHBaseRemoteDevice_Impl::getResourceWithResourceType(
                    const std::string &resourceType)
            {
                std::list< SHBaseRemoteResource* > resources;

                std::list< SHBaseRemoteResource* >::const_iterator iter;
                for (iter = this->m_resources.begin(); iter != this->m_resources.end(); iter++)
                {
                    std::list<std::string> rts = (*iter)->getResourceTypes();
                    if (std::find(rts.begin(), rts.end(), resourceType) != rts.end())
                    {
                        resources.push_back(*iter);
                    }
                }

                return resources;
            }

            const SHBaseRemoteResource* SHBaseRemoteDevice_Impl::getResourceWithResourceUri(
                    const std::string &resourceUri)
            {
                std::list<SHBaseRemoteResource*>::const_iterator iter;
                for(iter = this->m_resources.begin(); iter != this->m_resources.end(); iter++)
                {
                    if ((*iter)->getUri() == resourceUri)
                    {
                        return (const SHBaseRemoteResource*)(*iter);
                    }
                }

                return NULL;
            }

            std::list<SHBaseRemoteResource*> SHBaseRemoteDevice_Impl::getAllResources()
            {
                return m_resources;
            }

            void SHBaseRemoteDevice_Impl::addResource(SHBaseRemoteResource *resource)
            {
                m_resources.push_back(resource);
            }
        }
    }
}
