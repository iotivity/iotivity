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

#ifndef WEBSERVICE_HTTP_BODY_JSON_PARSER_H_
#define WEBSERVICE_HTTP_BODY_JSON_PARSER_H_

#include "base/values.h"
#include "http_body/interface_http_body_parser.h"
#include "http_header/http_header_defined.h"

namespace webservice
{

    class HTTPBodyJsonParser : public IHTTPBodyParser
    {
        public:
            /* NOT used for this class */
            virtual ~HTTPBodyJsonParser(void) {}

            /**
              * Create this class.
              * @return IHTTPBodyParser*
              */
            static IHTTPBodyParser *Create(void);

            /**
              * Get content type for this class,
              * This class is parser for Json format.
              * So, The return is always "kHttpContentTypeApplicationJSON"
              * @return HttpContentType
              */
            const HttpContentType GetCurrentContentType(void);

            /**
              * Parser string type(json format) to dictionary value.
              * @param[in] std::string&
              * @param[out] base::DictionaryValue**
              * @return bool
              */
            virtual bool ParseRequestBody(const std::string &in_request_body,
                                          base::DictionaryValue **out_request_body);

        private:
            /* NOT used for this class */
            virtual bool ParseHead(const std::string &in_request_body,
                                   base::DictionaryValue *out_request_body) { return false; }

            /* NOT used for this class */
            virtual bool ParseBody(const std::string &in_request_body,
                                   base::DictionaryValue *out_request_body) { return false; }

            /* NOT used for this class */
            virtual bool ParseTail(const std::string &in_request_body,
                                   base::DictionaryValue *out_request_body) { return false; }

        public:
            /**
              * Register function to create this class.
              * @param[in] HttpContentType
              * @param[in] IHTTPBodyParser*
              * @return static RegisterHTTPBodyParser
              */
            static RegisterHTTPBodyParser regJsonParser;
    };
}

#endif /* WEBSERVICE_HTTP_BODY_JSON_PARSER_H_ */

