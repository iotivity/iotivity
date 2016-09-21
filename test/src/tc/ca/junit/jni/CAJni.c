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

#include <jni.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <android/log.h>

#include "cainterface.h"
#include "cacommon.h"
#include "caadapterutils.h"

#include "org_iotivity_CAJni.h"

#define  LOG_TAG   "CA_JNI"
#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define IDENTITY     ("1111111111111111")
#define RS_CLIENT_PSK   ("AAAAAAAAAAAAAAAA")

#define CONFIG_FILE "/data/local/tmp/config.ini"

#define SEND_REQ_ACK "SendReqAck"
#define SEND_RES_ACK "SendResAck"
#define SEND_REQ_CONFIG "Configure"

#define ARBITRARY_ADDRESS "78:05:71:C3:EA:03"
#define ARBITRARY_PORT 10000

#define NORMAL_PAYLOAD_LENGTH 10
#define LARGE_PAYLOAD_LENGTH 1500
#define MAX_BUF_LEN 2500
#define MAX_NUM_DEVICE 10
#define MAX_LINE_SIZE 50
#define TOTAL_WORD 3

#define NETWORK_TYPE_LOBV_INVALID -1

#define NETWORKINFO_ENDPOINT_VALID 0
#define NETWORKINFO_ENDPOINT_UFV 1
#define NETWORKINFO_ENDPOINT_NULL 2
#define NETWORKINFO_SIZE_VALID 0
#define NETWORKINFO_SIZE_USV 1
#define NETWORKINFO_SIZE_NULL 2

#define ENDPOINT_VALID  0
#define ENDPOINT_INVALID_FLAG 1
#define ENDPOINT_INVALID_ADAPTER 2
#define ENDPOINT_INVALID_ADDRESS 3
#define ENDPOINT_INVALID_PORT 4
#define ENDPOINT_INVALID_ENDPOINT 5
#define ENDPOINT_ESV_ADDRESS 6
#define ENDPOINT_URV_ENDPOINT 7
#define ENDPOINT_NULL 8

#define TOKEN_TYPE_INVALID_REFERENCE 0
#define TOKEN_TYPE_NULL 1
#define TOKEN_TYPE_VALID 2

const int URI_TYPE_NULL_ENDPOINT = -3;
const int URI_TYPE_ARBITRARY_ENDPOINT = -2;
const int URI_TYPE_NULL = -1;
const int URI_TYPE_EMPTY = 0;
const int URI_TYPE_NORMAL = 1;
const int URI_TYPE_CONFIG = 2;

const int PAYLOAD_TYPE_NULL = -1;
const int PAYLOAD_TYPE_EMPTY = 0;
const int PAYLOAD_TYPE_NORMAL = 1;
const int PAYLOAD_TYPE_LARGE = 2;
const int PAYLOAD_TYPE_CONFIG = 3;

const int HEADER_NONE = 0;
const int HEADER_ONE = 1;
const int HEADER_TWO = 2;

const int HANDLER_NORMAL = 1;
const int HANDLER_NULL = 0;
const int HANDLER_SECOND = 2;

const int TOTAL_MESSAGE = 1;
const int MAX_ATTEMPT = 10;

static const char COAP_PREFIX[] =  "coap://";
static const char COAPS_PREFIX[] = "coaps://";
static const char COAP_TCP_PREFIX[] =  "coap+tcp://";
static const uint16_t COAP_PREFIX_LEN = sizeof(COAP_PREFIX) - 1;
static const uint16_t COAPS_PREFIX_LEN = sizeof(COAPS_PREFIX) - 1;
static const uint16_t COAP_TCP_PREFIX_LEN = sizeof(COAP_TCP_PREFIX) - 1;

const int firstOptionId = 1234;
const int secondOptionId = 1235;
const char firstOptionData[] = "Hello";
const char secondOptionData[] = "World";

typedef enum
{
    MESSAGE_NONSECURE = 0, MESSAGE_SECURE
} MessageSecurityType;

typedef enum
{
    MESSAGE_INCOMING = 0, MESSAGE_OUTGOING
} MessageInOutType;

typedef enum
{
    MESSAGE_PAYLOAD = 0, MESSAGE_URI, MESSAGE_HEADER
} MessageDataType;

typedef enum
{
    MESSAGE_UNICAST = 0, MESSAGE_MULTICAST
} MessageCommunicationType;

typedef enum
{
    SEND_MESSAGE = 0, SELECT_NETWORK, UNSELECT_NETWORK, STOP_SIM, TRANSFER_INFO
} SimulatorTask;

typedef enum
{
    MESSAGE_RESPONSE = 0,
    MESSAGE_REQUEST
} MessageCommandType;

struct TestCaseInfo
{
    MessageSecurityType securityType;
    MessageInOutType inOutType;
    CAMethod_t caMethod;
    MessageDataType validationMethod;
    MessageCommunicationType communicationType;
    CAMessageType_t messageType;
    char payload[2048];
    CAHeaderOption_t *options;
    uint8_t numOptions;
}  s_tcInfo;

static jobject g_responseListenerObject = NULL;
static JavaVM *g_jvm;

static char configPayload[100];
int m_availableNetwork;

CAEndpoint_t *m_endpoint = NULL;
CAToken_t m_token = NULL;

char m_remoteAddressSet[10][50];
int m_remoteAddressCount;
int m_receiveCount[10];
int m_remotePort;
int isAddressAlreadyKnown = 1;
int EDNPOINT_LOBV_PORT = -1;

int getIndexOfRemoteAddress(const char* address);
static void errorHandler(const CAEndpoint_t *object, const CAErrorInfo_t* errorInfo);
static void delete_global_references(JNIEnv *env, jclass cls);
void getRandomString(size_t length);
const char* getResultName(CAResult_t result);
static CAResult_t get_network_type(uint32_t selectedNetwork);
bool checkHeader(CAHeaderOption_t *options, uint32_t len);
int compareAndIncrement(const char* endpointResourceUri,
        const char *otherResourceUri, const char* payload, const char* remoteAddress);
void comparePrefixAndIncrement(const CAEndpoint_t* endpoint, const char* resourceUri, const char *q);
bool isSendAddressUnknown(const CAEndpoint_t* endpoint);
bool showAndCheckEndpointAttributes(const CAEndpoint_t* endpoint, char* resourceUri);
bool showAndCheckResponseInfoAttributes(const CAResponseInfo_t* responseInfo);
bool showAndCheckRequestInfoAttributes(const CARequestInfo_t* requestInfo);
void responseHandler(const CAEndpoint_t* endpoint,
        const CAResponseInfo_t* responseInfo);
void requestHandler(const CAEndpoint_t* endpoint,
        const CARequestInfo_t* requestInfo);
