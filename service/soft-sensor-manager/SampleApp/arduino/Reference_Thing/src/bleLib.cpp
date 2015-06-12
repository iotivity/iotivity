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

#include "bleLib.h"
#include <stdarg.h>

//#define __INTERNAL_DEBUG__        1

#define LIMIT_COUNT         100
#define DUMMY_SIZE          30
#define DEBUG_SIZE          DUMMY_SIZE
#define SERIAL_SIZE         100

#define BLE     Serial2

bool isConnected = false;
int SerialCnt = 0;
int BLECnt = 0;
char SelfMaxaddr[19] = {0,};

char debugD[DEBUG_SIZE] = {0,};


char SerialData[SERIAL_SIZE] = {0,};
char BLEData[SERIAL_SIZE] = {0,};


void Cble::init(long baudRate, int ble_mode, char *SelfMaxAddr)
{
    debug_printf("BLE shiled is initialed.-\r\n");

    if (SelfMaxAddr == NULL && ble_mode != BLE_NOCHANGE )
    {
        debug_printf("Error : Insert Maxaddress of Arduino BLE shiled.\r\n");
        exit(-1);
    }
    sprintf(SelfMaxaddr, "%s", SelfMaxAddr);

    BLE.begin(baudRate);
    BLE.setTimeout(1);
    delay(1000);

    memset(debugD, 0, DEBUG_SIZE);

    if ( ble_mode != BLE_NOCHANGE )
    {
        while ( IsSelfArduino() == false )
            this->pollingDisconnect();
    }

    if ( ble_mode == BLE_MASTER )
    {
        pollingRequest("AT+ROLE1", debugD, 8);
        memset(debugD, 0, DEBUG_SIZE);
        delay(150);
        pollingRequest("AT+MODE0", debugD, 8);
        memset(debugD, 0, DEBUG_SIZE);
        delay(150);
        pollingRequest("AT+NOTI0", debugD, 8);
        memset(debugD, 0, DEBUG_SIZE);
    }
    else if ( ble_mode == BLE_SLAVER )
    {
        pollingRequest("AT+ROLE0", debugD, 8);
        memset(debugD, 0, DEBUG_SIZE);
        delay(150);
        pollingRequest("AT+MODE2", debugD, 8);
        memset(debugD, 0, DEBUG_SIZE);
        delay(150);
        pollingRequest("AT+NOTI0", debugD, 8);
        memset(debugD, 0, DEBUG_SIZE);
    }

    if ( ble_mode != BLE_NOCHANGE )
    {
        while ( IsSelfArduino() == false )
            this->pollingDisconnect();
    }

    delay(250);
}

void Cble::StatusRead( void )
{
    debug_printf("StatusRead function called.\r\n");

    pollingRequest("AT+ROLE?", debugD, 9);
    memset(debugD, 0, DEBUG_SIZE);
    delay(150);
    pollingRequest("AT+MODE?", debugD, 8);
    memset(debugD, 0, DEBUG_SIZE);
    delay(150);
    pollingRequest("AT+IMME?", debugD, 9);
    memset(debugD, 0, DEBUG_SIZE);
    delay(150);
    pollingRequest("AT+TYPE?", debugD, 8);
    memset(debugD, 0, DEBUG_SIZE);
    delay(150);
    pollingRequest("AT+POWE?", debugD, 8);
    memset(debugD, 0, DEBUG_SIZE);
    delay(150);
    pollingRequest("AT+NOTI?", debugD, 8);
    memset(debugD, 0, DEBUG_SIZE);
    delay(150);
    pollingRequest("AT+PIO1?", debugD, 8);
    memset(debugD, 0, DEBUG_SIZE);
    delay(150);
}

bool Cble::IsConnected( void )
{
    return isConnected;
}

bool Cble::IsSelfArduino( void )
{
#ifdef __INTERNAL_DEBUG__
    debug_printf("IsSelfArduino is called.\r\n");
#endif
    int length = mustHaveRequest("AT+ADDR?", debugD, 20);

    if ( strstr(debugD, SelfMaxaddr) )
    {
        isConnected = false;
        memset(debugD, 0, length);
        return true;
    }
    else
    {
        isConnected = true;
        memset(debugD, 0, length);
        return false;
    }
}

