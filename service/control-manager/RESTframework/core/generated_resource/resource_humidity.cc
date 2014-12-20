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
#include "generated_resource/resource_humidity.h"
#include "base/logging.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kObjectName   = "humidity";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseResource*
      * @return static RegisterResource
      */
    RegisterResource ResourceHumidity::regResource(kObjectName, &ResourceHumidity::Create);

    ResourceHumidity::ResourceHumidity()
    {
        pGetFunc_ = &CommonWrapperFunctions::GetResourceHumidity;
        pPutFunc_ = &CommonWrapperFunctions::PutResourceHumidity;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        current_ = new Float32;
        current_->SetProperty(1, 1, false);
        current_->SetFunctionPtr(NULL, NULL, "DeviceHumidity_current", "service_param");
        elements_["current"] = current_;

        desired_ = new Float32;
        desired_->SetProperty(0, 1, true);
        desired_->SetFunctionPtr(NULL, NULL);
        elements_["desired"] = desired_;

        unit_ = new HumidityUnitType;
        unit_->SetProperty(1, 1, true);
        unit_->SetFunctionPtr(NULL, NULL, "DeviceHumidity_unit", "service_param");
        elements_["unit"] = unit_;
    }

    ResourceHumidity::~ResourceHumidity()
    {
        SAFE_DESTROY(this->current_);
        SAFE_DESTROY(this->desired_);
        SAFE_DESTROY(this->unit_);
    }

    /**
      * Create this class.
      * @return BaseResource*
      */
    BaseResource *ResourceHumidity::Create()
    {
        DLOG(ERROR) << "ResourceHumidity is created";
        return new ResourceHumidity;
    }

}