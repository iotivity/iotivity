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
#include "cJSON.h"
#include "ocserverbasicops.h"

int gQuitFlag = 0;

static LEDResource LED;
// This variable determines instance number of the LED resource.
// Used by POST method to create a new instance of LED resource.
static int gCurrLedInstance = 0;
#define SAMPLE_MAX_NUM_POST_INSTANCE  2
static LEDResource gLedInstance[SAMPLE_MAX_NUM_POST_INSTANCE];

char *gResourceUri= (char *)"/a/led";

static uint16_t OC_WELL_KNOWN_PORT = 5683;

//This function takes the request as an input and returns the response
//in JSON format.
char* constructJsonResponse (OCEntityHandlerRequest *ehRequest)
{
    cJSON *json = cJSON_CreateObject();
    cJSON *format;
    char *jsonResponse;
    LEDResource *currLEDResource = &LED;

    if (ehRequest->resource == gLedInstance[0].handle)
    {
        currLEDResource = &gLedInstance[0];
        gResourceUri = (char *) "a/led/0";
    }
    else if (ehRequest->resource == gLedInstance[1].handle)
    {
        currLEDResource = &gLedInstance[1];
        gResourceUri = (char *) "a/led/1";
    }

    if(OC_REST_PUT == ehRequest->method)
    {
        cJSON *putJson = cJSON_Parse((char *)ehRequest->reqJSONPayload);
        currLEDResource->state = ( !strcmp(cJSON_GetObjectItem(putJson,"state")->valuestring ,
                "on") ? true:false);
        currLEDResource->power = cJSON_GetObjectItem(putJson,"power")->valuedouble;
        cJSON_Delete(putJson);
    }

    cJSON_AddStringToObject(json,"href",gResourceUri);
    cJSON_AddItemToObject(json, "rep", format=cJSON_CreateObject());
    cJSON_AddStringToObject(format, "state", (char *) (currLEDResource->state ? "on":"off"));
    cJSON_AddNumberToObject(format, "power", currLEDResource->power);

    jsonResponse = cJSON_Print(json);
    cJSON_Delete(json);
    return jsonResponse;
}

void ProcessGetRequest (OCEntityHandlerRequest *ehRequest)
{
    char *getResp = constructJsonResponse(ehRequest);

    if (ehRequest->resJSONPayloadLen > strlen ((char *)getResp))
    {
        strncpy((char *)ehRequest->resJSONPayload, getResp,
                strlen((char *)getResp));
    }
    else
    {
        OC_LOG_V (INFO, TAG, "Response buffer: %d bytes is too small",
                ehRequest->resJSONPayloadLen);
    }

    free(getResp);
}

void ProcessPutRequest (OCEntityHandlerRequest *ehRequest)
{
    char *putResp = constructJsonResponse(ehRequest);

    if (ehRequest->resJSONPayloadLen > strlen ((char *)putResp))
    {
        strncpy((char *)ehRequest->resJSONPayload, putResp,
                strlen((char *)putResp));
    }
    else
    {
        OC_LOG_V (INFO, TAG, "Response buffer: %d bytes is too small",
                ehRequest->resJSONPayloadLen);
    }

    free(putResp);
}

