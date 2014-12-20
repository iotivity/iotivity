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

#ifndef WEBSERVICE_INTERFACE_HTTP_STATUS_CODE_H_
#define WEBSERVICE_INTERFACE_HTTP_STATUS_CODE_H_

#include "base/values.h"
#include "http_start_line/http_status_code_defined.h"

namespace webservice
{

    class IHTTPStatusCode
    {
        public:
            /**
              * Initialize HTTP status code data.
              * @param[in] HTTPResponseType : response type for http status code
              * @return void
              */
            virtual void InitHTTPStatusCode(HTTPResponseType response_error_code) = 0;

            /**
              * Generate HTTP status code from http response type.
              * @return int
              */
            virtual int GenerateHTTPStatusCode(void) = 0;

            /**
              * Make error code and description for HTTP response.
              * @param[out] HTTPResponseType
              * @return bool
              */
            virtual bool MakeHTTPStatusCodeAndDescription(base::DictionaryValue **in_response_body) = 0;

            /**
              * Check whether status code is for informational status or not
              * @return bool
              */
            virtual bool IsInformationalStatus(void) = 0;

            /**
              * Check whether status code is for successful status or not
              * @return bool
              */
            virtual bool IsSuccessfulStatus(void) = 0;

            /**
              * Check whether status code is for redirection status or not
              * @return bool
              */
            virtual bool IsRedirectionStatus(void) = 0;

            /**
              * Check whether status code is for client error status or not
              * @return bool
              */
            virtual bool IsClientErrorStatus(void) = 0;

            /**
              * Check whether status code is for server error status or not
              * @return bool
              */
            virtual bool IsServerErrorStatus(void) = 0;
    };

}

#endif /* WEBSERVICE_INTERFACE_HTTP_STATUS_CODE_H_ */

