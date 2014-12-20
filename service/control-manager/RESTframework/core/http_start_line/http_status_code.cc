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

#include "base/values.h"
#include "http_start_line/http_status_code.h"
#include "http_start_line/http_status_code_defined.h"
#include "http_start_line/http_status_code_impl.h"

namespace webservice
{

    /**
      * Class Constructor
      * @param[in] HTTPResponseType : response type
      */
    HTTPStatusCode::HTTPStatusCode(HTTPResponseType response_error_code)
    {
        pHttpStatusCodeImpl_ = NULL;
        pHttpStatusCodeImpl_ = new HTTPStatusCodeImpl;
        if (pHttpStatusCodeImpl_)
        {
            pHttpStatusCodeImpl_->InitHTTPStatusCode(response_error_code);
        }
    }

    /**
      * Class Destructor
      */
    HTTPStatusCode::~HTTPStatusCode()
    {
        if (pHttpStatusCodeImpl_)
        {
            delete pHttpStatusCodeImpl_;
            pHttpStatusCodeImpl_ = NULL;
        }
    }

    /**
      * Generate HTTP status code from http response type.
      * @return int
      */
    int HTTPStatusCode::GenerateHTTPStatusCode()
    {
        const int http_status_code = kHttpStatusCodeErrServerInternalServerError;
        return pHttpStatusCodeImpl_ ? pHttpStatusCodeImpl_->GenerateHTTPStatusCode() : http_status_code;
    }

    /**
      * Make error code and description for HTTP response.
      * @param[out] HTTPResponseType
      * @return bool
      */
    bool HTTPStatusCode::MakeHTTPStatusCodeAndDescription(base::DictionaryValue **in_response_body)
    {
        const bool ret = false;
        return pHttpStatusCodeImpl_ ? pHttpStatusCodeImpl_->MakeHTTPStatusCodeAndDescription(
                   in_response_body) : ret;
    }

    /**
      * Check whether status code is for informational status or not
      * @return bool
      */
    bool HTTPStatusCode::IsInformationalStatus()
    {
        const bool ret = false;
        return pHttpStatusCodeImpl_ ? pHttpStatusCodeImpl_->IsInformationalStatus() : ret;
    }

    /**
      * Check whether status code is for successful status or not
      * @return bool
      */
    bool HTTPStatusCode::IsSuccessfulStatus()
    {
        const bool ret = false;
        return pHttpStatusCodeImpl_ ? pHttpStatusCodeImpl_->IsSuccessfulStatus() : ret;
    }

    /**
      * Check whether status code is for redirection status or not
      * @return bool
      */
    bool HTTPStatusCode::IsRedirectionStatus()
    {
        const bool ret = false;
        return pHttpStatusCodeImpl_ ? pHttpStatusCodeImpl_->IsRedirectionStatus() : ret;
    }

    /**
      * Check whether status code is for client error status or not
      * @return bool
      */
    bool HTTPStatusCode::IsClientErrorStatus()
    {
        const bool ret = false;
        return pHttpStatusCodeImpl_ ? pHttpStatusCodeImpl_->IsClientErrorStatus() : ret;
    }

    /**
      * Check whether status code is for server error status or not
      * @return bool
      */
    bool HTTPStatusCode::IsServerErrorStatus()
    {
        const bool ret = false;
        return pHttpStatusCodeImpl_ ? pHttpStatusCodeImpl_->IsServerErrorStatus() : ret;
    }

}
