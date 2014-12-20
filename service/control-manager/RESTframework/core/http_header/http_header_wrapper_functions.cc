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

#include <list>
#include <string>

#include "base/string_number_conversions.h"
#include "base/string_util.h"
#include "http_body/creator_http_body_generator.h"
#include "http_header/http_header_defined.h"
#include "http_header/http_header_wrapper_functions.h"
#include "base/logging.h"

namespace webservice
{

    /**
       * Negotiate content-type from accept in http request header
       * @param[in] accept : accept in http request header
       * @return HttpContentType
       */
    HttpContentType HttpHeaderWrapperFunctions::NegotiateContentType(const std::string &accept)
    {
        // If a client does not take accpet in request header, default value is "application/json".
        if ("" == accept) return kHttpContentTypeApplicationJSON;

        // Parse accept field in http request
        std::list<stAcceptValue> accept_value_list;
        bool result = HttpHeaderWrapperFunctions::ParseAcceptList(accept, accept_value_list);
        if (!result)
        {
            // accept is existed or our generator is not supported
            DLOG(ERROR) << "NegotiateContentType(Default)";
            return kHttpContentTypeApplicationJSON;
        }
        // Negotiation
        if (accept_value_list.size())
        {
            // sort content type using q vaule
            accept_value_list.sort(ParamCompare);
            // negoriation : return first list
            std::list<stAcceptValue>::iterator it = accept_value_list.begin();
            std::string accept_type_str = it->content_type;
            HttpContentType content_type = HttpHeaderWrapperFunctions::ChangeContentTypeToEnum(accept_type_str);
            return content_type;
        }
        else
        {
            // HttpHeaderWrapperFunctions::ParseAcceptList already processed it,
            // but if somebody change above function, we need checking that accept list is 0.
            return kHttpContentTypeApplicationJSON;
        }
    }

    /**
       * Change Content Type to HttpContentType(Enum) from string
       * @param[in] content_type_text : string type
       * @return HttpContentType
       */
    HttpContentType HttpHeaderWrapperFunctions::ChangeContentTypeToEnum(const std::string
            &content_type_text)
    {
        // Default value is JSON, so if accept is "*/*" then return JSON format.
        if ("*/*" == content_type_text) return kHttpContentTypeApplicationJSON;
        HttpContentType content_type = kHttpContentTypeUnknown;
        int loop = 0;
        do
        {
            // Check contest type as string type with defined table (content_type_tbl_)
            if (webservice::content_type_tbl_[loop].content_type_text == content_type_text)
            {
                content_type = webservice::content_type_tbl_[loop].content_type;
                break;
            }
            loop++;
            // defined table get kHttpContentTypeUnknown as final type.
        }
        while (kHttpContentTypeUnknown != (webservice::content_type_tbl_[loop].content_type));

        return content_type;
    }

    /**
       * Parse Accept from http request header
       * if content type is supported and q value is 1, immediately return the content type,
       * else supported content type is saved to list with q value, and return them.
       * @param[in] accept : accept in http request header
       * @param[out] accept_value_list : parsed accept list
       * @return bool
       */
    bool HttpHeaderWrapperFunctions::ParseAcceptList(const std::string &accept,
            std::list<stAcceptValue> &accept_value_list)
    {
        if ("" == accept) return false;
        bool ret = false;
        std::string accept_local = "";
        std::string accept_sub_str = "";
        int q_value_local = 0;

        // Remove space character
        RemoveChars(accept, " ", &accept_local);
        DLOG(INFO) << "Accept : " << accept_local;

        // delimiter for each accept
        std::string::size_type pos_eq = accept_local.find(',');
        do
        {
            // If accept take 2 more, then this logic is used
            // Separate accept string, delimiter character is ","
            if (std::string::npos != pos_eq) accept_sub_str = accept_local.substr(0, pos_eq);
            else accept_sub_str = accept_local;

            // this is for sorting with q value.
            stAcceptValue accept_value;

            // delimiter for each q value, which means order of priority for content type
            std::string::size_type pos_q_value = accept_sub_str.find("q=");
            if (std::string::npos != pos_q_value)
            {
                // To get q value as string type in accept field
                // 2 means size of "q="
                std::string q_value_str = accept_sub_str.substr(pos_q_value + 2, pos_eq);
                // When process sorting, integer type is better than double type
                double q_value = 0;
                if (base::StringToDouble(q_value_str, &q_value))
                {
                    q_value_local = static_cast<int>(q_value * 1000 + 0.5);
                }
                else
                {
                    q_value_local = 0;
                }

                // Get each accept string
                accept_sub_str = accept_local.substr(0, pos_q_value - 1);
                DLOG(INFO) << "accept_sub_str : " << accept_sub_str << ", q_value:" << q_value;

                // Convert content type to enumeration from string
                HttpContentType content_type = HttpHeaderWrapperFunctions::ChangeContentTypeToEnum(accept_sub_str);
                // check existed parser
                CreatorHTTPBodyGenerator &factory = CreatorHTTPBodyGenerator::GetInstance();
                if (factory.IsRegisteredObject(content_type))
                {
                    accept_value.content_type = accept_sub_str;
                    accept_value.q_value = q_value_local;
                    accept_value_list.push_back(accept_value);
                    ret = true;
                }
            }
            else
            {
                // This accept does not take any q value.
                DLOG(INFO) << "accept_sub_str : " << accept_sub_str;

                // Convert content type to enumeration from string
                HttpContentType content_type = HttpHeaderWrapperFunctions::ChangeContentTypeToEnum(accept_sub_str);
                accept_value.content_type = accept_sub_str;
                accept_value.q_value = 1000;

                // This is that "q value" is 1, so if the system support the content type generator,
                // No need more checking
                // check existed parser
                CreatorHTTPBodyGenerator &factory = CreatorHTTPBodyGenerator::GetInstance();
                if (factory.IsRegisteredObject(content_type))
                {
                    accept_value_list.clear();
                    accept_value_list.push_back(accept_value);
                    return true;
                }
            }
            if (std::string::npos != pos_eq)
            {
                accept_local = accept_local.substr(pos_eq + 1);
                pos_eq = accept_local.find(',');
            }
            else
            {
                accept_local = "";
            }
        }
        while (0 != accept_local.size());
        return ret;
    }

    /**
       * Compare each content type from accept in http request header
       * @param[in] first_param
       * @param[in] second_param
       * @return bool
       */
    bool HttpHeaderWrapperFunctions::ParamCompare (const stAcceptValue &first_param,
            const stAcceptValue &second_param)
    {
        return first_param.q_value >= second_param.q_value;
    }

}

