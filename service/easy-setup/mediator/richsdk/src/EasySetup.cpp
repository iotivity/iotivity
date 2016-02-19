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
                            const ProvConfig& provConfig, const WiFiOnboadingConnection& onboardingconn)
        {
            OIC_LOG(DEBUG,EASYSETUP_TAG,"Entered findDeviceInProvisioningList ()");

            std::vector< std::shared_ptr< RemoteEnrollee > >::iterator it;

            std::shared_ptr< RemoteEnrollee > remoteEnrollee = nullptr;
            for(auto it : m_activeEnrolleeList)
            {
                OIC_LOG_V(DEBUG,EASYSETUP_TAG,"entered the iterator");

                ProvConfig activeEnrolleConfig =  it->getProvConfig();
                WiFiOnboadingConnection activeEnrolleConn = it->getOnboardConn();
                if ((0 == memcmp(&activeEnrolleConfig.provData,
                                &provConfig.provData, sizeof(ProvConfig))) &&
                    (0 == memcmp(&activeEnrolleConn.ipAddress,
                          &onboardingconn.ipAddress, sizeof(onboardingconn.ipAddress))))
                {
                    remoteEnrollee = it;
                    return remoteEnrollee;
                }
            }

            OIC_LOG_V(DEBUG,EASYSETUP_TAG,"Return nullptr for  findDeviceInProvisioningList call");
            return remoteEnrollee;
        }

        bool EasySetup::addDeviceToProvisioningList(const RemoteEnrollee::shared_ptr remoteEnrollee)
        {
            ProvConfig remoteEnrolleConfig =  remoteEnrollee->getProvConfig();
            WiFiOnboadingConnection remoteEnrolleConn = remoteEnrollee->getOnboardConn();

            for (auto it : m_activeEnrolleeList)
            {
                ProvConfig activeEnrolleConfig =  it->getProvConfig();
                WiFiOnboadingConnection activeEnrolleConn = it->getOnboardConn();
                if ( (0 == memcmp(&activeEnrolleConfig.provData,
                                &remoteEnrolleConfig.provData,
                                sizeof(ProvConfig)))  &&
                     (0 == memcmp(&activeEnrolleConn.ipAddress,
                                &remoteEnrolleConn.ipAddress,
                                sizeof(remoteEnrolleConn.ipAddress)))
                   )
                {
                    return false;
                }
            }

            OIC_LOG_V(DEBUG,EASYSETUP_TAG,"Adding new device RemoteEnrollee list");
            m_activeEnrolleeList.push_back(remoteEnrollee);
            return true;
        }

        std::shared_ptr<RemoteEnrollee> EasySetup::createEnrolleeDevice (
                                        const ProvConfig& provConfig, const WiFiOnboadingConnection& wifiOnboardingconn)
        {
            if (findDeviceInProvisioningList(provConfig,wifiOnboardingconn) != nullptr)
            {
                throw ESBadRequestException { "Device already created exception" };
            }

            RemoteEnrollee::shared_ptr remoteEnrollee;

            remoteEnrollee = std::make_shared< RemoteEnrollee > (provConfig, wifiOnboardingconn);


            if (!addDeviceToProvisioningList (remoteEnrollee))
            {
                return nullptr;
            }

            return remoteEnrollee;
        }


    }
}

