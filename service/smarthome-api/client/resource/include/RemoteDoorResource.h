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
#ifndef SMARTHOME_API_CLIENT_REMOTEDOOR_H_
#define SMARTHOME_API_CLIENT_REMOTEDOOR_H_

#include <SHBaseRemoteResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            namespace OpenState
            {
                const std::string Open = "Open";
                const std::string Closed = "Closed";
            }

            enum OpenAlarmState
            {
                DEACTIVATE = 0, ACTIVATE
            };

            class RemoteDoorResourceDelegate
            {
            public:
                virtual ~RemoteDoorResourceDelegate() {}

                virtual void onGetOpenState(std::string state, ResultCode code) = 0;
                virtual void onGetOpenDuration(std::string duration, ResultCode code) = 0;
                virtual void onGetOpenAlarmState(OpenAlarmState, ResultCode code) = 0;
                virtual void onActivateOpenAlarm(ResultCode code) = 0;
                virtual void onDeActivateOpenAlarm(ResultCode code) = 0;
            };

            class RemoteDoorResource: public SHBaseRemoteResource,
                    public SHBaseRemoteResourceDelegate
            {
            friend class SHBaseRemoteResourceBuilder;
            public:
                virtual ~RemoteDoorResource();

                void getOpenState();
                void getOpenDuration();
                void getOpenAlarmState();
                void changeOpenAlarmState(OpenAlarmState state);

                void setRemoteDoorResourceDelegate(
                        RemoteDoorResourceDelegate* delegate);

            protected:
                RemoteDoorResource();

            private:
                virtual void onGet(PropertyBundle bundle, ResultCode ret);
                virtual void onSet(PropertyBundle bundle, ResultCode ret);
                virtual void onObserve(PropertyBundle bundle, const ObserveResponse obsType,
                        ResultCode ret);

            private:
                RemoteDoorResourceDelegate *m_delegate;
            };
        }
    }
}
#endif /* SMARTHOME_API_CLIENT_REMOTEDOOR_H_ */
