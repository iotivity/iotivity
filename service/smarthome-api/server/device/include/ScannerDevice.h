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
#ifndef SMARTHOME_API_SERVER_SCANNER_H_
#define SMARTHOME_API_SERVER_SCANNER_H_

#include <SHBaseDevice.h>
#include <BinarySwitchResource.h>
#include <OperationalStateResource.h>
#include <AutomaticDocumentFeederResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            namespace MachineState
            {
                const static std::string IDLE = "idle";
                const static std::string PROCESSING = "processing";
                const static std::string STOPPED = "stopped";
            }

            namespace JobState
            {
                const static std::string PENDING = "pending";
                const static std::string PENDINGHELD = "pendingHeld";
                const static std::string PROCESSING = "processing";
                const static std::string CANCELLED = "cancelled";
                const static std::string ABORTED = "aborted";
                const static std::string COMPLETED = "completed";
            }

            namespace AdfState
            {
                const static std::string PROCESSING = "adfProcessing";
                const static std::string EMPTY = "dfEmpty";
                const static std::string JAM = "adfJam";
                const static std::string LOADED = "adfLoaded";
                const static std::string MISPICK = "adfMispick";
                const static std::string HATCH_OPEN = "adfHatchOpen";
                const static std::string DUPLEX_PAGE_TOO_SHORT = "adfDuplexPageTooShort";
                const static std::string DUPLEX_PAGE_TOO_LONG = "adfDuplexPageTooLong";
                const static std::string MULTIPICK_DETECTED = "adfMultipickDetected";
                const static std::string INPUT_TRAY_FAILED = "adfInputTrayFailed";
                const static std::string INPUT_TRAY_OVERLOADED = "adfInputTrayOverloaded";
            }

            class ScannerDevice: public SHBaseDevice
            {
            public:
                ScannerDevice();
                virtual ~ScannerDevice();

            public:
                BinarySwitchResource m_binarySwitch;
                OperationalStateResource m_operationalState;
                AutomaticDocumentFeederResource m_automaticDocumentFeeder;
            };
        }
    }
}

#endif /* SMARTHOME_API_SERVER_SCANNER_H_ */
