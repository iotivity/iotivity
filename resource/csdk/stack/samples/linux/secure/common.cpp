//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "common.h"

#include "ocstack.h"
#include "experimental/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define TAG "sample-common"

const char *getResult(OCStackResult result) {
    switch (result) {
    case OC_STACK_OK:
        return "OC_STACK_OK";
    case OC_STACK_RESOURCE_CREATED:
        return "OC_STACK_RESOURCE_CREATED";
    case OC_STACK_RESOURCE_DELETED:
        return "OC_STACK_RESOURCE_DELETED";
    case OC_STACK_CONTINUE:
        return "OC_STACK_CONTINUE";
    case OC_STACK_RESOURCE_CHANGED:
        return "OC_STACK_RESOURCE_CHANGED";
    case OC_STACK_INVALID_URI:
        return "OC_STACK_INVALID_URI";
    case OC_STACK_INVALID_QUERY:
        return "OC_STACK_INVALID_QUERY";
    case OC_STACK_INVALID_IP:
        return "OC_STACK_INVALID_IP";
    case OC_STACK_INVALID_PORT:
        return "OC_STACK_INVALID_PORT";
    case OC_STACK_INVALID_CALLBACK:
        return "OC_STACK_INVALID_CALLBACK";
    case OC_STACK_INVALID_METHOD:
        return "OC_STACK_INVALID_METHOD";
    case OC_STACK_INVALID_PARAM:
        return "OC_STACK_INVALID_PARAM";
    case OC_STACK_INVALID_OBSERVE_PARAM:
        return "OC_STACK_INVALID_OBSERVE_PARAM";
    case OC_STACK_NO_MEMORY:
        return "OC_STACK_NO_MEMORY";
    case OC_STACK_COMM_ERROR:
        return "OC_STACK_COMM_ERROR";
    case OC_STACK_TIMEOUT:
        return "OC_STACK_TIMEOUT";
    case OC_STACK_ADAPTER_NOT_ENABLED:
        return "OC_STACK_ADAPTER_NOT_ENABLED";
    case OC_STACK_NOTIMPL:
        return "OC_STACK_NOTIMPL";
    case OC_STACK_NO_RESOURCE:
        return "OC_STACK_NO_RESOURCE";
    case OC_STACK_RESOURCE_ERROR:
        return "OC_STACK_RESOURCE_ERROR";
    case OC_STACK_SLOW_RESOURCE:
        return "OC_STACK_SLOW_RESOURCE";
    case OC_STACK_DUPLICATE_REQUEST:
        return "OC_STACK_DUPLICATE_REQUEST";
    case OC_STACK_NO_OBSERVERS:
        return "OC_STACK_NO_OBSERVERS";
    case OC_STACK_OBSERVER_NOT_FOUND:
        return "OC_STACK_OBSERVER_NOT_FOUND";
    case OC_STACK_VIRTUAL_DO_NOT_HANDLE:
        return "OC_STACK_VIRTUAL_DO_NOT_HANDLE";
    case OC_STACK_INVALID_OPTION:
        return "OC_STACK_INVALID_OPTION";
    case OC_STACK_MALFORMED_RESPONSE:
        return "OC_STACK_MALFORMED_RESPONSE";
    case OC_STACK_INVALID_REQUEST_HANDLE:
        return "OC_STACK_INVALID_REQUEST_HANDLE";
    case OC_STACK_INVALID_DEVICE_INFO:
        return "OC_STACK_INVALID_DEVICE_INFO";
    case OC_STACK_INVALID_JSON:
        return "OC_STACK_INVALID_JSON";
    case OC_STACK_UNAUTHORIZED_REQ:
        return "OC_STACK_UNAUTHORIZED_REQ";
    case OC_STACK_TOO_LARGE_REQ:
        return "OC_STACK_TOO_LARGE_REQ";
    #ifdef WITH_PRESENCE
    case OC_STACK_PRESENCE_STOPPED:
        return "OC_STACK_PRESENCE_STOPPED";
    case OC_STACK_PRESENCE_TIMEOUT:
        return "OC_STACK_PRESENCE_TIMEOUT";
    case OC_STACK_PRESENCE_DO_NOT_HANDLE:
        return "OC_STACK_PRESENCE_DO_NOT_HANDLE";
    #endif
    case OC_STACK_USER_DENIED_REQ:
        return "OC_STACK_USER_DENIED_REQ";
    case OC_STACK_NOT_ACCEPTABLE:
        return "OC_STACK_NOT_ACCEPTABLE";
    case OC_STACK_FORBIDDEN_REQ:
        return "OC_STACK_FORBIDDEN_REQ";
    case OC_STACK_INTERNAL_SERVER_ERROR:
        return "OC_STACK_INTERNAL_SERVER_ERROR";
    case OC_STACK_GATEWAY_TIMEOUT:
        return "OC_STACK_GATEWAY_TIMEOUT";
    case OC_STACK_SERVICE_UNAVAILABLE:
        return "OC_STACK_SERVICE_UNAVAILABLE";
    case OC_STACK_ERROR:
        return "OC_STACK_ERROR";
    default:
        return "UNKNOWN";
    }
}

void StripNewLineChar(char* str) {
    int i = 0;
    if (str)
    {
        while( str[i])
        {
            if (str[i] == '\n')
                str[i] = '\0';
            i++;
        }
    }
}

