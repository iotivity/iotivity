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

#include "abstract_resource/simple_object_delete_child_strategy.h"
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
    HTTPResponseType SimpleObjectDeleteChildStrategy::FindAndProcessChildObject(
        const HTTPRequestLine &request_line, const HTTPRequestHeader &request_header,
        base::Value *request_body,
        HTTPResponseHeader &response_header, base::Value *response_body,
        std::map<std::string, std::string> &id_deps, const std::string &current_uri,
        BaseSimpleObject *p_simple_obj)
    {
        DLOG(INFO) << "Delete Child Strategy for Simple Object";

        if (NULL == p_simple_obj)
        {
            LOG(ERROR) << "Input instance is NULL!";
            return RT_500_ALLOC_FAIL;
        }

        int method = request_line.GetHTTPMethod();
        if (web_util::HTTP_METHOD_DELETE != method)
        {
            LOG(FATAL) << "This is only for PUT method!";
            return RT_501_NOT_IMPLEMENTED;
        }
        return FindAndProcessChildObjectForIdentifiedObject(request_line, request_header, request_body,
                response_header, response_body, id_deps, current_uri, p_simple_obj);
    }

    /**
      * Find child object & Process child object for identified object
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
    HTTPResponseType SimpleObjectDeleteChildStrategy::FindAndProcessChildObjectForIdentifiedObject(
        const HTTPRequestLine &request_line, const HTTPRequestHeader &request_header,
        base::Value *request_body,
        HTTPResponseHeader &response_header, base::Value *response_body,
        std::map<std::string, std::string> &id_deps, const std::string &current_uri,
        BaseSimpleObject *p_simple_obj)
    {
        DLOG(INFO) << "No Child Strategy for Identified Object";
        if (NULL == p_simple_obj)
        {
            LOG(ERROR) << "Input instance is NULL!";
            return RT_500_ALLOC_FAIL;
        }

        HTTPResponseType response_error_code = RT_200_OK;
        // Change format for http request
        HttpRequest req(const_cast<HTTPRequestLine *> (&request_line),
                        const_cast<HTTPRequestHeader *> (&request_header), request_body);
        // Change format for http response
        HttpResponse res(const_cast<HTTPResponseHeader *> (&response_header), response_body);
        // Merge http request and response
        HTTPMessage http(req, res);
        std::map<std::string, BaseSimpleObject *>::const_iterator it;
        const std::map<std::string, BaseSimpleObject *> &child_map = p_simple_obj->GetChildMap();
        const int http_method = request_line.GetHTTPMethod();

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
                // Get IDs
                std::vector<std::string> id_list;
                if (0 != p_simple_obj->GetObjectIDList(child_object_name, &id_list, &http))
                {
                    std::vector<std::string>::iterator id_list_iter;
                    for (id_list_iter = id_list.begin(); id_list_iter != id_list.end(); id_list_iter++)
                    {
                        base::Value *request_child = NULL;
                        if (p_child_obj->ParseChildRequestBody(http_method, request_body, &request_child, *id_list_iter))
                        {
                            std::string found_uri_add_child = current_uri + "/" + *id_list_iter;
                            response_error_code = p_child_obj->ProcessRequest(request_line, request_header, request_child,
                                                  response_header, response_body, id_deps, found_uri_add_child);
                            DLOG(ERROR) << "found_resource_uri : " << found_uri_add_child << ", error_code : " <<
                                        response_error_code;
                        }
                    }
                }
            }
            if (RT_200_START > response_error_code
                || RT_300_START <= response_error_code) return response_error_code;
        }
        return response_error_code;
    }

}
