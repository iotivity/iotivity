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

int returnResponse(const CAEndpoint_t* endPoint, char* resourceUri, char* payload, int payloadSize,
        CAMessageType_t type, CAResponseResult_t responseCode, uint16_t messageId, CAToken_t token,
        uint8_t tokenLength, CAHeaderOption_t *options, uint8_t numOptions)
{
    printf("[returnResponse] in\n");

    CAInfo_t responseData = { 0 };

    responseData.payload = payload;

    if(payload != NULL)
    {
        responseData.payloadSize = payloadSize;
    }
    else
    {
        responseData.payloadSize = 0;
    }

    responseData.type = type;
    responseData.messageId = messageId;
    responseData.resourceUri = resourceUri;
    responseData.token = token;
    responseData.tokenLength = tokenLength;
    responseData.options = options;
    responseData.numOptions = numOptions;

    CAResponseInfo_t responseInfo = { 0 };
    responseInfo.result = responseCode;
    responseInfo.info = responseData;

    printf("Sending response....\n");

    CAResult_t res = CASendResponse(endPoint, &responseInfo);

    printf("Response Send....\n");

    if (res != CA_STATUS_OK)
    {
        printf("send response error\n");
    }
    else
    {
        printf("send response success\n");
    }

    printf("[returnResponse] out\n");

    return 1;
}

void requestHandler(const CAEndpoint_t* endPoint, const CARequestInfo_t* requestInfo)
{
    char *payload = NULL;
    CAMessageType_t messageType = CA_MSG_ACKNOWLEDGE;

    printf("requestHandler in\n");

    if(!endPoint)
    {
        printf("endPoint is NULL\n");
        return;
    }

    if(!requestInfo)
    {
        printf("requestInfo is NULL\n");
        return;
    }

    printf("IP %s, Port %d\n", endPoint->addr, endPoint->port);
    printf("Message Id: %d\n", requestInfo->info.messageId);

    if (requestInfo->info.options)
    {
        printf("Header Option Found\n");

        uint32_t len = requestInfo->info.numOptions;
        uint32_t i;
        for (i = 0; i < len; i++)
        {
            printf("Option ID : %d\n", requestInfo->info.options[i].optionID);
            printf("Option Data[%d]: %s\n", requestInfo->info.options[i].optionLength,
                   requestInfo->info.options[i].optionData);
        }
    }
    else
    {
        printf("No Header Option Found\n");
    }

    printf("PayloadSize: %d\n", requestInfo->info.payloadSize);

    if(requestInfo->info.payload)
    {
        payload = getString(requestInfo->info.payload, requestInfo->info.payloadSize);
        printf("Payload: %s\n", payload);
    }
    else
    {
        printf("Payload is NULL\n");
    }

    if(!requestInfo->info.resourceUri)
    {
        printf("ResourceUri is NULL\n");
        return;
    }

    printf("ResourceUri: %s\n", requestInfo->info.resourceUri);

    returnResponse(endPoint, requestInfo->info.resourceUri,
        requestInfo->info.payload, requestInfo->info.payloadSize, messageType, CA_VALID,
        requestInfo->info.messageId, requestInfo->info.token, requestInfo->info.tokenLength,
        requestInfo->info.options, requestInfo->info.numOptions);

    printf("requestHandler out\n");
}

void responseHandler(const CAEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo)
{

}

void errorHandler(const CAEndpoint_t *endPoint, const CAErrorInfo_t* errorInfo)
{
    printf("errorHandler in\n");

    printf("errorHandler out\n");
}

int main()
{
    CAResult_t result;
    uint32_t tempSize = 0;
    CAEndpoint_t *tempInfo = NULL;

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

    result = CAStartListeningServer();
    if (result != CA_STATUS_OK)
    {
        printf("CAStartListeningServer failed. return value is %d", result);
        return -1;
    }

    result = CAGetNetworkInformation(&tempInfo, &tempSize);
    if (CA_STATUS_OK != result || NULL == tempInfo || 0 >= tempSize)
    {
        printf("Network not connected\n");
        free(tempInfo);
        return;
    }

    int index;
    for (index = 0; index < tempSize; index++)
    {
        printf("Type: %d\n", tempInfo[index].adapter);
        printf("Address: %s\n", tempInfo[index].addr);
    }


    printf("Unsecured Port: %d;\n", caglobals.ip.u4.port);
    printf("Secured Port: %d;\n", caglobals.ip.u4s.port);

    while(1)
    {
        result = CAHandleRequestResponse();
        if (result != CA_STATUS_OK)
        {
            printf("CAHandleRequestResponse failed. return value is %d", result);
            return -1;
        }
    }
}
