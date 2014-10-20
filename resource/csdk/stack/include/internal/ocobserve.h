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

/* In CoAP sequence number is a 24 bit field */
#define MAX_SEQUENCE_NUMBER              (0xFFFFFF)
#define OC_RESOURCE_OBSERVE_REGISTER     (0)
#define OC_RESOURCE_OBSERVE_DEREGISTER   (1)
#define OC_RESOURCE_NO_OBSERVE           (2)

#define MAX_OBSERVER_FAILED_COMM         (2)
#define MAX_OBSERVER_NON_COUNT           (3)

#define OC_OBSERVER_NOT_INTERESTED       (0)
#define OC_OBSERVER_STILL_INTERESTED     (1)
#define OC_OBSERVER_FAILED_COMM          (2)

/* This information is stored for each registerd observer */
typedef struct ResourceObserver {
    // URI of observed resource
    unsigned char *resUri;
    //Quality of service of the request
    OCQualityOfService qos;
    // Query
    unsigned char *query;
    // CoAP token for the observe request
    OCCoAPToken *token;
    // Resource handle
    OCResource *resource;
    // IP address & port of client registered for observe
    OCDevAddr *addr;
    // number of times the server failed to reach the observer
    uint8_t failedCommCount;
    // number of times the server sent NON notifications
    uint8_t NONCount;
    // force the qos value to CON
    uint8_t forceCON;
    // next node in this list
    struct ResourceObserver *next;
} ResourceObserver;

OCStackResult OCObserverStatus(OCCoAPToken * token, uint8_t status);

OCStackResult ProcessObserveRequest (OCResource *resource, OCRequest *request);

OCStackResult SendObserverNotification (OCMethod method, OCResource *resPtr, uint32_t maxAge);

void DeleteObserverList();

OCStackResult AddObserver ( const char   *resUri,
                            const char   *query,
                            OCCoAPToken * token,
                            OCDevAddr    *addr,
                            OCResource   *resHandle,
                            OCQualityOfService qos);
OCStackResult DeleteObserver (OCCoAPToken * token);

ResourceObserver* GetObserver (const OCCoAPToken * token);

#endif //OC_OBSERVE_H
