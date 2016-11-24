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
#include <atomic>
#include <condition_variable>

#include "ESRichCommon.h"
#include "OCProvisioningManager.hpp"

namespace OIC
{
    namespace Service
    {
        #define ENROLEE_SECURITY_TAG "ES_ENROLLEE_SECURITY"
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
            ESResult provisionOwnership();
            std::string getUUID() const;

        private:
            std::shared_ptr< OC::OCResource > m_ocResource;
            SecurityProvStatusCb m_securityProvStatusCb;
            SecurityPinCb m_securityPinCb;
            SecProvisioningDbPathCb m_secProvisioningDbPathCb;

            std::mutex m_mtx;
            std::condition_variable m_cond;
            std::atomic<bool> OTMResult;
            std::atomic<bool> removeDeviceResult;
            std::atomic<bool> aclResult;
            std::atomic<bool> certResult;

            std::shared_ptr< OC::OCSecureResource > m_securedResource;

            ESResult performOwnershipTransfer();
            bool isOwnedDeviceRegisteredInSVRDB();
            void removeDeviceWithUuidCB(OC::PMResultList_t *result, int hasError);
            void ownershipTransferCb(OC::PMResultList_t *result, int hasError);
            void convertUUIDToString(const uint8_t uuid[UUID_SIZE],
                                                std::string& uuidString);

#if defined(__WITH_DTLS__) && defined(__WITH_TLS__)
        public:
            ESResult provisionSecurityForCloudServer(
                std::string cloudUuid, int credId);
        private:
            ESResult performCertProvisioningForCloudServer(
                std::shared_ptr< OC::OCSecureResource > ownedDevice,
                int credId);
            ESResult performACLProvisioningForCloudServer(
                std::shared_ptr< OC::OCSecureResource > ownedDevice,
                std::string& cloudUuid);
            OicSecAcl_t* createAcl(const OicUuid_t cloudUuid);
            void ACLProvisioningCb(PMResultList_t *result, int hasError);
            void CertProvisioningCb(PMResultList_t *result, int hasError);
#endif //defined(__WITH_DTLS__) && defined(__WITH_TLS__)
        };
    }
}

#endif /* ENROLLEE_SECURITY_H_*/

