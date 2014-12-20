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

#include <iostream>
#include <string>

#include "base/version.h"
#include "http_header/http_header_defined.h"
#include "http_header/http_header_impl.h"
#include "http_header/http_request_header.h"
#include "http_header.h"
#include "base/logging.h"

namespace webservice
{

    HTTPRequestHeader::HTTPRequestHeader(const web_util::HttpHeaderData &http_request_info)
    {
        pHTTPRequestHeaderImpl_ = NULL;
        pHTTPRequestHeaderImpl_ = new HTTPHeaderImpl;
        pHTTPRequestHeaderImpl_->InitHTTPHeader(http_request_info);
    }

    HTTPRequestHeader::~HTTPRequestHeader()
    {
        if (pHTTPRequestHeaderImpl_)
        {
            delete pHTTPRequestHeaderImpl_;
            pHTTPRequestHeaderImpl_ = NULL;
        }
    }

    inline std::string HTTPRequestHeader::GetValue(std::string name) const
    {
        return pHTTPRequestHeaderImpl_->GetValue(name);
    }

    const std::string HTTPRequestHeader::GetAccept() const
    {
        return this->GetValue("Accept");
    }

    const std::string HTTPRequestHeader::GetETag() const
    {
        return this->GetValue("ETag");
    }

    HttpContentType HTTPRequestHeader::GetContentType() const
    {
        const std::string content_type_str = this->GetValue("Content-Type");
        std::string content_type_sub_str = "";

        /* Default is JSON type */
        if (content_type_str == "") return kHttpContentTypeApplicationJSON;

        DLOG(INFO) << "Get Content-Type : " << content_type_str;
        const std::string::size_type pos_eq = content_type_str.find(';');
        if (std::string::npos != pos_eq)
        {
            content_type_sub_str = content_type_str.substr(0, pos_eq);
        }
        else
        {
            content_type_sub_str = content_type_str;
        }
        HttpContentType content_type = kHttpContentTypeUnknown;
        int loop = 0;

        do
        {
            if (webservice::content_type_tbl_[loop].content_type_text == content_type_sub_str)
            {
                content_type = webservice::content_type_tbl_[loop].content_type;
                break;
            }
            loop++;
        }
        while (kHttpContentTypeUnknown != (webservice::content_type_tbl_[loop].content_type));
        return content_type;
    }

    const std::string HTTPRequestHeader::GetCharset() const
    {
        std::string content_type_str = this->GetValue("Content-Type");
        std::string charset_str = "";
        const std::string::size_type pos_eq = content_type_str.find("charset=");
        if (std::string::npos != pos_eq)
        {
            charset_str = content_type_str.substr(pos_eq + 8);
        }
        return charset_str;
    }

    const std::string HTTPRequestHeader::GetContentLanguage() const
    {
        return this->GetValue("Content-Language");
    }

    const std::string HTTPRequestHeader::GetDate() const
    {
        return this->GetValue("Date");
    }

    const std::string HTTPRequestHeader::GetSessionId() const
    {
        return this->GetValue("X-API-SessionID");
    }

    const std::string HTTPRequestHeader::GetClientSHPVersion() const
    {
        std::string x_api_version_of_client = this->GetValue("X-API-Version");
        std::string new_api_version_of_client = "";

        if ("" == x_api_version_of_client)
        {
            x_api_version_of_client = std::string(kServiceableTheLowestAPIVersion);
        }
        // Remove 'v' from x_api_version_of_client
        new_api_version_of_client = x_api_version_of_client.substr(x_api_version_of_client.find("v") + 1);
        DLOG(INFO) << "new_api_version_of_client : " << new_api_version_of_client;

        BaseVersion api_version_of_client = BaseVersion(new_api_version_of_client);
        if (!api_version_of_client.IsValid())
        {
            LOG(ERROR) << "Invalid version header value :: " << x_api_version_of_client;
            x_api_version_of_client = std::string(kServiceableTheLowestAPIVersion);
            new_api_version_of_client = x_api_version_of_client.substr(x_api_version_of_client.find("v") + 1);
            api_version_of_client = BaseVersion(new_api_version_of_client);
        }
        return api_version_of_client.GetString();
    }

    const std::string HTTPRequestHeader::GetHost() const
    {
        return this->GetValue("Host");
    }

    const std::string HTTPRequestHeader::GetPeerId() const
    {
        return this->GetValue("X-API-PeerID");
    }

}
