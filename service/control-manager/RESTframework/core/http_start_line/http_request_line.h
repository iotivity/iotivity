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

#ifndef WEBSERVICE_HTTP_REQUEST_LINE_H_
#define WEBSERVICE_HTTP_REQUEST_LINE_H_

#include <map>
#include <string>
#include <vector>

#include "rest_engine_export.h"

namespace webservice
{

    class IHTTPRequestLine; /* forward declaration */
    class REST_ENGINE_EXPORT HTTPRequestLine
    {
        public:
            /**
              * Class Constructor
              * @param[in] int : Method
              * @param[in] string : URI
              * @param[in] string : query parameter
              */
            HTTPRequestLine(int type, std::string uri, std::string query_parameter);

            /**
              * Class Destructor
              */
            virtual ~HTTPRequestLine(void);

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
              * Get filter value to vector type.
              * @param[out] list_query_key
              * @return bool
              */
            bool GetFilterKeyString(std::vector<std::string> *list_query_key) const;

            /**
              * Get http method
              * @return int
              */
            const int GetHTTPMethod(void) const;

        private:
            IHTTPRequestLine *pHttpRequestLineImpl_;
    };

}

#endif /* WEBSERVICE_HTTP_REQUEST_LINE_H_ */

