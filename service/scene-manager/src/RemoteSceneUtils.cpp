//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "RemoteSceneUtils.h"

namespace OIC
{
    namespace Service
    {

        RCSRemoteResourceObject::Ptr createRCSResourceObject(
            const std::string &address, const OCConnectivityType ct,
            const std::vector< std::string > &vecRT, const std::vector< std::string > &vecIF)
        {
            RCSRemoteResourceObject::Ptr pResource = nullptr;

            std::string hostaddress, uri;
            SceneUtils::getHostUriString(address, &hostaddress, &uri);

            OC::OCResource::Ptr pOCResource =
                OC::OCPlatform::constructResourceObject
                (hostaddress, uri, ct, false, vecRT, vecIF);

            if (pOCResource)
            {
                pResource = RCSRemoteResourceObject::fromOCResource(pOCResource);
            }

            return pResource;
        }

    }
}