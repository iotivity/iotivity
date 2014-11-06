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
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <ocstack.h>
#include <logger.h>

const char *getResult(OCStackResult result);

#define TAG PCF("ocservercontainer")

int gQuitFlag = 0;
int gLightUnderObservation = 0;
void createResources();
typedef struct LIGHTRESOURCE{
    OCResourceHandle handle;
    bool state;
    int power;
} LightResource;

static LightResource light;

// TODO : hard coded for now, change after Sprint10
const char rspGetRoomDefault[] = "{\"href\":\"/a/room\",\"rep\":{\"name\":\"John's Room\"}}";
const char rspGetRoomCollection[] = "{\"href\":\"/a/room\"}";
// TODO : Needs to be changed to retrieve current status of room and return that in response
const char rspPutRoomDefault[] = "{\"href\":\"/a/room\",\"rep\":{\"name\":\"John's Room\"}}";
const char rspPutRoomCollection[] = "{\"href\":\"/a/room\"}";
const char rspFailureRoom[] = "{\"href\":\"/a/room\",\"rep\":{\"error\":\"ROOM_OP_FAIL\"}}";

// TODO : hard coded for now, change after Sprint4
const char rspGetLightDefault[] = "{\"href\":\"/a/light\",\"rep\":{\"state\":\"false\",\"color\":\"0\"}}";
const char rspGetLightCollection[] = "{\"href\":\"/a/light\"}";
// TODO : Needs to be changed to retrieve current status of light and return that in response
const char rspPutLightDefault[] = "{\"href\":\"/a/light\",\"rep\":{\"state\":\"true\",\"color\":\"0\"}}";
const char rspPutLightCollection[] = "{\"href\":\"/a/light\"}";
const char rspFailureLight[] = "{\"href\":\"/a/light\",\"rep\":{\"error\":\"LIGHT_OP_FAIL\"}}";


// TODO : hard coded for now, change after Sprint4
const char rspGetFanDefault[] = "{\"href\":\"/a/fan\",\"rep\":{\"state\":\"true\",\"speed\":10}}";
const char rspGetFanCollection[] = "{\"href\":\"/a/fan\"}";
// TODO : Needs to be changed to retrieve current status of fan and return that in response
const char rspPutFanDefault[] = "{\"href\":\"/a/fan\",\"rep\":{\"state\":\"false\",\"speed\":0}}";
const char rspPutFanCollection[] = "{\"href\":\"/a/fan\"}";
const char rspFailureFan[] = "{\"href\":\"/a/fan\",\"rep\":{\"error\":\"FAN_OP_FAIL\"}}";

