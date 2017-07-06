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
#ifndef SMARTHOME_API_SERVER_BINARYSWITCH_H_
#define SMARTHOME_API_SERVER_BINARYSWITCH_H_

#include <SHBaseResource.h>

/**
 * This file contains the declaration of classes and its members related to
 * BinarySwitchResource.
 */

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of callback functions handling events and requests
             * for BinarySwitchResource class.
             */
            class BinarySwitchResourceDelegate
            {
            public:
                /**
                 * Virtual destructor
                 */
                virtual ~BinarySwitchResourceDelegate() {}

                /**
                 * API to handle turn on request
                 *
                 * @return Returns ::SUCCESS if success
                 */
                virtual ResultCode turnOnCallback() = 0;

                /**
                 * API to handle turn off request
                 *
                 * @return Returns ::SUCCESS if success
                 */
                virtual ResultCode turnOffCallback() = 0;
            };

            /**
             * This class contains a set of functions to change the properties of BinarySwitch.
             */
            class BinarySwitchResource: public SHBaseResource, public SHBaseResourceDelegate
            {
            public:
                BinarySwitchResource();
                virtual ~BinarySwitchResource();

                /**
                 * API to get the status of the switch
                 *
                 * @return true if the switch is on
                 */
                bool getState();

                /**
                 * API to set the status of the switch
                 *
                 * @param[in] state Status of the switch
                 */
                void setState(bool state);

                /**
                 * API to set delegate of BinarySwitchResource
                 *
                 * @param[in] switchDelegate Instance that inherit BinarySwitchResourceDelegate
                 * class
                 */
                void setDelegate(BinarySwitchResourceDelegate *delegate);

            private:
                virtual ResultCode onGet(RequestId requestId, const ResourceQuery& query);
                virtual ResultCode onSet(RequestId requestId, const PropertyBundle& bundle,
                                         const ResourceQuery& query);

                BinarySwitchResourceDelegate *m_delegate;
            };
        }
    }
}

#endif /* SMARTHOME_API_SERVER_BINARYSWITCH_H_ */
