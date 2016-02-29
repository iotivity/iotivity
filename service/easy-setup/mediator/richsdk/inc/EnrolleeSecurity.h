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

#ifndef ENROLLEE_SECURITY_H_
#define ENROLLEE_SECURITY_H_

#include <functional>

#include "ESRichCommon.h"
#include "OCProvisioningManager.h"

namespace OIC
{
    namespace Service
    {
#define ENROLEE_SECURITY_TAG "ENROLEE_SECURITY"

        class RemoteEnrolleeResource;
        class OCSecureResource;

        /**
         * This class contains the methods needed for security  layer interaction.
         *
         * @see EnrolleeSecurity
         */
        class EnrolleeSecurity
        {
        public:
            EnrolleeSecurity(std::shared_ptr< RemoteEnrolleeResource > remoteEnrolleeResource,
            std::string secDbPath);
            ESResult registerCallbackHandler(EnrolleeSecStatusCb enrolleeSecStatusCb,
                    SecurityPinCb securityPinCb, SecProvisioningDbPathCb secProvisioningDbPathCb);
            EasySetupState performOwnershipTransfer();

        private:
            std::shared_ptr< RemoteEnrolleeResource > m_remoteEnrolleeResource;
            EnrolleeSecStatusCb m_enrolleeSecStatusCb;
            SecurityPinCb m_securityPinCb;
            SecProvisioningDbPathCb m_secProvisioningDbPathCb;
            std::shared_ptr< OC::OCSecureResource > m_unownedDevice;

            EnrolleeSecState m_enrolleeSecState;std::shared_ptr< OC::OCSecureResource > m_securedResource;

            std::shared_ptr< OC::OCSecureResource > getEnrollee(OC::DeviceList_t &list);
            void ownershipTransferCb(OC::PMResultList_t *result, int hasError);
            void convertUUIDToString(OicUuid_t uuid, std::string& uuidString);
        };
    }
}

#endif /* ENROLLEE_SECURITY_H_*/

