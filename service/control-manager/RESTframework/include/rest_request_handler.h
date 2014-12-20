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

#ifndef REST_REQUEST_HANDLER_H_
#define REST_REQUEST_HANDLER_H_

#include <map>
#include <string>
#include <vector>

#include "base/memory/singleton.h"
#include "http_header.h"
#include "rest_engine_export.h"

namespace webservice
{

    class REST_ENGINE_EXPORT RestRequestHandler
    {
        public:
            /**
               * Returns pointer to RestRequestHandler singleton object
               * @param void
               * @return RestRequestHandler*
               */
            static webservice::RestRequestHandler *GetInstance(void);

            /**
               * Initializes the rest framework module
               * @param void
               * @return bool
               */
            bool Init(void);

            /**
               * Un-initializes the rest framework module
               * @param void
               * @return bool
               */
            bool DeInit(void);

            /**
               * Deletes RestRequestHandler singleton object
               * @param void
               * @return void
               */
            void DestroyInstance(void);

            /**
               * Processes the URI request and updates o/p parameter
               *                   (response_code, content_type) with result
               * @param[in] req_type : http request type
               * @param[in] req_uri : request uri
               * @param[in] query_parameters : http query params
               * @param[in] req_body : json request body
               * @param[in] header_request_info : extra http header information
               * @param response_code : http response code
               * @param header_response_info : http response headers from SLEP
               * @return string
               */
            web_util::RestResponse ProcessRequest(int req_type, std::string req_uri,
                                                  std::string query_parameters, std::string req_body,
                                                  const web_util::HttpHeaderData &http_request_info, int *response_code,
                                                  web_util::HttpHeaderData *http_response_info, std::string *response);
        private:
            // friend member, it creates RestRequestHandler object
            friend struct DefaultSingletonTraits<webservice::RestRequestHandler>;
    };
}

#endif /* REST_REQUEST_HANDLER_H_ */
