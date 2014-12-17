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
#include <fcntl.h>
#include <errno.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "cacommon.h"
#include "cainterface.h"

static GMainLoop *mainloop;
static GIOChannel *channel;
static guint g_test_io_watch_id;
static GError *g_err_Sample;

pthread_t thread;

#define MAX_BUF_LEN 1024
#define MAX_OPT_LEN 16

char get_menu();
void process();

void initialize();
void start_listening_server();
void start_discovery_server();
void find_resource();
void send_request();
void send_response();
void advertise_resource();
void send_notification();
void select_network();
void unselect_network();
void handle_request_response();

void request_handler(const CARemoteEndpoint_t *object, const CARequestInfo_t *requestInfo);
void response_handler(const CARemoteEndpoint_t *object, const CAResponseInfo_t *responseInfo);
void send_request_tmp(CARemoteEndpoint_t *endpoint, CAToken_t token);
void terminate();
CAConnectivityType_t get_network_type();


void pthread_func()
{
    g_main_loop_run(mainloop);
}

int main()
{

    printf("=============================================\n");
    printf("\t\tsample main\n");
    printf("=============================================\n");

    process();

    return 0;
}

void process()
{
    while (1)
    {
        char menu = toupper(get_menu());

        switch (menu)
        {
            case 'Q': // quits the sample program
                printf("quit..!!\n");
                g_main_loop_quit(mainloop);
                return;

            case 'I': // Initialize interface
                initialize();
                break;

            case 'S': // start server
                start_listening_server();
                break;

            case 'C': // start client
                start_discovery_server();
                break;

            case 'F': // find resource
                find_resource();
                break;

            case 'R': // send request
                send_request();
                break;

            case 'A': // advertise resource
                advertise_resource();
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

            case 'T': // Terminate interface
                terminate();
                break;

            default:
                printf("not supported menu!!\n");
                break;
        }
    }

}

void initialize()
{
    mainloop = g_main_loop_new(NULL, FALSE);
    pthread_create (&thread, NULL, (void *) &pthread_func, NULL);

    CAInitialize();

    // network enable
    // default
    printf("select default network(WIFI)\n");
    CASelectNetwork(CA_WIFI);

    // set handler.
    CARegisterHandler(request_handler, response_handler);
}

void start_listening_server()
{
    printf("start listening server!!\n");

    CAStartListeningServer();
}

void start_discovery_server()
{
    printf("start discovery server at client!!\n");

    CAStartDiscoveryServer();
}

void find_resource()
{
    char buf[MAX_BUF_LEN];

    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("\n=============================================\n");
    printf("ex) a/light\n");
    printf("reference uri : ");

    gets(buf);

    // create token
    CAToken_t token = NULL;
    CAResult_t res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!\n");
        token = NULL;
    }

    printf("generated token %s\n", (token != NULL) ? token : "");

    res = CAFindResource(buf, token);

    if (res != CA_STATUS_OK)
    {
        printf("find resource error:%d !!\n", res);
    }
    else
    {
        printf("find resource for %s URI\n", buf);
    }

    printf("=============================================\n");
}

void send_request()
{
    char buf[MAX_BUF_LEN];

    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);
    CAConnectivityType_t selectedNetwork;

    selectedNetwork = get_network_type();

    printf("\n=============================================\n");
    printf("10.11.12.13:4545/resource_uri ( for IP )\n");
    printf("10:11:12:13:45:45/resource_uri ( for BT )\n");
    printf("uri : ");

    gets(buf);

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    CAResult_t res = CACreateRemoteEndpoint(buf,selectedNetwork, &endpoint);

    if (res != CA_STATUS_OK)
    {
        printf("create remote endpoint error!!");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    // create token
    CAToken_t token = NULL;
    res = CAGenerateToken(&token);

    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!");
        token = NULL;
    }

    printf("generated token %s\n", (token != NULL) ? token : "");

    CAInfo_t requestData;
    memset(&requestData, 0, sizeof(CAInfo_t));
    requestData.token = token;
    requestData.payload = "Temp Json Payload";

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

    printf("=============================================\n");
}

