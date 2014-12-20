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



#ifndef FLICKERLESSTYPE_H_
#define FLICKERLESSTYPE_H_

/**
*  @addtogroup xsd
*  @{
*/
/**
 * @class FlickerlessType
 *
 * The enumeration set of flickerless type which the camera device supports.
 */
class FlickerlessType
{
    public:

        /**
         * Variable that contains FlickerlessType type value.
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
            if (value == "F50") {}
            else if (value == "F60") {}
            else if (value == "Auto") {}
            else
                return false;
            return true;
        }
};
/** @} */
#endif
