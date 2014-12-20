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
#include "wrapped_component/response_listeners/detection_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kBuildPathId = "0";
    static const char *const kDeviceId = "device_id";
    static const char *const kMotionDetectionInUse = "motionDetectionInUse";
    static const char *const kMotionDetected = "motionDetected";
    static const char *const kMotionSensitivity = "motionSensitivity";
    static const char *const kMaxMotionSensitivity = "maxMotionSensitivity";
    static const char *const kSoundDetectionInUse = "soundDetectionInUse";
    static const char *const kSoundDetected = "soundDetected";
    static const char *const kSoundSensitivity = "soundSensitivity";
    static const char *const kMaxSoundSensitivity = "maxSoundSensitivity";
    static const char *const kBarrierDetectionInUse = "barrierDetectionInUse";
    static const char *const kBarrierDetected = "barrierDetected";

// FillGetDetectionResponse & ParseDetectionRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseDetectionRequestBody(base::DictionaryValue *input_dict_value,
            Detection &detection)
    {
        if (!input_dict_value)
        {
            return RT_400_BAD_REQUEST;
        }
        // motionDetectionInUse (boolean/optional)
        bool in_use;
        if (input_dict_value->GetBoolean(kMotionDetectionInUse, &in_use))
        {
            detection.mpMotionDetectionInUse = new bool();
            if (!detection.mpMotionDetectionInUse)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *detection.mpMotionDetectionInUse = in_use;
        }
        // motionDetected (boolean/optional)
        bool detected;
        if (input_dict_value->GetBoolean(kMotionDetected, &detected))
        {
            detection.mpMotionDetected = new bool();
            if (!detection.mpMotionDetected)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *detection.mpMotionDetected = detected;
        }
        // motionSensitivity (integer/optional)
        int motion_sensitivity;
        if (input_dict_value->GetInteger(kMotionSensitivity, &motion_sensitivity))
        {
            detection.mpMotionSensitivity = new uint8_t();
            if (!detection.mpMotionSensitivity)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(detection.mpMotionSensitivity) = motion_sensitivity;
        }
        // maxMotionSensitivity (integer/optional)
        int max_motion_sensitivity;
        if (input_dict_value->GetInteger(kMaxMotionSensitivity, &max_motion_sensitivity))
        {
            detection.mpMaxMotionSensitivity = new uint8_t();
            if (!detection.mpMaxMotionSensitivity)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(detection.mpMaxMotionSensitivity) = max_motion_sensitivity;
        }
        // soundDetectionInUse (boolean/optional)
        bool sound_detection_in_use;
        if (input_dict_value->GetBoolean(kSoundDetectionInUse, &sound_detection_in_use))
        {
            detection.mpSoundDetectionInUse = new bool();
            if (!detection.mpSoundDetectionInUse)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(detection.mpSoundDetectionInUse) = sound_detection_in_use;
        }
        // soundDetected (boolean/optional)
        bool sound_detected;
        if (input_dict_value->GetBoolean(kSoundDetected, &sound_detected))
        {
            detection.mpSoundDetected = new bool();
            if (!detection.mpSoundDetected)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(detection.mpSoundDetected) = sound_detected;
        }
        // soundSensitivity (integer/optional)
        int sound_sensitivity;
        if (input_dict_value->GetInteger(kSoundSensitivity, &sound_sensitivity))
        {
            detection.mpSoundSensitivity = new uint8_t();
            if (!detection.mpSoundSensitivity)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(detection.mpSoundSensitivity) = sound_sensitivity;
        }
        // maxSoundSensitivity (integer/optional)
        int max_sound_sensitivity;
        if (input_dict_value->GetInteger(kMaxSoundSensitivity, &max_sound_sensitivity))
        {
            detection.mpMaxSoundSensitivity = new uint8_t();
            if (!detection.mpMaxSoundSensitivity)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(detection.mpMaxSoundSensitivity) = max_sound_sensitivity;
        }
        // barrierDetectionInUse (boolean/optional)
        bool detection_in_use;
        if (input_dict_value->GetBoolean(kBarrierDetectionInUse, &detection_in_use))
        {
            detection.mpBarrierDetectionInUse = new bool();
            if (!detection.mpBarrierDetectionInUse)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(detection.mpBarrierDetectionInUse) = detection_in_use;
        }
        // barrierDetected (boolean/optional)
        bool barrier_detected;
        if (input_dict_value->GetBoolean(kBarrierDetected, &barrier_detected))
        {
            detection.mpBarrierDetected = new bool();
            if (!detection.mpBarrierDetected)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(detection.mpBarrierDetected) = barrier_detected;
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetDetectionResponse(void *input, void *p_out)
    {
        Detection *detection = reinterpret_cast<Detection *>(input);
        DictionaryValue *detection_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!detection || !detection_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        // motionDetectionInUse (boolean/optional)
        if (detection->mpMotionDetectionInUse)
        {
            DLOG(INFO) << "(motionDetectionInUse) is " << detection->mpMotionDetectionInUse;
            detection_values_dict->SetWithoutPathExpansion(kMotionDetectionInUse,
                    base::Value::CreateBooleanValue(detection->mpMotionDetectionInUse));
        }
        // motionDetected (boolean/optional)
        if (detection->mpMotionDetected)
        {
            DLOG(INFO) << "(motionDetected) is " << detection->mpMotionDetected;
            detection_values_dict->SetWithoutPathExpansion(kMotionDetected,
                    base::Value::CreateBooleanValue(detection->mpMotionDetected));
        }
        // motionSensitivity (integer/optional)
        if (detection->mpMotionSensitivity)
        {
            DLOG(INFO) << "(motionSensitivity) is " << detection->mpMotionSensitivity;
            detection_values_dict->SetWithoutPathExpansion(kMotionSensitivity,
                    base::Value::CreateIntegerValue(*detection->mpMotionSensitivity));
        }
        // maxMotionSensitivity (integer/optional)
        if (detection->mpMaxMotionSensitivity)
        {
            DLOG(INFO) << "(maxMotionSensitivity) is " << detection->mpMaxMotionSensitivity;
            detection_values_dict->SetWithoutPathExpansion(kMaxMotionSensitivity,
                    base::Value::CreateIntegerValue(*detection->mpMaxMotionSensitivity));
        }
        // soundDetectionInUse (boolean/optional)
        if (detection->mpSoundDetectionInUse)
        {
            DLOG(INFO) << "(soundDetectionInUse) is " << detection->mpSoundDetectionInUse;
            detection_values_dict->SetWithoutPathExpansion(kSoundDetectionInUse,
                    base::Value::CreateBooleanValue(detection->mpSoundDetectionInUse));
        }
        // soundDetected (boolean/optional)
        if (detection->mpSoundDetected)
        {
            DLOG(INFO) << "(soundDetected) is " << detection->mpSoundDetected;
            detection_values_dict->SetWithoutPathExpansion(kSoundDetected,
                    base::Value::CreateBooleanValue(detection->mpSoundDetected));
        }
        // soundSensitivity (integer/optional)
        if (detection->mpSoundSensitivity)
        {
            DLOG(INFO) << "(soundSensitivity) is " << detection->mpSoundSensitivity;
            detection_values_dict->SetWithoutPathExpansion(kSoundSensitivity,
                    base::Value::CreateIntegerValue(*detection->mpSoundSensitivity));
        }
        // maxSoundSensitivity (integer/optional)
        if (detection->mpMaxSoundSensitivity)
        {
            DLOG(INFO) << "(maxSoundSensitivity) is " << detection->mpMaxSoundSensitivity;
            detection_values_dict->SetWithoutPathExpansion(kMaxSoundSensitivity,
                    base::Value::CreateIntegerValue(*detection->mpMaxSoundSensitivity));
        }
        // barrierDetectionInUse (boolean/optional)
        if (detection->mpBarrierDetectionInUse)
        {
            DLOG(INFO) << "(barrierDetectionInUse) is " << detection->mpBarrierDetectionInUse;
            detection_values_dict->SetWithoutPathExpansion(kBarrierDetectionInUse,
                    base::Value::CreateBooleanValue(detection->mpBarrierDetectionInUse));
        }
        // barrierDetected (boolean/optional)
        if (detection->mpBarrierDetected)
        {
            DLOG(INFO) << "(barrierDetected) is " << detection->mpBarrierDetected;
            detection_values_dict->SetWithoutPathExpansion(kBarrierDetected,
                    base::Value::CreateBooleanValue(detection->mpBarrierDetected));
        }
        return RT_200_OK;
    }

    HTTPResponseType CommonWrapperFunctions::GetResourceDetection(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetResourceDetection Enter";
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
        DetectionResource *config_rsc = (DetectionResource *)cm_device->getResource("Detection");
        if (!config_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        config_rsc->buildPath(kBuildPathId);
        DetectionResourceResponseListener *respListener = DetectionResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        config_rsc->addResponseListener(*respListener);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device getDetection";
        if (config_rsc->getDetection(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeDetection;
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
                        FillGetDetectionResponse(ptr, p_out);
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
            LOG(ERROR) << "getDetection failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    HTTPResponseType CommonWrapperFunctions::PutResourceDetection(void *p_in, void *p_out)
    {
        DLOG(INFO) << "SetResourceDetection Enter";

        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)

        std::string device_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        Detection reqData;
        HTTPResponseType ret = ParseDetectionRequestBody(in_dict, reqData);
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
        DetectionResource *config_rsc = (DetectionResource *)cm_device->getResource("Detection");
        if (!config_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        config_rsc->buildPath(kBuildPathId);
        DetectionResourceResponseListener *respListener = DetectionResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        config_rsc->addResponseListener(*respListener);
        ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device putDetection";
        if (config_rsc->putDetection(requestId, reqData))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeDetection;
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
            LOG(ERROR) << "putDetection failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }
}