static OCEntityHandlerResult
HandleCallback(OCEntityHandlerRequest * ehRequest, const char* opStr, const char* errStr)
{
    OCEntityHandlerResult ret = OC_EH_OK;

    if (strlen(opStr) < ehRequest->resJSONPayloadLen)
    {
        strncat((char*)ehRequest->resJSONPayload, opStr, ehRequest->resJSONPayloadLen);
    }
    else if (strlen(errStr) < ehRequest->resJSONPayloadLen)
    {
        strncat((char*)ehRequest->resJSONPayload, errStr, ehRequest->resJSONPayloadLen);
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

OCEntityHandlerResult OCEntityHandlerRoomCb(OCEntityHandlerFlag flag,\
        OCEntityHandlerRequest * ehRequest )
{
    OCEntityHandlerResult ret = OC_EH_OK;

    OC_LOG_V(INFO, TAG, "Callback for Room");
    PrintReceivedMsgInfo(flag, ehRequest );

    if(ehRequest && flag == OC_REQUEST_FLAG )
    {
        std::string query = (const char*)ehRequest->query;

        if(OC_REST_GET == ehRequest->method)
        {
            if(query.find("oc.mi.def") != std::string::npos)
            {
                ret = HandleCallback(ehRequest, rspGetRoomDefault, rspFailureRoom);
                if(ret != OC_EH_ERROR)
                {
                    ret = HandleCallback(ehRequest, ",", ",");
                    ret = HandleCallback(ehRequest, rspGetLightCollection, rspFailureLight);
                }
                if(ret != OC_EH_ERROR)
                {
                    ret = HandleCallback(ehRequest, ",", ",");
                    ret = HandleCallback(ehRequest, rspGetFanCollection, rspFailureFan);
                }
            }
            else if(query.find("oc.mi.ll") != std::string::npos)
            {
                ret = HandleCallback(ehRequest, rspGetRoomCollection, rspFailureRoom);
                if(ret != OC_EH_ERROR)
                {
                    ret = HandleCallback(ehRequest, ",", ",");
                    ret = HandleCallback(ehRequest, rspGetLightCollection, rspFailureLight);
                }
                if(ret != OC_EH_ERROR)
                {
                    ret = HandleCallback(ehRequest, ",", ",");
                    ret = HandleCallback(ehRequest, rspGetFanCollection, rspFailureFan);
                }
            }
            else if(query.find("oc.mi.b") != std::string::npos)
            {
                ret = HandleCallback(ehRequest, rspGetRoomCollection, rspFailureRoom);
                if(ret != OC_EH_ERROR)
                {
                    ret = HandleCallback(ehRequest, ",", ",");
                    ret = HandleCallback(ehRequest, rspGetLightDefault, rspFailureLight);
                }
                if(ret != OC_EH_ERROR)
                {
                    ret = HandleCallback(ehRequest, ",", ",");
                    ret = HandleCallback(ehRequest, rspGetFanDefault, rspFailureFan);
                }
            }
        }
        if(OC_REST_PUT == ehRequest->method)
        {
            if(query.find("oc.mi.def") != std::string::npos)
            {
                if(ret != OC_EH_ERROR)
                {
                    ret = HandleCallback(ehRequest, rspPutRoomDefault, rspFailureRoom);
                }
            }
            if(query.find("oc.mi.ll") != std::string::npos)
            {
                if(ret != OC_EH_ERROR)
                {
                    ret = HandleCallback(ehRequest, rspPutRoomCollection, rspFailureRoom);
                }
                if(ret != OC_EH_ERROR)
                {
                    ret = HandleCallback(ehRequest, ",", ",");
                    ret = HandleCallback(ehRequest, rspPutLightCollection, rspFailureLight);
                }
                if(ret != OC_EH_ERROR)
                {
                    ret = HandleCallback(ehRequest, ",", ",");
                    ret = HandleCallback(ehRequest, rspPutFanCollection, rspFailureFan);
                }
            }
            if(query.find("oc.mi.b") != std::string::npos)
            {
                if(ret != OC_EH_ERROR)
                {
                    ret = HandleCallback(ehRequest, rspPutRoomCollection, rspFailureRoom);
                }
                if(ret != OC_EH_ERROR)
                {
                    ret = HandleCallback(ehRequest, ",", ",");
                    ret = HandleCallback(ehRequest, rspPutLightDefault, rspFailureLight);
                }
                if(ret != OC_EH_ERROR)
                {
                    ret = HandleCallback(ehRequest, ",", ",");
                    ret = HandleCallback(ehRequest, rspPutFanDefault, rspFailureFan);
                }
            }
        }
    }
    else if (ehRequest && flag == OC_OBSERVE_FLAG)
    {
        gLightUnderObservation = 1;
    }
    return ret;
}

OCEntityHandlerResult OCEntityHandlerLightCb(OCEntityHandlerFlag flag, OCEntityHandlerRequest * ehRequest ) {
    OCEntityHandlerResult ret = OC_EH_OK;

    OC_LOG_V(INFO, TAG, "Callback for Light");
    PrintReceivedMsgInfo(flag, ehRequest );

    if(ehRequest && flag == OC_REQUEST_FLAG)
    {
        if(OC_REST_GET == ehRequest->method)
        {
            ret = HandleCallback(ehRequest, rspGetLightDefault, rspFailureLight);
        }
        if(OC_REST_PUT == ehRequest->method)
        {
            ret = HandleCallback(ehRequest, rspPutLightDefault, rspFailureLight);
        }
    }
    else if (ehRequest && flag == OC_OBSERVE_FLAG)
    {
        gLightUnderObservation = 1;
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
            ret = HandleCallback(ehRequest, rspGetFanDefault, rspFailureFan);
        }
        if(OC_REST_PUT == ehRequest->method)
        {
            ret = HandleCallback(ehRequest, rspPutFanDefault, rspFailureFan);
        }
    }
    else if (ehRequest && flag == OC_OBSERVE_FLAG)
    {
        gLightUnderObservation = 1;
    }

    return ret;
}

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum) {
    if (signum == SIGINT) {
        gQuitFlag = 1;
    }
}

void *ChangeLightRepresentation (void *param)
{
    (void)param;
    OCStackResult result = OC_STACK_ERROR;

    while (1)
    {
        sleep(10);
        light.power += 5;
        if (gLightUnderObservation)
        {
     OC_LOG_V(INFO, TAG, " =====> Notifying stack of new power level %d\n", light.power);
            result = OCNotifyAllObservers (light.handle, OC_NA_QOS);
            if (OC_STACK_NO_OBSERVERS == result)
            {
                gLightUnderObservation = 0;
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
    uint16_t port = 0;
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
     * Declare and create the example resource: light
     */
    createResources();

    /*
     * Create a thread for changing the representation of the light
     */
    pthread_create (&threadId, NULL, ChangeLightRepresentation, (void *)NULL);

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
     * Cancel the light thread and wait for it to terminate
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
    light.state = false;

    OCResourceHandle fan;
    OCStackResult res = OCCreateResource(&fan,
            "core.fan",
            "oc.mi.def",
            "/a/fan",
            OCEntityHandlerFanCb,
            OC_DISCOVERABLE|OC_OBSERVABLE);
    OC_LOG_V(INFO, TAG, "Created fan resource with result: %s", getResult(res));

    OCResourceHandle light;
    res = OCCreateResource(&light,
            "core.light",
            "oc.mi.def",
            "/a/light",
            OCEntityHandlerLightCb,
            OC_DISCOVERABLE|OC_OBSERVABLE);
    OC_LOG_V(INFO, TAG, "Created light resource with result: %s", getResult(res));

    OCResourceHandle room;
    res = OCCreateResource(&room,
            "core.room",
            "oc.mi.b",
            "/a/room",
            OCEntityHandlerRoomCb,
            OC_DISCOVERABLE);
    OC_LOG_V(INFO, TAG, "Created room resource with result: %s", getResult(res));
    OCBindResourceInterfaceToResource(room, "oc.mi.ll");
    OCBindResourceInterfaceToResource(room, "oc.mi.def");

    res = OCBindResource(room, light);
    OC_LOG_V(INFO, TAG, "OC Bind Contained Resource to resource: %s", getResult(res));

    res = OCBindResource(room, fan);
    OC_LOG_V(INFO, TAG, "OC Bind Contained Resource to resource: %s", getResult(res));
}


