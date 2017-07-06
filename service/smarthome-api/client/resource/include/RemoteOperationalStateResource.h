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
 * RemoteOperationalStateResource.
 */

#ifndef SMARTHOME_API_CLIENT_REMOTEOPERATIONALSTATE_H_
#define SMARTHOME_API_CLIENT_REMOTEOPERATIONALSTATE_H_

#include <SHBaseRemoteResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions for callback of
             * RemoteOperationalStateResource class.
             */
            class RemoteOperationalStateResourceDelegate
            {
            public:
                /**
                 * API to handle a response about State get request.
                 *
                 * @param[in] machineState current machine state
                 * @param[in] jobState current job state
                 * @param[in] possibleMachineStates possible machine states
                 * @param[in] possibleJobStates possible job states
                 * @param[in] runningTime running time
                 * @param[in] remainingTime remaining time
                 * @param[in] progressPercentage progress percentage
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onGetState(std::string machineState, std::string jobState,
                        std::list<std::string> possibleMachineStates,
                        std::list<std::string> possibleJobStates, std::string runningTime,
                        std::string remainingTime, int progressPercentage, ResultCode ret) = 0;

                /**
                 * API to handle a response about MachineState change request.
                 *
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onChangeMachineState(ResultCode ret) = 0;

                /**
                 * API to handle a response about JobState change request.
                 *
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onChangeJobState(ResultCode ret) = 0;
            };

            /**
             * This class contains a set of functions to control RemoteOperationalStateResource.
             */
            class RemoteOperationalStateResource: public SHBaseRemoteResource,
                    public SHBaseRemoteResourceDelegate
            {
            friend class SHBaseRemoteResourceBuilder;
            public:

                /**
                 * API to get current state.
                 */
                void getState();

                /**
                 * API to change machine state.
                 *
                 * @param[in] state machine state
                 */
                void changeMachineState(std::string state);

                /**
                 * API to change job state.
                 * @param[in] state job state
                 */
                void changeJobState(std::string state);

                /**
                 * API to set an instance of delegate class.
                 * @param[in] delegate instance of delegate
                 */
                void setDelegate(RemoteOperationalStateResourceDelegate *delegate);

            public:
                RemoteOperationalStateResource();
                virtual ~RemoteOperationalStateResource();

            private:
                virtual void onGet(PropertyBundle bundle, ResultCode ret);
                virtual void onSet(PropertyBundle bundle, ResultCode ret);
                virtual void onObserve(PropertyBundle bundle, const ObserveResponse obsType,
                    ResultCode ret);

            private:
                RemoteOperationalStateResourceDelegate *m_delegate;
                bool m_changeMachineState;
                bool m_changeJobState;
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_REMOTEOPERATIONALSTATE_H_ */
