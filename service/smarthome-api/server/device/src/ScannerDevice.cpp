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
#include <ScannerDevice.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            ScannerDevice::ScannerDevice()
            {
                setType(DEVICE_TYPE::SCANNER);

                //add possible machine state for scanner
                m_operationalState.addPossibleMachineState(MachineState::IDLE);
                m_operationalState.addPossibleMachineState(MachineState::PROCESSING);
                m_operationalState.addPossibleMachineState(MachineState::STOPPED);

                //add possible job state for scanner
                m_operationalState.addPossibleJobState(JobState::PENDING);
                m_operationalState.addPossibleJobState(JobState::PENDINGHELD);
                m_operationalState.addPossibleJobState(JobState::PROCESSING);
                m_operationalState.addPossibleJobState(JobState::CANCELLED);
                m_operationalState.addPossibleJobState(JobState::ABORTED);
                m_operationalState.addPossibleJobState(JobState::COMPLETED);

                //add possible adf state for scanner
                m_automaticDocumentFeeder.addPossibleState(AdfState::PROCESSING);
                m_automaticDocumentFeeder.addPossibleState(AdfState::EMPTY);
                m_automaticDocumentFeeder.addPossibleState(AdfState::JAM);
                m_automaticDocumentFeeder.addPossibleState(AdfState::LOADED);
                m_automaticDocumentFeeder.addPossibleState(AdfState::MISPICK);
                m_automaticDocumentFeeder.addPossibleState(AdfState::HATCH_OPEN);
                m_automaticDocumentFeeder.addPossibleState(AdfState::DUPLEX_PAGE_TOO_SHORT);
                m_automaticDocumentFeeder.addPossibleState(AdfState::DUPLEX_PAGE_TOO_LONG);
                m_automaticDocumentFeeder.addPossibleState(AdfState::MULTIPICK_DETECTED);
                m_automaticDocumentFeeder.addPossibleState(AdfState::INPUT_TRAY_FAILED);
                m_automaticDocumentFeeder.addPossibleState(AdfState::INPUT_TRAY_OVERLOADED);
            }

            ScannerDevice::~ScannerDevice()
            {
            }
        }
    }
}
