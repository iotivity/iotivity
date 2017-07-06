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
 * RemoteDoorResource.
 */

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

            /**
             * This class contains a set of functions for callback of
             * RemoteDoorResource class.
             */
            class RemoteDoorResourceDelegate
            {
            public:
                virtual ~RemoteDoorResourceDelegate() {}

                /**
                 * API to handle a response about OpenState get request.
                 *
                 * @param[in] state open state value
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onGetOpenState(std::string state, ResultCode code) = 0;

                /**
                 * API to handle a response about OpenDuration get request.
                 *
                 * @param[in] duration duration value
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onGetOpenDuration(std::string duration, ResultCode code) = 0;

                /**
                 * API to handle a response about OpenAlarm get request.
                 *
                 * @param[in] OpenAlarmState alarm state value
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onGetOpenAlarmState(OpenAlarmState, ResultCode code) = 0;

                /**
                 * API to handle a response about OpenAlarm activate request.
                 *
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onActivateOpenAlarm(ResultCode code) = 0;

                /**
                 * API to handle a response about OpenAlarm deactivate request.
                 *
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onDeActivateOpenAlarm(ResultCode code) = 0;
            };

            /**
             * This class contains a set of functions to control RemoteDoorResource.
             */
            class RemoteDoorResource: public SHBaseRemoteResource,
                    public SHBaseRemoteResourceDelegate
            {
            friend class SHBaseRemoteResourceBuilder;
            public:
                virtual ~RemoteDoorResource();

                /**
                 * API to get openStatue.
                 */
                void getOpenState();

                /**
                 * API to get openDuration.
                 */
                void getOpenDuration();

                /**
                 * API to get openAlarmState.
                 */
                void getOpenAlarmState();

                /**
                 * API to chage openAlarmState.
                 *
                 * @param[in] state alram state
                 */
                void changeOpenAlarmState(OpenAlarmState state);

                /**
                 * API to set a instance of delegate calss.
                 *
                 * @param[in] delegate instance of delegate
                 */
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
