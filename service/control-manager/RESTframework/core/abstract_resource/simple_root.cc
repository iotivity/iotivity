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

#include "abstract_resource/simple_root.h"
#include "base/values.h"
#include "base_resource/interface_http_method.h"
#include "http_header/http_request_header.h"
#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"

namespace webservice
{

    /**
      * Get the instance of this object
      * @return SimpleRoot&
      */
    SimpleRoot &SimpleRoot::GetInstance()
    {
        static SimpleRoot instance("Root", "root");
        return instance;
    }

    /**
      * Create the object which is found from the client's request URI
      * @return IHTTPMethod* : the object instance
      */
    IHTTPMethod *SimpleRoot::CreateObject(std::string option = "")
    {
        return NULL;
    }

    /**
      * Get simple object type
      * This function is used to classify each simple object
      * @return SimpleObjectType
      */
    const SimpleObjectType SimpleRoot::GetSimpleObjectType()
    {
        return enSimpleUnknownType;
    }

    /**
      * Connect response body to parent body
      * This class is for resource object, so its parent class is always dictionary value type.
      * @param[in/out] p_value : parent response body instance
      * @param[in] p_value_child : child response body instance
      * @param[in] response_error_code
      * @param[in] is_force : if this is true, uncoditionally set child response body
      * @return void
      */
    void SimpleRoot::SetChildResponseBody(base::Value *p_value, base::Value *p_value_child,
                                          HTTPResponseType response_error_code, bool is_force)
    {
    }

    /**
      * Main process of simple object
      * Create found object and search its child object
      * @param[in] request_line : http request line
      * @param[in] request_header : http request header
      * @param[in] request_body : request body as value type
      * @param[out] response_header : http response header
      * @param[out] response_body : response body as value type
      * @param[in] id_deps : save all id of identified object instance
      * @param[in] current_uri : this mean current URI's depth
      * @return HTTPResponseType
      */
    HTTPResponseType SimpleRoot::ProcessRequest(const HTTPRequestLine &request_line,
            const HTTPRequestHeader &request_header, base::Value *request_body,
            HTTPResponseHeader &response_header, base::Value *response_body,
            std::map<std::string, std::string> &id_deps, const std::string &found_uri)
    {
        return RT_UNKNOWN;
    }

}
