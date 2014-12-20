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

#include "base_resource/base_resource.h"
#include "data_types/simple_array_data_type.h"
#include "data_types/simple_data_type.h"
#include "generated_resource/resource_thermostat.h"
#include "base/logging.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kObjectName   = "thermostat";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseResource*
      * @return static RegisterResource
      */
    RegisterResource ResourceThermostat::regResource(kObjectName, &ResourceThermostat::Create);

    ResourceThermostat::ResourceThermostat()
    {
        pGetFunc_ = &CommonWrapperFunctions::GetResourceThermostat;
        pPutFunc_ = &CommonWrapperFunctions::PutResourceThermostat;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        supportedThermostatModes_ = new ArrayOfString;
        supportedThermostatModes_->SetProperty(1, 1, false);
        supportedThermostatModes_->SetFunctionPtr(NULL, NULL);
        elements_["supportedThermostatModes"] = supportedThermostatModes_;

        thermostatMode_ = new String20;
        thermostatMode_->SetProperty(1, 1, true);
        thermostatMode_->SetFunctionPtr(NULL, NULL);
        elements_["thermostatMode"] = thermostatMode_;

        supportedFanModes_ = new ArrayOfString;
        supportedFanModes_->SetProperty(1, 1, false);
        supportedFanModes_->SetFunctionPtr(NULL, NULL);
        elements_["supportedFanModes"] = supportedFanModes_;

        fanMode_ = new String20;
        fanMode_->SetProperty(1, 1, true);
        fanMode_->SetFunctionPtr(NULL, NULL);
        elements_["fanMode"] = fanMode_;

        supportedOperatingStates_ = new ArrayOfString;
        supportedOperatingStates_->SetProperty(1, 1, false);
        supportedOperatingStates_->SetFunctionPtr(NULL, NULL);
        elements_["supportedOperatingStates"] = supportedOperatingStates_;

        operatingState_ = new String20;
        operatingState_->SetProperty(1, 1, true);
        operatingState_->SetFunctionPtr(NULL, NULL);
        elements_["operatingState"] = operatingState_;
    }

    ResourceThermostat::~ResourceThermostat()
    {
        SAFE_DESTROY(this->supportedThermostatModes_);
        SAFE_DESTROY(this->thermostatMode_);
        SAFE_DESTROY(this->supportedFanModes_);
        SAFE_DESTROY(this->fanMode_);
        SAFE_DESTROY(this->supportedOperatingStates_);
        SAFE_DESTROY(this->operatingState_);
    }

    /**
      * Create this class.
      * @return BaseResource*
      */
    BaseResource *ResourceThermostat::Create()
    {
        DLOG(ERROR) << "ResourceThermostat is created";
        return new ResourceThermostat;
    }

}