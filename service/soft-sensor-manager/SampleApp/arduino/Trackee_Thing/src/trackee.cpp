/******************************************************************
*
* Copyright 2014 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/



// Do not remove the include below
#include "Arduino.h"
#include "bleLib.h"
#include <stdio.h>

#include "logger.h"
#include "ocstack.h"
#include <string.h>

#include "oic_lanLib.h"
#include "trackee.h"

// proximity code s
#define DATA_EA     400
#define SLAVER_EA   2

#define ARDUINO_AVR_MEGA2560 1
/// This is the port which Arduino Server will use for all unicast communication with it's peers
#define OC_WELL_KNOWN_PORT 5683

PROGMEM const char TAG[] = "TrackeeSensor";

OCResourceHandle m_handle;

#if (ARDUINO == 0)
#include "proximity.h"

#define JSON_BASE "{\"href\":\"\",\"rep\":{"
#define JSON_BASE00 "\"0\":\"trackeeID\",\"1\":\"string\",\"2\":\""
#define JSON_BASE01 "\",\"3\":\"things\",\"4\":\"int\",\"5\":\""
#define JSON_BASE02 "\",\"6\":\"ID\",\"7\":\"string\",\"8\":\""
#define JSON_BASE03 "\",\"9\":\"distance\",\"10\":\"float\",\"11\":\""
#define JSON_BASE04 "\",\"12\":\"proximity\",\"13\":\"int\",\"14\":\""
#define JSON_BASE05 "\",\"15\":\"SD\",\"16\":\"float\",\"17\":\""
#define JSON_BASE06 "\",\"18\":\"ID\",\"19\":\"string\",\"20\":\""
#define JSON_BASE07 "\",\"21\":\"distance\",\"22\":\"float\",\"23\":\""
#define JSON_BASE08 "\",\"24\":\"proximity\",\"25\":\"int\",\"26\":\""
#define JSON_BASE09 "\",\"27\":\"SD\",\"28\":\"float\",\"29\":\""
#define JSON_BASE10 "\"}}"

typedef struct PROXIRESOURCE
{
    float m_distance[SLAVER_EA];
    float m_proximity[SLAVER_EA];
} PROXIResource;

PROXIResource PROXI;

int rssi[SLAVER_EA][arraysize];
int rssicnt[SLAVER_EA] = {0,};
int startindex[SLAVER_EA] = {0,};
int flag[SLAVER_EA] = {0,};
//#else
//bool bleWrite = true;
#endif

Cble ble;
//char trackeeID[13] = "9059AF170C1D";
char trackeeID[13] = "9059AF16FEF7";
int slaver_num = 0;
//char slaveList[SLAVER_EA][13]={"9059AF1704D7"};
char slaveList[SLAVER_EA][13] = {"9059AF1700EE", "34B1F7D004D2"};
int g_PROXIUnderObservation = 0;



const char *getResult(OCStackResult result);
void createResource();


#define LENGTH_VAR      50
bool JsonGenerator( char *jsonBuf, uint16_t buf_length )
{
    if ( g_PROXIUnderObservation == 1 )
    {
#if (ARDUINO == 0)
        PROXIResource *ref = &PROXI;
        int ref_cnt = SLAVER_EA;
        uint16_t base_length = 0;
        base_length = strlen(JSON_BASE) + strlen(JSON_BASE00) + strlen(JSON_BASE01) + strlen(
                          JSON_BASE02) + strlen(JSON_BASE03) + \
                      strlen(JSON_BASE04) + strlen(JSON_BASE05) + strlen(JSON_BASE06) + strlen(JSON_BASE07) + \
                      strlen(JSON_BASE08) + strlen(JSON_BASE09) + strlen(JSON_BASE10);

//      base_length = strlen(JSON_BASE00)+ strlen(JSON_BASE01)+ strlen(JSON_BASE02)+ strlen(JSON_BASE03)+\
//                    strlen(JSON_BASE04)+ strlen(JSON_BASE05)+ strlen(JSON_BASE10);

        OC_LOG_V(INFO, TAG, "base length = %d, buf_length=%d", base_length, buf_length );

        if ( ((long)buf_length - (long)base_length) < LENGTH_VAR )
        {
            OC_LOG_V(ERROR, TAG, "Error : length is very long.");
            return false;
        }

        sprintf(jsonBuf, JSON_BASE );
        sprintf(jsonBuf + strlen(jsonBuf), JSON_BASE00"%s"JSON_BASE01"%d", trackeeID, ref_cnt);
        sprintf(jsonBuf + strlen(jsonBuf), JSON_BASE02"%s"JSON_BASE03"%d.%03d", slaveList[0],
                (int)ref->m_distance[0], (int)((ref->m_distance[0] - (int)ref->m_distance[0]) * 1000.0) );
        sprintf(jsonBuf + strlen(jsonBuf), JSON_BASE04"%d"JSON_BASE05"0.0", (int)ref->m_proximity[0] );
        sprintf(jsonBuf + strlen(jsonBuf), JSON_BASE06"%s"JSON_BASE07"%d.%03d", slaveList[1],
                (int)ref->m_distance[1], (int)((ref->m_distance[1] - (int)ref->m_distance[1]) * 1000.0) );
        sprintf(jsonBuf + strlen(jsonBuf), JSON_BASE08"%d"JSON_BASE09"0.0", (int)ref->m_proximity[1] );
        strcpy(jsonBuf + strlen(jsonBuf), JSON_BASE10 );

        OC_LOG_V(INFO, TAG, "json length = %u", strlen(jsonBuf) );

        return true;

#else       // Slave Json Code.

        return true;
#endif
    }

    OC_LOG_V(INFO, TAG, "Not Support Observer unfounded mode." );
    return false;
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
OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag,
                                        OCEntityHandlerRequest *entityHandlerRequest )
{
    OCEntityHandlerResult ehRet = OC_EH_OK;

    if (entityHandlerRequest && (flag & OC_REQUEST_FLAG))
    {
        OC_LOG (INFO, TAG, PCF("Flag includes OC_REQUEST_FLAG"));
        if (OC_REST_GET == entityHandlerRequest->method)
        {
            if (JsonGenerator((char *)entityHandlerRequest->resJSONPayload, \
                              entityHandlerRequest->resJSONPayloadLen))
            {
            }
            else
            {
                ehRet = OC_EH_ERROR;
            }
        }
        if (OC_REST_PUT == entityHandlerRequest->method)
        {
            //Do something with the 'put' payload
            if (JsonGenerator((char *)entityHandlerRequest->resJSONPayload, \
                              entityHandlerRequest->resJSONPayloadLen))
            {
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
            g_PROXIUnderObservation = 1;
        }
        else if (OC_OBSERVE_DEREGISTER == entityHandlerRequest->obsInfo->action)
        {
            OC_LOG (INFO, TAG, PCF("Received OC_OBSERVE_DEREGISTER from client"));
        }
    }

    return ehRet;
}


#if (ARDUINO == 0)
void ChangePROXIRepresentation (void *param)
{
    (void)param;
    OCStackResult result = OC_STACK_ERROR;
    float avg[SLAVER_EA] = {0,};

    for (int i = 0; i < SLAVER_EA; i++)
    {
        if ( rssicnt[i] > arraysize - 1)
        {
            rssicnt[i] = rssicnt[i] % arraysize;
        }

        ble.streamDummy(NULL, NULL);
        /*  if( ble.pollingConnect(&slaveList[slaver_num][0]) == false )
            {
                OC_LOG_V("Error : Not Connected.\r\n");
            }*/
        while (ble.pollingConnect(&slaveList[slaver_num][0]) == false)
        {
            ble.streamDummy(NULL, NULL);
        }

        if ( ble.IsConnected() == true )
        {
            // print the string when a newline arrives:
            OC_LOG_V(INFO, TAG, "Connected. (%s)\r\n", slaveList[slaver_num]);

//          time = millis();
            for (int j = 0; j < RSSI_EA; j++)
            {
                rssi[i][rssicnt[i]] = ble.pollingGetRSSI();
                OC_LOG_V(INFO, TAG, "rssi val : %d \r\n", rssi[i][rssicnt[i]]);
                rssicnt[i]++;
            }
//          oldTime = millis();
//          float TpR = (float)(((oldTime-time)/1000.0)/20.0);
//          OC_LOG_V("time per rssi : %d.%03d\r\n", (int)TpR, (int)(TpR*1000.0 -((int)TpR)*1000.0) );

            while ( ble.IsSelfArduino() == false )
                ble.pollingDisconnect();

            slaver_num++;
            slaver_num = slaver_num % SLAVER_EA;
        }

        avg[i] = CalculateExponentialAverage(RSSI_EA, rssi[i], startindex[i], flag[i]);
        Serial.println(avg[i]);
        Serial.print("distance : ");

        PROXI.m_distance[i] = calculateDistance(avg[i], -58);

        if (PROXI.m_distance[i] <= 1)
        {
            PROXI.m_proximity[i] = 1;
        }
        else if (PROXI.m_distance[i] <= 2)
        {
            PROXI.m_proximity[i] = 2;
        }
        else
        {
            PROXI.m_proximity[i] = 3;
        }

        Serial.println(PROXI.m_distance[i]);
        Serial.println(PROXI.m_proximity[i]);
        //Serial.println(calculateDistance(avg[i], -58));
        startindex[i] += RSSI_EA;

        // This call displays the amount of free SRAM available on Arduino
        PrintArduinoMemoryStats();

        if (startindex[i] >= arraysize)
        {
            startindex[i] = 0;
        }

        if (flag[i] < (arraysize / RSSI_EA))
        {
            flag[i]++;
        }
    }

    result = OCNotifyAllObservers (m_handle, OC_NA_QOS);

    if (OC_STACK_NO_OBSERVERS == result)
    {
        OC_LOG_V(INFO, TAG, "g_PROXIUnderObservation is 0." );
        g_PROXIUnderObservation = 0;
    }

}
#endif




