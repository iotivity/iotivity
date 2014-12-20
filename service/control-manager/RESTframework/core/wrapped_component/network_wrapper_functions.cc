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
#include "wrapped_component/response_listeners/network_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/networks_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kId = "id";
    static const char *const kBuildPathId = "0";
    static const char *const kEnabled = "enabled";
    static const char *const kConnected = "connected";
    static const char *const kPriority = "priority";
    static const char *const kDhcpEnabled = "dhcpEnabled";
    static const char *const kIpAddress = "ipAddress";
    static const char *const kSubnetMask = "subnetMask";
    static const char *const kGateway = "gateway";
    static const char *const kDnsPrimary = "dnsPrimary";
    static const char *const kDnsSecondary = "dnsSecondary";
    static const char *const kMacAddress = "macAddress";
    static const char *const kDeviceId = "device_id";
    static const char *const kNetworkId = "network_id";
    static const char *const kDescription = "description";
    static const char *const kDevices = "devices";

// FillGetNetworkResponse & ParseNetworkRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseNetworkRequestBody(base::DictionaryValue *input_dict_value,
            Network &network)
    {
        DLOG(INFO) << "ParseNetworkRequestBody Enter";
        if (!input_dict_value)
        {
            return RT_400_BAD_REQUEST;
        }
        // enabled (Boolean/mandatory)
        bool enabled;
        if (input_dict_value->GetBoolean(kEnabled, &enabled))
        {
            network.mpEnabled = new bool();
            if (!network.mpEnabled)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(network.mpEnabled) = enabled;
        }
        else
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        // connected (Boolean/mandatory)
        bool connected;
        if (input_dict_value->GetBoolean(kConnected, &connected))
        {
            network.mpConnected = new bool();
            if (!network.mpConnected)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(network.mpConnected) = connected;
        }
        else
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        // priority (integer/optional)
        int priority;
        if (input_dict_value->GetInteger(kPriority, &priority))
        {
            network.mpPriority = new uint8_t();
            if (!network.mpPriority)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(network.mpPriority) = priority;
        }
        // dhcpEnabled (Boolean/mandatory)
        bool dhcp_enabled;
        if (input_dict_value->GetBoolean(kDhcpEnabled, &dhcp_enabled))
        {
            network.mpDhcpEnabled = new bool();
            if (!network.mpDhcpEnabled)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(network.mpDhcpEnabled) = dhcp_enabled;
        }
        else
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        // ipAddress (string/mandatory)
        std::string ip_address;
        if (input_dict_value->GetString(kIpAddress, &ip_address))
        {
            network.mpIpAddress = new String50();
            if (!network.mpIpAddress)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            network.mpIpAddress->value = ip_address;
        }
        else
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        // ipAddress (string/mandatory)
        std::string subnet_mask;
        if (input_dict_value->GetString(kSubnetMask, &subnet_mask))
        {
            network.mpSubnetMask = new String20();
            if (!network.mpSubnetMask)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            network.mpSubnetMask->value = subnet_mask;
        }
        else
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        // gateway (string/mandatory)
        std::string gateway;
        if (input_dict_value->GetString(kGateway, &gateway))
        {
            network.mpGateway = new String50();
            if (!network.mpGateway)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            network.mpGateway->value = gateway;
        }
        else
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        // dnsPrimary (string/mandatory)
        std::string dns_primary;
        if (input_dict_value->GetString(kDnsPrimary, &dns_primary))
        {
            network.mpDnsPrimary = new String50();
            if (!network.mpDnsPrimary)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            network.mpDnsPrimary->value = dns_primary;
        }
        else
        {
            LOG(ERROR) << "Mandatory field missing";
            return RT_400_BAD_REQUEST;
        }
        // dnsSecondary (string/optional)
        std::string dns_secondary;
        if (input_dict_value->GetString(kDnsSecondary, &dns_secondary))
        {
            network.mpDnsSecondary = new String50();
            if (!network.mpDnsSecondary)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            network.mpDnsSecondary->value = dns_secondary;
        }
        // description (string/optional)
        std::string desc;
        if (input_dict_value->GetString(kDescription, &desc))
        {
            network.mpDescription = new String50();
            if (!network.mpDescription)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            network.mpDescription->value = desc;
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetNetworksResponse(void *input, void *p_out)
    {
        Networks *networks = reinterpret_cast<Networks *>(input);
        std::vector<std::string> *id_list = reinterpret_cast<std::vector<std::string>*>(p_out);
        if (!networks || !id_list)
        {
            return RT_400_BAD_REQUEST;
        }
        // network list
        if (networks->mpNetwork.size())
        {
            std::list<Network *> nw_list = networks->mpNetwork;
            DLOG(INFO) << "(nw_list) size is " << nw_list.size();
            std::list<Network *>::iterator it_list;
            for (it_list = nw_list.begin(); it_list != nw_list.end(); it_list++)
            {
                id_list->push_back((*it_list)->mpId->value);
            }
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetNetworkResponse(const std::string &network_id, void *input,
            void *p_out)
    {
        Network *network = reinterpret_cast<Network *>(input);
        DictionaryValue *version_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!network || !version_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        version_values_dict->SetWithoutPathExpansion(kId, base::Value::CreateStringValue(network_id));
        // enabled (boolean/mandatory)
        if (network->mpEnabled)
        {
            DLOG(INFO) << "(enabled) is " << *network->mpEnabled;
            version_values_dict->SetWithoutPathExpansion(kEnabled,
                    base::Value::CreateBooleanValue(*network->mpEnabled));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // connected (boolean/mandatory)
        if (network->mpConnected)
        {
            DLOG(INFO) << "(connected) is " << *network->mpConnected;
            version_values_dict->SetWithoutPathExpansion(kConnected,
                    base::Value::CreateBooleanValue(*network->mpConnected));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // priority (Integer/optional)
        if (network->mpPriority)
        {
            DLOG(INFO) << "(priority) is " << network->mpPriority;
            version_values_dict->SetWithoutPathExpansion(kPriority,
                    base::Value::CreateIntegerValue(*network->mpPriority));
        }
        // dhcpEnabled (string/mandatory)
        if (network->mpDhcpEnabled)
        {
            DLOG(INFO) << "(dhcpEnabled) is " << network->mpDhcpEnabled;
            version_values_dict->SetWithoutPathExpansion(kDhcpEnabled,
                    base::Value::CreateBooleanValue(network->mpDhcpEnabled));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // ipAddress (string/mandatory)
        if (network->mpIpAddress)
        {
            DLOG(INFO) << "(ipAddress) is " << network->mpIpAddress;
            version_values_dict->SetWithoutPathExpansion(kIpAddress,
                    base::Value::CreateStringValue(network->mpIpAddress->value));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // subnetMask (string/mandatory)
        if (network->mpSubnetMask)
        {
            DLOG(INFO) << "(subnetMask) is " << network->mpSubnetMask;
            version_values_dict->SetWithoutPathExpansion(kSubnetMask,
                    base::Value::CreateStringValue(network->mpSubnetMask->value));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // gateway (string/mandatory)
        if (network->mpGateway)
        {
            DLOG(INFO) << "(gateway) is " << network->mpGateway;
            version_values_dict->SetWithoutPathExpansion(kGateway,
                    base::Value::CreateStringValue(network->mpGateway->value));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // dnsPrimary (string/mandatory)
        if (network->mpDnsPrimary)
        {
            DLOG(INFO) << "(dnsPrimary) is " << network->mpDnsPrimary;
            version_values_dict->SetWithoutPathExpansion(kDnsPrimary,
                    base::Value::CreateStringValue(network->mpDnsPrimary->value));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // dnsPrimary (string/optional)
        if (network->mpDnsSecondary)
        {
            DLOG(INFO) << "(dnsSecondary) is " << network->mpDnsSecondary;
            version_values_dict->SetWithoutPathExpansion(kDnsSecondary,
                    base::Value::CreateStringValue(network->mpDnsSecondary->value));
        }
        // macAddress (string/mandatory)
        if (network->mpMacAddress)
        {
            DLOG(INFO) << "(macAddress) is " << network->mpMacAddress;
            version_values_dict->SetWithoutPathExpansion(kMacAddress,
                    base::Value::CreateStringValue(network->mpMacAddress->value));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // description (string/optional)
        if (network->mpDescription)
        {
            DLOG(INFO) << "(description) is " << network->mpDescription;
            version_values_dict->SetWithoutPathExpansion(kDescription,
                    base::Value::CreateStringValue(network->mpDescription->value));
        }
        return RT_200_OK;
    }

    /**
      * Validation Check for ID
      * @param[in] string
      * @param[in] HTTPMessage*
      * @return static HTTPResponseType
      */
    HTTPResponseType CommonWrapperFunctions::ValidateNetworkID(const std::string &id,
            HTTPMessage *http)
    {
        DLOG(INFO) << "ValidateNetworkID " << id;
        std::vector<std::string> id_list;
        if (GetNetworkIDList(&id_list, http))
        {
            std::vector<std::string>::iterator it = id_list.begin();
            for (; it != id_list.end(); it++)
            {
                if (*it == id)
                {
                    DLOG(INFO) << "NetworkID found: " << id;
                    return RT_200_OK;
                }
            }
            // parsed the whole list and id not found, so return 404 from here
            return RT_404_NETWORK_ID;
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
    HTTPResponseType CommonWrapperFunctions::CreateNetworkID(void *p_in, std::string *id,
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
    HTTPResponseType CommonWrapperFunctions::DeleteNetworkID(void *p_in, const std::string &input_id,
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
    int CommonWrapperFunctions::GetNetworkIDList(std::vector<std::string> *id_list, HTTPMessage *http)
    {
        DLOG(INFO) << "GetNetworkIDList Enter";
        if (NULL == id_list || NULL == http)
        {
            LOG(ERROR) << "id_list or http is null";
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
        if (it == uri_field.end()) return -1;
        std::string device_id = *it;
        DLOG(INFO) << "device_id is :  " << device_id;
        int requestId;
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
        NetworksResource *networks_rsc = (NetworksResource *)cm_device->getResource("Networks");
        if (!networks_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return -1;
        }
        networks_rsc->buildPath(kBuildPathId);
        NetworksResourceResponseListener *respListener = NetworksResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return -1;
        }
        networks_rsc->addResponseListener(*respListener);
        LOG(ERROR) << "Network getNetworks";
        if (networks_rsc->getNetworks(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeNetworks;
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
                        FillGetNetworksResponse(ptr, id_list);
                    }
                }
            }
            else
            {
                return -1;
            }
            // delete the request id from CBmap, if any
            cm_wrapper->RemoveRequestFromCBMap(requestId);
        }
        else
        {
            LOG(ERROR) << "getNetworks failed";
            return -1;
        }
        return id_list->size();;
    }

    HTTPResponseType CommonWrapperFunctions::GetIdentifiedObjectNetwork(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetIdentifiedObjectNetwork Enter";
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
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
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
        NetworkResource *network_rsc = (NetworkResource *)cm_device->getResource("Network");
        if (!network_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        network_rsc->buildPath(kBuildPathId, network_id);
        NetworkResourceResponseListener *respListener = NetworkResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        network_rsc->addResponseListener(*respListener);
        int requestId = -1;
        LOG(ERROR) << "Network getNetwork";
        if (network_rsc->getNetwork(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeNetwork;
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
                        FillGetNetworkResponse(network_id, ptr, p_out);
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
            LOG(ERROR) << "getNetwork failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    HTTPResponseType CommonWrapperFunctions::PutIdentifiedObjectNetwork(void *p_in, void *p_out)
    {
        DLOG(INFO) << "PutIdentifiedObjectNetwork Enter";

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
        Network reqData;
        HTTPResponseType ret = ParseNetworkRequestBody(in_dict, reqData);
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
        NetworkResource *network_rsc = (NetworkResource *)cm_device->getResource("Network");
        if (!network_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        network_rsc->buildPath(kBuildPathId, network_id);
        NetworkResourceResponseListener *respListener = NetworkResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        network_rsc->addResponseListener(*respListener);
        int requestId = -1;
        LOG(ERROR) << "Device putNetwork";
        if (network_rsc->putNetwork(requestId, reqData))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeNetwork;
                cm_wrapper->AddRequestToCBMap(requestId, cm_db_data);
                boost::unique_lock<boost::mutex> lock(cm_db_data->lock);
                boost::system_time wait_time = boost::get_system_time() + boost::posix_time::milliseconds(
                                                   kRequestTimeout);
                if (!cm_db_data->condition.timed_wait(lock, wait_time))
                {
                    // Timeout
                    LOG(ERROR) << "Timeout happened";
                    cm_wrapper->RemoveRequestFromCBMap(requestId);
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
            LOG(ERROR) << "putNetwork failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }
}

