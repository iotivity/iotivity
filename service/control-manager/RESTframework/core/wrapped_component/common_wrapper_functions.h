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

#ifndef WEBSERVICE_COMMON_WRAPPER_FUNCTIONS_H_
#define WEBSERVICE_COMMON_WRAPPER_FUNCTIONS_H_

#include <string>
#include <vector>

#include "base/logging.h"
#include "base/values.h"
#include "base_resource/http_message.h"
#include "http_header/http_request_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"
#include "base/logging.h"
#include "wrapped_component/wrapper_input_type.h"
#include "rest_engine_export.h"

#undef CHECK_INPUT_ARGS
#define CHECK_INPUT_ARGS                                                          \
  if (NULL == p_in || NULL == p_out) {                                            \
    LOG(ERROR) << "Parameter is NULL!!, p_in : " << p_in << ", p_out : " << p_out;\
    return RT_400_BAD_REQUEST;                                                    \
  }

#undef CHECK_INPUT_DICTIONARY
#define CHECK_INPUT_DICTIONARY(x)             \
  if (NULL == x) {                            \
    LOG(ERROR) << "Input dictionary is NULL"; \
    return RT_400_BAD_REQUEST;                \
  }

namespace webservice
{

// (TODO) changed the time for testing, change it to 30 seconds
    const int kRequestTimeout = 10000; // in milliseconds (30 seconds)
// CommonWrapperFunctions is also related to web service, so use REST_ENGINE_EXPORT for this also.
    class REST_ENGINE_EXPORT CommonWrapperFunctions
    {
        public:

            static bool GetIdentifiedObjectDescription(void *p_in, void *p_out, const std::string &service_name,
                    const std::string &output_name);
            static bool SetIdentifiedObjectDescription(void *p_in, void *p_out, const std::string &service_name,
                    const std::string &input_name);

            static bool GetDeviceDescription(void *p_in, void *p_out, const std::string &service_name,
                                             const std::string &output_name);
            static bool SetDeviceDescription(void *p_in, void *p_out, const std::string &service_name,
                                             const std::string &input_name);

            // Add Set/Get wrappers here
            // Please do not modify above comment since it is being used by code generator

