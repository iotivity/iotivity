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

#ifndef WEBSERVICE_INTERFACE_HTTP_BODY_GENERATOR_H_
#define WEBSERVICE_INTERFACE_HTTP_BODY_GENERATOR_H_

#include <string>

#include "base/values.h"
#include "http_body/creator_http_body_generator.h"
#include "http_header/http_header_defined.h"

namespace webservice
{

    class IHTTPBodyGenerator
    {
        public:
            /**
              * Generate dictionary value to json format.
              * @param[in] base::DictionaryValue*
              * @return std::string
              */
            virtual std::string Generate(const base::DictionaryValue *in_response_dict) = 0;

            /**
              * Generate dictionary value to json format for head of http response body.
              * @param[in] base::DictionaryValue*
              * @return std::string
              */
            virtual std::string GenerateHead(const base::DictionaryValue *in_response_dict) = 0;

            /**
              * Generate dictionary value to json format for body of http response body.
              * @param[in] base::DictionaryValue*
              * @return std::string
              */
            virtual std::string GenerateBody(const base::DictionaryValue *in_response_dict) = 0;

            /**
              * Generate dictionary value to json format for tail of http response body.
              * @param[in] base::DictionaryValue*
              * @return std::string
              */
            virtual std::string GenerateTail(const base::DictionaryValue *in_response_dict) = 0;

            /**
              * Get content type for this class,
              * This class is for Json format.
              * @return HttpContentType
              */
            virtual const HttpContentType GetCurrentContentType(void) = 0;
    };

    class RegisterHTTPBodyGenerator
    {
        public:
            RegisterHTTPBodyGenerator(HttpContentType content_type, IHTTPBodyGenerator * (*ptr_func_creator)())
            {
                CreatorHTTPBodyGenerator &creator = CreatorHTTPBodyGenerator::GetInstance();
                creator.RegisterObject(content_type, ptr_func_creator);
            }
    };
}

#endif /* WEBSERVICE_INTERFACE_HTTP_BODY_GENERATOR_H_ */

