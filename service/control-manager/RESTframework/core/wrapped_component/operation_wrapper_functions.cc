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
#include "wrapped_component/response_listeners/operation_rsc_resp_listener.h"
#include "base/json/json_writer.h"

namespace webservice
{

    static const char *const kBuildPathId = "0";
    static const char *const kPower = "power";
    static const char *const kState = "state";
    static const char *const kSupportedProgress = "supportedProgress";
    static const char *const kProgress = "progress";
    static const char *const kOperationTime = "operationTime";
    static const char *const kOperationTimeMin = "operationTimeMin";
    static const char *const kOperationTimeMax = "operationTimeMax";
    static const char *const kRemainingTime = "remainingTime";
    static const char *const kRunningTime = "runningTime";
    static const char *const kDelayStartTime = "delayStartTime";
    static const char *const kDelayEndTime = "delayEndTime";
    static const char *const kProgressPercentage = "progressPercentage";
    static const char *const kKidsLock = "kidsLock";
    static const char *const kReset = "reset";
    static const char *const kDeviceId = "device_id";

// FillGetOperationResponse & ParseOperationRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseOperationRequestBody(base::DictionaryValue *input_dict_value,
            Operation &operation)
    {
        if (!input_dict_value)
        {
            return RT_400_BAD_REQUEST;
        }
        std::string val;
        base::JSONWriter::Write(input_dict_value, &val);
        DLOG(INFO) << "Recvd req body   " << val;
        // power (string/mandatory)
        std::string power;
        if (!input_dict_value->GetString(kPower, &power))
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        operation.mpPower = new OnType();
        if (!operation.mpPower)
        {
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        operation.mpPower->value = power;
        // state (string/optional)
        std::string state;
        if (input_dict_value->GetString(kState, &state))
        {
            operation.mpState = new StateType();
            if (!operation.mpState)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            operation.mpState->value = state;
        }
        // operationTime (string/optional)
        std::string operation_time;
        if (input_dict_value->GetString(kOperationTime, &operation_time))
        {
            operation.mpOperationTime = new OC::Cm::Serialization::Xsd::TimeType();
            if (!operation.mpOperationTime)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            operation.mpOperationTime->value = operation_time;
        }
        // delayStartTime (string/optional)
        std::string delay_start_time;
        if (input_dict_value->GetString(kDelayStartTime, &delay_start_time))
        {
            operation.mpDelayStartTime = new OC::Cm::Serialization::Xsd::TimeType();
            if (!operation.mpDelayStartTime)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            operation.mpDelayStartTime->value = delay_start_time;
        }
        // delayEndTime (string/optional)
        std::string delay_end_time;
        if (input_dict_value->GetString(kDelayEndTime, &delay_end_time))
        {
            operation.mpDelayEndTime = new OC::Cm::Serialization::Xsd::TimeType();
            if (!operation.mpDelayEndTime)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            operation.mpDelayEndTime->value = delay_end_time;
        }
        // kidsLock (string/optional)
        std::string kids_lock;
        if (input_dict_value->GetString(kKidsLock, &kids_lock))
        {
            operation.mpKidsLock = new StateType();
            if (!operation.mpKidsLock)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            operation.mpKidsLock->value = kids_lock;
        }
        // reset (string/optional)
        std::string reset;
        if (input_dict_value->GetString(kReset, &reset))
        {
            operation.mpReset = new OnType();
            if (!operation.mpReset)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            operation.mpReset->value = reset;
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetOperationResponse(void *input, void *p_out)
    {
        DLOG(INFO) << "FillGetOperationResponse Enter";
        Operation *operation = reinterpret_cast<Operation *>(input);
        DictionaryValue *operation_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!operation || !operation_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        // power (string/mandatory)
        DLOG(INFO) << "(power) is " << operation->mpPower->value;
        operation_values_dict->SetWithoutPathExpansion(kPower,
                base::Value::CreateStringValue(operation->mpPower->value));
        // state (string/mandatory)
        DLOG(INFO) << "(state) is " << operation->mpState->value;
        operation_values_dict->SetWithoutPathExpansion(kState,
                base::Value::CreateStringValue(operation->mpState->value));
        // supportedProgress (boolean/mandatory)
        if (operation->mpSupportedProgress)
        {
            DLOG(INFO) << "(supportedProgress) is " << operation->mpSupportedProgress;
            ListValue *supported_progress_list = new ListValue;
            if (supported_progress_list)
            {
                std::list<std::string>::const_iterator it_list;
                for (it_list = operation->mpSupportedProgress->members.begin();
                     it_list != operation->mpSupportedProgress->members.end(); it_list++)
                {
                    supported_progress_list->Append(base::Value::CreateStringValue(*it_list));
                }
                operation_values_dict->Set(kSupportedProgress, supported_progress_list);
            }
        }
        // progress (string/optional)
        if (operation->mpProgress)
        {
            DLOG(INFO) << "(progress) is " << operation->mpProgress->value;
            operation_values_dict->SetWithoutPathExpansion(kProgress,
                    base::Value::CreateStringValue(operation->mpProgress->value));
        }
        // operationTime (string/optional)
        if (operation->mpOperationTime)
        {
            DLOG(INFO) << "(operationTime) is " << operation->mpOperationTime->value;
            operation_values_dict->SetWithoutPathExpansion(kOperationTime,
                    base::Value::CreateStringValue(operation->mpOperationTime->value));
        }
        // operationTimeMin (string/optional)
        if (operation->mpOperationTimeMin)
        {
            DLOG(INFO) << "(operationTimeMin) is " << operation->mpOperationTimeMin->value;
            operation_values_dict->SetWithoutPathExpansion(kOperationTimeMin,
                    base::Value::CreateStringValue(operation->mpOperationTimeMin->value));
        }
        // operationTimeMax (string/optional)
        if (operation->mpOperationTimeMax)
        {
            DLOG(INFO) << "(operationTimeMax) is " << operation->mpOperationTimeMax->value;
            operation_values_dict->SetWithoutPathExpansion(kOperationTimeMax,
                    base::Value::CreateStringValue(operation->mpOperationTimeMax->value));
        }
        // remainingTime (string/optional)
        if (operation->mpRemainingTime)
        {
            DLOG(INFO) << "(remainingTime) is " << operation->mpRemainingTime->value;
            operation_values_dict->SetWithoutPathExpansion(kRemainingTime,
                    base::Value::CreateStringValue(operation->mpRemainingTime->value));
        }
        // runningTime (string/optional)
        if (operation->mpRunningTime)
        {
            DLOG(INFO) << "(runningTime) is " << operation->mpRunningTime->value;
            operation_values_dict->SetWithoutPathExpansion(kRunningTime,
                    base::Value::CreateStringValue(operation->mpRunningTime->value));
        }
        // delayStartTime (string/optional)
        if (operation->mpDelayStartTime)
        {
            DLOG(INFO) << "(delayStartTime) is " << operation->mpDelayStartTime->value;
            operation_values_dict->SetWithoutPathExpansion(kDelayStartTime,
                    base::Value::CreateStringValue(operation->mpDelayStartTime->value));
        }
        // delayEndTime (string/optional)
        if (operation->mpDelayEndTime)
        {
            DLOG(INFO) << "(delayEndTime) is " << operation->mpDelayEndTime->value;
            operation_values_dict->SetWithoutPathExpansion(kDelayEndTime,
                    base::Value::CreateStringValue(operation->mpDelayEndTime->value));
        }
        // progressPercentage (integer/optional)
        if (operation->mpProgressPercentage)
        {
            DLOG(INFO) << "(progressPercentage) is " << operation->mpProgressPercentage;
            operation_values_dict->SetWithoutPathExpansion(kProgressPercentage,
                    base::Value::CreateIntegerValue(*operation->mpProgressPercentage));
        }
        // kidsLock (string/optional)
        if (operation->mpKidsLock)
        {
            DLOG(INFO) << "(kidsLock) is " << operation->mpKidsLock->value;
            operation_values_dict->SetWithoutPathExpansion(kKidsLock,
                    base::Value::CreateStringValue(operation->mpKidsLock->value));
        }
        // reset (string/optional)
        if (operation->mpReset)
        {
            DLOG(INFO) << "(reset) is " << operation->mpReset->value;
            operation_values_dict->SetWithoutPathExpansion(kReset,
                    base::Value::CreateStringValue(operation->mpReset->value));
        }
        DLOG(INFO) << "FillGetOperationResponse Exit";
        return RT_200_OK;
    }

    HTTPResponseType CommonWrapperFunctions::GetResourceOperation(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetResourceOperation Enter";
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
        OperationResource *operation_rsc = (OperationResource *)cm_device->getResource("Operation");
        if (!operation_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        operation_rsc->buildPath(kBuildPathId);
        OperationResourceResponseListener *respListener = OperationResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        operation_rsc->addResponseListener(*respListener);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId;
        LOG(ERROR) << "Device getOperation";
        if (operation_rsc->getOperation(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeOperation;
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
                        FillGetOperationResponse(ptr, p_out);
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
            LOG(ERROR) << "getOperation failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    HTTPResponseType CommonWrapperFunctions::PutResourceOperation(void *p_in, void *p_out)
    {
        DLOG(INFO) << "SetResourceOperation Enter";
        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)

        std::string device_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        Operation reqData;
        HTTPResponseType ret = ParseOperationRequestBody(in_dict, reqData);
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
        OperationResource *operation_rsc = (OperationResource *)cm_device->getResource("Operation");
        if (!operation_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        operation_rsc->buildPath(kBuildPathId);
        OperationResourceResponseListener *respListener = OperationResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        operation_rsc->addResponseListener(*respListener);
        ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId;
        LOG(ERROR) << "Device putOperation";
        if (operation_rsc->putOperation(requestId, reqData))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeOperation;
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
            LOG(ERROR) << "putOperation failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }
}

