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
#include <PropertyBundle.h>
#include <CommonException.h>
#include <RemoteOperationalStateResource.h>
#include <logger.h>

#define TAG "OIC_SH_CLIENT_REMOTEOPERATIONALSTATE"

const static std::string KEY_CURRENT_MACHINESTATE = "currentMachineState";
const static std::string KEY_CURRENT_JOBSTATE = "currentJobState";
const static std::string KEY_MACHINESTATES = "machineStates";
const static std::string KEY_JOBSTATES = "jobStates";
const static std::string KEY_RUNNINGTIME = "runningTime";
const static std::string KEY_REMAININGTIME = "remainingTime";
const static std::string KEY_PROGRESSPERCENTAGE = "progressPercentage";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            RemoteOperationalStateResource::RemoteOperationalStateResource() :
                    m_delegate(NULL), m_changeJobState(false), m_changeMachineState(false)
            {
            }

            RemoteOperationalStateResource::~RemoteOperationalStateResource()
            {
            }

            void RemoteOperationalStateResource::getState()
            {
                getPropertyBundle();
                std::cout << "[RemoteOperationalState] getStatus requested.." << std::endl;
            }

            void RemoteOperationalStateResource::changeMachineState(std::string state)
            {
                PropertyBundle bundle;
                bundle.setValue(KEY_CURRENT_MACHINESTATE, state);

                std::cout << "[RemoteOperationalState] changeMachineState requested.." << std::endl;

                m_changeMachineState = true;
                setPropertyBundle(bundle);
            }

            void RemoteOperationalStateResource::changeJobState(std::string state)
            {
                PropertyBundle bundle;
                bundle.setValue(KEY_CURRENT_JOBSTATE, state);

                std::cout << "[RemoteOperationalState] changeJobState requested.." << std::endl;

                m_changeJobState = true;
                setPropertyBundle(bundle);
            }

            void RemoteOperationalStateResource::setDelegate(RemoteOperationalStateResourceDelegate
                *delegate)
            {
                SHBaseRemoteResource::setDelegate(this);

                this->m_delegate = delegate;
            }

            void RemoteOperationalStateResource::onGet(PropertyBundle bundle, ResultCode ret)
            {
                std::string currentMachineState;
                bundle.getValue(KEY_CURRENT_MACHINESTATE, currentMachineState);

                std::string currentJobState;
                bundle.getValue(KEY_CURRENT_JOBSTATE, currentJobState);

                std::list< std::string > possibleMachineStates;
                bundle.getValue(KEY_MACHINESTATES, possibleMachineStates);

                std::list< std::string > possibleJobStates;
                bundle.getValue(KEY_JOBSTATES, possibleJobStates);

                std::string runningTime;
                bundle.getValue(KEY_RUNNINGTIME, runningTime);

                std::string remainingTime;
                bundle.getValue(KEY_REMAININGTIME, remainingTime);

                int progressPercentage;
                bundle.getValue(KEY_PROGRESSPERCENTAGE, progressPercentage);

                m_delegate->onGetState(currentMachineState, currentJobState, possibleMachineStates,
                        possibleJobStates, runningTime, remainingTime, progressPercentage, ret);
            }

            void RemoteOperationalStateResource::onSet(PropertyBundle bundle, ResultCode ret)
            {
                if (m_changeMachineState)
                {
                    m_changeMachineState = false;

                    std::string currentMachineState;
                    bundle.getValue(KEY_CURRENT_MACHINESTATE, currentMachineState);
                    m_delegate->onChangeMachineState(ret);
                }

                if (m_changeJobState)
                {
                    m_changeJobState = false;

                    std::string currentJobState;
                    bundle.getValue(KEY_CURRENT_JOBSTATE, currentJobState);
                    m_delegate->onChangeJobState(ret);
                }
            }

            void RemoteOperationalStateResource::onObserve(PropertyBundle bundle,
                    const ObserveResponse obsType,
                    ResultCode ret)
            {
                OIC_LOG(DEBUG, TAG, "Entered onObserve");
                return;
            }
        }
    }
}
