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



/**
 * @file
 *
 * This file contains the Internal include file used by lower layers of the OC stack
 *
 */

#ifndef OCSTACKINTERNAL_H_
#define OCSTACKINTERNAL_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <stdbool.h>
#include "ocstack.h"
#include "occlientcb.h"
#include "oclogger.h"
#include "ocrandom.h"

#include "octypes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

/** Default device entity Handler.*/
extern OCDeviceEntityHandler DefaultDeviceHandler;

/** Default Callback parameter.*/
extern void* DefaultDeviceHandlerCallbackParameter;

//-----------------------------------------------------------------------------
// Internal function prototypes
//-----------------------------------------------------------------------------

/**
 * Handler function for sending a response from multiple resources, such as a collection.
 * Aggregates responses from multiple resource until all responses are received then sends the
 * concatenated response
 *
 * TODO: Need to add a timeout in case a (remote?) resource does not respond
 *
 * @param token         Token to search for.
 * @param tokenLength   Length of token.
 * @param status        Feedback status.
 * @return
 *     ::OCStackResult
 */

OCStackResult OCStackFeedBack(const OCToken_t *token, uint8_t status);

OCStackResult SendDirectStackResponse(const CAMessageInfo_t *message, const OCMessageType_t type);

#ifdef WITH_PRESENCE

/**
 * Notify Presence subscribers that a resource has been modified.
 *
 * @param resourceType    Handle to the resourceType linked list of resource that was modified.
 * @param trigger         The simplified reason this API was invoked.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
void NotifyPresenceChange(const OCResource *resource, OCPresenceTrigger trigger);

/**
 * Send Stop Notification to Presence subscribers.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult SendStopNotification();
#endif // WITH_PRESENCE

/**
 * Bind a resource interface to a resource.
 *
 * @param resource Target resource.
 * @param resourceInterfaceName Resource interface.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult BindResourceInterfaceToResource(OCResource* resource,
                                            const char *resourceInterfaceName);
/**
 * Bind a resource type to a resource.
 *
 * @param resource Target resource.
 * @param resourceTypeName Name of resource type.
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult BindResourceTypeToResource(OCResource* resource,
                                            const char *resourceTypeName);

/**
 * Get a byte representation of the server instance ID.
 * The memory is managed internal to this function, so freeing it externally will
 * result in a runtime error.
 *
 * Note: This will NOT seed the RNG, so it must be called after the RNG is seeded.
 * This is done automatically during the OCInit process,
 * so ensure that this call is done after that.
 *
 * @return A uint8_t representation the server instance ID.
 */
const uint8_t* OCGetServerInstanceID(void);

/**
 * Get a string representation the server instance ID.
 * The memory is managed internal to this function, so freeing externally will result
 * in a runtime error.
 * Note: This will NOT seed the RNG, so it must be called after the RNG is seeded.
 * This is done automatically during the OCInit process,
 * so ensure that this call is done after that.
 *
 * @return A string representation  the server instance ID.
 */
const char* OCGetServerInstanceIDString(void);

/**
 * Map OCQualityOfService to CAMessageType.
 *
 * @param qos Input qos.
 *
 * @return CA message type for a given qos.
 */
OCMessageType_t qualityOfServiceToMessageType(OCQualityOfService qos);

#ifdef WITH_PRESENCE
const char *convertTriggerEnumToString(OCPresenceTrigger trigger);

OCPresenceTrigger convertTriggerStringToEnum(const char * triggerStr);
#endif

void HandleCAResponses(const CAMessageInfo_t* responseInfo);

bool HandleCAResponsesClient(const CAMessageInfo_t* responseInfo);

bool HandleCAResponsesServer(const CAMessageInfo_t* responseInfo);

void HandleCARequests(CAMessageInfo_t* messageInfo);

OCStackResult OCProcessPresence();

OCStackResult OCInitClient();

OCStackResult OCInitServer();

void OCStopClient();

void OCStopServer();

void OCSendMessage(CAMessageInfo_t *messageInfo);

#define VERIFY_SUCCESS(op, successCode) { if ((op) != (successCode)) \
            {OC_LOG_V(FATAL, TAG, "%s failed!!", #op); goto exit;} }
#define VERIFY_NON_NULL(arg, logLevel, retVal) { if (!(arg)) { OC_LOG((logLevel), \
             TAG, PCF(#arg " is NULL")); return (retVal); } }
#define VERIFY_NON_NULL_NR(arg, logLevel) { if (!(arg)) { OC_LOG((logLevel), \
             TAG, PCF(#arg " is NULL")); return; } }
#define VERIFY_NON_NULL_V(arg) { if (!arg) {OC_LOG_V(FATAL, TAG, "%s is NULL", #arg);\
    goto exit;} }

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* OCSTACKINTERNAL_H_ */

