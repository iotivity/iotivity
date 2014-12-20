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

#include "base/slep_util/time_util.h"
#include "base/string_util.h"
#include "boost/bimap.hpp"
#include "http_header/http_request_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"
#include "wrapped_component/common_wrapper_functions.h"
#include "wrapped_component/control_manager_wrapper.h"
#include "wrapped_component/response_listeners/device_rsc_resp_listener.h"

// (TODO) The Version class in base ("version.h") is renamed as BaseVersion to avoid redefinition error of 'Version' class. (The problem is, Shp framework also defines a version class (auto generated code) and both classes are not kept in any namespace.

namespace webservice
{

    static const char *const kBuildPathId = "0";
    static const char *const kId = "id";
    static const char *const kUuid = "uuid";
    static const char *const kName = "name";
    static const char *const kType = "type";
    static const char *const kResources = "resources";
    static const char *const kConnected = "connected";
    static const char *const kDescription = "description";
    static const char *const kDeviceId = "device_id";
    static const char *const kDevice = "Device";

// (TODO) Check is two way mapping is necessary, if not use normal map itself
    static boost::bimap<std::string, OC::Cm::DeviceType> device_type_map;
    static bool device_type_map_initialized = false;

    static void InitializeDeviceTypeMap()
    {
        if (device_type_map_initialized)
        {
            DLOG(INFO) << "Map already initialized";
            return;
        }
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Unknown",
                               OC::Cm::DeviceType_Unknown));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Gateway",
                               OC::Cm::DeviceType_Gateway));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Smartphone",
                               OC::Cm::DeviceType_Smartphone));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Tablet",
                               OC::Cm::DeviceType_Tablet));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("TV",
                               OC::Cm::DeviceType_TV));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Refrigerator",
                               OC::Cm::DeviceType_Refrigerator));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Washer",
                               OC::Cm::DeviceType_Washer));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Dryer",
                               OC::Cm::DeviceType_Dryer));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Air_Conditioner",
                               OC::Cm::DeviceType_Air_Conditioner));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Dishwasher",
                               OC::Cm::DeviceType_Dishwasher));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Oven",
                               OC::Cm::DeviceType_Oven));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Microwave_Oven",
                               OC::Cm::DeviceType_Microwave_Oven));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Cooktop",
                               OC::Cm::DeviceType_Cooktop));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Robot_Cleaner",
                               OC::Cm::DeviceType_Robot_Cleaner));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Light",
                               OC::Cm::DeviceType_Light));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("IP_Camera",
                               OC::Cm::DeviceType_IP_Camera));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Door_Lock",
                               OC::Cm::DeviceType_Door_Lock));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Door_Phone",
                               OC::Cm::DeviceType_Door_Phone));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Thermostat",
                               OC::Cm::DeviceType_Thermostat));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Water_Heater",
                               OC::Cm::DeviceType_Water_Heater));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Smart_Plug",
                               OC::Cm::DeviceType_Smart_Plug));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Generic_Sensor",
                               OC::Cm::DeviceType_Generic_Sensor));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Motion_Detector",
                               OC::Cm::DeviceType_Motion_Detector));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Heater",
                               OC::Cm::DeviceType_Heater));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Coffee_Pot",
                               OC::Cm::DeviceType_Coffee_Pot));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Iron",
                               OC::Cm::DeviceType_Iron));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Vacuum_Cleaner",
                               OC::Cm::DeviceType_Vacuum_Cleaner));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Zigbee_Bridge",
                               OC::Cm::DeviceType_Zigbee_Bridge));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("Zwave_Bridge",
                               OC::Cm::DeviceType_Zwave_Bridge));
        device_type_map.insert(boost::bimap<std::string, OC::Cm::DeviceType>::value_type("All",
                               OC::Cm::DeviceType_All));
        device_type_map_initialized = true;
    }

