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
#include "wrapped_component/response_listeners/configuration_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kInstalledLocation = "installedLocation";
    static const char *const kRegion = "region";
    static const char *const kCurrency = "currency";
    static const char *const kRemoteControlEnabled = "remoteControlEnabled";
    static const char *const kPairingStart = "pairingStart";
    static const char *const kUnpairingStart = "unpairingStart";
    static const char *const kPairingTimeout = "pairingTimeout";
    static const char *const kSubDeviceIDs = "subDeviceIDs";
    static const char *const kIsSubDevice = "isSubDevice";
    static const char *const kConnectedDeviceType = "connectedDeviceType";
    static const char *const kBuildPathId = "0";
    static const char *const kDeviceId = "device_id";

// FillGetConfigurationResponse & ParseConfigurationRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseConfigurationRequestBody(DictionaryValue *input_dict_value,
            Configuration &configuration)
    {
        DLOG(INFO) << "ParseConfigurationRequestBody";
        if (!input_dict_value)
        {
            LOG(ERROR) << "i/p or o/p param is NULL";
            return RT_400_BAD_REQUEST;
        }
        // installedLocation (string/optional)
        std::string installed_location;
        if (input_dict_value->GetString(kInstalledLocation, &installed_location))
        {
            configuration.mpInstalledLocation = new InternalLocationType();
            if (!configuration.mpInstalledLocation)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            configuration.mpInstalledLocation->value = installed_location;
        }
        // region (string/optional)
        std::string region;
        if (input_dict_value->GetString(kRegion, &region))
        {
            configuration.mpRegion = new String20();
            if (!configuration.mpRegion)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            configuration.mpRegion->value = region;
        }
        // currency (string/optional)
        std::string currency;
        if (input_dict_value->GetString(kCurrency, &currency))
        {
            configuration.mpCurrency = new CurrencyType();
            if (!configuration.mpCurrency)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            configuration.mpCurrency->value = currency;
        }
        // pairingStart (string/optional)
        std::string pairing_start;
        if (input_dict_value->GetString(kPairingStart, &pairing_start))
        {
            configuration.mpPairingStart = new OnType();
            if (!configuration.mpPairingStart)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            configuration.mpPairingStart->value = pairing_start;
        }
        // unpairingStart (string/optional)
        std::string unpairing_start;
        if (input_dict_value->GetString(kUnpairingStart, &unpairing_start))
        {
            configuration.mpUnpairingStart = new OnType();
            if (!configuration.mpUnpairingStart)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            configuration.mpUnpairingStart->value = unpairing_start;
        }
        // subDeviceIDs (arrayofstring/optional)
        ListValue *sub_devices_list = NULL;
        if (input_dict_value->GetList(kSubDeviceIDs, &sub_devices_list))
        {
            configuration.mpSubDeviceIDs = new ArrayOfString();
            for (int i = 0; i < static_cast<int>(sub_devices_list->GetSize()); i++)
            {
                std::string id;
                sub_devices_list->GetString((size_t)i, &id);
                if (!id.empty())
                {
                    configuration.mpSubDeviceIDs->members.push_back(id);
                }
            }
        }
        // isSubDevice (boolean/optional)
        bool is_sub_device;
        if (input_dict_value->GetBoolean(kIsSubDevice, &is_sub_device))
        {
            configuration.mpIsSubDevice = new bool();
            if (!configuration.mpIsSubDevice)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(configuration.mpIsSubDevice) = is_sub_device;
        }
        // connectedDeviceType (Devicetype/optional)
        std::string connected_device_type;
        if (input_dict_value->GetString(kConnectedDeviceType, &connected_device_type))
        {
            configuration.mpConnectedDeviceType = new DeviceType();
            if (!configuration.mpConnectedDeviceType)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            configuration.mpConnectedDeviceType->value = connected_device_type;
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetConfigurationResponse(void *p_in, void *p_out)
    {
        DLOG(INFO) << "FillGetConfigurationResponse";
        CHECK_INPUT_ARGS
        Configuration *configuration = reinterpret_cast<Configuration *>(p_in);
        DictionaryValue *configuration_values_dict = reinterpret_cast<DictionaryValue *>(p_out);

        // installedLocation (string/optional)
        if (configuration->mpInstalledLocation)
        {
            DLOG(INFO) << "(installedLocation) is " << configuration->mpInstalledLocation->value;
            configuration_values_dict->SetWithoutPathExpansion(kInstalledLocation,
                    base::Value::CreateStringValue(configuration->mpInstalledLocation->value));
        }
        // region (string/optional)
        if (configuration->mpRegion)
        {
            DLOG(INFO) << "(region) is " << configuration->mpRegion->value;
            configuration_values_dict->SetWithoutPathExpansion(kRegion,
                    base::Value::CreateStringValue(configuration->mpRegion->value));
        }
        // currency (string/optional)
        if (configuration->mpCurrency)
        {
            DLOG(INFO) << "(currency) is " << configuration->mpCurrency->value;
            configuration_values_dict->SetWithoutPathExpansion(kCurrency,
                    base::Value::CreateStringValue(configuration->mpCurrency->value));
        }
        // remoteControlEnabled (boolean/optional)
        if (configuration->mpRemoteControlEnabled)
        {
            DLOG(INFO) << "(remoteControlEnabled) is " << configuration->mpRemoteControlEnabled;
            configuration_values_dict->SetWithoutPathExpansion(kRemoteControlEnabled,
                    base::Value::CreateBooleanValue(*configuration->mpRemoteControlEnabled));
        }
        // pairingStart (boolean)
        if (configuration->mpPairingStart)
        {
            DLOG(INFO) << "(pairingStart) is " << configuration->mpPairingStart->value;
            configuration_values_dict->SetWithoutPathExpansion(kPairingStart,
                    base::Value::CreateStringValue(configuration->mpPairingStart->value));
        }
        // unpairingStart (boolean)
        if (configuration->mpUnpairingStart)
        {
            DLOG(INFO) << "(unpairingStart) is " << configuration->mpUnpairingStart->value;
            configuration_values_dict->SetWithoutPathExpansion(kUnpairingStart,
                    base::Value::CreateStringValue(configuration->mpUnpairingStart->value));
        }
        // pairingTimeout (boolean)
        if (configuration->mpPairingTimeout)
        {
            DLOG(INFO) << "(pairingTimeout) is " << *configuration->mpPairingTimeout;
            configuration_values_dict->SetWithoutPathExpansion(kPairingTimeout,
                    base::Value::CreateIntegerValue(*configuration->mpPairingTimeout));
        }
        // subDeviceIDs (boolean)
        if (configuration->mpSubDeviceIDs)
        {
            DLOG(INFO) << "(subDeviceIDs) is " << configuration->mpSubDeviceIDs;
            ListValue *sub_device_ids_list = new ListValue;
            if (sub_device_ids_list)
            {
                std::list<std::string>::const_iterator it_list;
                for (it_list = configuration->mpSubDeviceIDs->members.begin();
                     it_list != configuration->mpSubDeviceIDs->members.end(); it_list++)
                {
                    DLOG(INFO) << "Subdevice id: " << (*it_list);
                    sub_device_ids_list->Append(base::Value::CreateStringValue(*it_list));
                }
                configuration_values_dict->Set(kSubDeviceIDs, sub_device_ids_list);
            }
        }
        // isSubDevice (boolean)
        if (configuration->mpIsSubDevice)
        {
            DLOG(INFO) << "(isSubDevice) is " << configuration->mpIsSubDevice;
            configuration_values_dict->SetWithoutPathExpansion(kIsSubDevice,
                    base::Value::CreateBooleanValue(*configuration->mpIsSubDevice));
        }
        // connectedDeviceType (boolean)
        if (configuration->mpConnectedDeviceType)
        {
            DLOG(INFO) << "(connectedDeviceType) is " << configuration->mpConnectedDeviceType->value;
            configuration_values_dict->SetWithoutPathExpansion(kConnectedDeviceType,
                    base::Value::CreateStringValue(configuration->mpConnectedDeviceType->value));
        }
        return RT_200_OK;
    }

    HTTPResponseType CommonWrapperFunctions::GetResourceConfiguration(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetResourceConfiguration Enter";
        CHECK_INPUT_ARGS
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
        ConfigurationResource *config_rsc = (ConfigurationResource *)
                                            cm_device->getResource("Configuration");
        if (!config_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        ConfigurationResourceResponseListener *respListener =
            ConfigurationResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        config_rsc->buildPath(kBuildPathId);
        config_rsc->addResponseListener(*respListener);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device getConfiguration";
        if (config_rsc->getConfiguration(requestId))
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
                    if (ptr && resp_status == RT_200_OK)
                    {
                        DLOG(INFO) << "Valid data received from callback";
                        // populate the response string
                        FillGetConfigurationResponse(ptr, p_out);
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
            LOG(ERROR) << "getConfiguration failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    HTTPResponseType CommonWrapperFunctions::PutResourceConfiguration(void *p_in, void *p_out)
    {
        DLOG(INFO) << "PutResourceConfiguration Enter";

        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)

        std::string device_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        Configuration reqData;
        HTTPResponseType ret = ParseConfigurationRequestBody(in_dict, reqData);
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
        ConfigurationResource *config_rsc = (ConfigurationResource *)
                                            cm_device->getResource("Configuration");
        if (!config_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        ConfigurationResourceResponseListener *respListener =
            ConfigurationResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        config_rsc->buildPath(kBuildPathId);
        ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device putConfiguration";
        if (config_rsc->putConfiguration(requestId, reqData))
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
            LOG(ERROR) << "putConfiguration failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }
}

