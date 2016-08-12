/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
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

#define MAX_BUF_LEN 2048
#define IDENTITY     ("1111111111111111")
#define RS_CLIENT_PSK   ("AAAAAAAAAAAAAAAA")

#include "cacommon.h"
#include "cainterface.h"
#include "casecurityinterface.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#ifdef TIZEN
#include <glib.h>
#include <pthread.h>

static GMainLoop *g_mainloop = NULL;
pthread_t thread;
#endif

int32_t getDtlsPskCredentials( CADtlsPskCredType_t type, const unsigned char *desc,
      size_t desc_len, unsigned char *result, size_t result_length);

int32_t getDtlsPskCredentials( CADtlsPskCredType_t type, const unsigned char *desc,
      size_t desc_len, unsigned char *result, size_t result_length)
{
    printf("getDtlsPskCredentials in\n");

    int32_t ret = -1;

    if (NULL == result)
    {
        return ret;
    }

    switch (type)
    {
        case CA_DTLS_PSK_HINT:
        case CA_DTLS_PSK_IDENTITY:
          printf("CAGetDtlsPskCredentials CA_DTLS_PSK_IDENTITY\n");
            if (result_length < sizeof(IDENTITY))
            {
                printf("ERROR : Wrong value for result for storing IDENTITY\n");
                return ret;
            }

            memcpy(result, IDENTITY, sizeof(IDENTITY));
            ret = sizeof(IDENTITY);
            break;

        case CA_DTLS_PSK_KEY:
          printf("CAGetDtlsPskCredentials CA_DTLS_PSK_KEY\n");
            if ((desc_len == sizeof(IDENTITY)) &&
                memcmp(desc, IDENTITY, sizeof(IDENTITY)) == 0)
            {
                if (result_length < sizeof(RS_CLIENT_PSK))
                {
                    printf("ERROR : Wrong value for result for storing RS_CLIENT_PSK\n");
                    return ret;
                }

                memcpy(result, RS_CLIENT_PSK, sizeof(RS_CLIENT_PSK));
                ret = sizeof(RS_CLIENT_PSK);
            }
            break;

        default:

            printf("Wrong value passed for PSK_CRED_TYPE.\n");
            ret = -1;
    }

    printf("getDtlsPskCredentials out\n");

    return ret;
}

char* getString(char a[], int length)
{
    char str[MAX_BUF_LEN];

    int i;

    for(i = 0; i < length; i++)
    {
        str[i] = a[i];
    }

    str[length] = 0;

    return str;
}

void requestHandler(const CAEndpoint_t* endPoint, const CARequestInfo_t* requestInfo)
{

}

void responseHandler(const CAEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo)
{
    char *payload = NULL;
    CAMessageType_t messageType = CA_MSG_ACKNOWLEDGE;

    printf("requestHandler in\n");

    if(!endPoint)
    {
        printf("endPoint is NULL\n");
        return;
    }

    if(!responseInfo)
    {
        printf("requestInfo is NULL\n");
        return;
    }

    printf("IP %s, Port %d\n", endPoint->addr, endPoint->port);
    printf("Message Id: %d\n", responseInfo->info.messageId);

    if (responseInfo->info.options)
    {
        printf("Header Option Found\n");

        uint32_t len = responseInfo->info.numOptions;
        uint32_t i;
        for (i = 0; i < len; i++)
        {
            printf("Option ID : %d\n", responseInfo->info.options[i].optionID);
            printf("Option Data[%d]: %s\n", responseInfo->info.options[i].optionLength,
                    responseInfo->info.options[i].optionData);
        }
    }
    else
    {
        printf("No Header Option Found\n");
    }

    printf("PayloadSize: %d\n", responseInfo->info.payloadSize);

    if(responseInfo->info.payload)
    {
        payload = getString(responseInfo->info.payload, responseInfo->info.payloadSize);
        printf("Payload: %s\n", payload);
    }
    else
    {
        printf("Payload is NULL\n");
    }

    if(!responseInfo->info.resourceUri)
    {
        printf("ResourceUri is NULL\n");
        return;
    }

    printf("ResourceUri: %s\n", responseInfo->info.resourceUri);

    printf("requestHandler out\n");
}


