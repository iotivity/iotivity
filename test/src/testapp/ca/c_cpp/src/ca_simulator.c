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

#define IDENTITY     ("1111111111111111")
#define RS_CLIENT_PSK   ("AAAAAAAAAAAAAAAA")

#define SIM_REQ_CONFIG "Configure"
#define SIM_REQ_ACK "SendReqAck"
#define SIM_REQ_QUERY "SendReqQry"
#define SIM_RES_ACK "SendResAck"

#define MAX_BUF_LEN 2048
#define MAX_OPT_LEN 16
#define MAX_SLEEP_TIME 1
#define CH_ZERO '0'

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <stdarg.h>

#ifdef __TC_PLATFORM_LINUX__
#include <unistd.h>
#endif

#ifdef __TC_PLATFORM_TIZEN__
#include <unistd.h>
#include <glib.h>
#include <pthread.h>

#endif

#include "cacommon.h"
#include "cainterface.h"
#include "casimulator.h"
#include "cautilinterface.h"

#ifdef __TC_PLATFORM_ARDUINO__
#include "Arduino.h"
#ifdef ARDUINOWIFI
#include "WiFi.h"
#elif defined ARDUINOETH
#include "Ethernet.h"
#endif
#endif

#ifdef __TC_PLATFORM_TIZEN__
static GMainLoop *g_mainloop = NULL;
pthread_t thread;
#endif

typedef enum
{
    SEND_MESSAGE = 0, SELECT_NETWORK, UNSELECT_NETWORK, STOP_SIM, TRANSFER_INFO
} SimulatorTask;

typedef enum
{
    MESSAGE_RESPONSE = 0, MESSAGE_REQUEST
} MessageCommandType;

typedef struct
{
    SimulatorTask operationType;
    MessageCommandType messageType;
    CAMethod_t caMethod;
    CAToken_t token;
    uint8_t tokenLength;
    int numberOfTimes;
    int interval;
    int bufLength;
    char* resourceUri;
    char payload[MAX_BUF_LEN];
    int payloadSize;
    uint16_t messageId;
} TestConfiguration;

int g_selectedNetwork = 0;
bool g_simulatorProcess = true;
int g_messageId = -1;
bool g_firstMessage = true;
int g_mode;
int g_server = 0;
int g_client = 1;

void output(const char *format, ...)
{
//#ifdef __TC_PLATFORM_LINUX__
#if defined (__TC_PLATFORM_LINUX__) || defined (__TC_PLATFORM_ANDROID_NATIVE__) || defined (__TC_PLATFORM_TIZEN__)
    va_list arg;

    va_start (arg, format);

    vfprintf (stdout, format, arg);

    va_end (arg);
#endif

#ifdef __TC_PLATFORM_ARDUINO__
    Serial.print(format);
#endif

}

