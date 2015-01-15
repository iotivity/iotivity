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

#include "oic_malloc.h"

#define MAX_BUF_LEN 100 //1024
#define MAX_OPT_LEN 16

#define printf Serial.println
//#define printf

bool isLeSelected = false;

void print_menu();
void process();

void initialize();
void start_listening_server();
void start_discovery_server();
void find_resource();
void send_request();
void send_request_all();
void send_response(CARemoteEndpoint_t *endpoint, const CAInfo_t* info);
void advertise_resource();
void send_notification();
void select_network();
void unselect_network();
void handle_request_response();

void request_handler(const CARemoteEndpoint_t *object, const CARequestInfo_t *requestInfo);
void response_handler(const CARemoteEndpoint_t *object, const CAResponseInfo_t *responseInfo);
void send_request_tmp(CARemoteEndpoint_t *endpoint, CAToken_t token);
void terminate();

void getData(char *readInput, int16_t bufferLength, int16_t *dataLength)
{
    while (!Serial.available());
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

CAConnectivityType_t getConnectivityType()
{
    char type[2];
    memset(type, 0, sizeof(char));
    printf("Select network");
    printf("ETHERNET: 0");
    printf("WIFI: 1");
    printf("EDR: 2");
    printf("LE: 3");


    int16_t typeLen = 0;
    getData(type, sizeof(type), &typeLen);
    int num = type[0] - '0';
    switch (num)
    {
        case 0:
            return CA_ETHERNET;
        case 1:
            return CA_WIFI;
        case 2:
            return CA_EDR;
        case 3:
            return CA_LE;
    }
    return CA_ETHERNET;
}
void setup()
{
    Serial.begin (115200);

    printf("============");
    printf("CA SAMPLE");
    printf("============");
    print_menu();
}

void loop()
{
    char buffer[5];
    memset(buffer, 0, sizeof(buffer));
    int16_t len;
    if (Serial.available() > 0)
    {
        getData(buffer, sizeof(buffer), &len);
        switch (toupper(buffer[0]))
        {
            case 'M': // menu
                print_menu();
                break;

            case 'Q': // quit
                printf("quit");
                return;

            case 'I': // Initialize interface
                initialize();
                break;

            case 'S': // start listening server
                start_listening_server();
                break;

            case 'D': // start discovery server
                start_discovery_server();
                break;

            case 'F': // find resource
                find_resource();
                break;

            case 'R': // send request
                send_request();
                break;
            case 'E': //send request to all
                send_request_all();
                break;
            case 'A': // advertise resource
                advertise_resource();
                break;

            case 'B': // send notification
                send_notification();
                break;

            case 'N': // select network
                select_network();
                break;

            case 'X': // unselect network
                unselect_network();
                break;

            case 'H': // handle request response
                handle_request_response();
                break;

            case 'T': // handle request response
                terminate();
                break;

            default:
                printf("wrong menu");
                break;
        }
    }
    //1:Add check for startserver before calling below api
    if (true == isLeSelected)
    {
        handle_request_response();
    }
    delay(1000);
}

void initialize()
{
    CAInitialize();
    select_network();
    // set handler.
    CARegisterHandler(request_handler, response_handler);
}

void start_listening_server()
{
    printf("start server");
    CAStartListeningServer();
}

void start_discovery_server()
{
    printf("start server");
    CAStartDiscoveryServer();
}

void find_resource()
{
    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("============");
    printf("ex) /a/light");
    printf("uri: ");
    int len = 0;
    getData(buf, sizeof(buf), &len);

    // create token
    CAToken_t token = NULL;
    CAResult_t res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!");
        token = NULL;
    }

    printf("token");
    printf((token != NULL) ? token : "");

    res = CAFindResource(buf, token);
    if (res != CA_STATUS_OK)
    {
        printf("find error");
    }
    else
    {
        printf("success: ");
        printf(buf);
    }
    CADestroyToken(token);
}

