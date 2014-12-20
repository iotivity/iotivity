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

#ifndef WEBSERVICE_HTTP_REQUEST_LINE_IMPL_H_
#define WEBSERVICE_HTTP_REQUEST_LINE_IMPL_H_

#include <map>
#include <string>
#include <vector>

#include "base/synchronization/lock.h"
#include "http_start_line/interface_http_request_line.h"

namespace webservice
{

    class HTTPRequestLineImpl : public IHTTPRequestLine
    {
        public:
            /**
              * Class Constructor
              */
            HTTPRequestLineImpl(void);

            /**
              * Class Destructor
              */
            virtual ~HTTPRequestLineImpl(void) {}

            /**
              * Initialize HTTP request line data.
              * @param[in] int : request type (HTTP Method)
              * @param[in] std::string : URI
              * @param[in] std::string : query parameter
              * @return void
              */
            void InitHTTPRequestLine(int type, std::string uri, std::string query_parameter);

            /**
              * Get URI to vector type.
              * @return std::vector<std::string>
              */
            const std::vector<std::string> &GetUriFields(void) const;

            /**
              * Get URI to string type.
              * @return std::string&
              */
            const std::string &GetOriginUri(void) const;

            /**
              * Get query content to map type.
              * @return std::map<std::string, std::string>
              */
            const std::map<std::string, std::string> &GetQueryContent(void) const;

            /**
              * Get http method
              * @return int
              */
            const int GetHTTPMethod(void) const;

        private:
            /**
              * Parse URI and Query paramter
              * @param[in] string : URI
              * @param[in] string : query paramter
              * @return bool
              */
            bool ParseUriAndQuery(const std::string &uri, const std::string &query);

            /**
              * Save query parameter to map
              * @param[in] string : query
              * @param[out] std::map<std::string, std::string>* : query paramter map
              * @return bool
              */
            bool NameValueMap(const std::string &query, std::map<std::string, std::string> *name_value_pair);

        private:
            /* Auto lock key for initialization */
            base::Lock init_lock_;

            /* HTTP Method type */
            int request_type_;

            /* HTTP Request URI */
            std::string request_uri_;

            /* HTTP Request query paramter */
            std::string query_parameter_;

            /* Parsed URI */
            std::vector<std::string> uri_fields_;

            /* Mapped query paramter */
            std::map<std::string, std::string> query_content_;
    };

}

#endif /* WEBSERVICE_HTTP_REQUEST_LINE_IMPL_H_ */

