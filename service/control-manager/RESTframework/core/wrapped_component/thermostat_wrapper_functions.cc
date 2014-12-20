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
#include "wrapped_component/response_listeners/thermostat_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kBuildPathId = "0";
    static const char *const kSupportedThermostatModes = "supportedThermostatModes";
    static const char *const kThermostatMode = "thermostatMode";
    static const char *const kSupportedFanModes = "supportedFanModes";
    static const char *const kFanMode = "fanMode";
    static const char *const kSupportedOperatingStates = "supportedOperatingStates";
    static const char *const kOperatingState = "operatingState";
    static const char *const kDeviceId = "device_id";

// FillGetThermostatResponse & ParseThermostatRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseThermostatRequestBody(base::DictionaryValue *input_dict_value,
            Thermostat &thermostat)
    {
        if (!input_dict_value)
        {
            return RT_400_BAD_REQUEST;
        }
        // thermostatMode (string/mandatory)
        std::string thermostat_mode;
        if (input_dict_value->GetString(kThermostatMode, &thermostat_mode))
        {
            thermostat.mpThermostatMode = new String20();
            if (!thermostat.mpThermostatMode)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            thermostat.mpThermostatMode->value = thermostat_mode;
        }
        else
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        // fanMode (string/mandatory)
        std::string fan_mode;
        if (input_dict_value->GetString(kFanMode, &fan_mode))
        {
            thermostat.mpFanMode = new String20();
            if (!thermostat.mpFanMode)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            thermostat.mpFanMode->value = fan_mode;
        }
        else
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        // operatingState (string/mandatory)
        std::string operating_state;
        if (input_dict_value->GetString(kOperatingState, &operating_state))
        {
            thermostat.mpOperatingState = new String20();
            if (!thermostat.mpOperatingState)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            thermostat.mpOperatingState->value = operating_state;
        }
        else
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetThermostatResponse(void *input, void *p_out)
    {
        DLOG(INFO) << "FillGetThermostatResponse Enter";
        Thermostat *thermostat = reinterpret_cast<Thermostat *>(input);
        DictionaryValue *thermostat_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!thermostat || !thermostat_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        // supportedThermostatModes (ArrayOfString/mandatory)
        if (thermostat->mpSupportedThermostatModes)
        {
            DLOG(INFO) << "(supportedThermostatModes) is " << thermostat->mpSupportedThermostatModes;
            ListValue *supported_thermo_modes_list = new ListValue;
            if (supported_thermo_modes_list)
            {
                std::list<std::string>::const_iterator it_list;
                for (it_list = thermostat->mpSupportedThermostatModes->members.begin();
                     it_list != thermostat->mpSupportedThermostatModes->members.end(); it_list++)
                {
                    supported_thermo_modes_list->Append(base::Value::CreateStringValue(*it_list));
                }
                thermostat_values_dict->Set(kSupportedThermostatModes, supported_thermo_modes_list);
            }
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // thermostatMode (string/mandatory)
        if (thermostat->mpThermostatMode)
        {
            DLOG(INFO) << "(thermostatMode) is " << thermostat->mpThermostatMode->value;
            thermostat_values_dict->SetWithoutPathExpansion(kThermostatMode,
                    base::Value::CreateStringValue(thermostat->mpThermostatMode->value));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // supportedFanModes (ArrayOfString/mandatory)
        if (thermostat->mpSupportedFanModes)
        {
            DLOG(INFO) << "(supportedFanModes) is " << thermostat->mpSupportedFanModes;
            ListValue *supported_fan_modes_list = new ListValue;
            if (supported_fan_modes_list)
            {
                std::list<std::string>::const_iterator it_list;
                for (it_list = thermostat->mpSupportedFanModes->members.begin();
                     it_list != thermostat->mpSupportedFanModes->members.end(); it_list++)
                {
                    supported_fan_modes_list->Append(base::Value::CreateStringValue(*it_list));
                }
                thermostat_values_dict->Set(kSupportedFanModes, supported_fan_modes_list);
            }
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // fanMode (string/mandatory)
        if (thermostat->mpFanMode)
        {
            DLOG(INFO) << "(fanMode) is " << thermostat->mpFanMode->value;
            thermostat_values_dict->SetWithoutPathExpansion(kFanMode,
                    base::Value::CreateStringValue(thermostat->mpFanMode->value));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // supportedOperatingStates (ArrayOfString/mandatory)
        if (thermostat->mpSupportedOperatingStates)
        {
            DLOG(INFO) << "(supportedOperatingStates) is " << thermostat->mpSupportedOperatingStates;
            ListValue *supported_operating_states_list = new ListValue;
            if (supported_operating_states_list)
            {
                std::list<std::string>::const_iterator it_list;
                for (it_list = thermostat->mpSupportedOperatingStates->members.begin();
                     it_list != thermostat->mpSupportedOperatingStates->members.end(); it_list++)
                {
                    supported_operating_states_list->Append(base::Value::CreateStringValue(*it_list));
                }
                thermostat_values_dict->Set(kSupportedOperatingStates, supported_operating_states_list);
            }
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // operatingState (string/mandatory)
        if (thermostat->mpOperatingState)
        {
            DLOG(INFO) << "(operatingState) is " << thermostat->mpOperatingState->value;
            thermostat_values_dict->SetWithoutPathExpansion(kOperatingState,
                    base::Value::CreateStringValue(thermostat->mpOperatingState->value));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        DLOG(INFO) << "FillGetThermostatResponse Exit";
        return RT_200_OK;
    }

    HTTPResponseType CommonWrapperFunctions::GetResourceThermostat(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetResourceThermostat Enter";
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
        ThermostatResource *thermo_rsc = (ThermostatResource *)cm_device->getResource("Thermostat");
        if (!thermo_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        thermo_rsc->buildPath(kBuildPathId);
        ThermostatResourceResponseListener *respListener =
            ThermostatResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        thermo_rsc->addResponseListener(*respListener);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId;
        LOG(ERROR) << "Device getThermostat";
        if (thermo_rsc->getThermostat(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeThermostat;
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
                        FillGetThermostatResponse(ptr, p_out);
                    }
                    ret = resp_status;
                }
            }
            else
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            cm_wrapper->RemoveRequestFromCBMap(requestId);
        }
        else
        {
            LOG(ERROR) << "getThermostat failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    HTTPResponseType CommonWrapperFunctions::PutResourceThermostat(void *p_in, void *p_out)
    {
        DLOG(INFO) << "SetResourceThermostat Enter";

        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)

        std::string device_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        Thermostat reqData;
        HTTPResponseType ret = ParseThermostatRequestBody(in_dict, reqData);
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
        ThermostatResource *thermo_rsc = (ThermostatResource *)cm_device->getResource("Thermostat");
        if (!thermo_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        thermo_rsc->buildPath(kBuildPathId);
        ThermostatResourceResponseListener *respListener =
            ThermostatResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        thermo_rsc->addResponseListener(*respListener);
        ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId;
        LOG(ERROR) << "Device putThermostat";
        if (thermo_rsc->putThermostat(requestId, reqData))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeThermostat;
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
            cm_wrapper->RemoveRequestFromCBMap(requestId);
        }
        else
        {
            LOG(ERROR) << "putThermostat failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }
}

