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
#include "ocstack.h"
#include "logger.h"

const char *getResult(OCStackResult result);

#define TAG PCF("ocserver")

static int gObserveNotifyType = 3;

int gQuitFlag = 0;
int gLEDUnderObservation = 0;
void createLEDResource();
typedef struct LEDRESOURCE{
    OCResourceHandle handle;
    bool state;
    int power;
} LEDResource;

static LEDResource LED;

typedef struct {
    OCObservationId observationId;
    bool            valid;
} Observers;

#define SAMPLE_MAX_NUM_OBSERVATIONS     8
Observers interestedObservers[SAMPLE_MAX_NUM_OBSERVATIONS];

#ifdef WITH_PRESENCE
static int stopPresenceCount = 10;
#endif

// TODO: hard coded for now, change after Sprint4
const char responsePayloadGet[] = "{\"href\":\"/a/led\",\"rep\":{\"state\":\"on\",\"power\":10}}";
const char responsePayloadPut[] = "{\"href\":\"/a/led\",\"rep\":{\"state\":\"off\",\"power\":0}}";
static uint16_t OC_WELL_KNOWN_PORT = 5683;

void ProcessGetRequest (OCEntityHandlerRequest *ehRequest)
{
    if (ehRequest->resJSONPayloadLen > strlen ((char *)responsePayloadGet))
    {
        strncpy((char *)ehRequest->resJSONPayload, responsePayloadGet,
                strlen((char *)responsePayloadGet));
    }
    else
    {
        OC_LOG_V (INFO, TAG, "Response buffer: %d bytes is too small",
                ehRequest->resJSONPayloadLen);
    }
}

void ProcessPutRequest (OCEntityHandlerRequest *ehRequest)
{
    if (ehRequest->resJSONPayloadLen > strlen ((char *)responsePayloadPut))
    {
        strncpy((char *)ehRequest->resJSONPayload, responsePayloadPut,
                strlen((char *)responsePayloadPut));
    }
    else
    {
        OC_LOG_V (INFO, TAG, "Response buffer: %d bytes is too small",
                ehRequest->resJSONPayloadLen);
    }
}

void ProcessObserveRegister (OCEntityHandlerRequest *ehRequest)
{
    OC_LOG_V (INFO, TAG, "Received observation registration request with observation Id %d",
                ehRequest->obsInfo->obsId);
    for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; i++)
    {
        if (interestedObservers[i].valid == false)
        {
            interestedObservers[i].observationId = ehRequest->obsInfo->obsId;
            interestedObservers[i].valid = true;
            gLEDUnderObservation = 1;
            break;
        }
    }
}

void ProcessObserveDeregister (OCEntityHandlerRequest *ehRequest)
{
    bool clientStillObserving = false;

    OC_LOG_V (INFO, TAG, "Received observation deregistration request for observation Id %d",
                ehRequest->obsInfo->obsId);
    for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; i++)
    {
        if (interestedObservers[i].observationId == ehRequest->obsInfo->obsId)
        {
            interestedObservers[i].valid = false;
        }
        if (interestedObservers[i].valid == true)
        {
            // Even if there is one single client observing we continue notifying entity handler
            clientStillObserving = true;
        }
    }
    if (clientStillObserving == false)
        gLEDUnderObservation = 0;
}
OCEntityHandlerResult
OCDeviceEntityHandlerCb (OCEntityHandlerFlag flag,
                   OCEntityHandlerRequest *entityHandlerRequest, char* uri)
{

    OC_LOG_V (INFO, TAG, "Inside device default entity handler - flags: 0x%x, uri: %s", flag, uri);
    if (flag & OC_INIT_FLAG)
    {
        OC_LOG (INFO, TAG, "Flag includes OC_INIT_FLAG");
    }
    if (flag & OC_REQUEST_FLAG)
    {
        OC_LOG (INFO, TAG, "Flag includes OC_REQUEST_FLAG");
        if (entityHandlerRequest)
        {
            if (OC_REST_GET == entityHandlerRequest->method)
            {
                OC_LOG (INFO, TAG, "Received OC_REST_GET from client");
                ProcessGetRequest (entityHandlerRequest);
            }
            else if (OC_REST_PUT == entityHandlerRequest->method)
            {
                OC_LOG (INFO, TAG, "Received OC_REST_PUT from client");
                ProcessPutRequest (entityHandlerRequest);
            }
            else
            {
                OC_LOG_V (INFO, TAG, "Received unsupported method %d from client",
                            entityHandlerRequest->method);
            }
        }
    }
    if (flag & OC_OBSERVE_FLAG)
    {
        OC_LOG(INFO, TAG, "Flag includes OC_OBSERVE_FLAG");
        if (entityHandlerRequest)
        {
            if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo->action)
            {
                OC_LOG (INFO, TAG, "Received OC_OBSERVE_REGISTER from client");
            }
            else if (OC_OBSERVE_DEREGISTER == entityHandlerRequest->obsInfo->action)
            {
                OC_LOG (INFO, TAG, "Received OC_OBSERVE_DEREGISTER from client");
            }
        }
    }

    return OC_EH_OK;
}


