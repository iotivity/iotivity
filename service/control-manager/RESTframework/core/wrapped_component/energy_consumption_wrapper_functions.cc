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
#include "wrapped_component/response_listeners/energy_consumption_rsc_resp_listener.h"

namespace webservice
{

    static const char *const kBuildPathId = "0";
    static const char *const kBatteryCharge = "batteryCharge";
    static const char *const kInstantaneousPower = "instantaneousPower";
    static const char *const kCumulativeConsumption = "cumulativeConsumption";
    static const char *const kStartTime = "startTime";
    static const char *const kHourlyConsumption = "hourlyConsumption";
    static const char *const kDailyConsumption = "dailyConsumption";
    static const char *const kWeeklyConsumption = "weeklyConsumption";
    static const char *const kMonthlyConsumption = "monthlyConsumption";
    static const char *const kReportInterval = "reportInterval";
    static const char *const kUsageThreshold = "usageThreshold";
    static const char *const kSaveLocation = "saveLocation";
    static const char *const kDeviceId = "device_id";

// FillGetEnergyResponse & ParseEnergyRequestBody used only in this file, so keeping them as static
    static HTTPResponseType ParseEnergyRequestBody(base::DictionaryValue *input_dict_value,
            EnergyConsumption &energy_consumption)
    {
        if (!input_dict_value)
        {
            return RT_400_BAD_REQUEST;
        }
        // batteryCharge (uint8/optional)
        int battery_charge;
        if (input_dict_value->GetInteger(kBatteryCharge, &battery_charge))
        {
            energy_consumption.mpBatteryCharge = new uint8_t();
            if (!energy_consumption.mpBatteryCharge)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(energy_consumption.mpBatteryCharge) = battery_charge;
        }
        // instantaneousPower (Float32/optional)
        double power;
        if (input_dict_value->GetDouble(kInstantaneousPower, &power))
        {
            energy_consumption.mpInstantaneousPower = new Float32();
            if (!energy_consumption.mpInstantaneousPower)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            energy_consumption.mpInstantaneousPower->value = power;
        }
        // cumulativeConsumption (Float32/optional)
        double consumption;
        if (input_dict_value->GetDouble(kCumulativeConsumption, &consumption))
        {
            energy_consumption.mpCumulativeConsumption = new Float32();
            if (!energy_consumption.mpCumulativeConsumption)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            energy_consumption.mpCumulativeConsumption->value = consumption;
        }
        // startTime (string/optional)
        std::string start_time;
        if (input_dict_value->GetString(kStartTime, &start_time))
        {
            energy_consumption.mpStartTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (!energy_consumption.mpStartTime)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            energy_consumption.mpStartTime->value = start_time;
        }
        // hourlyConsumption (Float32/optional)
        double hourly_consumption;
        if (input_dict_value->GetDouble(kHourlyConsumption, &hourly_consumption))
        {
            energy_consumption.mpHourlyConsumption = new Float32();
            if (!energy_consumption.mpHourlyConsumption)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            energy_consumption.mpHourlyConsumption->value = hourly_consumption;
        }
        // dailyConsumption (Float32/optional)
        double daily_consumption;
        if (input_dict_value->GetDouble(kDailyConsumption, &daily_consumption))
        {
            energy_consumption.mpDailyConsumption = new Float32();
            if (!energy_consumption.mpDailyConsumption)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            energy_consumption.mpDailyConsumption->value = daily_consumption;
        }
        // weeklyConsumption (Float32/optional)
        double weekly_consumption;
        if (input_dict_value->GetDouble(kWeeklyConsumption, &weekly_consumption))
        {
            energy_consumption.mpWeeklyConsumption = new Float32();
            if (!energy_consumption.mpWeeklyConsumption)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            energy_consumption.mpWeeklyConsumption->value = weekly_consumption;
        }
        // monthlyConsumption (Float32/optional)
        double monthly_consumption;
        if (input_dict_value->GetDouble(kMonthlyConsumption, &monthly_consumption))
        {
            energy_consumption.mpMonthlyConsumption = new Float32();
            if (!energy_consumption.mpMonthlyConsumption)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            energy_consumption.mpMonthlyConsumption->value = monthly_consumption;
        }
        // reportInterval (uint8/optional)
        int report_interval;
        if (input_dict_value->GetInteger(kReportInterval, &report_interval))
        {
            energy_consumption.mpReportInterval = new uint32_t();
            if (!energy_consumption.mpReportInterval)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(energy_consumption.mpReportInterval) = report_interval;
        }
        // usageThreshold (Float32/optional)
        double usage_threshold;
        if (input_dict_value->GetDouble(kUsageThreshold, &usage_threshold))
        {
            energy_consumption.mpUsageThreshold = new Float32();
            if (!energy_consumption.mpUsageThreshold)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            energy_consumption.mpUsageThreshold->value = usage_threshold;
        }
        // saveLocation (Float32/optional)
        std::string save_ocation;
        if (input_dict_value->GetString(kSaveLocation, &save_ocation))
        {
            energy_consumption.mpSaveLocation = new std::string();
            if (!energy_consumption.mpSaveLocation)
            {
                return RT_500_INTERNAL_SERVER_ERROR;
            }
            *(energy_consumption.mpSaveLocation) = save_ocation;
        }
        return RT_200_OK;
    }

