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

#include "EasySetup.h"

#include "logger.h"
#include "ESException.h"
#include "RemoteEnrollee.h"

namespace OIC
{
    namespace Service
    {
        #define EASYSETUP_TAG "EASY_SETUP"

        EasySetup * EasySetup::s_instance = nullptr;

        EasySetup::EasySetup()
        {

        }

        EasySetup* EasySetup::getInstance ()
        {
            if (s_instance == nullptr)
            {
                s_instance = new EasySetup ();
            }
            return s_instance;
        }

        RemoteEnrollee::shared_ptr EasySetup::findDeviceInProvisioningList (
                            const EnrolleeNWProvInfo& enrolleeNWProvInfo)
        {
            OC_LOG(DEBUG,EASYSETUP_TAG,"Entered findDeviceInProvisioningList ()");

            std::vector< std::shared_ptr< RemoteEnrollee > >::iterator it;

            std::shared_ptr< RemoteEnrollee > remoteEnrollee = nullptr;
            for(auto it : m_activeEnrolleeList)
            {
                OC_LOG_V(DEBUG,EASYSETUP_TAG,"entered the iterator");

                if (0 == memcmp(&it->getEnrolleeProvisioningInfo().netAddressInfo,
                                &enrolleeNWProvInfo.netAddressInfo, sizeof(EnrolleeNWProvInfo)))
                {
                    remoteEnrollee = it;
                    return remoteEnrollee;
                }
            }

            OC_LOG_V(DEBUG,EASYSETUP_TAG,"Return nullptr for  findDeviceInProvisioningList call");
            return remoteEnrollee;
        }

        bool EasySetup::addDeviceToProvisioningList(const RemoteEnrollee::shared_ptr remoteEnrollee)
        {
            for (auto it : m_activeEnrolleeList)
            {
                if (0 == memcmp(&it->getEnrolleeProvisioningInfo().netAddressInfo,
                                &remoteEnrollee->getEnrolleeProvisioningInfo().netAddressInfo,
                                sizeof(EnrolleeNWProvInfo)))
                {
                    return false;
                }
            }

            OC_LOG_V(DEBUG,EASYSETUP_TAG,"Adding new device RemoteEnrollee list");
            m_activeEnrolleeList.push_back(remoteEnrollee);
            return true;
        }

        std::shared_ptr<RemoteEnrollee> EasySetup::createEnrolleeDevice (
                                        const EnrolleeNWProvInfo& enrolleeNWProvInfo)
        {
            if (findDeviceInProvisioningList(enrolleeNWProvInfo) != nullptr)
            {
                throw ESBadRequestException { "Device already created exception" };
            }

            RemoteEnrollee::shared_ptr remoteEnrollee;

            remoteEnrollee = std::make_shared< RemoteEnrollee > (enrolleeNWProvInfo);


            if (!addDeviceToProvisioningList (remoteEnrollee))
            {
                return nullptr;
            }

            return remoteEnrollee;
        }


    }
}

