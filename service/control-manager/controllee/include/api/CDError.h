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

#ifndef CDError_H_
#define CDError_H_

#include "CDErrorCode.h"
#include "string"

namespace OC
{
    namespace Cm
    {

        /**
         *
         * @class       CDError
         * @brief       represents an error object
         * This class represents an error object
         *
         */
        class CDError
        {

            public:

                /** Default constructor
                 * @param - none
                 * @return -none
                 */
                CDError();

                /**
                 * Default destructor
                 * @param - none
                 * @return -none
                 */
                virtual ~CDError();

                /** getErrorCode - Get the error code for the object.
                 * @param - none
                 * @return -ErrorCode-The ErrorCode of that was set for this object.
                 *          If not set, then the default error Code.(SHP_SUCCESS)
                 */
                ErrorCode getErrorCode();

                /* getErrorMessage - Get the error message set for the object.
                 * @param - none
                 * @return -string - The Error message of that was set for this object.
                 *          If not set, then string of 0 length is returned
                 */
                std::string getErrorMessage();

                /** setErrorCode - Set the Error code for the object.
                 * @param - errorCode - The error code of type ErrorCode
                 * @return -The object itself after the error code is set
                 */
                CDError setErrorCode(ErrorCode errorCode);

                /** setErrorMessage - Set the Error message for the object.
                 * @param - errorMessage - The error message.
                 * @return -CDError- The object itself
                 */
                CDError setErrorMessage(std::string message);

                /** operator== - If m_code is SHP_SUCESS then test with the input
                 * @param - bool - other
                 * @return -bool - Ture if other is true, else false
                 */
                bool operator==(bool other) const;

            private:

                ErrorCode m_code; //Error Code
                std::string m_message; //Error message
        };

    } /* namespace Cm */
} /* namespace OC */

#endif /* CMError_H_ */
