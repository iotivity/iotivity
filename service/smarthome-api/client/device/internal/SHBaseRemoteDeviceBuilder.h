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
#ifndef SMARTHOME_API_CLIENT_SHBASEREMOTEDEVICE_BUILDER_H_
#define SMARTHOME_API_CLIENT_SHBASEREMOTEDEVICE_BUILDER_H_

#include <string>
#include <list>
#include <ClientCallbackWrapper.h>
#include "octypes.h"

/**
 * @file
 *
 * This file contains the declaration of classes and its members related to
 * SHBaseRemoteDeviceBuilder.
 */
namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class SHBaseRemoteDevice;

            /**
             * @class SHBaseRemoteDeviceBuilder
             *
             * @brief This class contains a set of functions to make SHBaseRemoteDevice.
             */
            class SHBaseRemoteDeviceBuilder
            {
            public:
                /**
                 * API to create SHBaseRemoteDevice.
                 * SHBaseRemoteDevice and all classes that inherit it needs to be created
                 * in ClientWrapper class logic.
                 * Also, SHBaseRemoteDevice and all classes that inherit it has protected
                 * constructor. To construct in this class, SHBaseRemoteDevice and all classes
                 * that inherit it should declare this class to friend class.
                 *
                 * @param[in] payload  Payload from csdk.
                 * @return SHBaseRemoteDevice*  created device pointer
                 */
                static SHBaseRemoteDevice* createSHBaseRemoteDevice(
                        OCDiscoveryPayload *payload);

                /**
                 * API to Mapping specific resource to Device member resource.
                 *
                 * @param[in,out] device    Device for mapping specific resource.
                 * @param[in]     resource  Resource for classification.
                 * @return  true if resource is mapping successfully in device.
                 */
                static bool mappingResource(SHBaseRemoteDevice *device,
                                            SHBaseRemoteResource *resource);
            private:
                /**
                 * API to convert OCStringLL to std::list.
                 *
                 * @param[in] ll OCStringLL to be converted.
                 * @return std::list<std::string> converted list.
                 */
                static std::list<std::string> convertStringLLToList(OCStringLL *ll);

                /**
                 * API to make SHBaseRemoteDevice according to device type.
                 *
                 * @param[in] deviceResource  resource that contain device
                 *                            information.(/oic/d)
                 * @param[in] deviceId        device ID.
                 * @return SHBaseRemoteDevice*  created device pointer
                 */
                static SHBaseRemoteDevice* makeSHBaseRemoteDevice(
                    OCResourcePayload *deviceResource, char *deviceId);
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_SHBASEREMOTEDEVICE_BUILDER_H_ */