//The setup function is called once at startup of the sketch
void setup()
{
    Serial.begin(115200);

    // Add your initialization code here
    OC_LOG_INIT();

    OC_LOG(DEBUG, TAG, PCF("OCServer is starting..."));
    //    uint16_t port = OC_WELL_KNOWN_PORT;

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
    // Declare and create the example resource
    createResource();

    // This call displays the amount of free SRAM available on Arduino
    PrintArduinoMemoryStats();
#if (ARDUINO == 0)
    ble.init( (long)115200, BLE_MASTER, trackeeID);
#elif (ARDUINO == 1)
    ble.init( (long)115200, BLE_SLAVER, slaveList[0]);
#elif (ARDUINO == 2)
    ble.init( (long)115200, BLE_SLAVER, slaveList[1]);
#endif

//  ble.StatusRead();

    OC_LOG_V(INFO, TAG, "Program Start-\r\n");
}


// The loop function is called in an endless loop
void loop()
{
    // This artificial delay is kept here to avoid endless spinning
    // of Arduino microcontroller. Modify it as per specfic application needs.

    if (OCProcess() != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, PCF("OCStack process error"));
        return;
    }
#if (ARDUINO == 0)
    ChangePROXIRepresentation(NULL);
#else
    char *user_cmd = NULL;

//  ble.pollingDisconnect();

    user_cmd = ble.Debug2BLE(true);
    ble.BLE2Debug( true );

    if ( user_cmd )
    {
        free( user_cmd );
        user_cmd = NULL;
    }

#endif
}







void createResource()
{

    OCStackResult res = OCCreateResource(&m_handle,
                                         "SSManager.Sensor",
                                         OC_RSRVD_INTERFACE_DEFAULT,
                                         "/Tracker_Thing",
                                         OCEntityHandlerCb,
                                         OC_DISCOVERABLE | OC_OBSERVABLE);
    OC_LOG_V(INFO, TAG, "Created PROXI resource with result: %s", getResult(res));
}

const char *getResult(OCStackResult result)
{
    switch (result)
    {
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

