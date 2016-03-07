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

#include "SceneCommons.h"

#include <string>
#include "ocrandom.h"
#include "oic_malloc.h"
#include "RCSException.h"
#include "cainterface.h"
#include "OCPlatform.h"

namespace OIC
{
    namespace Service
    {
        std::string SceneUtils::OICGenerateUUIDStr()
        {
            uint8_t uuid[UUID_SIZE] = { 0, };
            char uuidStr[UUID_STRING_SIZE] = { 0, };
            if (RAND_UUID_OK == OCGenerateUuid(uuid))
            {
                if (RAND_UUID_OK == OCConvertUuidToString(uuid, uuidStr))
                {
                    return std::string(uuidStr);
                }
            }

            throw RCSException("Failed to generate UUID");
        }

        void SceneUtils::getHostUriString(
                const std::string address, std::string *host, std::string *uri)
        {
            unsigned int nextStartIndex = 0;
            int indexOfStr = 3;

            if (address.find(COAP_TAG) == std::string::npos)
            {
                indexOfStr = 1;
            }

            for (int i = 0; i < indexOfStr; i++)
            {
                nextStartIndex
                    = address.find_first_of("/", nextStartIndex);
                if (nextStartIndex == std::string::npos)
                {
                    throw RCSInvalidParameterException("address is invalid");
                }
                nextStartIndex += 1;
            }

            *host = address.substr(0, nextStartIndex - 1);
            *uri = address.substr(nextStartIndex - 1, std::string::npos);
        }

        std::string SceneUtils::getNetAddress()
        {
            CAEndpoint_t ** netInfo = (CAEndpoint_t **)OICMalloc(sizeof(CAEndpoint_t*)*5);

            if(netInfo == nullptr)
            {
                throw RCSException("memory allocation fail");
            }

            uint32_t size = 0;
            CAGetNetworkInformation(netInfo, &size);

            if (size == 0)
            {
                OICFree(netInfo);
                throw RCSException("Disabled Network");
            }

            for (uint32_t i = 0; i < size; ++i)
            {
                if (netInfo[i]->adapter == CATransportAdapter_t::CA_ADAPTER_IP)
                {
                    std::string retAddress
                        = std::string(netInfo[i]->addr) + ":" + std::to_string(netInfo[i]->port);

                    OICFree(netInfo);
                    return retAddress;
                }
            }

            OICFree(netInfo);
            throw RCSException("Not supported Network");
        }

        RCSRemoteResourceObject::Ptr SceneUtils::createRCSResourceObject(
            const std::string &address, const OCConnectivityType ct,
            const std::vector< std::string > &vecRT, const std::vector< std::string > &vecIF)
        {
            try
            {
                std::string hostaddress, uri;
                SceneUtils::getHostUriString(address, &hostaddress, &uri);

                OC::OCResource::Ptr pOCResource =
                    OC::OCPlatform::constructResourceObject(
                        hostaddress, uri, ct, false, vecRT, vecIF);

                return RCSRemoteResourceObject::fromOCResource(pOCResource);
            }
            catch (const std::exception &e)
            {
                throw RCSException("Fail to create RCSResourceObject");
            }
        }

    }
}
