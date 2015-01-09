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

#ifndef BLELIB_H_
#define BLELIB_H_

#include "Arduino.h"

#define BLE_NOCHANGE    -1
#define BLE_MASTER      1
#define BLE_SLAVER      0

#define BLE_DISCON      200
#define BLE_NODATA      100


/***************************************
 *
 *  HM-10 BLE Module Control Class.
 *  Target Board : Arduino-Mega.
 *
 **************************************/
class Cble
{
    public :
        /***
         * BLE Control Function.
         */
        // BLE initialization ( COM baud rate , BLE Master , Slave setting. )
        void init(long baudRate, int ble_mode, char *SelfMaxAddr);

        void StatusRead( void );

        bool IsSelfArduino( void );

        bool IsConnected( void );

        bool pollingConnect( const char *maxAddr );

        void pollingDisconnect( void );
        // Get RSSI by BLE communication. (polling method.)
        int pollingGetRSSI( void );
        // Send Request to BLE Module. And, Get the response. (polling method.)
        bool pollingRequest(const char *request, char *data, int dataLen);

        int mustHaveRequest(const char *request, char *data, int dataLen);

        void streamDummy( char *data, int dataLen);

        /***
         * Debug COM port to BLE COM port.
         * BLE COM port to Debug COM port.
         */
        // BLE Module to Debug Serial port.
        void BLE2Debug( int autoDiscon );
        // Debug Serial port to BLE Module.
        char *Debug2BLE( int BLEwrite );

        /***
         * If you first setting, you need next-function.
         */
        // Restore to factory setting data.
        void FactoryReset( void );
        // First setting of COM port.
        void firstUartSetting( void );
};



#if 0
#define debug_printf(...)   {                           \
    char* __temp__ = (char*)malloc(sizeof(char)*2048);      \
    sprintf(__temp__, __VA_ARGS__ );                    \
    Serial.print(__temp__);                                 \
    free(__temp__);                                                 \
}
#else
#define debug_printf(...)
#endif



#endif /* BLELIB_H_ */
