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

#ifndef DATETIMETYPE_H_
#define DATETIMETYPE_H_

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
                 * @class       DateTimeType
                 * @brief       Defines a DateTimeType
                 *
                 * @since       1.0
                 *
                 * This class defines and validates a DateTimeType defined by shp application profile
                 * Format of the DateTimeType :[yyyy]-[mm]-[dd]T[hh]:[mm]:[ss]
                 *
                 */
                class DateTimeType
                {

                    public:

                        std::string     value;  /**< Represents date time as string version */

                        /**
                         * This API validates that the DateTime set by SHP application profile conforms to the correct format.
                         * Format of the DateTimeType :[yyyy]-[mm]-[dd]T[hh]:[mm]:[ss]
                         *
                         * @return  returns @c true in case set DateTime is in correct format
                         *                  @c false in case of any error
                         */
                        bool validateContent() ;
                };

            }
        }
    }
}


#endif /* DATETIMETYPE_H_ */
