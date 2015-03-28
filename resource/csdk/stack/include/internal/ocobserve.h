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

#ifndef OC_OBSERVE_H
#define OC_OBSERVE_H

/* Sequence number is a 24 bit field */
#define MAX_SEQUENCE_NUMBER              (0xFFFFFF)

#define MAX_OBSERVER_FAILED_COMM         (2)
#define MAX_OBSERVER_NON_COUNT           (3)

/* This information is stored for each registerd observer */
typedef struct ResourceObserver
{
    // Observation Identifier for request
    OCObservationId observeId;
    // URI of observed resource
    char *resUri;
    // Query
    char *query;
    //token for the observe request
    CAToken_t token;
    //token length for the observe request
    uint8_t tokenLength;
    // Resource handle
    OCResource *resource;
    //TODO bundle it in Endpoint structure(address, uri, type, secured)
    /** Remote Endpoint address **/
    CAAddress_t addressInfo;
    /** Connectivity of the endpoint**/
    CAConnectivityType_t connectivityType;
    // Quality of service of the request
    OCQualityOfService qos;
    // number of times the server failed to reach the observer
    uint8_t failedCommCount;
    // number of times the server sent NON notifications
    uint8_t lowQosCount;
    // force the qos value to CON
    uint8_t forceHighQos;
    // next node in this list
    struct ResourceObserver *next;
} ResourceObserver;

#ifdef WITH_PRESENCE
OCStackResult SendAllObserverNotification (OCMethod method, OCResource *resPtr, uint32_t maxAge,
        OCResourceType *resourceType, OCQualityOfService qos);
#else
OCStackResult SendAllObserverNotification (OCMethod method, OCResource *resPtr, uint32_t maxAge,
        OCQualityOfService qos);
#endif
OCStackResult SendListObserverNotification (OCResource * resource,
        OCObservationId  *obsIdList, uint8_t numberOfIds,
        const char *notificationJSONPayload, uint32_t maxAge,
        OCQualityOfService qos);

void DeleteObserverList();

OCStackResult GenerateObserverId (OCObservationId *observationId);

OCStackResult AddObserver (const char         *resUri,
                           const char         *query,
                           OCObservationId    obsId,
                           CAToken_t          *token,
                           uint8_t            tokenLength,
                           OCResource         *resHandle,
                           OCQualityOfService qos,
                           const CAAddress_t  *addressInfo,
                           CAConnectivityType_t connectivityType);

OCStackResult DeleteObserverUsingToken (CAToken_t * token, uint8_t tokenLength);

ResourceObserver* GetObserverUsingToken (const CAToken_t * token, uint8_t tokenLength);

ResourceObserver* GetObserverUsingId (const OCObservationId observeId);

OCStackResult
CreateObserveHeaderOption (CAHeaderOption_t **caHdrOpt,
                           OCHeaderOption *ocHdrOpt,
                           uint8_t numOptions,
                           uint8_t observeFlag);
OCStackResult
GetObserveHeaderOption (uint32_t * observationOption,
                        CAHeaderOption_t *options,
                        uint8_t * numOptions);

#endif //OC_OBSERVE_H