    static HTTPResponseType FillGetEnergyResponse(void *input, void *p_out)
    {
        EnergyConsumption *energy_consumption = reinterpret_cast<EnergyConsumption *>(input);
        DictionaryValue *energy_consumption_values_dict = reinterpret_cast<DictionaryValue *>(p_out);
        if (!energy_consumption || !energy_consumption_values_dict)
        {
            return RT_400_BAD_REQUEST;
        }
        // batteryCharge (uint8/optional)
        if (energy_consumption->mpBatteryCharge)
        {
            DLOG(INFO) << "(batteryCharge) is " << energy_consumption->mpBatteryCharge;
            energy_consumption_values_dict->SetWithoutPathExpansion(kBatteryCharge,
                    base::Value::CreateIntegerValue(*energy_consumption->mpBatteryCharge));
        }
        // instantaneousPower (Float32/optional)
        if (energy_consumption->mpInstantaneousPower)
        {
            DLOG(INFO) << "(instantaneousPower) is " << energy_consumption->mpInstantaneousPower->value;
            energy_consumption_values_dict->SetWithoutPathExpansion(kInstantaneousPower,
                    base::Value::CreateDoubleValue(energy_consumption->mpInstantaneousPower->value));
        }
        // cumulativeConsumption (Float32/optional)
        if (energy_consumption->mpCumulativeConsumption)
        {
            DLOG(INFO) << "(cumulativeConsumption) is " << energy_consumption->mpCumulativeConsumption->value;
            energy_consumption_values_dict->SetWithoutPathExpansion(kCumulativeConsumption,
                    base::Value::CreateDoubleValue(energy_consumption->mpCumulativeConsumption->value));
        }
        // startTime (string/optional)
        if (energy_consumption->mpStartTime)
        {
            DLOG(INFO) << "(startTime) is " << energy_consumption->mpStartTime->value;
            energy_consumption_values_dict->SetWithoutPathExpansion(kStartTime,
                    base::Value::CreateStringValue(energy_consumption->mpStartTime->value));
        }
        // hourlyConsumption (Float32/optional)
        if (energy_consumption->mpHourlyConsumption)
        {
            DLOG(INFO) << "(hourlyConsumption) is " << energy_consumption->mpHourlyConsumption->value;
            energy_consumption_values_dict->SetWithoutPathExpansion(kHourlyConsumption,
                    base::Value::CreateDoubleValue(energy_consumption->mpHourlyConsumption->value));
        }
        // dailyConsumption (Float32/optional)
        if (energy_consumption->mpDailyConsumption)
        {
            DLOG(INFO) << "(dailyConsumption) is " << energy_consumption->mpDailyConsumption->value;
            energy_consumption_values_dict->SetWithoutPathExpansion(kDailyConsumption,
                    base::Value::CreateDoubleValue(energy_consumption->mpDailyConsumption->value));
        }
        // weeklyConsumption (Float32/optional)
        if (energy_consumption->mpWeeklyConsumption)
        {
            DLOG(INFO) << "(weeklyConsumption) is " << energy_consumption->mpWeeklyConsumption->value;
            energy_consumption_values_dict->SetWithoutPathExpansion(kWeeklyConsumption,
                    base::Value::CreateDoubleValue(energy_consumption->mpWeeklyConsumption->value));
        }
        // monthlyConsumption (Float32/optional)
        if (energy_consumption->mpMonthlyConsumption)
        {
            DLOG(INFO) << "(monthlyConsumption) is " << energy_consumption->mpMonthlyConsumption->value;
            energy_consumption_values_dict->SetWithoutPathExpansion(kMonthlyConsumption,
                    base::Value::CreateDoubleValue(energy_consumption->mpMonthlyConsumption->value));
        }
        // reportInterval (unit8/optional)
        if (energy_consumption->mpReportInterval)
        {
            DLOG(INFO) << "(reportInterval) is " << energy_consumption->mpReportInterval;
            energy_consumption_values_dict->SetWithoutPathExpansion(kReportInterval,
                    base::Value::CreateIntegerValue(*energy_consumption->mpReportInterval));
        }
        // usageThreshold (Float32/optional)
        if (energy_consumption->mpUsageThreshold)
        {
            DLOG(INFO) << "(usageThreshold) is " << energy_consumption->mpUsageThreshold->value;
            energy_consumption_values_dict->SetWithoutPathExpansion(kUsageThreshold,
                    base::Value::CreateDoubleValue(energy_consumption->mpUsageThreshold->value));
        }
        // saveLocation (string/optional)
        if (energy_consumption->mpSaveLocation)
        {
            DLOG(INFO) << "(saveLocation) is " << energy_consumption->mpSaveLocation;
            energy_consumption_values_dict->SetWithoutPathExpansion(kSaveLocation,
                    base::Value::CreateStringValue(*energy_consumption->mpSaveLocation));
        }
        return RT_200_OK;
    }

