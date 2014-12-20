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



#ifndef WIFIEVENTTYPE_H_
#define WIFIEVENTTYPE_H_

/**
*  @addtogroup xsd
*  @{
*/
/**
 * @class WifiEventType
 *
 * The enumeration set of event type which the WiFi module supports.
 */
class WifiEventType
{
    public:

        /**
         * Variable that contains WifiEventType type value.
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
            if (value == "Started") {}
            else if (value == "Connected") {}
            else if (value == "Disconnected") {}
            else
                return false;
            return true;
        }
};
/** @} */
#endif
