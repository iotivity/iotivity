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

#include <ctype.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cacommon.h"
#include "cainterface.h"
#include "Arduino.h"

#ifdef ARDUINOWIFI
#include "WiFi.h"
#elif defined ARDUINOETH
#include "Ethernet.h"
#endif

#include "oic_malloc.h"

#define MAX_BUF_LEN 100 //1024
#define MAX_OPT_LEN 16

static bool g_isLeSelected = false;

static void PrintMenu();
static void Process();
static void Initialize();
static void StartListeningServer();
static void StartDiscoveryServer();
static void FindResource();
static void SendRequest();
static void SendRequestAll();
static void SendResponse(CARemoteEndpoint_t *endpoint, const CAInfo_t* info);
static void AdvertiseResource();
static void SendNotification();
static void SelectNetwork();
static void UnselectNetwork();
static void HandleRequestResponse();

static void RequestHandler(const CARemoteEndpoint_t *object, const CARequestInfo_t *requestInfo);
static void ResponseHandler(const CARemoteEndpoint_t *object, const CAResponseInfo_t *responseInfo);
static void Terminate();

void GetData(char *readInput, size_t bufferLength, size_t *dataLength)
{
    if (!readInput || bufferLength == 0 || !dataLength)
    {
        Serial.println("Invalid buffer");
        return;
    }

    while (!Serial.available())
    {
        delay(500);
    }
    int len = 0;
    while (Serial.available())
    {
        delay(100);
        char c = Serial.read();
        if ('\n' != c && '\r' != c && len < bufferLength - 1)
        {
            readInput[len++] = c;
        }
        else
        {
            break;
        }
    }

    readInput[len] = '\0';
    Serial.flush();
    Serial.print("PD:");
    Serial.println(readInput);
    (*dataLength) = len;
}

CATransportType_t GetConnectivityType()
{
    char type[2] = {0};
    Serial.println("Select network");
    Serial.println("IPv4: 0");
    Serial.println("EDR: 2");
    Serial.println("LE: 3");

    size_t typeLen = 0;
    GetData(type, sizeof(type), &typeLen);
    if (0 >= typeLen)
    {
        Serial.println("i/p err,default ethernet");
        return CA_IPV4;
    }
    switch (type[0])
    {
        case '0':
            return CA_IPV4;
        case '2':
            return CA_EDR;
        case '3':
            return CA_LE;
    }
    return CA_IPV4;
}

void setup()
{
    Serial.begin (115200);

    Serial.println("============");
    Serial.println("CA SAMPLE");
    Serial.println("============");
    PrintMenu();
}

void loop()
{
    char buffer[5] = {0};
    size_t len;
    if (Serial.available() > 0)
    {
        GetData(buffer, sizeof(buffer), &len);
        if (0 >= len)
        {
            Serial.println("i/p err");
            return;
        }
        switch (toupper(buffer[0]))
        {
            case 'M': // menu
                PrintMenu();
                break;

            case 'Q': // quit
                Serial.println("quit");
                return;

            case 'I': // Initialize interface
                Initialize();
                break;

            case 'S': // start listening server
                StartListeningServer();
                break;

            case 'D': // start discovery server
                StartDiscoveryServer();
                break;

            case 'F': // find resource
                FindResource();
                break;

            case 'R': // send request
                SendRequest();
                break;
            case 'E': //send request to all
                SendRequestAll();
                break;
            case 'A': // advertise resource
                AdvertiseResource();
                break;

            case 'B': // send notification
                SendNotification();
                break;

            case 'N': // select network
                SelectNetwork();
                break;

            case 'X': // unselect network
                UnselectNetwork();
                break;

            case 'H': // handle request response
                HandleRequestResponse();
                break;

            case 'T': // handle request response
                Terminate();
                break;

            default:
                Serial.println("wrong menu");
                break;
        }
    }
    //1:Add check for startserver before calling below api
    if (g_isLeSelected)
    {
        HandleRequestResponse();
    }
    delay(1000);
}

