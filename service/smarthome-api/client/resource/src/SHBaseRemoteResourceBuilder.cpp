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
#include <SHBaseRemoteResourceBuilder.h>
#include <SHBaseRemoteResource_Impl.h>
#include <CommonApi.h>
#include <RemoteBinarySwitchResource.h>
#include <RemoteModeResource.h>
#include <RemoteLockStatusResource.h>
#include <sstream>
#include "logger.h"

#define TAG "OIC_SH_CLIENT_REMOTERESOURCE_BUILDER"

static const std::string EP_ADDR_SPLIT   = "://";
static const std::string EP_PORT_SPLIT   = ":";
static const std::string EP_BRAKET_START = "[";
static const std::string EP_BRAKET_END   = "]";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            SHBaseRemoteResource* SHBaseRemoteResourceBuilder::createSHBaseRemoteResource(
                    OCDevAddr& devAddr,
                    OCResourcePayload *resource)
            {
                if (!resource)
                {
                    OIC_LOG(ERROR, TAG, "invaild parameter!");
                    return NULL;
                }

                OCDevAddr currentDevAddr = devAddr;

                currentDevAddr.flags = resource->secure ?
                    static_cast<OCTransportFlags>(OC_FLAG_SECURE | devAddr.flags) :
                    devAddr.flags;

                currentDevAddr.port = (resource->port != 0) ? resource->port : devAddr.port;

                OCEndpointPayload* eps = resource->eps;
                std::list<std::string> epsList;
                if (eps)
                {
                    //parsing eps from payload
                    epsList = convertEpsLLToList(eps);
                }

                std::list<std::string> resourceTypes = convertStringLLToList(resource->types);
                std::list<std::string> interfaces = convertStringLLToList(resource->interfaces);

                SHBaseRemoteResource* shResource = makeSHBaseRemoteResource(resourceTypes);
                if (!shResource)
                {
                    OIC_LOG(ERROR, TAG, "makeSHBaseRemoteResource Failed!");
                    return NULL;
                }

                shResource->m_remoteResourceImpl->setSHBaseRemoteResource_Impl(
                        std::string(resource->uri),
                        currentDevAddr,
                        resource->bitmap,
                        resourceTypes,
                        interfaces,
                        epsList);

                return shResource;
            }

            SHBaseRemoteResource* SHBaseRemoteResourceBuilder::makeSHBaseRemoteResource(
                    std::list<std::string> resourceTypes)
            {
                SHBaseRemoteResource* shResource = NULL;
                std::list<std::string>::iterator iter;
                bool isVerified = false;
                for (iter = resourceTypes.begin(); iter != resourceTypes.end(); iter++)
                {
                    if (*iter == RESOURCE_TYPE::BINARYSWITCH)
                    {
                        if (!isVerified)
                        {
                            shResource = new RemoteBinarySwitchResource;
                            isVerified = true;
                        }
                        else
                        {
                            if (shResource)
                            {
                                delete shResource;
                                shResource = new SHBaseRemoteResource;
                                break;
                            }
                        }
                    }
                    else if (*iter == RESOURCE_TYPE::MODE)
                    {
                        if (!isVerified)
                        {
                            shResource = new RemoteModeResource;
                            isVerified = true;
                        }
                        else
                        {
                            if (shResource)
                            {
                                delete shResource;
                                shResource = new SHBaseRemoteResource;
                                break;
                            }
                        }
                    }
                    else if (*iter == RESOURCE_TYPE::LOCK_STATUS)
                    {
                        if (!isVerified)
                        {
                            shResource = new RemoteLockStatusResource;
                            isVerified = true;
                        }
                        else
                        {
                            if (shResource)
                            {
                                delete shResource;
                                shResource = new SHBaseRemoteResource;
                                break;
                            }
                        }
                    }
                    //TODO check pre-defined class
                }

                if (!isVerified)
                {
                    shResource = new SHBaseRemoteResource;
                }

                return shResource;
            }

            std::list<std::string> SHBaseRemoteResourceBuilder::convertStringLLToList(
                    OCStringLL *ll)
            {
                std::list<std::string> stringList;
                while (ll)
                {
                    stringList.push_back(ll->value);
                    ll = ll->next;
                }
                return stringList;
            }

            std::list<std::string> SHBaseRemoteResourceBuilder::convertEpsLLToList(
                    OCEndpointPayload *head)
            {
                std::list<std::string> stringList;
                while (head)
                {
                    std::ostringstream endpoint;
                    endpoint << head->tps << EP_ADDR_SPLIT;

                    switch (head->family)
                    {
                        case OC_DEFAULT_FLAGS:
                            // mac
                            endpoint << head->addr;
                            break;

                        case OC_IP_USE_V4:
                            endpoint << head->addr << EP_PORT_SPLIT << head->port;
                            break;

                        case OC_IP_USE_V6:
                            endpoint << EP_BRAKET_START << head->addr << EP_BRAKET_END
                                << EP_PORT_SPLIT << head->port;
                            break;
                        default:
                            head = head->next;
                            continue;
                    }

                    stringList.push_back(endpoint.str());
                    head = head->next;
                }
                return stringList;
            }
        }
    }
}