    HTTPResponseType CommonWrapperFunctions::GetResourceEnergyConsumption(void *p_in, void *p_out)
    {
        DLOG(INFO) << "GetResourceEnergyConsumption Enter";
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
        EnergyConsumptionResource *ec_config_rsc = (EnergyConsumptionResource *)
                cm_device->getResource("EnergyConsumption");
        if (!ec_config_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        ec_config_rsc->buildPath(kBuildPathId);
        EnergyConsumptionResourceResponseListener *respListener =
            EnergyConsumptionResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        ec_config_rsc->addResponseListener(*respListener);
        LOG(ERROR) << "Device getEnergyConsumption";
        HTTPResponseType ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId;
        if (ec_config_rsc->getEnergyConsumption(requestId))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeEnergyConsumption;
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
                        FillGetEnergyResponse(ptr, p_out);
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
            LOG(ERROR) << "getEnergyConsumption failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }

    HTTPResponseType CommonWrapperFunctions::PutResourceEnergyConsumption(void *p_in, void *p_out)
    {
        DLOG(INFO) << "PutResourceEnergyConsumption Enter";
        CHECK_INPUT_ARGS
        base::DictionaryValue *in_dict = (reinterpret_cast<WrapperInput *>(p_in))->dictionary;
        CHECK_INPUT_DICTIONARY(in_dict)

        std::string device_id = "";
        if (!in_dict->GetString(kDeviceId, &device_id))
        {
            DLOG(ERROR) << "Input parameter is not get device_id";
            return RT_400_BAD_REQUEST;
        }
        EnergyConsumption reqData;
        HTTPResponseType ret = ParseEnergyRequestBody(in_dict, reqData);
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
        EnergyConsumptionResource *ec_config_rsc = (EnergyConsumptionResource *)
                cm_device->getResource("EnergyConsumption");
        if (!ec_config_rsc)
        {
            LOG(ERROR) << "Get resource failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        ec_config_rsc->buildPath(kBuildPathId);
        EnergyConsumptionResourceResponseListener *respListener =
            EnergyConsumptionResourceResponseListener::GetInstance();
        if (!respListener)
        {
            LOG(ERROR) << "ResourceResponseListener GetInstance failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        ec_config_rsc->addResponseListener(*respListener);
        ret = RT_500_INTERNAL_SERVER_ERROR;
        int requestId;
        LOG(ERROR) << "Device putEnergyConsumption";
        if (ec_config_rsc->putEnergyConsumption(requestId, reqData))
        {
            CmCbData *cm_db_data = new CmCbData();
            if (cm_db_data)
            {
                cm_db_data->rsc_type = kResourceTypeEnergyConsumption;
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
            LOG(ERROR) << "putEnergyConsumption failed";
            return RT_500_INTERNAL_SERVER_ERROR;
        }
        return ret;
    }
}

