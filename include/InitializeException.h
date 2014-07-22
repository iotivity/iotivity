//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef _INITIALIZE_EXCEPTION_H_
#define _INITIALIZE_EXCEPTION_H_

#include <stdexcept>
#include <ocstack.h>

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
            case OC_STACK_ERROR:
                return "General Fault";
            default:
                return "Unknown Error";
            }
        }

    private:
        const std::string& m_errorMessage;
        OCStackResult m_reason;
    };
}

#endif