bool Cble::pollingConnect( const char *maxAddr )
{
#ifdef __INTERNAL_DEBUG__
    debug_printf("pollingConnect is called.\r\n");
#endif

#define CONNECT_SIZE        20

    char cmd[CONNECT_SIZE] = {0,};

    // is Connected Address ?
    pollingRequest("AT+ADDR?", debugD, 20);
    if ( strstr(debugD, maxAddr) )
    {
        isConnected = true;
        memset(debugD, 0, DEBUG_SIZE);
        return isConnected;
    }
    memset(debugD, 0, DEBUG_SIZE);

    // Try Connection.
    sprintf(cmd, "AT+CON%s", maxAddr);
    if ( pollingRequest(cmd, debugD, 8) == false )
    {
        debug_printf("Error : %s command is failed.\r\n", cmd );
        memset(debugD, 0, DEBUG_SIZE);
        pollingDisconnect();
        return false;
    }
    else if ( strstr(debugD, "OK+CONNA") )
    {
        isConnected = true;
        memset(debugD, 0, DEBUG_SIZE);
    }
    else
    {
        memset(debugD, 0, DEBUG_SIZE);
        pollingDisconnect();
        return false;
    }

    // Confirm Connected Address.
    pollingRequest("AT+ADDR?", debugD, 20);

    if ( strstr(debugD, maxAddr) == 0 )
    {
        isConnected = false;
        if ( strstr(debugD, "OK+CONNF") )
        {
            memset(debugD, 0, DEBUG_SIZE);
            streamDummy(debugD, 8);
        }
    }
    memset(debugD, 0, DEBUG_SIZE);
    delay(5);

    return isConnected;

#undef CONNECT_SIZE
}


#define  CMD_DISCONNECT     "(CMD_DISCON)"
void Cble::pollingDisconnect( void )
{
#ifdef __INTERNAL_DEBUG__
    debug_printf("pollingDisconnect is called.\r\n");
#endif
    int length = 0;

    if ( BLE.available() )
    {
        if ( BLE.findUntil(CMD_DISCONNECT, "\r\n") == true )
        {
            debug_printf("pollingDisconnect : Detected %s command.\r\n", CMD_DISCONNECT);

            do
            {
                debug_printf("pollingDisconnect : Send AT message.\r\n");
                if ( length )
                    memset(debugD, 0, length);

                length = mustHaveRequest("AT", debugD, 2);
            }
            while ( strstr(debugD, "OK") == NULL );

            debug_printf("pollingDisconnect() is Success.\r\n");
            memset(debugD, 0, length);
        }
    }

    delay(5);
}

int Cble::pollingGetRSSI( void )
{
#define RSSI_SIZE       11
#define RSSI_POS        7

    long time_cnt = 0;
    char c = 0;
    char index = 0;
    char rssi_res[RSSI_SIZE] = {0,};

    streamDummy(NULL, NULL);

    if ( isConnected == true )
    {
        BLE.write("AT+RSSI?");
        BLE.flush();
        delay(5);

        while (1)
        {
            if ( BLE.available() )
                c = (char)BLE.read();
            else
            {
                c = 0;
                time_cnt++;
            }

            if ( c )
            {
                rssi_res[index] = c;
                index++;
                if ( index == RSSI_SIZE - 1 )
                {
                    rssi_res[index] = '\0';
                    goto RESULT;
                }
            }

            //      debug_printf("time_cnt=%d\r\n", time_cnt);
            if ( time_cnt == 0xFFFFF )
            {
                debug_printf("Error : Time Out GetRSSI().\r\n");
                return BLE_NODATA;
            }

        }

RESULT :
#ifdef __INTERNAL_DEBUG__
        debug_printf("res= %s \r\n", rssi_res);
#endif
        char *Srssi = &(rssi_res[RSSI_POS]);
        //  debug_printf("[S] rssi=%s\r\n", Srssi);
        int rssi = atoi(Srssi);
        //  debug_printf("rssi=%d\r\n", rssi);

        return rssi;
    }

    return BLE_DISCON;

#undef RSSI_POS
#undef RSSI_SIZE
}

int Cble::mustHaveRequest(const char *request, char *data, int dataLen)
{
    int cnt = 0;
    int length = 0;
    int required_length = dataLen;
    char *data_index = data;

    BLE.write(request);
    BLE.flush();
    delay(150);

READ_STREAM :
    while ( (length = BLE.readBytes(data_index, required_length)) == 0 )
    {
        cnt++;
        delay(10);

        if ( cnt >= LIMIT_COUNT / 10 )
        {
#ifdef __INTERNAL_DEBUG__
            debug_printf("=====> Retry Request command Send. <=========\r\n");
#endif
            cnt = 0;
            BLE.write(request);
            BLE.flush();

            required_length = dataLen;
            data_index = data;
            length = 0;
            delay(50);
        }
    }

    required_length -= length;
    if ( required_length != 0 )
    {
        data_index = data_index + length;
        length = 0;
        goto READ_STREAM;
    }

#ifdef __INTERNAL_DEBUG__
    debug_printf("[ %s ] %s\r\n\r\n", request, data);
#endif
    return dataLen;
}

