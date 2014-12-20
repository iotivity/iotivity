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
#include "wrapped_component/response_listeners/information_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kDescription = "description";
    static const char *const kManufacturer = "manufacturer";
    static const char *const kModelID = "modelID";
    static const char *const kSerialNumber = "serialNumber";
    static const char *const kSalesLocation = "salesLocation";
    static const char *const kSpecification = "Specification";
    static const char *const kDeviceId = "device_id";
    static const char *const kPower = "power";
    static const char *const kDisplay = "display";
    static const char *const kSound = "sound";
    static const char *const kOtaSupported = "otaSupported";
    static const char *const kResetDeviceSupported = "resetDeviceSupported";
    static const char *const kResetFactoryDefaultSupported = "resetFactoryDefaultSupported";
    static const char *const kSupportedConnectivity = "supportedConnectivity";
    static const char *const kBuildPathId = "0";

    static HTTPResponseType FillSpecificationDetails(void *p_in, void *p_out)
    {
        DLOG(INFO) << "FillSpecificationDetail";
        CHECK_INPUT_ARGS
        Specification *specification = reinterpret_cast<Specification *>(p_in);
        DictionaryValue *spec_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!specification || !spec_dict)
        {
            LOG(ERROR) << "i/p or o/p param is NULL";
            return RT_400_BAD_REQUEST;
        }
        // no need to do null checking for the i/p params.
        // power (string/optional)
        if (specification->mpPower)
        {
            DLOG(INFO) << "(power) is " << specification->mpPower->value;
            spec_dict->SetWithoutPathExpansion(kPower,
                                               base::Value::CreateStringValue(specification->mpPower->value));
        }
        // display (string/optional)
        if (specification->mpDisplay)
        {
            DLOG(INFO) << "(display) is " << specification->mpDisplay->value;
            spec_dict->SetWithoutPathExpansion(kDisplay,
                                               base::Value::CreateStringValue(specification->mpDisplay->value));
        }
        // sound (string/optional)
        if (specification->mpSound)
        {
            DLOG(INFO) << "(sound) is " << specification->mpSound->value;
            spec_dict->SetWithoutPathExpansion(kSound,
                                               base::Value::CreateStringValue(specification->mpSound->value));
        }
        // otaSupported (boolean/optional)
        if (specification->mpOtaSupported)
        {
            DLOG(INFO) << "(otaSupported) is " << specification->mpOtaSupported;
            spec_dict->SetWithoutPathExpansion(kOtaSupported,
                                               base::Value::CreateBooleanValue(*specification->mpOtaSupported));
        }
        // resetDeviceSupported (boolean/optional)
        if (specification->mpResetDeviceSupported)
        {
            DLOG(INFO) << "(resetDeviceSupported) is " << specification->mpResetDeviceSupported;
            spec_dict->SetWithoutPathExpansion(kResetDeviceSupported,
                                               base::Value::CreateBooleanValue(*specification->mpResetDeviceSupported));
        }
        // resetFactoryDefaultSupported (boolean/optional)
        if (specification->mpResetFactoryDefaultSupported)
        {
            DLOG(INFO) << "(resetFactoryDefaultSupported) is " << specification->mpResetFactoryDefaultSupported;
            spec_dict->SetWithoutPathExpansion(kResetFactoryDefaultSupported,
                                               base::Value::CreateBooleanValue(*specification->mpResetFactoryDefaultSupported));
        }
        // supportedConnectivity (boolean/mandatory)
        if (specification->mpSupportedConnectivity)
        {
            ListValue *connectivity_list = new ListValue;
            if (connectivity_list)
            {
                std::list<ConnectivityType>::iterator it_list;
                for (it_list = specification->mpSupportedConnectivity->members.begin();
                     it_list != specification->mpSupportedConnectivity->members.end(); it_list++)
                {
                    ConnectivityType con_type = (*it_list);
                    if (con_type.value != "")
                    {
                        connectivity_list->Append(base::Value::CreateStringValue(con_type.value));
                    }
                }
                spec_dict->Set(kSupportedConnectivity, connectivity_list);
            }
            else
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
        }
        DLOG(INFO) << "FillSpecificationDetail Exit";
        return RT_200_OK;
    }

    static HTTPResponseType FillGetInformationResponse(void *p_in, void *p_out)
    {
        DLOG(INFO) << "FillGetInformationResponse";
        CHECK_INPUT_ARGS
        Information *information = reinterpret_cast<Information *>(p_in);
        DictionaryValue *information_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!information || !information_values_dict)
        {
            LOG(ERROR) << "i/p or o/p param is NULL";
            return RT_400_BAD_REQUEST;
        }
        // description (string/optional)
        if (information->mpDescription)
        {
            DLOG(INFO) << "(description) is " << information->mpDescription->value;
            information_values_dict->SetWithoutPathExpansion(kDescription,
                    base::Value::CreateStringValue(information->mpDescription->value));
        }
        // devicesubtype is not present in the SHP spec
        // manufacturer (string/optional)
        if (information->mpManufacturer)
        {
            DLOG(INFO) << "(manufacturer) is " << information->mpManufacturer->value;
            information_values_dict->SetWithoutPathExpansion(kManufacturer,
                    base::Value::CreateStringValue(information->mpManufacturer->value));
        }
        // modelID (string/optional)
        if (information->mpModelID)
        {
            DLOG(INFO) << "(modelID) is " << information->mpModelID->value;
            information_values_dict->SetWithoutPathExpansion(kModelID,
                    base::Value::CreateStringValue(information->mpModelID->value));
        }
        // SerialNumber (string/optional)
        if (information->mpSerialNumber)
        {
            DLOG(INFO) << "(serialNumber) is " << information->mpSerialNumber->value;
            information_values_dict->SetWithoutPathExpansion(kSerialNumber,
                    base::Value::CreateStringValue(information->mpSerialNumber->value));
        }
        // SalesLocation (LocationType/optional)
        if (information->mpSalesLocation)
        {
            DLOG(INFO) << "(salesLocation) is " << information->mpSalesLocation->value;
            information_values_dict->SetWithoutPathExpansion(kSalesLocation,
                    base::Value::CreateStringValue(information->mpSalesLocation->value));
        }
        DLOG(INFO) << "FillGetInformationResponse Exit";
        return RT_200_OK;
    }

    HTTPResponseType CommonWrapperFunctions::GetResourceInformation(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetResourceInformation Enter";
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
        InformationResource *info_rsc = (InformationResource *)cm_device->getResource("Information");
        if (!info_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        InformationResourceResponseListener *respListener =
            InformationResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        info_rsc->addResponseListener(*respListener);
        info_rsc->buildPath(kBuildPathId);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device getInformation";
        if (info_rsc->getInformation(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeInformation;
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
                        DLOG(INFO) << "Valid data received from callback  " << ptr;
                        // populate the response string
                        FillGetInformationResponse(ptr, p_out);
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
            LOG(ERROR) << "getInformation failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

// GetSpecification request is almost similar to GetResourceInformation
// Since Specification is a linked_child to Information, a separate wrapper function is required to populate the 'Specification' data.
    HTTPResponseType CommonWrapperFunctions::GetSpecification(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetSpecification Enter";
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
        InformationResource *info_rsc = (InformationResource *)cm_device->getResource("Information");
        if (!info_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        InformationResourceResponseListener *respListener =
            InformationResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        info_rsc->addResponseListener(*respListener);
        info_rsc->buildPath(kBuildPathId);
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId = -1;
        LOG(ERROR) << "Device getInformation";
        if (info_rsc->getInformation(requestId))
        {
            DLOG(INFO) << "specification requestId: " << requestId;
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeInformation;
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
                        DLOG(INFO) << "Valid data received from callback  " << ptr;
                        Information *information = reinterpret_cast<Information *>(ptr);
                        // populate the response string
                        FillSpecificationDetails(information->mpSpecification, p_out);
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
            LOG(ERROR) << "getInformation failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

}

