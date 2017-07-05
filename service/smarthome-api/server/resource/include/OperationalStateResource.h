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
            class OperationalStateResourceDelegate
            {
            public:
                virtual ResultCode onChangeMachineState(std::string state) = 0;
                virtual ResultCode onChangeJobState(std::string state) = 0;
            };

            class OperationalStateResource: public SHBaseResource,
                    public SHBaseResourceDelegate
            {
            public:
                OperationalStateResource();
                virtual ~OperationalStateResource();

                std::string getCurrentMachineState();
                std::string getCurrentJobState();
                std::list< std::string > getPossibleMachineStates();
                std::list< std::string > getPossibleJobStates();

                bool addPossibleMachineState(std::string state);
                bool addPossibleJobState(std::string state);
                bool setCurrentMachineState(std::string state);
                bool setCurrentJobState(std::string state);
                bool setProgressPercentage(int percentage);
                bool setRemainingTime(std::string time);
                bool setRunningTime(std::string time);

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
