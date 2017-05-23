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
#ifndef SMARTHOME_API_CLIENT_SHBASEREMOTEDEVICE_H_
#define SMARTHOME_API_CLIENT_SHBASEREMOTEDEVICE_H_

#include <string>
#include <list>

/**
 * @file
 *
 * This file contains the declaration of classes and its members related to
 * SHBaseRemoteDevice.
 */

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class SHBaseRemoteDevice_Impl;
            class SHBaseRemoteResource;

            /**
            * @class SHBaseRemoteDevice
            *
            * @brief This class contains a set of functions to control remote device that is found
            * in device discovery step. SHBaseRemoteDevice provides client-side APIs to communicate
            * with devices with resources.
            */
            class SHBaseRemoteDevice
            {
            public:
                virtual ~SHBaseRemoteDevice();

                /**
                 * API to get type of device
                 *
                 * @return type of device
                 */
                std::string getType();

                /**
                 * API to get list of remote resources.
                 *
                 * @param[in] resourceType Type of remote resource
                 * @return list of resources
                 */
                std::list< SHBaseRemoteResource* > getResourceWithResourceType(
                                                   const std::string &resourceType);

                /**
                 * API to get remote resource by Uri.
                 *
                 * @param[in] resourceUri Uri of remote resource
                 * @return found remote resource
                 */
                SHBaseRemoteResource* getResourceWithResourceUri(const std::string &resourceUri);

            protected:
                SHBaseRemoteDevice();

            private:
                SHBaseRemoteDevice_Impl *m_remoteDeviceImpl;
            };
        }
    }
}
#endif /* SMARTHOME_API_CLIENT_SHBASEREMOTEDEVICE_H_ */
