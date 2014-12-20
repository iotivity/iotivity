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

#ifndef WEBSERVICE_NEW_RESOURCE_REQUEST_HANDLER_CONTROL_MANAGER_RESPONSE_CODE_H_
#define WEBSERVICE_NEW_RESOURCE_REQUEST_HANDLER_CONTROL_MANAGER_RESPONSE_CODE_H_

#include "http_start_line/http_status_code_defined.h"

namespace webservice
{

// map the response code received from control manager to rest engine defined ones.
    static HTTPResponseType getResponseCode(int resp_code)
    {
        HTTPResponseType status_code = RT_UNKNOWN;
        switch (resp_code)
        {
            case 200:
                {
                    status_code = RT_200_OK;
                } break;
            case 201:
                {
                    status_code = RT_201_CREATE;
                } break;
            case 202:
                {
                    status_code = RT_202_ACCEPTED;
                } break;
            case 204:
                {
                    status_code = RT_204_NO_CONTENT;
                } break;
            case 400:
                {
                    status_code = RT_400_BAD_REQUEST;
                } break;
            case 404:
                {
                    status_code = RT_404_NOT_FOUND;
                } break;
            case 500:
                {
                    status_code = RT_500_INTERNAL_SERVER_ERROR;
                } break;
            // Default
            default:
                {
                    status_code = RT_UNKNOWN;
                } break;
        }
        return status_code;
    }
}

#endif /* WEBSERVICE_NEW_RESOURCE_REQUEST_HANDLER_CONTROL_MANAGER_RESPONSE_CODE_H_ */
