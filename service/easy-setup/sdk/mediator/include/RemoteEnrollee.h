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

#include "escommon.h"

namespace OIC
{
    namespace Service
    {
        class RemoteEnrolleeResource;

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
            RemoteEnrollee(const EnrolleeNWProvInfo& enrolleeNWProvInfo) ;

            ~RemoteEnrollee() = default;

            typedef std::shared_ptr< RemoteEnrollee > shared_ptr;

            /**
             * Callback definition to be invoked when EasySetup status is changed.
             * The same callback will be invoked when there is an error in the
             * EasySetup process.
             *
             * @see registerResourceHandler
             * @see ResourceState
             */
            typedef std::function< void(std::shared_ptr< EasySetupStatus >) >
                                                                        EasySetupStatusCB;

            /**
             * Register EasySetup status handler.
             *
             * @param callback Callback to get EasySetup status.
             *
             * @throws InvalidParameterException If callback is an empty function or null.
             * @throws ESBadRequestException If registration is already completed.
             *
             * @see EasySetupStatus
             */
            void registerEasySetupStatusHandler(EasySetupStatusCB callback);

            /**
             * Start provisioning of target Enrollers information to the Enrollee.
             *
             * @throws ESBadRequestException If RemoteEnrollee device not created prior to this call.
             *
             * @see RemoteEnrollee
             */
            virtual void startProvisioning();

            /**
             * Stop provisioning process that is currently in progress.
             *
             * @throws BadRequestException If provisioning is not in progress.
             */
            virtual void stopProvisioning();

            /**
             * Check if the Enrollee device provisioned.
             */
            virtual bool isEnrolleeProvisioned();

            /**
             * Get the Provisioning information provided for the current Enrollee.
             *
             * @return EnrolleeNWProvInfo Provisioning information provided for the current Enrollee.
             */
            virtual EnrolleeNWProvInfo& getEnrolleeProvisioningInfo ();

        private:
            std::shared_ptr< RemoteEnrolleeResource > m_remoteResource;
            EasySetupStatusCB m_easySetupStatusCb;
            EnrolleeNWProvInfo m_enrolleeNWProvInfo;
            CurrentESState m_currentESState;

            void provisioningStatusHandler (std::shared_ptr< ProvisioningStatus > provStatus);
        };
    }
}

#endif //REMOTE_ENROLLEE_H_
