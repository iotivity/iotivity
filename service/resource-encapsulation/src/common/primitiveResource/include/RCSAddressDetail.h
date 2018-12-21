/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef OIC_SERVICE_RCSADDRESSDETAIL_H
#define OIC_SERVICE_RCSADDRESSDETAIL_H

#include <string>

/** OIC namespace */
namespace OIC
{
    /** service namespace */
    namespace Service
    {
        class RCSAddress;

        class RCSAddressDetail
        {
        public:
            /** API is to get rcs details */
            static const RCSAddressDetail* getDetail(const RCSAddress&);

            RCSAddressDetail(const std::string& addr);
            RCSAddressDetail(std::string&& addr);

            const std::string& getAddress() const;

            /** API is to check multicast */
            bool isMulticast() const;

        private:
            std::string m_addr;
        };

    } /** service namespace */
} /** OIC namespace */

#endif // OIC_SERVICE_RCSADDRESSDETAIL_H
