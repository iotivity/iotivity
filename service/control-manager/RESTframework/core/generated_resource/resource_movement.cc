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
#include "generated_resource/resource_movement.h"
#include "base/logging.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kObjectName   = "movement";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseResource*
      * @return static RegisterResource
      */
    RegisterResource ResourceMovement::regResource(kObjectName, &ResourceMovement::Create);

    ResourceMovement::ResourceMovement()
    {
        pGetFunc_ = &CommonWrapperFunctions::GetResourceMovement;
        pPutFunc_ = &CommonWrapperFunctions::PutResourceMovement;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        behavior_ = new MovementType;
        behavior_->SetProperty(1, 1, true);
        behavior_->SetFunctionPtr(NULL, NULL, "DriveMode", "drive_mode");
        elements_["behavior"] = behavior_;
    }

    ResourceMovement::~ResourceMovement()
    {
        SAFE_DESTROY(this->behavior_);
    }

    /**
      * Create this class.
      * @return BaseResource*
      */
    BaseResource *ResourceMovement::Create()
    {
        DLOG(ERROR) << "ResourceMovement is created";
        return new ResourceMovement;
    }

}