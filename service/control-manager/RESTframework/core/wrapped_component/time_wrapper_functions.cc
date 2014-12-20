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
#include "wrapped_component/response_listeners/time_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kBuildPathId = "0";
    static const char *const kCurrentTime = "currentTime";
    static const char *const kDstStartTime = "dstStartTime";
    static const char *const kDstOffset = "dstOffset";
    static const char *const kDstEndTime = "dstEndTime";
    static const char *const kTzOffset = "tzOffset";
    static const char *const kTzID = "tzID";
    static const char *const kUtcTime = "utcTime";

// FillGetTimeResponse & ParseTimeRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseTimeRequestBody(base::DictionaryValue *input_dict_value, Time &time)
    {
        if (!input_dict_value)
        {
            return RT_400_BAD_REQUEST;
        }
        // currentTime (string/optional)
        std::string current_time;
        if (input_dict_value->GetString(kCurrentTime, &current_time))
        {
            time.mpCurrentTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (!time.mpCurrentTime)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            time.mpCurrentTime->value = current_time;
        }
        // dstStartTime (string/optional)
        std::string dst_start_time;
        if (input_dict_value->GetString(kDstStartTime, &dst_start_time))
        {
            time.mpDstStartTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (!time.mpDstStartTime)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            time.mpDstStartTime->value = dst_start_time;
        }
        // dstOffset (string/optional)
        std::string dst_offset;
        if (input_dict_value->GetString(kDstOffset, &dst_offset))
        {
            time.mpDstOffset = new OC::Cm::Serialization::Xsd::TimeOffsetType();
            if (!time.mpDstOffset)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            time.mpDstOffset->value = dst_offset;
        }
        // dstEndTime (string/optional)
        std::string dst_end_time;
        if (input_dict_value->GetString(kDstEndTime, &dst_end_time))
        {
            time.mpDstEndTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (!time.mpDstEndTime)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            time.mpDstEndTime->value = dst_end_time;
        }
        // tzOffset (string/optional)
        std::string tz_offset;
        if (input_dict_value->GetString(kTzOffset, &tz_offset))
        {
            time.mpTzOffset = new OC::Cm::Serialization::Xsd::TimeOffsetType();
            if (!time.mpTzOffset)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            time.mpTzOffset->value = tz_offset;
        }
        // tzID (string/optional)
        std::string tz_id;
        if (input_dict_value->GetString(kTzID, &tz_id))
        {
            time.mpTzID = new TimeZoneType();
            if (!time.mpTzID)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            time.mpTzID->value = tz_id;
        }
        // utcTime (string/optional)
        std::string utc_time;
        if (input_dict_value->GetString(kUtcTime, &utc_time))
        {
            time.mpUtcTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (!time.mpUtcTime)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            time.mpUtcTime->value = utc_time;
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetTimeResponse(void *input, void *p_out)
    {
        Time *time = reinterpret_cast<Time *>(input);
        DictionaryValue *time_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!time || !time_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        // currentTime (string/optional)
        if (time->mpCurrentTime)
        {
            DLOG(INFO) << "(currentTime) is " << time->mpCurrentTime->value;
            time_values_dict->SetWithoutPathExpansion(kCurrentTime,
                    base::Value::CreateStringValue(time->mpCurrentTime->value));
        }
        // dstStartTime (string/optional)
        if (time->mpDstStartTime)
        {
            DLOG(INFO) << "(dstStartTime) is " << time->mpDstStartTime->value;
            time_values_dict->SetWithoutPathExpansion(kDstStartTime,
                    base::Value::CreateStringValue(time->mpDstStartTime->value));
        }
        // dstOffset (string/optional)
        if (time->mpDstOffset)
        {
            DLOG(INFO) << "(dstOffset) is " << time->mpDstOffset->value;
            time_values_dict->SetWithoutPathExpansion(kDstOffset,
                    base::Value::CreateStringValue(time->mpDstOffset->value));
        }
        // dstEndTime (string/optional)
        if (time->mpDstEndTime)
        {
            DLOG(INFO) << "(dstEndTime) is " << time->mpDstEndTime->value;
            time_values_dict->SetWithoutPathExpansion(kDstEndTime,
                    base::Value::CreateStringValue(time->mpDstEndTime->value));
        }
        // tzOffset (string/optional)
        if (time->mpTzOffset)
        {
            DLOG(INFO) << "(tzOffset) is " << time->mpTzOffset->value;
            time_values_dict->SetWithoutPathExpansion(kTzOffset,
                    base::Value::CreateStringValue(time->mpTzOffset->value));
        }
        // tzID (string/optional)
        if (time->mpTzID)
        {
            DLOG(INFO) << "(tzID) is " << time->mpTzID->value;
            time_values_dict->SetWithoutPathExpansion(kTzID,
                    base::Value::CreateStringValue(time->mpTzID->value));
        }
        // utcTime (string/optional)
        if (time->mpUtcTime)
        {
            DLOG(INFO) << "(utcTime) is " << time->mpUtcTime->value;
            time_values_dict->SetWithoutPathExpansion(kUtcTime,
                    base::Value::CreateStringValue(time->mpUtcTime->value));
        }
        return RT_200_OK;
    }

    HTTPResponseType CommonWrapperFunctions::GetResourceTime(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetResourceTime Enter";
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)
        std::string device_id = "";
        if (!in_dict->GetString("device_id", &device_id))
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
        TimeResource *time_rsc = (TimeResource *)cm_device->getResource("Time");
        if (!time_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        time_rsc->buildPath(kBuildPathId);
        TimeResourceResponseListener *respListener = TimeResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        time_rsc->addResponseListener(*respListener);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device getTime";
        if (time_rsc->getTime(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeTime;
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
                        FillGetTimeResponse(ptr, p_out);
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
            LOG(ERROR) << "getTime failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    HTTPResponseType CommonWrapperFunctions::PutResourceTime(void *p_in, void *p_out)
    {
        DLOG(INFO) << "PutResourceTime Enter";

        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)

        std::string device_id = "";
        if (!in_dict->GetString("device_id", &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        Time reqData;
        HTTPResponseType ret = ParseTimeRequestBody(in_dict, reqData);
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
        TimeResource *time_rsc = (TimeResource *)cm_device->getResource("Time");
        if (!time_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        time_rsc->buildPath(kBuildPathId);
        TimeResourceResponseListener *respListener = TimeResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        time_rsc->addResponseListener(*respListener);
        ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device putTime";
        if (time_rsc->putTime(requestId, reqData))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeTime;
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
            LOG(ERROR) << "putTime failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }
}

