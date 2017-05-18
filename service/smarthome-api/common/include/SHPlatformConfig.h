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
 * This file contains the declaration of classes and its members related to
 * SHPlatformConfig.
 */

#ifndef SMARTHOME_API_COMMON_SHPLATFORMCONFIG_H_
#define SMARTHOME_API_COMMON_SHPLATFORMCONFIG_H_

#include <octypes.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * Host Mode of Operation.
             */
            namespace Mode
            {
                enum type
                {
                    Client   = OC_CLIENT,
                    Server   = OC_SERVER,
                    Both     = OC_CLIENT_SERVER,
                    Gateway  = OC_GATEWAY
                };
            }

            /**
             * Quality of Service attempts to abstract the guarantees provided
             * by the underlying transport protocol.
             */
            namespace QoS
            {
                enum type
                {
                    /** Packet delivery is best effort.*/
                    LowQoS   = OC_LOW_QOS,

                    /** Acknowledgements are used to confirm delivery.*/
                    HighQoS  = OC_HIGH_QOS,
                };
            }

            /**
             * Transport adapter of Operation.
             * One or more of these values may be used.
             */
            namespace Transport
            {
                enum type
                {
                    /** Use All adapters stack support. */
                    ALL              = OC_DEFAULT_ADAPTER,

                    /** IPv4 and IPv6, including 6LoWPAN. */
                    IP               = OC_ADAPTER_IP,

                    /** GATT over Bluetooth LE. */
                    BLE_GATT         = OC_ADAPTER_GATT_BTLE,

                    /** RFCOMM over Bluetooth EDR. */
                    BT_RFCOMM        = OC_ADAPTER_RFCOMM_BTEDR,
#ifdef RA_ADAPTER
                    /** Remote Access over XMPP. */
                    REMOTE_ACCESS    = OC_ADAPTER_REMOTE_ACCESS,
#endif
                    /** CoAP over TCP. */
                    TCP              = OC_ADAPTER_TCP,

                    /** NFC Transport for Messaging. */
                    NFC              = OC_ADAPTER_NFC
                };
            }

           /**
            * This class contains a set of base configuration informations
            * to initialize stack.
            */
            class SHPlatformConfig
            {
            public:
                /** Indicate whether we want to do server, client or both. */
                Mode::type            mode;

                /** Indicate Quality of Service in transport. */
                QoS::type             qos;

                /** Transport type to initialize. */
                Transport::type       transport;

                /** Indicate whether IPv4 is enabled or not in IP transport.(Default it true) */
                bool                  ipv4enabled;

                /** Indicate whether IPv6 is enabled or not in IP transport.(Default it true) */
                bool                  ipv6enabled;

                /** Persistant storage Handler structure (open/read/write/close/unlink). */
                OCPersistentStorage   *ps;

                SHPlatformConfig()
                    : mode(Mode::Both), qos(QoS::LowQoS),
                    transport(Transport::ALL),
                    ipv4enabled(true), ipv6enabled(true),
                    ps(NULL)
                {}

                SHPlatformConfig(const Mode::type mode_,
                        const QoS::type qos_,
                        const Transport::type transport_,
                        OCPersistentStorage *ps_ = NULL)
                    : mode(mode_), qos(qos_),
                    transport(transport_),
                    ipv4enabled(true), ipv6enabled(true),
                    ps(ps_)
                {}
            };
        }
    }
}

#endif /* SMARTHOME_API_COMMON_SHPLATFORMCONFIG_H_ */
