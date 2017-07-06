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
 * This file contains the declaration of classes and its members related to DiscoveryQuery.
 */

#ifndef SMARTHOME_API_CLIENT_DISCOVERYQUERY_H_
#define SMARTHOME_API_CLIENT_DISCOVERYQUERY_H_

#include <string>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions for setting option for discovering
             * remote devices.
             */
            class DiscoveryQuery
            {
            public:
                DiscoveryQuery();
                virtual ~DiscoveryQuery();

                /**
                 * API to set a device type
                 *
                 * @param[in] deviceType type of devices which want to find
                 */
                void setDeviceType(const std::string &deviceType);

                /**
                 * API to get a device type
                 *
                 * @return device type
                 */
                std::string getDeviceType() const;

            private:
                std::string m_deviceType;
                std::string m_hostAddress;
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_DISCOVERYQUERY_H_ */
