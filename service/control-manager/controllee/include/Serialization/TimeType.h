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

#ifndef TIMETYPE_H_
#define TIMETYPE_H_

namespace OC
{
    namespace Cm
    {
        namespace Serialization
        {
            namespace Xsd
            {

                /**
                 *
                 * @class       TimeType
                 * @brief       Defines a TimeType
                 *
                 * @since       1.0
                 *
                 * This class defines and validates a TimeType defined by shp application profile
                 * Format of the TimeType : [hh]:[mm]:[ss]
                 *
                 */

                class TimeType
                {

                    public:

                        std::string     value;  /**< Represents the time value as string format */
                        /**
                         * This API validates that the TimeType set by SHP application profile conforms to the correct format.
                         * Format of the TimeType : [hh]:[mm]:[ss]
                         *
                         * @return  returns @c true in case set TimeType is in correct format
                         *                  @c false in case of any error
                         */
                        bool validateContent() ;
                };

            }
        }
    }
}


#endif /* TIMETYPE_H_ */
