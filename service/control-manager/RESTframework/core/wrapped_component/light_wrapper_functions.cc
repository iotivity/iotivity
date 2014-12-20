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

#include <string>
#include <vector>

#include "http_header/http_request_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"
#include "wrapped_component/common_wrapper_functions.h"
#include "wrapped_component/control_manager_wrapper.h"
#include "wrapped_component/response_listeners/light_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kDeviceId = "device_id";
    static const char *const kBuildPathId = "0";
    static const char *const kLightPower = "lightPower";
    static const char *const kDimmingLevel = "dimmingLevel";
    static const char *const kMaxDimmingLevel = "maxDimmingLevel";
    static const char *const kRelativeDimmingLevel = "relativeDimmingLevel";
    static const char *const kTransitionTime = "transitionTime";
    static const char *const kActivatePatternID = "activatePatternID";
    static const char *const kOnPatternID = "onPatternID";
    static const char *const kHue = "hue";
    static const char *const kRelativeHue = "relativeHue";
    static const char *const kSaturation = "saturation";
    static const char *const kRelativeSaturation = "relativeSaturation";
    static const char *const kChromaticityX = "chromaticityX";
    static const char *const kChromaticityY = "chromaticityY";
    static const char *const kColorTemperature = "colorTemperature";

// FillGetLightResponse & ParseLightRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseLightRequestBody(base::DictionaryValue *input_dict_value,
            Light &light)
    {
        if (!input_dict_value)
        {
            return RT_400_BAD_REQUEST;
        }
        // lightPower (string/optional)
        std::string light_power;
        if (input_dict_value->GetString(kLightPower, &light_power))
        {
            light.mpLightPower->value = light_power;
        }
        // dimmingLevel (integer/mandatory)
        int dimming_level;
        if (!input_dict_value->GetInteger(kDimmingLevel, &dimming_level))
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        light.mpDimmingLevel = new uint8_t();
        if (!light.mpDimmingLevel)
        {
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        *(light.mpDimmingLevel) = dimming_level;
        // relativeDimmingLevel (inetger/optional)
        int relative_dimming_level;
        if (input_dict_value->GetInteger(kRelativeDimmingLevel, &relative_dimming_level))
        {
            light.mpRelativeDimmingLevel = new int16_t();
            if (!light.mpRelativeDimmingLevel)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(light.mpRelativeDimmingLevel) = relative_dimming_level;
        }
        // transitionTime (integer/optional)
        int transition_time;
        if (input_dict_value->GetInteger(kTransitionTime, &transition_time))
        {
            light.mpTransitionTime = new uint16_t();
            if (!light.mpTransitionTime)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(light.mpTransitionTime) = transition_time;
        }
        // activatePatternID (string/optional)
        std::string activate_pattern_id;
        if (input_dict_value->GetString(kActivatePatternID, &activate_pattern_id))
        {
            light.mpActivatePatternID = new String50();
            if (!light.mpActivatePatternID)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            light.mpActivatePatternID->value = activate_pattern_id;
        }
        // onPatternID (string/optional)
        std::string on_pattern_id;
        if (input_dict_value->GetString(kOnPatternID, &on_pattern_id))
        {
            light.mpOnPatternID = new String50();
            if (!light.mpOnPatternID)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            light.mpOnPatternID->value = on_pattern_id;
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetLightResponse(void *input, void *p_out)
    {
        Light *light = reinterpret_cast<Light *>(input);
        DictionaryValue *light_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!light || !light_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        // lightPower (string/optional)
        if (light->mpLightPower)
        {
            DLOG(INFO) << "(lightPower) is " << light->mpLightPower->value;
            light_values_dict->SetWithoutPathExpansion(kLightPower,
                    base::Value::CreateStringValue(light->mpLightPower->value));
        }
        // dimmingLevel (integer/mandatory)
        if (light->mpDimmingLevel)
        {
            DLOG(INFO) << "(dimmingLevel) is " << light->mpDimmingLevel;
            light_values_dict->SetWithoutPathExpansion(kDimmingLevel,
                    base::Value::CreateIntegerValue(*light->mpDimmingLevel));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // maxDimmingLevel (integer/mandatory)
        if (light->mpMaxDimmingLevel)
        {
            DLOG(INFO) << "(maxDimmingLevel) is " << *light->mpMaxDimmingLevel;
            light_values_dict->SetWithoutPathExpansion(kMaxDimmingLevel,
                    base::Value::CreateIntegerValue(*light->mpMaxDimmingLevel));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // relativeDimmingLevel (integer/optional)
        if (light->mpRelativeDimmingLevel)
        {
            DLOG(INFO) << "(relativeDimmingLevel) is " << *light->mpRelativeDimmingLevel;
            light_values_dict->SetWithoutPathExpansion(kRelativeDimmingLevel,
                    base::Value::CreateIntegerValue(*light->mpRelativeDimmingLevel));
        }
        // transitionTime (integer/optional)
        if (light->mpTransitionTime)
        {
            DLOG(INFO) << "(transitionTime) is " << *light->mpTransitionTime;
            light_values_dict->SetWithoutPathExpansion(kTransitionTime,
                    base::Value::CreateIntegerValue(*light->mpTransitionTime));
        }
        // activatePatternID (string/optional)
        if (light->mpActivatePatternID)
        {
            DLOG(INFO) << "(activatePatternID) is " << light->mpActivatePatternID->value;
            light_values_dict->SetWithoutPathExpansion(kActivatePatternID,
                    base::Value::CreateStringValue(light->mpActivatePatternID->value));
        }
        // onPatternID (string/optional)
        if (light->mpOnPatternID)
        {
            DLOG(INFO) << "(onPatternID) is " << light->mpOnPatternID->value;
            light_values_dict->SetWithoutPathExpansion(kOnPatternID,
                    base::Value::CreateStringValue(light->mpOnPatternID->value));
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillColorDetails(void *p_in, void *p_out)
    {
        DLOG(INFO) << "FillColorDetail";
        CHECK_INPUT_ARGS
        Color *color = reinterpret_cast<Color *>(p_in);
        DictionaryValue *color_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!color || !color_dict)
        {
            LOG(ERROR) << "i/p or o/p param is NULL";
            return RT_400_BAD_REQUEST;
        }
        // hue (integer/mandatory)
        if (color->mpHue)
        {
            DLOG(INFO) << "(hue) is " << *color->mpHue;
            color_dict->SetWithoutPathExpansion(kHue, base::Value::CreateIntegerValue(*color->mpHue));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // relativeHue (integer/mandatory)
        if (color->mpRelativeHue)
        {
            DLOG(INFO) << "(relativeHue) is " << *color->mpRelativeHue;
            color_dict->SetWithoutPathExpansion(kRelativeHue,
                                                base::Value::CreateIntegerValue(*color->mpRelativeHue));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // saturation (integer/mandatory)
        if (color->mpSaturation)
        {
            DLOG(INFO) << "(saturation) is " << *color->mpSaturation;
            color_dict->SetWithoutPathExpansion(kSaturation,
                                                base::Value::CreateIntegerValue(*color->mpSaturation));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // relativeSaturation (integer/mandatory)
        if (color->mpRelativeSaturation)
        {
            DLOG(INFO) << "(relativeSaturation) is " << *color->mpRelativeSaturation;
            color_dict->SetWithoutPathExpansion(kRelativeSaturation,
                                                base::Value::CreateIntegerValue(*color->mpRelativeSaturation));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // chromaticityX (integer/mandatory)
        if (color->mpChromaticityX)
        {
            DLOG(INFO) << "(chromaticityX) is " << *color->mpChromaticityX;
            color_dict->SetWithoutPathExpansion(kChromaticityX,
                                                base::Value::CreateIntegerValue(*color->mpChromaticityX));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // chromaticityY (integer/mandatory)
        if (color->mpChromaticityY)
        {
            DLOG(INFO) << "(chromaticityY) is " << *color->mpChromaticityY;
            color_dict->SetWithoutPathExpansion(kChromaticityY,
                                                base::Value::CreateIntegerValue(*color->mpChromaticityY));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // colorTemperature (integer/mandatory)
        if (color->mpColorTemperature)
        {
            DLOG(INFO) << "(colorTemperature) is " << *color->mpColorTemperature;
            color_dict->SetWithoutPathExpansion(kColorTemperature,
                                                base::Value::CreateIntegerValue(*color->mpColorTemperature));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        DLOG(INFO) << "FillColorDetail Exit";
        return RT_200_OK;
    }

    HTTPResponseType CommonWrapperFunctions::GetResourceLight(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetResourceLight Enter";
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)
        std::string device_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (!cm_wrapper)
        {
            LOG(ERROR) << "ControlManagerWrapper GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        OC::Cm::ControlManager *cm = cm_wrapper->GetControlManagerInstance();
        if (!cm)
        {
            LOG(ERROR) << "ControlManager GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        OC::Cm::Device *cm_device = (reinterpret_cast<OC::Cm::Device *>
                                     (cm->getDeviceFinder()->getDeviceByUUID(device_id)));
        if (!cm_device)
        {
            LOG(ERROR) << "getDeviceByUUID failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        LightResource *light_rsc = (LightResource *)cm_device->getResource("Light");
        if (!light_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        light_rsc->buildPath(kBuildPathId);
        LightResourceResponseListener *respListener = LightResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        light_rsc->addResponseListener(*respListener);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId;
        LOG(ERROR) << "Device getLight";
        if (light_rsc->getLight(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeLight;
                cm_wrapper->AddRequestToCBMap(requestId, cm_db_data);
                boost::unique_lock<boost::mutex> lock(cm_db_data->lock);
                boost::system_time wait_time = boost::get_system_time() + boost::posix_time::milliseconds(
                                                   kRequestTimeout);
                if (!cm_db_data->condition.timed_wait(lock, wait_time))
                {
                    // Timeout
                    LOG(ERROR) << "Timeout happened";
                    ret = RT_408_REQUEST_TIMEOUT;
                }
                else
                {
                    LOG(ERROR) << "Response received from CM";
                    void *ptr = NULL;
                    HTTPResponseType resp_status = RT_UNKNOWN;
                    cm_wrapper->GetdataFromCBMap(requestId, &ptr, &resp_status);
                    if (ptr && resp_status == RT_200_OK)
                    {
                        DLOG(INFO) << "Valid data received from callback";
                        // populate the response string
                        FillGetLightResponse(ptr, p_out);
                    }
                    ret = resp_status;
                }
            }
            else
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            // delete the request id from CBmap, if any
            cm_wrapper->RemoveRequestFromCBMap(requestId);
        }
        else
        {
            LOG(ERROR) << "getLight failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    HTTPResponseType CommonWrapperFunctions::PutResourceLight(void *p_in, void *p_out)
    {
        DLOG(INFO) << "SetResourceLight Enter";
        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)

        std::string device_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        Light reqData;
        HTTPResponseType ret = ParseLightRequestBody(in_dict, reqData);
        if (ret != RT_200_OK)
        {
            LOG(ERROR) << "Error Parsing request body";
            return ret;
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (!cm_wrapper)
        {
            LOG(ERROR) << "ControlManagerWrapper GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        OC::Cm::ControlManager *cm = cm_wrapper->GetControlManagerInstance();
        if (!cm)
        {
            LOG(ERROR) << "ControlManager GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        OC::Cm::Device *cm_device = (reinterpret_cast<OC::Cm::Device *>
                                     (cm->getDeviceFinder()->getDeviceByUUID(device_id)));
        if (!cm_device)
        {
            LOG(ERROR) << "getDeviceByUUID failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        LightResource *light_rsc = (LightResource *)cm_device->getResource("Light");
        if (!light_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        light_rsc->buildPath(kBuildPathId);
        LightResourceResponseListener *respListener = LightResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        light_rsc->addResponseListener(*respListener);
        ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId;
        LOG(ERROR) << "Device putLight";
        if (light_rsc->putLight(requestId, reqData))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeLight;
                cm_wrapper->AddRequestToCBMap(requestId, cm_db_data);
                boost::unique_lock<boost::mutex> lock(cm_db_data->lock);
                boost::system_time wait_time = boost::get_system_time() + boost::posix_time::milliseconds(
                                                   kRequestTimeout);
                if (!cm_db_data->condition.timed_wait(lock, wait_time))
                {
                    // Timeout
                    LOG(ERROR) << "Timeout happened";
                    ret = RT_408_REQUEST_TIMEOUT;
                }
                else
                {
                    LOG(ERROR) << "Response received from CM";
                    void *ptr = NULL;
                    HTTPResponseType resp_status = RT_UNKNOWN;
                    cm_wrapper->GetdataFromCBMap(requestId, &ptr, &resp_status);
                    ret = resp_status;
                }
            }
            else
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            // delete the request id from CBmap, if any
            cm_wrapper->RemoveRequestFromCBMap(requestId);
        }
        else
        {
            LOG(ERROR) << "putLight failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

// GetColor request is almost similar to GetResourceLight
// Since Color is a linked_child to Light, a separate wrapper function is required to populate the 'Color' data.
    HTTPResponseType CommonWrapperFunctions::GetColor(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetColor Enter";
        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)
        std::string device_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (!cm_wrapper)
        {
            LOG(ERROR) << "ControlManagerWrapper GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        OC::Cm::ControlManager *cm = cm_wrapper->GetControlManagerInstance();
        if (!cm)
        {
            LOG(ERROR) << "ControlManager GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        OC::Cm::Device *cm_device = (reinterpret_cast<OC::Cm::Device *>
                                     (cm->getDeviceFinder()->getDeviceByUUID(device_id)));
        if (!cm_device)
        {
            LOG(ERROR) << "getDeviceByUUID failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        LightResource *light_rsc = (LightResource *)cm_device->getResource("Light");
        if (!light_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        LightResourceResponseListener *respListener = LightResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        light_rsc->addResponseListener(*respListener);
        light_rsc->buildPath(kBuildPathId);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device getLight";
        if (light_rsc->getLight(requestId))
        {
            DLOG(INFO) << "color requestId: " << requestId;
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeLight;
                cm_wrapper->AddRequestToCBMap(requestId, cm_db_data);
                boost::unique_lock<boost::mutex> lock(cm_db_data->lock);
                boost::system_time wait_time = boost::get_system_time() + boost::posix_time::milliseconds(
                                                   kRequestTimeout);
                if (!cm_db_data->condition.timed_wait(lock, wait_time))
                {
                    // Timeout
                    LOG(ERROR) << "Timeout happened";
                    ret = RT_408_REQUEST_TIMEOUT;
                }
                else
                {
                    LOG(ERROR) << "Response received from CM";
                    void *ptr = NULL;
                    HTTPResponseType resp_status = RT_UNKNOWN;
                    cm_wrapper->GetdataFromCBMap(requestId, &ptr, &resp_status);
                    if (ptr && resp_status == RT_200_OK)
                    {
                        DLOG(INFO) << "Valid data received from callback  " << ptr;
                        Light *light = reinterpret_cast<Light *>(ptr);
                        // populate the response string
                        FillColorDetails(light->mpColor, p_out);
                    }
                    ret = resp_status;
                }
            }
            else
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            // delete the request id from CBmap, if any
            cm_wrapper->RemoveRequestFromCBMap(requestId);
        }
        else
        {
            LOG(ERROR) << "getLight failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

// PutColor request is almost similar to PutResourceLight
// Since Color is a linked_child to Light, a separate wrapper function is required to modify the 'Color' data.
    HTTPResponseType CommonWrapperFunctions::PutColor(void *p_in, void *p_out)
    {
        DLOG(INFO) << "PutColor Enter";
        // As of now control manager doesn't support color resource
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        return ret;
    }
}

