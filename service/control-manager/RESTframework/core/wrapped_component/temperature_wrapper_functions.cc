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
#include "wrapped_component/response_listeners/temperature_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/temperatures_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kBuildPathId = "0";
    static const char *const kDeviceId = "device_id";
    static const char *const kTemperatureId = "temperature_id";
    static const char *const kId = "id";
    static const char *const kName = "name";
    static const char *const kCurrent = "current";
    static const char *const kDesired = "desired";
    static const char *const kIncrement = "increment";
    static const char *const kMaximum = "maximum";
    static const char *const kMinimum = "minimum";
    static const char *const kUnit = "unit";
    static const char *const kDescription = "description";
    static const char *const kDevices = "devices";

// FillGetTemperatureResponse & ParseTemperatureRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseTemperatureRequestBody(base::DictionaryValue *input_dict_value,
            Temperature &temperature)
    {
        if (!input_dict_value)
        {
            return RT_400_BAD_REQUEST;
        }
        // desired (Float32/optional)
        double desired;
        if (input_dict_value->GetDouble(kDesired, &desired))
        {
            temperature.mpDesired = new Float32();
            if (!temperature.mpDesired)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            temperature.mpDesired->value = desired;
        }
        // increment (Float32/optional)
        double increment;
        if (input_dict_value->GetDouble(kIncrement, &increment))
        {
            temperature.mpIncrement = new Float32();
            if (!temperature.mpIncrement)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            temperature.mpIncrement->value = increment;
        }
        // unit (string/mandatory)
        std::string unit;
        if (input_dict_value->GetString(kUnit, &unit))
        {
            temperature.mpUnit = new TemperatureUnitType();
            if (!temperature.mpUnit)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            temperature.mpUnit->value = unit;
        }
        else
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        // description (string/optional)
        std::string description;
        if (input_dict_value->GetString(kDescription, &description))
        {
            temperature.mpDescription = new String50();
            if (!temperature.mpDescription)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            temperature.mpDescription->value = description;
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetTemperatureResponse(const std::string &temperature_id, void *input,
            void *p_out)
    {
        Temperature *temperature = reinterpret_cast<Temperature *>(input);
        DictionaryValue *temperature_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!temperature || !temperature_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        temperature_values_dict->SetWithoutPathExpansion(kId,
                base::Value::CreateStringValue(temperature_id));
        // name (string/optional)
        if (temperature->mpName)
        {
            DLOG(INFO) << "(name) is " << temperature->mpName->value;
            temperature_values_dict->SetWithoutPathExpansion(kName,
                    base::Value::CreateStringValue(temperature->mpName->value));
        }
        // current (Float32/optional)
        if (temperature->mpCurrent)
        {
            DLOG(INFO) << "(current) is " << temperature->mpCurrent->value;
            temperature_values_dict->SetWithoutPathExpansion(kCurrent,
                    base::Value::CreateDoubleValue(temperature->mpCurrent->value));
        }
        // desired (Float32/optional)
        if (temperature->mpDesired)
        {
            DLOG(INFO) << "(desired) is " << temperature->mpDesired->value;
            temperature_values_dict->SetWithoutPathExpansion(kDesired,
                    base::Value::CreateDoubleValue(temperature->mpDesired->value));
        }
        // increment (Float32/optional)
        if (temperature->mpIncrement)
        {
            DLOG(INFO) << "(increment) is " << temperature->mpIncrement->value;
            temperature_values_dict->SetWithoutPathExpansion(kIncrement,
                    base::Value::CreateDoubleValue(temperature->mpIncrement->value));
        }
        // maximum (integer/optional)
        if (temperature->mpMaximum)
        {
            DLOG(INFO) << "(maximum) is " << temperature->mpMaximum->value;
            temperature_values_dict->SetWithoutPathExpansion(kMaximum,
                    base::Value::CreateDoubleValue(temperature->mpMaximum->value));
        }
        // minimum (integer/optional)
        if (temperature->mpMinimum)
        {
            DLOG(INFO) << "(minimum) is " << temperature->mpMinimum;
            temperature_values_dict->SetWithoutPathExpansion(kMinimum,
                    base::Value::CreateDoubleValue(temperature->mpMinimum->value));
        }
        // unit (string/optional)
        if (temperature->mpUnit)
        {
            DLOG(INFO) << "(unit) is " << temperature->mpUnit->value;
            temperature_values_dict->SetWithoutPathExpansion(kUnit,
                    base::Value::CreateStringValue(temperature->mpUnit->value));
        }
        // description (string/optional)
        if (temperature->mpDescription)
        {
            DLOG(INFO) << "(description) is " << temperature->mpDescription;
            temperature_values_dict->SetWithoutPathExpansion(kDescription,
                    base::Value::CreateStringValue(temperature->mpDescription->value));
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetTemperaturesResponse(void *input, void *p_out)
    {
        Temperatures *temperatures = reinterpret_cast<Temperatures *>(input);
        std::vector<std::string> *id_list = reinterpret_cast<std::vector<std::string>*>(p_out);
        if (!temperatures || !id_list)
        {
            return RT_400_BAD_REQUEST;
        }
        // temperature list
        if (temperatures->mpTemperature.size())
        {
            std::list<Temperature *> temp_list = temperatures->mpTemperature;
            DLOG(INFO) << "(temp_list) size is " << temp_list.size();
            std::list<Temperature *>::iterator it_list;
            for (it_list = temp_list.begin(); it_list != temp_list.end(); it_list++)
            {
                id_list->push_back((*it_list)->mpId->value);
            }
        }
        return RT_200_OK;
    }

    /**
      * Validation Check for ID
      * @param[in] string
      * @param[in] HTTPMessage*
      * @return static HTTPResponseType
      */
    HTTPResponseType CommonWrapperFunctions::ValidateTemperatureID(const std::string &id,
            HTTPMessage *http)
    {
        std::vector<std::string> id_list;
        if (GetTemperatureIDList(&id_list, http))
        {
            std::vector<std::string>::iterator it = id_list.begin();
            for (; it != id_list.end(); it++)
            {
                if (*it == id)
                {
                    DLOG(INFO) << "TemperatureID found: " << id;
                    return RT_200_OK;
                }
            }
            // parsed the whole list and id not found, so return 404 from here
            return RT_404_TEMPERATURE_ID;
        }
        return RT_500_INTERNAL_SERVER_ERROR;
    }

    /**
      * Create ID
      * @param[in] void*
      * @param[out] string*
      * @param[in] HTTPMessage*
      * @return static HTTPResponseType
      */
    HTTPResponseType CommonWrapperFunctions::CreateTemperatureID(void *p_in, std::string *id,
            HTTPMessage *http)
    {
        return RT_501_NOT_IMPLEMENTED;
    }

    /**
      * Delete ID
      * @param[in] void*
      * @param[in] const string&
      * @param[in] HTTPMessage*
      * @return static HTTPResponseType
      */
    HTTPResponseType CommonWrapperFunctions::DeleteTemperatureID(void *p_in,
            const std::string &input_id, HTTPMessage *http)
    {
        return RT_501_NOT_IMPLEMENTED;
    }

    /**
      * Get Object ID list
      * @param[out] std::vector<std::string>* : id list
      * @param[in] HTTPMessage*
      * @return int
      */
    int CommonWrapperFunctions::GetTemperatureIDList(std::vector<std::string> *id_list,
            HTTPMessage *http)
    {
        DLOG(INFO) << "GetTemperatureIDList Enter";
        if (!id_list || !http) return -1;
        const std::vector<std::string> &uri_field = http->request.line->GetUriFields();
        std::vector<std::string>::const_iterator it;
        for (it = uri_field.begin(); it != uri_field.end(); it++)
        {
            if (kDevices == *it)
            {
                it++;
                break;
            }
        }
        if (it == uri_field.end()) return -1;
        std::string device_id = *it;
        DLOG(INFO) << "device_id is :  " << device_id;
        int requestId;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (!cm_wrapper)
        {
            LOG(ERROR) << "ControlManagerWrapper GetInstance failed";
            return -1;
        }
        OC::Cm::ControlManager *cm = cm_wrapper->GetControlManagerInstance();
        if (!cm)
        {
            LOG(ERROR) << "ControlManager GetInstance failed";
            return -1;
        }
        OC::Cm::Device *cm_device = (reinterpret_cast<OC::Cm::Device *>
                                     (cm->getDeviceFinder()->getDeviceByUUID(device_id)));
        if (!cm_device)
        {
            LOG(ERROR) << "getDeviceByUUID failed";
            return -1;
        }
        TemperaturesResource *temperatures_rsc = (TemperaturesResource *)
                cm_device->getResource("Temperatures");
        if (!temperatures_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return -1;
        }
        temperatures_rsc->buildPath(kBuildPathId);
        TemperaturesResourceResponseListener *respListener =
            TemperaturesResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return -1;
        }
        temperatures_rsc->addResponseListener(*respListener);
        LOG(ERROR) << "Temperature getTemperatures";
        if (temperatures_rsc->getTemperatures(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeTemperatures;
                cm_wrapper->AddRequestToCBMap(requestId, cm_db_data);
                boost::unique_lock<boost::mutex> lock(cm_db_data->lock);
                boost::system_time wait_time = boost::get_system_time() + boost::posix_time::milliseconds(
                                                   kRequestTimeout);
                if (!cm_db_data->condition.timed_wait(lock, wait_time))
                {
                    // Timeout
                    LOG(ERROR) << "Timeout happened";
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
                        FillGetTemperaturesResponse(ptr, id_list);
                    }
                }
            }
            else
            {
                return -1;
            }
            cm_wrapper->RemoveRequestFromCBMap(requestId);
        }
        else
        {
            LOG(ERROR) << "getTemperatures failed";
            return -1;
        }
        return id_list->size();
    }

    HTTPResponseType CommonWrapperFunctions::GetIdentifiedObjectTemperature(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetIdentifiedObjectDevice Enter";
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)
        std::string device_id = "";
        std::string temperature_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        if (!in_dict->GetString(kTemperatureId, &temperature_id))
        {
            DLOG(ERROR) << "Input parameter is not get temperature_id";
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
        TemperatureResource *temperature_rsc = (TemperatureResource *)cm_device->getResource("Temperature");
        if (!temperature_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        temperature_rsc->buildPath(kBuildPathId, temperature_id);
        TemperatureResourceResponseListener *respListener =
            TemperatureResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        temperature_rsc->addResponseListener(*respListener);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device getTemperature";
        if (temperature_rsc->getTemperature(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeTemperature;
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
                        FillGetTemperatureResponse(temperature_id, ptr, p_out);
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
            LOG(ERROR) << "getTemperature failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    HTTPResponseType CommonWrapperFunctions::PutIdentifiedObjectTemperature(void *p_in, void *p_out)
    {
        DLOG(INFO) << "SetIdentifiedObjectTemperature Enter";

        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)

        std::string device_id = "";
        std::string temperature_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        if (!in_dict->GetString(kTemperatureId, &temperature_id))
        {
            DLOG(ERROR) << "Input parameter is not get temperature_id";
            return RT_400_BAD_REQUEST;
        }
        DLOG(INFO) << "temperature_id: " << temperature_id << "  device_id:  " << device_id;
        Temperature reqData;
        HTTPResponseType ret = ParseTemperatureRequestBody(in_dict, reqData);
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
        TemperatureResource *temperature_rsc = (TemperatureResource *)cm_device->getResource("Temperature");
        if (!temperature_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        temperature_rsc->buildPath(kBuildPathId, temperature_id);
        TemperatureResourceResponseListener *respListener =
            TemperatureResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        temperature_rsc->addResponseListener(*respListener);
        ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device putTemperature";
        if (temperature_rsc->putTemperature(requestId, reqData))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeTemperature;
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
            LOG(ERROR) << "putTemperature failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }
}

