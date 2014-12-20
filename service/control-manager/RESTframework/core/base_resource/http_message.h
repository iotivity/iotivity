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

#ifndef WEBSERVICE_HTTP_MESSAGE_H_
#define WEBSERVICE_HTTP_MESSAGE_H_

#include "base/values.h"
#include "http_header/http_request_header.h"
#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"

namespace webservice
{
// Utility data structures
// Data structure which will be used to pass HTTP request information to Set/Get wrappers
    typedef struct HttpRequest
    {
        HTTPRequestLine *line;
        HTTPRequestHeader *headers;
        base::Value *body;
        HttpRequest(const HttpRequest &other)
        {
            line = other.line;
            headers = other.headers;
            body = other.body;
        }
        HttpRequest(HTTPRequestLine *p_line, HTTPRequestHeader *p_headers, base::Value *p_body)
        {
            line = p_line;
            headers = p_headers;
            body = p_body;
        }
        ~HttpRequest()
        {
            // all pointer hold by this data structure ar not owned by this so do not delete them
            line = NULL;
            headers = NULL;
            body = NULL;
        }
    } HttpRequest;
// Data structure which will be used to pass HTTP response to Set/Get wrappers
    typedef struct HttpResponse
    {
        HTTPResponseHeader *headers;
        base::Value *body;
        HTTPResponseType status;
        HttpResponse(const HttpResponse &other)
        {
            headers = other.headers;
            body = other.body;
            status = other.status;
        }
        HttpResponse(HTTPResponseHeader *p_headers, base::Value *p_body,
                     HTTPResponseType in_status = RT_UNKNOWN)
        {
            headers = p_headers;
            body = p_body;
            status = RT_UNKNOWN;
        }
        ~HttpResponse()
        {
            // all pointer hold by this data structure ar not owned by this so do not delete them
            headers = NULL;
            body = NULL;
            status = RT_UNKNOWN;
        }
    } HttpResponse;
// Data structure to hold HTTP request and response
    typedef struct HTTP
    {
        HttpRequest request;
        HttpResponse response;
        HTTP(HttpRequest req, HttpResponse res)
            : request(req),
              response(res)
        {
            // nothing
        }
        ~HTTP()
        {
            // nothing
        }
    } HTTPMessage;
}
#endif  //  WEBSERVICE_HTTP_MESSAGE_H_

