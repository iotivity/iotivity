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

#ifndef WEBSERVICE_INTERFACE_OBJECT_ID_MANAGER_H_
#define WEBSERVICE_INTERFACE_OBJECT_ID_MANAGER_H_

#include <vector>
#include <string>

#include "http_start_line/http_status_code_defined.h"
#include "object_id_manager/creator_object_id_manager.h"
#include "wrapped_component/wrapper_input_type.h"

namespace webservice
{

    class IObjectIDManager
    {
        public:
            /**
              * Validation Check for ID
              * @param[in] string
              * @return HTTPResponseType
              */
            virtual HTTPResponseType ValidateObjectID(std::string id, HTTPMessage *http = NULL) = 0;

            /**
              * Get Object ID list
              * @param[out] std::vector<std::string>* : id list
              * @return int
              */
            virtual int GetObjectIDList(std::vector<std::string> *id_list, HTTPMessage *http = NULL) = 0;

            /**
              * Creation ID
              * @param[in] void*
              * @param[out] string
              * @return static HTTPResponseType
              */
            virtual HTTPResponseType CreateObjectID(void *p_in, std::string *id, HTTPMessage *http = NULL) = 0;

            /**
              * Delete ID
              * @param[in] void*
              * @param[int] const string&
              * @return static HTTPResponseType
              */
            virtual HTTPResponseType DeleteObjectID(void *p_in, const std::string &id,
                                                    HTTPMessage *http = NULL) = 0;
    };

    class RegisterObjectIDManager
    {
        public:
            RegisterObjectIDManager(std::string object_name, IObjectIDManager * (*ptr_func_creator)())
            {
                CreatorObjectIDManager &creator = CreatorObjectIDManager::GetInstance();
                creator.RegisterObject(object_name, ptr_func_creator);
            }
    };

}

#endif /* WEBSERVICE_INTERFACE_OBJECT_ID_MANAGER_H_ */

