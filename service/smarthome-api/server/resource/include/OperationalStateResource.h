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
 * This file contains the declaration of classes and its members related to OperationalStateResource.
 */

#ifndef SMARTHOME_API_SERVER_OPERATIONALSTATE_H_
#define SMARTHOME_API_SERVER_OPERATIONALSTATE_H_

#include <SHBaseResource.h>
#include <CommonApi.h>
#include <PropertyBundle.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions for callback of
             * OperationalStateResource class.
             */
            class OperationalStateResourceDelegate
            {
            public:
                /**
                 * API to handle machine state change request .
                 *
                 * @param[in] state state to be changed
                 * @return ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual ResultCode onChangeMachineState(std::string state) = 0;

                /**
                 * API to handle job state change request .
                 *
                 * @param[in] state state to be changed
                 * @return ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual ResultCode onChangeJobState(std::string state) = 0;
            };

            /**
             * This class contains a set of functions to change the properties of OperationalState.
             */
            class OperationalStateResource: public SHBaseResource,
                    public SHBaseResourceDelegate
            {
            public:
                OperationalStateResource();
                virtual ~OperationalStateResource();

                /**
                 * API to get current machine state.
                 *
                 * @return string current machine state
                 */
                std::string getCurrentMachineState();

                /**
                 * API to get current job state.
                 *
                 * @return string current job state
                 */
                std::string getCurrentJobState();

                /**
                 * API to get possible machine states.
                 *
                 * @return list<string> possible machine states
                 */
                std::list< std::string > getPossibleMachineStates();

                /**
                 * API to get possible job states.
                 *
                 * @return list<string> possible job states
                 */
                std::list< std::string > getPossibleJobStates();

                /**
                 * API to add possible machine state change request .
                 *
                 * @param[in] state state to be added
                 * @return true if success
                 */
                bool addPossibleMachineState(std::string state);

                /**
                 * API to add possible job state change request .
                 *
                 * @param[in] state state to be added
                 * @return true if success
                 */
                bool addPossibleJobState(std::string state);

                /**
                 * API to set current machine state.
                 *
                 * @param[in] state current state
                 * @return true if success
                 */
                bool setCurrentMachineState(std::string state);

                /**
                 * API to set current job state.
                 *
                 * @param[in] state current state
                 * @return true if success
                 */
                bool setCurrentJobState(std::string state);

                /**
                 * API to set progress percentage.
                 *
                 * @param[in] percentage progress percentage
                 * @return true if success
                 */
                bool setProgressPercentage(int percentage);

                /**
                 * API to set remaining time.
                 *
                 * @param[in] time temaining time.
                 * @return true if success
                 */
                bool setRemainingTime(std::string time);

                /**
                 * API to set running time.
                 *
                 * @param[in] time running time
                 * @return true if success
                 */
                bool setRunningTime(std::string time);

                /**
                 * API to set delegate of resource.
                 *
                 * @param[in] delegate delegate instance
                 */
                void setDelegate(OperationalStateResourceDelegate *delegate);

            private:
                ResultCode onGet(RequestId requestId, const ResourceQuery& query);
                ResultCode onSet(RequestId requestId, const PropertyBundle& bundle,
                        const ResourceQuery& query);

            private:
                OperationalStateResourceDelegate *m_delegate;
            };
        }
    }
}

#endif /* SMARTHOME_API_SERVER_OPERATIONALSTATE_H_ */
