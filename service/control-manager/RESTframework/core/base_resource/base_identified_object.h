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

#ifndef WEBSERVICE_BASE_IDENTIFIED_OBJECT_H_
#define WEBSERVICE_BASE_IDENTIFIED_OBJECT_H_

#include <map>

#include "abstract_resource/creator_identified_object.h"
#include "base_resource/interface_http_method.h"
#include "wrapped_component/wrapper_input_type.h"
#include "data_types/simple_data_type.h"
#include "data_types/simple_array_data_type.h"
#include "http_header/http_request_header.h"
#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"
#include "base/values.h"

namespace webservice
{

    class ProcessAttribute;
    class BaseIdentifiedObject : public IHTTPMethod
    {
        public:
            BaseIdentifiedObject(std::string id, std::string object_name);
            virtual ~BaseIdentifiedObject(void);
            virtual HTTPResponseType ProcessHTTPRequestMethod(const HTTPRequestLine &request_line,
                    const HTTPRequestHeader &request_header, base::Value *request_body,
                    HTTPResponseHeader &response_header, base::Value *response_body,
                    std::map<std::string, std::string> &id_deps);
            virtual HTTPResponseType ValidateObjectID(void) = 0;
            /**
              * Create ID
              * @param[in] void*
              * @param[out] string
              * @return static HTTPResponseType
              */
            virtual HTTPResponseType CreateObjectID(void *p_in, std::string *id) = 0;
            /**
              * Delete ID
              * @param[in] void*
              * @param[out] string
              * @return static HTTPResponseType
              */
            virtual HTTPResponseType DeleteObjectID(void *p_in, const std::string &id) = 0;
            virtual void SaveCurrentIDWithName(std::map<std::string, std::string> &id_deps) = 0;

        private:
            virtual HTTPResponseType Get(const HTTPRequestLine &request_line,
                                         const HTTPRequestHeader &request_header, HTTPResponseHeader &response_header,
                                         base::Value *response_body, std::map<std::string, std::string> &id_deps);
            virtual HTTPResponseType Put(const HTTPRequestLine &request_line,
                                         const HTTPRequestHeader &request_header, base::Value *request_body,
                                         HTTPResponseHeader &response_header, base::Value *response_body,
                                         std::map<std::string, std::string> &id_deps);
            virtual HTTPResponseType Post(const HTTPRequestLine &request_line,
                                          const HTTPRequestHeader &request_header, base::Value *request_body,
                                          HTTPResponseHeader &response_header, base::Value *response_body,
                                          std::map<std::string, std::string> &id_deps);
            virtual HTTPResponseType Delete(const HTTPRequestLine &request_line,
                                            const HTTPRequestHeader &request_header, HTTPResponseHeader &response_header,
                                            base::Value *response_body, std::map<std::string, std::string> &id_deps);

            virtual void SetIDList(std::map<std::string, std::string> &id_deps,
                                   base::DictionaryValue *dict_value, bool is_clear);

        protected:
            std::map<std::string, BaseAttributeType *> elements_;
            String50 *id_;
            String50 *description_;
            HTTPMessage *http_;
            MethodGetFunc pGetFunc_;
            MethodPutFunc pPutFunc_;
            MethodPostFunc pPostFunc_;
            MethodDeleteFunc pDeleteFunc_;

        private:
            ProcessAttribute *p_attr;
    };

    class RegisterIdentifiedObject
    {
        public:
            RegisterIdentifiedObject(std::string uri_name, BaseIdentifiedObject * (*pCreateFunc)(std::string))
            {
                CreatorIdentifiedObject &creator = CreatorIdentifiedObject::GetInstance();
                creator.RegisterObject(uri_name, pCreateFunc);
            }
    };

}

#endif /* WEBSERVICE_BASE_IDENTIFIED_OBJECT_H_ */
