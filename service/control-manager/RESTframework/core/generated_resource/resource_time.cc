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
#include "generated_resource/resource_time.h"
#include "base/logging.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kObjectName   = "time";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseResource*
      * @return static RegisterResource
      */
    RegisterResource ResourceTime::regResource(kObjectName, &ResourceTime::Create);

    ResourceTime::ResourceTime()
    {
        pGetFunc_ = &CommonWrapperFunctions::GetResourceTime;
        pPutFunc_ = &CommonWrapperFunctions::PutResourceTime;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        currentTime_ = new DateTimeType;
        currentTime_->SetProperty(1, 1, true);
        currentTime_->SetFunctionPtr(NULL, NULL);
        elements_["currentTime"] = currentTime_;

        dstStartTime_ = new DateTimeType;
        dstStartTime_->SetProperty(0, 1, true);
        dstStartTime_->SetFunctionPtr(NULL, NULL);
        elements_["dstStartTime"] = dstStartTime_;

        dstOffset_ = new TimeOffsetType;
        dstOffset_->SetProperty(0, 1, true);
        dstOffset_->SetFunctionPtr(NULL, NULL);
        elements_["dstOffset"] = dstOffset_;

        dstEndTime_ = new DateTimeType;
        dstEndTime_->SetProperty(0, 1, true);
        dstEndTime_->SetFunctionPtr(NULL, NULL);
        elements_["dstEndTime"] = dstEndTime_;

        tzOffset_ = new TimeOffsetType;
        tzOffset_->SetProperty(0, 1, true);
        tzOffset_->SetFunctionPtr(NULL, NULL);
        elements_["tzOffset"] = tzOffset_;

        tzID_ = new TimeZoneType;
        tzID_->SetProperty(0, 1, true);
        tzID_->SetFunctionPtr(NULL, NULL);
        elements_["tzID"] = tzID_;

        utcTime_ = new DateTimeType;
        utcTime_->SetProperty(0, 1, true);
        utcTime_->SetFunctionPtr(NULL, NULL);
        elements_["utcTime"] = utcTime_;
    }

    ResourceTime::~ResourceTime()
    {
        SAFE_DESTROY(this->currentTime_);
        SAFE_DESTROY(this->dstStartTime_);
        SAFE_DESTROY(this->dstOffset_);
        SAFE_DESTROY(this->dstEndTime_);
        SAFE_DESTROY(this->tzOffset_);
        SAFE_DESTROY(this->tzID_);
        SAFE_DESTROY(this->utcTime_);
    }

    /**
      * Create this class.
      * @return BaseResource*
      */
    BaseResource *ResourceTime::Create()
    {
        DLOG(ERROR) << "ResourceTime is created";
        return new ResourceTime;
    }

}