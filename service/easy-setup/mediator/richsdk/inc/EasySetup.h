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

#ifndef EASYSETUP_H_
#define EASYSETUP_H_

#include <memory>
#include <vector>

#include "ESRichCommon.h"
#include "RemoteEnrollee.h"

namespace OIC
{
    namespace Service
    {
        class RemoteEnrollee;

        /**
         * This provides methods for Mediator devices to perform EasySetup of Enrollee devices.
         * EasySetup involves
         * 1) Ownership transfer for enabling secured communication between Mediator and Enrollee
         * devices.
         * 2) Provisioning of Enrollee devices using the credentails of target Enroller devices.
         */
        class EasySetup
        {
        public:
            /**
             * API for getting the instance of EasySetup singleton class.
             * @return EasySetup instance.
             */
            static EasySetup* getInstance();

            /**
             * This API is used for creating a remote Enrollee device instance.
             * @param ProvConfig Provisioning information for configuring the Enrollee.
             * @param WiFiOnboadingConnection Onboarding connection information for configuring the Enrollee.
             *
             * @throws ESBadRequestException If createEnrolleeDevice is invoked with the same
             *         provisioning information.
             *
             * @return Pointer to RemoteEnrollee instance.
             */
            std::shared_ptr<RemoteEnrollee> createEnrolleeDevice (
                         const ProvConfig& enrolleeNWProvInfo,
                         const WiFiOnboadingConnection& wifiOnboardingconn);
        private:
            EasySetup();
            ~EasySetup();

            RemoteEnrollee::shared_ptr findDeviceInProvisioningList(
                                const ProvConfig& enrolleeNWProvInfo,
                                const WiFiOnboadingConnection& wifiOnboardingconn);
            bool addDeviceToProvisioningList(const RemoteEnrollee::shared_ptr remoteEnrollee);
            bool deleteDeviceFromProvisioningList (const ProvConfig& enrolleeNWProvInfo);

            std::vector< RemoteEnrollee::shared_ptr > m_activeEnrolleeList;
            static EasySetup *s_instance;
        };
    }
}

#endif /* EASYSETUP_H_ */
