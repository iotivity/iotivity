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

#ifndef REMOTE_ENROLLEE_RESOURCE_H_
#define REMOTE_ENROLLEE_RESOURCE_H_

#include <mutex>
#include <memory>

#include "escommon.h"

#include "OCApi.h"

using namespace OC;

namespace OIC
{
    namespace Service
    {
        class OCResource;

        /**
         * This class contains the resource discovery methods.
         *
         * @see RemoteEnrolleeResource
         */
        class RemoteEnrolleeResource
        {
        public:
            typedef std::shared_ptr< RemoteEnrolleeResource > Ptr;

            typedef std::function< void(std::shared_ptr< ProvisioningStatus > provStatus) > ProvStatusCb;

            /**
             * RemoteEnrolleeResource constructor
             *
             * @param host Host information of the Enrollee device obtained from OnBoarding process.
             * @param connectivityType Connectivity type on which OnBoarding is performed
             *
             * @throw ESBadRequestException is thrown if the parameters are invalid
             */
            RemoteEnrolleeResource(const std::string& host, OCConnectivityType connectivityType);

            ~RemoteEnrolleeResource() = default;

            /**
             * Register provisioning status handler.
             *
             * @param callback Callback to get Provisioning status.
             *
             * @throws InvalidParameterException If callback is an empty function or null.
             * @throws ESBadRequestException If registration is already completed.
             *
             * @see ProvisioningStatus
             */
            void registerProvStatusCallback (ProvStatusCb provStatusCb);

            /**
             * Construct Remote OIC resource using the enrollee host and connectivity information
             * provided in the constructor of RemoteEnrolleeResource.
             *
             * @throws InvalidParameterException If the provided information is invalid.
             * @throws ESBadRequestException If resource is already constructed.
             *
             * @see ProvisioningStatus
             */
            void constructResourceObject();

            /**
             * Function for provisioning of Remote Enrollee resource using the information provided.
             *
             * @param enrolleeNWProvInfo Provisioning information for the Enrollee
             *
             * @throws InvalidParameterException If cb is empty.
             */
            virtual void provisionEnrollee(const EnrolleeNWProvInfo& enrolleeNWProvInfo);

            /**
             * Function for unprovisioning of Remote Enrollee and bring to unprovisioned state
             *
             * @throws ESBadRequestException If device is not provisioned already.
             */
            virtual void unprovisionEnrollee();



        private:
            std::shared_ptr< OC::OCResource > m_ocResource;
            std::mutex m_mutex;
            std::string m_host;
            OCConnectivityType m_connectivityType;
            ProvStatusCb m_provStatusCb;
            EnrolleeNWProvInfo m_enrolleeNWProvInfo;

            void onProvResource(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                    const int eCode);

            void onProvPostResource(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                    const int eCode);
        };
    }
}
#endif