void responseHandlerSecond(const CAEndpoint_t* endpoint,
        const CAResponseInfo_t* responseInfo);
void requestHandlerSecond(const CAEndpoint_t* endpoint,
        const CARequestInfo_t* requestInfo);
bool checkMessageReceiveCount(int total);

JNIEXPORT jint JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    LOGI("JNI_OnLoad");

    JNIEnv* env;
    if (JNI_OK != (*jvm)->GetEnv(jvm, (void**) &env, JNI_VERSION_1_6))
    {
        return -1;
    }
    g_jvm = jvm; /* cache the JavaVM pointer */

    CANativeJNISetJavaVM(g_jvm);

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *jvm, void *reserved)
{
    LOGI("JNI_OnUnload");

    JNIEnv* env;
    if (JNI_OK != (*jvm)->GetEnv(jvm, (void**) &env, JNI_VERSION_1_6))
    {
        return;
    }
    
    g_jvm = 0;
}

JNIEXPORT void JNICALL
Java_org_iotivity_CAJni_setNativeResponseListener(JNIEnv *env, jclass cls,
                                                                   jobject listener)
{
    LOGI("setNativeResponseListener");
    if (!env || !cls || !listener)
    {
        LOGI("Invalid input parameter");
        return;
    }

    g_responseListenerObject = (*env)->NewGlobalRef(env, listener);
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_readConfig(JNIEnv *env, jclass cls)
{
    FILE *fp;
    int sz;
    int i;
    int j;
    int cnt;
    char buffer[MAX_BUF_LEN];
    char output[MAX_BUF_LEN];
    char *word;
    char words[MAX_NUM_DEVICE][TOTAL_WORD][MAX_LINE_SIZE];
    char ips[MAX_BUF_LEN];
    m_availableNetwork = 0;

    LOGI("readConfig in");

    char networkTypesName[][20] = { "CA_IP", "CA_LE", "CA_EDR", "CA_TCP" };
    int numberOfShifts[] = { 0, 1, 2, 4 };

    fp = fopen(CONFIG_FILE, "r");
    if (fp == NULL)
    {
        LOGI("Failed to read the config.ini file");
        return false;
    }

    cnt = 0;

    while (fgets(buffer, MAX_BUF_LEN, fp))
    {
        word = strtok(buffer, " \n");
        strcpy(words[cnt][0], word);

        for (i = 1; i < TOTAL_WORD; i++)
        {
            word = strtok(NULL, " \n");

            if (!word)
            {
                break;
            }

            strcpy(words[cnt][i], word);
        }

        if (i < 3)
        {
            continue;
        }

        cnt++;
    }

    fclose(fp);
    m_remoteAddressCount = 0;

    /*for (i = 0; i < cnt; i++)
    {
        if (!strcmp(words[i][0], "IP"))
        {
            strcpy(m_remoteAddressSet[0], words[i][2]);
        }
        else if (!strcmp(words[i][0], "OTHER_IP_LIST"))
        {
            strcpy(ips, words[i][2]);
        }
        else if (!strcmp(words[i][0], "PORT"))
        {
            m_remotePort = atoi(words[i][2]);
        }
    }

    m_remoteAddressCount = 1;
    word = strtok(ips, " ");
    while(word != NULL)
    {
        strcpy(m_remoteAddressSet[m_remoteAddressCount++], word);
        word = strtok(NULL, " ");
    }*/

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < cnt; j++)
        {
            if (!strcmp(words[j][0], networkTypesName[i]))
            {
                int bit = atoi(words[j][2]);

                if (bit)
                {
                    m_availableNetwork |= (1 << numberOfShifts[i]);
                }

                break;
            }
        }
    }

    for (i = 0; i < m_remoteAddressCount; i++)
    {
        sprintf(output, "RememoteAddress: %s", m_remoteAddressSet[i]);
        LOGI("%s", output);
    }

    sprintf(output, "RemotePort: %d", m_remotePort);
    LOGI("%s", output);
    sprintf(output, "AvailableNetwork: %d", m_availableNetwork);
    LOGI("%s", output);

    if(m_availableNetwork == 0)
    {
        return false;
    }

    LOGI("readConfig out");

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_setContext(JNIEnv *env, jclass cls, jobject context, jobject activity)
{
    LOGI("RMInitialize");
    if (!env || !cls || !context)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    CANativeJNISetContext(env, context);
    CANativeSetActivity(env, activity);

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_setupTestCase(JNIEnv *env, jclass cls, jint inOutType, jint validationMethod,
        jint communicationType)
{
    LOGI("RMInitialize");
    if (!env || !cls)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    memset(&s_tcInfo, 0, sizeof(s_tcInfo));

    s_tcInfo.securityType = MESSAGE_NONSECURE;
    s_tcInfo.inOutType = inOutType;
    s_tcInfo.caMethod = CA_GET;
    s_tcInfo.messageType = CA_MSG_NONCONFIRM;
    s_tcInfo.validationMethod = validationMethod;
    s_tcInfo.communicationType = communicationType;
    s_tcInfo.numOptions = 0;

    int i;

    for(i = 0; i < m_remoteAddressCount; i++)
    {
        m_receiveCount[i] = 0;
    }

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_initialize(JNIEnv *env, jclass cls)
{
    LOGI("RMInitialize");
    if (!env || !cls)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    CAResult_t res = CAInitialize();

    if (CA_STATUS_OK != res)
    {
        LOGE("Could not Initialize");
        return false;
    }

    return true;
}

JNIEXPORT void JNICALL
Java_org_iotivity_CAJni_terminate(JNIEnv *env, jclass cls)
{
    LOGI("RMTerminate");
    if (!env || !cls)
    {
        LOGI("Invalid input parameter");
        return;
    }

    CATerminate();
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_selectNetwork(JNIEnv *env, jclass cls)
{
    return Java_org_iotivity_CAJni_selectCustomNetwork(env, cls, m_availableNetwork, CA_STATUS_OK);
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_selectCustomNetwork(JNIEnv *env, jclass cls, jlong networkType, jint expectedResult)
{
    LOGI("RMSelectNetwork Type : %lld", networkType);

    if (expectedResult != CASelectNetwork(networkType))
    {
        LOGE("Could not select network");
        return false;
    }

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_unSelectNetwork(JNIEnv *env, jclass cls)
{
    return Java_org_iotivity_CAJni_unSelectCustomNetwork(env, cls, m_availableNetwork, CA_STATUS_OK);
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_unSelectCustomNetwork(JNIEnv *env, jclass cls, jlong networkType, jint expectedResult)
{
    LOGI("RMUnSelectNetwork Type : %lld", networkType);

    if (expectedResult != CAUnSelectNetwork(networkType))
    {
        LOGE("Could not unselect network");
        return false;
    }
    
    return true;
}

void delete_global_references(JNIEnv *env, jclass cls)
{
    LOGI("delete_global_references");
    if (!env || !cls )
    {
        LOGI("Invalid input parameter");
        return;
    }

    (*env)->DeleteGlobalRef(env, g_responseListenerObject);
}

void getRandomString(size_t length)
{
    LOGI("[getRandomString] IN");

    int totalUpper = 26;
    int totalDigit = 10;
    int totalLetter = totalUpper + totalUpper;
    int totalAlphaNumeric = totalLetter + totalDigit;

    srand (time(NULL));

    for(int i = 0; i < length; i++)
    {
        int randomNumber = rand() % totalAlphaNumeric;

        if(randomNumber < 0)
        {
            randomNumber += totalAlphaNumeric;
        }

        if (randomNumber < totalUpper)
        {
            s_tcInfo.payload[i] = (randomNumber + 'A');
        }
        else if (randomNumber < totalLetter)
        {
            s_tcInfo.payload[i] = (randomNumber - totalUpper + 'a');
        }
        else
        {
            s_tcInfo.payload[i] = (randomNumber - totalLetter + '0');
        }
    }

    s_tcInfo.payload[length] = 0;

    LOGI("[getRandomString] OUT");    
}

const char* getResultName(CAResult_t result)
{
    if(result == 255)
    {
        return "CA_STATUS_FAILED";
    }

    const char * resultNames[] =
    {
        "CA_STATUS_OK",
        "CA_STATUS_INVALID_PARAM",
        "CA_ADAPTER_NOT_ENABLED",
        "CA_SERVER_STARTED_ALREADY",
        "CA_SERVER_NOT_STARTED",
        "CA_DESTINATION_NOT_REACHABLE",
        "CA_SOCKET_OPERATION_FAILED",
        "CA_SEND_FAILED",
        "CA_RECEVIE_FAILED",
        "CA_MEMORY_ALLOC_FAILED",
        "CA_REQUEST_TIMEOUT",
        "CA_DESTINATION_DISCONNECTED",
        "CA_NOT_SUPPORTED",
        "CA_STATUS_NOT_INITIALIZED",
        "CA_DTLS_AUTHENTICATION_FAILURE",
    };

    int index = (int)result;

    if(index >= 0 && index < 15)
    {
        return resultNames[index];
    }

    return "Unknown Result Type";
}


bool checkHeader(CAHeaderOption_t *options, uint32_t len)
{
    if(options)
    {
        LOGI("[checkHeader] option exist");
        uint32_t i;

        for (i = 0; i < len; i++)
        {
            LOGI("Option %d\n", i + 1);
            LOGI("ID : %d\n", options[i].optionID);
            LOGI("Data[%d]: %s\n", options[i].optionLength,
                    options[i].optionData);
        }

        LOGI("[checkHeader] vaue %d", len);
        LOGI("[checkHeader] vaue %d", s_tcInfo.numOptions);

        if(len == s_tcInfo.numOptions)
        {
            LOGI("[checkHeader] here");
            for (i = 0; i < len; i++)
            {
                if(options[i].optionID != s_tcInfo.options[i].optionID)
                {
                    LOGI("[checkHeader] here");
                    break;
                }
                if(options[i].optionLength != s_tcInfo.options[i].optionLength)
                {
                    LOGI("[checkHeader] here");
                    break;
                }
                if(strcmp(options[i].optionData, s_tcInfo.options[i].optionData))
                {
                    LOGI("[checkHeader] here");
                    break;
                }
            }

            LOGI("[checkHeader] len %d", len);

            if(i == len)
            {
                return true;
            }
        }
    }
    else if(len == s_tcInfo.numOptions)
    {
        LOGI("[checkHeader] option intentionally null");
        return true;
    }
    else
    {
        LOGI("[checkHeader] option unintentionally null");
        return false;
    }

    return false;
}

bool isSendAddressUnknown(const CAEndpoint_t* endpoint)
{
    LOGI("[isSendAddressUnknown] IN");
    int i;

    for(i = 0; i < m_remoteAddressCount; i++)
    {
        if(strcmp(m_remoteAddressSet[i], endpoint->addr))
        {
            LOGI("IP mismatch: [%s] [%s]", m_remoteAddressSet[i], endpoint->addr);
            return true;
        }
    }

    LOGI("[isSendAddressUnknown] OUT");

    return false;
}

bool showAndCheckEndpointAttributes(const CAEndpoint_t* endpoint, char* resourceUri)
{
    LOGI("[showAndCheckEndpointAttributes] IN");

    if (endpoint == NULL)
    {
        LOGI("CAEndpoint_t endpoint");
        return false;
    }

    LOGI("address : %s", endpoint->addr);
    LOGI("port : %d", endpoint->port);

    if(s_tcInfo.messageType == CA_MSG_CONFIRM || s_tcInfo.messageType == CA_MSG_NONCONFIRM)
    {
        if (resourceUri == NULL)
        {
            LOGI("Resource URI NULL");
            return false;
        }

        if (!strcmp(resourceUri, ""))
        {
            LOGI("Resource URI Empty");
            return false;
        }
    }

    LOGI("uri : %s", resourceUri);

    LOGI("[showAndCheckEndpointAttributes] OUT");

    return true;
}

bool showAndCheckResponseInfoAttributes(const CAResponseInfo_t* responseInfo)
{
    LOGI("[showAndCheckResponseInfoAttributes] IN");

    if (responseInfo == NULL)
    {
        LOGI("ResponseInfo NULL");
        return false;
    }

    LOGI("[showAndCheckResponseInfoAttributes] OUT");

    return true;
}

bool showAndCheckRequestInfoAttributes(const CARequestInfo_t* requestInfo)
{
    LOGI("[showAndCheckRequestInfoAttributes] IN");

    if (requestInfo == NULL)
    {
        LOGI("RequestInfo NULL");
        return false;
    }

    LOGI("[showAndCheckRequestInfoAttributes] OUT");

    return true;
}

int compareAndIncrement(const char* endpointResourceUri,
        const char *otherResourceUri, const char* payload, const char* remoteAddress)
{
    LOGI("[compareAndIncrement] IN");

    if (strstr(endpointResourceUri, otherResourceUri) != NULL)
    {
        LOGI("[compareAndIncrement] %s and %s\n", payload, s_tcInfo.payload);

        if (!strncmp(payload, s_tcInfo.payload, strlen(s_tcInfo.payload)))
        {
            LOGI("Valid message received");
            if (isAddressAlreadyKnown)
            {
                int index = getIndexOfRemoteAddress(remoteAddress);
                if(index >= 0)
                {
                    m_receiveCount[index]++;
                }
                else
                {
                    LOGI("Response Unknown Host");
                }
            }
            else
            {
                strcpy(m_remoteAddressSet[m_remoteAddressCount], remoteAddress);
                m_receiveCount[m_remoteAddressCount]++;
                m_remoteAddressCount ++;
            }
        }

        return 1;
    }

    LOGI("[compareAndIncrement] OUT");

    return 0;
}

void comparePrefixAndIncrement(const CAEndpoint_t* endpoint, const char* resourceUri, const char *q)
{
    LOGI("[comparePrefixAndIncrement] IN");

    LOGI("ResourceUri %s q %s", resourceUri, q);

    if (strstr(resourceUri, q) != NULL)
    {
        LOGI("%s: Valid Message Received", s_tcInfo.payload);

        if(isAddressAlreadyKnown)
        {
            int index = getIndexOfRemoteAddress(endpoint->addr);
            if(index >= 0)
            {
                m_receiveCount[index]++;
            }
            else
            {
                LOGI("Response Unknown Host");
            }
        }
        else
        {
            strcpy(m_remoteAddressSet[m_remoteAddressCount], endpoint->addr);
            m_receiveCount[m_remoteAddressCount]++;
            m_remoteAddressCount ++;
        }
    }
    else
    {
        LOGI("Substring not match");
    }

    LOGI("[comparePrefixAndIncrement] OUT");
}

bool checkRequestHandler(const CAEndpoint_t* endpoint,
        const CARequestInfo_t* requestInfo)
{
    LOGI("[checkRequestHandler] IN");

    if (!showAndCheckEndpointAttributes(endpoint, requestInfo->info.resourceUri))
    {
        return false;
    }

    if (isAddressAlreadyKnown && isSendAddressUnknown(endpoint))
    {
        return false;
    }

    if (!showAndCheckRequestInfoAttributes(requestInfo))
    {
        return false;
    }

    LOGI("[checkRequestHandler] OUT");

    return true;
}

bool checkResponseHandler(const CAEndpoint_t* endpoint,
        const CAResponseInfo_t* responseInfo)
{
    LOGI("[checkResponseHandler] IN");

    if (!showAndCheckEndpointAttributes(endpoint, responseInfo->info.resourceUri))
    {
        LOGI("showAndCheckEndpointAttributes false");
        return false;
    }

    if (isAddressAlreadyKnown && isSendAddressUnknown(endpoint))
    {
        LOGI("isSendAddressUnknown true");
        return false;
    }

    if (!showAndCheckResponseInfoAttributes(responseInfo))
    {
        LOGI("showAndCheckResponseInfoAttributes false");
        return false;
    }

    LOGI("[checkResponseHandler] OUT");

    return true;
}

void responseHandler(const CAEndpoint_t* endpoint,
        const CAResponseInfo_t* responseInfo)
{
    LOGI("[responseHandler] IN");

    if (!checkResponseHandler(endpoint, responseInfo))
    {
        return;
    }

    if(isAddressAlreadyKnown)
    {
        int index = getIndexOfRemoteAddress(endpoint->addr);
        if(index >= 0)
        {
            m_receiveCount[index]++;
        }
        else
        {
            LOGI("Response Unknown Host");
        }
    }
    else
    {
        strcpy(m_remoteAddressSet[m_remoteAddressCount], endpoint->addr);
        m_receiveCount[m_remoteAddressCount]++;
        m_remoteAddressCount ++;
    }

    LOGI("[responseHandler] OUT");
}

void requestHandler(const CAEndpoint_t* endpoint,
        const CARequestInfo_t* requestInfo)
{
    LOGI("[requestHandler] IN");

    if (!checkRequestHandler(endpoint, requestInfo))
    {
        return;
    }

    comparePrefixAndIncrement(endpoint, requestInfo->info.resourceUri, SEND_REQ_CONFIG);

    LOGI("[requestHandler] OUT");
}

void responseHandlerSecond(const CAEndpoint_t* endpoint,
        const CAResponseInfo_t* responseInfo)
{
    LOGI("[responseHandlerSecond] IN");

    if (!checkResponseHandler(endpoint, responseInfo))
    {
        return;
    }

    compareAndIncrement(responseInfo->info.resourceUri, SEND_REQ_CONFIG, responseInfo->info.payload, endpoint->addr);

    LOGI("[responseHandlerSecond] OUT");
}

void requestHandlerSecond(const CAEndpoint_t* endpoint,
        const CARequestInfo_t* requestInfo)
{
    LOGI("[requestHandlerSecond]");

    if (!showAndCheckEndpointAttributes(endpoint, requestInfo->info.resourceUri))
    {
        return;
    }

    if (isAddressAlreadyKnown && isSendAddressUnknown(endpoint))
    {
        return;
    }

    compareAndIncrement(requestInfo->info.resourceUri, SEND_REQ_CONFIG, requestInfo->info.payload, endpoint->addr);

    LOGI("[requestHandlerSecond] OUT");
}

int getIndexOfRemoteAddress(const char* address)
{
    int i;

    for(i = 0; i < m_remoteAddressCount; i++)
    {
        if(!strcmp(m_remoteAddressSet[i], address))
        {
            return i;
        }
    }

    return -1;
}

void errorHandler(const CAEndpoint_t *rep, const CAErrorInfo_t* errorInfo)
{
    LOGI("+++++++++++++++++++++++++++++++++++ErrorInfo+++++++++++++++++++++++++++++++++++");

    if (rep)
    {
        LOGI("Error Handler, Adapter Type : %d", rep->adapter);
        LOGI("Error Handler, Adapter Type : %s", rep->addr);
    }

    if (errorInfo)
    {
        const CAInfo_t *info = &errorInfo->info;
        LOGI("Error Handler, ErrorInfo :");
        LOGI("Error Handler result    : %d", errorInfo->result);
        LOGI("Error Handler token     : %s", info->token);
        LOGI("Error Handler messageId : %d", (uint16_t) info->messageId);
        LOGI("Error Handler resourceUri : %s", info->resourceUri);
        LOGI("Error Handler type      : %d", info->type);
        LOGI("Error Handler payload   : %s", info->payload);

        if(CA_ADAPTER_NOT_ENABLED == errorInfo->result)
        {
            LOGE("CA_ADAPTER_NOT_ENABLED, enable the adapter");
        }
        else if(CA_SEND_FAILED == errorInfo->result)
        {
            LOGE("CA_SEND_FAILED, unable to send the message, check parameters");
        }
        else if(CA_MEMORY_ALLOC_FAILED == errorInfo->result)
        {
            LOGE("CA_MEMORY_ALLOC_FAILED, insufficient memory");
        }
        else if(CA_SOCKET_OPERATION_FAILED == errorInfo->result)
        {
            LOGE("CA_SOCKET_OPERATION_FAILED, socket operation failed");
        }
        else if(CA_STATUS_FAILED == errorInfo->result)
        {
            LOGE("CA_STATUS_FAILED, message could not be delivered, internal error");
        }
    }
    LOGI("++++++++++++++++++++++++++++++++End of ErrorInfo++++++++++++++++++++++++++++++++");

    return;
}

bool checkMessageReceiveCount(int total)
{
    LOGI("[checkMessageReceiveCount] IN");

    int i;
    int j;
    bool flag;

    for(i = 0; i < MAX_ATTEMPT; i++)
    {
        sleep(1);
        flag = true;

        if (CA_STATUS_OK != CAHandleRequestResponse())
        {
            LOGE("Could not handle request and response");
            return false;
        }

        for (j = 0; i < m_remoteAddressCount; j++)
        {
            if(m_receiveCount[j] != total)
            {
                flag = false;
                break;
            }
        }
    }    

    LOGI("[checkMessageReceiveCount] OUT");

    return flag;
}

#ifdef __WITH_DTLS__

int32_t CAGetDtlsPskCredentials( CADtlsPskCredType_t type,
              const unsigned char *desc, size_t desc_len,
              unsigned char *result, size_t result_length)
{
    LOGI("CAGetDtlsPskCredentials IN");

    int32_t ret = -1;

    if (NULL == result)
    {
        return ret;
    }

    switch (type)
    {
        case CA_DTLS_PSK_HINT:
        case CA_DTLS_PSK_IDENTITY:

            if (result_length < sizeof(IDENTITY))
            {
                LOGE("ERROR : Wrong value for result for storing IDENTITY");
                return ret;
            }

            memcpy(result, IDENTITY, sizeof(IDENTITY));
            ret = sizeof(IDENTITY);
            break;

        case CA_DTLS_PSK_KEY:

            if ((desc_len == sizeof(IDENTITY)) &&
                memcmp(desc, IDENTITY, sizeof(IDENTITY)) == 0)
            {
                if (result_length < sizeof(RS_CLIENT_PSK))
                {
                    LOGE("ERROR : Wrong value for result for storing RS_CLIENT_PSK");
                    return ret;
                }

                memcpy(result, RS_CLIENT_PSK, sizeof(RS_CLIENT_PSK));
                ret = sizeof(RS_CLIENT_PSK);
            }
            break;

        default:

            LOGE("Wrong value passed for PSK_CRED_TYPE.");
            ret = -1;
    }

    LOGI("CAGetDtlsPskCredentials OUT\n");
    return ret;
}

#endif

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_destroyToken(JNIEnv *env, jclass cls)
{
    if(m_token != NULL)
    {
        CADestroyToken(m_token);
    }

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_registerHandler(JNIEnv *env, jclass cls, jint response, jint request)
{
    LOGI("RMRegisterHandler");
    if(!env || !cls)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    if(request == 1 && response == 1)
    {
        CARegisterHandler(requestHandler, responseHandler, errorHandler);
    }
    else if(request == 0 && response == 0)
    {
        CARegisterHandler(NULL, NULL, errorHandler);
    }
    else if(request == 0 && response == 1)
    {
        CARegisterHandler(NULL, responseHandler, errorHandler);
    }
    else if(request == 1 && response == 0)
    {
        CARegisterHandler(requestHandler, NULL, errorHandler);
    }
    else if(request == 1 && response == 2)
    {
        CARegisterHandler(requestHandler, responseHandler, errorHandler);
    }

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_sendRequest(JNIEnv *env, jclass cls, jint uriType,
                                                       jint payloadType, jint msgType,
                                                       jint methodType, jint numOfHeader,
                                                       jboolean isSecure)
{
    if (!env || !cls)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    isAddressAlreadyKnown = 1;

    CATransportFlags_t flags = isSecure ? CA_SECURE : CA_DEFAULT_FLAGS;

    CAEndpoint_t* endpoint = NULL;

    char* ip;

    if(uriType == URI_TYPE_NULL_ENDPOINT)
    {
        ip = NULL;
    }
    else if(uriType == URI_TYPE_ARBITRARY_ENDPOINT)
    {
        ip = ARBITRARY_ADDRESS;
    }
    else
    {
        ip = m_remoteAddressSet[0];
    }

    char* resourceUri;

    if(uriType == URI_TYPE_NULL)
    {
        resourceUri = NULL;
    }
    else if(uriType == URI_TYPE_EMPTY)
    {
        resourceUri = "";
    }
    else if(uriType == URI_TYPE_CONFIG)
    {
        resourceUri = SEND_REQ_CONFIG;
    }
    else
    {
        resourceUri = SEND_REQ_ACK;
    }

    CAResult_t res = CACreateEndpoint(flags, m_availableNetwork, (const char*)ip,
            m_remotePort, &endpoint);

    if (CA_STATUS_OK != res)
    {
        LOGE("Could not create remote end point");
        return false;
    }

    CAMessageType_t messageType = msgType;

    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        LOGE("token generate error!!");
        CADestroyEndpoint(endpoint);
        return false;
    }

    CAInfo_t requestData = { 0 };
    requestData.token = token;
    requestData.tokenLength = tokenLength;

    if(payloadType == PAYLOAD_TYPE_NULL)
    {
        requestData.payload = NULL;
        requestData.payloadSize = 0;
    }
    else if(payloadType == PAYLOAD_TYPE_EMPTY)
    {
        requestData.payload = (CAPayload_t) "";
        requestData.payloadSize = 0;
    }
    else if(payloadType == PAYLOAD_TYPE_NORMAL)
    {
        getRandomString(NORMAL_PAYLOAD_LENGTH);
        requestData.payload = (CAPayload_t) s_tcInfo.payload;
        requestData.payloadSize = strlen(s_tcInfo.payload);
    }
    else if(payloadType == PAYLOAD_TYPE_LARGE)
    {
        getRandomString(LARGE_PAYLOAD_LENGTH);
        requestData.payload = (CAPayload_t) s_tcInfo.payload;
        requestData.payloadSize = strlen(s_tcInfo.payload);
    }
    else if(payloadType == PAYLOAD_TYPE_CONFIG)
    {
        requestData.payload = (CAPayload_t) configPayload;
        requestData.payloadSize = strlen(configPayload);
    }

    requestData.type = messageType;
    requestData.resourceUri =  (CAURI_t) resourceUri;

    CARequestInfo_t requestInfo = { 0 };
    requestInfo.method = methodType;
    requestInfo.isMulticast = false;

    CAHeaderOption_t *headerOpt = NULL;

    if(numOfHeader > 0)
    {
        headerOpt = (CAHeaderOption_t*) calloc(1, sizeof(CAHeaderOption_t) * numOfHeader);
        if(numOfHeader >= 1)
        {
            headerOpt[0].optionID = firstOptionId;
            memcpy(headerOpt[0].optionData, firstOptionData, strlen(firstOptionData));
            headerOpt[0].optionLength = (uint16_t) strlen(firstOptionData);
        }
        if(numOfHeader >= 2)
        {
            headerOpt[1].optionID = secondOptionId;
            memcpy(headerOpt[1].optionData, secondOptionData, strlen(secondOptionData));
            headerOpt[1].optionLength = (uint16_t) strlen(secondOptionData);
        }

        requestData.numOptions = numOfHeader;
        requestData.options = headerOpt;
    }

    requestInfo.info = requestData;

    s_tcInfo.numOptions = numOfHeader;
    s_tcInfo.options = headerOpt;

    LOGI("validationMethod: %d", s_tcInfo.validationMethod);

    if (CA_STATUS_OK != CASendRequest(endpoint, &requestInfo))
    {
        LOGE("Could not send request");
    }

    if(endpoint != NULL)
    {
        CADestroyEndpoint(endpoint);
    }

    if(token != NULL)
    {
        CADestroyToken(token);
    }

    return true;
}

JNIEXPORT bool JNICALL 
Java_org_iotivity_CAJni_sendRequestToAll(JNIEnv *env, jclass cls, jint uriType,
                                                       jint payloadType, jint msgType,
                                                       jint methodType, jint numOfHeader)
{
    if (!env || !cls)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    LOGI("[sendRequestToAll] IN");

    char* resourceUri;

    if(uriType == URI_TYPE_NULL)
    {
        resourceUri = NULL;
    }
    else if(uriType == URI_TYPE_EMPTY)
    {
        resourceUri = "";
    }
    else if(uriType == URI_TYPE_CONFIG)
    {
        resourceUri = SEND_REQ_CONFIG;
    }
    else
    {
        resourceUri = SEND_REQ_ACK;
    }

    CAEndpoint_t *endpoint = NULL;
    CAResult_t res;

    if(uriType != URI_TYPE_NULL_ENDPOINT)
    {
        res = CACreateEndpoint(CA_DEFAULT_FLAGS, m_availableNetwork, "", 0, &endpoint);
        if (CA_STATUS_OK != res)
        {
            LOGE("create remote endpoint error, error code: %d", res);
            return false;
        }
    }
    else
    {
        return false;
    }

    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        LOGE("token generate error!!");
        if(uriType != URI_TYPE_NULL_ENDPOINT)
        {
            CADestroyEndpoint(endpoint);
        }
        return false;
    }

    LOGI("generated token %s", token);

    CAInfo_t caInfo = { 0 };
    caInfo.token = token;
    caInfo.tokenLength = tokenLength;
    
    if(payloadType == PAYLOAD_TYPE_NULL)
    {
        caInfo.payload = NULL;    
        caInfo.payloadSize = 0;
    }
    else if(payloadType == PAYLOAD_TYPE_EMPTY)
    {
        caInfo.payload = (CAPayload_t) "";    
        caInfo.payloadSize = 0;
    }
    else if(payloadType == PAYLOAD_TYPE_NORMAL)
    {
        getRandomString(NORMAL_PAYLOAD_LENGTH);
        caInfo.payload = (CAPayload_t) s_tcInfo.payload;
        caInfo.payloadSize = strlen(s_tcInfo.payload);
    }
    else if(payloadType == PAYLOAD_TYPE_LARGE)
    {
        getRandomString(LARGE_PAYLOAD_LENGTH);
        caInfo.payload = (CAPayload_t) s_tcInfo.payload;
        caInfo.payloadSize = strlen(s_tcInfo.payload);
    }

    caInfo.dataType = CA_REQUEST_DATA;
    caInfo.type = msgType;
    caInfo.resourceUri = (CAURI_t) resourceUri;

    CARequestInfo_t requestInfo = { 0 };
    requestInfo.method = methodType;
    requestInfo.isMulticast = true;

    CAHeaderOption_t *headerOpt = NULL;
    
    if(numOfHeader > 0)
    {    
        headerOpt = (CAHeaderOption_t*) calloc(1, sizeof(CAHeaderOption_t) * numOfHeader);
        if(numOfHeader >= 1)
        {
            headerOpt[0].optionID = firstOptionId;
            memcpy(headerOpt[0].optionData, firstOptionData, strlen(firstOptionData));
            headerOpt[0].optionLength = (uint16_t) strlen(firstOptionData);
        }
        if(numOfHeader >= 2)
        {
            headerOpt[1].optionID = secondOptionId;
            memcpy(headerOpt[1].optionData, secondOptionData, strlen(secondOptionData));
            headerOpt[1].optionLength = (uint16_t) strlen(secondOptionData);
        }
            
        caInfo.numOptions = numOfHeader;
        caInfo.options = headerOpt;

        s_tcInfo.options = headerOpt;
    }

    requestInfo.info = caInfo;
    
    s_tcInfo.numOptions = numOfHeader;

    bool flag = true;

    for (int messageCount = 0; messageCount < TOTAL_MESSAGE; messageCount++)
    {
        LOGE("Calling CASendRequestToAll request. Attempt No: %d",messageCount + 1);

        if (CA_STATUS_OK != CASendRequest(endpoint, &requestInfo))
        {
            LOGE("Could not send request");
            flag = false;
        }
    }

    if(endpoint != NULL)
    {
        CADestroyEndpoint(endpoint);
    }

    if(uriType != URI_TYPE_NULL_ENDPOINT && token != NULL)
    {
        CADestroyToken(token);
    }

    LOGI("[sendRequestToAll] OUT");

    return flag;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_sendResponse(JNIEnv *env, jclass cls,
                                                        jint uriType,
                                                       jint payloadType, jint msgType,
                                                       jint responseValue, jint numOfHeader)
{
    LOGI("RMSendResponse");
    if (!env || !cls)
    {
        LOGI("Invalid input parameter");
        return false;
    }
    
    CATransportFlags_t flags = CA_DEFAULT_FLAGS;

    CAEndpoint_t* endpoint = NULL;

    char* ip;

    if(uriType == URI_TYPE_NULL_ENDPOINT)
    {
        ip = NULL;
        return false;
    }
    else if(uriType == URI_TYPE_ARBITRARY_ENDPOINT)
    {
        ip = ARBITRARY_ADDRESS;
    }
    else
    {
        ip = m_remoteAddressSet[0];
    }

    char* resourceUri;

    if(uriType == URI_TYPE_NULL)
    {
        resourceUri = NULL;
    }
    else if(uriType == URI_TYPE_EMPTY)
    {
        resourceUri = "";
    }
    else
    {
        resourceUri = SEND_RES_ACK;
    }

    CAResult_t res = CACreateEndpoint(flags, m_availableNetwork, (const char*)ip,
            m_remotePort, &endpoint);

    if (CA_STATUS_OK != res)
    {
        LOGE("Could not create remote end point");
        return false;
    }    

    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        LOGE("token generate error!!");
        CADestroyEndpoint(endpoint);
        return false;
    }

    CAInfo_t caInfo = { 0 };
    caInfo.token = token;
    caInfo.tokenLength = tokenLength;
    
    if(payloadType == PAYLOAD_TYPE_NULL)
    {
        caInfo.payload = NULL;    
        caInfo.payloadSize = 0;
    }
    else if(payloadType == PAYLOAD_TYPE_EMPTY)
    {
        caInfo.payload = (CAPayload_t) "";    
        caInfo.payloadSize = 0;
    }
    else if(payloadType == PAYLOAD_TYPE_NORMAL)
    {
        getRandomString(NORMAL_PAYLOAD_LENGTH);
        caInfo.payload = (CAPayload_t) s_tcInfo.payload;
        caInfo.payloadSize = strlen(s_tcInfo.payload);
    }
    else if(payloadType == PAYLOAD_TYPE_LARGE)
    {
        getRandomString(LARGE_PAYLOAD_LENGTH);
        caInfo.payload = (CAPayload_t) s_tcInfo.payload;
        caInfo.payloadSize = strlen(s_tcInfo.payload);
    }
    
    caInfo.type = msgType;
    
    caInfo.resourceUri = (CAURI_t) resourceUri;
    caInfo.dataType = CA_RESPONSE_DATA;
    
    CAResponseInfo_t responseInfo = { 0 };
    responseInfo.result = responseValue;

    CAHeaderOption_t *headerOpt = NULL;
    
    if(numOfHeader > 0)
    {    
        headerOpt = (CAHeaderOption_t*) calloc(1, sizeof(CAHeaderOption_t) * numOfHeader);
        if(numOfHeader >= 1)
        {
            headerOpt[0].optionID = firstOptionId;
            memcpy(headerOpt[0].optionData, firstOptionData, strlen(firstOptionData));
            headerOpt[0].optionLength = (uint16_t) strlen(firstOptionData);
        }
        if(numOfHeader >= 2)
        {
            headerOpt[1].optionID = secondOptionId;
            memcpy(headerOpt[1].optionData, secondOptionData, strlen(secondOptionData));
            headerOpt[1].optionLength = (uint16_t) strlen(secondOptionData);
        }
            
        caInfo.numOptions = numOfHeader;
        caInfo.options = headerOpt;
    }
    
    LOGI("NumOfHeader: %d", numOfHeader);

    responseInfo.info = caInfo;

    s_tcInfo.numOptions = numOfHeader;
    s_tcInfo.options = headerOpt;

    bool flag = true;

    for (int messageCount = 0; messageCount < TOTAL_MESSAGE; messageCount++)
    {
        LOGE("Calling CASendResponse. Attempt No: %d", messageCount + 1);

        if (CA_STATUS_OK != CASendResponse(endpoint, &responseInfo))
        {
            LOGE("Could not send request");
            flag = false;
        }
    }

    if(endpoint != NULL)
    {
        CADestroyEndpoint(endpoint);
    }

    if(uriType != URI_TYPE_NULL_ENDPOINT && token != NULL)
    {
        CADestroyToken(token);
    }

    return flag;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_handleRequestResponse(JNIEnv *env, jclass cls)
{
    LOGI("RMHandleRequestResponse");
    if(!env || !cls)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    if (CA_STATUS_OK != CAHandleRequestResponse())
    {
        LOGE("Could not handle request and response");
        return false;
    }

    return true;
}

JNIEXPORT bool JNICALL 
Java_org_iotivity_CAJni_initNetwork(JNIEnv *env, jclass cls)
{
    CAResult_t result = CAInitialize();
    if (result != CA_STATUS_OK)
    {
        return false;
    }

    result = CASelectNetwork(m_availableNetwork);
    if (result != CA_STATUS_OK)
    {
        return false;
    }

    return true;
}

JNIEXPORT bool JNICALL 
Java_org_iotivity_CAJni_initServerNetwork(JNIEnv *env, jclass cls)
{
    CAResult_t result = CAInitialize();
    if (result != CA_STATUS_OK)
    {
        LOGE("CAInitialize Failed");
        return false;
    }

    result = CASelectNetwork(m_availableNetwork);
    if (result != CA_STATUS_OK)
    {
        LOGE("CASelectNetwork Failed");
        CATerminate();
        return false;
    }

    CARegisterHandler(requestHandler, responseHandler, errorHandler);

    result = CAStartListeningServer();

    if (result != CA_STATUS_OK)
    {
        LOGE("CAStartListeningServer Failed");
        CATerminate();
        return false;
    }

    return true;
}

JNIEXPORT bool JNICALL 
Java_org_iotivity_CAJni_initClientNetwork(JNIEnv *env, jclass cls)
{
    CAResult_t result = CAInitialize();
    if (result != CA_STATUS_OK)
    {
        return false;
    }

    result = CASelectNetwork(m_availableNetwork);
    if (result != CA_STATUS_OK)
    {
        CATerminate();
        return false;
    }

    CARegisterHandler(requestHandler, responseHandler, errorHandler);

    result = CAStartDiscoveryServer();
    LOGI("CAStartDiscoveryServer result: %d", result);
    if (result != CA_STATUS_OK)
    {
        CATerminate();
        return false;
    }

    return true;
}

JNIEXPORT bool JNICALL 
Java_org_iotivity_CAJni_checkReceiveCount(JNIEnv *env, jclass cls, jint total)
{
    if (!checkMessageReceiveCount(total))
    {
        return false;
    }

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_sendConfigurationRequest(JNIEnv *env, jclass cls, jint taskType, 
                                                        jint msgType, jint method)
{
    LOGI("[sendConfigurationRequest] IN");

    configPayload[0] = 0;

    switch (taskType)
    {
        case SEND_MESSAGE:
            strcat(configPayload, "0");
            break;

        case SELECT_NETWORK:
            strcat(configPayload, "1");
            break;

        case UNSELECT_NETWORK:
            strcat(configPayload, "2");
            break;

        case STOP_SIM:
            strcat(configPayload, "3");
            break;

        default:
            break;
    }

    switch (msgType)
    {
        case MESSAGE_RESPONSE:
            strcat(configPayload, "0");
            break;

        case MESSAGE_REQUEST:
            strcat(configPayload, "1");
            break;

        default:
            break;
    }

    char buffer[5];
    sprintf(buffer, "%04d", 1);
    strcat(configPayload, buffer);

    sprintf(buffer, "%04d", 0);
    strcat(configPayload, buffer);

    getRandomString(NORMAL_PAYLOAD_LENGTH);

    sprintf(buffer, "%04d", strlen(s_tcInfo.payload));
    strcat(configPayload, buffer);

    strcat(configPayload, s_tcInfo.payload);
    s_tcInfo.caMethod = method;

    LOGI("[sendConfigurationRequest] OUT");

    return Java_org_iotivity_CAJni_sendRequest
            (env, cls, URI_TYPE_CONFIG, PAYLOAD_TYPE_CONFIG, CA_MSG_NONCONFIRM, method, HEADER_NONE, false);
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_startListeningServer(JNIEnv *env, jclass cls, jint expectedResult)
{
    LOGI("[RMStartListeningServer] IN");

    if (!env || !cls)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    CAResult_t res = CAStartListeningServer();

    if (expectedResult != res)
    {
        LOGE("CAStartListeningServer Returned: %s Expected: %s", getResultName(res), getResultName(expectedResult));
        return false;
    }

    LOGI("[RMStopListeningServer] OUT");

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_stopListeningServer(JNIEnv *env, jclass cls, jint expectedResult)
{
    LOGI("[RMStopListeningServer] IN");

    if (!env || !cls)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    CAResult_t res = CAStopListeningServer();

    if (expectedResult != res)
    {
        LOGE("CAStopListeningServer Returned: %s Expected: %s", getResultName(res), getResultName(expectedResult));
        return false;
    }

    LOGI("[RMStopListeningServer] OUT");

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_startDiscoveryServer(JNIEnv *env, jclass cls, jint expectedResult)
{
    LOGI("[RMStartDiscoveryServer] IN");

    if (!env || !cls)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    CAResult_t res = CAStartDiscoveryServer();

    if (expectedResult != res)
    {
        LOGE("CAStopDiscoveryServer Returned: %s Expected: %s", getResultName(res), getResultName(expectedResult));
        return false;
    }

    LOGI("[RMStartDiscoveryServer] OUT");

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_generateToken(JNIEnv *env, jclass cls,
                                                         jint tokenType, jint tokenLength, jint expectedResult)
{
    m_token = NULL;
    CAResult_t res;
    CAToken_t **tokenAddress;

    if (TOKEN_TYPE_NULL == tokenType)
    {
        tokenAddress = NULL;
    }
    else if (TOKEN_TYPE_INVALID_REFERENCE == tokenType)
    {
        tokenAddress = m_token;
    }
    else
    {
        tokenAddress = &m_token;
    }

    res = CAGenerateToken(tokenAddress, tokenLength);
    if (res != expectedResult)
    {
        LOGE("CAGenerateToken, Returned: %s, Expected: %s", getResultName(res), getResultName(expectedResult));
        return false;
    }

    LOGI("[generateCustomToken] OUT");

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_getNetworkInfomation(JNIEnv *env, jclass cls, jint endpointType, jint sizeType, jint expectedResult)
{
    CAResult_t res;
    CAEndpoint_t **endpointAddress = NULL;
    uint32_t *sizeAddress = NULL;

    if (NETWORKINFO_ENDPOINT_VALID == endpointType)
    {
        CAEndpoint_t *endpoint = NULL;
        endpointAddress = &endpoint;
    }

    if (NETWORKINFO_SIZE_VALID == sizeType)
    {
        uint32_t tempSize = 0;
        sizeAddress = &tempSize;
    }
    
    res = CAGetNetworkInformation(endpointAddress, sizeAddress);
    if (expectedResult != res)
    {
        LOGE("CAGetNetworkInformation, Returned:  %s, Expected: %s", getResultName(res), getResultName(expectedResult));
        return false;
    }

    LOGI("[getNetworkInfomation] OUT");

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_createEndpoint(JNIEnv *env, jclass cls, jint targetArg, jint expectedResult)
{
    CAResult_t res;
    CATransportAdapter_t network = m_availableNetwork;
    CATransportFlags_t transportFlag = CA_DEFAULT_FLAGS;
    int port = ARBITRARY_PORT;
    char* ip = ARBITRARY_ADDRESS;
    m_endpoint = NULL;
    CAEndpoint_t **endpointAddress = &m_endpoint;

    switch (targetArg)
    {
        case ENDPOINT_VALID:
            break;

        case ENDPOINT_INVALID_FLAG:
            transportFlag = -1;
            break;

        case ENDPOINT_INVALID_ADAPTER:
            network = NETWORK_TYPE_LOBV_INVALID;
            break;

        case ENDPOINT_INVALID_ADDRESS:
            ip = NULL;
            break;

        case ENDPOINT_INVALID_PORT:
            port = EDNPOINT_LOBV_PORT;
            break;

        case ENDPOINT_INVALID_ENDPOINT:
            endpointAddress = NULL; 
            break;

        case ENDPOINT_ESV_ADDRESS:
            ip = "";
            break;

        case ENDPOINT_URV_ENDPOINT:
            endpointAddress = NULL; 
            break;

        default:
            LOGE("createEndpointWithExpectedResult: target argument mismatched with condition checking.");
            return false;
    }

    res = CACreateEndpoint(transportFlag, network, ip, port, endpointAddress);
    if (res != expectedResult)
    {
        LOGE("CACreateEndpoint, Returned: %s, Expected: %s", getResultName(res), getResultName(expectedResult));        
        return false;
    }

    LOGI("[createEndpoint] OUT");

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_destroyEndpoint(JNIEnv *env, jclass cls, jint endpointType)
{
    if (endpointType == ENDPOINT_VALID)
    {
        CADestroyEndpoint(m_endpoint);
    }
    else if (endpointType == ENDPOINT_NULL)
    {
        CADestroyEndpoint(NULL);
    }
    else
    {
        LOGE("Invalid endpointType");
        return false;        
    }

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_CAJni_establishConnectionWithServer(JNIEnv *env, jclass cls)
{
    LOGI("[establishConnectionWithServer] IN");

    int i;

    for(i = 0; i < m_remoteAddressCount; i++)
    {
        m_receiveCount[i] = 0;
    }

    if (!Java_org_iotivity_CAJni_initClientNetwork(env, cls))
    {
        LOGE("initClientNetwork Failed");
        return false;
    }

    isAddressAlreadyKnown = 0;

    if (!Java_org_iotivity_CAJni_sendRequestToAll(
        env, cls, URI_TYPE_NORMAL, PAYLOAD_TYPE_NORMAL, CA_MSG_NONCONFIRM, CA_GET, HEADER_NONE))
    {
        LOGE("sendRequestToAll Failed");
        return false;
    }
    
    if (!checkMessageReceiveCount(TOTAL_MESSAGE))
    {
        LOGE("checkMessageReceiveCount Failed");
        return false;
    }

    isAddressAlreadyKnown = 1;

    LOGI("[establishConnectionWithServer] OUT");

    return true;
}