void advertise_resource()
{
    char buf[MAX_BUF_LEN];

    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("\n=============================================\n");
    printf("uri : ");

    scanf("%s", buf);

    int optionNum = 0;
    char optionData[MAX_OPT_LEN];

    printf("Option Num : ");
    scanf("%d", &optionNum);
    CAHeaderOption_t *headerOpt;

    if (optionNum > 0)
    {
        headerOpt = (CAHeaderOption_t *) malloc(sizeof(CAHeaderOption_t) * optionNum);
        if (NULL == headerOpt)
        {
            printf("memory allocation failed!\n");
            return;
        }
        memset(headerOpt, 0, sizeof(CAHeaderOption_t) * optionNum);
    }

    int i;
    for (i = 0 ; i < optionNum ; i++)
    {
        int optionID = 0;
        printf("[%d] Option ID : ", i + 1);
        scanf("%d", &optionID);
        headerOpt[i].optionID = optionID;

        memset(optionData, 0, sizeof(char) * MAX_OPT_LEN);
        printf("[%d] Option Data : ", i + 1);
        scanf("%s", optionData);
        memcpy(headerOpt[i].optionData, optionData, MAX_OPT_LEN);
        printf("[%d] inputed option : ID : %d, data : %s\n", i + 1, optionID, optionData );

        headerOpt[i].optionLength = (uint16_t)strlen(optionData);
    }
    printf("\n=============================================\n");

    // create token
    CAToken_t token = NULL;
    CAResult_t res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!\n");
        token = NULL;
    }

    printf("generated token %s\n", (token != NULL) ? token : "");

    CAAdvertiseResource(buf, token, headerOpt, (uint8_t)optionNum);

    free(headerOpt);

}

void select_network()
{
    char buf[MAX_BUF_LEN];

    printf("\n=============================================\n");
    printf("\tselect network\n");
    printf("ETHERNET : 0\n");
    printf("WIFI : 1\n");
    printf("EDR : 2\n");
    printf("LE : 3\n");
    printf("select : ");

    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);
    gets(buf);

    int number = buf[0] - '0';

    number = (number < 0 || number > 3) ? 1 : number;

    CASelectNetwork(1 << number);

    printf("=============================================\n");
}

void unselect_network()
{
    char buf[MAX_BUF_LEN];

    printf("\n=============================================\n");
    printf("\tunselect enabled network\n");
    printf("ETHERNET : 0\n");
    printf("WIFI : 1\n");
    printf("EDR : 2\n");
    printf("LE : 3\n");
    printf("select : ");

    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);
    gets(buf);

    int number = buf[0] - '0';

    number = (number < 0 || number > 3) ? 1 : number;

    CAUnSelectNetwork(1 << number);
    printf("Terminating...\n");
    CATerminate();
    //pthread_join(thread, NULL);
    printf("=============================================\n");
}

char get_menu()
{
    char buf[MAX_BUF_LEN];

    printf("\n=============================================\n");
    printf("\t\tMenu\n");
    printf("\ti : Initialize\n");
    printf("\ts : start server\n");
    printf("\tc : start client\n");
    printf("\tf : find resource\n");
    printf("\tr : send request\n");
    printf("\ta : advertise resource\n");
    printf("\tn : select network\n");
    printf("\tx : unselect network\n");
    printf("\th : handle request response\n");
    printf("\tt : terminate\n");
    printf("\tq : quit\n");
    printf("=============================================\n");
    printf("select : ");

    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    gets(buf);

    return buf[0];
}

void handle_request_response()
{
    printf("handle_request_response\n");
    CAHandleRequestResponse();
}

