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
#include "wrapped_component/response_listeners/version_rsc_resp_listener.h"
#include "wrapped_component/response_listeners/versions_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kBuildPathId = "0";
    static const char *const kId = "id";
    static const char *const kType = "type";
    static const char *const kName = "name";
    static const char *const kNumber = "number";
    static const char *const kDeviceId = "device_id";
    static const char *const kVersionId = "version_id";
    static const char *const kNewVersionAvailable = "newVersionAvailable";
    static const char *const kUserAgreement = "userAgreement";
    static const char *const kDescription = "description";
    static const char *const kDevices = "devices";

// FillGetVersionResponse & ParseVersionRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseVersionRequestBody(base::DictionaryValue *input_dict_value,
            Version &version)
    {
        if (!input_dict_value)
        {
            return RT_400_BAD_REQUEST;
        }
        // userAgreement (boolean/mandatory)
        bool user_agreement;
        if (input_dict_value->GetBoolean(kUserAgreement, &user_agreement))
        {
            version.mpUserAgreement = new bool();
            if (!version.mpUserAgreement)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *version.mpUserAgreement = user_agreement;
        }
        // description (string/optional)
        std::string desc;
        if (input_dict_value->GetString(kDescription, &desc))
        {
            version.mpDescription = new String50();
            if (!version.mpDescription)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            version.mpDescription->value = desc;
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetVersionResponse(const std::string &version_id, void *input,
            void *p_out)
    {
        Version *version = reinterpret_cast<Version *>(input);
        DictionaryValue *version_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!version || !version_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        version_values_dict->SetWithoutPathExpansion(kId, base::Value::CreateStringValue(version_id));
        // type (string/mandatory)
        if (version->mpType)
        {
            DLOG(INFO) << "(type) is " << version->mpType->value;
            version_values_dict->SetWithoutPathExpansion(kType,
                    base::Value::CreateStringValue(version->mpType->value));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // name (string/optional)
        if (version->mpName)
        {
            DLOG(INFO) << "(name) is " << version->mpName->value;
            version_values_dict->SetWithoutPathExpansion(kName,
                    base::Value::CreateStringValue(version->mpName->value));
        }
        // number (string/mandatory)
        if (version->mpNumber)
        {
            DLOG(INFO) << "(modelID) is " << version->mpNumber->value;
            version_values_dict->SetWithoutPathExpansion(kNumber,
                    base::Value::CreateStringValue(version->mpNumber->value));
        }
        else
        {
            LOG(ERROR) << "Mandatory data missing";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        // newVersionAvailable (boolean/optional)
        if (version->mpNewVersionAvailable)
        {
            DLOG(INFO) << "(newVersionAvailable) is " << version->mpNewVersionAvailable;
            version_values_dict->SetWithoutPathExpansion(kNewVersionAvailable,
                    base::Value::CreateBooleanValue(version->mpNewVersionAvailable));
        }
        // userAgreement (boolean)
        if (version->mpUserAgreement)
        {
            DLOG(INFO) << "(userAgreement) is " << version->mpUserAgreement;
            version_values_dict->SetWithoutPathExpansion(kUserAgreement,
                    base::Value::CreateBooleanValue(version->mpUserAgreement));
        }
        // description (string/optional)
        if (version->mpDescription)
        {
            DLOG(INFO) << "(description) is " << version->mpDescription;
            version_values_dict->SetWithoutPathExpansion(kDescription,
                    base::Value::CreateStringValue(version->mpDescription->value));
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetVersionsResponse(void *input, void *p_out)
    {
        Versions *versions = reinterpret_cast<Versions *>(input);
        std::vector<std::string> *id_list = reinterpret_cast<std::vector<std::string>*>(p_out);
        if (!versions || !id_list)
        {
            return RT_400_BAD_REQUEST;
        }
        // version list
        if (versions->mpVersion.size())
        {
            std::list<Version *> version_list = versions->mpVersion;
            DLOG(INFO) << "(version_list) size is " << version_list.size();
            std::list<Version *>::iterator it_list;
            for (it_list = version_list.begin(); it_list != version_list.end(); it_list++)
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
    HTTPResponseType CommonWrapperFunctions::ValidateVersionID(const std::string &id,
            HTTPMessage *http)
    {
        DLOG(INFO) << "ValidateVersionID";
        std::vector<std::string> id_list;
        if (GetVersionIDList(&id_list, http))
        {
            std::vector<std::string>::iterator it = id_list.begin();
            for (; it != id_list.end(); it++)
            {
                if (*it == id)
                {
                    DLOG(INFO) << "VersionID found: " << id;
                    return RT_200_OK;
                }
            }
            // parsed the whole list and id not found, so return 404 from here
            return RT_404_VERSION_ID;
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
    HTTPResponseType CommonWrapperFunctions::CreateVersionID(void *p_in, std::string *id,
            HTTPMessage *http)
    {
        return RT_405_METHOD_NOT_ALLOWED;
    }

    /**
      * Delete ID
      * @param[in] void*
      * @param[in] const string&
      * @param[in] HTTPMessage*
      * @return static HTTPResponseType
      */
    HTTPResponseType CommonWrapperFunctions::DeleteVersionID(void *p_in, const std::string &input_id,
            HTTPMessage *http)
    {
        return RT_405_METHOD_NOT_ALLOWED;
    }

    /**
      * Get Object ID list
      * @param[out] std::vector<std::string>* : id list
      * @param[in] HTTPMessage*
      * @return int
      */
    int CommonWrapperFunctions::GetVersionIDList(std::vector<std::string> *id_list, HTTPMessage *http)
    {
        DLOG(INFO) << "GetVersionIDList Enter";
        if (!id_list || !http) return -1;
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
        VersionsResource *versions_rsc = (VersionsResource *)cm_device->getResource("Versions");
        if (!versions_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return -1;
        }
        versions_rsc->buildPath(kBuildPathId);
        VersionsResourceResponseListener *respListener = VersionsResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return -1;
        }
        versions_rsc->addResponseListener(*respListener);
        LOG(ERROR) << "Version getVersions";
        if (versions_rsc->getVersions(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeVersions;
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
                        FillGetVersionsResponse(ptr, id_list);
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
            LOG(ERROR) << "getVersions failed";
            return -1;
        }
        return id_list->size();
    }

    HTTPResponseType CommonWrapperFunctions::GetIdentifiedObjectVersion(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetIdentifiedObjectVersion Enter";
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)
        std::string device_id = "";
        std::string version_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        if (!in_dict->GetString(kVersionId, &version_id))
        {
            DLOG(ERROR) << "Input parameter is not get version_id";
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
        VersionResource *version_rsc = (VersionResource *)cm_device->getResource("Version");
        if (!version_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        version_rsc->buildPath(kBuildPathId, version_id);
        VersionResourceResponseListener *respListener = VersionResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        version_rsc->addResponseListener(*respListener);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device getVersion";
        if (version_rsc->getVersion(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeVersion;
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
                        FillGetVersionResponse(version_id, ptr, p_out);
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
            LOG(ERROR) << "getVersion failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    HTTPResponseType CommonWrapperFunctions::PutIdentifiedObjectVersion(void *p_in, void *p_out)
    {
        DLOG(INFO) << "PutIdentifiedObjectVersion Enter";

        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)

        std::string device_id = "";
        std::string version_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        if (!in_dict->GetString(kVersionId, &version_id))
        {
            DLOG(ERROR) << "Input parameter is not get version_id";
            return RT_400_BAD_REQUEST;
        }
        Version reqData;
        HTTPResponseType ret = ParseVersionRequestBody(in_dict, reqData);
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
        VersionResource *version_rsc = (VersionResource *)cm_device->getResource("Version");
        if (!version_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        version_rsc->buildPath(kBuildPathId, version_id);
        VersionResourceResponseListener *respListener = VersionResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        version_rsc->addResponseListener(*respListener);
        LOG(ERROR) << "Device putVersion";
        if (version_rsc->putVersion(requestId, reqData))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeVersion;
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
            LOG(ERROR) << "putVersion failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }
}

