/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef RESPONSE_H
#define RESPONSE_H

#include <map>
#include <list>
#include <string>
#include "RequestResponseBody.h"
#include "Header.h"
#include "Utils.h"
#include "IncludeResolver.h"

namespace RAML
{
    class Response
    {
        public:
            virtual std::map<std::string, Header> getHeaders() const;
            virtual void setHeader(const std::string &headerName, const Header &header);
            virtual std::string getDescription() const;
            virtual void setDescription(const std::string &description);
            virtual void setResponseBody(const std::string &typeName);
            virtual void setResponseBody(const std::string &type, const RequestResponseBody &body) ;
            virtual std::map<std::string, RequestResponseBody> getResponseBody() const;
            virtual RequestResponseBody &getResponseBody(const std::string bodyType);


            Response() {}
            Response(const YAML::Node &yamlNode, IncludeResolver *includeResolver) { readResponse(yamlNode, includeResolver);}
        private:
            void readResponse(const YAML::Node &yamlNode, IncludeResolver *includeResolver) ;
        private:
            std::string m_description;
            std::map<std::string, RequestResponseBody> m_responseBody;
            std::map<std::string, Header> m_headers;
            IncludeResolver *m_includeResolver;
    };
}
#endif
