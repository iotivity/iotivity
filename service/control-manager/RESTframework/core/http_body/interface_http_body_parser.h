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

#ifndef WEBSERVICE_INTERFACE_HTTP_BODY_PARSER_H_
#define WEBSERVICE_INTERFACE_HTTP_BODY_PARSER_H_

#include <string>

#include "base/values.h"
#include "http_body/creator_http_body_parser.h"
#include "http_header/http_header_defined.h"

namespace webservice
{

    class IHTTPBodyParser
    {
        public:
            /**
              * Parser string type to dictionary value.
              * @param[in] std::string&
              * @param[out] base::DictionaryValue**
              * @return bool
              */
            virtual bool ParseRequestBody(const std::string &in_request_body,
                                          base::DictionaryValue **out_request_body) = 0;

            /**
              * Parser string type to dictionary value for head of http request body.
              * @param[in] std::string&
              * @param[out] base::DictionaryValue*
              * @return bool
              */
            virtual bool ParseHead(const std::string &in_request_body,
                                   base::DictionaryValue *out_request_body) = 0;

            /**
              * Parser string type to dictionary value for body of http request body.
              * @param[in] std::string&
              * @param[out] base::DictionaryValue*
              * @return bool
              */
            virtual bool ParseBody(const std::string &in_request_body,
                                   base::DictionaryValue *out_request_body) = 0;

            /**
              * Parser string type to dictionary value for tail of http request body.
              * @param[in] std::string&
              * @param[out] base::DictionaryValue*
              * @return bool
              */
            virtual bool ParseTail(const std::string &in_request_body,
                                   base::DictionaryValue *out_request_body) = 0;

            /**
              * Get content type for this class,
              * This class is for Json format.
              * @return HttpContentType
              */
            virtual const HttpContentType GetCurrentContentType(void) = 0;
    };

    class RegisterHTTPBodyParser
    {
        public:
            RegisterHTTPBodyParser(HttpContentType content_type, IHTTPBodyParser * (*ptr_func_creator)())
            {
                CreatorHTTPBodyParser &creator = CreatorHTTPBodyParser::GetInstance();
                creator.RegisterObject(content_type, ptr_func_creator);
            }
    };

}

#endif /* WEBSERVICE_INTERFACE_HTTP_BODY_PARSER_H_ */
