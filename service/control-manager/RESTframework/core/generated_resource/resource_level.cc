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
#include "generated_resource/resource_level.h"
#include "base/logging.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kObjectName   = "level";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseResource*
      * @return static RegisterResource
      */
    RegisterResource ResourceLevel::regResource(kObjectName, &ResourceLevel::Create);

    ResourceLevel::ResourceLevel()
    {
        pGetFunc_ = &CommonWrapperFunctions::GetResourceLevel;
        pPutFunc_ = &CommonWrapperFunctions::PutResourceLevel;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        current_ = new UInt8;
        current_->SetProperty(1, 1, false);
        current_->SetFunctionPtr(NULL, NULL, "DeviceLevel_current", "service_param");
        elements_["current"] = current_;

        desired_ = new UInt8;
        desired_->SetProperty(1, 1, true);
        desired_->SetFunctionPtr(NULL, NULL, "DeviceLevel_desired", "service_param");
        elements_["desired"] = desired_;

        increment_ = new UInt8;
        increment_->SetProperty(0, 1, false);
        increment_->SetFunctionPtr(NULL, NULL, "DeviceLevel_increment", "service_param");
        elements_["increment"] = increment_;

        maximum_ = new UInt8;
        maximum_->SetProperty(0, 1, false);
        maximum_->SetFunctionPtr(NULL, NULL, "DeviceLevel_maximum", "service_param");
        elements_["maximum"] = maximum_;

        minimum_ = new UInt8;
        minimum_->SetProperty(0, 1, false);
        minimum_->SetFunctionPtr(NULL, NULL, "DeviceLevel_minimum", "service_param");
        elements_["minimum"] = minimum_;
    }

    ResourceLevel::~ResourceLevel()
    {
        SAFE_DESTROY(this->current_);
        SAFE_DESTROY(this->desired_);
        SAFE_DESTROY(this->increment_);
        SAFE_DESTROY(this->maximum_);
        SAFE_DESTROY(this->minimum_);
    }

    /**
      * Create this class.
      * @return BaseResource*
      */
    BaseResource *ResourceLevel::Create()
    {
        DLOG(ERROR) << "ResourceLevel is created";
        return new ResourceLevel;
    }

}