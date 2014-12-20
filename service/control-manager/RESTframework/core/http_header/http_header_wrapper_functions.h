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

#ifndef WEBSERVICE_HTTP_HEADER_WRAPPER_FUNCTIONS_H_
#define WEBSERVICE_HTTP_HEADER_WRAPPER_FUNCTIONS_H_

#include <list>
#include <string>

#include "http_header/http_header_defined.h"
#include "rest_engine_export.h"

namespace webservice
{

    typedef struct _stAcceptValue
    {
        std::string content_type;
        int q_value;
    } stAcceptValue;

    class REST_ENGINE_EXPORT HttpHeaderWrapperFunctions
    {
        public:
            /**
               * Negotiate content-type from accept in http request header
               * @param[in] accept : accept in http request header
               * @return HttpContentType
               */
            static HttpContentType NegotiateContentType(const std::string &accept);

        private:
            /**
               * Change Content Type to HttpContentType(Enum) from string
               * @param[in] content_type_text : string type
               * @return HttpContentType
               */
            static HttpContentType ChangeContentTypeToEnum(const std::string &content_type_text);
            /**
               * Parse Accept from http request header
               * if content type is supported and q value is 1, immediately return the content type,
               * else supported content type is saved to list with q value, and return them.
               * @param[in] accept : accept in http request header
               * @param[out] accept_value_list : parsed accept list
               * @return bool
               */
            static bool ParseAcceptList(const std::string &accept, std::list<stAcceptValue> &accept_value_list);
            /**
               * Compare each content type from accept in http request header
               * @param[in] first_param
               * @param[in] second_param
               * @return bool
               */
            static bool ParamCompare (const stAcceptValue &first, const stAcceptValue &second);

    };
}

#endif /* WEBSERVICE_HTTP_HEADER_WRAPPER_FUNCTIONS_H_ */

