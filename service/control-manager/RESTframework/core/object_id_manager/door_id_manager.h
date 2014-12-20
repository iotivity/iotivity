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

#ifndef WEBSERVICE_DOOR_ID_MANAGER_H_
#define WEBSERVICE_DOOR_ID_MANAGER_H_

#include <string>

#include "base_resource/http_message.h"
#include "http_start_line/http_status_code_defined.h"
#include "object_id_manager/interface_object_id_manager.h"

namespace webservice
{

    class DoorIDManager : public IObjectIDManager
    {
        public:
            /* NOT used for this class */
            virtual ~DoorIDManager(void) {}

            /**
              * Create this class.
              * @return IObjectIDManager*
              */
            static IObjectIDManager *Create(void);

            /**
              * Register function to create this class.
              * @param[in] string
              * @param[in] IObjectIDManager*
              * @return static RegisterObjectIDManager
              */
            static RegisterObjectIDManager reg_object_id_manager;

        public:
            /**
              * Validation Check for ID
              * @param[in] string
              * @return static HTTPResponseType
              */
            HTTPResponseType ValidateObjectID(std::string id, HTTPMessage *http = NULL);

            /**
              * Get Object ID list
              * @param[out] std::vector<std::string>* : id list
              * @return int
              */
            int GetObjectIDList(std::vector<std::string> *id_list, HTTPMessage *http = NULL);

            /**
              * Create ID
              * @param[in] void*
              * @param[out] string
              * @return static HTTPResponseType
              */
            HTTPResponseType CreateObjectID(void *p_in, std::string *id, HTTPMessage *http = NULL);

            /**
              * Delete ID
              * @param[in] void*
              * @param[int] const string&
              * @return static HTTPResponseType
              */
            HTTPResponseType DeleteObjectID(void *p_in, const std::string &id, HTTPMessage *http = NULL);
    };
}

#endif /* WEBSERVICE_DOOR_ID_MANAGER_H_ */


