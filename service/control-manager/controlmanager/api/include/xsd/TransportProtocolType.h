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



#ifndef TRANSPORTPROTOCOLTYPE_H_
#define TRANSPORTPROTOCOLTYPE_H_

/**
*  @addtogroup xsd
*  @{
*/
/**
 * @class TransportProtocolType
 *
 * The enumeration set of transport protocol type which the device supports.
 */
class TransportProtocolType
{
    public:

        /**
         * Variable that contains TransportProtocolType type value.
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
            if (value == "RTSP") {}
            else if (value == "HTTP") {}
            else
                return false;
            return true;
        }
};
/** @} */
#endif
