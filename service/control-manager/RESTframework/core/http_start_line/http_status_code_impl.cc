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

#include "base/string_number_conversions.h"
#include "base/synchronization/lock.h"
#include "base/values.h"
#include "http_start_line/http_status_code_defined.h"
#include "http_start_line/http_status_code_impl.h"

namespace webservice
{
    /**
      * Class Constructor
      */
    HTTPStatusCodeImpl::HTTPStatusCodeImpl(void)
    {
        response_type_code_ = RT_UNKNOWN;
    }

    /**
      * Initialize HTTP status code data.
      * @param[in] HTTPResponseType : response type for http status code
      * @return void
      */
    void HTTPStatusCodeImpl::InitHTTPStatusCode(HTTPResponseType response_error_code)
    {
        base::AutoLock locked(init_lock_);
        this->response_type_code_ = response_error_code;
    }

    /**
      * Generate HTTP status code from http response type.
      * @return int
      */
    int HTTPStatusCodeImpl::GenerateHTTPStatusCode()
    {
        if (this->response_type_code_ <= RT_200_END)
        {
            return kHttpStatusCodeSuccessOk;
        }
        else if (this->response_type_code_ <= RT_201_END)
        {
            return kHttpStatusCodeSuccessCreated;
        }
        else if (this->response_type_code_ <= RT_202_END)
        {
            return kHttpStatusCodeAccepted;
        }
        else if (this->response_type_code_ <= RT_204_END)
        {
            return kHttpStatusCodeSuccessNoContent;
        }
        else if (this->response_type_code_ <= RT_300_END)
        {
            return kHttpStatusCodeRedirectSpecialResponse;
        }
        else if (this->response_type_code_ <= RT_304_END)
        {
            return kHttpStatusCodeRedirectNotModified;
        }
        else if (this->response_type_code_ <= RT_400_END)
        {
            return kHttpStatusCodeErrClientBadRequest;
        }
        else if (this->response_type_code_ <= RT_404_END)
        {
            return kHttpStatusCodeErrClientNotFound;
        }
        else if (this->response_type_code_ <= RT_405_END)
        {
            return kHttpStatusCodeErrClientNotAllowed;
        }
        else if (this->response_type_code_ <= RT_406_END)
        {
            return kHttpStatusCodeErrClientNotAcceptable;
        }
        else if (this->response_type_code_ <= RT_408_END)
        {
            return kHttpStatusCodeErrClientRequestTimeOut;
        }
        else if (this->response_type_code_ <= RT_415_END)
        {
            return kHttpStatusCodeErrClientUnsupportedMediaType;
        }
        else if (this->response_type_code_ <= RT_500_END)
        {
            return kHttpStatusCodeErrServerInternalServerError;
        }
        else if (this->response_type_code_ <= RT_501_END)
        {
            return kHttpStatusCodeErrServerNotImplemented;
        }
        else if (this->response_type_code_ <= RT_503_END)
        {
            return kHttpStatusCodeErrServerServiceUnavailable;
        }
        return kHttpStatusCodeErrServerInternalServerError;
    }

    /**
      * Make error code and description for HTTP response.
      * @param[out] HTTPResponseType
      * @return bool
      */
    bool HTTPStatusCodeImpl::MakeHTTPStatusCodeAndDescription(base::DictionaryValue **in_response_body)
    {
        if ((NULL == in_response_body) || (NULL == *in_response_body))
        {
            return false;
        }

        std::string error_code = base::IntToString(this->response_type_code_);
        std::string response_string = HTTPStatusCodeImpl::GetResponseTypeAndDescription(this);
        (*in_response_body)->SetWithoutPathExpansion("errorCode",
                base::Value::CreateStringValue(error_code));
        (*in_response_body)->SetWithoutPathExpansion("errorDescription",
                base::Value::CreateStringValue(response_string));

        return true;
    }

    /**
      * Check whether status code is for information status or not
      * @return bool
      */
    bool HTTPStatusCodeImpl::IsInformationalStatus()
    {
        if (this->response_type_code_ >= RT_100_START &&
            this->response_type_code_ < RT_UNKNOWN)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
      * Check whether status code is for successful status or not
      * @return bool
      */
    bool HTTPStatusCodeImpl::IsSuccessfulStatus()
    {
        if (this->response_type_code_ >= RT_200_START &&
            this->response_type_code_ < RT_300_START)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
      * Check whether status code is for redirection status or not
      * @return bool
      */
    bool HTTPStatusCodeImpl::IsRedirectionStatus()
    {
        if (this->response_type_code_ >= RT_300_START &&
            this->response_type_code_ < RT_400_START)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
      * Check whether status code is for client error status or not
      * @return bool
      */
    bool HTTPStatusCodeImpl::IsClientErrorStatus()
    {
        if (this->response_type_code_ >= RT_400_START &&
            this->response_type_code_ < RT_500_START)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
      * Check whether status code is for server error status or not
      * @return bool
      */
    bool HTTPStatusCodeImpl::IsServerErrorStatus()
    {
        if (this->response_type_code_ >= RT_500_START &&
            this->response_type_code_ < RT_100_START)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
      * Get response error code function for static function
      * @return HTTPResponseType
      */
    const HTTPResponseType HTTPStatusCodeImpl::GetResponseTypeCode() const
    {
        return response_type_code_;
    }

    /**
      * Get response error code and description using response_type_description_tbl_ table
      * @param[int] HTTPStatusCodeImpl* : Instance for own class
      * @return HTTPResponseType
      */
    std::string HTTPStatusCodeImpl::GetResponseTypeAndDescription(HTTPStatusCodeImpl *pObj)
    {
        std::string response_description = kUnknownDescription;
        int loop = 0;
        do
        {
            if (webservice::response_type_description_tbl_[loop].response_type == pObj->GetResponseTypeCode())
            {
                response_description = webservice::response_type_description_tbl_[loop].response_description;
                break;
            }
            loop++;
        }
        while (RT_MAX != (webservice::response_type_description_tbl_[loop].response_type));
        return response_description;
    }

}
