//******************************************************************
///
// Copyright 2014 Intel Corporation All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef OCCOAP_H_
#define OCCOAP_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "ocstack.h"
#include "ocstackinternal.h"
#include <stdint.h>

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------


typedef enum {
    OC_COAP_OK = 0,
    OC_COAP_ERR
} OCCoAPResult;

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

/**
 * Initialize the CoAP client or server with the its IPv4 address and CoAP port
 *
 * @param ipAddr
 *     IP Address of host device
 * @param port
 *     Port of host device
 * @param mode
 *     Host device is client, server, or client-server
 *
  * @return
 *   0   - success
 *   TBD - TBD error
 */
int OCInitCoAP(const char *address, uint16_t port, OCMode mode);

/**
 * Discover OC resources
 *
 * @param method          - method to perform on the resource
 * @param Uri             - URI of the resource to interact with
 * @param asyncReturnFunc - asynchronous callback function that is invoked
 *                          by the stack when discovery or resource interaction is complete
 * @return
 *   0   - success
 *   TBD - TBD error
 */
int OCDoCoAPResource(OCMethod method, OCQualityOfService qos, OCToken * token, const char *Uri);


/**
 * Stop the CoAP client or server processing
 *
 * @return 0 - success, else - TBD error
 */
int OCStopCoAP();

/**
 * Called in main loop of CoAP client or server.  Allows low-level CoAP processing of
 * send, receive, timeout, discovery, callbacks, etc.
 *
 * @return 0 - success, else - TBD error
 */
int OCProcessCoAP();


#endif /* OCCOAP_H_ */
