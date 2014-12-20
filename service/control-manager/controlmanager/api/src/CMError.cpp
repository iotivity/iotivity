//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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



#include "CMError.h"

namespace OC
{
    namespace Cm
    {

        CMError::CMError()
        {
            m_code = CM_SUCCESS;
        }

        CMError::~CMError()
        {

        }

        CMError
        CMError::setErrorCode(ErrorCode errorCode)
        {
            m_code = errorCode;
            return *this;
        }

        CMError
        CMError::setErrorMessage(std::string message)
        {
            m_message = message;
            return *this;
        }

        ErrorCode
        CMError::getErrorCode()
        {
            return m_code;
        }

        std::string
        CMError::getErrorMessage()
        {
            return m_message;
        }

        bool
        CMError::operator==( bool other ) const
        {
            if ((m_code != CM_SUCCESS) && (false == other))
            {
                return true;
            }

            if ((m_code == CM_SUCCESS) && (true == other))
            {
                return true;
            }

            return false;
        }


    } /* namespace Cm */
} /* namespace OC */
