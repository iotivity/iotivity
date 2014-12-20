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

#include "base_resource/base_linked_child.h"
#include "data_types/simple_array_data_type.h"
#include "data_types/simple_data_type.h"
#include "generated_resource/linked_child_specification.h"
#include "base/logging.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kObjectName   = "specification";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseLinkedChild*
      * @return static RegisterLinkedChild
      */
    RegisterLinkedChild LinkedChildSpecification::regChildObject(kObjectName,
            &LinkedChildSpecification::Create);

    LinkedChildSpecification::LinkedChildSpecification()
    {
        pGetFunc_ = &CommonWrapperFunctions::GetSpecification;
        pPutFunc_ = NULL;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        power_ = new String20;
        power_->SetProperty(0, 1, false);
        power_->SetFunctionPtr(NULL, NULL);
        elements_["power"] = power_;

        display_ = new DisplayType;
        display_->SetProperty(0, 1, false);
        display_->SetFunctionPtr(NULL, NULL);
        elements_["display"] = display_;

        sound_ = new SoundType;
        sound_->SetProperty(0, 1, false);
        sound_->SetFunctionPtr(NULL, NULL);
        elements_["sound"] = sound_;

        otaSupported_ = new _boolean;
        otaSupported_->SetProperty(0, 1, false);
        otaSupported_->SetFunctionPtr(NULL, NULL);
        elements_["otaSupported"] = otaSupported_;

        resetDeviceSupported_ = new _boolean;
        resetDeviceSupported_->SetProperty(0, 1, false);
        resetDeviceSupported_->SetFunctionPtr(NULL, NULL);
        elements_["resetDeviceSupported"] = resetDeviceSupported_;

        resetFactoryDefaultSupported_ = new _boolean;
        resetFactoryDefaultSupported_->SetProperty(0, 1, false);
        resetFactoryDefaultSupported_->SetFunctionPtr(NULL, NULL);
        elements_["resetFactoryDefaultSupported"] = resetFactoryDefaultSupported_;

        supportedConnectivity_ = new ArrayOfConnectivityType;
        supportedConnectivity_->SetProperty(0, 1, false);
        supportedConnectivity_->SetFunctionPtr(NULL, NULL);
        elements_["supportedConnectivity"] = supportedConnectivity_;
    }

    LinkedChildSpecification::~LinkedChildSpecification()
    {
        SAFE_DESTROY(this->power_);
        SAFE_DESTROY(this->display_);
        SAFE_DESTROY(this->sound_);
        SAFE_DESTROY(this->otaSupported_);
        SAFE_DESTROY(this->resetDeviceSupported_);
        SAFE_DESTROY(this->resetFactoryDefaultSupported_);
        SAFE_DESTROY(this->supportedConnectivity_);
    }

    /**
      * Create this class.
      * @return BaseLinkedChild*
      */
    BaseLinkedChild *LinkedChildSpecification::Create()
    {
        DLOG(ERROR) << "LinkedChildSpecification is created";
        return new LinkedChildSpecification;
    }

}