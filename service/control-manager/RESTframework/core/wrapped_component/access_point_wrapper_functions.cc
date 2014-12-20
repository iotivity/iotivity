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
#include "wrapped_component/response_listeners/access_point_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/access_points_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kBuildPathId = "0";
    static const char *const kId = "id";
    static const char *const kChannel = "channel";
    static const char *const kSsid = "ssid";
    static const char *const kMacAddress = "macAddress";
    static const char *const kMaxRate = "maxRate";
    static const char *const kRssi = "rssi";
    static const char *const kSecurityType = "securityType";
    static const char *const kEncryptionType = "encryptionType";
    static const char *const kDescription = "description";
    static const char *const kDeviceId = "device_id";
    static const char *const kNetworkId = "network_id";
    static const char *const kAccessPointId = "accesspoint_id";
    static const char *const kDevices = "devices";
    static const char *const kNetworks = "networks";

// FillGetAccessPointResponse used only in this file, so keeping it as static

    static HTTPResponseType FillAccessPointResponse(const std::string &access_point_id, void *input,
            void *p_out)
    {
        AccessPoint *access_point = reinterpret_cast<AccessPoint *>(input);
        DictionaryValue *ap_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!access_point || !ap_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        ap_values_dict->SetWithoutPathExpansion(kId, base::Value::CreateStringValue(access_point_id));
        // channel (uint8/optional)
        if (access_point->mpChannel)
        {
            DLOG(INFO) << "(channel) is " << access_point->mpChannel;
            ap_values_dict->SetWithoutPathExpansion(kChannel,
                                                    base::Value::CreateIntegerValue(*(access_point->mpChannel)));
        }
        // ssid (string50/optional)
        if (access_point->mpSsid)
        {
            DLOG(INFO) << "(ssid) is " << access_point->mpSsid->value;
            ap_values_dict->SetWithoutPathExpansion(kSsid,
                                                    base::Value::CreateStringValue(access_point->mpSsid->value));
        }
        // MacAddress (string/optional)
        if (access_point->mpMacAddress)
        {
            DLOG(INFO) << "(MacAddress) is " << access_point->mpMacAddress->value;
            ap_values_dict->SetWithoutPathExpansion(kMacAddress,
                                                    base::Value::CreateStringValue(access_point->mpMacAddress->value));
        }
        // MaxRate (uint8/optional)
        if (access_point->mpMaxRate)
        {
            DLOG(INFO) << "(MaxRate) is " << access_point->mpMaxRate;
            ap_values_dict->SetWithoutPathExpansion(kMaxRate,
                                                    base::Value::CreateIntegerValue(*(access_point->mpMaxRate)));
        }
        // Rssi (int8/optional)
        if (access_point->mpRssi)
        {
            DLOG(INFO) << "(rssi) is " << access_point->mpRssi;
            ap_values_dict->SetWithoutPathExpansion(kRssi,
                                                    base::Value::CreateIntegerValue((*access_point->mpRssi)));
        }
        // SecurityType (string/optional)
        if (access_point->mpSecurityType)
        {
            DLOG(INFO) << "(SecurityType) is " << access_point->mpSecurityType->value;
            ap_values_dict->SetWithoutPathExpansion(kSecurityType,
                                                    base::Value::CreateStringValue(access_point->mpSecurityType->value));
        }
        // encryptionType (string/optional)
        if (access_point->mpEncryptionType)
        {
            DLOG(INFO) << "(encryptionType) is " << access_point->mpEncryptionType->value;
            ap_values_dict->SetWithoutPathExpansion(kEncryptionType,
                                                    base::Value::CreateStringValue(access_point->mpEncryptionType->value));
        }
        // description (string/optional)
        if (access_point->mpDescription)
        {
            DLOG(INFO) << "(description) is " << access_point->mpDescription;
            ap_values_dict->SetWithoutPathExpansion(kDescription,
                                                    base::Value::CreateStringValue(access_point->mpDescription->value));
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetAccessPointsResponse(void *input, void *p_out)
    {
        AccessPoints *access_points = reinterpret_cast<AccessPoints *>(input);
        std::vector<std::string> *id_list = reinterpret_cast<std::vector<std::string>*>(p_out);
        if (!access_points || !id_list)
        {
            return RT_400_BAD_REQUEST;
        }
        // temperature list
        if (access_points->mpAccessPoint.size())
        {
            std::list<AccessPoint *> ap_list = access_points->mpAccessPoint;
            DLOG(INFO) << "(ap_list) size is " << ap_list.size();
            std::list<AccessPoint *>::iterator it_list;
            for (it_list = ap_list.begin(); it_list != ap_list.end(); it_list++)
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
    HTTPResponseType CommonWrapperFunctions::ValidateAccessPointID(const std::string &id,
            HTTPMessage *http)
    {
        std::vector<std::string> id_list;
        if (GetAccessPointIDList(&id_list, http))
        {
            std::vector<std::string>::iterator it = id_list.begin();
            for (; it != id_list.end(); it++)
            {
                if (*it == id)
                {
                    DLOG(INFO) << "AccessPointID found: " << id;
                    return RT_200_OK;
                }
            }
            // parsed the whole list and id not found, so return 404 from here
            return RT_404_ACCESSPOINT_ID;
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
    HTTPResponseType CommonWrapperFunctions::CreateAccessPointID(void *p_in, std::string *id,
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
    HTTPResponseType CommonWrapperFunctions::DeleteAccessPointID(void *p_in,
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
    int CommonWrapperFunctions::GetAccessPointIDList(std::vector<std::string> *id_list,
            HTTPMessage *http)
    {
        DLOG(INFO) << "GetAccessPointIDList Enter";
        if (!id_list || !http)
        {
            LOG(ERROR) << "Invalid input parameters given";
            return -1;
        }
        std::string device_id = "";
        std::string network_id = "";
        const std::vector<std::string> &uri_field = http->request.line->GetUriFields();
        std::vector<std::string>::const_iterator it;
        for (it = uri_field.begin(); it != uri_field.end(); it++)
        {
            DLOG(INFO) << "Uri field:: " << *it;
            if (kDevices == *it)
            {
                it++;
                device_id = *it;
            }
            if (kNetworks == *it)
            {
                if (it != uri_field.end())
                {
                    it++;
                    if (it != uri_field.end())
                    {
                        network_id = *it;
                    }
                }
                break;
            }
        }
        DLOG(INFO) << "device_id is :  " << device_id << "  network_id is :  " << network_id;
        if (device_id.empty() || network_id.empty())
        {
            return 0;
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
        AccessPointsResource *access_points_rsc = (AccessPointsResource *)
                cm_device->getResource("AccessPoints");
        if (!access_points_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return -1;
        }
        access_points_rsc->buildPath(kBuildPathId, network_id);
        AccessPointsResourceResponseListener *respListener =
            AccessPointsResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return -1;
        }
        access_points_rsc->addResponseListener(*respListener);
        int requestId;
        LOG(ERROR) << "AccessPoint getAccessPoints";
        if (access_points_rsc->getAccessPoints(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeAccessPoints;
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
                        FillGetAccessPointsResponse(ptr, id_list);
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
            LOG(ERROR) << "getAccessPoints failed";
            return -1;
        }
        return id_list->size();
    }

    HTTPResponseType CommonWrapperFunctions::GetIdentifiedObjectAccessPoint(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetIdentifiedObjectAccessPoint Enter";
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)
        std::string device_id = "";
        std::string network_id = "";
        std::string access_point_id = "";
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
        if (!in_dict->GetString(kAccessPointId, &access_point_id))
        {
            DLOG(ERROR) << "Input parameter is not get access_point_id";
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
            return RT_404_DEVICE_ID;
        }
        AccessPointResource *ap_rsc = (AccessPointResource *)cm_device->getResource("AccessPoint");
        if (!ap_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        ap_rsc->buildPath(kBuildPathId, network_id, access_point_id);
        AccessPointResourceResponseListener *respListener =
            AccessPointResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        ap_rsc->addResponseListener(*respListener);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device getAccessPoint";
        if (ap_rsc->getAccessPoint(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeAccessPoint;
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
                        FillAccessPointResponse(access_point_id, ptr, p_out);
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
            LOG(ERROR) << "getAccessPoint Failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

}