// FillGetDeviceResponse & ParseDeviceRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseDeviceRequestBody(base::DictionaryValue *input_dict_value,
            Device &device)
    {
        LOG(ERROR) << "ParseDeviceRequestBody Enter";
        if (!input_dict_value)
        {
            LOG(ERROR) << "i/p or o/p param is NULL";
            return RT_400_BAD_REQUEST;
        }
        // name (string / optional)
        std::string name;
        if (input_dict_value->GetString(kName, &name))
        {
            device.mpName = new String30();
            if (device.mpName)
            {
                device.mpName->value = name;
            }
            else
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
        }
        // description (string / optional)
        std::string desc;
        if (input_dict_value->GetString(kDescription, &desc))
        {
            device.mpDescription = new String50();
            if (device.mpDescription)
            {
                device.mpDescription->value = desc;
            }
            else
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetDeviceResponse(const std::string &device_id, OC::Cm::Device *device,
            void *p_out)
    {
        LOG(ERROR) << "FillGetDeviceResponse";
        DictionaryValue *device_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!device || !device_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        DLOG(INFO) << "id: " << device_id;
        DLOG(INFO) << "uuid: " << device->getUUID();
        DLOG(INFO) << "name: " << device->getDeviceName();
        DLOG(INFO) << "online status: " << device->getDeviceOnlineStatus();
        DLOG(INFO) << "description: " << device->getDescription();
        device_dict->SetWithoutPathExpansion(kId, base::Value::CreateStringValue(device_id));
        // uuid (string / mandatory)
        device_dict->SetWithoutPathExpansion(kUuid, base::Value::CreateStringValue(device->getUUID()));
        // name (string / optional)
        //  (TODO) version check should be added for 'name'
        std::string device_name = device->getDeviceName();
        if (device_name != "")
        {
            device_dict->SetWithoutPathExpansion(kName, base::Value::CreateStringValue(device_name));
        }
        // initializing the devicemap here
        // (TODO) should be called from resource_request handler initialization
        InitializeDeviceTypeMap();
        // type (string / mandatory)
        OC::Cm::DeviceType device_type = device->getDeviceType();
        auto it = device_type_map.right.find(device_type);
        if (it != device_type_map.right.end())
        {
            device_dict->SetWithoutPathExpansion(kType, base::Value::CreateStringValue(it->second));
        }
        // resources (ArrayOfResourceType / mandatory)
        std::list<std::string> resource_list = device->getSupportedResource();
        if (resource_list.size())
        {
            DLOG(INFO) << "(resource_list) size is " << resource_list.size();
            ListValue *supported_rsc_list = new ListValue;
            if (supported_rsc_list)
            {
                std::list<std::string>::const_iterator it_list;
                for (it_list = resource_list.begin(); it_list != resource_list.end(); it_list++)
                {
                    supported_rsc_list->Append(base::Value::CreateStringValue(*it_list));
                }
                device_dict->Set(kResources, supported_rsc_list);
            }
        }
        // connected (boolean / optional)
        //  (TODO) version check should be added for 'connected'
        device_dict->SetWithoutPathExpansion(kConnected,
                                             base::Value::CreateBooleanValue(device->getDeviceOnlineStatus()));
        // description (string / optional)
        std::string desc = device->getDescription();
        if (desc != "")
        {
            device_dict->SetWithoutPathExpansion(kDescription, base::Value::CreateStringValue(desc));
        }
        // 'InformationLink'
        std::list<Device *> devices_list = device->m_devices.mpDevice;
        // get the 'InformationLink' from the device at position '0'
        InformationLink *info_link = devices_list.front()->mpInformationLink;
        DLOG(INFO) << "info link val: " << *info_link->mpHref;
        device_dict->SetWithoutPathExpansion("InformationLink",
                                             base::Value::CreateStringValue(*info_link->mpHref));
        return RT_200_OK;
    }
    /**
      * Validation Check for ID
      * @param[in] string
      * @param[in] HTTPMessage*
      * @return static HTTPResponseType
      */
    HTTPResponseType CommonWrapperFunctions::ValidateDeviceID(const std::string &id, HTTPMessage *http)
    {
        LOG(ERROR) << "ValidateDeviceID  " << id;
        std::string device_id = id;
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
        OC::Cm::Device *cm_device = cm->getDeviceFinder()->getDeviceByUUID(device_id);
        if (cm_device)
        {
            DLOG(INFO) << "Device found";
            return RT_200_OK;
        }
        LOG(ERROR) << "Device Not found";
        return RT_404_DEVICE_ID;
    }

    /**
      * Get Object ID list
      * @param[out] std::vector<std::string>* : id list
      * @param[in] HTTPMessage*
      * @return int
      */
    int CommonWrapperFunctions::GetDeviceIDList(std::vector<std::string> *id_list, HTTPMessage *http)
    {
        LOG(ERROR) << "GetDeviceIDList Enter";
        if (NULL == id_list) return -1;
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
        std::list<OC::Cm::Device *> recvd_list = cm->getDeviceFinder()->getDevices();
        DLOG(INFO) << "Recvd device list size : " << recvd_list.size();
        std::list<OC::Cm::Device *>::iterator it = recvd_list.begin();
        for (; it != recvd_list.end(); it++)
        {
            DLOG(INFO) << "Device UUID:: " << (*it)->getUUID();
            id_list->push_back((*it)->getUUID());
        }
        return id_list->size();
    }

    bool CommonWrapperFunctions::GetDeviceDescription(void *p_in, void *p_out,
            const std::string &service_name, const std::string &output_name)
    {
        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)
        std::string *out_str = reinterpret_cast<std::string *>(p_out);
        std::string device_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return false;
        }

        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            OC::Cm::ControlManager *cm = cm_wrapper->GetControlManagerInstance();
            if (cm)
            {
                OC::Cm::Device *cm_device = cm->getDeviceFinder()->getDeviceByUUID(device_id);
                if (!cm_device)
                {
                    LOG(ERROR) << "Device Not found";
                    return false;
                }
                else
                {
                    DLOG(INFO) << "Device found";
                    *out_str = cm_device->getDescription();
                }
            }
        }

        DLOG(INFO) << "GetDeviceDescription, device id : " << device_id << ", device description : " <<
                   *out_str;
        if ("" != *out_str) return true;
        else return false;
    }
    HTTPResponseType CommonWrapperFunctions::GetIdentifiedObjectDevice(void *p_in, void *p_out)
    {
        LOG(ERROR) << "GetIdentifiedObjectDevice Enter";
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
        OC::Cm::Device *device = cm->getDeviceFinder()->getDeviceByUUID(device_id);
        if (!device)
        {
            LOG(ERROR) << "getDeviceByUUID failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        DLOG(INFO) << "Valid data received from CM";
        // populate the response string
        FillGetDeviceResponse(device_id, device, p_out);
        return RT_200_OK;
    }

    HTTPResponseType CommonWrapperFunctions::PutIdentifiedObjectDevice(void *p_in, void *p_out)
    {
        LOG(ERROR) << "PutIdentifiedObjectDevice Enter";
        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)

        std::string device_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        Device reqData;
        HTTPResponseType ret = ParseDeviceRequestBody(in_dict, reqData);
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
        DeviceResource *device_rsc = (DeviceResource *)cm_device->getResource(kDevice);
        if (!device_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        device_rsc->buildPath(kBuildPathId);
        DeviceResourceResponseListener *respListener = DeviceResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        device_rsc->addResponseListener(*respListener);
        ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device putDevice";
        if (device_rsc->putDevice(requestId, reqData))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
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
            LOG(ERROR) << "putDevice failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    bool CommonWrapperFunctions::SetDeviceDescription(void *p_in, void *p_out,
            const std::string &service_name, const std::string &input_name)
    {
        LOG(ERROR) << "SetDeviceDescription Enter";
        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)
        std::string *set_value = reinterpret_cast<std::string *>(p_out);

        std::string device_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return false;
        }

        // get the device type from control manager
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            OC::Cm::ControlManager *cm = cm_wrapper->GetControlManagerInstance();
            if (cm)
            {
                OC::Cm::Device *cm_device = cm->getDeviceFinder()->getDeviceByUUID(device_id);
                if (!cm_device)
                {
                    LOG(ERROR) << "Device Not found";
                    return false;
                }
                else
                {
                    DLOG(INFO) << "Device found";
                    cm_device->setDescription((*set_value).c_str());
                }
            }
        }

        DLOG(ERROR) << "Set Value for SetDeviceDescription(" << device_id << ") : " << *set_value;
        return true;
    }
    /**
      * Create ID
      * @param[in] void*
      * @param[out] string*
      * @param[in] HTTPMessage*
      * @return static HTTPResponseType
      */
    HTTPResponseType CommonWrapperFunctions::CreateDeviceID(void *p_in, std::string *id,
            HTTPMessage *http)
    {
        return RT_400_NOT_READY;
    }

    /**
      * Delete ID
      * @param[in] void*
      * @param[in] const string&
      * @param[in] HTTPMessage*
      * @return static HTTPResponseType
      */
    HTTPResponseType CommonWrapperFunctions::DeleteDeviceID(void *p_in, const std::string &input_id,
            HTTPMessage *http)
    {
        LOG(ERROR) << "DeleteDeviceID Enter: " << input_id;
        std::string device_id = input_id;
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
        DeviceResource *device_rsc = (DeviceResource *)cm_device->getResource(kDevice);
        if (!device_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        device_rsc->buildPath(kBuildPathId);
        DeviceResourceResponseListener *respListener = DeviceResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        device_rsc->addResponseListener(*respListener);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device deleteDevice";
        if (device_rsc->deleteDevice(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
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
            LOG(ERROR) << "deleteDevice failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

}

