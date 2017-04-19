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

/**
 * @file
 *
 * This file contains the declaration of a class and its members related to SHBaseDevice.
 * SHBaseDevice provides server-side APIs to handle requests from client.
 */

#ifndef SMARTHOME_API_SERVER_SHBASEDEVICE_H_
#define SMARTHOME_API_SERVER_SHBASEDEVICE_H_

#include <string>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
            * @class SHBaseDevice
            *
            * @brief This class contains a set of functions to define device and
            * handle client requests.
            */
            class SHBaseDevice
            {
            public:
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

            public:
                SHBaseDevice();
                virtual ~SHBaseDevice();

            private:
                std::string m_deviceType;
            };
        }
    }
}
#endif /* SMARTHOME_API_SERVER_SHBASEDEVICE_H_ */
