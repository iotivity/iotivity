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
#include "generated_resource/resource_configuration.h"
#include "base/logging.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kObjectName   = "configuration";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseResource*
      * @return static RegisterResource
      */
    RegisterResource ResourceConfiguration::regResource(kObjectName, &ResourceConfiguration::Create);

    ResourceConfiguration::ResourceConfiguration()
    {
        pGetFunc_ = &CommonWrapperFunctions::GetResourceConfiguration;
        pPutFunc_ = &CommonWrapperFunctions::PutResourceConfiguration;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        installedLocation_ = new InternalLocationType;
        installedLocation_->SetProperty(0, 1, true);
        installedLocation_->SetFunctionPtr(NULL, NULL);
        elements_["installedLocation"] = installedLocation_;

        region_ = new String20;
        region_->SetProperty(0, 1, true);
        region_->SetFunctionPtr(NULL, NULL);
        elements_["region"] = region_;

        currency_ = new CurrencyType;
        currency_->SetProperty(0, 1, true);
        currency_->SetFunctionPtr(NULL, NULL);
        elements_["currency"] = currency_;

        remoteControlEnabled_ = new _boolean;
        remoteControlEnabled_->SetProperty(0, 1, false);
        remoteControlEnabled_->SetFunctionPtr(NULL, NULL);
        elements_["remoteControlEnabled"] = remoteControlEnabled_;

        pairingStart_ = new OnType;
        pairingStart_->SetProperty(0, 1, true);
        pairingStart_->SetFunctionPtr(NULL, NULL);
        elements_["pairingStart"] = pairingStart_;

        unpairingStart_ = new OnType;
        unpairingStart_->SetProperty(0, 1, true);
        unpairingStart_->SetFunctionPtr(NULL, NULL);
        elements_["unpairingStart"] = unpairingStart_;

        pairingTimeout_ = new UInt16;
        pairingTimeout_->SetProperty(0, 1, false);
        pairingTimeout_->SetFunctionPtr(NULL, NULL);
        elements_["pairingTimeout"] = pairingTimeout_;

        subDeviceIDs_ = new ArrayOfString;
        subDeviceIDs_->SetProperty(0, 1, true);
        subDeviceIDs_->SetFunctionPtr(NULL, NULL);
        elements_["subDeviceIDs"] = subDeviceIDs_;

        isSubDevice_ = new _boolean;
        isSubDevice_->SetProperty(0, 1, true);
        isSubDevice_->SetFunctionPtr(NULL, NULL);
        elements_["isSubDevice"] = isSubDevice_;

        connectedDeviceType_ = new DeviceType;
        connectedDeviceType_->SetProperty(0, 1, true);
        connectedDeviceType_->SetFunctionPtr(NULL, NULL);
        elements_["connectedDeviceType"] = connectedDeviceType_;
    }

    ResourceConfiguration::~ResourceConfiguration()
    {
        SAFE_DESTROY(this->installedLocation_);
        SAFE_DESTROY(this->region_);
        SAFE_DESTROY(this->currency_);
        SAFE_DESTROY(this->remoteControlEnabled_);
        SAFE_DESTROY(this->pairingStart_);
        SAFE_DESTROY(this->unpairingStart_);
        SAFE_DESTROY(this->pairingTimeout_);
        SAFE_DESTROY(this->subDeviceIDs_);
        SAFE_DESTROY(this->isSubDevice_);
        SAFE_DESTROY(this->connectedDeviceType_);
    }

    /**
      * Create this class.
      * @return BaseResource*
      */
    BaseResource *ResourceConfiguration::Create()
    {
        DLOG(ERROR) << "ResourceConfiguration is created";
        return new ResourceConfiguration;
    }

}