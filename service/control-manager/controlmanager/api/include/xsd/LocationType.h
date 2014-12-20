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



#ifndef LOCATIONTYPE_H_
#define LOCATIONTYPE_H_

/**
*  @addtogroup xsd
*  @{
*/
/**
 * @class LocationType
 *
 * The enumeration set of locations where the product is shipped.
 */
class LocationType
{
    public:

        /**
         * Variable that contains LocationType type value.
         */
        std::string value;

        /**
         * Check whether the value is following the rules or not.
         *
         * @return @c True if the value is valid@n
         *         @c False if the value is not valid
         */
        bool validateContent()
        {
            if (value == "Unknown") {}
            else if (value == "World_Wide") {}
            else if (value == "Asia") {}
            else if (value == "North_America") {}
            else if (value == "South_America") {}
            else if (value == "Europe") {}
            else if (value == "Middle_East") {}
            else if (value == "Africa") {}
            else if (value == "Oceania") {}
            else
                return false;
            return true;
        }
};
/** @} */
#endif
