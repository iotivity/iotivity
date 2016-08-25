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
#include <condition_variable>

#include "ESRichCommon.h"
#include "OCProvisioningManager.h"

namespace OIC
{
    namespace Service
    {
        #define ENROLEE_SECURITY_TAG "ENROLEE_SECURITY"
        #define UUID_SIZE (16)
        #define UUID_STRING_SIZE (37)


        class EnrolleeResource;
        class OCSecureResource;

        typedef std::vector<OCProvisionResult_t> PMResultList_t;

        /**
         * This class contains the methods needed for security  layer interaction.
         *
         * @see EnrolleeSecurity
         */
        class EnrolleeSecurity
        {
        public:
            EnrolleeSecurity(std::shared_ptr< OC::OCResource > resource,
            const std::string secDbPath);
            void registerCallbackHandler(
                const SecurityProvStatusCb securityProvStatusCb,
                const SecurityPinCb securityPinCb,
                const SecProvisioningDbPathCb secProvisioningDbPathCb);
            void provisionOwnership();
            ESResult provisionACLForCloudServer(
                const std::string cloudUuid);
            std::string getUUID() const;

        private:
            std::shared_ptr< OC::OCResource > m_ocResource;
            SecurityProvStatusCb m_securityProvStatusCb;
            SecurityPinCb m_securityPinCb;
            SecProvisioningDbPathCb m_secProvisioningDbPathCb;
            std::shared_ptr< OC::OCSecureResource > m_unownedDevice;

            std::mutex m_mtx;
            std::condition_variable m_cond;
            bool aclResult;

            std::shared_ptr< OC::OCSecureResource > m_securedResource;
            std::shared_ptr< OC::OCSecureResource > findEnrolleeSecurityResource(
                OC::DeviceList_t &list);
            void performOwnershipTransfer();
            bool isOwnedDeviceRegisteredInSVRDB();
            void removeDeviceWithUuidCB(OC::PMResultList_t *result, int hasError);
            void ownershipTransferCb(OC::PMResultList_t *result, int hasError);
            void convertUUIDToString(const uint8_t uuid[UUID_SIZE],
                                                std::string& uuidString);
            void convertStringToUUID(OicUuid_t& uuid, const std::string uuidString);
            OicSecAcl_t* createAcl(const OicUuid_t cloudUuid);

            void ACLProvisioningCb(PMResultList_t *result, int hasError);
        };
    }
}

#endif /* ENROLLEE_SECURITY_H_*/

