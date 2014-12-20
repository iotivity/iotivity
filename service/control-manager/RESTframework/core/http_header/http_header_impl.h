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

#ifndef WEBSERVICE_HTTP_HEADER_IMPL_H_
#define WEBSERVICE_HTTP_HEADER_IMPL_H_

#include <map>
#include <string>

#include "http_header.h"
#include "interface_http_header.h"
#include "base/synchronization/lock.h"


namespace webservice
{

    class HTTPHeaderImpl : public IHTTPHeader
    {
        public:
            virtual void InitHTTPHeader(const web_util::HttpHeaderData &http_request_info);
            virtual bool CopyHTTPHeader(web_util::HttpHeaderData &http_response_info);
            virtual std::string GetValue(std::string name);
            virtual void SetValue(const std::string &name, const std::string &value);

        private:
            virtual ~HTTPHeaderImpl(void) {}
            std::string GetStringStrategy(const std::string &input_string);

        private:
            base::Lock lock_;
            std::map<std::string, std::string> param_;
    };

}

#endif /* WEBSERVICE_HTTP_HEADER_IMPL_H_ */

