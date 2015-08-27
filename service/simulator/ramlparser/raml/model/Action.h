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

#ifndef ACTION_H
#define ACTION_H

#include <map>
#include <list>
#include <string>
#include "ActionType.h"
#include "Header.h"
#include "QueryParameter.h"
#include "RequestResponseBody.h"
#include "UriParameter.h"
#include "Response.h"
#include "Utils.h"
#include "IncludeResolver.h"

namespace RAML
{
    class Action
    {
        public:
            virtual ActionType getType() const;
            virtual void setType(const ActionType &type);
            virtual std::string getDescription() const;
            virtual void setDescription(const std::string &description);
            virtual std::map<std::string, Header> getHeaders() const;
            virtual void setHeader(const std::string &headerName, const Header &header);
            virtual std::map<std::string, QueryParameter> getQueryParameters()const;
            virtual void setQueryParameter(const std::string &paramName, const QueryParameter &queryParameter);
            virtual RequestResponseBody &getRequestBody(std::string bodyType);
            virtual std::map<std::string, RequestResponseBody> getRequestBody() const;
            virtual void setRequestBody(const std::string &typeName);
            virtual void setRequestBody(const std::string &typeName , const RequestResponseBody &body);
            virtual Response &getResponse(std::string responseCode);
            virtual std::map<std::string, Response> getResponses() const;
            virtual void setResponse(const std::string &responseCode, const Response &response);
            virtual std::list<std::string> getProtocols() const;
            virtual void setProtocol(const std::string &protocol);
            virtual std::map< std::string, UriParameter > getBaseUriParameters() const;
            virtual void setBaseUriParameter(const std::string &paramName ,
                                             const UriParameter  &baseUriParameter);
            virtual std::list<std::string> getTraits() const;
            virtual void setTrait(const std::string &trait);


            Action() { }
            Action(const ActionType actionType, const YAML::Node &yamlNode, IncludeResolver *includeResolver)
            {
                readAction(actionType, yamlNode, includeResolver);
            }

        private:
            virtual void readAction(const ActionType actionType, const YAML::Node &yamlNode,
                                    IncludeResolver *includeResolver);


        private:
            ActionType m_type;
            std::string m_description;
            std::map<std::string, Header> m_headers;
            std::map<std::string, QueryParameter> m_queryParameters;
            std::map<std::string, RequestResponseBody> m_requestBody;
            std::map<std::string, Response> m_responses;
            std::list<std::string> m_protocols;
            std::map< std::string, UriParameter > m_baseUriParameters;
            std::list<std::string> m_trait;

        private:
            IncludeResolver *m_includeResolver;
    };
}
#endif