#ifdef __TC_PLATFORM_ARDUINO__
char* getData()
{
    char readInput[MAX_BUF_LEN+1];

    while (!Serial.available())
    {

    }

    int len = 0;

    while (Serial.available())
    {
        delay(100);

        char c = Serial.read();

        if ('\n' != c && '\r' != c && len < MAX_BUF_LEN)
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
    Serial.print("PD: ");
    Serial.println(readInput);

    return readInput;
}
#endif

void customWait(int seconds)
{
#ifdef __TC_PLATFORM_LINUX__
    sleep(seconds);
#endif

#ifdef __TC_PLATFORM_ARDUINO__
    delay(seconds);
#endif
}

void clearDisplay()
{
#ifdef __TC_PLATFORM_LINUX__
    system("clear");
#endif

#ifdef __TC_PLATFORM_ARDUINO__
#endif
}

#ifdef __TC_PLATFORM_TIZEN__
void GMainLoopThread()
{
    g_main_loop_run(g_mainloop);
}
#endif

int initialize()
{
    CAResult_t result = CAInitialize();

    if (result != CA_STATUS_OK)
    {
        output("CAInitialize failed\n");
        return 0;
    }

    return 1;
}

char* getString(char a[], int length)
{
    char str[MAX_BUF_LEN];

    int i;

    for (i = 0; i < length; i++)
    {
        str[i] = a[i];
    }

    str[length] = 0;

    return str;
}

#ifdef __WITH_DTLS__

int32_t getDtlsPskCredentials( CADtlsPskCredType_t type, const unsigned char *desc,
        size_t desc_len, unsigned char *result, size_t result_length)
{
    output("getDtlsPskCredentials in\n");

    int32_t ret = -1;

    if (NULL == result)
    {
        return ret;
    }

    switch (type)
    {
        case CA_DTLS_PSK_HINT:
        case CA_DTLS_PSK_IDENTITY:
        output("CAGetDtlsPskCredentials CA_DTLS_PSK_IDENTITY\n");
        if (result_length < sizeof(IDENTITY))
        {
            output("ERROR : Wrong value for result for storing IDENTITY\n");
            return ret;
        }

        memcpy(result, IDENTITY, sizeof(IDENTITY));
        ret = sizeof(IDENTITY);
        break;

        case CA_DTLS_PSK_KEY:
        output("CAGetDtlsPskCredentials CA_DTLS_PSK_KEY\n");
        if ((desc_len == sizeof(IDENTITY)) &&
                memcmp(desc, IDENTITY, sizeof(IDENTITY)) == 0)
        {
            if (result_length < sizeof(RS_CLIENT_PSK))
            {
                output("ERROR : Wrong value for result for storing RS_CLIENT_PSK\n");
                return ret;
            }

            memcpy(result, RS_CLIENT_PSK, sizeof(RS_CLIENT_PSK));
            ret = sizeof(RS_CLIENT_PSK);
        }
        break;

        default:

        output("Wrong value passed for PSK_CRED_TYPE.\n");
        ret = -1;
    }

    output("getDtlsPskCredentials out\n");

    return ret;
}

int registerDtlsHandler()
{
    CAResult_t result = CARegisterDTLSCredentialsHandler(getDtlsPskCredentials);
    if (result != CA_STATUS_OK)
    {
        output("CARegisterDTLSCredentialsHandler FAILED\n");
    }

    return 1;
}

#endif

int selectNetwork()
{
    int number;
    int ret = 0;

    output("\n=============================================\n");

    output("\tSelect Network\n");
    output("IP     : 0\n");
    output("GATT   : 1\n");
    output("RFCOMM : 2\n");
    output("TCP    : 4\n");
    output("Select : ");

    scanf("%d", &number);
//    number = 0;

    if (number >= 0 && number <= 4)
    {
        g_selectedNetwork = 1 << number;

        output("Selecting Network ...\n");

        CAResult_t result = CASelectNetwork(g_selectedNetwork);

        if (result == CA_STATUS_OK)
        {
            output("CASelectNetwork Successful\n");
            ret = 1;
        }
        else
        {
            output("CASelectNetwork Failed\n");
            ret = 0;
        }
    }
    else
    {
        output("Invalid selection\n");
        ret = 0;
    }

    output("\n=============================================\n");

    return ret;
}

int selectAndStartServerOrClient()
{
    output("\n=============================================\n");

    output("\tSelect Server/Client\n");
    output("Server\t\t: 0\n");
    output("Client\t\t: 1\n");

    int ret = 0;
    CAResult_t result;

#if defined (__TC_PLATFORM_LINUX__) || defined (__TC_PLATFORM_ANDROID_NATIVE__) || defined (__TC_PLATFORM_TIZEN__)
    output("Select\t\t: ");
    scanf("%d", &g_mode);
//    g_mode = 0;
#endif

#ifdef __TC_PLATFORM_ARDUINO__

    while(1)
    {
        output("Select\t\t: \n");
        if (Serial.available() > 0)
        {
            char* buffer = getData();

            if (buffer[0] == '0' || buffer[0] == '1')
            {
                g_mode = atoi(buffer);
                break;
            }
            else
            {
                Serial.println("Select Server/Client (0/1)");
            }
        }

        delay(500);
    }

#endif

    if (g_mode < 0 || g_mode > 1)
    {
        output("Invalid Selection\n");
    }
    else if (g_mode == g_server)
    {
        output("Server is starting ...\n");
        ret = startServer();
    }
    else if (g_mode == g_client)
    {
        output("Client is starting ...\n");
        ret = startClient();
    }

    output("\n=============================================\n");

    return ret;
}

int startServer()
{
    CAResult_t result = CAStartListeningServer();
    if (result != CA_STATUS_OK)
    {
        output("CAStartListeningServer Failed\n");
        return 0;
    }
    else
    {
        output("Server Started Successfully\n");
    }

    return 1;
}

int startClient()
{
    CAResult_t result = CAStartDiscoveryServer();
    if (result != CA_STATUS_OK)
    {
        output("CAStartDiscoveryServer\n");
        return 0;
    }
    else
    {
        output("Client Started Successfully\n");
    }

    return 1;
}

int handleMessage()
{
    CAResult_t res = CAHandleRequestResponse();

    if (res != CA_STATUS_OK)
    {
        return 0;
    }

    return 1;
}

int returnRequest(const CAEndpoint_t* endPoint, char* resourceUri, char* payload, int payloadSize,
        CAMessageType_t type, CAMethod_t method, CAToken_t token, uint8_t tokenLength,
        CAHeaderOption_t *options, uint8_t numOptions)
{
    output("[returnRequest] in\n");

    CAInfo_t requestData =
    { 0 };
    requestData.token = token;
    requestData.tokenLength = tokenLength;
    requestData.payload = payload;
    if (payload != NULL)
    {
        requestData.payloadSize = payloadSize;
    }
    else
    {
        requestData.payloadSize = 0;
    }

    requestData.messageId = 0;
    requestData.type = type;
    requestData.resourceUri = (CAURI_t) resourceUri;
    requestData.options = options;
    requestData.numOptions = numOptions;

    CARequestInfo_t requestInfo =
    { 0 };
    requestInfo.method = method;
    requestInfo.info = requestData;
    requestInfo.isMulticast = false;

    CAResult_t res = CASendRequest(endPoint, &requestInfo);
    if (res != CA_STATUS_OK)
    {
        output("send request error\n");
        return 0;
    }
    else
    {
        output("send request success\n");
    }

    output("[returnRequest] out\n");

    return 1;
}

int returnResponse(const CAEndpoint_t* endPoint, char* resourceUri, char* payload, int payloadSize,
        CAMessageType_t type, CAResponseResult_t responseCode, uint16_t messageId, CAToken_t token,
        uint8_t tokenLength, CAHeaderOption_t *options, uint8_t numOptions)
{
    output("[returnResponse] in\n");

    CAInfo_t responseData =
    { 0 };

    responseData.payload = payload;

    if (payload != NULL)
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

    CAResponseInfo_t responseInfo =
    { 0 };
    responseInfo.result = responseCode;
    responseInfo.info = responseData;

    output("Sending response....\n");

    CAResult_t res = CASendResponse(endPoint, &responseInfo);

    output("Response Send....\n");

    if (res != CA_STATUS_OK)
    {
        output("send response error\n");
    }
    else
    {
        output("send response success\n");
    }

    output("[returnResponse] out\n");

    return 1;
}

void processRequestResponse(void)
{
    bool showSuccess = true;
    bool showError = true;

    while (g_simulatorProcess)
    {
        if (!handleMessage())
        {
            if (showError)
            {
                output("handle request error\n");
                showError = false;
                showSuccess = true;
            }
        }
        else
        {
            if (showSuccess)
            {
                output("handle request success\n");
                showSuccess = false;
                showError = true;
            }
        }

        customWait(MAX_SLEEP_TIME);
    }
}

void selectNetworkWithParam(char *chNetwork)
{

    int number = (int) (chNetwork - CH_ZERO);
    number = (number < 0 || number > 3) ? 1 : number;
    g_selectedNetwork = number;
    CASelectNetwork(1 << g_selectedNetwork);
}

void unSelectNetwork(void)
{
    output("unselected network in\n");

    CAResult_t res = CAUnSelectNetwork(1 << g_selectedNetwork);
    if (res != CA_STATUS_OK)
    {
        output("UNSelectNetwork fail\n");
    }

    output("unselected network out\n");
}

void returnMessage(const CAEndpoint_t* endPoint, TestConfiguration* testConfig)
{
    int index = 0;

    output("returnMessage in\n");

    switch (testConfig->messageType)
    {
        case MESSAGE_RESPONSE:
            for (index = 0; index < testConfig->numberOfTimes; index++)
            {
                returnResponse(endPoint, testConfig->resourceUri, testConfig->payload,
                        testConfig->payloadSize, CA_MSG_NONCONFIRM, CA_VALID, testConfig->messageId,
                        testConfig->token, testConfig->tokenLength, NULL, 0);
                customWait(testConfig->interval);
            }
            break;
        case MESSAGE_REQUEST:
            for (index = 0; index < testConfig->numberOfTimes; index++)
            {
                returnRequest(endPoint, testConfig->resourceUri, testConfig->payload,
                        testConfig->payloadSize, CA_MSG_NONCONFIRM, testConfig->caMethod,
                        testConfig->token, testConfig->tokenLength, NULL, 0);
                customWait(testConfig->interval);
            }
            break;
        default:
            break;
    }

    output("returnMessage out\n");
}

void requestHandler(const CAEndpoint_t* endPoint, const CARequestInfo_t* requestInfo)
{
    TestConfiguration testConfig;
    char numConversion[4];

    output("requestHandler in\n");

    if (!endPoint)
    {
        output("endPoint is NULL\n");
        return;
    }

    if (!requestInfo)
    {
        output("requestInfo is NULL\n");
        return;
    }

    output("IP %s, Port %d\n", endPoint->addr, endPoint->port);
    output("Message Id: %d\n", requestInfo->info.messageId);

    if (requestInfo->info.options)
    {
        output("Header Option Found\n");

        uint32_t len = requestInfo->info.numOptions;
        uint32_t i;
        for (i = 0; i < len; i++)
        {
            output("Option ID : %d\n", requestInfo->info.options[i].optionID);
            output("Option Data[%d]: %s\n", requestInfo->info.options[i].optionLength,
                    requestInfo->info.options[i].optionData);
        }
    }
    else
    {
        output("No Header Option Found\n");
    }

    if (requestInfo->info.type == CA_MSG_RESET)
    {
        char str[12];

        output("messageType: CA_MSG_RESET\n");

        sprintf(str, "%d", requestInfo->info.messageId);

        output("calling returnResponse ...\n");

        returnResponse(endPoint, SIM_RES_ACK, str, strlen(str), CA_MSG_NONCONFIRM, CA_VALID,
                requestInfo->info.messageId, requestInfo->info.token, requestInfo->info.tokenLength,
                requestInfo->info.options, requestInfo->info.numOptions);

        output("returnResponse called\n");

        return;
    }

    output("PayloadSize: %d\n", requestInfo->info.payloadSize);

    char *payload = NULL;

    if (requestInfo->info.payload)
    {
        payload = getString(requestInfo->info.payload, requestInfo->info.payloadSize);
        output("Payload: %s\n", payload);
    }
    else
    {
        output("Payload is NULL\n");
    }

    if (!requestInfo->info.resourceUri)
    {
        output("ResourceUri is NULL\n");
        return;
    }

    output("ResourceUri: %s\n", requestInfo->info.resourceUri);

    if (strstr(requestInfo->info.resourceUri, SIM_REQ_CONFIG) != NULL)
    {
        output("ResourceUri Type: SIM_REQ_CONFIG\n");

        testConfig.operationType = (requestInfo->info.payload[0] - CH_ZERO);

        switch (testConfig.operationType)
        {
            case SEND_MESSAGE:
                output("OperationType: SEND_MESSAGE\n");

                testConfig.messageType = (requestInfo->info.payload[1] - CH_ZERO);

                memset(&numConversion, 0, sizeof(numConversion));
                strncpy(numConversion, &requestInfo->info.payload[2], sizeof(numConversion));
                testConfig.numberOfTimes = atoi(numConversion); //4 byte

                memset(&numConversion, 0, sizeof(numConversion));
                strncpy(numConversion, &requestInfo->info.payload[6], sizeof(numConversion));
                testConfig.interval = atoi(numConversion); //4 byte

                memset(&numConversion, 0, sizeof(numConversion));
                strncpy(numConversion, &requestInfo->info.payload[10], sizeof(numConversion));
                testConfig.bufLength = atoi(numConversion); //4 byte

                memset(&testConfig.payload, 0, sizeof(char) * MAX_BUF_LEN);
                strncpy(testConfig.payload, &requestInfo->info.payload[14], testConfig.bufLength);
                testConfig.payload[testConfig.bufLength + 1] = '\0';
                testConfig.payloadSize = strlen(testConfig.payload);

                testConfig.caMethod = requestInfo->method;
                testConfig.resourceUri = requestInfo->info.resourceUri;
                testConfig.messageId = requestInfo->info.messageId;

                if (requestInfo->info.token != NULL)
                {
                    testConfig.token = requestInfo->info.token;
                    testConfig.tokenLength = requestInfo->info.tokenLength;
                }
                else
                {
                    testConfig.token = "";
                    testConfig.tokenLength = 0;
                }

                returnMessage(endPoint, &testConfig);

                break;

            case SELECT_NETWORK:
                output("OperationType: SELECT_NETWORK\n");
                selectNetworkWithParam(&requestInfo->info.payload[1]);
                break;

            case UNSELECT_NETWORK:
                output("OperationType: UNSELECT_NETWORK\n");
                selectNetworkWithParam(&requestInfo->info.payload[1]);
                break;

            case STOP_SIM:
                output("OperationType: STOP_SIM\n");
                g_simulatorProcess = false;
                break;

            default:
                output("OperationType: Unknown\n");
                break;
        }
    }
    else if (strstr(requestInfo->info.resourceUri, SIM_REQ_ACK) != NULL)
    {
        output("resourceUri Type: SIM_REQ_ACK\n");

        CAMessageType_t messageType;

        if (requestInfo->info.type == CA_MSG_CONFIRM)
        {
            output("Message Type: CA_MSG_CONFIRM\n");

            if (g_messageId != requestInfo->info.messageId)
            {
                output("Ignoring first time\n");
                g_messageId = requestInfo->info.messageId;
                return;
            }

            messageType = CA_MSG_ACKNOWLEDGE;
        }
        else
        {
            output("Message Type: CA_MSG_NONCONFIRM\n");
            messageType = CA_MSG_NONCONFIRM;
        }

        output("calling returnResponse ...\n");

        returnResponse(endPoint, requestInfo->info.resourceUri, requestInfo->info.payload,
                requestInfo->info.payloadSize, messageType, CA_VALID, requestInfo->info.messageId,
                requestInfo->info.token, requestInfo->info.tokenLength, requestInfo->info.options,
                requestInfo->info.numOptions);

        output("returnResponse called\n");
    }
    else
    {
        output("ResourceUri Type is Unknown\n");
    }

    output("requestHandler out\n");
}

void responseHandler(const CAEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo)
{
    output("responseHandler in\n");

    if (!endPoint)
    {
        output("endPoint is NULL\n");
        return;
    }

    if (!responseInfo)
    {
        output("responseInfo is NULL\n");
        return;
    }

    output("IP %s, Port %d\n", endPoint->addr, endPoint->port);
    output("MessageId: %d\n", responseInfo->info.messageId);

    if (responseInfo->info.options)
    {
        output("Header Option Found\n");

        uint32_t len = responseInfo->info.numOptions;
        uint32_t i;
        for (i = 0; i < len; i++)
        {
            output("Option ID: %d\n", responseInfo->info.options[i].optionID);
            output("Option Data[%d]: %s\n", responseInfo->info.options[i].optionLength,
                    responseInfo->info.options[i].optionData);
        }
    }
    else
    {
        output("No Header Option Found\n");
    }

    if (responseInfo->info.type == CA_MSG_RESET)
    {
        char str[12];

        output("Message Type: CA_MSG_RESET\n");

        sprintf(str, "%d", responseInfo->info.messageId);

        output("calling returnRequest ...\n");

        returnRequest(endPoint, SIM_REQ_ACK, str, strlen(str), CA_MSG_NONCONFIRM, CA_GET,
                responseInfo->info.token, responseInfo->info.tokenLength,
                responseInfo->info.options, responseInfo->info.numOptions);

        output("returnRequest called\n");

        return;
    }

    char *payload;

    if (responseInfo->info.payload)
    {
        payload = getString(responseInfo->info.payload, responseInfo->info.payloadSize);
        output("Payload: %s\n", payload);
    }
    else
    {
        output("Payload is NULL\n");
    }

    if (!responseInfo->info.resourceUri)
    {
        output("ResourceUri is NULL\n");
        return;
    }

    output("ResourceUri: %s\n", responseInfo->info.resourceUri);

    if (strstr(responseInfo->info.resourceUri, SIM_RES_ACK) != NULL)
    {
        CAMessageType_t messageType;

        if (responseInfo->info.type == CA_MSG_NONCONFIRM)
        {
            output("Message Type: CA_MSG_NONCONFIRM\n");
            messageType = CA_MSG_NONCONFIRM;
        }
        else if (responseInfo->info.type == CA_MSG_CONFIRM)
        {
            output("Message Type: CA_MSG_CONFIRM\n");
            messageType = CA_MSG_ACKNOWLEDGE;

            if (g_firstMessage == true)
            {
                output("Ignoring first time\n");
                g_firstMessage = false;
                return;
            }

            g_firstMessage = true;
        }
        else
        {
            output("Message Type: Unknown\n");
        }

        output("calling returnRequest ...\n");

        returnRequest(endPoint, responseInfo->info.resourceUri, responseInfo->info.payload,
                responseInfo->info.payloadSize, messageType, CA_GET, responseInfo->info.token,
                responseInfo->info.tokenLength, responseInfo->info.options,
                responseInfo->info.numOptions);

        output("returnRequest called\n");
    }
    else
    {
        output("Unknown ResourceUri Type\n");
    }
}

void errorHandler(const CAEndpoint_t *endPoint, const CAErrorInfo_t* errorInfo)
{
    output("errorHandler in\n");

    if (!errorInfo)
    {
        output("ErrorInfo NULL\n");
        return;
    }

    if (!endPoint)
    {
        output("endPoint is NULL\n");
        return;
    }

    output("IP %s, Port %d\n", endPoint->addr, endPoint->port);

    const CAInfo_t *info = &errorInfo->info;

    if (!info)
    {
        output("CAInfo is NULL\n");
        return;
    }

    output("result: %d\n", errorInfo->result);

    if (info->token)
    {
        output("token: %s\n", info->token);
    }
    else
    {
        output("token is NULL\n");
    }

    output("tokenLength: %d\n", info->tokenLength);
    output("messageId: %d\n", (uint16_t) info->messageId);
    output("type: %d\n", info->type);
    output("payloadSize: %d\n", info->payloadSize);

    if (info->resourceUri)
    {
        output("resourceUri: %s\n", info->resourceUri);
    }
    else
    {
        output("resourceUri is NULL\n");
    }

    if (info->payload)
    {
        output("payload: %s\n", getString(info->payload, info->payloadSize));
    }
    else
    {
        output("Payload is NULL\n");
    }

    if (CA_ADAPTER_NOT_ENABLED == errorInfo->result)
    {
        output("CA_ADAPTER_NOT_ENABLED, enable the adapter\n");
    }
    else if (CA_SEND_FAILED == errorInfo->result)
    {
        output("CA_SEND_FAILED, unable to send the message, check parameters\n");
    }
    else if (CA_MEMORY_ALLOC_FAILED == errorInfo->result)
    {
        output("CA_MEMORY_ALLOC_FAILED, insufficient memory\n");
    }
    else if (CA_SOCKET_OPERATION_FAILED == errorInfo->result)
    {
        output("CA_SOCKET_OPERATION_FAILED, socket operation failed\n");
    }
    else if (CA_STATUS_FAILED == errorInfo->result)
    {
        output("CA_STATUS_FAILED, message could not be delivered, internal error\n");
    }

    output("errorHandler out\n");
}

int registerMessageHandler()
{
    CARegisterHandler(requestHandler, responseHandler, errorHandler);

    return 1;
}

#ifdef __TC_PLATFORM_ARDUINO__

void setup()
{
    Serial.print("setup in");

    Serial.begin(115200);
    Serial.println("=======================");
    Serial.println("CA Simulator");
    Serial.println("=======================");
    Serial.println("Enter P to pause & R to Resume (if paused) simulator");
    Serial.println("=======================");

#ifdef ARDUINOETH
    g_selectedNetwork = CA_ADAPTER_IP;
    // Note: ****Update the MAC address here with your shield's MAC address****
    uint8_t ETHERNET_MAC[] =
    {   0x90, 0xA2, 0xDA, 0x0F, 0xE0, 0xD8};
    uint8_t error = Ethernet.begin(ETHERNET_MAC);
    if (error == 0)
    {
        Serial.print("Failed: ");
        Serial.println(error);
        return;
    }
    Serial.print("Ethernet IP: ");
    Serial.println(Ethernet.localIP());
    Serial.print("Port: ");
    Serial.println(caglobals.ip.u4.port);
#endif

#ifdef ARDUINOWIFI
    g_selectedNetwork = CA_ADAPTER_IP;

    const char ssid[] = "NETGEAR24"; // your network SSID (name)
    const char pass[] = "mightyrabbit219";// your network password

    int16_t status = WL_IDLE_STATUS; // the Wifi radio's status

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
    Serial.print("Wifi IP:");
    Serial.println(WiFi.localIP());
#endif

#ifdef LE_ADAPTER
    g_selectedNetwork = CA_ADAPTER_GATT_BTLE;
#endif

    CAResult_t result = CA_STATUS_OK;

    result = CAInitialize();
    if (result == CA_STATUS_OK)
    {
        Serial.println("Initialization Successful");
    }

    result = CASelectNetwork(CA_ADAPTER_IP);
    if (result == CA_STATUS_OK)
    {
        Serial.println("Network Selection Successful");
    }

//    result = CAStartListeningServer();
//    if (result == CA_STATUS_OK)
//    {
//        Serial.println("Listening Server Started");
//    }

    selectAndStartServerOrClient();

    registerMessageHandler();
    Serial.println("Callbacks registered");
}

void loop()
{
    char buffer[5];
    memset(buffer, 0, sizeof(buffer));
    int16_t len;
    int loop_status = 1;

    CAResult_t result = CA_STATUS_OK;

    if (g_simulatorProcess)
    {
        result = CAHandleRequestResponse();

        if (result == CA_STATUS_OK)
        {
            Serial.println("CAHandleRequestResponse Succeeded");
        }
        else
        {
            Serial.println("CAHandleRequestResponse Failed!");
        }

        delay(MAX_SLEEP_TIME * 1000);
    }

    if (Serial.available() > 0)
    {
        char* buffer = getData();
        switch (toupper(buffer[0]))
        {
            case 'P':
            g_simulatorProcess = false;
            Serial.println("Simulator paused");
            break;

            case 'R':
            g_simulatorProcess = true;
            Serial.println("Simulator resumed");
            break;
            case 'E':
            g_simulatorProcess = false;
            loop_status = 0;
            Serial.println("Simulator stopped");
            break;
        }
    }

    if(!loop_status)
    {
        exit(0);
    }
}

#endif

#if defined (__TC_PLATFORM_LINUX__) || defined (__TC_PLATFORM_ANDROID_NATIVE__) || defined (__TC_PLATFORM_TIZEN__)
int main()
{
    CAResult_t result;
    CAEndpoint_t *tempInfo = NULL;
    uint32_t tempSize = 0;

    clearDisplay();

    output("[CASimulator] IN\n");

#ifdef __TC_PLATFORM_TIZEN__
    g_mainloop = g_main_loop_new(NULL, FALSE);
    if(!g_mainloop)
    {
        output("g_main_loop_new failed\n");
        return -1;
    }

    if (pthread_create(&thread, NULL, (void *) &GMainLoopThread, NULL) < 0)
    {
        output("pthread_create failed in initialize\n");
        return -1;
    }
#endif

    caglobals.server = true;

    if (!initialize())
    {
        return -1;
    }

#ifdef __WITH_DTLS__
    if(!registerDtlsHandler())
    {
        return -1;
    }
#endif

    if(!selectNetwork())
    {
        return -1;
    }

    if(!selectAndStartServerOrClient())
    {
        return -1;
    }

    if(!registerMessageHandler())
    {
        return -1;
    }

    if(g_selectedNetwork == CA_ADAPTER_IP)
    {
        output("Unsecured Port: %d;\n", CAGetAssignedPortNumber(CA_ADAPTER_IP, CA_IPV4));
        output("Secured Port: %d;\n", CAGetAssignedPortNumber(CA_ADAPTER_IP, CA_SECURE|CA_IPV4));
    }
    else if(g_selectedNetwork == CA_ADAPTER_TCP)
    {
        output("Unsecured Port: %d;\n", CAGetAssignedPortNumber(CA_ADAPTER_TCP, CA_IPV4));
    }

    processRequestResponse();

    result = CAUnSelectNetwork(g_selectedNetwork);
    if (result != CA_STATUS_OK)
    {
        output("CAUnSelectNetwork Failed\n");
        return -1;
    }

    CATerminate();

    output("[CASimulator] out\n");

#ifdef __TC_PLATFORM_TIZEN__
    g_main_loop_quit(g_mainloop);
#endif

    return 0;
}

#endif
