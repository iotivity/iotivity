/******************************************************************
*
* Copyright 2018 Open Connectivity Foundation All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "dependencies/common.h"
#if USE_HW
    #include "common.h"
    #include "MySignals.h"
    #include "MySignals_BLE.h"
    #define USE_MYSIGNALS 1
#else
    #include <string.h>
    #include <inttypes.h>
    #define USE_MYSIGNALS 0
#endif
#include "MySignals/bloodpressure.h"


//!Struct to store data of the BloodPressure
struct bloodPressureBLEDataVector
{
  uint16_t systolic;
  uint16_t diastolic;
  uint16_t pulse;
};

//!Vector to store the glucometer measures and dates.
bloodPressureBLEDataVector bloodPressureBLEData;

// Write here the MAC address of BLE device to find
char MAC_BP[14] = "508CB1664FA6";

uint8_t available_bp = 0;
uint8_t connected_bp = 0;
uint8_t connection_handle_bp = 0;

#define BP_HANDLE 18

uint16_t getSystolic() {
    return bloodPressureBLEData.systolic;
}

uint16_t getDiastolic() {
    return bloodPressureBLEData.diastolic;
}

uint16_t getPulseRate() {
    return bloodPressureBLEData.pulse;
}


uint8_t getMySignalBloodPressure() {
    uint8_t ret = 0;
    #if USE_HW
    char strbuf[30] = {0x00, };
    available_bp = MySignals_BLE.scanDevice(MAC_BP, 1000, TX_POWER_MAX);

    MySignals.disableMuxUART();

    memset(strbuf, 0x00, 30);
    sprintf(strbuf, "BP available: %d\n", available_bp);
    printf(strbuf);

    MySignals.enableMuxUART();

    printf ("start access \n");
    if (available_bp == 1)
    {

        if (MySignals_BLE.connectDirect(MAC_BP) == 1)
        {
            MySignals.println("Connected");

            connected_bp = 1;
            delay(8000);
            uint8_t data = 'e';
            //To subscribe the BP measure write an ASCII letter "e"
            if (MySignals_BLE.attributeWrite(MySignals_BLE.connection_handle,  BP_HANDLE, &data, 1) == 0)
            {

                MySignals.println("Subscribed");

                unsigned long previous = millis();
                do
                {

                    if (MySignals_BLE.waitEvent(1000) == BLE_EVENT_ATTCLIENT_ATTRIBUTE_VALUE)
                    {
                        //Search letter "g" in ASCII
                        if (MySignals_BLE.event[9] == 0x67)
                        {

                            if (MySignals_BLE.event[10] == 0x2f)
                            {
                            //Ojo esto esta mal en la guia de comandos

                            //Primero da la alta -> sistolica
                            uint8_t sh = MySignals_BLE.event[11] - 48;
                            uint8_t sm = MySignals_BLE.event[12] - 48;
                            uint8_t sl = MySignals_BLE.event[13] - 48;
                            bloodPressureBLEData.systolic = (sh * 100) + (sm * 10) + sl;

                            //Primero da la baja -> diastolica
                            uint8_t dh = MySignals_BLE.event[15] - 48;
                            uint8_t dm = MySignals_BLE.event[16] - 48;
                            uint8_t dl = MySignals_BLE.event[17] - 48;
                            bloodPressureBLEData.diastolic = (dh * 100) + (dm * 10) + dl;

                            uint8_t ph = MySignals_BLE.event[19] - 48;
                            uint8_t pm = MySignals_BLE.event[20] - 48;
                            uint8_t pl = MySignals_BLE.event[21] - 48;
                            bloodPressureBLEData.pulse = (ph * 100) + (pm * 10) + pl;


                            MySignals.disableMuxUART();
                            memset(strbuf, 0x00, 30);
                            sprintf(strbuf, "Systolic: %d", bloodPressureBLEData.systolic);
                            MySignals.println(&(strbuf[0]));

                            memset(strbuf, 0x00, 30);
                            sprintf(strbuf, "Diastolic: %d", bloodPressureBLEData.diastolic);
                            MySignals.println(&(strbuf[0]));

                            memset(strbuf, 0x00, 30);
                            sprintf(strbuf, "Pulse/min: %d", bloodPressureBLEData.pulse);
                            MySignals.println(&(strbuf[0]));

                            MySignals.enableMuxUART();

                            connected_bp = 0;
                            ret = 1;
                            }
                        }
                    }
                }
                while ((connected_bp == 1) && ((millis() - previous) < 40000));

                data = 'i';
                MySignals_BLE.attributeWrite(MySignals_BLE.connection_handle,  BP_HANDLE, &data , 1);
                delay(100);


                MySignals_BLE.disconnect(MySignals_BLE.connection_handle);

                connected_bp = 0;

            }
            else
            {
                MySignals.println("Error subscribing");
            }

        }
        else
        {
            connected_bp = 0;
            MySignals.println("Not Connected");
        }
    }
    else if (available_bp == 0)
    {
        //Do nothing
    }
    else
    {
        MySignals_BLE.hardwareReset();
        MySignals_BLE.initialize_BLE_values();

    }
    printf ("end access \n");
    #endif
    return ret;
}

uint8_t startBloodPressure() {
    memset(&bloodPressureBLEData, 0x00, sizeof(bloodPressureBLEDataVector));
#if USE_MYSIGNALS
    uint8_t ret = initMySignals();
    if(ret == 1) {
        ret = getMySignalBloodPressure();
    }
    return ret;
#else
    return 0;
#endif
}