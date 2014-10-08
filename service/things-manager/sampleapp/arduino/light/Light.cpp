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

const prog_char TAG[] = "LIGHTSERVER";

int gQuitFlag = 0;
int gTHUnderObservation = 0;
void createLightResource();
typedef struct LIGHTRESOURCE {
    OCResourceHandle handle;
    String state;
} LightResource;

static LightResource Light;

String responsePayloadPut = "{\"href\":\"/oc_light\",\"rep\":{\"powerstate\":\"off\"}}";
String responsePayloadGet = "{\"href\":\"/oc_light\",\"rep\":{\"powerstate\":\"off\"}}";

String JsonGenerator(LightResource Light){

//{"href":"/oc_light",{"powerstate":"on"}}
	String begin = "{\"href\":\"/oc_light\",\"rep\":{\"powerstate\":\"";
	String end = "\"}}";
    String ss;

	ss = begin + Light.state + end;
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

			int length = strlen((const char *)entityHandlerRequest->reqJSONPayload) + 1;
			char request[length];

			strncpy(request, (const char *)entityHandlerRequest->reqJSONPayload, length);
            /*
			for( unsigned int i = 0; i < length; i += 17 )
			{
				OC_LOG_V(INFO, TAG, "\n\n\n\n \t\t entityHandlerRequest : %s",
										 request + i);
			}

			OC_LOG_V(INFO, TAG, "\t\t PCH : %s \n\n\n\n", request);
            */
			
			char* pch = strstr(request, "powerstate");
			//OC_LOG_V(INFO, TAG, "\t\t BEFORE PCH : %s \n\n\n\n", pch);
			pch = pch + strlen( "powerstate\":\"" );

			//OC_LOG_V(INFO, TAG, "\t\t AFTER PCH : %s \n\n\n\n", pch);

			if( strncmp(pch, "on", strlen("on"))  == 0 )
			{
				OC_LOG_V(INFO, TAG, "\t\t TURN ON the LIGHT");
				pinMode(LEDPOWERPIN,OUTPUT);
				digitalWrite(LEDPOWERPIN, HIGH);

				Light.state = "on";
			}
			else
			{
				OC_LOG_V(INFO, TAG, "\t\t TURN OFF the LIGHT");
				
				pinMode(LEDPOWERPIN,OUTPUT);
				digitalWrite(LEDPOWERPIN, LOW);

				Light.state = "off";
			}

            int str_len1 = responsePayloadPut.length() + 1;
            char charBuf1[str_len1];

            responsePayloadPut.toCharArray(charBuf1, str_len1);

            if(strlen(charBuf1) < entityHandlerRequest->resJSONPayloadLen)
            {
            	strncpy((char *)entityHandlerRequest->resJSONPayload, charBuf1, entityHandlerRequest->resJSONPayloadLen);
            }
             else
               ehRet = OC_EH_ERROR;
        }
    } else if (entityHandlerRequest && flag == OC_OBSERVE_FLAG) {
        gTHUnderObservation = 1;

    }

    return ehRet;
}

//The setup function is called once at startup of the sketch
void setup()
{
    //
    // Add your initialization code here
    OC_LOG_INIT();

    OC_LOG(DEBUG, TAG, "LightServer is starting...");
    uint16_t port = USE_RANDOM_PORT;

    //Mac address of my ethernet shield
//    uint8_t ETHERNET_MAC[] = {0x90, 0xA2, 0xDA, 0x0E, 0xB8, 0xAB};
    uint8_t ETHERNET_MAC[] = {0x10, 0xa9, 0x2e, 0xae, 0x83, 0x3b};
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
     * Declare and create the example resource: Light
     */

    createLightResource();

}


// The loop function is called in an endless loop
void loop()
{
    delay(1000);
    if (OCProcess() != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack process error");
        return;
    }

	JsonGenerator(Light);
}

void createLightResource() {
    Light.state = "off";

/*
    OCStackResult res = OCCreateResource(&Light.handle,
                                         "SoftSensorManager.Sensor",
                                         "oc.mi.def",
                                         "/Thing_TempHumSensor",
                                         OCEntityHandlerCb,
                                         OC_DISCOVERABLE|OC_OBSERVABLE);
*/

    OCStackResult res = OCCreateResource(&Light.handle,
                                         "core.light",
                                         "oc.mi.def",
                                         "/core/light",
                                         OCEntityHandlerCb,
                                         OC_DISCOVERABLE|OC_OBSERVABLE);

    OC_LOG_V(INFO, TAG, "Created Light resource with result: %s", getResult(res));

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


