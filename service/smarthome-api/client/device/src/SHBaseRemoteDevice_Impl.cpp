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
#include <algorithm>
#include <SHBaseRemoteDevice_Impl.h>
#include <SHBaseRemoteResource.h>

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
            }

            std::string SHBaseRemoteDevice_Impl::getType()
            {
                return m_deviceType;
            }

            void SHBaseRemoteDevice_Impl::setType(std::string deviceType)
            {
                m_deviceType = deviceType;
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
