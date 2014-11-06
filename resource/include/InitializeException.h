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

#ifndef _INITIALIZE_EXCEPTION_H_
#define _INITIALIZE_EXCEPTION_H_

#include <stdexcept>
#include <ocstack.h>
#include "StringConstants.h"

namespace OC
{
    class InitializeException : public std::exception
    {
    public:
        InitializeException(const std::string& msg, OCStackResult reasonCode): m_errorMessage(msg), m_reason(reasonCode)
        {
        }

        OCStackResult ReasonCode()
        {
            return m_reason;
        }

        std::string Message()
        {
            return m_errorMessage;
        }

        std::string Reason()
        {
            switch(m_reason)
            {
            case OC_STACK_OK:
                return OC::InitException::NO_ERROR;
            case OC_STACK_INVALID_URI:
                return OC::InitException::INVALID_URI;
            case OC_STACK_INVALID_IP:
                return OC::InitException::INVALID_IP;
            case OC_STACK_INVALID_PORT:
                return OC::InitException::INVALID_PORT;
            case OC_STACK_INVALID_CALLBACK:
                return OC::InitException::INVALID_CB;
            case OC_STACK_INVALID_METHOD:
                return OC::InitException::INVALID_METHOD;
            case OC_STACK_ERROR:
                return OC::InitException::GENERAL_FAULT;
            default:
                return OC::InitException::UNKNOWN_ERROR;
            }
        }

    private:
        const std::string& m_errorMessage;
        OCStackResult m_reason;
    };
}

#endif
