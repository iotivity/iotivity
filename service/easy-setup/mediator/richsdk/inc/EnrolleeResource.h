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

#ifndef ENROLLEE_RESOURCE_H_
#define ENROLLEE_RESOURCE_H_

#include <mutex>
#include <memory>

#include "ESRichCommon.h"

#include "OCApi.h"

using namespace OC;

namespace OIC
{
    namespace Service
    {
        class OCResource;
        class EnrolleeSecurity;

        /**
         * This class contains the resource discovery methods.
         *
         * @see EnrolleeResource
         */
        class EnrolleeResource
        {
            friend class EnrolleeSecurity;

        public:

            /**
             * EnrolleeResource constructor
             *
             * @param enrolleeNWProvInfo Provisioning information for the Enrollee
             *
             * @throw ESBadRequestException is thrown if the parameters are invalid
             */
            EnrolleeResource(std::shared_ptr< OC::OCResource > resource);
            // EnrolleeResource(const DataProvInfo &enrolleeNWProvInfo,
            //                                    const WiFiOnboadingConnection &onboardingconn);

            ~EnrolleeResource() = default;

            /**
             * Register provisioning status handler.
             *
             * @param callback Callback to get Provisioning status.
             *
             * @throws InvalidParameterException If callback is an empty function or null.
             * @throws ESBadRequestException If registration is already completed.
             *
             * @see DataProvisioningStatus
             */
            void registerRequestPropertyDataStatusCallback (RequestPropertyDataStatusCb callback);
            void registerProvStatusCallback (DataProvStatusCb callback);

            void RequestPropertyData();

            /**
             * Function for provisioning of Remote Enrollee resource using the information provided.
             *
             * @throws InvalidParameterException If cb is empty.
             */
            void provisionEnrollee(const DataProvInfo& dataProvInfo);

            /**
             * Function for unprovisioning of Remote Enrollee and bring to unprovisioned state
             *
             * @throws ESBadRequestException If device is not provisioned already.
             */
            void unprovisionEnrollee();

        private:
            std::shared_ptr< OC::OCResource > m_ocResource;

            RequestPropertyDataStatusCb m_RequestPropertyDataStatusCb;
            DataProvStatusCb m_dataProvStatusCb;

            DataProvInfo m_dataProvInfo;

        private:
            void onRequestPropertyDataResponse(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                    const int eCode);
            void checkProvInformationCb(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                    const int eCode);
            PropertyData parsePropertyDataFromRepresentation(const OCRepresentation& rep);
        };
    }
}
#endif
