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

#include "CDError.h"

namespace OC
{
    namespace Cm
    {

        /*
         * Default constructor
         */
        CDError::CDError()
        {
            m_code = SHP_SUCCESS;
            m_message = "";
        }

        /*
         *
         *  Default destructor
         *
         */
        CDError::~CDError()
        {

        }

        /*
         * setErrorCode - Set the Error code for the object.
         */
        CDError CDError::setErrorCode(ErrorCode errorCode)
        {
            m_code = errorCode;
            return *this;
        }

        /*
         * setErrorMessage - Set the Error message for the object.
         */
        CDError CDError::setErrorMessage(std::string message)
        {
            m_message = message;
            return *this;
        }

        /*
         *  getErrorCode - Get the error code for the object.
         */
        ErrorCode CDError::getErrorCode()
        {
            return m_code;
        }

        /*
         * getErrorMessage - Get the error message set for the object.
         */
        std::string CDError::getErrorMessage()
        {
            return m_message;
        }

        /*
         * operator== - If m_code is SHP_SUCESS then test with the input
         */
        bool CDError::operator==(bool other) const
        {
            if ((m_code != SHP_SUCCESS) && (false == other))
            {
                return true;
            }

            if ((m_code == SHP_SUCCESS) && (true == other))
            {
                return true;
            }

            return false;
        }

    } /* namespace Cm */
} /* namespace OC */
