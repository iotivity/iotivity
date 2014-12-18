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

// Do not remove the include below
#include "Arduino.h"
#include "bleLib.h"
#include <stdio.h>

#include "logger.h"
#include "ocstack.h"
#include <string.h>

#include "oic_lanLib.h"

#define ARDUINO_AVR_MEGA2560 1
/// This is the port which Arduino Server will use for all unicast communication with it's peers
#define OC_WELL_KNOWN_PORT 5683

#define JSON_BASE00 "{\"href\":\"\",\"rep\":{"
#define JSON_BASE01 "\"0\":\"MAC\",\"1\":\"string\",\"2\":\""
#define JSON_BASE02 "\",\"3\":\"SERVICETYPE\",\"4\":\"string\",\"5\":\""
#define JSON_BASE03 "\"}}"

typedef struct REFERRESOURCE {
    OCResourceHandle m_handle;
    char* m_macaddress;
    char* m_servicetype;
} REFERResource;

PROGMEM const char TAG[] = "ReferenceSensor";

REFERResource REFER;
Cble ble;

char macaddress[13]={"9059AF1704D7"};
char servicetype[15]={"BOOK"};

int g_REFERUnderObservation = 0;

const char *getResult(OCStackResult result);
void createREFERResource();

#define LENGTH_VAR		100
static int base_length = 0;

bool JsonGenerator( REFERResource& ref, char* jsonBuf, uint16_t buf_length )
{
	if( (buf_length - base_length) < LENGTH_VAR )
	{
		OC_LOG_V(ERROR, TAG, "Error : length is very long.");
		return false;
	}

	sprintf(jsonBuf, JSON_BASE00 JSON_BASE01"%s",ref.m_macaddress);
	sprintf(jsonBuf+strlen(jsonBuf), JSON_BASE02"%s"JSON_BASE03, ref.m_servicetype);

	Serial.println(jsonBuf);

	return true;
}

// On Arduino Atmel boards with Harvard memory architecture, the stack grows
// downwards from the top and the heap grows upwards. This method will print
// the distance(in terms of bytes) between those two.
// See here for more details :
// http://www.atmel.com/webdoc/AVRLibcReferenceManual/malloc_1malloc_intro.html
void PrintArduinoMemoryStats()
{
#ifdef ARDUINO_AVR_MEGA2560
    //This var is declared in avr-libc/stdlib/malloc.c
    //It keeps the largest address not allocated for heap
    extern char *__brkval;
    //address of tmp gives us the current stack boundry
    int tmp;
    OC_LOG_V(INFO, TAG, "Stack: %u         Heap: %u", (unsigned int)&tmp, (unsigned int)__brkval);
    OC_LOG_V(INFO, TAG, "Unallocated Memory between heap and stack: %u",
             ((unsigned int)&tmp - (unsigned int)__brkval));
#endif
}

// This is the entity handler for the registered resource.
// This is invoked by OCStack whenever it recevies a request for this resource.
OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest )
{
    OCEntityHandlerResult ehRet = OC_EH_OK;
    OCEntityHandlerResponse response = {0};
    char payload[MAX_RESPONSE_LENGTH] = {0};

    if(entityHandlerRequest && (flag & OC_REQUEST_FLAG))
    {
        OC_LOG (INFO, TAG, PCF("Flag includes OC_REQUEST_FLAG"));
        if(OC_REST_GET == entityHandlerRequest->method)
        {
        	 if(JsonGenerator( REFER, payload, MAX_RESPONSE_LENGTH))
        	 {
        	 }
           else
            {
                ehRet = OC_EH_ERROR;
            }
        }
        if(OC_REST_PUT == entityHandlerRequest->method)
        {
            //Do something with the 'put' payload
            if (JsonGenerator( REFER, payload, MAX_RESPONSE_LENGTH))
            {
            }
            else
            {
                ehRet = OC_EH_ERROR;
            }
         }
    }

    if (ehRet == OC_EH_OK)
   {
	   // Format the response.  Note this requires some info about the request
	   response.requestHandle = entityHandlerRequest->requestHandle;
	   response.resourceHandle = entityHandlerRequest->resource;
	   response.ehResult = ehRet;
	   response.payload = (unsigned char *)payload;
	   response.payloadSize = strlen(payload);
	   response.numSendVendorSpecificHeaderOptions = 0;
	   memset(response.sendVendorSpecificHeaderOptions, 0, sizeof response.sendVendorSpecificHeaderOptions);
	   memset(response.resourceUri, 0, sizeof response.resourceUri);
	   // Indicate that response is NOT in a persistent buffer
	   response.persistentBufferFlag = 0;

	   // Send the response
	   if (OCDoResponse(&response) != OC_STACK_OK)
	   {
		   OC_LOG(ERROR, TAG, "Error sending response");
		   ehRet = OC_EH_ERROR;
	   }
    }

    if (entityHandlerRequest && (flag & OC_OBSERVE_FLAG))
    {
        if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo.action)
        {
            OC_LOG (INFO, TAG, PCF("Received OC_OBSERVE_REGISTER from client"));
            g_REFERUnderObservation = 1;
        }
        else if (OC_OBSERVE_DEREGISTER == entityHandlerRequest->obsInfo.action)
        {
            OC_LOG (INFO, TAG, PCF("Received OC_OBSERVE_DEREGISTER from client"));
        }
    }

    return ehRet;
}


