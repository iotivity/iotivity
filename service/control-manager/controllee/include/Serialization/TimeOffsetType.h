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

#ifndef TIMEOFFSETTYPE_H_
#define TIMEOFFSETTYPE_H_

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
                 * @class       TimeOffsetType
                 * @brief       Defines a TimeOffsetType
                 *
                 * @since       1.0
                 *
                 * This class defines and validates a TimeOffsetType defined by shp application profile. TimeOffset is the offset of a particular time zone to the standard time.
                 * Format of the TimeOffsetType :+[hh]:[mm] or -[hh]:[mm]
                 *
                 */
                class TimeOffsetType
                {

                    public:

                        std::string     value;  /**< Represents the time value as string format */
                        /**
                         * This API validates that the TimeOffsetType set by SHP application profile conforms to the correct format.
                         * Format of the TimeOffsetType :+[hh]:[mm] or -[hh]:[mm]
                         *
                         * @return  returns @c true in case set TimeOffsetType is in correct format
                         *                  @c false in case of any error
                         */
                        bool validateContent() ;
                };

            } /* namespace Xsd */
        } /* namespace Serialization */
    } /* namespace Cm */
} /* namespace OC */
#endif /* TIMEOFFSETTYPE_H_ */
