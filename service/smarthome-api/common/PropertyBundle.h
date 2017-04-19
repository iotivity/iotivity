/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
#ifndef SMARTHOME_API_COMMON_PROPERTYBUNDLE_H_
#define SMARTHOME_API_COMMON_PROPERTYBUNDLE_H_

#include <PropertyBundle_Impl.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /*
             * Container class to store multiple properties.
             */
            class PropertyBundle: public PropertyBundle_Impl
            {
            public:
                template <typename T>
                bool getValue(const std::string& key, T& val) const
                {
                    return PropertyBundle_Impl::getValue(key, val);
                }

                template <typename T>
                bool setValue(const std::string& key, T& val)
                {
                    return PropertyBundle_Impl::setValue(key, val);
                }
            };
        }
    }
}

#endif /* SMARTHOME_API_COMMON_PROPERTYBUNDLE_H_ */
