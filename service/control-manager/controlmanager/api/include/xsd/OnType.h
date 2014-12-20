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



#ifndef ONTYPE_H_
#define ONTYPE_H_


/**
*  @addtogroup xsd
*  @{
*/
/**
 * @class OnType
 *
 * The enumeration set of On/Off type which the device supports.
 */
class OnType
{
    public:

        /**
         * Variable that contains OnType type value.
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
            if (value == "On") {}
            else if (value == "Off") {}
            else if (value == "Not_Used") {}
            else if (value == "Not_Supported") {}
            else
                return false;
            return true;
        }
};
/** @} */
#endif
