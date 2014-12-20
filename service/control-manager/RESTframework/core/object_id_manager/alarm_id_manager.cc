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

#include "base/values.h"
#include "base_resource/http_message.h"
#include "http_start_line/http_status_code_defined.h"
#include "object_id_manager/alarm_id_manager.h"
#include "object_id_manager/interface_object_id_manager.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

    /**
    * Register function to create this class.
    * @param[in] string
    * @param[in] IObjectIDManager*
    * @return static RegisterObjectIDManager
    */
    RegisterObjectIDManager AlarmIDManager::reg_object_id_manager("alarm", &AlarmIDManager::Create);

    /**
    * Create this class.
    * @return IObjectIDManager*
    */
    IObjectIDManager *AlarmIDManager::Create()
    {
        // This class will allocate in own class, so you should be careful to create this class.
        // Do NOT forget memory free of this allocated object.
        // The use of smart pointer is one of them for the solution.
        return new AlarmIDManager;
    }

    /**
      * Validation Check for ID
      * @param[in] string
      * @return static HTTPResponseType
      */
    HTTPResponseType AlarmIDManager::ValidateObjectID(std::string id, HTTPMessage *http)
    {
        return CommonWrapperFunctions::ValidateAlarmID(id, http);
    }

    /**
      * Get Object ID list
      * @param[out] std::vector<std::string>* : id list
      * @return int
      */
    int AlarmIDManager::GetObjectIDList(std::vector<std::string> *id_list, HTTPMessage *http)
    {
        return CommonWrapperFunctions::GetAlarmIDList(id_list, http);
    }

    /**
      * Create ID
      * @param[in] void*
      * @param[out] string*
      * @return static HTTPResponseType
      */
    HTTPResponseType AlarmIDManager::CreateObjectID(void *p_in, std::string *id, HTTPMessage *http)
    {
        DLOG(ERROR) << "Creating alarm";
        return CommonWrapperFunctions::CreateAlarmID(p_in, id, http);
    }

    /**
      * Delete ID
      * @param[in] void*
      * @param[int] const string&
      * @return static HTTPResponseType
      */
    HTTPResponseType AlarmIDManager::DeleteObjectID(void *p_in, const std::string &id,
            HTTPMessage *http)
    {
        return CommonWrapperFunctions::DeleteAlarmID(p_in, id, http);
    }

}
