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

#include <memory>

#include "ESRichCommon.h"
#include "OCApi.h"

using namespace OC;

namespace OIC
{
    namespace Service
    {
        class OCResource;
        class EnrolleeResource;
        class CloudResource;
        class EnrolleeSecurity;

        /**
         * This class represents Remote Enrollee device instance.
         * It will provide APIs for Mediator to perform operations to enable the Enrollee device
         * to connect to the Enroller.
         */
        class RemoteEnrollee
        {
        public:
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
            void  initRemoteEnrollee();

            /**
             * Start provisioning of target Enrollers information to the Enrollee.
             *
             * @throws ESBadRequestException If RemoteEnrollee device not created prior to this call.
             *
             * @see RemoteEnrollee
             */
            void requestPropertyData(RequestPropertyDataStatusCb callback);

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
            void startDataProvisioning(DataProvStatusCb callback);

            /**
             * Start provisioning of target Enrollers information to the Enrollee.
             *
             * @throws ESBadRequestException If RemoteEnrollee device not created prior to this call.
             *
             * @see RemoteEnrollee
             */
            void startCloudProvisioning(CloudProvStatusCb callback);

            /**
             * Get the Provisioning information provided for the current Enrollee.
             *
             * @return DataProvInfo Provisioning information provided for the current Enrollee.
             */
            DataProvInfo getDataProvInfo();

            void setDataProvInfo(const DataProvInfo& );
            void setCloudProvInfo(const CloudProvInfo& );

        private:
            RemoteEnrollee();

            ESResult discoverResource();
            ESResult ESDiscoveryTimeout(unsigned short waittime);
            void onDeviceDiscovered(std::shared_ptr<OC::OCResource> resource);
            void initCloudResource();

            void RequestPropertyDataStatusHandler (std::shared_ptr< RequestPropertyDataStatus > status);
            void dataProvisioningStatusHandler (std::shared_ptr< DataProvisioningStatus > status);
            void cloudProvisioningStatusHandler (std::shared_ptr< CloudProvisioningStatus > status);
            void easySetupSecurityStatusCallback(std::shared_ptr< SecProvisioningStatus > status);

        private:
            std::shared_ptr< OC::OCResource > m_ocResource;
            std::shared_ptr< EnrolleeResource > m_enrolleeResource;
            std::shared_ptr< EnrolleeSecurity > m_enrolleeSecurity;
            std::shared_ptr< CloudResource > m_cloudResource;

            std::string  m_deviceId;
            bool m_discoveryResponse;

            EnrolleeSecStatusCb m_enrolleeSecStatusCb;
            RequestPropertyDataStatusCb m_RequestPropertyDataStatusCb;
            SecurityPinCb m_securityPinCb;
            SecProvisioningDbPathCb m_secProvisioningDbPathCb;
            DataProvStatusCb m_dataProvStatusCb;
            CloudProvStatusCb m_cloudProvStatusCb;

            DataProvInfo m_dataProvInfo;
            CloudProvInfo m_cloudProvInfo;
            PropertyData m_propertyData;

            friend class EasySetup;
        };
    }
}

#endif //REMOTE_ENROLLEE_H_
