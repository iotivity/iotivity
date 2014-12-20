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

#include "base/json/json_writer.h"
#include "http_body/http_body_json_generator.h"
#include "http_header/http_header_defined.h"

namespace webservice
{

    /**
    * Register function to create this class.
    * @param[in] HttpContentType
    * @param[in] IHTTPBodyGenerator*
    * @return static RegisterHTTPBodyGenerator
    */
    RegisterHTTPBodyGenerator HTTPBodyJsonGenerator::regJsonParser(kHttpContentTypeApplicationJSON,
            &HTTPBodyJsonGenerator::Create);

    /**
    * Create this class.
    * @return IHTTPBodyGenerator*
    */
    IHTTPBodyGenerator *HTTPBodyJsonGenerator::Create()
    {
        // This class will allocate in own class, so you should be careful to create this class.
        // Do NOT forget memory free of this allocated object.
        // The use of smart pointer is one of them for the solution.
        return new HTTPBodyJsonGenerator;
    }

    /**
    * Get content type for this class,
    * This class is generator for Json format.
    * So, The return is always "web_util::kHttpContentTypeApplicationJSON"
    * @return HttpContentType
    */
    const HttpContentType HTTPBodyJsonGenerator::GetCurrentContentType()
    {
        return kHttpContentTypeApplicationJSON;
    }

    /**
    * Generate dictionary value to json format.
    * @param[in] base::DictionaryValue*
    * @return std::string
    */
    std::string HTTPBodyJsonGenerator::Generate(const base::DictionaryValue *in_request_body)
    {
        std::string out_response_body = "";
        base::JSONWriter::Write(in_request_body, &out_response_body);
        return out_response_body;
    }

}