            static HTTPResponseType GetResourceDetection(void *p_in, void *p_out);
            static HTTPResponseType GetIdentifiedObjectDevice(void *p_in, void *p_out);
            static HTTPResponseType PutIdentifiedObjectDevice(void *p_in, void *p_out);
            static HTTPResponseType GetResourceInformation(void *p_in, void *p_out);
            static HTTPResponseType GetSpecification(void *p_in, void *p_out);
            static HTTPResponseType GetIdentifiedObjectVersion(void *p_in, void *p_out);
            static HTTPResponseType PutIdentifiedObjectVersion(void *p_in, void *p_out);
            static HTTPResponseType GetResourceWiFi(void *p_in, void *p_out);
            static HTTPResponseType PutResourceWiFi(void *p_in, void *p_out);
            static HTTPResponseType GetIdentifiedObjectAccessPoint(void *p_in, void *p_out);
            static HTTPResponseType GetResourceConfiguration(void *p_in, void *p_out);
            static HTTPResponseType PutResourceConfiguration(void *p_in, void *p_out);
            static HTTPResponseType GetIdentifiedObjectNetwork(void *p_in, void *p_out);
            static HTTPResponseType PutIdentifiedObjectNetwork(void *p_in, void *p_out);
            static HTTPResponseType GetResourceTime(void *p_in, void *p_out);
            static HTTPResponseType PutResourceTime(void *p_in, void *p_out);
            static HTTPResponseType GetIdentifiedObjectAlarm(void *p_in, void *p_out);
            static HTTPResponseType GetResourceEnergyConsumption(void *p_in, void *p_out);
            static HTTPResponseType PutResourceEnergyConsumption(void *p_in, void *p_out);
            static HTTPResponseType GetIdentifiedObjectDoor(void *p_in, void *p_out);
            static HTTPResponseType PutIdentifiedObjectDoor(void *p_in, void *p_out);
            static HTTPResponseType GetResourceHumidity(void *p_in, void *p_out);
            static HTTPResponseType PutResourceHumidity(void *p_in, void *p_out);
            static HTTPResponseType GetResourceLight(void *p_in, void *p_out);
            static HTTPResponseType PutResourceLight(void *p_in, void *p_out);
            static HTTPResponseType GetColor(void *p_in, void *p_out);
            static HTTPResponseType PutColor(void *p_in, void *p_out);
            static HTTPResponseType GetResourceMode(void *p_in, void *p_out);
            static HTTPResponseType PutResourceMode(void *p_in, void *p_out);
            static HTTPResponseType GetResourceMovement(void *p_in, void *p_out);
            static HTTPResponseType PutResourceMovement(void *p_in, void *p_out);
            static HTTPResponseType GetResourceOperation(void *p_in, void *p_out);
            static HTTPResponseType PutResourceOperation(void *p_in, void *p_out);
            static HTTPResponseType GetIdentifiedObjectTemperature(void *p_in, void *p_out);
            static HTTPResponseType PutResourceDetection(void *p_in, void *p_out);
            static HTTPResponseType PutIdentifiedObjectTemperature(void *p_in, void *p_out);
            static HTTPResponseType GetImage(void *p_in, void *p_out);
            static HTTPResponseType GetResourceThermostat(void *p_in, void *p_out);
            static HTTPResponseType PutResourceThermostat(void *p_in, void *p_out);
            static HTTPResponseType GetResourceLevel(void *p_in, void *p_out);
            static HTTPResponseType PutResourceLevel(void *p_in, void *p_out);
            // Add Multiple Attribute Get/Put/Post/Delete wrappers here
            // Please do not modify above comment since it is being used by code generator

