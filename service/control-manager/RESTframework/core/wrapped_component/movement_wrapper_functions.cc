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
#include "wrapped_component/response_listeners/movement_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kBuildPathId = "0";
    static const char *const kBehavior = "behavior";
    static const char *const kDeviceId = "device_id";

// FillGetMovementResponse & ParseMovementRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseMovementRequestBody(base::DictionaryValue *input_dict_value,
            Movement &movement)
    {
        if (!input_dict_value)
        {
            return RT_400_BAD_REQUEST;
        }
        // behavior (string/mandatory)
        std::string behavior;
        if (input_dict_value->GetString(kBehavior, &behavior))
        {
            movement.mpBehavior = new MovementType();
            if (!movement.mpBehavior)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            movement.mpBehavior->value = behavior;
        }
        else
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetMovementResponse(void *input, void *p_out)
    {
        DLOG(INFO) << "FillGetMovementResponse Enter";
        Movement *movement = reinterpret_cast<Movement *>(input);
        DictionaryValue *movement_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!movement || !movement_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        // behavior (string/mandatory)
        if (movement->mpBehavior)
        {
            DLOG(INFO) << "(behavior) is " << movement->mpBehavior->value;
            movement_values_dict->SetWithoutPathExpansion(kBehavior,
                    base::Value::CreateStringValue(movement->mpBehavior->value));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        DLOG(INFO) << "FillGetMovementResponse Exit";
        return RT_200_OK;
    }

    HTTPResponseType CommonWrapperFunctions::GetResourceMovement(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetResourceMovement Enter";
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
        MovementResource *movement_rsc = (MovementResource *)cm_device->getResource("Movement");
        if (!movement_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        movement_rsc->buildPath(kBuildPathId);
        MovementResourceResponseListener *respListener = MovementResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        movement_rsc->addResponseListener(*respListener);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId;
        LOG(ERROR) << "Device getMovement";
        if (movement_rsc->getMovement(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeMovement;
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
                        FillGetMovementResponse(ptr, p_out);
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
            LOG(ERROR) << "getMovement failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    HTTPResponseType CommonWrapperFunctions::PutResourceMovement(void *p_in, void *p_out)
    {
        DLOG(INFO) << "SetResourceMovement Enter";
        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)

        std::string device_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        Movement reqData;
        HTTPResponseType ret = ParseMovementRequestBody(in_dict, reqData);
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
        MovementResource *movement_rsc = (MovementResource *)cm_device->getResource("Movement");
        if (!movement_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        movement_rsc->buildPath(kBuildPathId);
        MovementResourceResponseListener *respListener = MovementResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        movement_rsc->addResponseListener(*respListener);
        ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId;
        LOG(ERROR) << "Device putMovement";
        if (movement_rsc->putMovement(requestId, reqData))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeMovement;
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
            LOG(ERROR) << "putMovement failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }
}

