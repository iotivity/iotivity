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
            EnrolleeResource(std::shared_ptr< OC::OCResource > resource);

            ~EnrolleeResource() = default;

            void registerGetConfigurationStatusCallback (GetConfigurationStatusCb callback);
            void registerDevicePropProvStatusCallback (DevicePropProvStatusCb callback);

            void getConfiguration();

            void provisionEnrollee(const DeviceProp& deviceProp);

        private:
            std::shared_ptr< OC::OCResource > m_ocResource;

            GetConfigurationStatusCb m_getConfigurationStatusCb;
            DevicePropProvStatusCb m_devicePropProvStatusCb;

        private:
            void onGetConfigurationResponse(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                    const int eCode);
            void checkProvInformationCb(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                    const int eCode);
            EnrolleeConf parseEnrolleeConfFromRepresentation(const OCRepresentation& rep);
        };
    }
}
#endif