void request_handler(const CARemoteEndpoint_t *object, const CARequestInfo_t *requestInfo)
{

    printf("[CALLBACK] request_handler, uri : %s, data : %s\n",
           (object != NULL) ? object->resourceUri : "",
           (requestInfo != NULL) ? requestInfo->info.payload : "");

    printf("[CALLBACK] request_handler, address : %s\n",
           (object != NULL) ? object->addressInfo.IP.ipAddress : "");

    if (requestInfo->info.options)
    {
        uint32_t len =  requestInfo->info.numOptions;
        uint32_t i;
        for (i = 0 ; i < len ; i++)
        {
            printf("[CALLBACK] request_handler, option ID : %d\n", requestInfo->info.options[i].optionID);
            printf("[CALLBACK] request_handler, options data length : %d\n",
                   requestInfo->info.options[i].optionLength);
            printf("[CALLBACK] request_handler, options data : %s\n", requestInfo->info.options[i].optionData );
        }
    }

    printf("send response with URI\n");
    send_response(object, (requestInfo != NULL) ? requestInfo->info.token : "");

}

void response_handler(const CARemoteEndpoint_t *object, const CAResponseInfo_t *responseInfo)
{

    printf("[CALLBACK] response_handler, uri : %s, data : %s\n",
           (object != NULL) ? object->resourceUri : "",
           (responseInfo != NULL) ? responseInfo->info.payload : "");

    printf("[CALLBACK] response_handler, address : %s\n",
           (object != NULL) ? object->addressInfo.IP.ipAddress : "");

    if (responseInfo->info.options)
    {
        uint32_t len =  responseInfo->info.numOptions;
        uint32_t i;
        for (i = 0 ; i < len ; i++)
        {
            printf("[CALLBACK] response_handler, option ID : %d\n", responseInfo->info.options[i].optionID);
            printf("[CALLBACK] response_handler, options data length : %d\n",
                   responseInfo->info.options[i].optionLength);
            printf("[CALLBACK] response_handler, options data : %s\n",
                   responseInfo->info.options[i].optionData );
        }
    }

    //printf("send request with URI\n");
    //send_request_tmp(object, (responseInfo != NULL) ? responseInfo->info.token : "");
}

void send_response(CARemoteEndpoint_t *endpoint, CAToken_t request_token)
{

    printf("\n=============================================\n");

    CAInfo_t responseData;
    //responseData = (CAInfo*) malloc(sizeof(CAInfo));
    memset(&responseData, 0, sizeof(CAInfo_t));
    responseData.token = request_token;
    responseData.payload = "response payload";

    CAResponseInfo_t responseInfo;
    //responseInfo = (CAResponseInfo*) malloc(sizeof(CAResponseInfo));
    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
    responseInfo.result = 203;
    responseInfo.info = responseData;

    // send request (connectivityType from remoteEndpoint of request Info)
    CASendResponse(endpoint, &responseInfo);

    printf("=============================================\n");

}

void send_request_tmp(CARemoteEndpoint_t *endpoint, CAToken_t token)
{

    printf("\n=============================================\n");

    CAInfo_t requestData;
    memset(&requestData, 0, sizeof(CAInfo_t));
    requestData.token = token;
    requestData.payload = "Temp Json Payload";

    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    endpoint->connectivityType = CA_WIFI;
    CASendRequest(endpoint, &requestInfo);

    printf("=============================================\n");

}

void terminate()
{
    unselect_network();
}

CAConnectivityType_t get_network_type()
{
    char buf[MAX_BUF_LEN];

    printf("\n=============================================\n");
    printf("\tselect network type\n");
    printf("ETHERNET : 0\n");
    printf("WIFI : 1\n");
    printf("EDR : 2\n");
    printf("LE : 3\n");
    printf("select : ");

    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);
    gets(buf);

    int number = buf[0] - '0';

    number = (number < 0 || number > 3) ? 0 : 1 << number;

    if (number & CA_ETHERNET)
    {
        return CA_ETHERNET;
    }
    if (number & CA_WIFI)
    {
        return CA_WIFI;
    }
    if (number & CA_EDR)
    {
        return CA_EDR;
    }
    if (number & CA_LE)
    {
        return CA_LE;
    }

    printf("\n=============================================\n");
}