void send_request()
{
    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    CAConnectivityType_t selectedNetwork;
    selectedNetwork = getConnectivityType();

    printf("============");
    printf("10.11.12.13:4545/res_uri (for IP)");
    printf("10:11:12:13:45:45/res_uri (for BT)");
    printf("uri: ");

    int16_t len = 0;
    getData(buf, (int16_t)sizeof(buf), &len);

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    CAResult_t res = CACreateRemoteEndpoint(buf,selectedNetwork,&endpoint);
    if (res != CA_STATUS_OK)
    {
        printf("Out of memory");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("\n=============================================\n");
    printf("0:CON, 1:NON\n");
    printf("select message type : ");

    getData(buf, (int16_t)sizeof(buf), &len);

    int32_t msg_type = (CAMessageType_t) (buf[0] == '0' || buf[0] == '1') ? buf[0] - '0' : 0;
    CAMessageType_t msgType; // = (CAMessageType_t) (buf[0] == '0' || buf[0] == '1') ? buf[0] - '0' : 0;
    if(msg_type == 0)
	msgType = CA_MSG_CONFIRM;
    else if(msg_type == 1)
	msgType = CA_MSG_NONCONFIRM;
    else if(msg_type == 2)
	msgType = CA_MSG_ACKNOWLEDGE;
    else if(msg_type == 3)
	msgType = CA_MSG_RESET;

    // create token
    CAToken_t token = NULL;
    res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token error");
        token = NULL;
    }

    printf((token != NULL) ? token : "");

    CAInfo_t requestData;
    memset(&requestData, 0, sizeof(CAInfo_t));
    requestData.token = token;
    requestData.payload = (CAPayload_t)"Json Payload";

    requestData.type = msgType;

    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    CASendRequest(endpoint, &requestInfo);
    if (token != NULL)
    {
        CADestroyToken(token);
    }

    // destroy remote endpoint
    if (endpoint != NULL)
    {
        CADestroyRemoteEndpoint(endpoint);
    }

    printf("============");
}

void send_request_all()
{
    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    CAConnectivityType_t selectedNetwork;
    selectedNetwork = getConnectivityType();

    printf("=========");
    printf("10.11.12.13:4545/resource_uri ( for IP )");
    printf("10:11:12:13:45:45/resource_uri ( for BT )");
    printf("uri : ");

    int16_t len = 0;
    getData(buf, (int16_t)sizeof(buf), &len);

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    CAResult_t res = CACreateRemoteEndpoint(buf, selectedNetwork, &endpoint);

    if (res != CA_STATUS_OK)
    {
        printf("create remote endpoint error");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }


    CAGroupEndpoint_t *group = NULL;
    group = (CAGroupEndpoint_t *)OICMalloc(sizeof(CAGroupEndpoint_t));
    group->connectivityType = endpoint->connectivityType;
    group->resourceUri = endpoint->resourceUri;


    // create token
    CAToken_t token = NULL;
    res = CAGenerateToken(&token);

    if (res != CA_STATUS_OK)
    {
        printf("token error");
        token = NULL;
    }

    printf((token != NULL) ? token : "");

    CAInfo_t requestData;
    memset(&requestData, 0, sizeof(CAInfo_t));
    requestData.token = token;
    requestData.payload = "Temp Json Payload";
    requestData.type = CA_MSG_NONCONFIRM;
    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    // CASendRequest(endpoint, &requestInfo);
    CASendRequestToAll(group, &requestInfo);

    if (token != NULL)
    {
        CADestroyToken(token);
    }

    // destroy remote endpoint
    if (endpoint != NULL)
    {
        CADestroyRemoteEndpoint(endpoint);
    }

    OICFree(group);

    printf("==========");
}

