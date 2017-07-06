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
 * This file contains the declaration of classes and its members related to DoorResource.
 */

#ifndef SMARTHOME_API_SERVER_DOOR_H_
#define SMARTHOME_API_SERVER_DOOR_H_

#include <SHBaseResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            enum DoorState
            {
                OPEN = 0, CLOSED
            };

            enum OpenAlarmState
            {
                DEACTIVATE = 0, ACTIVATE
            };

            /**
             * This class contains a set of functions for callback of
             * DoorResource class.
             */
            class DoorResourceDelegate
            {
            public:
                virtual ~DoorResourceDelegate() {}

                /**
                 * API to handle OpenAlarm activate request.
                 *
                 * @return ResultCode ::SUCCESS if success
                 */
                virtual ResultCode onActivateOpenAlarm() = 0;

                /**
                 * API to handle OpenAlarm deactivate request.
                 *
                 * @return ResultCode ::SUCCESS if success
                 */
                virtual ResultCode onDeactivateOpenAlarm() = 0;
            };

            /**
             * This class contains a set of functions to change the properties of Door.
             */
            class DoorResource: public SHBaseResource, public SHBaseResourceDelegate
            {
            public:
                DoorResource();
                virtual ~DoorResource();

                /**
                 * API to chanage the openState.
                 *
                 * @return true if success
                 */
                bool changeOpenState(DoorState state);

                /**
                 * API to get the OpenState.
                 *
                 * @return current open state
                 */
                DoorState getOpenState();

                /**
                 * API to change the openAlarm state.
                 *
                 * @return true if success
                 */
                bool changeOpenAlarmState(OpenAlarmState state);

                /**
                 * API to get the openAlarm state.
                 *
                 * @return openAlarm state
                 */
                OpenAlarmState getOpenAlarmState();

                /**
                 * API to set the delegate.
                 *
                 * @param[in] delegate instance of delegate
                 */
                void setDelegate(DoorResourceDelegate* delegate);

            private:
                ResultCode onGet(RequestId requestId, const ResourceQuery& query);
                ResultCode onSet(RequestId requestId, const PropertyBundle& bundle,
                                 const ResourceQuery& query);

                DoorResourceDelegate *m_delegate;
            };
        }
    }
}

#endif /* SMARTHOME_API_SERVER_DOOR_H_ */
