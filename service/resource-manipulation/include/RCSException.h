//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

/**
 * @file
 *
 * This file defines a class to handle exception thrown for resource manipulation.
 */

#ifndef RES_MANIPULATION_RCSEXCEPTION_H
#define RES_MANIPULATION_RCSEXCEPTION_H

#include <string>

#include <octypes.h>

namespace OIC
{
    namespace Service
    {

        /**
         * @class   RCSException
         * @brief    This class helps to throw wide range of exception to the application/developers.
         *               It inherits the standard exception class.
         *
         */
        class RCSException: public std::exception
        {
            public:
                /**
                * Default Constructor
                */
                RCSException();

                /**
                * Parametrized Constructor  to set exception description as a string.
                *
                * @param what - exception description
                */
                explicit RCSException(const std::string &what);

                /**
                * Parametrized Constructor  to set exception description as a string.
                *
                * @param what - exception description
                */
                explicit RCSException(std::string &&what);

                /**
                * virtual destructor
                */
                virtual ~RCSException() noexcept {}

                /**
                * API for returning the exception description in string format
                *
                * @return  const char* - exception description
                */
                virtual const char *what() const noexcept;

            private:
                /**
                 *  Exception description
                 */
                std::string m_what;
        };

        /**
         * @class   PlatformException
         * @brief   This class helps in throwing platform exception to the application/developers.
         *              It inherits from RCSException class.
         *
         * NOTE: OCStackResult is defined in octypes.h.
         */
        class PlatformException: public RCSException
        {
            public:
                explicit PlatformException(OCStackResult reason);

                /**
                 * API for getting exception code.
                 *
                 * @return  OCStackResult - exception code.
                 *
                 */
                OCStackResult getReasonCode() const;
                /**
                 * API for getting exception reason.
                 *
                 * @return string - exception reason as a string.
                 *
                 */
                std::string getReason() const;

            private:
                /*
                * reason for the exception, stored as OCStackResult value.
                */
                OCStackResult m_reason;
        };
    }
}

#endif // RES_MANIPULATION_RCSEXCEPTION_H
