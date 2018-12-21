/******************************************************************
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

#ifndef _NS_UTIL__H_
#define _NS_UTIL__H_

#include "experimental/logger.h"
#include <octypes.h>
#include <stdbool.h>
#include "ocstack.h"
#include "ocpayload.h"
#include "octypes.h"
#include "NSStructs.h"
#include "NSConstants.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "experimental/ocrandom.h"
#include "oic_time.h"
#include "NSProviderSystem.h"

/**
 * Copy OC entity handler on request.
 *
 * @param entityHandlerRequest   entity handler request to handle the incomming requests
 *
 * @return entity handler request pointer
 */
OCEntityHandlerRequest *NSCopyOCEntityHandlerRequest(OCEntityHandlerRequest *entityHandlerRequest);

/**
 * Free OC entity handler on request.
 * @param entityHandlerRequest   entity handler request to handle the incomming requests
 * @return NS_OK
 */
NSResult NSFreeOCEntityHandlerRequest(OCEntityHandlerRequest *entityHandlerRequest);

/**
 * Initialize the message.
 *
 * @return message.
 */
NSMessage * NSInitializeMessage(void);

/**
 * Free the messagei object.
 *
 * @param obj    message object.
 *
 * @return NS_OK for successfully free, otherwise NS_ERROR.
 */
NSResult NSFreeMessage(NSMessage *obj);

/**
 * Duplicate the message.
 *
 * @param copyMsg     notification mesage to duplicate
 *
 * @return new message
 */
NSMessage * NSDuplicateMessage(NSMessage *copyMsg);

/**
 * Free sync information object.
 *
 * @param obj   object to free.
 *
 * @return NS_OK for successfully free, otherwise NS_ERROR.
 */
NSResult NSFreeSync(NSSyncInfo *obj);

/**
 * Duplicate the sync.
 *
 * @param copyMsg    notification sync information to copy
 *
 * @return new message
 */
NSSyncInfo * NSDuplicateSync(NSSyncInfo *copyMsg);

/**
 * Get the sync information.
 *
 * @param payload    oc payload
 *
 * @return sync information
 */
NSSyncInfo * NSGetSyncInfo(OCPayload * payload);

/**
 * Free the consumer object.
 *
 * @param obj   object to free.
 *
 * @return NS_OK for successfully free, otherwise NS_ERROR.
 */
NSResult NSFreeConsumer(NSConsumer *obj);

/**
 * Duplicate the consumer.
 *
 * @param copyMsg    notification consumer to duplicate
 *
 * @return new message
 */
NSConsumer * NSDuplicateConsumer(NSConsumer *copyMsg);

/**
 * Initialize the topic list.
 *
 * @return topic list.
 */
NSTopicList * NSInitializeTopicList(void);

/**
 * Set property string.
 *
 * @param msgPayload   oc representation payload
 * @param name         payload name value to copy
 * @param copyString   copy value according to the type
 */
void NSDuplicateSetPropertyString(OCRepPayload **msgPayload, const char *name, const char *copyString);

/**
 * Set integer property.
 *
 * @param msgPayload  oc message payload
 * @param name        payload name value to copy
 * @param value       copy int value
 */
void NSDuplicateSetPropertyInt(OCRepPayload ** msgPayload, const char * name, int64_t value);

/**
 * Generate UUID string.
 *
 * @param uuidStr  store uuid string
 *
 * @return NS_OK if success, otherwise NS_ERROR
 */
NSResult NSGenerateUUIDStr(char uuidStr[UUID_STRING_SIZE]);

/**
 * Get value from query using comparekey.
 *
 * @param query       query information
 * @param compareKey  get value from query using key
 *
 * @return value
 */
char * NSGetValueFromQuery(char *query, char * compareKey);

/**
 * Free the object.
 *
 * @param obj  object to free
 *
 * @return NS_OK if success, otherwise NS_FAIL
 */
NSResult NSFreeMalloc(char ** obj);

/**
 * Free media contents.
 *
 * @param obj   object of media contents
 *
 * @return NS_OK if success, otherwise NS_FAIL
 */
NSResult NSFreeMediaContents(NSMediaContents * obj);

/**
 * Duplicate media contents.
 *
 * @param copyObj  media content object
 *
 * @return new object
 */
NSMediaContents * NSDuplicateMediaContents(NSMediaContents * copyObj);

/**
 * Find value in payload.
 *
 * @param payload  oc representation payload
 * @param name     value to find
 *
 * @return value
 */
OCRepPayloadValue* NSPayloadFindValue(const OCRepPayload* payload, const char* name);

/**
 * Change the address.
 *
 * @param address address to change
 *
 * @return changed address
 */
OCDevAddr * NSChangeAddress(const char * address);

/**
 * OC stack result.
 *
 * @param ret   result value
 *
 * @return true or false
 */
bool NSOCResultToSuccess(OCStackResult ret);

#endif /* _NS_UTIL__H_ */
