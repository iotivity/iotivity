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
#include <CommonApi.h>
#include <CommonException.h>
#include <ResourceQuery.h>
#include <OperationalStateResource.h>

const static std::string URI_OPERATIONALSTATE = "/operationalstate";
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
            OperationalStateResource::OperationalStateResource() :
                    m_delegate(NULL), SHBaseResource(URI_OPERATIONALSTATE,
                            RESOURCE_TYPE::OPERATIONAL_STATE,
                            RESOURCE_PROPERTY::DISCOVERABLE | RESOURCE_PROPERTY::OBSERVABLE)

            {
            }

            OperationalStateResource::~OperationalStateResource()
            {
            }

            std::string OperationalStateResource::getCurrentMachineState()
            {
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                std::string currentMachineState;
                storedBundle.getValue(KEY_CURRENT_MACHINESTATE, currentMachineState);

                return currentMachineState;
            }

            std::list< std::string > OperationalStateResource::getPossibleMachineStates()
            {
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                std::list< std::string > possibleMachineStates;
                storedBundle.getValue(KEY_MACHINESTATES, possibleMachineStates);

                return possibleMachineStates;
            }

            std::string OperationalStateResource::getCurrentJobState()
            {
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                std::string currentJobState;
                storedBundle.getValue(KEY_CURRENT_JOBSTATE, currentJobState);

                return currentJobState;
            }

            std::list< std::string > OperationalStateResource::getPossibleJobStates()
            {
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                std::list< std::string > possibleJobStates;
                storedBundle.getValue(KEY_JOBSTATES, possibleJobStates);

                return possibleJobStates;
            }

            bool OperationalStateResource::addPossibleMachineState(std::string state)
            {
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                std::list< std::string > possibleMachineStates;
                storedBundle.getValue(KEY_MACHINESTATES, possibleMachineStates);

                possibleMachineStates.push_back(state);
                storedBundle.setValue(KEY_MACHINESTATES, possibleMachineStates);

                setPropertyBundle(storedBundle);

                return true;
            }

            bool OperationalStateResource::addPossibleJobState(std::string state)
            {
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                std::list< std::string > possibleJobStates;
                storedBundle.getValue(KEY_JOBSTATES, possibleJobStates);

                possibleJobStates.push_back(state);
                storedBundle.setValue(KEY_JOBSTATES, possibleJobStates);

                setPropertyBundle(storedBundle);

                return true;
            }

            bool OperationalStateResource::setCurrentMachineState(std::string state)
            {
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                storedBundle.setValue(KEY_CURRENT_MACHINESTATE, state);

                setPropertyBundle(storedBundle);

                return true;
            }

            bool OperationalStateResource::setCurrentJobState(std::string state)
            {
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                storedBundle.setValue(KEY_CURRENT_JOBSTATE, state);

                setPropertyBundle(storedBundle);

                return true;
            }

            bool OperationalStateResource::setProgressPercentage(int percentage)
            {
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                storedBundle.setValue(KEY_PROGRESSPERCENTAGE, percentage);

                setPropertyBundle(storedBundle);

                return true;
            }

            bool OperationalStateResource::setRemainingTime(std::string time)
            {
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                storedBundle.setValue(KEY_REMAININGTIME, time);

                setPropertyBundle(storedBundle);

                return true;
            }

            bool OperationalStateResource::setRunningTime(std::string time)
            {
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                storedBundle.setValue(KEY_RUNNINGTIME, time);

                setPropertyBundle(storedBundle);

                return true;
            }

            void OperationalStateResource::setDelegate(
                    OperationalStateResourceDelegate *delegate)
            {
                SHBaseResource::setDelegate(this);

                this->m_delegate = delegate;
            }

            ResultCode OperationalStateResource::onGet(RequestId requestId,
                    const ResourceQuery& query)
            {
                std::cout << "[OperationalState] onGet" << std::endl;

                return SUCCESS;
            }

            ResultCode OperationalStateResource::onSet(RequestId requestId,
                    const PropertyBundle& bundle, const ResourceQuery& query)
            {
                std::cout << "[OperationalState] onSet" << std::endl;

                ResultCode retCode = FAIL;

                std::string state;
                if (NULL != this->m_delegate
                        && bundle.contains(KEY_CURRENT_MACHINESTATE))
                {
                    bundle.getValue(KEY_CURRENT_MACHINESTATE, state);
                    retCode = m_delegate->onChangeMachineState(state);
                }
                else if (NULL != this->m_delegate
                        && bundle.contains(KEY_CURRENT_JOBSTATE))
                {
                    bundle.getValue(KEY_CURRENT_JOBSTATE, state);
                    retCode = m_delegate->onChangeJobState(state);
                }
                else
                {
                    std::cout << "[OperationalState]Invaild property key" << std::endl;
                    return FAIL;
                }

                return retCode;
            }
        }
    }
}