char tempdata[200];
void ChangeREFERRepresentation (void *param)
{
	(void)param;
	OCStackResult result = OC_STACK_ERROR;
	result = OCNotifyAllObservers (REFER.m_handle, OC_NA_QOS);

	OC_LOG_V(INFO, TAG, "%s", JsonGenerator( REFER, tempdata, 200));

	if (OC_STACK_NO_OBSERVERS == result)
	{
		OC_LOG_V(INFO, TAG, "g_REFERUnderObservation is 0." );
		g_REFERUnderObservation = 0;
	}
}

//The setup function is called once at startup of the sketch
void setup()
{
	Serial.begin(115200);

   REFER.m_macaddress = macaddress;
   REFER.m_servicetype = servicetype;

	// Add your initialization code here
	OC_LOG_INIT();

	OC_LOG(DEBUG, TAG, PCF("OCServer is starting..."));

	// Connect to Ethernet or WiFi network
	if (ConnectToNetwork() != 0)
	{
		OC_LOG(ERROR, TAG, "Unable to connect to network");
		return;
	}

	// Initialize the OC Stack in Server mode
	if (OCInit(NULL, OC_WELL_KNOWN_PORT, OC_SERVER) != OC_STACK_OK)
	{
		OC_LOG(ERROR, TAG, PCF("OCStack init error"));
		return;
	}

    OCStartPresence(60);
	// Declare and create the example resource: PROXI
	createREFERResource();

	ble.init( (long)115200, BLE_SLAVER,  REFER.m_macaddress);

//	ble.StatusRead();

	char str0[] = JSON_BASE00;
	char str1[] = JSON_BASE01;
	char str2[] = JSON_BASE02;
	char str3[] = JSON_BASE03;

	base_length = strlen(str0)+ strlen(str1)+ strlen(str2)+ strlen(str3);

	OC_LOG_V(INFO, TAG, "Program Start-\r\n");
}

// The loop function is called in an endless loop
void loop()
{
    // This artificial delay is kept here to avoid endless spinning
    // of Arduino microcontroller. Modify it as per specfic application needs.

    // This call displays the amount of free SRAM available on Arduino
    PrintArduinoMemoryStats();
	delay(5000);
	if (OCProcess() != OC_STACK_OK)
	{
		OC_LOG(ERROR, TAG, PCF("OCStack process error"));
		return;
	}
	ChangeREFERRepresentation(NULL);

	char* user_msg = NULL;
	user_msg = ble.Debug2BLE( true );
	ble.BLE2Debug( true );

	if ( user_msg )
	{
		free( user_msg );
		user_msg = NULL;
	}
}

void createREFERResource() {

    OCStackResult res = OCCreateResource(&REFER.m_handle,
                                         "SoftSensorManager.Sensor",
                                         "oc.mi.def",
                                         "/Reference_Thing",
                                         OCEntityHandlerCb,
                                         OC_DISCOVERABLE|OC_OBSERVABLE);
    OC_LOG_V(INFO, TAG, "Created REFER resource with result: %s", getResult(res));
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