OCEntityHandlerResult
OCEntityHandlerCb (OCEntityHandlerFlag flag,
                   OCEntityHandlerRequest *entityHandlerRequest)
{
    OC_LOG_V (INFO, TAG, "Inside entity handler - flags: 0x%x", flag);
    if (flag & OC_INIT_FLAG)
    {
        OC_LOG (INFO, TAG, "Flag includes OC_INIT_FLAG");
    }
    if (flag & OC_REQUEST_FLAG)
    {
        OC_LOG (INFO, TAG, "Flag includes OC_REQUEST_FLAG");
        if (entityHandlerRequest)
        {
            if (OC_REST_GET == entityHandlerRequest->method)
            {
                OC_LOG (INFO, TAG, "Received OC_REST_GET from client");
                ProcessGetRequest (entityHandlerRequest);
            }
            else if (OC_REST_PUT == entityHandlerRequest->method)
            {
                OC_LOG (INFO, TAG, "Received OC_REST_PUT from client");
                ProcessPutRequest (entityHandlerRequest);
            }
            else
            {
                OC_LOG_V (INFO, TAG, "Received unsupported method %d from client",
                            entityHandlerRequest->method);
            }
        }
    }
    if (flag & OC_OBSERVE_FLAG)
    {
        OC_LOG(INFO, TAG, "Flag includes OC_OBSERVE_FLAG");
        if (entityHandlerRequest)
        {
            if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo->action)
            {
                OC_LOG (INFO, TAG, "Received OC_OBSERVE_REGISTER from client");
                ProcessObserveRegister (entityHandlerRequest);
            }
            else if (OC_OBSERVE_DEREGISTER == entityHandlerRequest->obsInfo->action)
            {
                OC_LOG (INFO, TAG, "Received OC_OBSERVE_DEREGISTER from client");
                ProcessObserveDeregister (entityHandlerRequest);
            }
        }
    }
    if(entityHandlerRequest->rcvdVendorSpecificHeaderOptions &&
            entityHandlerRequest->numRcvdVendorSpecificHeaderOptions)
    {
        OC_LOG (INFO, TAG, "Received vendor specific options");
        uint8_t i = 0;
        OCHeaderOption * rcvdOptions = entityHandlerRequest->rcvdVendorSpecificHeaderOptions;
        for( i = 0; i < entityHandlerRequest->numRcvdVendorSpecificHeaderOptions; i++)
        {
            if(((OCHeaderOption)rcvdOptions[i]).protocolID == OC_COAP_ID)
            {
                OC_LOG_V(INFO, TAG, "Received option with OC_COAP_ID and ID %u with",
                        ((OCHeaderOption)rcvdOptions[i]).optionID );
                OC_LOG_BUFFER(INFO, TAG, ((OCHeaderOption)rcvdOptions[i]).optionData,
                        ((OCHeaderOption)rcvdOptions[i]).optionLength);
            }
        }
        OCHeaderOption * sendOptions = entityHandlerRequest->sendVendorSpecificHeaderOptions;
        uint8_t option2[] = {21,22,23,24,25,26,27,28,29,30};
        uint8_t option3[] = {31,32,33,34,35,36,37,38,39,40};
        sendOptions[0].protocolID = OC_COAP_ID;
        sendOptions[0].optionID = 2248;
        memcpy(sendOptions[0].optionData, option2, sizeof(option2));
        sendOptions[0].optionLength = 10;
        sendOptions[1].protocolID = OC_COAP_ID;
        sendOptions[1].optionID = 2600;
        memcpy(sendOptions[1].optionData, option3, sizeof(option3));
        sendOptions[1].optionLength = 10;
        entityHandlerRequest->numSendVendorSpecificHeaderOptions = 2;
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

    uint8_t j = 0;
    uint8_t numNotifies = (SAMPLE_MAX_NUM_OBSERVATIONS)/2;
    OCObservationId obsNotify[numNotifies];

    while (1)
    {
        sleep(10);
        LED.power += 5;
        if (gLEDUnderObservation)
        {
            OC_LOG_V(INFO, TAG, " =====> Notifying stack of new power level %d\n", LED.power);
            if (gObserveNotifyType == 1)
            {
                // Notify list of observers. Alternate observers on the list will be notified.
                j = 0;
                for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; (i=i+2))
                {
                    if (interestedObservers[i].valid == true)
                    {
                        obsNotify[j] = interestedObservers[i].observationId;
                        j++;
                    }
                }
                result = OCNotifyListOfObservers (LED.handle, obsNotify, j,
                        (unsigned char *)responsePayloadGet);
            }
            else if (gObserveNotifyType == 0)
            {
                // Notifying all observers
                result = OCNotifyAllObservers (LED.handle);
                if (OC_STACK_NO_OBSERVERS == result)
                {
                    OC_LOG (INFO, TAG,
                            "=======> No more observers exist, stop sending observations");
                    gLEDUnderObservation = 0;
                }
            }
            else
            {
                OC_LOG (ERROR, TAG, "Incorrect notification type selected");
            }
        }
        #ifdef WITH_PRESENCE
        OC_LOG_V(INFO, TAG, "================ presence count %d",stopPresenceCount);
        if(!stopPresenceCount--)
        {
            OC_LOG(INFO, TAG, "================ stopping presence");
            OCStopPresence();
        }
        #endif
    }
    return NULL;
}

