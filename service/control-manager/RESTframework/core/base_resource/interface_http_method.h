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

#ifndef WEBSERVICE_INTERFACE_HTTP_METHOD_H_
#define WEBSERVICE_INTERFACE_HTTP_METHOD_H_

#include <map>

#include "base/values.h"
#include "http_header/http_request_header.h"
#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"

namespace webservice
{

    class IHTTPMethod
    {
        public:
            virtual ~IHTTPMethod(void) {}
            virtual HTTPResponseType ProcessHTTPRequestMethod(const HTTPRequestLine &request_line,
                    const HTTPRequestHeader &request_header, base::Value *request_body,
                    HTTPResponseHeader &response_header, base::Value *response_body,
                    std::map<std::string, std::string> &id_deps) = 0;

        protected:
            typedef HTTPResponseType (*MethodGetFunc)(void *, void *);
            typedef HTTPResponseType (*MethodPutFunc)(void *, void *);
            typedef HTTPResponseType (*MethodPostFunc)(void *, void *, std::string *);
            typedef HTTPResponseType (*MethodDeleteFunc)(void *, void *);

            virtual HTTPResponseType Get(const HTTPRequestLine &request_line,
                                         const HTTPRequestHeader &request_header, HTTPResponseHeader &response_header,
                                         base::Value *response_body, std::map<std::string, std::string> &id_deps) = 0;
            virtual HTTPResponseType Put(const HTTPRequestLine &request_line,
                                         const HTTPRequestHeader &request_header, base::Value *request_body,
                                         HTTPResponseHeader &response_header, base::Value *response_body,
                                         std::map<std::string, std::string> &id_deps) = 0;
            virtual HTTPResponseType Post(const HTTPRequestLine &request_line,
                                          const HTTPRequestHeader &request_header, base::Value *request_body,
                                          HTTPResponseHeader &response_header, base::Value *response_body,
                                          std::map<std::string, std::string> &id_deps) = 0;
            virtual HTTPResponseType Delete(const HTTPRequestLine &request_line,
                                            const HTTPRequestHeader &request_header, HTTPResponseHeader &response_header,
                                            base::Value *response_body, std::map<std::string, std::string> &id_deps) = 0;
    };

}

#endif /* WEBSERVICE_INTERFACE_HTTP_METHOD_H_ */