bool Cble::pollingRequest(const char *request, char *data, int dataLen)
{
    int cnt = 0;
    int length = 0;
    int required_length = dataLen;
    char *data_index = data;

    BLE.write(request);
    BLE.flush();

    if ( data_index )
    {
        delay(250);

READ_STREAM :
        while ( (length = BLE.readBytes(data_index, required_length)) == 0 )
        {
            cnt++;
            if ( cnt >= LIMIT_COUNT )
            {
                debug_printf("[ %s ] TimeOut : No data.\r\n\r\n", request);
                return false;
            }
        }

        required_length -= length;
        if ( required_length != 0 )
        {
            data_index = data_index + length;
            length = 0;
            goto READ_STREAM;
        }

#ifdef __INTERNAL_DEBUG__
        debug_printf("[ %s ] %s\r\n\r\n", request, data);
#endif
    }
    return true;
}

void Cble::streamDummy( char *data, int dataLen)
{
    int cnt = 0;
    int length = 0;
    int required_length = dataLen;
    char *data_index = data;

    if ( data && required_length )
    {
READ_STREAM :
        while ( (length = BLE.readBytes(data_index, required_length)) == 0 )
        {
            cnt++;
            if ( cnt >= LIMIT_COUNT )
            {
                debug_printf("[ streamDummy ] TimeOut : No data.\r\n\r\n");
                return ;
            }
        }

        required_length -= length;
        if ( required_length != 0 )
        {
            data_index = data_index + length;
            length = 0;
            goto READ_STREAM;
        }

        debug_printf("[ streamDummy ] %s\r\n\r\n", data);
    }
    else
    {
        if ( BLE.available() )
        {
            BLE.readBytes(debugD, DEBUG_SIZE);
#ifdef __INTERNAL_DEBUG__
            debug_printf("[ streamDummy ] %s\r\n\r\n", debugD);
#endif
            memset(debugD, 0, DEBUG_SIZE);
        }
    }

}


#define REGARD_DISCON           5000
int needDiscon = 0;
unsigned long global_cnt = 0;

void Cble::BLE2Debug( int autoDiscon )
{
    int cnt = 0;
    uint8_t length = 0;

    if ( BLE.available() )
    {
        while ( (length = BLE.readBytes(BLEData, SERIAL_SIZE)) == 0 )
        {
            cnt++;
            if ( cnt >= LIMIT_COUNT )
                return ;
        }

        global_cnt = 0;
        needDiscon = 0;

        Serial.println(BLEData);
        memset(BLEData, 0, length);
    }
    else if ( autoDiscon )
    {
        global_cnt++;
//      debug_printf("global_cnt=%u , ", global_cnt );
//      debug_printf("needDiscon=%d\r\n", needDiscon);
        if ( !needDiscon && global_cnt >= REGARD_DISCON )
        {
            needDiscon = 1;
            debug_printf("result : global_cnt=%u , ", global_cnt );
            debug_printf("needDiscon=%d\r\n", needDiscon);
        }

        if ( needDiscon )
        {
            debug_printf("Auto Discon : global_cnt=%u , ", global_cnt );
            debug_printf("needDiscon=%d\r\n", needDiscon);
            if ( pollingRequest("AT", debugD, 2) == true )
            {
                global_cnt = 0;
                needDiscon = 0;
            }
        }
    }
}

char *Cble::Debug2BLE( int BLEwrite )
{
    char *result = NULL;

    if ( Serial.available() )
    {
        char c = (char)Serial.read();

        switch (c)
        {
            case '\0':
                return result;
            case '\r':
            case '\n':
                debug_printf("\r\n");
                if (BLEwrite && SerialCnt)
                {
                    BLE.write(SerialData);
                    BLE.flush();
                }

                result = (char *)malloc(sizeof(char) * (SerialCnt + 1));
                if (NULL == result)
                {
                    debug_printf("Debug2BLE malloc failed .\r\n");
                    break;
                }
                memcpy(result, SerialData, SerialCnt );
                result[SerialCnt] = NULL;

                memset(SerialData, 0, SerialCnt + 1);
                SerialCnt = 0;
                break;
            default :
                SerialData[SerialCnt] = c;
                SerialCnt++;
                Serial.print(c);
                break;
        }
    }

    return result;
}


void Cble::FactoryReset( void )
{
    debug_printf("FactoryReset is called.\r\n");

    pollingRequest("AT+RENEW", debugD, 8);
    memset(debugD, 0, DEBUG_SIZE);
}


void Cble::firstUartSetting( void )
{
    debug_printf("firstUartSetting is called.\r\n");

    Serial.begin(9600);
    BLE.begin(9600);
    BLE.setTimeout(1);
    delay(1000);

    pollingRequest("AT+BAUD4", debugD, 8);
    memset(debugD, 0, DEBUG_SIZE);

    debug_printf("Please Power off and on.\r\n");
    BLE.end();
//  delay(1000);
//
//  BLE.begin(115200);
//  delay(1000);
//
//  pollingRequest("AT", debugD, DEBUG_SIZE);
//  memset(debugD, 0, DEBUG_SIZE);


    exit(0);
}