void Initialize()
{
    if(CAInitialize() != CA_STATUS_OK)
    {
        Serial.println("Initialize failed");
        return;
    }
    SelectNetwork();
    // set handler.
    CARegisterHandler(RequestHandler, ResponseHandler);
}

void StartListeningServer()
{
    Serial.println("listening server");
    CAResult_t ret = CAStartListeningServer();
    if(ret != CA_STATUS_OK)
    {
        Serial.print("listening failed: ");
        Serial.println(ret);
        return;
    }
}

void StartDiscoveryServer()
{
    Serial.println("discovery server");
    CAResult_t ret = CAStartDiscoveryServer();
    if(ret != CA_STATUS_OK)
    {
        Serial.print("discovery failed: ");
        Serial.println(ret);
        return;
    }
}

void FindResource()
{
    char buf[MAX_BUF_LEN] = {0};
    Serial.println("============");
    Serial.println("ex) /a/light");
    Serial.println("uri: ");
    size_t len = 0;
    GetData(buf, sizeof(buf), &len);
    if (0 >= len)
    {
        Serial.println("i/p err");
        return;
    }
    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    CAResult_t res = CAGenerateToken(&token, tokenLength);
    if (res != CA_STATUS_OK || (!token))
    {
        Serial.println("token error");
        return;
    }

    Serial.print("token:");
    Serial.println(token);

    res = CAFindResource(buf, token, tokenLength);
    if (res != CA_STATUS_OK)
    {
        Serial.print("find error: ");
        Serial.println(res);
    }
    else
    {
        Serial.println("success: ");
        Serial.println(buf);
    }
    CADestroyToken(token);
}

