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

#include "base/string_util.h"
#include "http_header/http_header_impl.h"

namespace webservice
{

    void HTTPHeaderImpl::InitHTTPHeader(const web_util::HttpHeaderData &http_request_info)
    {
        base::AutoLock locked(lock_);
        for (int i = 0 ; i < web_util::HttpHeaderData::NUM_HEADER_PARAMS ; i++)
        {
            if (http_request_info.is_this_set[i] == 1)
            {
                std::string key = GetStringStrategy(http_request_info.param_name[i]);
                std::string value = http_request_info.param_value[i];
                param_.insert(std::pair<std::string, std::string>(key, value));
            }
        }
    }

    bool HTTPHeaderImpl::CopyHTTPHeader(web_util::HttpHeaderData &http_response_info)
    {
        int num = 0;
        std::map<std::string, std::string>::iterator it;
        for (it = this->param_.begin(); it != this->param_.end(); it++)
        {
            http_response_info.param_name[num] = it->first;
            http_response_info.param_value[num] = it->second;
            http_response_info.is_this_set[num] = 1;
            http_response_info.flag_outheader_overwrite[num] = 0;
            num++;
        }
        return (num > 0) ? true : false;
    }

    std::string HTTPHeaderImpl::GetValue(std::string name)
    {
        return param_[GetStringStrategy(name)];
    }

    void HTTPHeaderImpl::SetValue(const std::string &name, const std::string &value)
    {
        base::AutoLock locked(lock_);
        param_[name] = value;
    }

    std::string HTTPHeaderImpl::GetStringStrategy(const std::string &input_string)
    {
        const bool string_to_lower = true;
        return string_to_lower ? StringToLowerASCII(input_string) : input_string;
    }

}
