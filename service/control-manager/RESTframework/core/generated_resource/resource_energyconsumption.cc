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
#include "generated_resource/resource_energyconsumption.h"
#include "base/logging.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kObjectName   = "energyconsumption";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseResource*
      * @return static RegisterResource
      */
    RegisterResource ResourceEnergyConsumption::regResource(kObjectName,
            &ResourceEnergyConsumption::Create);

    ResourceEnergyConsumption::ResourceEnergyConsumption()
    {
        pGetFunc_ = &CommonWrapperFunctions::GetResourceEnergyConsumption;
        pPutFunc_ = &CommonWrapperFunctions::PutResourceEnergyConsumption;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        batteryCharge_ = new UInt8;
        batteryCharge_->SetProperty(0, 1, false);
        batteryCharge_->SetFunctionPtr(NULL, NULL, "DeviceEnergyConsumption_batteryCharge",
                                       "service_param");
        elements_["batteryCharge"] = batteryCharge_;

        instantaneousPower_ = new Float32;
        instantaneousPower_->SetProperty(0, 1, false);
        instantaneousPower_->SetFunctionPtr(NULL, NULL);
        elements_["instantaneousPower"] = instantaneousPower_;

        cumulativeConsumption_ = new Float32;
        cumulativeConsumption_->SetProperty(0, 1, false);
        cumulativeConsumption_->SetFunctionPtr(NULL, NULL);
        elements_["cumulativeConsumption"] = cumulativeConsumption_;

        startTime_ = new DateTimeType;
        startTime_->SetProperty(0, 1, false);
        startTime_->SetFunctionPtr(NULL, NULL);
        elements_["startTime"] = startTime_;

        hourlyConsumption_ = new Float32;
        hourlyConsumption_->SetProperty(0, 1, false);
        hourlyConsumption_->SetFunctionPtr(NULL, NULL);
        elements_["hourlyConsumption"] = hourlyConsumption_;

        dailyConsumption_ = new Float32;
        dailyConsumption_->SetProperty(0, 1, false);
        dailyConsumption_->SetFunctionPtr(NULL, NULL);
        elements_["dailyConsumption"] = dailyConsumption_;

        weeklyConsumption_ = new Float32;
        weeklyConsumption_->SetProperty(0, 1, false);
        weeklyConsumption_->SetFunctionPtr(NULL, NULL);
        elements_["weeklyConsumption"] = weeklyConsumption_;

        monthlyConsumption_ = new Float32;
        monthlyConsumption_->SetProperty(0, 1, false);
        monthlyConsumption_->SetFunctionPtr(NULL, NULL);
        elements_["monthlyConsumption"] = monthlyConsumption_;

        reportInterval_ = new UInt32;
        reportInterval_->SetProperty(0, 1, true);
        reportInterval_->SetFunctionPtr(NULL, NULL);
        elements_["reportInterval"] = reportInterval_;

        usageThreshold_ = new Float32;
        usageThreshold_->SetProperty(0, 1, true);
        usageThreshold_->SetFunctionPtr(NULL, NULL);
        elements_["usageThreshold"] = usageThreshold_;

        saveLocation_ = new _anyURI;
        saveLocation_->SetProperty(0, 1, false);
        saveLocation_->SetFunctionPtr(NULL, NULL);
        elements_["saveLocation"] = saveLocation_;
    }

    ResourceEnergyConsumption::~ResourceEnergyConsumption()
    {
        SAFE_DESTROY(this->batteryCharge_);
        SAFE_DESTROY(this->instantaneousPower_);
        SAFE_DESTROY(this->cumulativeConsumption_);
        SAFE_DESTROY(this->startTime_);
        SAFE_DESTROY(this->hourlyConsumption_);
        SAFE_DESTROY(this->dailyConsumption_);
        SAFE_DESTROY(this->weeklyConsumption_);
        SAFE_DESTROY(this->monthlyConsumption_);
        SAFE_DESTROY(this->reportInterval_);
        SAFE_DESTROY(this->usageThreshold_);
        SAFE_DESTROY(this->saveLocation_);
    }

    /**
      * Create this class.
      * @return BaseResource*
      */
    BaseResource *ResourceEnergyConsumption::Create()
    {
        DLOG(ERROR) << "ResourceEnergyConsumption is created";
        return new ResourceEnergyConsumption;
    }

}