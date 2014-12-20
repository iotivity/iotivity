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
#include "base_resource/base_link.h"
#include "data_types/simple_array_data_type.h"
#include "data_types/simple_data_type.h"
#include "http_header/http_request_header.h"
#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"
#include "http_header.h"
#include "base/logging.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kHrefKey = "href";

    BaseLink::BaseLink(const std::string &uri)
    {
        href_ = new _anyURI(uri);
        href_->SetProperty(1, 1, false);
        elements_["href"] = href_;
    }

    BaseLink::~BaseLink()
    {
        SAFE_DESTROY(this->href_)
    }

    HTTPResponseType BaseLink::ProcessHTTPRequestMethod(const HTTPRequestLine &request_line,
            const HTTPRequestHeader &request_header, base::Value *request_body,
            HTTPResponseHeader &response_header, base::Value *response_body,
            std::map<std::string, std::string> &id_deps)
    {
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

    HTTPResponseType BaseLink::Get(const HTTPRequestLine &request_line,
                                   const HTTPRequestHeader &request_header,
                                   HTTPResponseHeader &response_header, base::Value *response_body,
                                   std::map<std::string, std::string> &id_deps)
    {
        HTTPResponseType en_response_type = RT_200_OK;
        base::DictionaryValue *response_dict = static_cast<base::DictionaryValue *>(response_body);
        std::map<std::string, BaseAttributeType *>::iterator it;

        DLOG(INFO) << "BaseLink Get Method (element size : " << elements_.size() << ")";
        for (it = elements_.begin(); it != elements_.end(); it++)
        {
            std::string name = it->first;
            BaseAttributeType *attr = it->second;
            DLOG(INFO) << "Run String Type in Get method for Link";
            if (attr->HasDefaultValue())
            {
                std::string *default_value = reinterpret_cast<std::string *>(attr->GetDefault());
                if (default_value != NULL)
                {
                    std::string link_str = "/" + *default_value;
                    response_dict->SetWithoutPathExpansion(name, base::Value::CreateStringValue(link_str));
                }
                else
                {
                    if (attr->IsMandatoryAttribute())
                    {
                        DLOG(ERROR) << "Manatory Attribute : " << name << ", but Default Value is NULL";
                        en_response_type = RT_404_MANDATORY_ATTRIBUTE;
                    }
                }
            }
            else
            {
                if (attr->IsMandatoryAttribute())
                {
                    DLOG(ERROR) << "Manatory Attribute : " << name;
                    en_response_type = RT_404_MANDATORY_ATTRIBUTE;
                }
            }
        }
        return en_response_type;
    }

    HTTPResponseType BaseLink::Put(const HTTPRequestLine &request_line,
                                   const HTTPRequestHeader &request_header, base::Value *request_body,
                                   HTTPResponseHeader &response_header, base::Value *response_body,
                                   std::map<std::string, std::string> &id_deps)
    {
        HTTPResponseType en_response_type = RT_204_NO_CONTENT;
        return en_response_type;
    }

    HTTPResponseType BaseLink::Post(const HTTPRequestLine &request_line,
                                    const HTTPRequestHeader &request_header, base::Value *request_body,
                                    HTTPResponseHeader &response_header, base::Value *response_body,
                                    std::map<std::string, std::string> &id_deps)
    {
        HTTPResponseType en_response_type = RT_405_SHP;
        return en_response_type;
    }

    HTTPResponseType BaseLink::Delete(const HTTPRequestLine &request_line,
                                      const HTTPRequestHeader &request_header,
                                      HTTPResponseHeader &response_header, base::Value *response_body,
                                      std::map<std::string, std::string> &id_deps)
    {
        return RT_405_METHOD_NOT_ALLOWED;
    }

}
