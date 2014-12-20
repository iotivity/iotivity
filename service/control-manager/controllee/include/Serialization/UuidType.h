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

#ifndef UUIDTYPE_H_
#define UUIDTYPE_H_

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
                 * @class       UuidType
                 * @brief       Defines a UuidType
                 *
                 * @since       1.0
                 *
                 * This class defines and validates a uuid defined by shp application profile
                 * Format of the UUID : 8-4-4-4-12 (36 characters) where each number represents the number of characters.
                 *
                 */
                class UuidType
                {

                    public:

                        std::string     value;  /**< Represents the value of the UUID type */
                        /**
                         * This API validates that the uuid set by SHP application profile conforms to the correct format.
                         *  Format of the UUID : 8-4-4-4-12 (36 characters) where each number represents the number of characters.
                         *
                         * @return  returns @c true in case set uuid is in correct format@n
                         *                  @c false in case of any error
                         */
                        bool validateContent() ;
                };

            }
        }
    }
}


#endif /* UUIDTYPE_H_ */
