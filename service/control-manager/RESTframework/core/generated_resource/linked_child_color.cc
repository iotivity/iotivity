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
#include "generated_resource/linked_child_color.h"
#include "base/logging.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kObjectName   = "color";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseLinkedChild*
      * @return static RegisterLinkedChild
      */
    RegisterLinkedChild LinkedChildColor::regChildObject(kObjectName, &LinkedChildColor::Create);

    LinkedChildColor::LinkedChildColor()
    {
        pGetFunc_ = &CommonWrapperFunctions::GetColor;
        pPutFunc_ = &CommonWrapperFunctions::PutColor;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        hue_ = new UInt8;
        hue_->SetProperty(1, 1, true);
        hue_->SetFunctionPtr(NULL, NULL);
        elements_["hue"] = hue_;

        relativeHue_ = new Int16;
        relativeHue_->SetProperty(1, 1, true);
        relativeHue_->SetFunctionPtr(NULL, NULL);
        elements_["relativeHue"] = relativeHue_;

        saturation_ = new UInt8;
        saturation_->SetProperty(1, 1, true);
        saturation_->SetFunctionPtr(NULL, NULL);
        elements_["saturation"] = saturation_;

        relativeSaturation_ = new Int16;
        relativeSaturation_->SetProperty(1, 1, true);
        relativeSaturation_->SetFunctionPtr(NULL, NULL);
        elements_["relativeSaturation"] = relativeSaturation_;

        chromaticityX_ = new UInt16;
        chromaticityX_->SetProperty(1, 1, true);
        chromaticityX_->SetFunctionPtr(NULL, NULL);
        elements_["chromaticityX"] = chromaticityX_;

        chromaticityY_ = new UInt16;
        chromaticityY_->SetProperty(1, 1, true);
        chromaticityY_->SetFunctionPtr(NULL, NULL);
        elements_["chromaticityY"] = chromaticityY_;

        colorTemperature_ = new UInt16;
        colorTemperature_->SetProperty(1, 1, true);
        colorTemperature_->SetFunctionPtr(NULL, NULL);
        elements_["colorTemperature"] = colorTemperature_;
    }

    LinkedChildColor::~LinkedChildColor()
    {
        SAFE_DESTROY(this->hue_);
        SAFE_DESTROY(this->relativeHue_);
        SAFE_DESTROY(this->saturation_);
        SAFE_DESTROY(this->relativeSaturation_);
        SAFE_DESTROY(this->chromaticityX_);
        SAFE_DESTROY(this->chromaticityY_);
        SAFE_DESTROY(this->colorTemperature_);
    }

    /**
      * Create this class.
      * @return BaseLinkedChild*
      */
    BaseLinkedChild *LinkedChildColor::Create()
    {
        DLOG(ERROR) << "LinkedChildColor is created";
        return new LinkedChildColor;
    }

}