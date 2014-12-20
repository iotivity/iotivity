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
#include "wrapped_component/response_listeners/alarm_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/alarms_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kBuildPathId = "0";
    static const char *const kId = "id";
    static const char *const kDescription = "description";
    static const char *const kTriggeredTime = "triggeredTime";
    static const char *const kAlarmType = "alarmType";
    static const char *const kCode = "code";
    static const char *const kMulticastEnabled = "multicastEnabled";
    static const char *const kDevices = "devices";
    static const char *const kDeviceId = "device_id";
    static const char *const kAlarmId = "alarm_id";

    static HTTPResponseType FillGetAlarmsResponse(void *input, void *p_out)
    {
        DLOG(INFO) << "FillGetAlarmsResponse Enter";
        Alarms *alarms = reinterpret_cast<Alarms *>(input);
        std::vector<std::string> *id_list = reinterpret_cast<std::vector<std::string>*>(p_out);
        if (!alarms || !id_list)
        {
            return RT_400_BAD_REQUEST;
        }
        // alarm list
        DLOG(INFO) << "(alarm_list) size is " << alarms->mpAlarm.size();
        if (alarms->mpAlarm.size())
        {
            std::list<Alarm *> alarm_list = alarms->mpAlarm;
            DLOG(INFO) << "(alarm_list) size is " << alarm_list.size();
            std::list<Alarm *>::iterator it_list;
            for (it_list = alarm_list.begin(); it_list != alarm_list.end(); it_list++)
            {
                id_list->push_back((*it_list)->mpId->value);
            }
        }
        DLOG(INFO) << "FillGetAlarmsResponse Exit";
        return RT_200_OK;
    }

    static HTTPResponseType FillGetAlarmResponse(const std::string &alarm_id, void *input, void *p_out)
    {
        Alarm *alarm = reinterpret_cast<Alarm *>(input);
        DictionaryValue *alarm_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!alarm || !alarm_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        alarm_values_dict->SetWithoutPathExpansion(kId, base::Value::CreateStringValue(alarm_id));
        // triggeredTime (string/mandatory)
        if (alarm->mpTriggeredTime)
        {
            DLOG(INFO) << "(triggeredTime) is " << alarm->mpTriggeredTime->value;
            alarm_values_dict->SetWithoutPathExpansion(kTriggeredTime,
                    base::Value::CreateStringValue(alarm->mpTriggeredTime->value));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // alarmType (string/mandatory)
        if (alarm->mpAlarmType)
        {
            DLOG(INFO) << "(alarmType) is " << alarm->mpAlarmType->value;
            alarm_values_dict->SetWithoutPathExpansion(kAlarmType,
                    base::Value::CreateStringValue(alarm->mpAlarmType->value));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // code (string/optional)
        if (alarm->mpCode)
        {
            DLOG(INFO) << "(code) is " << alarm->mpCode->value;
            alarm_values_dict->SetWithoutPathExpansion(kCode,
                    base::Value::CreateStringValue(alarm->mpCode->value));
        }
        // multicastEnabled (boolean/optional)
        if (alarm->mpMulticastEnabled)
        {
            DLOG(INFO) << "(multicastEnabled) is " << alarm->mpMulticastEnabled;
            alarm_values_dict->SetWithoutPathExpansion(kMulticastEnabled,
                    base::Value::CreateBooleanValue(alarm->mpMulticastEnabled));
        }
        // description (string/optional)
        if (alarm->mpDescription)
        {
            DLOG(INFO) << "(description) is " << alarm->mpDescription;
            alarm_values_dict->SetWithoutPathExpansion(kDescription,
                    base::Value::CreateStringValue(alarm->mpDescription->value));
        }
        return RT_200_OK;
    }

    /**
      * Validation Check for ID
      * @param[in] string
      * @param[in] HTTPMessage*
      * @return static HTTPResponseType
      */
    HTTPResponseType CommonWrapperFunctions::ValidateAlarmID(const std::string &id, HTTPMessage *http)
    {
        std::vector<std::string> id_list;
        if (GetAlarmIDList(&id_list, http))
        {
            std::vector<std::string>::iterator it = id_list.begin();
            for (; it != id_list.end(); it++)
            {
                if (*it == id)
                {
                    DLOG(INFO) << "AlarmID found: " << id;
                    return RT_200_OK;
                }
            }
            // parsed the whole list and id not found, so return 404 from here
            return RT_404_ALARM_ID;
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
    HTTPResponseType CommonWrapperFunctions::CreateAlarmID(void *p_in, std::string *id,
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
    HTTPResponseType CommonWrapperFunctions::DeleteAlarmID(void *p_in, const std::string &input_id,
            HTTPMessage *http)
    {
        return RT_501_NOT_IMPLEMENTED;
    }

    /**
      * Get Object ID list
      * @param[out] std::vector<std::string>* : id list
      * @param[in] HTTPMessage*
      * @return int
      */
    int CommonWrapperFunctions::GetAlarmIDList(std::vector<std::string> *id_list, HTTPMessage *http)
    {
        DLOG(INFO) << "GetAccessPointIDList Enter";
        if (!id_list || !http)
        {
            LOG(ERROR) << "Invalid input parameters given";
            return -1;
        }
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
        if (it == uri_field.end()) return 0;
        std::string device_id = *it;
        DLOG(INFO) << "device_id is :  " << device_id;
        if (device_id.empty())
        {
            return -1;
        }
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
        AlarmsResource *alarms_rsc = (AlarmsResource *)cm_device->getResource("Alarms");
        if (!alarms_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return -1;
        }
        alarms_rsc->buildPath(kBuildPathId);
        AlarmsResourceResponseListener *respListener = AlarmsResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return -1;
        }
        alarms_rsc->addResponseListener(*respListener);
        int requestId;
        LOG(ERROR) << "Alarm getAlarms";
        if (alarms_rsc->getAlarms(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeAlarms;
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
                        FillGetAlarmsResponse(ptr, id_list);
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
            LOG(ERROR) << "getAlarms failed";
            return -1;
        }
        return id_list->size();
    }

    HTTPResponseType CommonWrapperFunctions::GetIdentifiedObjectAlarm(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetIdentifiedObjectDevice Enter";
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)
        std::string device_id = "";
        std::string alarm_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        if (!in_dict->GetString(kAlarmId, &alarm_id))
        {
            DLOG(ERROR) << "Input parameter is not get alarm_id";
            return RT_400_BAD_REQUEST;
        }
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
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
            return RT_404_DEVICE_ID;
        }
        AlarmResource *alarm_rsc = (AlarmResource *)cm_device->getResource("Alarm");
        if (!alarm_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        alarm_rsc->buildPath(kBuildPathId, alarm_id);
        AlarmResourceResponseListener *respListener = AlarmResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        alarm_rsc->addResponseListener(*respListener);
        LOG(ERROR) << "Device getAlarm";
        if (alarm_rsc->getAlarm(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeAlarm;
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
                        FillGetAlarmResponse(alarm_id, ptr, p_out);
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
            LOG(ERROR) << "getAlarm failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }
}

