/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef CA_MANAGER_H_
#define CA_MANAGER_H_

#include <OCApi.h>

namespace OC
{
    /**
    * This namespace contains the main entrance/functionality to monitoring network changes.
    * It may be used with OC::CAManager::functionName. To set a custom callback function,
    * the implementer must make a call to CAManager::setNetworkMonitorHandler.
    */
    namespace CAManager
    {
        // typedef to get adapter status changes from CA.
        typedef std::function<void(const std::string&, OCConnectivityType,
                                   bool)> ConnectionChangedCallback;

        // typedef to get connection status changes from CA.
        typedef std::function<void(OCTransportAdapter, bool)> AdapterChangedCallback;

        /**
        * Set network monitoring handler.
        * @param adapterHandler adapter state change handler to handle changes for
        *                       any transport types.
        * @param connectionHandler connection state change handler to handle changes for
        *                          connection with remote devices.
        * @return Returns ::OC_STACK_OK if success.
        */
        OCStackResult setNetworkMonitorHandler(AdapterChangedCallback adapterHandler,
                                               ConnectionChangedCallback connectionHandler);

        /**
        * Set port number to use.
        * @param adapter transport adapter type to assign the specified port number.
        * @param flag transport flag information.
        * @param port the specified port number to use.
        * @return Returns ::OC_STACK_OK if success.
        */
        OCStackResult setPortNumberToAssign(OCTransportAdapter adapter,
                                            OCTransportFlags flag, uint16_t port);

        /**
        * Get the assigned port number.
        * @param adapter transport adapter type to get the opened port number.
        * @param flag   transport flag information.
        * @return Returns currently assigned port number.
        */
        uint16_t getAssignedPortNumber(OCTransportAdapter adapter, OCTransportFlags flag);

#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
        /**
         * Select the cipher suite for TLS/DTLS handshake.
         * @param cipher  cipher suite (Note : Make sure endianness).
         *                    0x35   : TLS_RSA_WITH_AES_256_CBC_SHA
         *                    0xC018 : TLS_ECDH_anon_WITH_AES_128_CBC_SHA
         *                    0xC037 : TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256
         *                    0xC0AE : TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
         * @param adapter transport adapter type.
         * @return Returns ::OC_STACK_OK if success.
         */
        OCStackResult setCipherSuite(const uint16_t cipher, OCTransportAdapter adapter);
#endif // defined(__WITH_DTLS__) || defined(__WITH_TLS__)
    }
}

#endif // CA_MANAGER_H_



