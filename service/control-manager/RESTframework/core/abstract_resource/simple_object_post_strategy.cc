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

#include "abstract_resource/simple_object_post_strategy.h"
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
      * Find child object & Process child object
      * @param[in] const HTTPRequestLine&
      * @param[in] const HTTPRequestHeader&
      * @param[in] base::Value*
      * @param[out] HTTPResponseHeader&
      * @param[out] base::Value*
      * @param[out] std::map<std::string, std::string>&
      * @param[in] const std::string&
      * @param[in] BaseSimpleObject*
      * @return HTTPResponseType
      */
    HTTPResponseType SimpleObjectPostStrategy::FindAndProcessChildObject(const HTTPRequestLine
            &request_line, const HTTPRequestHeader &request_header, base::Value *request_body,
            HTTPResponseHeader &response_header, base::Value *response_body,
            std::map<std::string, std::string> &id_deps, const std::string &current_uri,
            BaseSimpleObject *p_simple_obj)
    {
        if (NULL == p_simple_obj)
        {
            LOG(ERROR) << "Input instance is NULL!";
            return RT_500_ALLOC_FAIL;
        }

        HTTPResponseType response_error_code = RT_200_OK;
        const int http_method = request_line.GetHTTPMethod();
        if (web_util::HTTP_METHOD_POST != http_method)
        {
            LOG(ERROR) << "Invalid usage, this is only for POST method.";
            return RT_500_INTERNAL_SERVER_ERROR;
        }

        std::map<std::string, BaseSimpleObject *>::const_iterator it;
        const std::map<std::string, BaseSimpleObject *> &child_map = p_simple_obj->GetChildMap();

        // Find Child Object, and run child object.
        for (it = child_map.begin(); it != child_map.end(); it++)
        {
            std::string child_object_name = it->first;
            BaseSimpleObject *p_child_obj = it->second;
            if (NULL == p_child_obj)
            {
                LOG(ERROR) << "Simple child instance is NULL!";
                return RT_500_ALLOC_FAIL;
            }

            if (enSimpleIdentifiedObject == p_child_obj->GetSimpleObjectType())
            {
                base::Value *response_child = NULL;
                if (!p_child_obj->CreateChildResponseBody(&response_child)) return RT_500_ALLOC_FAIL;
                response_error_code = p_child_obj->ProcessRequest(request_line, request_header, request_body,
                                      response_header, response_child, id_deps, current_uri);
                p_child_obj->SetChildResponseBody(response_body, response_child, response_error_code, false);
                if (RT_200_START > response_error_code
                    || RT_300_START <= response_error_code) return response_error_code;
            }
        }
        return response_error_code;
    }

}
