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
#include "generated_resource/resource_operation.h"
#include "base/logging.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kObjectName   = "operation";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseResource*
      * @return static RegisterResource
      */
    RegisterResource ResourceOperation::regResource(kObjectName, &ResourceOperation::Create);

    ResourceOperation::ResourceOperation()
    {
        pGetFunc_ = &CommonWrapperFunctions::GetResourceOperation;
        pPutFunc_ = &CommonWrapperFunctions::PutResourceOperation;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        power_ = new OnType;
        power_->SetProperty(1, 1, true);
        power_->SetFunctionPtr(NULL, NULL, "DeviceOperation_power", "service_param");
        elements_["power"] = power_;

        state_ = new StateType;
        state_->SetProperty(0, 1, true);
        state_->SetFunctionPtr(NULL, NULL, "DeviceOperation_state", "service_param");
        elements_["state"] = state_;

        supportedProgress_ = new ArrayOfString;
        supportedProgress_->SetProperty(0, 1, false);
        supportedProgress_->SetFunctionPtr(NULL, NULL, "DeviceOperation_supportedProgress",
                                           "supported_progresses");
        elements_["supportedProgress"] = supportedProgress_;

        progress_ = new String20;
        progress_->SetProperty(0, 1, false);
        progress_->SetFunctionPtr(NULL, NULL, "DeviceOperation_progress", "service_param");
        elements_["progress"] = progress_;

        operationTime_ = new TimeType;
        operationTime_->SetProperty(0, 1, true);
        operationTime_->SetFunctionPtr(NULL, NULL, "DeviceOperation_operationTime", "service_param");
        elements_["operationTime"] = operationTime_;

        operationTimeMin_ = new TimeType;
        operationTimeMin_->SetProperty(0, 1, false);
        operationTimeMin_->SetFunctionPtr(NULL, NULL);
        elements_["operationTimeMin"] = operationTimeMin_;

        operationTimeMax_ = new TimeType;
        operationTimeMax_->SetProperty(0, 1, false);
        operationTimeMax_->SetFunctionPtr(NULL, NULL);
        elements_["operationTimeMax"] = operationTimeMax_;

        remainingTime_ = new TimeType;
        remainingTime_->SetProperty(0, 1, false);
        remainingTime_->SetFunctionPtr(NULL, NULL, "DeviceOperation_remainingTime", "service_param");
        elements_["remainingTime"] = remainingTime_;

        runningTime_ = new TimeType;
        runningTime_->SetProperty(0, 1, false);
        runningTime_->SetFunctionPtr(NULL, NULL);
        elements_["runningTime"] = runningTime_;

        delayStartTime_ = new TimeType;
        delayStartTime_->SetProperty(0, 1, true);
        delayStartTime_->SetFunctionPtr(NULL, NULL);
        elements_["delayStartTime"] = delayStartTime_;

        delayEndTime_ = new TimeType;
        delayEndTime_->SetProperty(0, 1, true);
        delayEndTime_->SetFunctionPtr(NULL, NULL);
        elements_["delayEndTime"] = delayEndTime_;

        progressPercentage_ = new UInt8;
        progressPercentage_->SetProperty(0, 1, false);
        progressPercentage_->SetFunctionPtr(NULL, NULL, "DeviceOperation_progressPercentage",
                                            "service_param");
        elements_["progressPercentage"] = progressPercentage_;

        kidsLock_ = new StateType;
        kidsLock_->SetProperty(0, 1, true);
        kidsLock_->SetFunctionPtr(NULL, NULL, "DeviceOperation_kidsLock", "service_param");
        elements_["kidsLock"] = kidsLock_;

        reset_ = new OnType;
        reset_->SetProperty(0, 1, true);
        reset_->SetFunctionPtr(NULL, NULL);
        elements_["reset"] = reset_;
    }

    ResourceOperation::~ResourceOperation()
    {
        SAFE_DESTROY(this->power_);
        SAFE_DESTROY(this->state_);
        SAFE_DESTROY(this->supportedProgress_);
        SAFE_DESTROY(this->progress_);
        SAFE_DESTROY(this->operationTime_);
        SAFE_DESTROY(this->operationTimeMin_);
        SAFE_DESTROY(this->operationTimeMax_);
        SAFE_DESTROY(this->remainingTime_);
        SAFE_DESTROY(this->runningTime_);
        SAFE_DESTROY(this->delayStartTime_);
        SAFE_DESTROY(this->delayEndTime_);
        SAFE_DESTROY(this->progressPercentage_);
        SAFE_DESTROY(this->kidsLock_);
        SAFE_DESTROY(this->reset_);
    }

    /**
      * Create this class.
      * @return BaseResource*
      */
    BaseResource *ResourceOperation::Create()
    {
        DLOG(ERROR) << "ResourceOperation is created";
        return new ResourceOperation;
    }

}