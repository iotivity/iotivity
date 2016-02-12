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

#ifndef UTILITY_H_
#define UTILITY_H_

#ifndef WITH_ARDUINO
#include <memory>
#endif

#include "ocstack.h"
#include "octypes.h"

#ifndef WITH_ARDUINO
namespace OIC
{
    namespace Service
    {
        typedef enum
        {
            ES_PROVISIONING_ERROR = -1,
            ES_NEED_PROVISIONING,
            ES_PROVISIONED_ALREADY,
            ES_PROVISIONING_SUCCESS
        } ESState;

        typedef enum
        {
            ES_UNKNOWN = 0,
            ES_ONBOARDED,
            ES_OWNED,
            ES_PROVISIONED
        } CurrentESState;

        typedef enum
        {
            ES_SEC_UNKNOWN = 0,
            ES_SEC_OWNED,
            ES_SEC_ACL_PROVISIONED,
            ES_SEC_CREDS_PROVISIONED
        } EnrolleeSecState;

        /**
         * Security Provisioning Status
         */
        class SecProvisioningResult
        {
        public:
            std::shared_ptr< SecProvisioningResult > shared_ptr;
            SecProvisioningResult(std::string deviceUUID, ESResult result) :
                m_devUUID(deviceUUID), m_result(result)
            {

            }

            std::string getDeviceUUID()
            {
                return m_devUUID;
            }

            ESResult getResult()
            {
                return m_result;
            }
        private:
            std::string m_devUUID;
            ESResult m_result;
        };

        /**
         * Callback function definition for providing Enrollee security status .
         */
        typedef std::function< void(std::shared_ptr<SecProvisioningResult>) > EnrolleeSecStatusCb;

        /**
         * Callback definition to be invoked when the security stack expects a pin from application.
         */
        typedef std::function< void(std::string&) > SecurityPinCb;

        /**
         * Callback definition to be invoked when the stack expects a db path.
         */
        typedef std::function< void(std::string&) > SecProvisioningDbPathCb;

        class ProvisioningStatus
        {
        public:
            std::shared_ptr< ProvisioningStatus > shared_ptr;
            ProvisioningStatus(ESResult result, ESState esState) :
                    m_result(result), m_esState(esState)
            {

            }

            ESResult& getESResult()
            {
                return m_result;
            }

            ESState& getESState()
            {
                return m_esState;
            }
        private:
            ESResult m_result;
            ESState m_esState;
        };

        class EasySetupStatus
        {
        public:
            std::shared_ptr< EasySetupStatus > shared_ptr;
            EasySetupStatus(const EasySetupState& easySetupState,
                    const EnrolleeNWProvInfo& enrolleeNWProvInfo) :
                    m_easySetupState(easySetupState), m_enrolleeNWProvInfo(enrolleeNWProvInfo)
            {

            }

            EnrolleeNWProvInfo& getEasySetupNWProvInfo()
            {
                return m_enrolleeNWProvInfo;
            }

            EasySetupState& getEasySetupState()
            {
                return m_easySetupState;
            }
        private:
            EasySetupState m_easySetupState;
            EnrolleeNWProvInfo m_enrolleeNWProvInfo;
        };
    }
}
#endif //WITH_ARDUINO

#endif //UTILITY_H_