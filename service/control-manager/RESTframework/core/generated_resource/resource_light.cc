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
#include "generated_resource/resource_light.h"
#include "base/logging.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kObjectName   = "light";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseResource*
      * @return static RegisterResource
      */
    RegisterResource ResourceLight::regResource(kObjectName, &ResourceLight::Create);

    ResourceLight::ResourceLight()
    {
        pGetFunc_ = &CommonWrapperFunctions::GetResourceLight;
        pPutFunc_ = &CommonWrapperFunctions::PutResourceLight;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        lightPower_ = new OnType;
        lightPower_->SetProperty(0, 1, true);
        lightPower_->SetFunctionPtr(NULL, NULL);
        elements_["lightPower"] = lightPower_;

        dimmingLevel_ = new UInt8;
        dimmingLevel_->SetProperty(1, 1, true);
        dimmingLevel_->SetFunctionPtr(NULL, NULL, "DeviceLight_dimmingLevel", "service_param");
        elements_["dimmingLevel"] = dimmingLevel_;

        maxDimmingLevel_ = new UInt8;
        maxDimmingLevel_->SetProperty(1, 1, false);
        maxDimmingLevel_->SetFunctionPtr(NULL, NULL);
        elements_["maxDimmingLevel"] = maxDimmingLevel_;

        relativeDimmingLevel_ = new Int16;
        relativeDimmingLevel_->SetProperty(0, 1, true);
        relativeDimmingLevel_->SetFunctionPtr(NULL, NULL);
        elements_["relativeDimmingLevel"] = relativeDimmingLevel_;

        transitionTime_ = new UInt16;
        transitionTime_->SetProperty(0, 1, true);
        transitionTime_->SetFunctionPtr(NULL, NULL);
        elements_["transitionTime"] = transitionTime_;

        activatePatternID_ = new String50;
        activatePatternID_->SetProperty(0, 1, true);
        activatePatternID_->SetFunctionPtr(NULL, NULL);
        elements_["activatePatternID"] = activatePatternID_;

        onPatternID_ = new String50;
        onPatternID_->SetProperty(0, 1, true);
        onPatternID_->SetFunctionPtr(NULL, NULL);
        elements_["onPatternID"] = onPatternID_;
    }

    ResourceLight::~ResourceLight()
    {
        SAFE_DESTROY(this->lightPower_);
        SAFE_DESTROY(this->dimmingLevel_);
        SAFE_DESTROY(this->maxDimmingLevel_);
        SAFE_DESTROY(this->relativeDimmingLevel_);
        SAFE_DESTROY(this->transitionTime_);
        SAFE_DESTROY(this->activatePatternID_);
        SAFE_DESTROY(this->onPatternID_);
    }

    /**
      * Create this class.
      * @return BaseResource*
      */
    BaseResource *ResourceLight::Create()
    {
        DLOG(ERROR) << "ResourceLight is created";
        return new ResourceLight;
    }

}