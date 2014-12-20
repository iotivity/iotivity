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
#include "generated_resource/resource_information.h"
#include "base/logging.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kObjectName   = "information";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseResource*
      * @return static RegisterResource
      */
    RegisterResource ResourceInformation::regResource(kObjectName, &ResourceInformation::Create);

    ResourceInformation::ResourceInformation()
    {
        pGetFunc_ = &CommonWrapperFunctions::GetResourceInformation;
        pPutFunc_ = NULL;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        description_ = new String100;
        description_->SetProperty(0, 1, false);
        description_->SetFunctionPtr(NULL, NULL);
        elements_["description"] = description_;

        manufacturer_ = new String20;
        manufacturer_->SetProperty(0, 1, false);
        manufacturer_->SetFunctionPtr(NULL, NULL);
        elements_["manufacturer"] = manufacturer_;

        deviceSubType_ = new String30;
        deviceSubType_->SetProperty(0, 1, false);
        deviceSubType_->SetFunctionPtr(NULL, NULL);
        elements_["deviceSubType"] = deviceSubType_;

        modelID_ = new String20;
        modelID_->SetProperty(0, 1, false);
        modelID_->SetFunctionPtr(NULL, NULL);
        elements_["modelID"] = modelID_;

        serialNumber_ = new String50;
        serialNumber_->SetProperty(0, 1, false);
        serialNumber_->SetFunctionPtr(NULL, NULL);
        elements_["serialNumber"] = serialNumber_;

        salesLocation_ = new LocationType;
        salesLocation_->SetProperty(0, 1, false);
        salesLocation_->SetFunctionPtr(NULL, NULL);
        elements_["salesLocation"] = salesLocation_;
    }

    ResourceInformation::~ResourceInformation()
    {
        SAFE_DESTROY(this->description_);
        SAFE_DESTROY(this->manufacturer_);
        SAFE_DESTROY(this->deviceSubType_);
        SAFE_DESTROY(this->modelID_);
        SAFE_DESTROY(this->serialNumber_);
        SAFE_DESTROY(this->salesLocation_);
    }

    /**
      * Create this class.
      * @return BaseResource*
      */
    BaseResource *ResourceInformation::Create()
    {
        DLOG(ERROR) << "ResourceInformation is created";
        return new ResourceInformation;
    }

}