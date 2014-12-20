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

#ifndef WEBSERVICE_BASE_LINK_H_
#define WEBSERVICE_BASE_LINK_H_

#include <map>

#include "abstract_resource/creator_link.h"
#include "base/values.h"
#include "base_resource/interface_http_method.h"
#include "data_types/simple_array_data_type.h"
#include "data_types/simple_data_type.h"
#include "http_header/http_request_header.h"
#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"

namespace webservice
{

    class BaseLink : public IHTTPMethod
    {
        public:
            BaseLink(const std::string &uri);
            virtual ~BaseLink(void);
            virtual HTTPResponseType ProcessHTTPRequestMethod(const HTTPRequestLine &request_line,
                    const HTTPRequestHeader &request_header, base::Value *request_body,
                    HTTPResponseHeader &response_header, base::Value *response_body,
                    std::map<std::string, std::string> &id_deps);

        private:
            virtual HTTPResponseType Get(const HTTPRequestLine &request_line,
                                         const HTTPRequestHeader &request_header, HTTPResponseHeader &response_header,
                                         base::Value *response_body, std::map<std::string, std::string> &id_deps);
            virtual HTTPResponseType Put(const HTTPRequestLine &request_line,
                                         const HTTPRequestHeader &request_header, base::Value *request_body,
                                         HTTPResponseHeader &response_header, base::Value *response_body,
                                         std::map<std::string, std::string> &id_deps);
            virtual HTTPResponseType Post(const HTTPRequestLine &request_line,
                                          const HTTPRequestHeader &request_header, base::Value *request_body,
                                          HTTPResponseHeader &response_header, base::Value *response_body,
                                          std::map<std::string, std::string> &id_deps);
            virtual HTTPResponseType Delete(const HTTPRequestLine &request_line,
                                            const HTTPRequestHeader &request_header, HTTPResponseHeader &response_header,
                                            base::Value *response_body, std::map<std::string, std::string> &id_deps);

        protected:
            std::map<std::string, BaseAttributeType *> elements_;
            _anyURI *href_;
    };

    class RegisterLink
    {
        public:
            RegisterLink(std::string uri_name, BaseLink * (*pCreateFunc)(const std::string &))
            {
                CreatorLink &creator = CreatorLink::GetInstance();
                creator.RegisterObject( uri_name, pCreateFunc );
            }
    };

}

#endif /* WEBSERVICE_BASE_LINK_H_ */
