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

#define PCF(str) ((const prog_char*)(F(str)))
#define dht11_pin 7  //Temperature & Humidity pin

const prog_char TAG[] PROGMEM = "THServer";

int gQuitFlag = 0;
int gTHUnderObservation = 0;

void createTHResource();
typedef struct THRESOURCE {
    OCResourceHandle handle;
    int temp;
    int humid;
} THResource;

static THResource TH;

String responsePayloadPut = "{\"oc\":[{\"href\":\"\",\"rep\":{\"temperature\":\"0\",\"humidity\":\"0\"}}]}";
String responsePayloadGet = "{\"oc\":[{\"href\":\"\",\"rep\":{\"temperature\":\"0\",\"humidity\":\"0\"}}]}";


/* read pin data */
byte read_dht11_dat()
{
  byte i = 0;
  byte result=0;
  for(i=0; i< 8; i++)
  {
    while (!digitalRead(dht11_pin));
    delayMicroseconds(30);
    if (digitalRead(dht11_pin) != 0 )
      bitSet(result, 7-i);
    while (digitalRead(dht11_pin));
  }
  return result;
}

/* Json Generator */
String JsonGenerator(THResource TH){
   String a = "{\"oc\":[{\"href\":\"\",\"rep\":{\"temperature\":\"";
   String b = "\",\"humidity\":\"";
   String c = "\"}}]}";

   String ss;

    ss = a + TH.temp + b + TH.humid + c;
	return ss;
}

static uint8_t modCounter = 0;
void *ChangeTHRepresentation (void *param)
{
    (void)param;
    OCStackResult result = OC_STACK_ERROR;

    modCounter += 1;
    if(modCounter % 10 == 0)  // Matching the timing that the Linux Server App uses for the same functionality.
    {
         /* Temperature & Humidity value check */
         byte dht11_dat[5];
         byte i;// start condition

         digitalWrite(dht11_pin, LOW);
         delay(18);
         digitalWrite(dht11_pin, HIGH);
         delayMicroseconds(1);

         pinMode(dht11_pin, INPUT);
         delayMicroseconds(40);

         if (digitalRead(dht11_pin))
         {
           Serial.println("dht11 start condition 1 not met");  // wait for DHT response signal: LOW
           delay(1000);
           return NULL;

         }
         delayMicroseconds(80);
         if (!digitalRead(dht11_pin))
         {
           Serial.println("dht11 start condition 2 not met");  // wait for second response signal:HIGH
           return NULL;
         }

         delayMicroseconds(80);				// now ready for data reception
         for (i=0; i<5; i++)
         {  dht11_dat[i] = read_dht11_dat();}  		// recieved 40 bits data. 

         pinMode(dht11_pin, OUTPUT);
         digitalWrite(dht11_pin, HIGH);
         byte dht11_check_sum = dht11_dat[0]+dht11_dat[2];     // check check_sum
         if(dht11_dat[4]!= dht11_check_sum)
         {
           Serial.println("DHT11 checksum error");
         }

         delay(2000); 					// fresh time
         TH.humid = dht11_dat[0];
         TH.temp = dht11_dat[2];

         responsePayloadGet = JsonGenerator(TH);

/* observe */
        if (gTHUnderObservation)
        {
            OC_LOG_V(INFO, TAG, " =====> Notifying stack of new humid level %d\n", TH.humid);
            OC_LOG_V(INFO, TAG, " =====> Notifying stack of new temp level %d\n", TH.temp);

            result = OCNotifyObservers (TH.handle);

            if (OC_STACK_NO_OBSERVERS == result)
            {
                gTHUnderObservation = 0;
            }
        }
    }
    return NULL;
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
    if(entityHandlerRequest && flag == OC_REQUEST_FLAG) { 
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

/* The setup function is called once at startup of the sketch */
void setup()
{
    pinMode(dht11_pin, OUTPUT);   
    digitalWrite(dht11_pin, HIGH);

    OC_LOG_INIT();

    OC_LOG(DEBUG, TAG, PCF("THServer is starting..."));
    uint16_t port = USE_RANDOM_PORT;

    /* Mac address of my ethernet shield */
    uint8_t ETHERNET_MAC[] = {0x90, 0xA2, 0xDA, 0x0E, 0xB8, 0xAB};
    uint8_t error = Ethernet.begin(ETHERNET_MAC);
    Serial.print(Ethernet.localIP());
    if (error  == 0)
    {
        OC_LOG_V(ERROR, TAG, "error is: %d", error);
        return;
    }

    if (OCInit(NULL, port, OC_SERVER) != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("OCStack init error"));
        return;
    }

    /*
     * Declare and create the resource: TH
     */

    createTHResource();

}

/* The loop function is called in an endless loop */
void loop()
{
    delay(1000);
    if (OCProcess() != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("OCStack process error"));
        return;
    }
    ChangeTHRepresentation(NULL);
}

void createTHResource() {
    TH.humid = 0;
    TH.temp = 0;

    OCStackResult res = OCCreateResource(&TH.handle,
                                         "SoftSensorManager.Sensor",
                                         "oc.mi.def",
                                         "/Thing_TempHumSensor",
                                         OCEntityHandlerCb,
                                         OC_DISCOVERABLE|OC_OBSERVABLE);
    OC_LOG_V(INFO, TAG, "Created TH resource with result: %s", getResult(res));
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


