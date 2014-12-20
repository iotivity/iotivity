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

#include "base/json/json_reader.h"
#include "http_body/http_body_json_parser.h"
#include "http_header/http_header_defined.h"
#include "base/logging.h"

namespace webservice
{

    /**
    * Register function to create this class.
    * @param[in] HttpContentType
    * @param[in] IHTTPBodyParser*
    * @return static RegisterHTTPBodyParser
    */
    RegisterHTTPBodyParser HTTPBodyJsonParser::regJsonParser(kHttpContentTypeApplicationJSON,
            &HTTPBodyJsonParser::Create);

    /**
    * Create this class.
    * @return IHTTPBodyParser*
    */
    IHTTPBodyParser *HTTPBodyJsonParser::Create()
    {
        // This class will allocate in own class, so you should be careful to create this class.
        // Do NOT forget memory free of this allocated object.
        // The use of smart pointer is one of them for the solution.
        return new HTTPBodyJsonParser;
    }

    /**
    * Get content type for this class,
    * This class is parser for Json format.
    * So, The return is always "kHttpContentTypeApplicationJSON"
    * @return HttpContentType
    */
    const HttpContentType HTTPBodyJsonParser::GetCurrentContentType()
    {
        return kHttpContentTypeApplicationJSON;
    }

    /**
    * Parser string type(json format) to dictionary value.
    * @param[in] std::string&
    * @param[out] base::DictionaryValue**
    * @return bool
    */
    bool HTTPBodyJsonParser::ParseRequestBody(const std::string &in_request_body,
            base::DictionaryValue **out_request_body)
    {
        *out_request_body = static_cast<base::DictionaryValue *>(base::JSONReader::Read(in_request_body));
        if (NULL != *out_request_body && (*out_request_body)->IsType(Value::TYPE_DICTIONARY))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

}
