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
 * This file contains the declaration of classes and its members related to
 * RemoteBinarySwitchResource.
 */

#ifndef SMARTHOME_API_CLIENT_REMOTEBINARYSWITCH_H_
#define SMARTHOME_API_CLIENT_REMOTEBINARYSWITCH_H_

#include <SHBaseRemoteResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions for callback of
             * RemoteBinarySwitchResource class.
             */
            class RemoteBinarySwitchResourceDelegate
            {
            public:
                /**
                 * Virtual destructor
                 */
                virtual ~RemoteBinarySwitchResourceDelegate() {}

                /**
                 * API to handle a response about turn on request
                 *
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onTurnOn(ResultCode ret) = 0;

                /**
                 * API to handle a response about turn off request
                 *
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onTurnOff(ResultCode ret) = 0;

                /**
                 * API to handle a response about get state request
                 *
                 * @param[in] value response value from server-side binary switch resource
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onGetState(bool value, ResultCode ret) = 0;
            };

            /**
             * This class contains a set of functions to control BinarySwitch resource.
             */
            class RemoteBinarySwitchResource: public SHBaseRemoteResource,
                    public SHBaseRemoteResourceDelegate
            {
            friend class SHBaseRemoteResourceBuilder;
            public:

                virtual ~RemoteBinarySwitchResource();

                /**
                 * API to set delegate of RemoteBinarySwitchResource
                 *
                 * @param[in] delegate  Instance that inherit RemoteBinarySwitchResourceDelegate
                 * class
                 */
                void setRemoteBinarySwitchResourceDelegate(
                        RemoteBinarySwitchResourceDelegate *delegate);

                /**
                 * API to turn on the remote binary switch
                 */
                void on();

                /**
                 * API to turn off the remote binary switch
                 */
                void off();

                /**
                 * API to get the status of remote binary switch
                 */
                void getState();

            protected:
                RemoteBinarySwitchResource();

            private:
                virtual void onGet(PropertyBundle bundle, ResultCode ret);
                virtual void onSet(PropertyBundle bundle, ResultCode ret);
                virtual void onObserve(PropertyBundle bundle, const ObserveResponse obsType,
                        ResultCode ret);

            private:
                RemoteBinarySwitchResourceDelegate *m_delegate;
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_REMOTEBINARYSWITCH_H_ */
