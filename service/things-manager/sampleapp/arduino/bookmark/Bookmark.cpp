//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

#include "Arduino.h"

#include "logger.h"
#include "ocstack.h"
#include  <string.h>

#include <EthernetServer.h>
#include <Ethernet.h>
#include <Dns.h>
#include <EthernetClient.h>
#include <util.h>
#include <EthernetUdp.h>
#include <Dhcp.h>


const char *getResult(OCStackResult result);

#define LEDPOWERPIN 2

const prog_char TAG[] = "BOOKMARK";

static int prevBookState = 0;
static int publishState = 0;
static int gBookmarkObservation = 0;



typedef struct BOOKMARKRESOURCE {
    OCResourceHandle handle;
    unsigned int level;
} BookmarkResource;

static BookmarkResource Bookmark;

String responsePayloadPut = "{\"href\":\"/oc_bookmark\",\"rep\":{\"level\":0}}";
String responsePayloadGet = "{\"href\":\"/oc_bookmark\",\"rep\":{\"level\":0}}";

String JsonGenerator(BookmarkResource bookmark){
    //{"href":"/oc_bookmark",{"level":5}}
    String begin = "{\"href\":\"/oc_bookmark\",\"rep\":{\"level\":";
    String end = "}}";
    String ss;

    ss = begin + bookmark.level + end;
    return ss;
}

OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest ) {
    const char* typeOfMessage;

    OCEntityHandlerResult ehRet = OC_EH_OK;

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
    if(entityHandlerRequest && flag == OC_REQUEST_FLAG) { //[CL]
        if(OC_REST_GET == entityHandlerRequest->method) {

            int str_len = responsePayloadGet.length() + 1;
            char charBuf[str_len+1];

            responsePayloadGet.toCharArray(charBuf, str_len);

            if(strlen(charBuf) < entityHandlerRequest->resJSONPayloadLen)
            {
                strncpy((char *)entityHandlerRequest->resJSONPayload, charBuf, entityHandlerRequest->resJSONPayloadLen);
            }
            else
                ehRet = OC_EH_ERROR;
        }

        if(OC_REST_PUT == entityHandlerRequest->method) {

            int str_len = responsePayloadPut.length() + 1;
            char charBuf[str_len+1];

            responsePayloadPut.toCharArray(charBuf, str_len);

            if(strlen(charBuf) < entityHandlerRequest->resJSONPayloadLen)
            {
                strncpy((char *)entityHandlerRequest->resJSONPayload, charBuf, entityHandlerRequest->resJSONPayloadLen);
            }
             else
               ehRet = OC_EH_ERROR;
        }
    } else if (entityHandlerRequest && flag == OC_OBSERVE_FLAG) {
        gBookmarkObservation = 1;

    }

    return ehRet;
}

void createBookmarkResource() {
    Bookmark.level = 2.5;

    OCStackResult res = OCCreateResource(&Bookmark.handle,
                                         "core.bookmark",
                                         "oc.mi.def",
                                         "/core/bookmark",
                                         OCEntityHandlerCb,
                                         OC_DISCOVERABLE|OC_OBSERVABLE);

    OC_LOG_V(INFO, TAG, "Created Bookmark resource with result: %s", getResult(res));

    pinMode(13, OUTPUT);
}

//The setup function is called once at startup of the sketch
void setup()
{
    //
    // Add your initialization code here
    OC_LOG_INIT();

    OC_LOG(DEBUG, TAG, "BookmarkServer is starting...");
    uint16_t port = USE_RANDOM_PORT;

    //Mac address of my ethernet shield
    uint8_t ETHERNET_MAC[] = {0x00, 0x40, 0xa0, 0xee, 0x85, 0xf4};
    uint8_t error = Ethernet.begin(ETHERNET_MAC);
    Serial.print(Ethernet.localIP());
    if (error  == 0)
    {
        OC_LOG_V(ERROR, TAG, "error is: %d", error);
        return;
    }

    if (OCInit(NULL, port, OC_SERVER) != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack init error");
        return;
    }

    /*
     * Declare and create the example resource: Bookmark
     */

    createBookmarkResource();

}

void sensingPressure() {
    // Read the input on analog pin 0:
    int sensorValue = analogRead(A0);
  
    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 10V):
    float voltage = sensorValue * (5.0 / 1023.0);
 
    OC_LOG_V(INFO, TAG, "\t\t\t\t Book State :: %d\n\n", 
                    prevBookState);
    //  OC_LOG_V(INFO, TAG, "Sensing values: %d", voltage);

    if (voltage > 2.5 && prevBookState == 0) {
        prevBookState = 5;
    
        if (publishState != 1) 
        {
          publishState = 1;
        }

    }
  
    else if (voltage > 2.5 && prevBookState != 0) {
        prevBookState = 5;
    }

    else if (voltage < 2.5 && prevBookState != 0) {
        prevBookState -= 1;
        if(prevBookState == 0)
        {
            publishState = 1;
        }
    }


    Bookmark.level = prevBookState;
    responsePayloadGet  = JsonGenerator(Bookmark);

    
    if(gBookmarkObservation)
    {
        if(publishState)
        {   

            OC_LOG_V(INFO, TAG, "\n\\t\t\t\tnCall OCNotifyObservers:: %d\n\n", 
                            prevBookState);

            int result = OCNotifyObservers(Bookmark.handle);
            if( OC_STACK_NO_OBSERVERS == result )
            {
                gBookmarkObservation = 0;
            }
            publishState = 0;
        }
    }
}


// The loop function is called in an endless loop
void loop()
{
    delay(500);
    if (OCProcess() != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack process error");
        return;
    }

    sensingPressure();
}

const char *getResult(OCStackResult result) {
    switch (result) {
    case OC_STACK_OK:
        return "OC_STACK_OK";
    case OC_STACK_INVALID_URI:
        return "OC_STACK_INVALID_URI";
    case OC_STACK_INVALID_QUERY:
        return "OC_STACK_INVALID_QUERY";
    case OC_STACK_INVALID_IP:
        return "OC_STACK_INVALID_IP";
    case OC_STACK_INVALID_PORT:
        return "OC_STACK_INVALID_PORT";
    case OC_STACK_INVALID_CALLBACK:
        return "OC_STACK_INVALID_CALLBACK";
    case OC_STACK_INVALID_METHOD:
        return "OC_STACK_INVALID_METHOD";
    case OC_STACK_NO_MEMORY:
        return "OC_STACK_NO_MEMORY";
    case OC_STACK_COMM_ERROR:
        return "OC_STACK_COMM_ERROR";
    case OC_STACK_INVALID_PARAM:
        return "OC_STACK_INVALID_PARAM";
    case OC_STACK_NOTIMPL:
        return "OC_STACK_NOTIMPL";
    case OC_STACK_NO_RESOURCE:
        return "OC_STACK_NO_RESOURCE";
    case OC_STACK_RESOURCE_ERROR:
        return "OC_STACK_RESOURCE_ERROR";
    case OC_STACK_SLOW_RESOURCE:
        return "OC_STACK_SLOW_RESOURCE";
    case OC_STACK_NO_OBSERVERS:
        return "OC_STACK_NO_OBSERVERS";
    case OC_STACK_ERROR:
        return "OC_STACK_ERROR";
    default:
        return "UNKNOWN";
    }
}


