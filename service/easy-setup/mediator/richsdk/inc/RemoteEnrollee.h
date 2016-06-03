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

#ifndef REMOTE_ENROLLEE_H_
#define REMOTE_ENROLLEE_H_

#include "ESRichCommon.h"

namespace OIC
{
    namespace Service
    {
        class EnrolleeResource;
        class EnrolleeSecurity;

        /**
         * This class represents Remote Enrollee device instance.
         * It will provide APIs for Mediator to perform operations to enable the Enrollee device
         * to connect to the Enroller.
         */
        class RemoteEnrollee
        {
        public:
            /**
             * RemoteEnrollee constructor
             *
             * @param enrolleeNWProvInfo Provisioning information for the Enrollee
             *
             * @throw ESBadRequestException is thrown if the parameters are invalid
             */
            RemoteEnrollee(const WiFiOnboadingConnection& wifiOnboardingconn);

            ~RemoteEnrollee() = default;

#ifdef __WITH_DTLS__
            /**
             * Register Security status and other information callback handlers.
             *
             * @param secProvisioningDbCb Callback to be invoked when the stack expects a
             *        path for the provisioning db.
             * @param securityPinCb Callback to get security pin during pin based ownership transfer.
             *
             * @throws InvalidParameterException If callback is an empty function or null.
             * @throws ESBadRequestException If registration is already completed.
             *
             * @see SecProvisioningResult
             */
            ESResult registerSecurityCallbackHandler(SecurityPinCb securityPinCb,
                    SecProvisioningDbPathCb secProvisioningDbPathCb);
#endif //__WITH_DTLS__

            /**
             * Start provisioning of target Enrollers information to the Enrollee.
             *
             * @throws ESBadRequestException If RemoteEnrollee device not created prior to this call.
             *
             * @see RemoteEnrollee
             */
             void initRemoteEnrollee(InitRemoteEnrolleeStatusCb callback);

             /**
             * Start provisioning of target Enrollers information to the Enrollee.
             *
             * @throws ESBadRequestException If RemoteEnrollee device not created prior to this call.
             *
             * @see RemoteEnrollee
             */
            void startSecurityProvisioning(EnrolleeSecStatusCb callback);

            /**
             * Start provisioning of target Enrollers information to the Enrollee.
             *
             * @throws ESBadRequestException If RemoteEnrollee device not created prior to this call.
             *
             * @see RemoteEnrollee
             */
            void getCapabilityData(RequestCapabilityStatusCb callback);

            /**
             * Start provisioning of target Enrollers information to the Enrollee.
             *
             * @throws ESBadRequestException If RemoteEnrollee device not created prior to this call.
             *
             * @see RemoteEnrollee
             */
            void startDataProvisioning(const ProvConfig& dataProvConfig, DataProvStatusCb callback);

            /**
             * Stop provisioning process that is currently in progress.
             *
             * @throws BadRequestException If provisioning is not in progress.
             */
            void stopProvisioning();

            /**
             * Check if the Enrollee device provisioned.
             */
            bool isEnrolleeProvisioned();

            /**
             * Get the Provisioning information provided for the current Enrollee.
             *
             * @return ProvConfig Provisioning information provided for the current Enrollee.
             */
            ProvConfig getProvConfig ();

            /**
             * Get the Onboarding connection information between Mediator and Enrollee.
             *
             * @return WiFiOnboadingConnection information between Mediator and Enrollee.
             */

            WiFiOnboadingConnection getOnboardConn();

        private:
            std::shared_ptr< EnrolleeResource > m_remoteResource;
            EnrolleeSecStatusCb m_enrolleeSecStatusCb;
            InitRemoteEnrolleeStatusCb m_initRemoteEnrolleeStatusCb;
            RequestCapabilityStatusCb m_requestCapabilityStatusCb;
            SecurityPinCb m_securityPinCb;
            SecProvisioningDbPathCb m_secProvisioningDbPathCb;
            DataProvStatusCb m_dataProvStatusCb;
            ProvConfig m_ProvConfig;
            WiFiOnboadingConnection m_wifiOnboardingconn;

            std::shared_ptr< EnrolleeSecurity > m_enrolleeSecurity;
            CurrentESState m_currentESState;
            bool m_isSecured;

            void InitRemoteEnrolleeStatusHandler (std::shared_ptr< InitRemoteEnrolleeStatus > initRemoteEnrolleeStatus);
            void requestCapabilityStatusHandler (std::shared_ptr< RequestCapabilityStatus > requestCapabilityStatus);
            void dataProvisioningStatusHandler (std::shared_ptr< ProvisioningStatus > provStatus);
            void easySetupSecurityStatusCallback(
            std::shared_ptr< SecProvisioningStatus > secProvisioningResult);
        };
    }
}

#endif //REMOTE_ENROLLEE_H_
