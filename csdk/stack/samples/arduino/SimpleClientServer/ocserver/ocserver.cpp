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

#include "logger.h"
#include "ocstack.h"
#include <string.h>

#ifdef ARDUINOWIFI
// Arduino WiFi Shield
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#else
// Arduino Ethernet Shield
#include <EthernetServer.h>
#include <Ethernet.h>
#include <Dns.h>
#include <EthernetClient.h>
#include <util.h>
#include <EthernetUdp.h>
#include <Dhcp.h>
#endif

const char *getResult(OCStackResult result);

PROGMEM const char TAG[] = "ArduinoServer";

int gLEDUnderObservation = 0;
void createLEDResource();
typedef struct LEDRESOURCE{
    OCResourceHandle handle;
    bool state;
    int power;
} LEDResource;

static LEDResource LED;

static char responsePayloadGet[] = "{\"href\":\"/a/led\",\"rep\":{\"state\":\"on\",\"power\":10}}";
static char responsePayloadPut[] = "{\"href\":\"/a/led\",\"rep\":{\"state\":\"off\",\"power\":0}}";

/// This is the port which Arduino Server will use for all unicast communication with it's peers
static uint16_t OC_WELL_KNOWN_PORT = 5683;

#ifdef ARDUINOWIFI
// Arduino WiFi Shield
// Note : Arduino WiFi Shield currently does NOT support multicast and therefore
// this server will NOT be listening on 224.0.1.187 multicast address.

/// WiFi Shield firmware with Intel patches
static const char INTEL_WIFI_SHIELD_FW_VER[] = "1.2.0";

/// WiFi network info and credentials
char ssid[] = "mDNSAP";
char pass[] = "letmein9";

int ConnectToNetwork()
{
    char *fwVersion;
    int status = WL_IDLE_STATUS;
    // check for the presence of the shield:
    if (WiFi.status() == WL_NO_SHIELD)
    {
        OC_LOG(ERROR, TAG, PCF("WiFi shield not present"));
        return -1;
    }

    // Verify that WiFi Shield is running the firmware with all UDP fixes
    fwVersion = WiFi.firmwareVersion();
    OC_LOG_V(INFO, TAG, "WiFi Shield Firmware version %s", fwVersion);
    if ( strncmp(fwVersion, INTEL_WIFI_SHIELD_FW_VER, sizeof(INTEL_WIFI_SHIELD_FW_VER)) !=0 )
    {
        OC_LOG(DEBUG, TAG, PCF("!!!!! Upgrade WiFi Shield Firmware version !!!!!!"));
        return -1;
    }

    // attempt to connect to Wifi network:
    while (status != WL_CONNECTED)
    {
        OC_LOG_V(INFO, TAG, "Attempting to connect to SSID: %s", ssid);
        status = WiFi.begin(ssid,pass);

        // wait 10 seconds for connection:
        delay(10000);
    }
    OC_LOG(DEBUG, TAG, PCF("Connected to wifi"));

    IPAddress ip = WiFi.localIP();
    OC_LOG_V(INFO, TAG, "IP Address:  %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return 0;
}
#else
// Arduino Ethernet Shield
int ConnectToNetwork()
{
    // Note: ****Update the MAC address here with your shield's MAC address****
    uint8_t ETHERNET_MAC[] = {0x90, 0xA2, 0xDA, 0x0E, 0xC4, 0x05};
    uint8_t error = Ethernet.begin(ETHERNET_MAC);
    if (error  == 0)
    {
        OC_LOG_V(ERROR, TAG, "error is: %d", error);
        return -1;
    }

    IPAddress ip = Ethernet.localIP();
    OC_LOG_V(INFO, TAG, "IP Address:  %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return 0;
}
#endif //ARDUINOWIFI

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

    if(entityHandlerRequest && (flag & OC_REQUEST_FLAG))
    {
        OC_LOG (INFO, TAG, PCF("Flag includes OC_REQUEST_FLAG"));
        if(OC_REST_GET == entityHandlerRequest->method)
        {
            if (strlen(responsePayloadGet) < entityHandlerRequest->resJSONPayloadLen)
            {
                strncpy((char *)entityHandlerRequest->resJSONPayload, responsePayloadGet, entityHandlerRequest->resJSONPayloadLen);
            }
            else
            {
                ehRet = OC_EH_ERROR;
            }
        }
        if(OC_REST_PUT == entityHandlerRequest->method)
        {
            //Do something with the 'put' payload
            if (strlen(responsePayloadPut) < entityHandlerRequest->resJSONPayloadLen)
            {
                strncpy((char *)entityHandlerRequest->resJSONPayload, responsePayloadPut, entityHandlerRequest->resJSONPayloadLen);
            }
            else
            {
                ehRet = OC_EH_ERROR;
            }
         }
    }
    if (entityHandlerRequest && (flag & OC_OBSERVE_FLAG))
    {
        if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo->action)
        {
            OC_LOG (INFO, TAG, PCF("Received OC_OBSERVE_REGISTER from client"));
            gLEDUnderObservation = 1;
        }
        else if (OC_OBSERVE_DEREGISTER == entityHandlerRequest->obsInfo->action)
        {
            OC_LOG (INFO, TAG, PCF("Received OC_OBSERVE_DEREGISTER from client"));
        }
    }

    return ehRet;
}


// This method is used to display 'Observe' functionality of OC Stack.
static uint8_t modCounter = 0;
void *ChangeLEDRepresentation (void *param)
{
    (void)param;
    OCStackResult result = OC_STACK_ERROR;
    modCounter += 1;
    if(modCounter % 10 == 0)  // Matching the timing that the Linux Sample Server App uses for the same functionality.
    {
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



//The setup function is called once at startup of the sketch
void setup()
{
    // Add your initialization code here

    // Note : This will initialize Serial port on Arduino at 115200 bauds
    OC_LOG_INIT();

    OC_LOG(DEBUG, TAG, PCF("OCServer is starting..."));
    uint16_t port = OC_WELL_KNOWN_PORT;

    // Connect to Ethernet or WiFi network
    if (ConnectToNetwork() != 0)
    {
        OC_LOG(ERROR, TAG, PCF("Unable to connect to network"));
        return;
    }

    // Initialize the OC Stack in Server mode
    if (OCInit(NULL, port, OC_SERVER) != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("OCStack init error"));
        return;
    }

    // Declare and create the example resource: LED
    createLEDResource();

}

// The loop function is called in an endless loop
void loop()
{
    // This artificial delay is kept here to avoid endless spinning
    // of Arduino microcontroller. Modify it as per specfic application needs.
    delay(2000);

    // This call displays the amount of free SRAM available on Arduino
    PrintArduinoMemoryStats();

    // Give CPU cycles to OCStack to perform send/recv and other OCStack stuff
    if (OCProcess() != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("OCStack process error"));
        return;
    }
    ChangeLEDRepresentation(NULL);
}

void createLEDResource()
{
    LED.state = false;
    OCStackResult res = OCCreateResource(&LED.handle,
            "core.led",
            "oc.mi.def",
            "/a/led",
            OCEntityHandlerCb,
            OC_DISCOVERABLE|OC_OBSERVABLE);
    OC_LOG_V(INFO, TAG, "Created LED resource with result: %s", getResult(res));
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
