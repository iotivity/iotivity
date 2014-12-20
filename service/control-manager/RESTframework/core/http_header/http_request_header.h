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

#ifndef WEBSERVICE_HTTP_REQUEST_HEADER_H_
#define WEBSERVICE_HTTP_REQUEST_HEADER_H_

#include <string>

#include "base/version.h"
#include "http_header/http_header_defined.h"
#include "http_header.h"
#include "rest_engine_export.h"

namespace webservice
{
    using web_util::HttpHeaderData;
    class IHTTPHeader;
    class REST_ENGINE_EXPORT HTTPRequestHeader
    {
        public:
            HTTPRequestHeader(const web_util::HttpHeaderData &http_request_info);
            virtual ~HTTPRequestHeader(void);

            inline std::string GetValue(std::string name) const;
            const std::string GetAccept(void) const;
            HttpContentType GetContentType(void) const;
            const std::string GetCharset(void) const;
            const std::string GetContentLanguage(void) const;
            const std::string GetDate(void) const;
            const std::string GetSessionId() const;
            const std::string GetClientSHPVersion(void) const;
            const std::string GetETag(void) const;
            const std::string GetHost() const;
            const std::string GetPeerId() const;

        private:
            IHTTPHeader *pHTTPRequestHeaderImpl_;
    };

}

#endif /* WEBSERVICE_HTTP_REQUEST_HEADER_H_ */

