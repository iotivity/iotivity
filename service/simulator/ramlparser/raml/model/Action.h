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
 *      http://www.apache.org/licenses/LICENSE-2.0
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
            virtual std::map<std::string, HeaderPtr > const &getHeaders() const;
            virtual void setHeader(const std::string &headerName, const HeaderPtr &header);
            virtual std::map<std::string, QueryParameterPtr > const &getQueryParameters()const;
            virtual void setQueryParameter(const std::string &paramName,
                                           const QueryParameterPtr &queryParameter);

            virtual RequestResponseBodyPtr getRequestBody(const std::string &bodyType);
            virtual std::map<std::string, RequestResponseBodyPtr> const &getRequestBody() const;
            virtual void setRequestBody(const std::string &typeName);
            virtual void setRequestBody(const std::string &typeName , const RequestResponseBodyPtr &body);

            virtual ResponsePtr getResponse(const std::string &responseCode);
            virtual std::map<std::string, ResponsePtr> const &getResponses() const;
            virtual void setResponse(const std::string &responseCode, const ResponsePtr &response);
            virtual std::list<std::string> const &getProtocols() const;
            virtual void setProtocol(const std::string &protocol);
            virtual std::map< std::string, UriParameterPtr > const &getBaseUriParameters() const;
            virtual void setBaseUriParameter(const std::string &paramName ,
                                             const UriParameterPtr &baseUriParameter);
            virtual std::list<std::string> const &getTraits() const;
            virtual void setTrait(const std::string &trait);


            Action(): m_includeResolver(NULL) { }
            Action(const ActionType actionType, const YAML::Node &yamlNode,
                   const IncludeResolverPtr &includeResolver)
                : m_includeResolver(includeResolver)
            {
                readAction(actionType, yamlNode);
            }
            Action(const Action &action) = default;
        private:
            virtual void readAction(const ActionType actionType, const YAML::Node &yamlNode);


        private:
            ActionType m_type;
            std::string m_description;
            std::map<std::string, HeaderPtr> m_headers;
            std::map<std::string, QueryParameterPtr> m_queryParameters;
            std::map<std::string, RequestResponseBodyPtr> m_requestBody;
            std::map<std::string, ResponsePtr> m_responses;
            std::list<std::string> m_protocols;
            std::map< std::string, UriParameterPtr > m_baseUriParameters;
            std::list<std::string> m_trait;

        private:
            IncludeResolverPtr m_includeResolver;
    };

    typedef std::shared_ptr<Action> ActionPtr;
}
#endif
