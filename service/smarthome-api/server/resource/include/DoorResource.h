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

            class DoorResourceDelegate
            {
            public:
                virtual ~DoorResourceDelegate() {}

                virtual ResultCode onActivateOpenAlarm() = 0;
                virtual ResultCode onDeactivateOpenAlarm() = 0;
            };

            class DoorResource: public SHBaseResource, public SHBaseResourceDelegate
            {
            public:
                DoorResource();
                virtual ~DoorResource();

                bool changeOpenState(DoorState state);
                DoorState getOpenState();

                bool changeOpenAlarmState(OpenAlarmState state);
                OpenAlarmState getOpenAlarmState();
                
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
