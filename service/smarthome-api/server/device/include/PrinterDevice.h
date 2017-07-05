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
#ifndef SMARTHOME_API_SERVER_PRINTER_H_
#define SMARTHOME_API_SERVER_PRINTER_H_

#include <SHBaseDevice.h>
#include <BinarySwitchResource.h>
#include <OperationalStateResource.h>

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

            class PrinterDevice: public SHBaseDevice
            {
            public:
                PrinterDevice();
                virtual ~PrinterDevice();

            public:
                BinarySwitchResource m_binarySwitch;
                OperationalStateResource m_operationalState;
            };
        }
    }
}

#endif /* SMARTHOME_API_SERVER_PRINTER_H_ */
