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

#ifndef OC_OBSERVE_H
#define OC_OBSERVE_H

/* In CoAP sequence number is a 24 bit field */
#define MAX_SEQUENCE_NUMBER              0xFFFFFF
#define OC_RESOURCE_OBSERVE_REGISTER     "0"
#define OC_RESOURCE_OBSERVE_DEREGISTER   "1"

/* Vijay: TODO add comments */
typedef struct ObserveResourceServer {
    // xxxxxxxxxxxxxxxx
    unsigned char *resUri;
    // xxxxxxxxxxxxxxxxx
    unsigned char *query;
    // xxxxxxxxxxxxxxxxx
    OCCoAPToken *coapTok;
    // xxxxxxxxxxxxxxxxx
    OCResource *resource;
    // xxxxxxxxxxxxxxxxx
    OCDevAddr *addr;
    // next node in this list
    struct ObserveResourceServer *next;
} ObserveResourceServer;

OCStackResult ProcessObserveRequest (OCResource *resource, OCRequest *request);

OCStackResult SendObserverNotification (OCResourceHandle handle, OCResource *resPtr);

#endif //OC_OBSERVE_H
