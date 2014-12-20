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

#ifndef WEBSERVICE_HTTP_RESPONSE_HEADER_H_
#define WEBSERVICE_HTTP_RESPONSE_HEADER_H_

#include <string>

#include "http_header/http_header_defined.h"
#include "http_header.h"
#include "rest_engine_export.h"

namespace webservice
{

    class IHTTPHeader;  /* forward declaration */
    class REST_ENGINE_EXPORT HTTPResponseHeader
    {
        public:
            HTTPResponseHeader(void);
            virtual ~HTTPResponseHeader(void);

            inline void SetValue(const std::string &name, const std::string &value);
            void SetContentType(const HttpContentType &content_type);
            void SetCharset(const std::string &char_set);
            void SetContentLanguage(const std::string &contenct_language);
            void SetDate(const std::string &date);
            void SetETag(const std::string &etag);
            void SetServerSHPVersion(const std::string &version);
            void SetLocation(const std::string &host, const std::string &uri, const std::string &id);

            bool CopyHTTPResponseHeader(web_util::HttpHeaderData &http_response_info);

        private:
            IHTTPHeader *pHTTPResponseHeaderImpl_;
    };

}

#endif /* WEBSERVICE_HTTP_RESPONSE_HEADER_H_ */