void SendRequest()
{
    char buf[MAX_BUF_LEN] = {0};
    CATransportType_t selectedNetwork;
    selectedNetwork = GetConnectivityType();

    Serial.println("============");
    Serial.println("10.11.12.13:4545/res_uri (for IP)");
    Serial.println("10:11:12:13:45:45/res_uri (for BT)");
    Serial.println("uri: ");

    size_t len = 0;
    GetData(buf, sizeof(buf), &len);
    if (0 >= len)
    {
        Serial.println("i/p err");
        return;
    }

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    CAResult_t res = CACreateRemoteEndpoint(buf,selectedNetwork,&endpoint);
    if (res != CA_STATUS_OK)
    {
        Serial.println("Out of memory");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    memset(buf, 0, sizeof(buf));

    Serial.println("\n=============================================\n");
    Serial.println("0:CON, 1:NON\n");
    Serial.println("select message type : ");
    GetData(buf, sizeof(buf), &len);
    CAMessageType_t msgType = CA_MSG_CONFIRM;

    if (0 >= len)
    {
        Serial.println("i/p err,default: 0");
    }
    else if(buf[0] == '1')
    {
        msgType = CA_MSG_NONCONFIRM;
    }

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if (res != CA_STATUS_OK || (!token))
    {
        Serial.println("token error");
        return;
    }

    Serial.println(token);
    CAInfo_t requestData = {CA_MSG_RESET};
    requestData.token = token;
    requestData.tokenLength = tokenLength;
    requestData.payload = (CAPayload_t)"Json Payload";

    requestData.type = msgType;

    CARequestInfo_t requestInfo = {CA_GET, {CA_MSG_RESET}};
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    CASendRequest(endpoint, &requestInfo);
    if (NULL != token)
    {
        CADestroyToken(token);
    }

    // destroy remote endpoint
    if (endpoint != NULL)
    {
        CADestroyRemoteEndpoint(endpoint);
    }

    CADestroyToken(token);
    Serial.println("============");
}

void SendRequestAll()
{
    char buf[MAX_BUF_LEN] = {0};

    CATransportType_t selectedNetwork;
    selectedNetwork = GetConnectivityType();

    Serial.println("=========");
    Serial.println("10.11.12.13:4545/resource_uri ( for IP )");
    Serial.println("10:11:12:13:45:45/resource_uri ( for BT )");
    Serial.println("uri : ");

    size_t len = 0;
    GetData(buf, sizeof(buf), &len);
    if (0 >= len)
    {
        Serial.println("i/p err");
        return;
    }

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    CAResult_t res = CACreateRemoteEndpoint(buf, selectedNetwork, &endpoint);

    if (res != CA_STATUS_OK)
    {
        Serial.println("create remote endpoint error");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    CAGroupEndpoint_t *group = NULL;
    group = (CAGroupEndpoint_t *)OICMalloc(sizeof(CAGroupEndpoint_t));
    group->transportType = endpoint->transportType;
    group->resourceUri = endpoint->resourceUri;

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if (res != CA_STATUS_OK || (!token))
    {
        Serial.println("token error");
        return;
    }

    Serial.println(token);

    CAInfo_t requestData = {CA_MSG_RESET};
    requestData.token = token;
    requestData.tokenLength = tokenLength;
    requestData.payload = "Temp Json Payload";
    requestData.type = CA_MSG_NONCONFIRM;

    CARequestInfo_t requestInfo = {CA_GET, {CA_MSG_RESET}};
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    // CASendRequest(endpoint, &requestInfo);
    CASendRequestToAll(group, &requestInfo);

    if (NULL != token)
    {
        CADestroyToken(token);
    }

    // destroy remote endpoint
    if (endpoint != NULL)
    {
        CADestroyRemoteEndpoint(endpoint);
    }

    OICFree(group);
    Serial.println("==========");
}

void AdvertiseResource()
{
    char buf[MAX_BUF_LEN] = {0};

    Serial.println("============");
    Serial.println("uri: ");

    size_t len = 0;
    GetData(buf, sizeof(buf), &len);
    if (0 >= len)
    {
        Serial.println("no i/p");
        return;
    }

    int16_t optionNum = 0;
    char optionData[MAX_OPT_LEN] = {0};
    char optionNumBuf[2] = {0};

    Serial.println("Option Num: ");
    GetData(optionNumBuf, sizeof(optionNumBuf), &len);
    if (0 >= len)
    {
        Serial.println("no i/p,0 option");
    }
    else
    {
        optionNum = atoi(optionNumBuf);
        Serial.println(optionNum);
    }

    CAHeaderOption_t *headerOpt = NULL;
    if (optionNum > 0)
    {
        headerOpt = (CAHeaderOption_t *) OICCalloc(optionNum, sizeof(CAHeaderOption_t));
        if (NULL == headerOpt)
        {
            Serial.println("Out of memory");
            return;
        }
    }

    int i;
    for (i = 0 ; i < optionNum ; i++)
    {
        int optionID = 0;
        char getOptionID[4];
        Serial.println("Opt ID:");
        GetData(getOptionID, sizeof(getOptionID), &len);
        if (0 >= len)
        {
            Serial.println("no i/p");
            continue;
        }
        else
        {
            optionID = atoi(getOptionID);
        }

        memset(optionData, 0, sizeof(optionData));
        Serial.println("Opt Data:");
        GetData(optionData, sizeof(optionData), &len);
        if (0 >= len)
        {
            Serial.println("no i/p");
            continue;
        }

        headerOpt[i].optionID = optionID;
        memcpy(headerOpt[i].optionData, optionData, strlen(optionData));
        Serial.println("ID:");
        Serial.println(optionID);
        Serial.println("Data:");
        Serial.println(optionData);

        headerOpt[i].optionLength = (uint16_t)strlen(optionData);
    }

    Serial.println("============");
    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    CAResult_t res = CAGenerateToken(&token, tokenLength);
    if (res != CA_STATUS_OK || (!token))
    {
        Serial.println("token error");
        return;
    }

    Serial.println("token");
    Serial.println(token);

    CAAdvertiseResource(buf, token, tokenLength, headerOpt, (uint8_t)optionNum);
    OICFree(headerOpt);
    CADestroyToken(token);
}

void SendNotification()
{
    char buf[MAX_BUF_LEN] = {0};
    CATransportType_t selectedNetwork;
    selectedNetwork = GetConnectivityType();

    Serial.println("============");
    Serial.println("10.11.12.13:4545/res_uri (for IP)");
    Serial.println("10:11:12:13:45:45/res_uri (for BT)");
    Serial.println("uri: ");

    size_t len = 0;
    GetData(buf, sizeof(buf), &len);
    if (0 >= len)
    {
        Serial.println("i/p err");
        return;
    }

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    CAResult_t res = CACreateRemoteEndpoint(buf,selectedNetwork,&endpoint);
    if (CA_STATUS_OK != res)
    {
        Serial.println("Out of memory");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if (res != CA_STATUS_OK || (!token))
    {
        Serial.println("token error");
        return;
    }

    CAInfo_t respondData = {CA_MSG_NONCONFIRM};
    respondData.token = token;
    respondData.tokenLength = tokenLength;
    respondData.payload = (CAPayload_t)"Notification Data";

    CAResponseInfo_t responseInfo = {CA_BAD_REQ, {CA_MSG_RESET}};
    responseInfo.result = CA_SUCCESS;
    responseInfo.info = respondData;

    // send request
    CASendNotification(endpoint, &responseInfo);
    // destroy remote endpoint
    if (NULL != endpoint)
    {
        CADestroyRemoteEndpoint(endpoint);
    }

    CADestroyToken(token);
    Serial.println("============");
}

void SelectNetwork()
{
    char buf[MAX_BUF_LEN] = {0};

    Serial.println("============");
    Serial.println("Select network");
    Serial.println("IPv4: 0");
    Serial.println("EDR: 2");
    Serial.println("LE: 3\n");

    size_t len = 0;
    GetData(buf, sizeof(buf), &len);
    int number = buf[0] - '0';
    if (0 >= len || number < 0 || number > 3)
    {
        Serial.println("Wrong i/p. WIFI selected");
        number = 1;
    }

    switch (number)
    {
        case 0:
            {
#ifdef ARDUINOWIFI
                const char ssid[] = "SSID";              // your network SSID (name)
                const char pass[] = "SSID_Password";     // your network password
                int16_t status = WL_IDLE_STATUS;         // the Wifi radio's status

                if (WiFi.status() == WL_NO_SHIELD)
                {
                    Serial.println("ERROR:No Shield");
                    return;
                }

                while (status != WL_CONNECTED)
                {
                    Serial.print("connecting: ");
                    Serial.println(ssid);
                    // WiFi.begin api is weird. ssid should also be taken as const char *
                    // Connect to WPA/WPA2 network:
                    status = WiFi.begin((char *)ssid, pass);
                }
#elif defined ARDUINOETH
                // Note: ****Update the MAC address here with your shield's MAC address****
                uint8_t ETHERNET_MAC[] = {0x90, 0xA2, 0xDA, 0x0E, 0xC4, 0x05};
                uint8_t error = Ethernet.begin(ETHERNET_MAC);
                if (error  == 0)
                {
                    Serial.print("Failed: ");
                    Serial.println(error);
                    return;
                }
#endif
            }
            break;
        case 2:
            // Nothing TBD here
            break;
        case 3:
            g_isLeSelected = true;
            break;
    }

    CASelectNetwork(CATransportType_t(1<<number));
    Serial.println("============");
}

void UnselectNetwork()
{
    char buf[MAX_BUF_LEN] = {0};

    Serial.println("============");
    Serial.println("Unselect network");
    Serial.println("IPv4: 0");
    Serial.println("EDR: 2");
    Serial.println("LE: 3\n");

    size_t len = 0;
    GetData(buf, sizeof(buf), &len);
    int number = buf[0] - '0';
    Serial.println(number);
    if (0 >= len || number < 0 || number > 3)
    {
        Serial.println("Wrong i/p. WIFI selected");
        number = 1;
    }
    if (number == 3)
    {
        g_isLeSelected = false;
    }
    CAUnSelectNetwork(1 << number);
    Serial.println("Terminate");
    CATerminate();
    Serial.println("============");
}

void PrintMenu()
{

    Serial.println("============");
    Serial.println("i: Initialize");
    Serial.println("s: start listening server");
    Serial.println("d: start discovery server");
    Serial.println("f: find resource");
    Serial.println("r: send request");
    Serial.println("e: send request to all");
    Serial.println("a: advertise resource");
    Serial.println("b: send notification");
    Serial.println("n: select network");
    Serial.println("x: unselect network");
    Serial.println("h: handle request response");
    Serial.println("t: terminate");
    Serial.println("q: quit");
    Serial.println("============");
}

void HandleRequestResponse()
{
    CAHandleRequestResponse();
}

void RequestHandler(const CARemoteEndpoint_t *object, const CARequestInfo_t *requestInfo)
{
    if (!object)
    {
        Serial.println("Remote endpoint is NULL!");
        return;
    }

    if (!requestInfo)
    {
        Serial.println("Request info is NULL!");
        return;
    }

    Serial.println("uri: ");
    Serial.println(object->resourceUri);
    Serial.println("RAddr: ");
    Serial.println(object->addressInfo.IP.ipAddress);
    Serial.println("Port: ");
    Serial.println(object->addressInfo.IP.port);
    Serial.println("data: ");
    Serial.println(requestInfo->info.payload);
    Serial.println("Type: ");
    Serial.println(requestInfo->info.type);

    if (requestInfo->info.options)
    {
        uint32_t len = requestInfo->info.numOptions;
        uint32_t i;
        for (i = 0; i < len; i++)
        {
            Serial.println("Option:");
            Serial.println(i+1);
            Serial.println("ID:");
            Serial.println(requestInfo->info.options[i].optionID);
            Serial.println("Data:");
            Serial.println((char*)requestInfo->info.options[i].optionData);
        }
    }
    Serial.println("send response");
    SendResponse((CARemoteEndpoint_t *)object, (requestInfo != NULL) ? &requestInfo->info : NULL);
}

void ResponseHandler(const CARemoteEndpoint_t *object, const CAResponseInfo_t *responseInfo)
{
    if (object && object->resourceUri)
    {
        Serial.print("uri: ");
        Serial.println(object->resourceUri);
    }

    if (responseInfo)
    {
        Serial.print("data: ");
        Serial.println(responseInfo->info.payload);
        Serial.print("Type: ");
        Serial.println(responseInfo->info.type);
        Serial.print("res result=");
        Serial.println(responseInfo->result);
    }
}

void SendResponse(CARemoteEndpoint_t *endpoint, const CAInfo_t* info)
{
    char buf[MAX_BUF_LEN] = {0};

    Serial.println("============");
    CAInfo_t responseData = {CA_MSG_RESET};
    if(info && info->type == CA_MSG_CONFIRM)
    {
        responseData.type = CA_MSG_ACKNOWLEDGE;
    }
    else
    {
        responseData.type = CA_MSG_NONCONFIRM;
    }

    responseData.messageId = (info != NULL) ? info->messageId : 0;
    responseData.token = (info != NULL) ? (CAToken_t)info->token : NULL;
    responseData.tokenLength = (info != NULL) ? info->tokenLength : 0;
    responseData.payload = (CAPayload_t)"response payload";

    CAResponseInfo_t responseInfo = {CA_BAD_REQ, {CA_MSG_RESET}};
    responseInfo.result = (CAResponseResult_t)203;
    responseInfo.info = responseData;

    // send request (connectivityType from remoteEndpoint of request Info)
    CAResult_t res = CASendResponse(endpoint, &responseInfo);
    if(res != CA_STATUS_OK)
    {
        Serial.println("Snd Resp error");
    }
    else
    {
        Serial.println("Snd Resp success");
    }

    Serial.println("============");
}

void Terminate()
{
    UnselectNetwork();
}

