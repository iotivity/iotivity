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
 * @file
 *
 * This file contains the declaration of reserved variables.
 */

#ifndef SMARTHOME_API_COMMON_COMMONAPI_H_
#define SMARTHOME_API_COMMON_COMMONAPI_H_

#include <string>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            // Typedef for Request ID.
            typedef void* RequestId;
            typedef void* SHRequestHandle;

            namespace RESERVED_URI
            {
                const std::string DEVICE = "/oic/d";
                const std::string PLATFORM = "/oic/p";
            }

            enum ResultCode
            {
                SUCCESS, FAIL, KEEP,

                OBSERVE_REGISTER_SUCCESS,
                OBSERVE_REGISTER_FAIL,
                OBSERVE_NOTIFY,
                OBSERVE_UNREGISTER_SUCCESS,
                OBSERVE_UNREGISTER_FAIL
            };

            enum ObserveResponse
            {
                REGISTER,
                UNREGISTER,
                NOTIFY
            };

            namespace DEVICE_TYPE
            {
                const std::string UNKNOWN = "oic.wk.d";
                const std::string LIGHT = "oic.d.light";
                const std::string ROBOTCLEANER = "oic.d.robotcleaner";
                const std::string SMART_LOCK = "oic.d.smartlock";
                const std::string DRYER = "oic.d.dryer";
                const std::string WATER_VALVE = "oic.d.watervalve";
                const std::string BLIND = "oic.d.blind";
                const std::string DOOR = "oic.d.door";
                const std::string RECEIVER = "oic.d.receiver";
                const std::string TELEVISION = "oic.d.tv";
                const std::string CAMERA = "oic.d.camera";
                const std::string PRINTER = "oic.d.printer";
                const std::string SCANNER = "oic.d.scanner";
                const std::string PRINTER_MULTIFUNCTION = "oic.d.multifunctionprinter";
            }

            namespace RESOURCE_TYPE
            {
                const std::string BINARYSWITCH = "oic.r.switch.binary";
                const std::string MODE = "oic.r.mode";
                const std::string LOCK_STATUS = "oic.r.lock.status";
                const std::string OPENLEVEL = "oic.r.openlevel";
                const std::string AUDIO = "oic.r.audio";
                const std::string MEDIA_INPUT = "oic.r.media.input";
                const std::string MEDIA_OUTPUT = "oic.r.media.output";
                const std::string MEDIA = "oic.r.media";
                const std::string OPERATIONAL_STATE = "oic.r.operational.state";
                const std::string AUTOMATICDOCUMENT_FEEDER = "oic.r.automaticdocumentfeeder";
            }

            namespace INTERFACE
            {
                const std::string BASELINE = "oic.if.baseline";
                const std::string LINKS_LIST = "oic.if.ll";
                const std::string BATCH = "oic.if.b";
                const std::string READ_ONLY = "oic.if.r";
                const std::string READ_WRITE = "oic.if.rw";
                const std::string ACTUATOR = "oic.if.a";
                const std::string SENSOR = "oic.if.s";
            }

            /**
             * Resource Properties.
             * The value of a policy property is defined as bitmap.
             * The LSB represents OC_DISCOVERABLE and Second LSB bit represents OC_OBSERVABLE and
             * so on. Not including the policy property is equivalent to zero.
             */
            namespace RESOURCE_PROPERTY
            {
                enum
                {
                    /** When none of the bits are set, the resource is non-discoverable &
                     *  non-observable by the client.*/
                    NONE = (0),

                    /** When this bit is set, the resource is allowed to be discovered by clients.*/
                    DISCOVERABLE = (1 << 0),

                    /** When this bit is set, the resource is allowed to be observed by clients.*/
                    OBSERVABLE = (1 << 1),
                };
            }
        }
    }
}
#endif /* SMARTHOME_API_COMMON_COMMONAPI_H_ */
