//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "RCSDiscoveryManager.h"
#include "PrimitiveResource.h"
#include "logger.h"

#define CLIENT_W_TAG  PCF("RCSDiscoveryManager")

using namespace OIC::Service;

namespace
{
    void findCallback(std::shared_ptr<PrimitiveResource> primitiveResource,
                      RCSDiscoveryManager::OnResourceDiscoveredCallback OnResourceDiscovered )
    {
        OC_LOG(DEBUG, CLIENT_W_TAG, "findCallback entry");
        if (nullptr == primitiveResource)
        {
            OC_LOG(ERROR, CLIENT_W_TAG, "findCallback::primitiveResource NULL Parameter");
            return ;
        }

        std::shared_ptr< RCSRemoteResourceObject>  primitiveClientResource =
            std::shared_ptr< RCSRemoteResourceObject>(new RCSRemoteResourceObject(primitiveResource));

        OnResourceDiscovered(primitiveClientResource); //passing PrimitiveClientResource to application

        OC_LOG(DEBUG, CLIENT_W_TAG, "findcb exit");
    }
}

namespace OIC
{
    namespace Service
    {
        RCSDiscoveryManager *RCSDiscoveryManager:: getInstance()
        {
            OC_LOG(DEBUG, CLIENT_W_TAG, "RCSDiscoveryManager:: getInstance entry");
            static RCSDiscoveryManager *s_instance;
            static std::mutex s_mutex;
            if (!s_instance)
            {
                std::lock_guard<std::mutex> lock(s_mutex);
                if (!s_instance)
                {
                    s_instance = new RCSDiscoveryManager();
                }
            }
            OC_LOG(DEBUG, CLIENT_W_TAG, "RCSDiscoveryManager:: getInstance exit");
            return s_instance;
        }

        void RCSDiscoveryManager::discoverResource(const RCSAddress &address,
                const std::string &resourceURI,
                OnResourceDiscoveredCallback cb)
        {

            OC_LOG(DEBUG, CLIENT_W_TAG, "RCSDiscoveryManager::discoverResource entry");
            if ( resourceURI.empty() )
            {
                OC_LOG(ERROR, CLIENT_W_TAG, "discoverResource NULL resourceURI");
                throw InvalidParameterException { "discoverResource NULL resourceURI'" };
            }
            else if ( !cb )
            {
                OC_LOG(ERROR, CLIENT_W_TAG, "discoverResource NULL Callback");
                throw InvalidParameterException { "discoverResource NULL Callback'" };
            }
            OIC::Service::discoverResource(address, resourceURI, std::bind(findCallback,
                                           std::placeholders::_1,
                                           cb));

            OC_LOG(DEBUG, CLIENT_W_TAG, "RCSDiscoveryManager::discoverResource exit");
        }
    }
}
