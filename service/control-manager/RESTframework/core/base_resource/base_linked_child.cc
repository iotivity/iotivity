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

#include "base/values.h"
#include "base_resource/base_linked_child.h"
#include "base_resource/process_attribute.h"
#include "data_types/simple_array_data_type.h"
#include "data_types/simple_data_type.h"
#include "http_header/http_request_header.h"
#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"
#include "base/logging.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    BaseLinkedChild::BaseLinkedChild()
    {
        p_attr = new ProcessAttribute;
        http_ = NULL;
        pGetFunc_ = NULL;
        pPutFunc_ = NULL;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;
    }

    BaseLinkedChild::~BaseLinkedChild()
    {
        SAFE_DESTROY(p_attr)
        delete http_;
    }

    HTTPResponseType BaseLinkedChild::ProcessHTTPRequestMethod(const HTTPRequestLine &request_line,
            const HTTPRequestHeader &request_header, base::Value *request_body,
            HTTPResponseHeader &response_header, base::Value *response_body,
            std::map<std::string, std::string> &id_deps)
    {
        HttpRequest req(const_cast<HTTPRequestLine *> (&request_line),
                        const_cast<HTTPRequestHeader *> (&request_header), request_body);
        HttpResponse res(const_cast<HTTPResponseHeader *> (&response_header), response_body);
        http_ = new HTTPMessage(req, res);
        HTTPResponseType response_error_code = RT_405_METHOD_NOT_ALLOWED;
        switch (request_line.GetHTTPMethod())
        {
            case web_util::HTTP_METHOD_GET:
                {
                    response_error_code = Get(request_line, request_header, response_header, response_body, id_deps);
                } break;
            case web_util::HTTP_METHOD_PUT:
                {
                    response_error_code = Put(request_line, request_header, request_body, response_header,
                                              response_body, id_deps);
                } break;
            case web_util::HTTP_METHOD_POST:
                {
                    response_error_code = Post(request_line, request_header, request_body, response_header,
                                               response_body, id_deps);
                } break;
            case web_util::HTTP_METHOD_DELETE:
                {
                    response_error_code = Delete(request_line, request_header, response_header, response_body, id_deps);
                } break;
            default:
                response_error_code = RT_405_METHOD_NOT_ALLOWED;
                break;
        }

        return response_error_code;
    }

    HTTPResponseType BaseLinkedChild::Get(const HTTPRequestLine &request_line,
                                          const HTTPRequestHeader &request_header,
                                          HTTPResponseHeader &response_header, base::Value *response_body,
                                          std::map<std::string, std::string> &id_deps)
    {
        HTTPResponseType en_response_type = RT_UNKNOWN;
        HTTPResponseType en_error_response_type = RT_200_OK;
        base::DictionaryValue *response_dict = static_cast<base::DictionaryValue *>(response_body);
        std::map<std::string, BaseAttributeType *>::iterator it;

        base::DictionaryValue input_dictionary_value;
        SetIDList(id_deps, &input_dictionary_value);
        WrapperInput input = WrapperInput(http_, &input_dictionary_value);

        base::DictionaryValue function_response;
        function_response.Clear();
        if (NULL != this->pGetFunc_)
        {
            (*(this->pGetFunc_))(reinterpret_cast<void *>(&input),
                                 reinterpret_cast<void *>(&function_response));
        }

        DLOG(INFO) << "BaseResource Get Method (element size : " << elements_.size() << ")";
        for (it = elements_.begin(); it != elements_.end(); it++)
        {
            std::string name = it->first;
            BaseAttributeType *attr = it->second;
            if (function_response.HasKey(name))
            {
                en_response_type = p_attr->GetAttributeWithFunctionResponse(name, attr, &function_response,
                                   response_dict);
            }
            else
            {
                en_response_type = p_attr->GetAttribute(name, attr, &input, response_dict);
            }
            if (RT_200_START > en_response_type
                || RT_300_START <= en_response_type) en_error_response_type = en_response_type;
        }
        return en_error_response_type;
    }

    HTTPResponseType BaseLinkedChild::Put(const HTTPRequestLine &request_line,
                                          const HTTPRequestHeader &request_header, base::Value *request_body,
                                          HTTPResponseHeader &response_header, base::Value *response_body,
                                          std::map<std::string, std::string> &id_deps)
    {
        HTTPResponseType en_response_type = RT_UNKNOWN;
        HTTPResponseType en_error_response_type = RT_204_NO_CONTENT;
        if (NULL == request_body) return en_error_response_type;
        base::DictionaryValue *request_body_dict = static_cast<base::DictionaryValue *>(request_body);
        base::DictionaryValue input_dictionary_value;
        SetIDList(id_deps, &input_dictionary_value);
        WrapperInput input = WrapperInput(http_, &input_dictionary_value);
        DLOG(INFO) << "BaseResource Put Method (element size : " << elements_.size() << ")";
        // Pre Check : unnecessary, data type, restriction
        for (base::DictionaryValue::key_iterator dict_key_it = request_body_dict->begin_keys();
             dict_key_it != request_body_dict->end_keys(); ++dict_key_it)
        {
            std::string input_key = *dict_key_it;
            en_response_type = p_attr->PutPreChecking(input_key, this->elements_, request_body_dict);
            if (RT_200_START > en_response_type || RT_300_START <= en_response_type) return en_response_type;
        }
        // Processing attributes
        std::map<std::string, BaseAttributeType *>::iterator it;
        for (it = elements_.begin(); it != elements_.end(); it++)
        {
            std::string name = it->first;
            BaseAttributeType *attr = it->second;
            if ("id" == name) continue;
            en_response_type = p_attr->PutAttribute(name, attr, &input, request_body_dict);
            if (RT_200_START > en_response_type
                || RT_300_START <= en_response_type) en_error_response_type = en_response_type;
        }
        return en_error_response_type;
    }

    HTTPResponseType BaseLinkedChild::Post(const HTTPRequestLine &request_line,
                                           const HTTPRequestHeader &request_header, base::Value *request_body,
                                           HTTPResponseHeader &response_header, base::Value *response_body,
                                           std::map<std::string, std::string> &id_deps)
    {
        HTTPResponseType en_response_type = RT_405_SHP;
        return en_response_type;
    }

    HTTPResponseType BaseLinkedChild::Delete(const HTTPRequestLine &request_line,
            const HTTPRequestHeader &request_header,
            HTTPResponseHeader &response_header, base::Value *response_body,
            std::map<std::string, std::string> &id_deps)
    {
        return RT_405_METHOD_NOT_ALLOWED;
    }

    void BaseLinkedChild::SetIDList(std::map<std::string, std::string> &id_deps,
                                    base::DictionaryValue *dict_value)
    {
        std::map<std::string, std::string>::iterator it;
        dict_value->Clear();
        for (it = id_deps.begin(); it != id_deps.end(); it++)
        {
            DLOG(INFO) << it->first << " " << it->second;
            dict_value->SetWithoutPathExpansion(it->first, base::Value::CreateStringValue(it->second));
        }
    }

}
