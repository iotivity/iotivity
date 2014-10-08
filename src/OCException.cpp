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

#include "OCException.h"

std::string OC::OCException::reason(const OCStackResult sr)
{
    switch(sr)
    {
        case OC_STACK_OK:
               return "No Error";
        case OC_STACK_INVALID_URI:
            return "Invalid URI";
        case OC_STACK_INVALID_IP:
            return "Invalid IP";
        case OC_STACK_INVALID_PORT:
            return "Invalid Port";
        case OC_STACK_INVALID_CALLBACK:
            return "Invalid Callback";
        case OC_STACK_INVALID_METHOD:
            return "Invalid Method";
        case OC_STACK_INVALID_QUERY:
            return "Invalid Query";
        case OC_STACK_INVALID_PARAM:
            return "Invalid Parameter";
        case OC_STACK_INVALID_OBSERVE_PARAM:
            return "Invalid Observe Parameter";
        case OC_STACK_NO_MEMORY:
            return "No Memory";
        case OC_STACK_COMM_ERROR:
            return "Communication Error";
        case OC_STACK_NOTIMPL:
            return "Not Implemented";
        case OC_STACK_NO_RESOURCE:
            return "Resource Not Found";
        case OC_STACK_RESOURCE_ERROR:
            return "Resource Error";
        case OC_STACK_SLOW_RESOURCE:
            return "Slow Resource";
        case OC_STACK_NO_OBSERVERS:
            return "No Observers";
       case OC_STACK_OBSERVER_NOT_FOUND:
            return "Stack observer not found";
       case OC_STACK_OBSERVER_NOT_ADDED:
            return "Stack observer not added";
       case OC_STACK_OBSERVER_NOT_REMOVED:
            return "Stack observer not removed";
#ifdef WITH_PRESENCE
       case OC_STACK_PRESENCE_STOPPED:
            return "Stack presence stopped";
       case OC_STACK_PRESENCE_DO_NOT_HANDLE:
            return "Stack presence should not be handled";
#endif
       case OC_STACK_INVALID_OPTION:
            return "Invalid option";
       case OC_STACK_MALFORMED_RESPONSE:
            return "Malformed response";
       case OC_STACK_ERROR:
            return "General Fault";
    }

    return "Unknown Error";
}

