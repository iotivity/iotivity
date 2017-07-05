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
#include <PrinterDevice.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            PrinterDevice::PrinterDevice()
            {
                setType(DEVICE_TYPE::PRINTER);

                //add possible machine state for printer
                m_operationalState.addPossibleMachineState(MachineState::IDLE);
                m_operationalState.addPossibleMachineState(MachineState::PROCESSING);
                m_operationalState.addPossibleMachineState(MachineState::STOPPED);

                //add possible job state for printer
                m_operationalState.addPossibleJobState(JobState::PENDING);
                m_operationalState.addPossibleJobState(JobState::PENDINGHELD);
                m_operationalState.addPossibleJobState(JobState::PROCESSING);
                m_operationalState.addPossibleJobState(JobState::CANCELLED);
                m_operationalState.addPossibleJobState(JobState::ABORTED);
                m_operationalState.addPossibleJobState(JobState::COMPLETED);
            }

            PrinterDevice::~PrinterDevice()
            {
            }
        }
    }
}
