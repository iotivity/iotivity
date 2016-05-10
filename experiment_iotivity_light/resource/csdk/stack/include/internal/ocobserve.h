//********************************************************************
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

/**
 * @file
 *
 * This file contains the data structure and APIs for registered resource as an observer.
 *
 */


#ifndef OC_OBSERVE_H
#define OC_OBSERVE_H

/** Sequence number is a 24 bit field, per https://tools.ietf.org/html/draft-ietf-core-observe-16.*/
#define MAX_SEQUENCE_NUMBER              (0xFFFFFF)

/** Maximum number of observers to reach */

#define MAX_OBSERVER_FAILED_COMM         (2)

/** Maximum number of observers to reach for resources with low QOS */
#define MAX_OBSERVER_NON_COUNT           (3)

/**
 * Data structure to hold informations for each registered observer.
 */
typedef struct ResourceObserver
{
    /** Observation Identifier for request.*/
    OCObservationId observeId;

    /** URI of observed resource.*/
    char resUri[URI_RESOURCE_LENGTH];

    /** Query.*/
    char query[URI_QUERY_LENGTH];

    /** token for the observe request.*/
    OCToken_t token;

    /** Resource handle.*/
    OCResource *resource;

    /** Remote Endpoint. */
    OCDevAddr devAddr;

    /** Quality of service of the request.*/
    OCQualityOfService qos;

    /** number of times the server failed to reach the observer.*/
    uint8_t failedCommCount;

    /** number of times the server sent NON notifications.*/
    uint8_t lowQosCount;

    /** force the qos value to CON.*/
    uint8_t forceHighQos;

    /** next node in this list.*/
    struct ResourceObserver *next;
} ResourceObserver;

/**
 * Create an observe response and send to all observers in the observe list.
 *
 * @param method          RESTful method.
 * @param resPtr          Observed resource.
 * @param maxAge          Time To Live (in seconds) of observation.
 * @param resourceType    Resource type.  Allows resource type name to be added to response.
 * @param qos             Quality of service of resource.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult SendAllObserverNotification(OCResource *resource, OCQualityOfService qos);

/**
 * Notify specific observers with updated value of representation.
 *
 * @param resource                  Observed resource.
 * @param obsIdList                 List of observation ids that need to be notified.
 * @param numberOfIds               Number of observation ids included in obsIdList.
 * @param notificationJSONPayload   JSON encoded payload to send in notification.
 * @param maxAge                    Time To Live (in seconds) of observation.
 * @param qos                       Desired quality of service of the observation notifications.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult SendListObserverNotification (OCResource * resource,
        OCObservationId  *obsIdList, uint8_t numberOfIds,
        const OCRepPayload *payload, uint32_t maxAge,
        OCQualityOfService qos);

/**
 * Delete all observers in the observe list.
 */
void DeleteObserverList();

/**
 * Create a unique observation ID.
 *
 * @param observationId           Pointer to generated ID.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult GenerateObserverId (OCObservationId *observationId);

/**
 * Add observer for a resource.
 *
 * @param resUri          Resource URI string.
 * @param query           Query string.
 * @param obsId           Observation ID.
 * @param token           Request token.
 * @param tokenLength     Length of token.
 * @param resHandle       Resource handle.
 * @param qos             Quality of service of observation.
 * @param devAddr         Device address.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult AddObserver (const char         *resUri,
                           const char         *query,
                           OCObservationId    obsId,
                           const OCToken_t    *token,
                           OCResource         *resHandle,
                           OCQualityOfService qos,
                           const OCDevAddr    *devAddr);

/**
 * Delete observer with specified token from list of observers.
 * Free memory that was allocated for the observer in the list.
 *
 * @param token Token to search for.
 * @param tokenLength Length of token.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
 OCStackResult DeleteObserverUsingToken(const OCToken_t *token);

/**
 * Search the list of observers for the specified token.
 *
 * @param token            Token to search for.
 * @param tokenLength      Length of token.
 *
 * @return Pointer to found observer.
 */
ResourceObserver* GetObserverUsingToken (const OCToken_t *token);

/**
 * Search the list of observers for the specified observe ID.
 *
 * @param observeId        Observer ID to search for.
 *
 * @return Pointer to found observer.
 */
ResourceObserver* GetObserverUsingId (const OCObservationId observeId);

/**
 *  Copy the observe option from a received request.
 *
 * @param observationOption      Pointer to observe option value.  Should be either
 *                               ::OC_OBSERVE_REGISTER, ::OC_OBSERVE_DEREGISTER, or
 *                               ::OC_OBSERVE_NO_OPTION if observe not found.
 *
 * @param options                Options in received request.  Observe option is removed if found.
 * @param numOptions             Number of options in the received request.  Decremented if observe
 *                               option is extracted.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult
GetObserveHeaderOption (uint32_t *observationOption,
                        OCHeaderOption *options,
                        uint8_t *numOptions);

#endif //OC_OBSERVE_H

