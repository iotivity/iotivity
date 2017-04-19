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
 * This file contains the declaration of classes and its members related to DeviceDiscovery.
 * DeviceDiscovery provides device-level discovery that devices with resources are found.
 */

#ifndef SMARTHOME_API_CLIENT_DEVICEDISCOVERY_H_
#define SMARTHOME_API_CLIENT_DEVICEDISCOVERY_H_

#include <list>
#include <DiscoveryQuery.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class SHBaseRemoteDevice;

            /**
             * @class DeviceDiscoveryDelegate
             *
             * @brief This class contains a set of functions for callback of
             * DeviceDiscoveryDelegate class.
             */
            class DeviceDiscoveryDelegate
            {
            public:
                /**
                 * API to handle the response when remote devices are found
                 *
                 * @param[in] remoteDevice information of a remote device
                 */
                virtual void onFindRemoteDevice(SHBaseRemoteDevice *remoteDevice) = 0;

                /**
                 * API to handle the response when remote devices are found
                 *
                 * @param[in] remoteDevice information of remote devices
                 */
                virtual void onFindRemoteDevice(std::list< SHBaseRemoteDevice* > remoteDevice) = 0;
            };

            /**
             * @class DeviceDiscovery
             *
             * @brief This class contains a set of functions for discovering remote devices
             */
            class DeviceDiscovery
            {
            public:
                DeviceDiscovery();
                ~DeviceDiscovery();

                /**
                 * API to set a delegate instance
                 *
                 * @param[in] delegate instance which is in charge of handling event
                 */
                void setDelegate(DeviceDiscoveryDelegate *delegate);

                /**
                 * API to find remote devices
                 * This api return a response whenever a remote device is found
                 */
                void findRemoteDevices();

                /**
                 * API to find remote devices using query
                 * This api return a response whenever a remote device is found
                 *
                 * @param[in] query instance which includes discovery query
                 */
                void findRemoteDevices(const DiscoveryQuery &query);

                /**
                 * API to find remote devices.
                 * This api returns a response only once when all remote devices is found
                 * within a certain time
                 */
                void findAllRemoteDevices();

                /**
                 * API to find remote devices using query.
                 * This api returns a response only once when all remote devices is found
                 * within a certain time
                 *
                 * @param[in] query instance which includes discovery query
                 */
                void findAllRemoteDevices(const DiscoveryQuery &query);

            private:
                DeviceDiscoveryDelegate *m_delegate;
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_DEVICEDISCOVERY_H_ */
