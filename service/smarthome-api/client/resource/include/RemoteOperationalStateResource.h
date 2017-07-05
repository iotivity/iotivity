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
#ifndef SMARTHOME_API_CLIENT_REMOTEOPERATIONALSTATE_H_
#define SMARTHOME_API_CLIENT_REMOTEOPERATIONALSTATE_H_

#include <SHBaseRemoteResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class RemoteOperationalStateResourceDelegate
            {
            public:
                virtual void onGetState(std::string machineState, std::string jobState,
                        std::list< std::string > possibleMachineStates,
                        std::list< std::string > possibleJobStates, std::string runningTime,
                        std::string remainingTime, int progressPercentage, ResultCode ret) = 0;
                virtual void onChangeMachineState(ResultCode ret) = 0;
                virtual void onChangeJobState(ResultCode ret) = 0;
            };

            class RemoteOperationalStateResource: public SHBaseRemoteResource,
                    public SHBaseRemoteResourceDelegate
            {
            friend class SHBaseRemoteResourceBuilder;
            public:
                void getState();
                void changeMachineState(std::string state);
                void changeJobState(std::string state);

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
