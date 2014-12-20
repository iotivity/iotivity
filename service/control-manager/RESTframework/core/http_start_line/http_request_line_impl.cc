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

#include "base/synchronization/lock.h"
#include "base/string_split.h"
#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line_impl.h"
#include "base/logging.h"

namespace webservice
{

    /**
      * Class Constructor
      */
    HTTPRequestLineImpl::HTTPRequestLineImpl(void)
    {
        request_type_ = web_util::HTTP_METHOD_UNSET;
    }
    /**
      * Initialize HTTP request line data.
      * @param[in] int : request type (HTTP Method)
      * @param[in] std::string : URI
      * @param[in] std::string : query parameter
      * @return void
      */
    void HTTPRequestLineImpl::InitHTTPRequestLine(int type, std::string uri,
            std::string query_parameter)
    {
        base::AutoLock locked(init_lock_);
        this->request_type_ = type;
        this->request_uri_ = uri;
        this->query_parameter_ = query_parameter;
        this->ParseUriAndQuery(uri, query_parameter);
    }

    /**
      * Get URI to vector type.
      * @return std::vector<std::string>
      */
    const std::vector<std::string> &HTTPRequestLineImpl::GetUriFields() const
    {
        return this->uri_fields_;
    }

    /**
      * Get URI to string type.
      * @return std::string&
      */
    const std::string &HTTPRequestLineImpl::GetOriginUri() const
    {
        return this->request_uri_;
    }

    /**
      * Get http method
      * @return int
      */
    const int HTTPRequestLineImpl::GetHTTPMethod() const
    {
        return this->request_type_;
    }

    /**
      * Get query content to map type.
      * @return std::map<std::string, std::string>
      */
    const std::map<std::string, std::string> &HTTPRequestLineImpl::GetQueryContent() const
    {
        return this->query_content_;
    }

    /**
      * Parse URI and Query paramter
      * @param[in] string : URI
      * @param[in] string : query paramter
      * @return bool
      */
    bool HTTPRequestLineImpl::ParseUriAndQuery(const std::string &uri, const std::string &query)
    {
        // parse the uri and query and update the uri_fields_ and query_content_
        // Get resource hierarchy structure from URI path
        // In case the URI ends with a '/' - it must be removed
        int char_pos = uri.length() - 1;
        char last_char = uri.at(char_pos);
        std::string modified_uri;

        modified_uri = uri;
        if ('/' == last_char)
        {
            modified_uri = modified_uri.substr(0, char_pos);
        }
        base::SplitString(modified_uri, '/', &(this->uri_fields_));

        // Get each key and value pair from URI query
        if (query.length() > 0)
        {
            this->query_content_.clear();
            if (NameValueMap(query, &(this->query_content_)) == false)
            {
                LOG(ERROR) << "Unable to parse URI query arguments \n";
                this->uri_fields_.clear();
                this->query_content_.clear();
                return false;
            }
        }
        return true;
    }

    /**
      * Save query parameter to map
      * @param[in] string : query
      * @param[out] std::map<std::string, std::string>* : query paramter map
      * @return bool
      */
    bool HTTPRequestLineImpl::NameValueMap(const std::string &query,
                                           std::map<std::string, std::string> *name_value_pair)
    {
        // Stores the kay and values of query in a map<string, string>
        std::string temp = "";
        std::string name = "";
        std::string value = "";
        std::vector<std::string>::iterator it;
        std::vector<std::string> words;

        if ((NULL == name_value_pair) || query.empty())
        {
            LOG(ERROR) << "NULL arguments are sent \n";
            return false;
        }

        // Split name, value pairs in a query into vector<string>
        base::SplitString(query, '&', &words);

        // Get the key and value from each key value pair and store them in
        // map<string, string>
        for (it = words.begin(); it != words.end(); ++it)
        {
            temp = *it;
            // Find key value seperator position in key value pair
            const std::string::size_type pos_eq = temp.find('=');

            if (pos_eq  != std::string::npos)
            {
                name = temp.substr(0, pos_eq);
                value = temp.substr(pos_eq + 1);
                name_value_pair->insert(std::pair<std::string, std::string>(name, value));
            }
            else
            {
                // If seperator is not found in a key value pair, value is empty
                // i.e value = ""
                name = temp;
                value = "";
                name_value_pair->insert(std::pair<std::string, std::string>(name, value));
            }
        }
        words.clear();
        return true;
    }

}
