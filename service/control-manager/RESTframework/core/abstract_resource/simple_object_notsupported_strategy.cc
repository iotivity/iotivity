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

#include <map>
#include <string>

#include "abstract_resource/simple_object_notsupported_strategy.h"
#include "base/values.h"
#include "base_resource/base_simple_object.h"
#include "http_header/http_request_header.h"
#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"
#include "base/logging.h"

namespace webservice
{

    /**
      * Find child object and process child object
      * @param[in] request_line : http request line
      * @param[in] request_header : http request header
      * @param[in] request_body : request body as value type
      * @param[out] response_header : http response header
      * @param[out] response_body : response body as value type
      * @param[in] id_deps : save all id of identified object instance
      * @param[in] current_uri : this mean current URI's depth
      * @param[out] p_simple_obj
      * @return HTTPResponseType
      */
    HTTPResponseType SimpleObjectNotSupportedStrategy::FindAndProcessChildObject(
        const HTTPRequestLine &request_line, const HTTPRequestHeader &request_header,
        base::Value *request_body,
        HTTPResponseHeader &response_header, base::Value *response_body,
        std::map<std::string, std::string> &id_deps, const std::string &current_uri,
        BaseSimpleObject *p_simple_obj)
    {
        HTTPResponseType response_error_code = RT_501_CHILD_OBJECT;
        return response_error_code;
    }

}