void errorHandler(const CAEndpoint_t *endPoint, const CAErrorInfo_t* errorInfo)
{
    printf("errorHandler in\n");

    printf("errorHandler out\n");
}

int main()
{
    CAResult_t result;
    int port;
    char ip[1024];
    uint32_t tempSize = 0;
    CAToken_t token;
    CAEndpoint_t* endpoint;
    CAEndpoint_t* tempInfo = NULL;
    char resourceUri[1024] = "SendReqAck";
    char payload[1024] = "aaaaaaaa";
    char command[100];

#ifdef TIZEN
    g_mainloop = g_main_loop_new(NULL, FALSE);
    if(!g_mainloop)
    {
       printf("g_main_loop_new failed\n");
       return -1;
    }

    if (pthread_create(&thread, NULL, (void *) &GMainLoopThread, NULL) < 0)
    {
       printf("pthread_create failed in initialize\n");
       return -1;
    }
#endif

    result = CAInitialize();
    if (result != CA_STATUS_OK)
    {
        printf("CAInitialize failed. return value is %d", result);
        return -1;
    }

    CARegisterHandler(requestHandler, responseHandler, errorHandler);

    result = CARegisterDTLSCredentialsHandler(getDtlsPskCredentials);
    if (result != CA_STATUS_OK)
    {
        printf("CARegisterDTLSCredentialsHandler failed. return value is %d", result);
    }

    result = CASelectNetwork(CA_ADAPTER_IP);
    if (result != CA_STATUS_OK)
    {
        printf("CASelectNetwork failed. return value is %d", result);
        return -1;
    }

    result = CAStartDiscoveryServer();
    if (result != CA_STATUS_OK)
    {
        printf("CAStartDiscoveryServer failed. return value is %d", result);
        return -1;
    }

    result = CAGenerateToken(&token, CA_MAX_TOKEN_LEN);
    if (result != CA_STATUS_OK)
    {
        printf("CAGenerateToken failed. return value is %d", result);
        return -1;
    }

    printf("Enter Server IP: ");
    scanf("%s", ip);
    printf("Enter Server Secure Port: ");
    scanf("%d", &port);

    result = CACreateEndpoint(CA_SECURE, CA_ADAPTER_IP, (const char*)ip, port, &endpoint);
    if (result != CA_STATUS_OK)
    {
        printf("CACreateEndpoint failed. return value is %d", result);
        return -1;
    }

    CAInfo_t requestData = { 0 };
    requestData.token = token;
    requestData.tokenLength = CA_MAX_TOKEN_LEN;
    requestData.resourceUri = (CAURI_t) resourceUri;
    int payloadSize = 0;

    payloadSize = strlen(payload);
    requestData.payload = (CAPayload_t) malloc(payloadSize);
    requestData.payloadSize = payloadSize;
    memcpy(requestData.payload, payload, payloadSize);

    requestData.type = CA_MSG_NONCONFIRM;
    CAHeaderOption_t* headerOpt = NULL;

    CARequestInfo_t requestInfo = { 0 };
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;
    requestInfo.isMulticast = false;

    result = CASendRequest(endpoint, &requestInfo);
    if (result != CA_STATUS_OK)
    {
        printf("CASendRequest failed. return value is %d", result);
        return -1;
    }

    CADestroyEndpoint(endpoint);
    CADestroyToken(token);

    if(requestData.payload)
    {
        free(requestData.payload);
    }

    printf("press 'h' to call CAHandleRequestResponse and 'q' to quit\n");

    while(scanf("%s", command) == 1)
    {
        printf("press 'h' to call CAHandleRequestResponse and 'q' to quit\n");
        if(command[0] == 'q')
        {
            break;
        }
        if(command[0] == 'h')
        {
            result = CAHandleRequestResponse();
            if (result != CA_STATUS_OK)
            {
                printf("CAHandleRequestResponse failed. return value is %d", result);
                return -1;
            }
        }
    }
}
