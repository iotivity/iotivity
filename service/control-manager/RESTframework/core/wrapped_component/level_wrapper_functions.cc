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
#include "wrapped_component/response_listeners/level_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kDeviceId = "device_id";
    static const char *const kBuildPathId = "0";
    static const char *const kCurrent = "current";
    static const char *const kDesired = "desired";
    static const char *const kIncrement = "increment";
    static const char *const kMaximum = "maximum";
    static const char *const kMinimum = "minimum";

// FillGetLevelResponse & ParseLevelRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseLevelRequestBody(base::DictionaryValue *input_dict_value,
            Level &level)
    {
        if (!input_dict_value)
        {
            return RT_400_BAD_REQUEST;
        }
        // desired (integer/mandatory)
        int desired;
        if (input_dict_value->GetInteger(kDesired, &desired))
        {
            level.mpDesired = new uint8_t();
            if (!level.mpDesired)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(level.mpDesired) = desired;
        }
        else
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetLevelResponse(void *input, void *p_out)
    {
        Level *level = reinterpret_cast<Level *>(input);
        DictionaryValue *level_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!level || !level_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        // current (integer/mandatory)
        if (level->mpCurrent)
        {
            level_values_dict->SetWithoutPathExpansion(kCurrent,
                    base::Value::CreateIntegerValue(*level->mpCurrent));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // desired (integer/mandatory)
        if (level->mpDesired)
        {
            level_values_dict->SetWithoutPathExpansion(kDesired,
                    base::Value::CreateIntegerValue(*level->mpDesired));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // increment (integer/optional)
        if (level->mpIncrement)
        {
            DLOG(INFO) << "(increment) is " << level->mpIncrement;
            level_values_dict->SetWithoutPathExpansion(kIncrement,
                    base::Value::CreateIntegerValue(*level->mpIncrement));
        }
        // maximum (integer/optional)
        if (level->mpMaximum)
        {
            DLOG(INFO) << "(maximum) is " << level->mpMaximum;
            level_values_dict->SetWithoutPathExpansion(kMaximum,
                    base::Value::CreateIntegerValue(*level->mpMaximum));
        }
        // minimum (integer/optional)
        if (level->mpMinimum)
        {
            DLOG(INFO) << "(minimum) is " << level->mpMinimum;
            level_values_dict->SetWithoutPathExpansion(kMinimum,
                    base::Value::CreateIntegerValue(*level->mpMinimum));
        }
        return RT_200_OK;
    }

    HTTPResponseType CommonWrapperFunctions::GetResourceLevel(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetResourceLevel Enter";
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
        LevelResource *config_rsc = (LevelResource *)cm_device->getResource("Level");
        if (!config_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        config_rsc->buildPath(kBuildPathId);
        LevelResourceResponseListener *respListener = LevelResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        config_rsc->addResponseListener(*respListener);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId;
        LOG(ERROR) << "Device getLevel";
        if (config_rsc->getLevel(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeLevel;
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
                        FillGetLevelResponse(ptr, p_out);
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
            LOG(ERROR) << "getLevel failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    HTTPResponseType CommonWrapperFunctions::PutResourceLevel(void *p_in, void *p_out)
    {
        DLOG(INFO) << "SetResourceLevel Enter";

        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)

        std::string device_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        Level reqData;
        HTTPResponseType ret = ParseLevelRequestBody(in_dict, reqData);
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
        LevelResource *config_rsc = (LevelResource *)cm_device->getResource("Level");
        if (!config_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        config_rsc->buildPath(kBuildPathId);
        LevelResourceResponseListener *respListener = LevelResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        config_rsc->addResponseListener(*respListener);
        ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId;
        LOG(ERROR) << "Device putLevel";
        if (config_rsc->putLevel(requestId, reqData))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeLevel;
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
            LOG(ERROR) << "putLevel failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }
}

