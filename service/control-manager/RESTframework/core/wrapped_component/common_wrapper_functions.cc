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

#include <string>
#include <vector>
#include <uuid/uuid.h>

#include "base/string_util.h"
#include "base_resource/http_message.h"
#include "http_header/http_request_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

// generate a unique id for every new object
    std::string CommonWrapperFunctions::GenerateUUID()
    {
        std::string uuidStr = "";
        char charUuid[256];
        uuid_t uu;
        uuid_generate(uu);
        uuid_unparse_lower(uu, charUuid);
        snprintf(charUuid, (strlen(charUuid) + 1),
                 "%02x%02x%02x%02x-"
                 "%02x%02x-"
                 "%02x%02x-"
                 "%02x%02x-"
                 "%02x%02x%02x%02x%02x%02x",
                 uu[0], uu[1], uu[2], uu[3],
                 uu[4], uu[5],
                 uu[6], uu[7],
                 uu[8], uu[9],
                 uu[10], uu[11], uu[12], uu[13], uu[14], uu[15]);
        uuidStr = charUuid;
        return uuidStr;
    }

    bool CommonWrapperFunctions::GetIdentifiedObjectDescription(void *p_in, void *p_out,
            const std::string &service_name, const std::string &output_name)
    {
        if ("device" == service_name)
        {
            return GetDeviceDescription(p_in, p_out, "", "");
        }
        return false;
    }

    bool CommonWrapperFunctions::SetIdentifiedObjectDescription(void *p_in, void *p_out,
            const std::string &service_name, const std::string &input_name)
    {
        if ("device" == service_name)
        {
            return SetDeviceDescription(p_in, p_out, "", "");
        }
        return false;
    }

// Add ID manager wrappers here
// Please do not modify above comment since it is being used by code generator

}

