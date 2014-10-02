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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <ocstack.h>
#include <logger.h>

const char *getResult(OCStackResult result);

#define TAG PCF("ocservercontainer")

int gQuitFlag = 0;
int gLEDUnderObservation = 0;
void createResources();
typedef struct LEDRESOURCE{
    OCResourceHandle handle;
    bool state;
    int power;
} LEDResource;

static LEDResource LED;

// TODO : hard coded for now, change after Sprint4
const char rspGetLed[] = "{\"href\":\"/a/led\",\"rep\":{\"state\":\"on\",\"color\":\"yellow\"}}";
// TODO : Needs to be changed to retrieve current status of led and return that in response
const char rspPutLed[] = "{\"href\":\"/a/led\",\"rep\":{\"state\":\"off\",\"color\":\"off\"}}";
const char rspFailureLed[] = "{\"href\":\"/a/led\",\"rep\":{\"error\":\"LED_OP_FAIL\"}}";


// TODO : hard coded for now, change after Sprint4
const char rspGetFan[] = "{\"href\":\"/a/fan\",\"rep\":{\"state\":\"on\",\"speed\":10}}";
// TODO : Needs to be changed to retrieve current status of fan and return that in response
const char rspPutFan[] = "{\"href\":\"/a/fan\",\"rep\":{\"state\":\"off\",\"speed\":0}}";
const char rspFailureFan[] = "{\"href\":\"/a/fan\",\"rep\":{\"error\":\"FAN_OP_FAIL\"}}";

static OCEntityHandlerResult
HandleCallback(OCEntityHandlerRequest * ehRequest, const char* opStr, const char* errStr)
{
    OCEntityHandlerResult ret = OC_EH_OK;

    if (strlen(opStr) < ehRequest->resJSONPayloadLen)
    {
        strncpy((char*)ehRequest->resJSONPayload, opStr, ehRequest->resJSONPayloadLen);
    }
    else if (strlen(errStr) < ehRequest->resJSONPayloadLen)
    {
        strncpy((char*)ehRequest->resJSONPayload, errStr, ehRequest->resJSONPayloadLen);
        ret = OC_EH_ERROR;
    }
    else
    {
        ret = OC_EH_ERROR;
    }

    return ret;
}

static void
PrintReceivedMsgInfo(OCEntityHandlerFlag flag, OCEntityHandlerRequest * ehRequest )
{
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

    OC_LOG_V(INFO, TAG, "Receiving message type: %s, method %s",
             typeOfMessage,
             (ehRequest->method == OC_REST_GET) ? "OC_REST_GET" : "OC_REST_PUT" );
}

OCEntityHandlerResult OCEntityHandlerLedCb(OCEntityHandlerFlag flag, OCEntityHandlerRequest * ehRequest ) {
    OCEntityHandlerResult ret = OC_EH_OK;

    OC_LOG_V(INFO, TAG, "Callback for Led");
    PrintReceivedMsgInfo(flag, ehRequest );

    if(ehRequest && flag == OC_REQUEST_FLAG)
    {
        if(OC_REST_GET == ehRequest->method)
        {
            ret = HandleCallback(ehRequest, rspGetLed, rspFailureLed);
        }
        if(OC_REST_PUT == ehRequest->method)
        {
            ret = HandleCallback(ehRequest, rspPutLed, rspFailureLed);
        }
    }
    else if (ehRequest && flag == OC_OBSERVE_FLAG)
    {
        gLEDUnderObservation = 1;
    }

    return ret;
}

OCEntityHandlerResult OCEntityHandlerFanCb(OCEntityHandlerFlag flag, OCEntityHandlerRequest * ehRequest ) {
    OCEntityHandlerResult ret = OC_EH_OK;

    OC_LOG_V(INFO, TAG, "Callback for Fan");
    PrintReceivedMsgInfo(flag, ehRequest );

    if(ehRequest && flag == OC_REQUEST_FLAG)
    {
        if(OC_REST_GET == ehRequest->method)
        {
            ret = HandleCallback(ehRequest, rspGetFan, rspFailureFan);
        }
        if(OC_REST_PUT == ehRequest->method)
        {
            ret = HandleCallback(ehRequest, rspPutFan, rspFailureFan);
        }
    }
    else if (ehRequest && flag == OC_OBSERVE_FLAG)
    {
        gLEDUnderObservation = 1;
    }

    return ret;
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
            result = OCNotifyAllObservers (LED.handle);
            if (OC_STACK_NO_OBSERVERS == result)
            {
                gLEDUnderObservation = 0;
            }
        }
    }
    return NULL;
}

int main() {
    printf("hello world from main\n");
    OC_LOG(DEBUG, TAG, "OCServer is starting...");
    uint8_t addr[20] = {0};
    uint8_t* paddr = NULL;
    uint16_t port = 5683;
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
    createResources();

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
        sleep(2);
    }

    /*
     * Cancel the LED thread and wait for it to terminate
     */
    pthread_cancel(threadId);
    pthread_join(threadId, NULL);

    OC_LOG(INFO, TAG, "Exiting ocserver main loop...");

    if (OCStop() != OC_STACK_OK) {
        OC_LOG(ERROR, TAG, "OCStack process error");
    }

    return 0;
}
void createResources() {
    LED.state = false;
    OCResourceHandle room;
    OCStackResult res = OCCreateResource(&room,
            "core.sroom",
            "oc.mi.ll",
            "/a/sroom",
            NULL,
            OC_DISCOVERABLE);
    OC_LOG_V(INFO, TAG, "Created room resource with result: %s", getResult(res));

    OCResourceHandle light;
    res = OCCreateResource(&light,
            "core.light",
            "oc.mi.def",
            "/a/led",
            OCEntityHandlerLedCb,
            OC_DISCOVERABLE|OC_OBSERVABLE);
    OC_LOG_V(INFO, TAG, "Created light resource with result: %s", getResult(res));

    OCResourceHandle fan;
    res = OCCreateResource(&fan,
            "core.fan",
            "oc.mi.def",
            "/a/fan",
            OCEntityHandlerFanCb,
            OC_DISCOVERABLE|OC_OBSERVABLE);
    OC_LOG_V(INFO, TAG, "Created fan resource with result: %s", getResult(res));

    res = OCBindResource(room, light);
    OC_LOG_V(INFO, TAG, "OC Bind Contained Resource to resource: %s", getResult(res));

    res = OCBindResource(room, fan);
    OC_LOG_V(INFO, TAG, "OC Bind Contained Resource to resource: %s", getResult(res));
}


