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

#include "casimulator.h"
#include "cacommon.h"
#include "cainterface.h"
#include "cautilinterface.h"
#include "casecurityinterface.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef __LINUX__
#include <unistd.h>
#endif

#ifdef __WITH_DTLS__
#include "ca_adapter_net_ssl.h"
#include "ssl_ciphersuites.h"
#endif

#ifdef TIZEN
#include <glib.h>
#include <pthread.h>

static GMainLoop *g_mainloop = NULL;
pthread_t thread;
#endif

int g_selectedNetwork = 0;
size_t g_identityLegth;
size_t g_pskLength;

void getString(char a[], char str[], int length)
{
    int i;

    for(i = 0; i < length; i++)
    {
        str[i] = a[i];
    }

    printf("\n");

    str[length] = 0;
}

#ifdef __WITH_DTLS__

CAResult_t dtlsHandshakeCb(const CAEndpoint_t *endpoint, const CAErrorInfo_t *info)
{
    if (NULL == endpoint)
    {
      printf("endpoint is null");
      return CA_STATUS_FAILED;
    }

    if (NULL == info)
    {
        printf("ErrorInfo: %d", info->result);
        return CA_STATUS_FAILED;
    }
    printf("Remote device Address %s:%d:", endpoint->addr, endpoint->port);
    printf("ErrorInfo is null");
    return CA_STATUS_OK;
}

void initCipherSuiteList(bool * list, const char* deviceId)
{
    (void) deviceId;

    printf("%s In\n", __func__);

    if (NULL == list)
    {
        printf("Out %s", __func__);
        printf("NULL list param");
        return;
    }

    list[0] = true;
    list[1] = true;

    printf("%s Out\n", __func__);
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
        if (result_length < g_identityLegth)
        {
            printf("ERROR : Wrong value for result for storing IDENTITY\n");
            return ret;
        }

        memcpy(result, IDENTITY, g_identityLegth);
        ret = g_identityLegth;
        break;

        case CA_DTLS_PSK_KEY:
        printf("CAGetDtlsPskCredentials CA_DTLS_PSK_KEY\n");
        if ((desc_len == g_identityLegth) &&
                memcmp(desc, IDENTITY, g_identityLegth) == 0)
        {
            if (result_length < g_pskLength)
            {
                printf("ERROR : Wrong value for result for storing RS_CLIENT_PSK\n");
                return ret;
            }

            memcpy(result, RS_CLIENT_PSK, g_pskLength);
            ret = g_pskLength;
        }
        break;

        default:

        printf("Wrong value passed for PSK_CRED_TYPE.\n");
        ret = -1;
    }

    printf("getDtlsPskCredentials out\n");

    return ret;
}

int setupSecurity(int selectedTransport)
{
    g_identityLegth = strlen(IDENTITY);
    g_pskLength = strlen(RS_CLIENT_PSK);

    int result = CAregisterPskCredentialsHandler(getDtlsPskCredentials);
    if (result != CA_STATUS_OK)
    {
        printf("CAregisterPskCredentialsHandler failed. return value is %d\n", result);
        return 0;
    }

    if(selectedTransport == CA_ADAPTER_TCP)
    {
        result = CAregisterSslHandshakeCallback(dtlsHandshakeCb);
        if (result != CA_STATUS_OK)
        {
            printf("CAregisterSslHandshakeCallback failed. return value is %d\n", result);
            return 0;
        }
    }

    CAsetCredentialTypesCallback(initCipherSuiteList);

    if(selectedTransport == CA_ADAPTER_IP)
    {
        result = CASelectCipherSuite(MBEDTLS_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256, (CATransportAdapter_t)selectedTransport);
    }
    else if(selectedTransport == CA_ADAPTER_TCP)
    {
        result = CAsetTlsCipherSuite(MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8);
    }

    if (result != CA_STATUS_OK)
    {
        printf("CACipherSuite failed. return value is %d\n", result);
        return 0;
    }

    return 1;
}

#endif

int returnResponse(const CAEndpoint_t* endPoint, char* resourceUri, char* payload, int payloadSize,
        CAMessageType_t type, CAResponseResult_t responseCode, uint16_t messageId, CAToken_t token,
        uint8_t tokenLength, CAHeaderOption_t *options, uint8_t numOptions)
{
    printf("[returnResponse] in\n");

    CAInfo_t responseData;

    responseData.payload = (CAPayload_t)payload;

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
    responseData.dataType = CA_RESPONSE_DATA;
    responseData.payloadFormat = CA_FORMAT_UNDEFINED;
    responseData.acceptFormat = CA_FORMAT_UNDEFINED;

    CAResponseInfo_t responseInfo;
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
        char str[MAX_BUF_LEN];
        getString((char*)requestInfo->info.payload, str, requestInfo->info.payloadSize);
        payload = str;
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
        (char*)requestInfo->info.payload, requestInfo->info.payloadSize, messageType, CA_VALID,
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
    int number;
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

    result = CAInitialize(CA_ADAPTER_TCP);
    if (result != CA_STATUS_OK)
    {
        printf("CAInitialize failed. return value is %d", result);
        return -1;
    }

    CARegisterHandler(requestHandler, responseHandler, errorHandler);

    printf("\tSelect Network\n");
    printf("IP     : 0\n");
    printf("TCP    : 4\n");
    printf("Select : ");

    scanf("%d", &number);
    g_selectedNetwork = 1 << number;

    result = CASelectNetwork((CATransportAdapter_t)g_selectedNetwork);
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

#ifdef __WITH_DTLS__
    if(!setupSecurity(g_selectedNetwork))
    {
        return -1;
    }
#endif

    if(g_selectedNetwork == CA_ADAPTER_IP)
    {
        printf("Unsecured Port: %d;\n", CAGetAssignedPortNumber(CA_ADAPTER_IP, CA_IPV4));
#ifdef __WITH_DTLS__
        printf("Secured Port: %d;\n", CAGetAssignedPortNumber(CA_ADAPTER_IP, (CATransportFlags_t)(CA_SECURE|CA_IPV4)));
#endif
    }
    else if(g_selectedNetwork == CA_ADAPTER_TCP)
    {
        printf("Unsecured Port: %d;\n", CAGetAssignedPortNumber(CA_ADAPTER_TCP, CA_IPV4));
#ifdef __WITH_DTLS__
        printf("Secured Port: %d;\n", CAGetAssignedPortNumber(CA_ADAPTER_TCP, (CATransportFlags_t)(CA_SECURE|CA_IPV4)));
#endif
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

