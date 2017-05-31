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
            class SHBaseDevice_Impl;
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
                std::string getType() const;

                /**
                 * API to set type of device.
                 *
                 * @param[in] type type of device.
                 */
                void setType(const std::string& type);

                /**
                 * API to get name of device
                 *
                 * @return name of device
                 */
                std::string getName() const;

                /**
                 * API to set name of device.
                 *
                 * @param[in] name name of device.
                 */
                void setName(const std::string& name);

                /*
                 * API to start presence of resources that have the device.
                 *
                 * @param[in] ttl time to live (seconds)
                 * @par
                 * If ttl is '0', then the default value will be used (60 Seconds).
                 * If ttl is greater than OC_MAX_PRESENCE_TTL_SECONDS, then the ttl will be set to
                 * OC_MAX_PRESENCE_TTL_SECONDS.
                 * @par
                 */
                void startPresence(const unsigned int ttl);

                /**
                 * API to stop presence annonuncements.
                 */
                void stopPresence();

            public:
                SHBaseDevice();
                virtual ~SHBaseDevice();

            private:
                std::string m_deviceType;
                SHBaseDevice_Impl *m_deviceImpl;
            };
        }
    }
}
#endif /* SMARTHOME_API_SERVER_SHBASEDEVICE_H_ */
