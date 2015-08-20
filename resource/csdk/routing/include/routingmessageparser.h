/* ****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
 * This file contains the APIs for routing message parser.
 */
#ifndef ROUTING_MESSAGE_PARSER_H_
#define ROUTING_MESSAGE_PARSER_H_

#include "routingutility.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Constructs payload for Resource Discovery.
 * @param[in]       gatewayId               Gateway ID.
 * @param[in/out]   payload                 Encoded Payload for Gateway ID.
 * @return  ::OC_STACK_OK or Appropriate error code.
 */
OCStackResult RMPConstructGatewayPayload(uint32_t gatewayId, OCRepPayload **payload);

/**
 * Constructs payload for Observe Response.
 * @param[in]      gatewayId               Gateway ID.
 * @param[in]      seqNum                  Sequence Number of Gateway.
 * @param[in/out]  routingtable            linklist with Routing Table Entries.
 * @param[in]      isUpdateSeqNeeded       Response type of payload response/notification.
 * @param[in/out]  payload                 Payload with gateway Data.
 * @return  ::OC_STACK_OK or Appropriate error code.
 */
OCStackResult RMPConstructObserveResPayload(uint32_t gatewayId, uint32_t seqNum,
                                            u_linklist_t *routingtable, bool isUpdateSeqNeeded,
                                            OCRepPayload **payload);

/**
 * Constructs payload for Removal Nodes.
 * @param[in]       gatewayId           Gateway ID.
 * @param[in]       seqNum              Sequence Number of Gateway.
 * @param[in]       removedGateways     linklist with Removed gateway entries.
 * @param[in]       isUpdateSeqNeeded   Response type of payload response/notification.
 * @param[in/out]   removedPayload      Encoded Payloads of removed routing table entries.
 * @return  ::OC_STACK_OK or Appropriate error code.
 */
OCStackResult RMPConstructRemovalPayload(uint32_t gatewayId, uint32_t seqNum,
                                         u_linklist_t *removedGateways, bool isUpdateSeqNeeded,
                                         OCRepPayload **removedPayload);

/**
 * Constructs payload for Removal Nodes.
 * @param[in/out]   payload              Payload.
 * @param[in/out]   payloadSize          Payload Size.
 * @param[in/out]   gatewayId            Gateway Id.
 * @return  ::OC_STACK_OK or Appropriate error code.
 */
OCStackResult RMPParseRequestPayload(const uint8_t* payload, size_t payloadSize,
                                     uint32_t *gatewayId);

/**
 *Constructs payload for Removal Nodes.
 * @param[in]       payload              Payload with gateways route table encoded.
 * @param[in/out]   seqNum               Sequence Number of Gateway.
 * @param[in/out]   gatewayId            Gateway Id.
 * @param[in/out]   gatewayTable         Parsed routing table from Payload.
 * @param[in/out]   isUpdateSeqNeeded   Response type of payload response/notification.
 * @return  ::OC_STACK_OK or Appropriate error code.
 */
OCStackResult RMPParseResponsePayload(OCRepPayload *payload, uint32_t *gatewayId, uint32_t *seqNum,
                                      u_linklist_t **gatewayTable, bool *isUpdateSeqNeeded);

/**
 * Frees payload.
 * @param[in/out]   payload              Encoded Payload for Gateway ID.
 * @return  NONE.
 */
void RMPFreePayload(OCRepPayload *payload);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ROUTING_MESSAGE_PARSER_H_ */

