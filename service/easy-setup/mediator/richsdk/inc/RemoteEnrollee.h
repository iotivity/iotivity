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
#include <iostream>
#include <condition_variable>

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

        typedef std::function<void(const std::shared_ptr<OC::OCResource> resource)>  onDeviceDiscoveredCb;

        /**
         * This class represents Remote Enrollee device instance. What operation the class provides:
         * 1) Ownership transfer for enabling secured communication between Mediator and Enrollee
         * devices.
         * 2) Provision WiFi AP information used for which Enrollee is going to connect to the AP
         * 3) Provision Device confiruation setting, i.e. language, country, and etc
         * 4) Provision Cloud information used for which Enrollee is going to register to the cloud
         */
        class RemoteEnrollee : public std::enable_shared_from_this<RemoteEnrollee>
        {
        public:
            ~RemoteEnrollee() = default;

            /**
             * Get an Enrollee's status which includes provisioning status and last error code
             *
             * @param callback will give the requested status
             *
             * @throws ESBadRequestException If RemoteEnrollee device not created prior to this call.
             *
             * @see GetStatusCb
             */
            void getStatus(const GetStatusCb callback);

            /**
             * Get an Enrollee's configuration which includes WiFi supported frequency and device name
             *
             * @param callback will give the requested configuration
             *
             * @throws ESBadRequestException If RemoteEnrollee device not created prior to this call.
             *
             * @see GetConfigurationStatusCb
             */
            void getConfiguration(const GetConfigurationStatusCb callback);

             /**
             * Do security provisioning such as ownership tranfer to Enrollee.
             *
             * @param callback will give the result if the security provisioning succeeds or fails for some reasons
             *
             * @throws ESBadRequestException If RemoteEnrollee device not created prior to this call.
             *
             * @see SecurityProvStatusCb
             */
            void provisionSecurity(const SecurityProvStatusCb callback);

            /**
             * Provision WiFi AP information and device configuration to Enrollee
             * 1. WiFi AP information includes a SSID, password, auth type, and encryption type.
             * 2. Device configuration includes a language (IETF language tags) and country (ISO 3166-1 Alpha-2)
             *
             * @param devProp a data structure storing the above information to be delivered
             * @param callback will give the result if the provisioning succeeds or fails
             *
             * @throws ESBadRequestException If RemoteEnrollee device not created prior to this call.
             *
             * @see DeviceProp
             * @see DevicePropProvStatusCb
             */
            void provisionDeviceProperties(const DeviceProp& devProp,
                                               const DevicePropProvStatusCb callback);

            /**
             * Provision Cloud information to Enrollee, which includes Auth code, auth provider,
             * Cloud interface server URL, and etc.
             * In this function, Discovery for the Enrollee will happen again in a given network.
             * Because, this function is expected to call *AFTER* the Enrollee disconnects its Soft AP
             * and successfully connects to the certain WiFi AP. In that case, Mediator should discover
             * the Enrollee with a certain Device ID in the network.
             *
             * @param cloudProp a data structure storing the above information to be delivered
             * @param callback will give the result if the provisioning succeeds or fails
             *
             * @throws ESBadRequestException If RemoteEnrollee device not created prior to this call.
             *
             * @see CloudProp
             * @see CloudPropProvStatusCb
             */
            void provisionCloudProperties(const CloudProp& cloudProp,
                                              const CloudPropProvStatusCb callback);

        private:
            RemoteEnrollee(const std::shared_ptr< OC::OCResource > resource);

            ESResult discoverResource();

            static void onDiscoveredCallback(const std::shared_ptr<OC::OCResource> resource,
            std::weak_ptr<RemoteEnrollee> this_ptr);

            void onDeviceDiscovered(const std::shared_ptr<OC::OCResource> resource);
            void initCloudResource();

            void getStatusHandler
                (const std::shared_ptr< GetEnrolleeStatus > status) const;
            void getConfigurationStatusHandler
                (const std::shared_ptr< GetConfigurationStatus > status) const;
            void devicePropProvisioningStatusHandler
                (const std::shared_ptr< DevicePropProvisioningStatus > status) const;
            void cloudPropProvisioningStatusHandler
                (const std::shared_ptr< CloudPropProvisioningStatus > status) const;
            void securityStatusHandler
                (const std::shared_ptr< SecProvisioningStatus > status) const;

        private:
            std::shared_ptr< OC::OCResource > m_ocResource;
            std::shared_ptr< EnrolleeResource > m_enrolleeResource;
            std::shared_ptr< EnrolleeSecurity > m_enrolleeSecurity;
            std::shared_ptr< CloudResource > m_cloudResource;

            std::string  m_deviceId;
            bool m_discoveryResponse;

            std::mutex m_discoverymtx;
            std::condition_variable m_cond;

            SecurityProvStatusCb m_securityProvStatusCb;
            GetStatusCb m_getStatusCb;
            GetConfigurationStatusCb m_getConfigurationStatusCb;
            SecurityPinCb m_securityPinCb;
            SecProvisioningDbPathCb m_secProvisioningDbPathCb;
            DevicePropProvStatusCb m_devicePropProvStatusCb;
            CloudPropProvStatusCb m_cloudPropProvStatusCb;

            friend class EasySetup;
        };
    }
}

#endif //REMOTE_ENROLLEE_H_
