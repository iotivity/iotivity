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
#include "generated_resource/resource_wifi.h"
#include "base/logging.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kObjectName   = "wifi";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseResource*
      * @return static RegisterResource
      */
    RegisterResource ResourceWiFi::regResource(kObjectName, &ResourceWiFi::Create);

    ResourceWiFi::ResourceWiFi()
    {
        pGetFunc_ = &CommonWrapperFunctions::GetResourceWiFi;
        pPutFunc_ = &CommonWrapperFunctions::PutResourceWiFi;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        operationType_ = new WifiOperationType;
        operationType_->SetProperty(0, 1, true);
        operationType_->SetFunctionPtr(NULL, NULL);
        elements_["operationType"] = operationType_;

        channel_ = new UInt8;
        channel_->SetProperty(0, 1, true);
        channel_->SetFunctionPtr(NULL, NULL);
        elements_["channel"] = channel_;

        ssid_ = new String50;
        ssid_->SetProperty(0, 1, true);
        ssid_->SetFunctionPtr(NULL, NULL);
        elements_["ssid"] = ssid_;

        rssi_ = new Int8;
        rssi_->SetProperty(0, 1, false);
        rssi_->SetFunctionPtr(NULL, NULL);
        elements_["rssi"] = rssi_;

        securityType_ = new String20;
        securityType_->SetProperty(0, 1, true);
        securityType_->SetFunctionPtr(NULL, NULL);
        elements_["securityType"] = securityType_;

        encryptionType_ = new String20;
        encryptionType_->SetProperty(0, 1, true);
        encryptionType_->SetFunctionPtr(NULL, NULL);
        elements_["encryptionType"] = encryptionType_;

        securityKey_ = new String100;
        securityKey_->SetProperty(0, 1, true);
        securityKey_->SetFunctionPtr(NULL, NULL);
        elements_["securityKey"] = securityKey_;

        wifiDirectStatus_ = new WifiEventType;
        wifiDirectStatus_->SetProperty(0, 1, false);
        wifiDirectStatus_->SetFunctionPtr(NULL, NULL);
        elements_["wifiDirectStatus"] = wifiDirectStatus_;

        wpsEnabled_ = new _boolean;
        wpsEnabled_->SetProperty(0, 1, true);
        wpsEnabled_->SetFunctionPtr(NULL, NULL);
        elements_["wpsEnabled"] = wpsEnabled_;

        wpsStatus_ = new WifiEventType;
        wpsStatus_->SetProperty(0, 1, false);
        wpsStatus_->SetFunctionPtr(NULL, NULL);
        elements_["wpsStatus"] = wpsStatus_;
    }

    ResourceWiFi::~ResourceWiFi()
    {
        SAFE_DESTROY(this->operationType_);
        SAFE_DESTROY(this->channel_);
        SAFE_DESTROY(this->ssid_);
        SAFE_DESTROY(this->rssi_);
        SAFE_DESTROY(this->securityType_);
        SAFE_DESTROY(this->encryptionType_);
        SAFE_DESTROY(this->securityKey_);
        SAFE_DESTROY(this->wifiDirectStatus_);
        SAFE_DESTROY(this->wpsEnabled_);
        SAFE_DESTROY(this->wpsStatus_);
    }

    /**
      * Create this class.
      * @return BaseResource*
      */
    BaseResource *ResourceWiFi::Create()
    {
        DLOG(ERROR) << "ResourceWiFi is created";
        return new ResourceWiFi;
    }

}