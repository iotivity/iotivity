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
#include "wrapped_component/response_listeners/door_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/doors_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kBuildPathId = "0";
    static const char *const kDeviceId = "device_id";
    static const char *const kDoorId = "door_id";
    static const char *const kLock = "lock";
    static const char *const kPassword = "password";
    static const char *const kOneTimePassword = "oneTimePassword";
    static const char *const kMode = "mode";
    static const char *const kEmergencyAlarm = "emergencyAlarm";
    static const char *const kDescription = "description";
    static const char *const kId = "id";
    static const char *const kName = "name";
    static const char *const kOpenState = "openState";
    static const char *const kOpenElapsedTime = "openElapsedTime";
    static const char *const kOpenTime = "openTime";

// FillGetDoorResponse & ParseDoorRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseDoorRequestBody(base::DictionaryValue *input_dict_value, Door &door)
    {
        if (!input_dict_value)
        {
            return RT_400_BAD_REQUEST;
        }
        // lock (string/optional)
        std::string lock;
        if (input_dict_value->GetString(kLock, &lock))
        {
            door.mpLock = new LockType();
            if (!door.mpLock)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            door.mpLock->value = lock;
        }
        // password (string/optional)
        std::string password;
        if (input_dict_value->GetString(kPassword, &password))
        {
            door.mpPassword = new String20();
            if (!door.mpPassword)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            door.mpPassword->value = password;
        }
        // oneTimePassword (string/optional)
        std::string one_time_password;
        if (input_dict_value->GetString(kOneTimePassword, &one_time_password))
        {
            door.mpOneTimePassword = new String20();
            if (!door.mpOneTimePassword)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            door.mpOneTimePassword->value = one_time_password;
        }
        // mode (string/optional)
        std::string mode;
        if (input_dict_value->GetString(kMode, &mode))
        {
            door.mpMode = new DoorLockModeType();
            if (!door.mpMode)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            door.mpMode->value = mode;
        }
        // emergencyAlarm (boolean/optional)
        bool alarm;
        if (input_dict_value->GetBoolean(kEmergencyAlarm, &alarm))
        {
            door.mpEmergencyAlarm = new bool();
            if (!door.mpEmergencyAlarm)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *door.mpEmergencyAlarm = alarm;
        }
        // description (boolean/optional)
        std::string description;
        if (input_dict_value->GetString(kDescription, &description))
        {
            door.mpDescription = new String50();
            if (!door.mpDescription)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            door.mpDescription->value = description;
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetDoorResponse(const std::string &door_id, void *input, void *p_out)
    {
        Door *door = reinterpret_cast<Door *>(input);
        DictionaryValue *door_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!door || !door_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        door_values_dict->SetWithoutPathExpansion(kId, base::Value::CreateStringValue(door_id));
        // name (string/optional)
        if (door->mpName)
        {
            DLOG(INFO) << "(name) is " << door->mpName->value;
            door_values_dict->SetWithoutPathExpansion(kName,
                    base::Value::CreateStringValue(door->mpName->value));
        }
        // openState (string/optional)
        if (door->mpOpenState)
        {
            DLOG(INFO) << "(openState) is " << door->mpOpenState->value;
            door_values_dict->SetWithoutPathExpansion(kOpenState,
                    base::Value::CreateStringValue(door->mpOpenState->value));
        }
        // openElapsedTime (string/optional)
        if (door->mpOpenElapsedTime)
        {
            DLOG(INFO) << "(openElapsedTime) is " << door->mpOpenElapsedTime->value;
            door_values_dict->SetWithoutPathExpansion(kOpenElapsedTime,
                    base::Value::CreateStringValue(door->mpOpenElapsedTime->value));
        }
        // openTime (string/optional)
        if (door->mpOpenTime)
        {
            DLOG(INFO) << "(openTime) is " << door->mpOpenTime->value;
            door_values_dict->SetWithoutPathExpansion(kOpenTime,
                    base::Value::CreateStringValue(door->mpOpenTime->value));
        }
        // lock (string/optional)
        if (door->mpLock)
        {
            DLOG(INFO) << "(lock) is " << door->mpLock->value;
            door_values_dict->SetWithoutPathExpansion(kLock,
                    base::Value::CreateStringValue(door->mpLock->value));
        }
        // description (string/optional)
        if (door->mpDescription)
        {
            DLOG(INFO) << "(description) is " << door->mpDescription;
            door_values_dict->SetWithoutPathExpansion(kDescription,
                    base::Value::CreateStringValue(door->mpDescription->value));
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetDoorsResponse(void *input, void *p_out)
    {
        Doors *doors = reinterpret_cast<Doors *>(input);
        std::vector<std::string> *id_list = reinterpret_cast<std::vector<std::string>*>(p_out);
        if (!doors || !id_list)
        {
            return RT_400_BAD_REQUEST;
        }
        // door list
        if (doors->mpDoor.size())
        {
            std::list<Door *> door_list = doors->mpDoor;
            DLOG(INFO) << "(door_list) size is " << door_list.size();
            std::list<Door *>::iterator it_list;
            for (it_list = door_list.begin(); it_list != door_list.end(); it_list++)
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
    HTTPResponseType CommonWrapperFunctions::ValidateDoorID(const std::string &id, HTTPMessage *http)
    {
        std::vector<std::string> id_list;
        if (GetDoorIDList(&id_list, http))
        {
            std::vector<std::string>::iterator it = id_list.begin();
            for (; it != id_list.end(); it++)
            {
                if (*it == id)
                {
                    DLOG(INFO) << "DoorID found: " << id;
                    return RT_200_OK;
                }
            }
            // parsed the whole list and id not found, so return 404 from here
            return RT_404_DOOR_ID;
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
    HTTPResponseType CommonWrapperFunctions::CreateDoorID(void *p_in, std::string *id,
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
    HTTPResponseType CommonWrapperFunctions::DeleteDoorID(void *p_in, const std::string &input_id,
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
    int CommonWrapperFunctions::GetDoorIDList(std::vector<std::string> *id_list, HTTPMessage *http)
    {
        DLOG(INFO) << "GetDoorIDList Enter";
        if (!id_list || !http) return -1;
        const std::vector<std::string> &uri_field = http->request.line->GetUriFields();
        std::vector<std::string>::const_iterator it;
        for (it = uri_field.begin(); it != uri_field.end(); it++)
        {
            if ("devices" == *it)
            {
                it++;
                break;
            }
        }
        if (it == uri_field.end()) return -1;
        std::string device_id = *it;
        DLOG(INFO) << "device_id is :  " << device_id;
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
        DoorsResource *doors_rsc = (DoorsResource *)cm_device->getResource("Doors");
        if (!doors_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return -1;
        }
        doors_rsc->buildPath(kBuildPathId);
        DoorsResourceResponseListener *respListener = DoorsResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return -1;
        }
        doors_rsc->addResponseListener(*respListener);
        int requestId;
        LOG(ERROR) << "Door getDoors";
        if (doors_rsc->getDoors(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeDoors;
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
                        FillGetDoorsResponse(ptr, id_list);
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
            LOG(ERROR) << "getDoors failed";
            return -1;
        }
        return id_list->size();;
    }

    HTTPResponseType CommonWrapperFunctions::GetIdentifiedObjectDoor(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetIdentifiedObjectDevice Enter";
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)
        std::string device_id = "";
        std::string door_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        if (!in_dict->GetString(kDoorId, &door_id))
        {
            DLOG(ERROR) << "Input parameter is not get door_id";
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
        DoorResource *door_rsc = (DoorResource *)cm_device->getResource("Door");
        if (!door_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        door_rsc->buildPath(kBuildPathId, door_id);
        DoorResourceResponseListener *respListener = DoorResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        door_rsc->addResponseListener(*respListener);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId;
        LOG(ERROR) << "Device getDoor";
        if (door_rsc->getDoor(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeDoor;
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
                        FillGetDoorResponse(door_id, ptr, p_out);
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
            LOG(ERROR) << "getDoor failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    HTTPResponseType CommonWrapperFunctions::PutIdentifiedObjectDoor(void *p_in, void *p_out)
    {
        DLOG(INFO) << "PutIdentifiedObjectDoor Enter";

        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)

        std::string device_id = "";
        std::string door_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        if (!in_dict->GetString(kDoorId, &door_id))
        {
            DLOG(ERROR) << "Input parameter is not get door_id";
            return RT_400_BAD_REQUEST;
        }
        Door reqData;
        HTTPResponseType ret = ParseDoorRequestBody(in_dict, reqData);
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
        DoorResource *door_rsc = (DoorResource *)cm_device->getResource("Door");
        if (!door_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        door_rsc->buildPath(kBuildPathId, door_id);
        DoorResourceResponseListener *respListener = DoorResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        door_rsc->addResponseListener(*respListener);
        ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId;
        LOG(ERROR) << "Device putDoor";
        if (door_rsc->putDoor(requestId, reqData))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeDoor;
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
            LOG(ERROR) << "putDoor failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }
}