void advertise_resource()
{
    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("============");
    printf("uri: ");

    int16_t len = 0;
    getData(buf, (int16_t)sizeof(buf), &len);

    int16_t optionNum = 0;
    char optionData[MAX_OPT_LEN];
    char optionNumBuf[2];

    printf("Option Num: ");
    getData(optionNumBuf, (int16_t)sizeof(optionNumBuf), &len);
    optionNum = optionNumBuf[0]  - '0';
    printf(optionNum);


    CAHeaderOption_t *headerOpt = NULL;
    if (optionNum > 0)
    {
        headerOpt = (CAHeaderOption_t *) OICMalloc(sizeof(CAHeaderOption_t) * optionNum);
        if (NULL == headerOpt)
        {
            printf("Out of memory");
            return;
        }
        memset(headerOpt, 0, sizeof(CAHeaderOption_t) * optionNum);
    }

    int i;
    for (i = 0 ; i < optionNum ; i++)
    {
        int optionID = 0;
        char getOptionID[2];
        getData(getOptionID, (int16_t)sizeof(getOptionID), &len);
        printf("Option Num: ");
        printf(i + 1);
        optionID = getOptionID[0];
        printf(optionID);

        headerOpt[i].optionID = optionID;
        memset(optionData, 0, sizeof(char) * MAX_OPT_LEN);
        printf("Option Data itr");
        printf(i + 1);

        int len = 0;
        getData(optionData, (int16_t)sizeof(optionData), &len);
        memcpy(headerOpt[i].optionData, optionData, strlen(optionData));
        printf(i + 1);
        printf("data");
        printf(optionData);

        headerOpt[i].optionLength = (uint16_t)strlen(optionData);
    }

    printf("============");
    // create token
    CAToken_t token = NULL;
    CAResult_t res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!");
        token = NULL;
    }

    printf("token");
    printf((token != NULL) ? token : "");

    CAAdvertiseResource(buf, token, headerOpt, (uint8_t)optionNum);
    if (NULL != headerOpt)
    {
        OICFree(headerOpt);
    }
    CADestroyToken(token);
}

void send_notification()
{
    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);
    CAConnectivityType_t selectedNetwork;
    selectedNetwork = getConnectivityType();

    printf("============");
    printf("10.11.12.13:4545/res_uri (for IP)");
    printf("10:11:12:13:45:45/res_uri (for BT)");
    printf("uri: ");

    int16_t len = 0;
    getData(buf, (int16_t)sizeof(buf), &len);

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    CAResult_t res = CACreateRemoteEndpoint(buf,selectedNetwork,&endpoint);
    if (CA_STATUS_OK != res)
    {
        printf("Out of memory");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    CAInfo_t respondeData;
    memset(&respondeData, 0, sizeof(CAInfo_t));
    respondeData.token = (CAToken_t)"token";
    respondeData.payload = (CAPayload_t)"Notification Data";

    CAResponseInfo_t responseInfo;
    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
    responseInfo.result = CA_SUCCESS;
    responseInfo.info = respondeData;

    // send request
    CASendNotification(endpoint, &responseInfo);
    // destroy remote endpoint
    if (NULL != endpoint)
    {
        CADestroyRemoteEndpoint(endpoint);
    }
    printf("============");
}

void select_network()
{
    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("============");
    printf("Select network");
    printf("ETHERNET: 0");
    printf("WIFI: 1");
    printf("EDR: 2");
    printf("LE: 3\n");

    int16_t len = 0;
    getData(buf, (int16_t)sizeof(buf), &len);
    int number = buf[0] - '0';
    number = (number < 0 || number > 3) ? 1 : number;
    if (number == 3)
    {
        isLeSelected = true;
    }
    CASelectNetwork(1 << number);
    printf("============");
}

