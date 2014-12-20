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

#ifndef WEBSERVICE_SRC_NEW_RESOURCE_HANDLER_GENERATED_RESOURCE_IDENTIFIED_OBJECT_DOOR_H_
#define WEBSERVICE_SRC_NEW_RESOURCE_HANDLER_GENERATED_RESOURCE_IDENTIFIED_OBJECT_DOOR_H_

#include <map>

#include "base_resource/base_identified_object.h"
#include "data_types/simple_data_type.h"
#include "data_types/simple_array_data_type.h"
#include "http_start_line/http_status_code_defined.h"

namespace webservice
{

    class IdentifiedObjectDoor : public BaseIdentifiedObject
    {
        public:
            IdentifiedObjectDoor(std::string id, std::string object_name);
            virtual ~IdentifiedObjectDoor(void);
            /**
              * Create this class.
              * @return BaseIdentifiedObject*
              */
            static BaseIdentifiedObject *Create(std::string id);

        private:
            /**
              * Validation Check for ID
              * @return static HTTPResponseType
              */
            virtual HTTPResponseType ValidateObjectID(void);
            /**
              * Create ID
              * @param[in] void*
              * @param[out] string*
              * @return static HTTPResponseType
              */
            virtual HTTPResponseType CreateObjectID(void *p_in, std::string *id);
            /**
              * Delete ID
              * @param[in] void*
              * @param[in] const string&
              * @return static HTTPResponseType
              */
            virtual HTTPResponseType DeleteObjectID(void *p_in, const std::string &id);
            /**
              * Save current ID for child URI
              * @param[in] std::map&
              * @return void
              */
            virtual void SaveCurrentIDWithName(std::map<std::string, std::string> &id_deps);

        public:
            /**
              * Register function to create this class.
              * @param[in] string
              * @param[in] BaseIdentifiedObject*
              * @return static RegisterIdentifiedObject
              */
            static RegisterIdentifiedObject regIdentifiedObject;

        private:
            String10 *name_;
            OpenType *openState_;
            TimeType *openElapsedTime_;
            DateTimeType *openTime_;
            LockType *lock_;
            String20 *password_;
            String20 *oneTimePassword_;
            _boolean *emergencyAlarm_;
    };

}

#endif /* WEBSERVICE_SRC_NEW_RESOURCE_HANDLER_GENERATED_RESOURCE_IDENTIFIED_OBJECT_DOOR_H_ */