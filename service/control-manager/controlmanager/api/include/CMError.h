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



#ifndef CMError_H_
#define CMError_H_

#include "CMConfig.h"
#include "CMErrorCode.h"
#include "string"

namespace OC
{
    namespace Cm
    {

        /**
        *  @addtogroup ControlManager
        *  @{
        */

        /**
         *
         * @class       CMError
         * @brief       represents an error object
         *
         * @since       1.0
         *
         *
         * This class represents an error object
         *
         */
        class CMAPI CMError
        {

            public:

                /**
                 * Constructor of this class
                 *
                 */
                CMError();

                /**
                 * Default destructor of this class
                 */
                virtual ~CMError();

                /**
                 * returns error code
                 *
                 * @return error code
                 */
                ErrorCode getErrorCode() ;

                /**
                 * returns error description
                 *
                 * @return error description
                 */
                std::string getErrorMessage();

                /**
                 * sets error code
                 *
                 * @param[in] errorCode error code
                 */
                CMError setErrorCode(ErrorCode errorCode);

                /**
                 * sets error description
                 *
                 * @param[in]  message error description
                 */
                CMError setErrorMessage(std::string message);

                /**
                 * overloaded == operator.
                 */
                bool operator==( bool other ) const;

            private:

                ErrorCode   m_code ;

                std::string m_message ;
        };

        /** @} */

    } /* namespace Cm */
} /* namespace OC */



#endif /* CMError_H_ */
