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

#ifndef WEBSERVICE_HTTP_STATUS_CODE_IMPL_H_
#define WEBSERVICE_HTTP_STATUS_CODE_IMPL_H_

#include <string>

#include "base/synchronization/lock.h"
#include "base/values.h"
#include "http_start_line/http_status_code_defined.h"
#include "http_start_line/interface_http_status_code.h"

namespace webservice
{

    class HTTPStatusCodeImpl : public IHTTPStatusCode
    {
        public:
            /**
              * Class Constructor
              */
            HTTPStatusCodeImpl(void);

            /**
              * Class Destructor
              */
            virtual ~HTTPStatusCodeImpl(void) {}

            /**
              * Initialize HTTP status code data.
              * @param[in] HTTPResponseType : response type for http status code
              * @return void
              */
            void InitHTTPStatusCode(HTTPResponseType response_error_code);

            /**
              * Generate HTTP status code from http response type.
              * @return int
              */
            int GenerateHTTPStatusCode(void);

            /**
              * Make error code and description for HTTP response.
              * @param[out] HTTPResponseType
              * @return bool
              */
            bool MakeHTTPStatusCodeAndDescription(base::DictionaryValue **in_response_body);

            /**
              * Check whether status code is for information status or not
              * @return bool
              */
            bool IsInformationalStatus(void);

            /**
              * Check whether status code is for successful status or not
              * @return bool
              */
            bool IsSuccessfulStatus(void);

            /**
              * Check whether status code is for redirection status or not
              * @return bool
              */
            bool IsRedirectionStatus(void);

            /**
              * Check whether status code is for client error status or not
              * @return bool
              */
            bool IsClientErrorStatus(void);

            /**
              * Check whether status code is for server error status or not
              * @return bool
              */
            bool IsServerErrorStatus(void);

        private:
            /**
              * Get response error code function for static function
              * @return HTTPResponseType
              */
            const HTTPResponseType GetResponseTypeCode(void) const;
            /**
              * Get response error code and description using response_type_description_tbl_ table
              * @param[int] HTTPStatusCodeImpl* : Instance for own class
              * @return HTTPResponseType
              */
            static std::string GetResponseTypeAndDescription(HTTPStatusCodeImpl *pObj);

        private:
            /* Auto lock key for initialization */
            base::Lock init_lock_;

            /* response error code */
            HTTPResponseType response_type_code_;
    };

}

#endif /* WEBSERVICE_HTTP_STATUS_CODE_IMPL_H_ */

