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
#ifndef SMARTHOME_API_CLIENT_CLIENTCALLBACKWRAPPER_H_
#define SMARTHOME_API_CLIENT_CLIENTCALLBACKWRAPPER_H_

#include "octypes.h"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class SHBaseRemoteResource;
            class SHBaseRemoteDevice_Impl;

            namespace ClientCallbackWrapper
            {
                // below functions are callback for OCClientResponseHandler
                namespace DeviceDiscoveryCallback
                {
                    OCStackApplicationResult findRemoteDevices(void *context, OCDoHandle handle,
                                                               OCClientResponse * clientResponse);
                    OCStackApplicationResult findRemoteDevicesWithQuery
                                             (void *context, OCDoHandle handle,
                                             OCClientResponse * clientResponse);
                    OCStackApplicationResult findRemoteDevicesAll
                                             (void *context, OCDoHandle handle,
                                             OCClientResponse * clientResponse);
                    OCStackApplicationResult findRemoteDevicesAllWithQuery
                                             (void *context, OCDoHandle handle,
                                             OCClientResponse * clientResponse);
                }

                namespace RemoteResourceCallback
                {
                    OCStackApplicationResult onGet(void *context, OCDoHandle handle,
                                                   OCClientResponse * clientResponse);
                    OCStackApplicationResult onSet(void *context, OCDoHandle handle,
                                                   OCClientResponse * clientResponse);
                }

                class CallbackHelper
                {
                public:
                    // callback for OCClientContextDeleter
                    static void CallbackContextDeleter(void* context);

                    // TODO
                    // Add callback for obs, presence , more...

                    static void destroyRemoteDevice(SHBaseRemoteDevice_Impl* remoteDevice);

                    static bool setRemoteDevice(SHBaseRemoteDevice_Impl* remoteDevice_Impl,
                                         OCDiscoveryPayload* discoveryPayload);

                    static bool setRemoteResource(SHBaseRemoteResource* remoteResource,
                                           OCResourcePayload* resourcePayload);
                };
            }
        }
    }
}

#endif /*SMARTHOME_API_CLIENT_CLIENTCALLBACKWRAPPER_H_*/