void unselect_network()
{
    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("============");
    printf("Unselect network");
    printf("ETHERNET: 0");
    printf("WIFI: 1");
    printf("EDR: 2");
    printf("LE: 3\n");

    int16_t len = 0;
    getData(buf, (int16_t)sizeof(buf), &len);
    int number = buf[0] - '0';
    printf(number);
    number = (number < 0 || number > 3) ? 1 : number;
    if (number == 3)
    {
        isLeSelected = false;
    }
    CAUnSelectNetwork(1 << number);
    printf("Terminate");
    CATerminate();
    printf("============");
}

void print_menu()
{

    printf("============");
    printf("i: Initialize");
    printf("s: start listening server");
    printf("d: start discovery server");
    printf("f: find resource");
    printf("r: send request");
    printf("e: send request to all");
    printf("a: advertise resource");
    printf("b: send notification");
    printf("n: select network");
    printf("x: unselect network");
    printf("h: handle request response");
    printf("t: terminate");
    printf("q: quit");
    printf("============");
}

void handle_request_response()
{
    CAHandleRequestResponse();
}

void request_handler(const CARemoteEndpoint_t *object, const CARequestInfo_t *requestInfo)
{
    if (!object)
    {
        printf("Remote endpoint is NULL!");
        return;
    }

    if (!requestInfo)
    {
        printf("Request info is NULL!");
        return;
    }

    printf("uri: ");
    printf(object->resourceUri);
    printf("RAddr: ");
    printf(object->addressInfo.IP.ipAddress);
    printf("Port: ");
    printf(object->addressInfo.IP.port);
    printf("data: ");
    printf(requestInfo->info.payload);

    if (requestInfo->info.options)
    {
        uint32_t len = requestInfo->info.numOptions;
        uint32_t i;
        for (i = 0; i < len; i++)
        {
            printf("Option:");
            printf(i+1);
            printf("ID:");
            printf(requestInfo->info.options[i].optionID);
            printf("Data:");
            printf((char*)requestInfo->info.options[i].optionData);
        }
    }
    printf("send response");
    send_response((CARemoteEndpoint_t *)object, (requestInfo != NULL) ? &requestInfo->info : NULL);
}

void response_handler(const CARemoteEndpoint_t *object, const CAResponseInfo_t *responseInfo)
{
    printf("uri: ");
    printf((object != NULL) ? object->resourceUri : "");
    printf("data: ");
    printf((responseInfo != NULL) ? responseInfo->info.payload : "");
    printf("res result=");
    printf(responseInfo->result);
}

void send_response(CARemoteEndpoint_t *endpoint, const CAInfo_t* info)
{

    printf("============");

    CAInfo_t responseData;
    memset(&responseData, 0, sizeof(CAInfo_t));
    responseData.type =
            (info != NULL) ?
                    ((info->type == CA_MSG_CONFIRM) ? CA_MSG_ACKNOWLEDGE : CA_MSG_NONCONFIRM) :
                    CA_MSG_NONCONFIRM;
    responseData.messageId = (info != NULL) ? info->messageId : 0;
    responseData.token = (info != NULL) ? (CAToken_t)info->token : (CAToken_t)"";
    responseData.payload = (CAPayload_t)"response payload";

    CAResponseInfo_t responseInfo;
    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
    responseInfo.result = (CAResponseResult_t)203;
    responseInfo.info = responseData;

    // send request (connectivityType from remoteEndpoint of request Info)
    CAResult_t res = CASendResponse(endpoint, &responseInfo);
    if(res != CA_STATUS_OK)
    {
        printf("Snd Resp error");
    }
    else
    {
        printf("Snd Resp success");
    }

    printf("============");

}

void send_request_tmp(CARemoteEndpoint_t *endpoint, CAToken_t token)
{

    printf("============");

    CAInfo_t requestData;
    memset(&requestData, 0, sizeof(CAInfo_t));
    requestData.token = token;
    requestData.payload = (CAPayload_t)"Json Payload";

    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    endpoint->connectivityType = CA_WIFI;
    CASendRequest(endpoint, &requestInfo);

    printf("============");

}

void terminate()
{
    unselect_network();
}