static void PrintUsage()
{
    OC_LOG(INFO, TAG, "Usage : ocserver -o <0|1>");
    OC_LOG(INFO, TAG, "-o 0 : Notify all observers");
    OC_LOG(INFO, TAG, "-o 1 : Notify list of observers");
}

int main(int argc, char* argv[])
{
    uint8_t addr[20] = {0};
    uint8_t* paddr = NULL;
    uint16_t port = OC_WELL_KNOWN_PORT;
    uint8_t ifname[] = "eth0";
    pthread_t threadId;
    int opt;

    while ((opt = getopt(argc, argv, "o:")) != -1)
    {
        switch(opt)
        {
            case 'o':
                gObserveNotifyType = atoi(optarg);
                break;
            default:
                PrintUsage();
                return -1;
        }
    }

    if ((gObserveNotifyType != 0) && (gObserveNotifyType != 1))
    {
        PrintUsage();
        return -1;
    }

    OC_LOG(DEBUG, TAG, "OCServer is starting...");
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
    #ifdef WITH_PRESENCE
    if (OCStartPresence(0) != OC_STACK_OK) {
        OC_LOG_V(ERROR, TAG, "OCStack presence/discovery error");
        return 0;
    }
    #endif

    OCSetDefaultDeviceEntityHandler(OCDeviceEntityHandlerCb);

    /*
     * Declare and create the example resource: LED
     */
    createLEDResource();

    // Initialize observations data structure for the resource
    for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; i++)
    {
       interestedObservers[i].valid = false;
    }

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
