/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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

#ifndef WEBSERVICE_SRC_NEW_RESOURCE_HANDLER_GENERATED_RESOURCE_RESOURCE_CONFIGURATION_H_
#define WEBSERVICE_SRC_NEW_RESOURCE_HANDLER_GENERATED_RESOURCE_RESOURCE_CONFIGURATION_H_

#include "base_resource/base_resource.h"
#include "data_types/simple_data_type.h"
#include "data_types/simple_array_data_type.h"

namespace webservice
{

    class ResourceConfiguration : public BaseResource
    {
        public:
            ResourceConfiguration(void);
            virtual ~ResourceConfiguration(void);
            /**
              * Create this class.
              * @return BaseResource*
              */
            static BaseResource *Create(void);

        public:
            /**
              * Register function to create this class.
              * @param[in] string
              * @param[in] BaseResource*
              * @return static RegisterResource
              */
            static RegisterResource regResource;

        private:
            InternalLocationType *installedLocation_;
            String20 *region_;
            CurrencyType *currency_;
            _boolean *remoteControlEnabled_;
            OnType *pairingStart_;
            OnType *unpairingStart_;
            UInt16 *pairingTimeout_;
            ArrayOfString *subDeviceIDs_;
            _boolean *isSubDevice_;
            DeviceType *connectedDeviceType_;
    };

}

#endif /* WEBSERVICE_SRC_NEW_RESOURCE_HANDLER_GENERATED_RESOURCE_RESOURCE_CONFIGURATION_H_ */