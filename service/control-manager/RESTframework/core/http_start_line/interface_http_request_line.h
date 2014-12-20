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

#ifndef WEBSERVICE_INTERFACE_HTTP_REQUEST_LINE_H_
#define WEBSERVICE_INTERFACE_HTTP_REQUEST_LINE_H_

#include <map>
#include <string>
#include <vector>

namespace webservice
{

    class IHTTPRequestLine
    {
        public:
            /**
              * Initialize HTTP request line data.
              * @param[in] int : request type (HTTP Method)
              * @param[in] std::string : URI
              * @param[in] std::string : query parameter
              * @return void
              */
            virtual void InitHTTPRequestLine(int type, std::string uri, std::string query_parameter) = 0;

            /**
              * Get URI to vector type.
              * @return std::vector<std::string>
              */
            virtual const std::vector<std::string> &GetUriFields(void) const = 0;

            /**
              * Get URI to string type.
              * @return std::string&
              */
            virtual const std::string &GetOriginUri(void) const = 0;

            /**
              * Get query content to map type.
              * @return std::map<std::string, std::string>
              */
            virtual const std::map<std::string, std::string> &GetQueryContent(void) const = 0;

            /**
              * Get http method
              * @return int
              */
            virtual const int GetHTTPMethod(void) const = 0;
    };

}

#endif /* WEBSERVICE_INTERFACE_HTTP_REQUEST_LINE_H_ */

