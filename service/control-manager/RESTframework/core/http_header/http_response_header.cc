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

#include "http_header/http_header_defined.h"
#include "http_header/http_header_impl.h"
#include "http_header/http_response_header.h"

namespace webservice
{
    HTTPResponseHeader::HTTPResponseHeader()
    {
        pHTTPResponseHeaderImpl_ = NULL;
        pHTTPResponseHeaderImpl_ = new HTTPHeaderImpl;
    }

    HTTPResponseHeader::~HTTPResponseHeader()
    {
        if (pHTTPResponseHeaderImpl_)
        {
            delete pHTTPResponseHeaderImpl_;
            pHTTPResponseHeaderImpl_ = NULL;
        }
    }

    inline void HTTPResponseHeader::SetValue(const std::string &name, const std::string &value)
    {
        pHTTPResponseHeaderImpl_->SetValue(name, value);
    }

    void HTTPResponseHeader::SetContentType(const HttpContentType &content_type)
    {
        std::string content_type_text = "";
        int loop = 0;

        do
        {
            if (webservice::content_type_tbl_[loop].content_type == content_type)
            {
                content_type_text = webservice::content_type_tbl_[loop].content_type_text;
                break;
            }
            loop++;
        }
        while (kHttpContentTypeUnknown != (webservice::content_type_tbl_[loop].content_type));
        pHTTPResponseHeaderImpl_->SetValue("Content-Type", content_type_text);
    }

    void HTTPResponseHeader::SetCharset(const std::string &char_set)
    {
        // pHTTPResponseHeaderImpl_->SetValue(name, char_set);
    }

    void HTTPResponseHeader::SetContentLanguage(const std::string &contenct_language)
    {
        pHTTPResponseHeaderImpl_->SetValue("Content-Language", contenct_language);
    }

    void HTTPResponseHeader::SetDate(const std::string &date)
    {
        pHTTPResponseHeaderImpl_->SetValue("Date", date);
    }

    void HTTPResponseHeader::SetETag(const std::string &etag)
    {
        pHTTPResponseHeaderImpl_->SetValue("ETag", etag);
    }

    void HTTPResponseHeader::SetServerSHPVersion(const std::string &version)
    {
        pHTTPResponseHeaderImpl_->SetValue("X-API-Version", version);
    }

    void HTTPResponseHeader::SetLocation(const std::string &host, const std::string &uri,
                                         const std::string &id)
    {
        std::string location = "http://" + host + "/" + uri + "/" + id;
        pHTTPResponseHeaderImpl_->SetValue("Location", location);
    }

    bool HTTPResponseHeader::CopyHTTPResponseHeader(web_util::HttpHeaderData &http_response_info)
    {
        bool bRet = pHTTPResponseHeaderImpl_->CopyHTTPHeader(http_response_info);
        return bRet;
    }

}