            // Create functions
            /**
              * Validation Check for ID
              * @param[in] string
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType ValidateDeviceID(const std::string &id, HTTPMessage *http);
            /**
              * Get Object ID list
              * @param[out] std::vector<std::string>* : id list
              * @param[in] HTTPMessage*
              * @return int
              */
            static int GetDeviceIDList(std::vector<std::string> *id_list, HTTPMessage *http);
            /**
              * Create ID
              * @param[in] void*
              * @param[out] string*
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType CreateDeviceID(void *p_in, std::string *id, HTTPMessage *http);
            /**
              * Delete ID
              * @param[in] void*
              * @param[in] const string&
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType DeleteDeviceID(void *p_in, const std::string &input_id, HTTPMessage *http);

            /**
              * Validation Check for ID
              * @param[in] string
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType ValidateAlarmID(const std::string &id, HTTPMessage *http);
            /**
              * Get Object ID list
              * @param[out] std::vector<std::string>* : id list
              * @param[in] HTTPMessage*
              * @return int
              */
            static int GetAlarmIDList(std::vector<std::string> *id_list, HTTPMessage *http);
            /**
              * Create ID
              * @param[in] void*
              * @param[out] string*
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType CreateAlarmID(void *p_in, std::string *id, HTTPMessage *http);
            /**
              * Delete ID
              * @param[in] void*
              * @param[in] const string&
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType DeleteAlarmID(void *p_in, const std::string &input_id, HTTPMessage *http);

            /**
              * Validation Check for ID
              * @param[in] string
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType ValidateAccessPointID(const std::string &id, HTTPMessage *http);
            /**
              * Get Object ID list
              * @param[out] std::vector<std::string>* : id list
              * @param[in] HTTPMessage*
              * @return int
              */
            static int GetAccessPointIDList(std::vector<std::string> *id_list, HTTPMessage *http);
            /**
              * Create ID
              * @param[in] void*
              * @param[out] string*
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType CreateAccessPointID(void *p_in, std::string *id, HTTPMessage *http);
            /**
              * Delete ID
              * @param[in] void*
              * @param[in] const string&
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType DeleteAccessPointID(void *p_in, const std::string &input_id,
                    HTTPMessage *http);

            /**
              * Validation Check for ID
              * @param[in] string
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType ValidateNetworkID(const std::string &id, HTTPMessage *http);
            /**
              * Get Object ID list
              * @param[out] std::vector<std::string>* : id list
              * @param[in] HTTPMessage*
              * @return int
              */
            static int GetNetworkIDList(std::vector<std::string> *id_list, HTTPMessage *http);
            /**
              * Create ID
              * @param[in] void*
              * @param[out] string*
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType CreateNetworkID(void *p_in, std::string *id, HTTPMessage *http);
            /**
              * Delete ID
              * @param[in] void*
              * @param[in] const string&
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType DeleteNetworkID(void *p_in, const std::string &input_id, HTTPMessage *http);

            /**
              * Validation Check for ID
              * @param[in] string
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType ValidateVersionID(const std::string &id, HTTPMessage *http);
            /**
              * Get Object ID list
              * @param[out] std::vector<std::string>* : id list
              * @param[in] HTTPMessage*
              * @return int
              */
            static int GetVersionIDList(std::vector<std::string> *id_list, HTTPMessage *http);
            /**
              * Create ID
              * @param[in] void*
              * @param[out] string*
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType CreateVersionID(void *p_in, std::string *id, HTTPMessage *http);
            /**
              * Delete ID
              * @param[in] void*
              * @param[in] const string&
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType DeleteVersionID(void *p_in, const std::string &input_id, HTTPMessage *http);

            /**
              * Validation Check for ID
              * @param[in] string
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType ValidateDoorID(const std::string &id, HTTPMessage *http);
            /**
              * Get Object ID list
              * @param[out] std::vector<std::string>* : id list
              * @param[in] HTTPMessage*
              * @return int
              */
            static int GetDoorIDList(std::vector<std::string> *id_list, HTTPMessage *http);
            /**
              * Create ID
              * @param[in] void*
              * @param[out] string*
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType CreateDoorID(void *p_in, std::string *id, HTTPMessage *http);
            /**
              * Delete ID
              * @param[in] void*
              * @param[in] const string&
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType DeleteDoorID(void *p_in, const std::string &input_id, HTTPMessage *http);

            /**
              * Validation Check for ID
              * @param[in] string
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType ValidateMessageID(const std::string &id, HTTPMessage *http);
            /**
              * Validation Check for ID
              * @param[in] string
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType ValidateTemperatureID(const std::string &id, HTTPMessage *http);
            /**
              * Get Object ID list
              * @param[out] std::vector<std::string>* : id list
              * @param[in] HTTPMessage*
              * @return int
              */
            static int GetTemperatureIDList(std::vector<std::string> *id_list, HTTPMessage *http);
            /**
              * Create ID
              * @param[in] void*
              * @param[out] string*
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType CreateTemperatureID(void *p_in, std::string *id, HTTPMessage *http);
            /**
              * Delete ID
              * @param[in] void*
              * @param[in] const string&
              * @param[in] HTTPMessage*
              * @return static HTTPResponseType
              */
            static HTTPResponseType DeleteTemperatureID(void *p_in, const std::string &input_id,
                    HTTPMessage *http);

            // Add ID manager wrappers here
            // Please do not modify above comment since it is being used by code generator

        private:

            static std::string GetDeviceProperty(const std::string &device_id,
                                                 const std::string &property_name);
            static std::string GenerateUUID(void);

            static bool GetUserUUID(const std::string &session_id, std::string *p_user_uuid);
            static bool GetAccountDefaultValue(std::string *p_option_value);

            static bool ValidateUser(const std::string &user_uuid);
    };
}

#endif /* WEBSERVICE_COMMON_WRAPPER_FUNCTIONS_H_ */

