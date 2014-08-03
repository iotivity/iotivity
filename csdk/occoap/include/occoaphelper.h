//******************************************************************
//
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
//

#ifndef OCCOAPHELPER_H_
#define OCCOAPHELPER_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "coap.h"
#include "ocstack.h"
#include "ocstackinternal.h"
#include <unistd.h>
#include <limits.h>
#include <ctype.h>

#define MAX_TOKEN_LENGTH (8)

typedef struct {
    uint8_t token[MAX_TOKEN_LENGTH];
    size_t tokenLength;
} OCCoAPToken;

// Convert OCStack code to CoAP code
uint8_t OCToCoAPResponseCode(OCStackResult result);

// Internal function to generate a coap pdu based on passed parameters
coap_pdu_t *
GenerateCoAPPdu(uint8_t msgType, uint8_t code, unsigned short id,
        size_t tokenLength, uint8_t * token, unsigned char * payloadJSON,
        coap_list_t *options);

// Call back function used by libcoap to order option in coap pdu
int OrderOptions(void *a, void *b);

// Internal function to create an option node for coap pdu
coap_list_t *
CreateNewOptionNode(unsigned short key, unsigned int length,
        unsigned char *data);

// Internal function to create OCRequest struct at the server from a received coap pdu
OCStackResult FormOCRequest(const coap_queue_t * rcvdRequest,
        OCRequest * * requestLoc, unsigned char * uriBuf,
        unsigned char * queryBuf);

// Internal function to create OCEntityHandlerRequest at the server from a received coap pdu
OCStackResult FormOCEntityHandlerRequest(const coap_queue_t * rcvdRequest,
        OCEntityHandlerRequest * * entityHandlerRequestLoc,
        unsigned char * bufRes, unsigned char * query);

// Internal function to retrieve a Token from received coap pdu
OCStackResult RetrieveOCCoAPToken(const coap_queue_t * rcvdRequest,
        OCCoAPToken * * rcvdTokenLoc);

// Internal function to create OCResponse struct at the client from a received coap pdu
OCStackResult FormOCResponse(const coap_queue_t * rcvdResponse,
        OCResponse * * responseLoc);

// Internal function to create OCClientResponse struct at the client from a received coap pdu
OCStackResult FormOCClientResponse(const coap_queue_t * rcvdResponse,
        OCClientResponse * * clientResponseLoc);

#endif /* OCCOAPHELPER_H_ */
