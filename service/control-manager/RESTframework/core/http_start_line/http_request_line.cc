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

#include <map>
#include <string>
#include <vector>

#include "http_start_line/http_request_line.h"
#include "http_start_line/http_request_line_impl.h"

namespace webservice
{

    /**
      * Class Constructor
      * @param[in] int : Method
      * @param[in] string : URI
      * @param[in] string : query parameter
      */
    HTTPRequestLine::HTTPRequestLine(int type, std::string uri, std::string query_parameter)
    {
        pHttpRequestLineImpl_ = NULL;
        pHttpRequestLineImpl_ = new HTTPRequestLineImpl;
        if (pHttpRequestLineImpl_)
        {
            pHttpRequestLineImpl_->InitHTTPRequestLine(type, uri, query_parameter);
        }
    }

    /**
      * Class Destructor
      */
    HTTPRequestLine::~HTTPRequestLine()
    {
        if (pHttpRequestLineImpl_)
        {
            delete pHttpRequestLineImpl_;
            pHttpRequestLineImpl_ = NULL;
        }
    }

    /**
      * Get URI to vector type.
      * @return std::vector<std::string>
      */
    const std::vector<std::string> &HTTPRequestLine::GetUriFields() const
    {
        static std::vector<std::string> uri_field;
        return pHttpRequestLineImpl_ ? pHttpRequestLineImpl_->GetUriFields() : uri_field;
    }

    /**
      * Get URI to string type.
      * @return std::string&
      */
    const std::string &HTTPRequestLine::GetOriginUri() const
    {
        static std::string origin_uri = "";
        return pHttpRequestLineImpl_ ? pHttpRequestLineImpl_->GetOriginUri() : origin_uri;
    }

    /**
      * Get query content to map type.
      * @return std::map<std::string, std::string>
      */
    const std::map<std::string, std::string> &HTTPRequestLine::GetQueryContent() const
    {
        static std::map<std::string, std::string> query_content;
        return pHttpRequestLineImpl_ ? pHttpRequestLineImpl_->GetQueryContent() : query_content;
    }

    /**
      * Get filter value to vector type.
      * @param[out] list_query_key
      * @return bool
      */
    bool HTTPRequestLine::GetFilterKeyString(std::vector<std::string> *list_query_key) const
    {
        if (NULL == list_query_key) return false;
        const std::map<std::string, std::string> &query_content = GetQueryContent();
        std::map<std::string, std::string>::const_iterator query_it = query_content.find("filter");
        // If filter param is not existed, ignore
        if (query_content.end() == query_it) return false;

        const std::string &filter_keys = query_it->second;
        if ("" == filter_keys) return false;

        std::string dummy_string = filter_keys;
        std::string found_key = "";

        for (size_t delimiter_position = dummy_string.find(',');
             delimiter_position != std::string::npos;
             delimiter_position = dummy_string.find(','))
        {
            found_key = dummy_string.substr(0, delimiter_position);
            list_query_key->push_back(found_key);
            dummy_string.erase(0, delimiter_position + 1);
        }
        found_key = dummy_string;
        list_query_key->push_back(found_key);

        return true;
    }

    /**
      * Get http method
      * @return int
      */
    const int HTTPRequestLine::GetHTTPMethod() const
    {
        const int http_method = -1;
        return pHttpRequestLineImpl_ ? pHttpRequestLineImpl_->GetHTTPMethod() : http_method;
    }

}
