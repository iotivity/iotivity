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

/**
 * @file
 *
 * This file contains the declaration of a class and its members related to
 * SHBaseDevice_Impl.
 */

#include <string>
#include <list>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class SHBaseResource;

            /**
            * @class SHBaseDevice_Impl
            *
            * @brief This class contains a set of base functions to control device.
            */
            class SHBaseDevice_Impl
            {
            friend class SHBaseDevice;
            protected:
                /**
                 * API to get type of device
                 *
                 * @return type of device
                 */
                std::string getType();

                /**
                 * API to set type of device.
                 *
                 * @param[in] type type of device.
                 */
                void setType(std::string type);

                /**
                 * API to get name of device
                 *
                 * @return name of device
                 */
                std::string getName();

                /**
                 * API to set name of device.
                 *
                 * @param[in] name name of device.
                 */
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
