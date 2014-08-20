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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "ocstack.h"
#include "logger.h"

const char *getResult(OCStackResult result);

#define TAG PCF("ocserver")

int gQuitFlag = 0;
int gLEDUnderObservation = 0;
void createLEDResource();
typedef struct LEDRESOURCE{
    OCResourceHandle handle;
    bool state;
    int power;
} LEDResource;

static LEDResource LED;

// TODO: hard coded for now, change after Sprint4
const char responsePayloadGet[] = "{\"href\":\"/a/led\",\"rep\":{\"state\":\"on\",\"power\":10}}";
const char responsePayloadPut[] = "{\"href\":\"/a/led\",\"rep\":{\"state\":\"off\",\"power\":0}}";
static uint16_t OC_WELL_KNOWN_PORT = 5683;

OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest ) {
    const char* typeOfMessage;

    switch (flag) {
        case OC_INIT_FLAG:
            typeOfMessage = "OC_INIT_FLAG";
            break;
        case OC_REQUEST_FLAG:
            typeOfMessage = "OC_REQUEST_FLAG";
            break;
        case OC_OBSERVE_FLAG:
            typeOfMessage = "OC_OBSERVE_FLAG";
            break;
        default:
            typeOfMessage = "UNKNOWN";
    }
    OC_LOG_V(INFO, TAG, "Receiving message type: %s", typeOfMessage);
    if(entityHandlerRequest && flag == OC_REQUEST_FLAG){ //[CL]
    if(OC_REST_GET == entityHandlerRequest->method)
            strncpy((char *)entityHandlerRequest->resJSONPayload, responsePayloadGet, entityHandlerRequest->resJSONPayloadLen);
        if(OC_REST_PUT == entityHandlerRequest->method) {
            OC_LOG_V(INFO, TAG, "PUT JSON payload from client: %s", entityHandlerRequest->reqJSONPayload);
            strncpy((char *)entityHandlerRequest->resJSONPayload, responsePayloadPut, entityHandlerRequest->resJSONPayloadLen);
            }

    } else if (entityHandlerRequest && flag == OC_OBSERVE_FLAG) {
        //TODO : Should the OC_OBSERVE_FLAG behave just like a GET according to the standard?
        gLEDUnderObservation = 1;
    }

    return OC_EH_OK;
}

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum) {
    if (signum == SIGINT) {
        gQuitFlag = 1;
    }
}

void *ChangeLEDRepresentation (void *param)
{
    (void)param;
    OCStackResult result = OC_STACK_ERROR;

    while (1)
    {
        sleep(10);
        LED.power += 5;
        if (gLEDUnderObservation)
        {
            OC_LOG_V(INFO, TAG, " =====> Notifying stack of new power level %d\n", LED.power);
            result = OCNotifyObservers (LED.handle);
            if (OC_STACK_NO_OBSERVERS == result)
            {
                gLEDUnderObservation = 0;
            }
        }
    }
    return NULL;
}

int main() {
    OC_LOG(DEBUG, TAG, "OCServer is starting...");
    uint8_t addr[20] = {0};
    uint8_t* paddr = NULL;
    uint16_t port = OC_WELL_KNOWN_PORT;
    uint8_t ifname[] = "eth0";
    pthread_t threadId;

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    if ( OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr,
                               sizeof(addr)) == ERR_SUCCESS)
    {
        OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
        paddr = addr;
    }

    if (OCInit((char *) paddr, port, OC_SERVER) != OC_STACK_OK) {
        OC_LOG(ERROR, TAG, "OCStack init error");
        return 0;
    }

    /*
     * Declare and create the example resource: LED
     */
    createLEDResource();

    /*
     * Create a thread for changing the representation of the LED
     */
    pthread_create (&threadId, NULL, ChangeLEDRepresentation, (void *)NULL);

    // Break from loop with Ctrl-C
    OC_LOG(INFO, TAG, "Entering ocserver main loop...");
    signal(SIGINT, handleSigInt);
    while (!gQuitFlag) {
        if (OCProcess() != OC_STACK_OK) {
            OC_LOG(ERROR, TAG, "OCStack process error");
            return 0;
        }
        sleep(3);
    }

    OC_LOG(INFO, TAG, "Exiting ocserver main loop...");

    if (OCStop() != OC_STACK_OK) {
        OC_LOG(ERROR, TAG, "OCStack process error");
    }

    return 0;
}
void createLEDResource() {
    LED.state = false;
    OCStackResult res = OCCreateResource(&LED.handle,
            "core.led",
            "core.rw",
            "/a/led",
            OCEntityHandlerCb,
            OC_DISCOVERABLE|OC_OBSERVABLE);
    OC_LOG_V(INFO, TAG, "Created LED resource with result: %s", getResult(res));
}
