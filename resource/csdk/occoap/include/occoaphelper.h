//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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
#ifndef WITH_ARDUINO
#include <unistd.h>
#endif
#include <limits.h>
#include <ctype.h>
#include "coap.h"
#include "ocstack.h"
#include "occoaptoken.h"
#include "ocstackinternal.h"

/**
 * The Max Size of the buffer that is used to parse uri and query
 * individually.
 */
#define MAX_URI_QUERY_BUF_SIZE ((MAX_URI_LENGTH) >= (MAX_QUERY_LENGTH) ?\
                            (MAX_URI_LENGTH) : (MAX_QUERY_LENGTH))

// checks if optionID is within vendor specific range
OCStackResult isVendorSpecific(uint16_t optionID);

OCStackResult FormOCResponse(OCResponse * * responseLoc,  ClientCB * cbNode, uint32_t maxAge,
        unsigned char * fullUri, unsigned char * rcvdUri, CAToken_t * rcvdToken,
        OCClientResponse * clientResponse, unsigned char * bufRes);

// Internal function to create OCClientResponse struct at the client from a received coap pdu
OCStackResult FormOCClientResponse(OCClientResponse * clientResponse,
        OCStackResult result, OCDevAddr * remote, uint32_t seqNum,
        const unsigned char * resJSONPayload);

#endif /* OCCOAPHELPER_H_ */
