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
#include "wrapped_component/response_listeners/mode_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kBuildPathId = "0";
    static const char *const kDeviceId = "device_id";
    static const char *const kModeId = "mode_id";
    static const char *const kModes = "modes";
    static const char *const kSupportedModes = "supportedModes";

// FillGetModeResponse & ParseModeRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseModeRequestBody(base::DictionaryValue *input_dict_value, Mode &mode)
    {
        if (!input_dict_value)
        {
            return RT_400_BAD_REQUEST;
        }
        // modes (arrayofstring/mandatory)
        ListValue *modes_list = NULL;
        if (input_dict_value->GetList(kModes, &modes_list))
        {
            mode.mpModes = new ArrayOfString();
            for (int i = 0; i < static_cast<int>(modes_list->GetSize()); i++)
            {
                std::string id;
                modes_list->GetString((size_t)i, &id);
                if (!id.empty())
                {
                    mode.mpModes->members.push_back(id);
                }
            }
        }
        else
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetModeResponse(void *input, void *p_out)
    {
        Mode *mode = reinterpret_cast<Mode *>(input);
        DictionaryValue *mode_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!mode || !mode_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        // supportedModes (ArrayOfString/mandatory)
        if (mode->mpSupportedModes)
        {
            DLOG(INFO) << "(supportedModes) is " << mode->mpSupportedModes;
            ListValue *supported_modes_list = new ListValue;
            if (supported_modes_list)
            {
                std::list<std::string>::const_iterator it_list;
                for (it_list = mode->mpSupportedModes->members.begin();
                     it_list != mode->mpSupportedModes->members.end(); it_list++)
                {
                    supported_modes_list->Append(base::Value::CreateStringValue(*it_list));
                }
                mode_values_dict->Set(kSupportedModes, supported_modes_list);
            }
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // modes (boolean/mandatory)
        if (mode->mpModes)
        {
            DLOG(INFO) << "(modes) is " << mode->mpModes;
            ListValue *modes_list = new ListValue;
            if (modes_list)
            {
                std::list<std::string>::const_iterator it_list;
                for (it_list = mode->mpModes->members.begin(); it_list != mode->mpModes->members.end(); it_list++)
                {
                    modes_list->Append(base::Value::CreateStringValue(*it_list));
                }
                mode_values_dict->Set(kModes, modes_list);
            }
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return RT_200_OK;
    }

    HTTPResponseType CommonWrapperFunctions::GetResourceMode(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetResourceMode Enter";
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
        ModeResource *config_rsc = (ModeResource *)cm_device->getResource("Mode");
        if (!config_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        config_rsc->buildPath(kBuildPathId);
        ModeResourceResponseListener *respListener = ModeResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        config_rsc->addResponseListener(*respListener);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device getMode";
        if (config_rsc->getMode(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeMode;
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
                        FillGetModeResponse(ptr, p_out);
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
            LOG(ERROR) << "getMode failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    HTTPResponseType CommonWrapperFunctions::PutResourceMode(void *p_in, void *p_out)
    {
        DLOG(INFO) << "PutResourceMode Enter";
        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)

        std::string device_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        Mode reqData;
        HTTPResponseType ret = ParseModeRequestBody(in_dict, reqData);
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
        ModeResource *config_rsc = (ModeResource *)cm_device->getResource("Mode");
        if (!config_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        config_rsc->buildPath(kBuildPathId);
        ModeResourceResponseListener *respListener = ModeResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        config_rsc->addResponseListener(*respListener);
        ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId;
        LOG(ERROR) << "Device putMode";
        if (config_rsc->putMode(requestId, reqData))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeMode;
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
            LOG(ERROR) << "putMode failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }
}

