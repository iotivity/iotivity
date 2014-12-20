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

#ifndef WEBSERVICE_WEB_CACHE_WRAPPER_FUNCTIONS_H_
#define WEBSERVICE_WEB_CACHE_WRAPPER_FUNCTIONS_H_

#include <string>

#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"


namespace webservice
{

    class WebCacheWrapperFunctions
    {
        public:
            /**
               * Process web cach
               * @param[in] request_uri : request uri which is defined to string
               * @param[in] etag
               * @param[in] p_is_etag_present
               * @param[in] p_is_uri_present_in_cache
               * @return bool
               */
            static bool ProcessWebCache(const HTTPRequestLine &request_line, const std::string &etag,
                                        bool *p_is_etag_present, bool *p_is_uri_present_in_cache);
            /**
               * This is to update cache
               * @param[in] request_uri : request uri which is defined to string
               * @param[in] request_method : request method for checking "Get" method, web cache is valid only for "Get" method.
               * @param[in] is_etag_present
               * @param[in] is_uri_present_in_cache
               * @param[in] response_code : http status code
               * @param[out] response_header : to set new ETag
               * @return bool
               */
            static void UpdateCache(const std::string &request_uri, const int request_method,
                                    bool is_etag_present,
                                    bool is_uri_present_in_cache, HTTPResponseType response_code, HTTPResponseHeader &response_header);

        private:
            /**
              * This is to check "ETag" existence in request information.
              * @param[in] request_uri : request uri which is defined to string
              * @param[in] etag : etag string
              * @param[in] is_uri_present_in_cache
              * @return bool
              */
            static bool ValidateETagField(const std::string &request_uri, const std::string &etag,
                                          bool *is_uri_present_in_cache);
    };
}

#endif /* WEBSERVICE_WEB_CACHE_WRAPPER_FUNCTIONS_H_ */

