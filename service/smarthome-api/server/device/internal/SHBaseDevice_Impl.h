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
#ifndef SMARTHOME_API_SERVER_SHBASEDEVICE_IMPL_H_
#define SMARTHOME_API_SERVER_SHBASEDEVICE_IMPL_H_

#include <string>
#include <list>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class SHBaseResource;

            class SHBaseDevice_Impl
            {
            friend class SHBaseDevice;

            protected:
                std::string getType();

                void setType(std::string type);

                std::string getName();

                void setName(std::string name);

            public:
                SHBaseDevice_Impl();
                virtual ~SHBaseDevice_Impl();

            private:
                std::string m_deviceType;
                std::string m_deviceName;
            };
        }
    }
}

#endif /* SMARTHOME_API_SERVER_SHBASEDEVICE_IMPL_H_ */
