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

#ifndef WEB_UTIL_HTTP_HEADER_H_
#define WEB_UTIL_HTTP_HEADER_H_

#include <string>

#include "http_status_code.h"

namespace web_util
{

    typedef enum
    {
        kRestFwUnknownError = -1,
        kRestFwProcessRequstFailed = 0,
        kRestFwUnInitializedError,
        kRestFwProcessRequstSuccess,
    } RestResponse;

    typedef enum
    {
        kRestFwUnknownState = -1,
        kRestFwInitialized,
        kRestFwUnInitialized,
    } RestFwState;

    typedef struct HttpHeaderData
    {
        enum {NUM_HEADER_PARAMS = 20};
        // (TODO) The following arrays can be replaced with list/vector in future
        std::string param_name[NUM_HEADER_PARAMS];
        std::string param_value[NUM_HEADER_PARAMS];
        int flag_outheader_overwrite[NUM_HEADER_PARAMS];
        int is_this_set[NUM_HEADER_PARAMS];
        HttpHeaderData()
            : param_name(),
              param_value(),
              flag_outheader_overwrite(),
              is_this_set()
        {
            // nothing
        }
        ~HttpHeaderData()
        {
            // If a string is not cleared it may lead to 'still reachable' memory leaks
            // so clearing string members of 'HttpHeaderData' data structure
            for (int iter = 0; iter < NUM_HEADER_PARAMS; iter++)
            {
                param_name[iter].clear();
                param_value[iter].clear();
            }
        }
    } HttpHeaderData;

    typedef struct HTTPReq
    {
        int req_type;
        std::string req_uri;
        std::string req_body;
        std::string query_parameters;
        HttpHeaderData *http_request_info;
    } HTTPReq;

    typedef struct
    {
        HttpStatusCode status;
        std::string response_body;
        HttpHeaderData headers;
    } HTTPResponse;

    typedef struct HttpAPIVersion
    {
        unsigned int majorVersion;
        unsigned int minorVersion;
        unsigned int releaseVersion;
    } HttpAPIVersion;

    typedef enum
    {
        HTTP_METHOD_UNSET = -1,
        HTTP_METHOD_GET,
        HTTP_METHOD_POST,
        HTTP_METHOD_HEAD,
        HTTP_METHOD_OPTIONS,
        HTTP_METHOD_PROPFIND,  /* WebDAV */
        HTTP_METHOD_MKCOL,
        HTTP_METHOD_PUT,
        HTTP_METHOD_DELETE,
        HTTP_METHOD_COPY,
        HTTP_METHOD_MOVE,
        HTTP_METHOD_PROPPATCH,
        HTTP_METHOD_REPORT, /* DeltaV */
        HTTP_METHOD_CHECKOUT,
        HTTP_METHOD_CHECKIN,
        HTTP_METHOD_VERSION_CONTROL,
        HTTP_METHOD_UNCHECKOUT,
        HTTP_METHOD_MKACTIVITY,
        HTTP_METHOD_MERGE,
        HTTP_METHOD_LOCK,
        HTTP_METHOD_UNLOCK,
        HTTP_METHOD_LABEL,
        HTTP_METHOD_CONNECT
    } HttpMethod;

};  // namespace web_util
#endif  // WEB_UTIL_HTTP_HEADER_H_
