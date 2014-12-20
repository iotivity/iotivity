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

#ifndef WEBSERVICE_HTTP_BODY_JSON_GENERATOR_H_
#define WEBSERVICE_HTTP_BODY_JSON_GENERATOR_H_

#include "base/values.h"
#include "http_body/interface_http_body_generator.h"
#include "http_header/http_header_defined.h"

namespace webservice
{

    class HTTPBodyJsonGenerator : public IHTTPBodyGenerator
    {
        public:
            /* NOT used for this class */
            virtual ~HTTPBodyJsonGenerator(void) {}

            /**
              * Create this class.
              * @return IHTTPBodyGenerator*
              */
            static IHTTPBodyGenerator *Create(void);

            /**
              * Get content type for this class,
              * This class is generator for Json format.
              * So, The return is always "web_util::kHttpContentTypeApplicationJSON"
              * @return HttpContentType
              */
            virtual const HttpContentType GetCurrentContentType(void);

            /**
              * Generate dictionary value to json format.
              * @param[in] base::DictionaryValue*
              * @return std::string
              */
            virtual std::string Generate(const base::DictionaryValue *in_response_dict);

        private:
            /* NOT used for this class */
            virtual std::string GenerateHead(const base::DictionaryValue *in_response_dict) { return NULL; }

            /* NOT used for this class */
            virtual std::string GenerateBody(const base::DictionaryValue *in_response_dict) { return NULL; }

            /* NOT used for this class */
            virtual std::string GenerateTail(const base::DictionaryValue *in_response_dict) { return NULL; }

        public:
            /**
              * Register function to create this class.
              * @param[in] HttpContentType
              * @param[in] IHTTPBodyGenerator*
              * @return static RegisterHTTPBodyGenerator
              */
            static RegisterHTTPBodyGenerator regJsonParser;
    };
}

#endif /* WEBSERVICE_HTTP_BODY_JSON_GENERATOR_H_ */