void ProcessPostRequest (OCEntityHandlerRequest *ehRequest)
{
    char *respPLPost_led = NULL;
    cJSON *json;
    cJSON *format;

    /*
     * The entity handler determines how to process a POST request.
     * Per the REST paradigm, POST can also be used to update representation of existing
     * resource or create a new resource.
     * In the sample below, if the POST is for /a/led then a new instance of the LED
     * resource is created with default representation (if representation is included in
     * POST payload it can be used as initial values) as long as the instance is
     * lesser than max new instance count. Once max instance count is reached, POST on
     * /a/led updated the representation of /a/led (just like PUT)
     */

    if (ehRequest->resource == LED.handle)
    {
        if (gCurrLedInstance < SAMPLE_MAX_NUM_POST_INSTANCE)
        {
            // Create new LED instance
            char newLedUri[15] = "/a/led/";
            sprintf (newLedUri + strlen(newLedUri), "%d", gCurrLedInstance);

            json = cJSON_CreateObject();

            cJSON_AddStringToObject(json,"href",gResourceUri);
            cJSON_AddItemToObject(json, "rep", format=cJSON_CreateObject());
            cJSON_AddStringToObject(format, "createduri", (char *) newLedUri);

            if (0 == createLEDResource (newLedUri, &gLedInstance[gCurrLedInstance], false, 0))
            {
                OC_LOG (INFO, TAG, "Created new LED instance");
                gLedInstance[gCurrLedInstance].state = 0;
                gLedInstance[gCurrLedInstance].power = 0;
                gCurrLedInstance++;
                respPLPost_led = cJSON_Print(json);
            }

            cJSON_Delete(json);
        }
        else
        {
            respPLPost_led = constructJsonResponse(ehRequest);
        }
    }
    else
    {
        for (int i = 0; i < SAMPLE_MAX_NUM_POST_INSTANCE; i++)
        {
            if (ehRequest->resource == gLedInstance[i].handle)
            {
                if (i == 0)
                {
                    respPLPost_led = constructJsonResponse(ehRequest);
                    break;
                }
                else if (i == 1)
                {
                    respPLPost_led = constructJsonResponse(ehRequest);
                }
            }
        }
    }

    if (respPLPost_led != NULL && ehRequest->resJSONPayloadLen > strlen ((char *)respPLPost_led))
    {
        strncpy((char *)ehRequest->resJSONPayload, respPLPost_led,
                strlen((char *)respPLPost_led));
    }
    else
    {
        OC_LOG_V (INFO, TAG, "Response buffer: %d bytes is too small",
                ehRequest->resJSONPayloadLen);
    }

    free(respPLPost_led);
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
            else if (OC_REST_POST == entityHandlerRequest->method)
            {
                OC_LOG (INFO, TAG, "Received OC_REST_POST from client");
                ProcessPostRequest (entityHandlerRequest);
            }
            else
            {
                OC_LOG_V (INFO, TAG, "Received unsupported method %d from client",
                        entityHandlerRequest->method);
            }
        }
    }
    return OC_EH_OK;
}

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum)
{
    if (signum == SIGINT)
    {
        gQuitFlag = 1;
    }
}

int main(int argc, char* argv[])
{
    uint8_t addr[20] = {0};
    uint8_t* paddr = NULL;
    uint16_t port = OC_WELL_KNOWN_PORT;
    uint8_t ifname[] = "eth0";
    struct timespec timeout;

    OC_LOG(DEBUG, TAG, "OCServer is starting...");
    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    if ( OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr,
                sizeof(addr)) == ERR_SUCCESS)
    {
        OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
        paddr = addr;
    }

    if (OCInit((char *) paddr, port, OC_SERVER) != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack init error");
        return 0;
    }

    /*
     * Declare and create the example resource: LED
     */
    createLEDResource(gResourceUri, &LED, false, 0);

    timeout.tv_sec  = 0;
    timeout.tv_nsec = 100000000L;

    // Break from loop with Ctrl-C
    OC_LOG(INFO, TAG, "Entering ocserver main loop...");
    signal(SIGINT, handleSigInt);
    while (!gQuitFlag)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            OC_LOG(ERROR, TAG, "OCStack process error");
            return 0;
        }
        nanosleep(&timeout, NULL);
        //sleep(2);
    }

    OC_LOG(INFO, TAG, "Exiting ocserver main loop...");

    if (OCStop() != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack process error");
    }

    return 0;
}

int createLEDResource (char *uri, LEDResource *ledResource, bool resourceState, int resourcePower)
{
    if (!uri)
    {
        OC_LOG(ERROR, TAG, "Resource URI cannot be NULL");
        return -1;
    }

    ledResource->state = resourceState;
    ledResource->power= resourcePower;
    OCStackResult res = OCCreateResource(&(ledResource->handle),
            "core.led",
            "oc.mi.def",
            uri,
            OCEntityHandlerCb,
            OC_DISCOVERABLE|OC_OBSERVABLE | OC_SECURE);
    OC_LOG_V(INFO, TAG, "Created LED resource with result: %s", getResult(res));

    return 0;
}
