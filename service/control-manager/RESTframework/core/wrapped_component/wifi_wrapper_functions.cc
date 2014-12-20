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
#include "wrapped_component/response_listeners/wifi_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kBuildPathId = "0";
    static const char *const kDeviceId = "device_id";
    static const char *const kNetworkId = "network_id";
    static const char *const kOperationType = "operationType";
    static const char *const kChannel = "channel";
    static const char *const kSsid = "ssid";
    static const char *const kRssi = "rssi";
    static const char *const kSecurityType = "securityType";
    static const char *const kEncryptionType = "encryptionType";
    static const char *const kSecurityKey = "securityKey";
    static const char *const kWifiDirectStatus = "wifiDirectStatus";
    static const char *const kWpsEnable = "wpsEnable";
    static const char *const kWpsStatus = "wpsStatus";

// FillGetWiFiResponse & ParseWiFiRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseWiFiRequestBody(base::DictionaryValue *input_dict_value, WiFi &wifi)
    {
        if (!input_dict_value)
        {
            return RT_400_BAD_REQUEST;
        }
        // operationType (string/optional)
        std::string operation_tyype;
        if (input_dict_value->GetString(kOperationType, &operation_tyype))
        {
            wifi.mpOperationType = new WifiOperationType();
            if (!wifi.mpOperationType)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            wifi.mpOperationType->value = operation_tyype;
        }
        // channel (integer/optional)
        int channel;
        if (input_dict_value->GetInteger(kChannel, &channel))
        {
            wifi.mpChannel = new uint8_t();
            if (!wifi.mpChannel)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(wifi.mpChannel) = channel;
        }
        // ssid (string/optional)
        std::string ssid;
        if (input_dict_value->GetString(kSsid, &ssid))
        {
            wifi.mpSsid = new String50();
            if (!wifi.mpSsid)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            wifi.mpSsid->value = ssid;
        }
        // securityType (string/optional)
        std::string security_type;
        if (input_dict_value->GetString(kSecurityType, &security_type))
        {
            wifi.mpSecurityType = new String20();
            if (!wifi.mpSecurityType)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            wifi.mpSecurityType->value = security_type;
        }
        // encryptionType (string/optional)
        std::string encryption_type;
        if (input_dict_value->GetString(kEncryptionType, &encryption_type))
        {
            wifi.mpEncryptionType = new String20();
            if (!wifi.mpEncryptionType)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            wifi.mpEncryptionType->value = encryption_type;
        }
        // securityKey (string/optional)
        std::string security_key;
        if (input_dict_value->GetString(kSecurityKey, &security_key))
        {
            wifi.mpSecurityKey = new String100();
            if (!wifi.mpSecurityKey)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            wifi.mpSecurityKey->value = security_key;
        }
        // wpsEnable (boolean/optional)
        bool wps_enable;
        if (input_dict_value->GetBoolean(kWpsEnable, &wps_enable))
        {
            wifi.mpWpsEnabled = new bool();
            if (!wifi.mpWpsEnabled)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(wifi.mpWpsEnabled) = wps_enable;
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetWiFiResponse(void *input, void *p_out)
    {
        WiFi *wifi = reinterpret_cast<WiFi *>(input);
        DictionaryValue *wifi_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!wifi || !wifi_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        // operationType (string/optional)
        if (wifi->mpOperationType)
        {
            DLOG(INFO) << "(operationType) is " << wifi->mpOperationType->value;
            wifi_values_dict->SetWithoutPathExpansion(kOperationType,
                    base::Value::CreateStringValue(wifi->mpOperationType->value));
        }
        // channel (integer/optional)
        if (wifi->mpChannel)
        {
            DLOG(INFO) << "(channel) is " << wifi->mpChannel;
            wifi_values_dict->SetWithoutPathExpansion(kChannel,
                    base::Value::CreateIntegerValue(*wifi->mpChannel));
        }
        // ssid (string/optional)
        if (wifi->mpSsid)
        {
            DLOG(INFO) << "(ssid) is " << wifi->mpSsid->value;
            wifi_values_dict->SetWithoutPathExpansion(kSsid,
                    base::Value::CreateStringValue(wifi->mpSsid->value));
        }
        // rssi (integer/optional)
        if (wifi->mpRssi)
        {
            DLOG(INFO) << "(rssi) is " << wifi->mpRssi;
            wifi_values_dict->SetWithoutPathExpansion(kRssi, base::Value::CreateIntegerValue(*wifi->mpRssi));
        }
        // securityType (string/optional)
        if (wifi->mpSecurityType)
        {
            DLOG(INFO) << "(securityType) is " << wifi->mpSecurityType->value;
            wifi_values_dict->SetWithoutPathExpansion(kSecurityType,
                    base::Value::CreateStringValue(wifi->mpSecurityType->value));
        }
        // encryptionType (string/optional)
        if (wifi->mpEncryptionType)
        {
            DLOG(INFO) << "(encryptionType) is " << wifi->mpEncryptionType->value;
            wifi_values_dict->SetWithoutPathExpansion(kEncryptionType,
                    base::Value::CreateStringValue(wifi->mpEncryptionType->value));
        }
        // securityKey (string/optional)
        if (wifi->mpSecurityKey)
        {
            DLOG(INFO) << "(securityKey) is " << wifi->mpSecurityKey->value;
            wifi_values_dict->SetWithoutPathExpansion(kSecurityKey,
                    base::Value::CreateStringValue(wifi->mpSecurityKey->value));
        }
        // wifiDirectStatus (string/optional)
        if (wifi->mpWifiDirectStatus)
        {
            DLOG(INFO) << "(wifiDirectStatus) is " << wifi->mpWifiDirectStatus->value;
            wifi_values_dict->SetWithoutPathExpansion(kWifiDirectStatus,
                    base::Value::CreateStringValue(wifi->mpWifiDirectStatus->value));
        }
        // wpsEnable (boolean/optional)
        if (wifi->mpWpsStatus)
        {
            DLOG(INFO) << "(wpsEnabled) is " << wifi->mpWpsStatus;
            wifi_values_dict->SetWithoutPathExpansion(kWpsEnable,
                    base::Value::CreateBooleanValue(wifi->mpWpsStatus));
        }
        return RT_200_OK;
    }

    HTTPResponseType CommonWrapperFunctions::GetResourceWiFi(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetResourceWiFi Enter";
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)
        std::string device_id = "";
        std::string network_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        if (!in_dict->GetString(kNetworkId, &network_id))
        {
            DLOG(ERROR) << "Input parameter is not get network_id";
            return RT_400_BAD_REQUEST;
        }
        DLOG(INFO) << "device id : " << device_id << "  nw_id: " << network_id;
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
        WiFiResource *wifi_rsc = (WiFiResource *)cm_device->getResource("WiFi");
        if (!wifi_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        wifi_rsc->buildPath(kBuildPathId, network_id);
        WiFiResourceResponseListener *respListener = WiFiResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        wifi_rsc->addResponseListener(*respListener);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device getWiFi";
        if (wifi_rsc->getWiFi(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeWiFi;
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
                        FillGetWiFiResponse(ptr, p_out);
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
            LOG(ERROR) << "getWiFi failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    HTTPResponseType CommonWrapperFunctions::PutResourceWiFi(void *p_in, void *p_out)
    {
        DLOG(INFO) << "PutResourceWiFi Enter";

        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)

        std::string device_id = "";
        std::string network_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        if (!in_dict->GetString(kNetworkId, &network_id))
        {
            DLOG(ERROR) << "Input parameter is not get network_id";
            return RT_400_BAD_REQUEST;
        }
        WiFi reqData;
        HTTPResponseType ret = ParseWiFiRequestBody(in_dict, reqData);
        if (ret != RT_200_OK)
        {
            LOG(ERROR) << "Error Parsing request body";
            return ret;
        }

        ret = RT_500_INTERNAL_SERVER_ERROR;
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
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        WiFiResource *wifi_rsc = (WiFiResource *)cm_device->getResource("WiFi");
        if (!wifi_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        wifi_rsc->buildPath(kBuildPathId, network_id);
        WiFiResourceResponseListener *respListener = WiFiResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        wifi_rsc->addResponseListener(*respListener);
        LOG(ERROR) << "Device putWiFi";
        if (wifi_rsc->putWiFi(requestId, reqData))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeWiFi;
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
            LOG(ERROR) << "putWiFi failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }
}

