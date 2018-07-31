/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef ENROLLEE_RESOURCE_H_
#define ENROLLEE_RESOURCE_H_

#include <mutex>
#include <memory>

#include "ESRichCommon.h"

#include "OCApi.h"

using namespace OC;

/** OIC namespace */
namespace OIC
{
    /** service namespace */
    namespace Service
    {
        class OCResource;
        class EnrolleeSecurity;

        typedef std::function<void(const HeaderOptions& headerOptions,
                                   const OCRepresentation& rep,
                                   const int eCode)> ESEnrolleeResourceCb;
        /**
         * This class contains the resource discovery methods.
         *
         * @see EnrolleeResource
         */
        class EnrolleeResource : public std::enable_shared_from_this<EnrolleeResource>
        {
            friend class EnrolleeSecurity;

        public:
            /**
             * constructor
             * @param[in] resource   oc resource
             */
            EnrolleeResource(std::shared_ptr< OC::OCResource > resource);

            ~EnrolleeResource() = default;

            /**
             * This method is for register the get status callback
             * @param[in] callback     enrolle status callback
             */
            void registerGetStatusCallback(const GetStatusCb callback);

            /**
             * This method is for register get configuration status callback
             * @param[in] callback     enrollee configuration status callback
             */
            void registerGetConfigurationStatusCallback(
                const GetConfigurationStatusCb callback);

            /**
             * This method is for register device property provision callback
             * @param[in] callback     Enrollee device property provisioning status callback
             */
            void registerDevicePropProvStatusCallback(
                const DevicePropProvStatusCb callback);

            /**
             * This method is for register connect request status callback
             * @param[in] calback      connect request status callback
             */
            void registerConnectRequestStatusCallback(
                const ConnectRequestStatusCb callback);

            /**
             * This method is for get enrollee's configuration
             */
            void getConfiguration();

            /**
             * This method is for get device status
             */
            void getStatus();
            /**
             * provision the device properties
             * @param[in] deviceProp   device properties
             */
            void provisionProperties(const DeviceProp& deviceProp);
            /**
             * This API is to request the enrollee to connect
             * @param[in] connectTypes   connect type for enrollee
             */
            void requestToConnect(const std::vector<ES_CONNECT_TYPE> &connectTypes);

        private:
            std::shared_ptr< OC::OCResource > m_ocResource;

            GetStatusCb m_getStatusCb;
            GetConfigurationStatusCb m_getConfigurationStatusCb;
            DevicePropProvStatusCb m_devicePropProvStatusCb;
            ConnectRequestStatusCb m_connectRequestStatusCb;

        private:
            static void onEnrolleeResourceSafetyCB(const HeaderOptions& headerOptions,
                                    const OCRepresentation& rep,
                                    const int eCode,
                                    ESEnrolleeResourceCb cb,
                                    std::weak_ptr<EnrolleeResource> this_ptr);


            void onGetStatusResponse(const HeaderOptions& headerOptions,
                                                const OCRepresentation& rep,
                                                const int eCode);
            void onGetConfigurationResponse(const HeaderOptions& headerOptions,
                                                           const OCRepresentation& rep,
                                                           const int eCode);
            void onProvisioningResponse(const HeaderOptions& headerOptions,
                                                    const OCRepresentation& rep,
                                                    const int eCode);
            void onConnectRequestResponse(const HeaderOptions& headerOptions,
                                                    const OCRepresentation& rep,
                                                    const int eCode);

            // Determine Specification Version Enrollee follows using header options from discovered resource
            int GetOCFServerVersion();
        };
    }
}
#endif
