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

#ifndef __INTEL_OCEXCEPTION_H_2014_07_10
 #define __INTEL_OCEXCEPTION_H_2014_07_10

#include <stdexcept>
#include <ocstack.h>
namespace OC {

typedef std::runtime_error reflection_exception;

class OCException : public std::runtime_error
{
    public:
        OCException(const std::string& msg, OCStackResult reason = OC_STACK_ERROR): std::runtime_error(msg),  m_reason(reason) {}
        std::string reason()
        {
            switch(m_reason)
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
                    return "Invalid Param";
                case OC_STACK_INVALID_OBSERVE_PARAM:
                    return "Invalid Observe Param";
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
                case OC_STACK_ERROR:
                    return "General Fault";
                default:
                    return "Unknown Error";
            }
        }

    private:
        OCStackResult m_reason;
};

} // namespace OC

#endif
