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
#include "generated_resource/resource_detection.h"
#include "base/logging.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kObjectName   = "detection";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseResource*
      * @return static RegisterResource
      */
    RegisterResource ResourceDetection::regResource(kObjectName, &ResourceDetection::Create);

    ResourceDetection::ResourceDetection()
    {
        pGetFunc_ = &CommonWrapperFunctions::GetResourceDetection;
        pPutFunc_ = &CommonWrapperFunctions::PutResourceDetection;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        motionDetectionInUse_ = new _boolean;
        motionDetectionInUse_->SetProperty(0, 1, true);
        motionDetectionInUse_->SetFunctionPtr(NULL, NULL);
        elements_["motionDetectionInUse"] = motionDetectionInUse_;

        motionDetected_ = new _boolean;
        motionDetected_->SetProperty(0, 1, false);
        motionDetected_->SetFunctionPtr(NULL, NULL);
        elements_["motionDetected"] = motionDetected_;

        motionSensitivity_ = new UInt8;
        motionSensitivity_->SetProperty(0, 1, true);
        motionSensitivity_->SetFunctionPtr(NULL, NULL);
        elements_["motionSensitivity"] = motionSensitivity_;

        maxMotionSensitivity_ = new UInt8;
        maxMotionSensitivity_->SetProperty(0, 1, false);
        maxMotionSensitivity_->SetFunctionPtr(NULL, NULL);
        elements_["maxMotionSensitivity"] = maxMotionSensitivity_;

        soundDetectionInUse_ = new _boolean;
        soundDetectionInUse_->SetProperty(0, 1, true);
        soundDetectionInUse_->SetFunctionPtr(NULL, NULL);
        elements_["soundDetectionInUse"] = soundDetectionInUse_;

        soundDetected_ = new _boolean;
        soundDetected_->SetProperty(0, 1, false);
        soundDetected_->SetFunctionPtr(NULL, NULL);
        elements_["soundDetected"] = soundDetected_;

        soundSensitivity_ = new UInt8;
        soundSensitivity_->SetProperty(0, 1, true);
        soundSensitivity_->SetFunctionPtr(NULL, NULL);
        elements_["soundSensitivity"] = soundSensitivity_;

        maxSoundSensitivity_ = new UInt8;
        maxSoundSensitivity_->SetProperty(0, 1, false);
        maxSoundSensitivity_->SetFunctionPtr(NULL, NULL);
        elements_["maxSoundSensitivity"] = maxSoundSensitivity_;

        barrierDetectionInUse_ = new _boolean;
        barrierDetectionInUse_->SetProperty(0, 1, true);
        barrierDetectionInUse_->SetFunctionPtr(NULL, NULL);
        elements_["barrierDetectionInUse"] = barrierDetectionInUse_;

        barrierDetected_ = new _boolean;
        barrierDetected_->SetProperty(0, 1, false);
        barrierDetected_->SetFunctionPtr(NULL, NULL);
        elements_["barrierDetected"] = barrierDetected_;
    }

    ResourceDetection::~ResourceDetection()
    {
        SAFE_DESTROY(this->motionDetectionInUse_);
        SAFE_DESTROY(this->motionDetected_);
        SAFE_DESTROY(this->motionSensitivity_);
        SAFE_DESTROY(this->maxMotionSensitivity_);
        SAFE_DESTROY(this->soundDetectionInUse_);
        SAFE_DESTROY(this->soundDetected_);
        SAFE_DESTROY(this->soundSensitivity_);
        SAFE_DESTROY(this->maxSoundSensitivity_);
        SAFE_DESTROY(this->barrierDetectionInUse_);
        SAFE_DESTROY(this->barrierDetected_);
    }

    /**
      * Create this class.
      * @return BaseResource*
      */
    BaseResource *ResourceDetection::Create()
    {
        DLOG(ERROR) << "ResourceDetection is created";
        return new ResourceDetection;
    }

}