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
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <android/log.h>

#include "cainterface.h"
#include "cacommon.h"
#include "caadapterutils.h"

#include "org_iotivity_ca_service_RMInterface.h"

#define  LOG_TAG   "JNI_INTERFACE_SAMPLE"
#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define IDENTITY     ("1111111111111111")
#define RS_CLIENT_PSK   ("AAAAAAAAAAAAAAAA")

#define NETWORK_TYPE_LOBV_INVALID			-1
#define NETWORK_TYPE_UOBV_INVALID			4294967296
#define NETWORK_TYPE_CA_DEFAULT_ADAPTER		2
#define NETWORK_TYPE_CA_ALL_ADAPTERS		3

#define Get_NETWORK_INFO_WITHOUT_INIT		0
#define Get_NETWORK_INFO_UFV_SIZE			1
#define Get_NETWORK_INFO_UFV_INFO			2

#define CREATE_ENDPOINT_INVALID_FLAG		0
#define CREATE_ENDPOINT_INVALID_ADAPTER		1
#define CREATE_ENDPOINT_INVALID_ADDRESS		2
#define CREATE_ENDPOINT_INVALID_PORT		3
#define CREATE_ENDPOINT_INVALID_ENDPOINT	4
#define CREATE_ENDPOINT_ESV_ADDRESS			5
#define CREATE_ENDPOINT_URV_ENDPOINT		6

#define TOKEN_TYPE_INVALID_REFERENCE		0
#define TOKEN_TYPE_NULL						1

#define TOKEN_LENGTH_LBV_VALID				1
#define TOKEN_LENGTH_LOBV_INVALID			-1
#define TOKEN_LENGTH_UOBV_INVALID			9

const int URI_TYPE_NULL_ENDPOINT = -3;
const int URI_TYPE_WRONG_ENDPOINT = -2;
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

const int INVALID_NETWORK = 0;
const int TOTAL_MESSAGE = 1;
const int TRY_COUNT = 3;
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

static const int NORMAL_PAYLOAD_LENGTH = 10;
static const int LARGE_PAYLOAD_LENGTH = 1500;

const char SEND_REQ_ACK[] = "SendReqAck";
const char SEND_RES_ACK[] = "SendResAck";
const char SEND_REQ_CONFIG[] = "Configure";

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
jint m_availableNetwork;

CAEndpoint_t *m_endpoint = NULL;
CAToken_t m_token = NULL;

char m_remoteAddress[50];
char m_remoteAddressSet[10][50];
int m_remoteAddressCount;
int m_remoteAddressIndex;
int m_receiveCount[10];
int m_remotePort;

char WRONG_ADDRESS[50] = "78:05:71:C3:EA:03";
int WRONG_PORT = 1000;

int ENDPOINT_PORT = 1000;
int EDNPOINT_LOBV_PORT = -1;
char ENDPOINT_IP[] = "100.100.100.100";

bool getIndexOfRemoteAddress(char* address);
static void error_handler(const CAEndpoint_t *object, const CAErrorInfo_t* errorInfo);
static void delete_global_references(JNIEnv *env, jobject obj);
void getRandomString(size_t length);
const char* getResultName(CAResult_t result);
static void callback(char *subject, char *receivedData);
static CAResult_t get_network_type(uint32_t selectedNetwork);
bool checkHeader(CAHeaderOption_t *options, uint32_t len);
bool initializeClientNetwork();
bool sendRequestToAllAddress(int uriType, int payloadType, int msgType, int methodType, int numOfHeader);
int compareAndIncrement(const char* endpointResourceUri,
        const char *otherResourceUri, const char* payload);
void comparePrefixAndIncrement(const CAEndpoint_t* endpoint, const char* resourceUri, const char *q);
bool isHostUnknown(const CAEndpoint_t* endpoint);
bool showAndCheckRemoteEndpointAttributes(const CAEndpoint_t* endpoint, char* resourceUri);
bool showAndCheckResponseInfoAttributes(const CAResponseInfo_t* responseInfo);
bool showAndCheckRequestInfoAttributes(const CARequestInfo_t* requestInfo);
void handleServerRequest(const CAEndpoint_t* endpoint, const CARequestInfo_t* requestInfo);
void handleServerResponse(const CAEndpoint_t* endpoint,
        const CAResponseInfo_t* responseInfo);
void handleClientRequest(const CAEndpoint_t* endpoint,
        const CARequestInfo_t* requestInfo);
void handleClientResponse(const CAEndpoint_t* endpoint,
        const CAResponseInfo_t* responseInfo);
void handleClientResponseSecond(const CAEndpoint_t* endpoint,
        const CAResponseInfo_t* responseInfo);
void handleServerRequestSecond(const CAEndpoint_t* endpoint,
        const CARequestInfo_t* requestInfo);
bool checkRequestHandler(const CAEndpoint_t* endpoint,
        const CARequestInfo_t* requestInfo);
bool checkResponseHandler(const CAEndpoint_t* endpoint,
        const CAResponseInfo_t* responseInfo);
bool checkMessageReceiveCount(int total);

bool getIndexOfRemoteAddress(char* address)
{
    int i;
    bool flag = false;

    for(i = 0; i < m_remoteAddressCount; i++)
    {
        if(!strcmp(m_remoteAddressSet[i], address))
        {
            m_remoteAddressIndex = i;
            flag = true;
            break;
        }
    }

    return flag;
}

void error_handler(const CAEndpoint_t *rep, const CAErrorInfo_t* errorInfo)
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

void callback(char *subject, char *receivedData)
{
	LOGI("callback is called");

    bool isAttached = false;
    JNIEnv* env;

    if (!g_responseListenerObject)
    {
        LOGE("g_responseListenerObject is NULL, cannot have callback");
        return;
    }

    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        LOGI("Could not get JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            LOGE("AttachCurrentThread has failed");
            return;
        }
        isAttached = true;
    }
    LOGI("getting object class ");

    jclass cls = (*env)->GetObjectClass(env, g_responseListenerObject);
    if (!cls)
    {
        LOGE("could not get class");
        goto detach_thread;
    }

    jmethodID mid = (*env)->GetMethodID(env, cls, "OnResponseReceived",
                                        "(Ljava/lang/String;Ljava/lang/String;)V");
    if (!mid)
    {
        LOGE("could not get Method ID");
        goto detach_thread;
    }

    jstring jsubject = (*env)->NewStringUTF(env, (char*) subject);
    if (!jsubject)
    {
        LOGE("NewStringUTF failed");
        goto detach_thread;
    }

    jstring jreceivedData = (*env)->NewStringUTF(env, (char*) receivedData);
    if (!jreceivedData)
    {
        LOGE("NewStringUTF failed");
        goto detach_thread;
    }

    (*env)->CallVoidMethod(env, g_responseListenerObject, mid, jsubject, jreceivedData);

detach_thread :
    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
        LOGI("DetachCurrentThread");
    }
}

void delete_global_references(JNIEnv *env, jobject obj)
{
    LOGI("delete_global_references");
    if (!env || !obj )
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

bool initializeClientNetwork()
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

    CARegisterHandler(handleClientRequest, handleClientResponse, error_handler);

    result = CAStartDiscoveryServer();
    LOGI("CAStartDiscoveryServer result: %d", result);
    if (result != CA_STATUS_OK)
    {
        CATerminate();
        return false;
    }
    return true;
}

bool sendRequestToAllAddress(int uriType, int payloadType, int msgType, int methodType, int numOfHeader)
{
    LOGI("[sendRequestToAllAddress] IN");

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
		res = CACreateEndpoint(CA_DEFAULT_FLAGS, m_availableNetwork, NULL, 0, &endpoint);
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

    LOGI("[sendRequestToAllAddress] OUT");

	return flag;
}

bool isHostUnknown(const CAEndpoint_t* endpoint)
{
    LOGI("[isHostUnknown] IN");

    if(strcmp(m_remoteAddress, endpoint->addr))
    {
    	LOGI("IP mismatch: [%s] [%s]", m_remoteAddress, endpoint->addr);
    	callback("IP:", "mismatch");
    	callback("endpoint->addr: ", endpoint->addr);
    	callback("m_remoteAddress: ", endpoint->addr);
    	return true;
    }

    LOGI("[isHostUnknown] OUT");

    return false;
}

bool showAndCheckRemoteEndpointAttributes(const CAEndpoint_t* endpoint, char* resourceUri)
{
    LOGI("[showAndCheckRemoteEndpointAttributes] IN");

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

    LOGI("[showAndCheckRemoteEndpointAttributes] OUT");

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
        const char *otherResourceUri, const char* payload)
{
    LOGI("[compareAndIncrement] IN");

    if (strstr(endpointResourceUri, otherResourceUri) != NULL)
    {
        LOGI("[compareAndIncrement] %s and %s\n", payload, s_tcInfo.payload);

        if (!strncmp(payload, s_tcInfo.payload, strlen(s_tcInfo.payload)))
        {
            LOGI("Valid message received");

            if(getIndexOfRemoteAddress(m_remoteAddress))
            {
                m_receiveCount[m_remoteAddressIndex]++;
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

        if(getIndexOfRemoteAddress(endpoint->addr))
        {
            m_receiveCount[m_remoteAddressIndex]++;
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

    if (!showAndCheckRemoteEndpointAttributes(endpoint, requestInfo->info.resourceUri))
    {
        return false;
    }

    if (isHostUnknown(endpoint))
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

    if (!showAndCheckRemoteEndpointAttributes(endpoint, responseInfo->info.resourceUri))
    {
    	LOGI("showAndCheckRemoteEndpointAttributes false");
        return false;
    }

    if (isHostUnknown(endpoint))
    {
    	LOGI("isHostUnknown true");
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

void handleServerRequest(const CAEndpoint_t* endpoint,
        const CARequestInfo_t* requestInfo)
{
    LOGI("[handleServerRequest] IN");
    LOGI("validationMethod: %d", s_tcInfo.validationMethod);

    if (!checkRequestHandler(endpoint, requestInfo))
    {
        return;
    }

    if(s_tcInfo.messageType == CA_MSG_RESET)
    {
        if(getIndexOfRemoteAddress(endpoint->addr))
        {
            m_receiveCount[m_remoteAddressIndex]++;
        }
        return;
    }

    if (s_tcInfo.validationMethod == MESSAGE_URI && s_tcInfo.communicationType == MESSAGE_MULTICAST)
    {
        if (s_tcInfo.inOutType == MESSAGE_INCOMING)
        {
            comparePrefixAndIncrement(endpoint, requestInfo->info.resourceUri, s_tcInfo.payload);
            return;
        }
        else if (s_tcInfo.inOutType == MESSAGE_OUTGOING)
        {
            comparePrefixAndIncrement(endpoint, requestInfo->info.resourceUri, s_tcInfo.payload);
            return;
        }
    }

    if (s_tcInfo.inOutType == MESSAGE_OUTGOING && s_tcInfo.validationMethod == MESSAGE_URI
            && s_tcInfo.communicationType == MESSAGE_UNICAST)
    {
        comparePrefixAndIncrement(endpoint, requestInfo->info.resourceUri, SEND_RES_ACK);
        return;
    }

    if (s_tcInfo.validationMethod == MESSAGE_PAYLOAD)
    {
        strcpy(m_remoteAddress, endpoint->addr);
        int ret = compareAndIncrement(requestInfo->info.resourceUri, SEND_RES_ACK,
                requestInfo->info.payload);

        if (!ret)
        {
            if (requestInfo->method == s_tcInfo.caMethod)
            {
                ret = compareAndIncrement(requestInfo->info.resourceUri, SEND_REQ_CONFIG,
                        requestInfo->info.payload);
            }
            else
            {
                LOGI("Invalid Method for SEND_REQ_CONFIG");
            }
        }

        if (!ret)
        {
            LOGI("Data mismatch!");
        }
    }

    if (s_tcInfo.validationMethod == MESSAGE_HEADER)
    {
        LOGI("[handleServerRequest] MESSAGE_HEADER");

        if(checkHeader(requestInfo->info.options, requestInfo->info.numOptions))
        {
            comparePrefixAndIncrement(endpoint, requestInfo->info.resourceUri, SEND_RES_ACK);
        }
    }

    LOGI("[handleServerRequest] OUT");
}

void handleServerResponse(const CAEndpoint_t* endpoint,
        const CAResponseInfo_t* responseInfo)
{
    LOGI("[handleServerResponse] IN");

    if (!checkResponseHandler(endpoint, responseInfo))
    {
        return;
    }

    if(getIndexOfRemoteAddress(endpoint->addr))
    {
        m_receiveCount[m_remoteAddressIndex]++;
    }

    LOGI("[handleServerResponse] OUT");
}

void handleClientRequest(const CAEndpoint_t* endpoint,
        const CARequestInfo_t* requestInfo)
{
    LOGI("[handleClientRequest] IN");

    if (!checkRequestHandler(endpoint, requestInfo))
    {
        return;
    }

    comparePrefixAndIncrement(endpoint, requestInfo->info.resourceUri, SEND_REQ_CONFIG);

    LOGI("[handleClientRequest] OUT");
}

void handleClientResponse(const CAEndpoint_t* endpoint,
        const CAResponseInfo_t* responseInfo)
{
    LOGI("[handleClientResponse] IN");

    if (!checkResponseHandler(endpoint, responseInfo))
    {
        return;
    }

    if(s_tcInfo.messageType == CA_MSG_RESET)
    {
        if(getIndexOfRemoteAddress(endpoint->addr))
        {
            m_receiveCount[m_remoteAddressIndex]++;
        }
        return;
    }

    if (s_tcInfo.inOutType == MESSAGE_OUTGOING && s_tcInfo.validationMethod == MESSAGE_URI)
    {
        comparePrefixAndIncrement(endpoint, responseInfo->info.resourceUri, SEND_REQ_ACK);

        return;
    }

    if (!showAndCheckResponseInfoAttributes(responseInfo))
    {
        return;
    }

    if (s_tcInfo.validationMethod == MESSAGE_PAYLOAD)
    {
        LOGI("Payload %s\n", responseInfo->info.payload);
        
        strcpy(m_remoteAddress, endpoint->addr);

        int ret = compareAndIncrement(responseInfo->info.resourceUri, SEND_REQ_ACK,
                responseInfo->info.payload);

        if (!ret)
        {
            ret = compareAndIncrement(responseInfo->info.resourceUri, SEND_REQ_CONFIG,
                    responseInfo->info.payload);
        }

        if (!ret)
        {
            LOGI("Data mismatch!");
        }
    }

    if (s_tcInfo.validationMethod == MESSAGE_HEADER)
    {
        LOGI("[handleClientResponse] MESSAGE_HEADER");

        if(checkHeader(responseInfo->info.options, responseInfo->info.numOptions))
        {
            comparePrefixAndIncrement(endpoint, responseInfo->info.resourceUri, SEND_REQ_ACK);
        }
    }

    LOGI("[handleClientResponse] OUT");
}

void handleClientResponseSecond(const CAEndpoint_t* endpoint,
        const CAResponseInfo_t* responseInfo)
{
    LOGI("[handleClientResponseSecond] IN");

    if (!checkResponseHandler(endpoint, responseInfo))
    {
        return;
    }

    compareAndIncrement(responseInfo->info.resourceUri, SEND_REQ_CONFIG, responseInfo->info.payload);

    LOGI("[handleClientResponseSecond] OUT");
}

void handleServerRequestSecond(const CAEndpoint_t* endpoint,
        const CARequestInfo_t* requestInfo)
{
    LOGI("[handleServerRequestSecond]");

    if (!showAndCheckRemoteEndpointAttributes(endpoint, requestInfo->info.resourceUri))
    {
        return;
    }

    if (isHostUnknown(endpoint))
    {
        return;
    }

    compareAndIncrement(requestInfo->info.resourceUri, SEND_REQ_CONFIG, requestInfo->info.payload);

    LOGI("[handleServerRequestSecond] OUT");
}

bool checkMessageReceiveCount(int total)
{
    LOGI("[checkMessageReceiveCount] IN");	

    int i;
    bool flag;

    for(i  = 0; i < MAX_ATTEMPT; i++)
    {
        sleep(1);
        flag = true;

        if (CA_STATUS_OK != CAHandleRequestResponse())
        {
            LOGE("Could not handle request and response");
            return false;
        }

        for (i = 0; i < m_remoteAddressCount; i++)
        {
            if(getIndexOfRemoteAddress(m_remoteAddressSet[i]))
            {
                if(m_receiveCount[m_remoteAddressIndex] != total)
                {
                    flag = false;
                    break;
                }
            }
            else
            {
                flag = false;
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
Java_org_iotivity_service_ca_test_helper_RMInterface_setNativeResponseListener(JNIEnv *env, jobject obj,
                                                                   jobject listener)
{
    LOGI("setNativeResponseListener");
    if (!env || !obj || !listener)
    {
        LOGI("Invalid input parameter");
        return;
    }

    g_responseListenerObject = (*env)->NewGlobalRef(env, listener);
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_setContext(JNIEnv *env, jobject obj, jobject context, jobject activity)
{
    LOGI("RMInitialize");
    if (!env || !obj || !context)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    CANativeJNISetContext(env, context);
    CANativeSetActivity(env, activity);

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_setupTestCase(JNIEnv *env, jobject obj, jint inOutType, jint validationMethod,
        jint communicationType)
{
    LOGI("RMInitialize");
    if (!env || !obj)
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

    m_remoteAddressIndex = -1;

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_initialize(JNIEnv *env, jobject obj)
{
    LOGI("RMInitialize");
    if (!env || !obj)
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
Java_org_iotivity_service_ca_test_helper_RMInterface_terminate(JNIEnv *env, jobject obj)
{
    LOGI("RMTerminate");
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return;
    }

    CATerminate();
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_startListeningServer(JNIEnv *env, jobject obj)
{
    LOGI("RMStartListeningServer");
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    if (CA_STATUS_OK != CAStartListeningServer())
    {
        LOGE("Could not start Listening server");
        return false;
    }

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_startDiscoveryServer(JNIEnv *env, jobject obj)
{
    LOGI("RMStartDiscoveryServer");
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    if (CA_STATUS_OK != CAStartDiscoveryServer())
    {
        LOGE("Could not start discovery server");
        return false;
    }
    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_createEndpoint(JNIEnv *env, jobject obj)
{
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    CATransportFlags_t flags = CA_DEFAULT_FLAGS;

    CAResult_t res = CACreateEndpoint(flags, m_availableNetwork, (const char*)(ENDPOINT_IP), ENDPOINT_PORT, &m_endpoint);
    if (CA_STATUS_OK != res)
    {
        LOGE("Could not create remote end point");
        return false;
    }
    
    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_destroyEndpoint(JNIEnv *env, jobject obj)
{
    if(m_endpoint != NULL)
    {
        CADestroyEndpoint(m_endpoint);
    }

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_generateToken(JNIEnv *env, jobject obj)
{
    CAResult_t res = CAGenerateToken(&m_token, CA_MAX_TOKEN_LEN);
    if ((CA_STATUS_OK != res) || (!m_token))
    {
        return false;
    }
    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_destroyToken(JNIEnv *env, jobject obj)
{
    if(m_token != NULL)
    {
        CADestroyToken(m_token);
    }

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_registerServerHandler(JNIEnv *env, jobject obj)
{
    LOGI("RMRegisterHandler");
    if(!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    CARegisterHandler(handleServerRequest, handleServerResponse, error_handler);
	
    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_registerClientHandler(JNIEnv *env, jobject obj)
{
    LOGI("RMRegisterHandler");
    if(!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    CARegisterHandler(handleClientRequest, handleClientResponse, error_handler);
	
    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_registerCustomServerHandler(JNIEnv *env, jobject obj, jint response, jint request)
{
    LOGI("RMRegisterHandler");
    if(!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    if(request == 1 && response == 1)
    {
        CARegisterHandler(handleServerRequest, handleServerResponse, error_handler);
    }
    else if(request == 0 && response == 0)
    {
        CARegisterHandler(NULL, NULL, error_handler);
    }
    else if(request == 0 && response == 1)
    {
        CARegisterHandler(NULL, handleServerResponse, error_handler);
    }
    else if(request == 1 && response == 0)
    {
        CARegisterHandler(handleServerRequest, NULL, error_handler);
    }
    else if(request == 2 && response == 1)
    {
        CARegisterHandler(handleServerRequestSecond, handleServerResponse, error_handler);
    }
	
    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_registerCustomClientHandler(JNIEnv *env, jobject obj, jint response, jint request)
{
    LOGI("RMRegisterHandler");
    if(!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    if(request == 1 && response == 1)
    {
        CARegisterHandler(handleClientRequest, handleClientResponse, error_handler);
    }
    else if(request == 0 && response == 0)
    {
        CARegisterHandler(NULL, NULL, error_handler);
    }
    else if(request == 0 && response == 1)
    {
        CARegisterHandler(NULL, handleClientResponse, error_handler);
    }
    else if(request == 1 && response == 0)
    {
        CARegisterHandler(handleClientRequest, NULL, error_handler);
    }
    else if(request == 1 && response == 2)
    {
        CARegisterHandler(handleClientRequest, handleClientResponseSecond, error_handler);
    }

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_sendRequest(JNIEnv *env, jobject obj, jint uriType,
                                                       jint payloadType, jint msgType,
                                                       jint methodType, jint numOfHeader,
                                                       jboolean isSecure)
{
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    CATransportFlags_t flags = isSecure ? CA_SECURE : CA_DEFAULT_FLAGS;

    CAEndpoint_t* endpoint = NULL;

    char* ip;

	if(uriType == URI_TYPE_NULL_ENDPOINT)
	{
		ip = NULL;
	}
	else if(uriType == URI_TYPE_WRONG_ENDPOINT)
	{
		ip = WRONG_ADDRESS;
	}
	else
	{
		ip = m_remoteAddress;
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
Java_org_iotivity_service_ca_test_helper_RMInterface_sendRequestToAll(JNIEnv *env, jobject obj, jint uriType,
                                                       jint payloadType, jint msgType,
                                                       jint methodType, jint numOfHeader)
{
	if (!env || !obj)
	{
		LOGI("Invalid input parameter");
		return false;
	}

    if (!sendRequestToAllAddress(uriType, payloadType, msgType, methodType, numOfHeader))
    {
        return false;
    }

	return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_sendResponse(JNIEnv *env, jobject obj,
                                                        jint uriType,
                                                       jint payloadType, jint msgType,
                                                       jint responseValue, jint numOfHeader)
{
    LOGI("RMSendResponse");
	if (!env || !obj)
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
	else if(uriType == URI_TYPE_WRONG_ENDPOINT)
	{
		ip = WRONG_ADDRESS;
	}
	else
	{
		ip = m_remoteAddress;
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
Java_org_iotivity_service_ca_test_helper_RMInterface_selectNetwork(JNIEnv *env, jobject obj)
{
    LOGI("RMSelectNetwork Type : %d", m_availableNetwork);
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    if (CA_STATUS_OK != CASelectNetwork(m_availableNetwork))
    {
        LOGE("Could not select network");
        return false;
    }

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_selectCustomNetwork(JNIEnv *env, jobject obj,
                                                         jint networkType)
{
    LOGI("RMSelectNetwork Type : %d", networkType);
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    if (CA_STATUS_OK != CASelectNetwork(networkType))
    {
        LOGE("Could not select network");
        return false;
    }

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_unSelectNetwork(JNIEnv *env, jobject obj)
{
    LOGI("RMUnSelectNetwork Type : %d", m_availableNetwork);
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    if (CA_STATUS_OK != CAUnSelectNetwork(m_availableNetwork))
    {
        LOGE("Could not unselect network");
        return false;
    }
    
    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_unSelectCustomNetwork(JNIEnv *env, jobject obj,
                                                           jint networkType)
{
    LOGI("RMUnSelectNetwork Type : %d", networkType);
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    if (CA_STATUS_OK != CAUnSelectNetwork(networkType))
    {
        LOGE("Could not unselect network");
        return false;
    }
    
    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_getNetworkInfomation(JNIEnv *env, jobject obj, jboolean positive)
{
    LOGI("RMGetNetworkInfomation");
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    if(positive)
    {
        CAEndpoint_t *tempInfo = NULL;
        uint32_t tempSize = 0;
        CAResult_t res = CAGetNetworkInformation(&tempInfo, &tempSize);
        if (CA_STATUS_OK != res)
        {
            LOGE("Could not start get network information");
            free(tempInfo);
            return false;
        }
    }
    else
    {        
        CAResult_t res = CAGetNetworkInformation(NULL, NULL);
        if (CA_STATUS_OK == res)
        {
            LOGE("Could not start get network information");
            return false;
        }
    }

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_handleRequestResponse(JNIEnv *env, jobject obj)
{
    LOGI("RMHandleRequestResponse");
    if(!env || !obj)
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
Java_org_iotivity_service_ca_test_helper_RMInterface_initNetwork(JNIEnv *env, jobject obj)
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
Java_org_iotivity_service_ca_test_helper_RMInterface_initServerNetwork(JNIEnv *env, jobject obj)
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

    CARegisterHandler(handleServerRequest, handleServerResponse, error_handler);

    result = CAStartListeningServer();

    if (result != CA_STATUS_OK)
    {
        CATerminate();
        return false;
    }

    return true;
}

JNIEXPORT bool JNICALL 
Java_org_iotivity_service_ca_test_helper_RMInterface_initClientNetwork(JNIEnv *env, jobject obj)
{
    if (!initializeClientNetwork)
    {
        return false;
    }

    return true;
}

JNIEXPORT bool JNICALL 
Java_org_iotivity_service_ca_test_helper_RMInterface_initServerWithoutNetwork(JNIEnv *env, jobject obj)
{
    CAResult_t result = CAInitialize();
    if (result != CA_STATUS_OK)
    {
        return false;
    }

    CARegisterHandler(handleServerRequest, handleServerResponse, error_handler);

    result = CAStartListeningServer();

    if (result != CA_STATUS_OK)
    {
        CATerminate();
        return false;
    }

    return true;
}

JNIEXPORT bool JNICALL 
Java_org_iotivity_service_ca_test_helper_RMInterface_initClientWithoutNetwork(JNIEnv *env, jobject obj)
{
    CAResult_t result = CAInitialize();
    if (result != CA_STATUS_OK)
    {
        return false;
    }

    CARegisterHandler(handleClientRequest, handleClientResponse, error_handler);

    result = CAStartDiscoveryServer();

    if (result != CA_STATUS_OK)
    {
        CATerminate();
        return false;
    }

    return true;
}

JNIEXPORT bool JNICALL 
Java_org_iotivity_service_ca_test_helper_RMInterface_checkReceiveCount(JNIEnv *env, jobject obj, jint total)
{
    if (!checkMessageReceiveCount(total))
    {
        return false;
    }

    return true;
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_setNetwork
  (JNIEnv *env, jobject obj, jint network)
{
    m_availableNetwork = network;
}

JNIEXPORT void JNICALL 
Java_org_iotivity_service_ca_test_helper_RMInterface_setRemoteIp(JNIEnv *env, jobject obj, jstring ip)
{
    const char* temp = (*env)->GetStringUTFChars(env, ip, NULL);
    LOGI("Temp Remote IP: %s", temp);
    callback("Temp Remote IP: ", temp);
    strcpy(m_remoteAddress, temp);
    strcpy(m_remoteAddressSet[0], temp);
    LOGI("Remote IP: %s", m_remoteAddress);
    callback("Remote IP: ", m_remoteAddress);

    m_remoteAddressCount = 1;
}

JNIEXPORT void JNICALL 
Java_org_iotivity_service_ca_test_helper_RMInterface_setRemoteAddressSet(JNIEnv *env, jobject obj, jobjectArray addressSet)
{
    int i;
    m_remoteAddressCount = (*env)->GetArrayLength(env, addressSet);

    if(m_remoteAddressCount != 0)
    {
        for (i = 0; i < m_remoteAddressCount; i++)
        {
            jstring string = (jstring) (*env)->GetObjectArrayElement(env, addressSet, i);
            const char* temp = (*env)->GetStringUTFChars(env, string, NULL);
	        LOGI("Temp Remote IP: %s", temp);
	        callback("Temp Remote IP: ", temp);
	        strcpy(m_remoteAddressSet[i], temp);
	        LOGI("Remote IP: %s", m_remoteAddressSet[i]);
	        callback("Remote IP: ", m_remoteAddressSet[i]);
        }
    }
    else
    {
        LOGI("No address found");
    }
}


JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_sendConfigurationRequest(JNIEnv *env, jobject obj, jint taskType, 
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

    return Java_org_iotivity_service_ca_test_helper_RMInterface_sendRequest
    		(env, obj, URI_TYPE_CONFIG, PAYLOAD_TYPE_CONFIG, CA_MSG_NONCONFIRM, method, HEADER_NONE, false);
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_setDtls(JNIEnv *env, jobject obj)
{
    CAResult_t result = CARegisterDTLSCredentialsHandler(CAGetDtlsPskCredentials);

    if (result != CA_STATUS_OK)
    {
    	LOGI("CARegisterDTLSCredentialsHandler FAILED");
        return false;
    }

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_startListeningServerCustom(JNIEnv *env, jobject obj,
                                                         jint expectedResult)
{
    LOGI("[RMStartListeningServerCustom] IN");
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

	CAResult_t res = CAStartListeningServer();

	if (expectedResult != res)
	{
		LOGE("CAStartListeningServer	Returned: %s	Expected: %s", getResultName(res), getResultName(expectedResult));
		return false;
	}

    LOGI("[RMStartListeningServerCustom] OUT");

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_stopListeningServer(JNIEnv *env, jobject obj, jint expectedResult)
{
    LOGI("[RMStopListeningServer] IN");

    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    CAResult_t res = CAStopListeningServer();

	if (expectedResult != res)
	{
		LOGE("CAStopListeningServer	Returned: %s Expected: %s", getResultName(res), getResultName(expectedResult));
		return false;
	}

    LOGI("[RMStopListeningServer] OUT");

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_generateCustomToken(JNIEnv *env, jobject obj,
                                                         jint targetARG, jint expectedResult)
{
    LOGI("[generateCustomToken] IN");

    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    m_token = NULL;
    bool flag = true;
    CAResult_t res;
    CAToken_t *invalidToken = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    switch (targetARG)
    {
        case TOKEN_TYPE_INVALID_REFERENCE:
            flag = false;
            res = CAGenerateToken(invalidToken, tokenLength);
            break;

        case TOKEN_LENGTH_LBV_VALID:
            tokenLength = TOKEN_LENGTH_LBV_VALID;
            break;

        case TOKEN_LENGTH_LOBV_INVALID:
            tokenLength = TOKEN_LENGTH_LOBV_INVALID;
            break;

        case TOKEN_LENGTH_UOBV_INVALID:
            tokenLength = TOKEN_LENGTH_UOBV_INVALID;
            break;

        default:
            LOGE("generateCustomToken:	target argument mismatched with condition checking.");
    }

    if (flag)
    {
        res = CAGenerateToken(&m_token, tokenLength);
    }

    if (m_token != NULL)
    {
        CADestroyToken(m_token);
    }

	if (res != expectedResult)
    {
        LOGE("CAGenerateToken, Returned:  %s, Expected: %s", getResultName(res), getResultName(expectedResult));
        return false;
    }

    LOGI("[generateCustomToken] OUT");

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_selectNetworkWithExpectedResult(JNIEnv *env, jobject obj,
                                                           jlong networkType, jint expectedResult)
{
    LOGI("selectNetworkWithExpectedResult IN");
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

	CAResult_t res;
    CATransportAdapter_t network;

    switch (networkType)
    {
        case NETWORK_TYPE_CA_ALL_ADAPTERS:
            network = CA_ALL_ADAPTERS;
            break;

        case NETWORK_TYPE_CA_DEFAULT_ADAPTER:
            network = CA_DEFAULT_ADAPTER;
            break;

        case -1:
            network = -1;
            break;

        case 4294967296:
            network = 4294967296;
            break;

        default:
            LOGE("selectNetworkWithExpectedResult:	networktype param mismatched with condition checking.");
            network = networkType;
    }

    res = CASelectNetwork(network);

    if (res != expectedResult)
    {
        LOGE("CASelectNetwork, Returned:  %s, Expected: %s", getResultName(res), getResultName(expectedResult));
        return false;
    }

    LOGI("selectNetworkWithExpectedResult OUT");

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_unSelectNetworkWithExpectedResult(JNIEnv *env, jobject obj,
                                                           jlong networkType, jint expectedResult)
{
    LOGI("[unSelectNetworkWithExpectedResult] IN");
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

	CAResult_t res;
    CATransportAdapter_t network;

    switch (networkType)
    {
        case NETWORK_TYPE_CA_ALL_ADAPTERS:
            network = CA_ALL_ADAPTERS;
            break;

        case NETWORK_TYPE_CA_DEFAULT_ADAPTER:
            network = CA_DEFAULT_ADAPTER;
            break;

        case 4294967296:
            network = 4294967296;
            break;

        default:
            LOGE("unSelectNetworkWithExpectedResult:	networktype param mismatched with condition checking.");
            network = networkType;
    }

    res = CAUnSelectNetwork(network);

    if (res != expectedResult)
    {
        LOGE("CAUnSelectNetwork, Returned:  %s, Expected: %s", getResultName(res), getResultName(expectedResult));
        return false;
    }

    LOGI("unSelectNetworkWithExpectedResult OUT");

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_startDiscoveryWithExpectedResult(JNIEnv *env, jobject obj,
                                                           jint expectedResult)
{
    LOGI("startDiscoveryWithExpectedResult IN");
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

	CAResult_t res;

	res = CAStartDiscoveryServer();
	if (expectedResult != res)
    {
        LOGE("startDiscoveryWithExpectedResult: expectedResult param mismatched with condition checking.");
        return false;
    }

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_getNetworkInfomationWithExpectedResult(JNIEnv *env, jobject obj, jint targetARG, jint expectedResult)
{
    LOGI("[getNetworkInfomationWithExpectedResult] IN:");
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    CAResult_t res;
    CAEndpoint_t *tempInfo = NULL;
    CAEndpoint_t **unformatedTempInfo = NULL;
    uint32_t tempSize = 0;
    uint32_t *unformatedTempSize = 0;

    switch (targetARG)
    {
        case Get_NETWORK_INFO_WITHOUT_INIT:
            res = CAGetNetworkInformation(&tempInfo, &tempSize);
            break;

        case Get_NETWORK_INFO_UFV_SIZE:
            res = CAGetNetworkInformation(&tempInfo, unformatedTempSize);
            break;

        case Get_NETWORK_INFO_UFV_INFO:
            res = CAGetNetworkInformation(unformatedTempInfo, &tempSize);
            break;

        default:
            LOGE("getNetworkInfomationWithExpectedResult:	target argument mismatched with condition checking.");
    }

    if (res != expectedResult)
    {
        LOGE("CAGetNetworkInformation, Returned:  %s, Expected: %s", getResultName(res), getResultName(expectedResult));
        return false;
    }

    LOGI("[getNetworkInfomationWithExpectedResult] OUT");

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_createEndpointWithExpectedResult(JNIEnv *env, jobject obj, jint targetARG, jint expectedResult)
{
    LOGI("[createEndpointWithExpectedResult] IN");
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    CATransportAdapter_t network = m_availableNetwork;
    CATransportFlags_t transportFlag = CA_DEFAULT_FLAGS;
    int port = 6298;
    char* ip = (char *) "107.109.214.164";
	m_endpoint = NULL;
    bool flag = true;
    
    CAResult_t res;

    switch (targetARG)
    {
        case CREATE_ENDPOINT_INVALID_FLAG:
            transportFlag = -1;
            break;

        case CREATE_ENDPOINT_INVALID_ADAPTER:
            network = NETWORK_TYPE_LOBV_INVALID;
            break;

        case CREATE_ENDPOINT_INVALID_ADDRESS:
            flag = false;
            res = CACreateEndpoint(transportFlag, network, NULL, port, &m_endpoint);
            break;

        case CREATE_ENDPOINT_INVALID_PORT:
            port = EDNPOINT_LOBV_PORT;
            break;

        case CREATE_ENDPOINT_INVALID_ENDPOINT:
            flag = false;
            res = CACreateEndpoint(transportFlag, network, (const char*)ip, port, NULL);
            break;

        case CREATE_ENDPOINT_ESV_ADDRESS:
            ip = "";
            break;

        case CREATE_ENDPOINT_URV_ENDPOINT:
            flag = false;
            CAEndpoint_t** unformatedEndpoint = NULL;    
            res = CACreateEndpoint(transportFlag,  network, (const char*)ip, port, unformatedEndpoint);
            if (unformatedEndpoint != NULL)
            {
                CADestroyEndpoint(*unformatedEndpoint);
            }
            break;

        default:
            LOGE("createEndpointWithExpectedResult:	target argument mismatched with condition checking.");
    }

    if (flag)
    {
        res = CACreateEndpoint(transportFlag, network, (const char*)ip, port, &m_endpoint);
    }

	if (res != expectedResult)
	{
		LOGE("CACreateEndpoint, Returned:  %s, Expected: %s", getResultName(res), getResultName(expectedResult));
		if(m_endpoint != NULL)
		{
			CADestroyEndpoint(m_endpoint);
		}
		return false;
	}

    LOGI("[createEndpointWithExpectedResult] OUT");

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_destroyEndpointInvalidEndpoint(JNIEnv *env, jobject obj)
{
    LOGI("destroyEndpoint_invalidEndpoint IN:");
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return false;
    }

    CADestroyEndpoint(NULL);

    return true;
}

JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_generateTokenAndDestroyToken(JNIEnv *env, jobject obj)
{
	LOGI("generateTokenAndDestroyToken IN:");
	if (!env || !obj)
	{
		LOGI("Invalid input parameter");
		return false;
	}

	CAResult_t res = CAGenerateToken(&m_token, CA_MAX_TOKEN_LEN);
	if ((CA_STATUS_OK != res) || (!m_token))
	{
		LOGE("Failed to generate Token");
		return false;
	}

	if (strlen(m_token) != CA_MAX_TOKEN_LEN)
	{
		LOGE("Generated token Length mismatched");
		return false;
	}

	CADestroyToken(m_token);

	return true;
}
JNIEXPORT bool JNICALL
Java_org_iotivity_service_ca_test_helper_RMInterface_establishConnectionWithServer(JNIEnv *env, jobject obj)
{
	LOGI("[establishConnectionWithServer] IN");

	if (!env || !obj)
	{
		LOGI("Invalid input parameter");
		return false;
	}

    int i;

    for(i = 0; i < m_remoteAddressCount; i++)
    {
        m_receiveCount[i] = 0;
    }

    m_remoteAddressIndex = -1;

    if (!initializeClientNetwork())
    {
        LOGE("initializeClientNetwork Failed");
        return false;
    }

    if (!sendRequestToAllAddress(URI_TYPE_NORMAL, PAYLOAD_TYPE_NORMAL, CA_MSG_NONCONFIRM, CA_GET, HEADER_NONE))
    {
        LOGE("sendRequestToAllAddress Failed");
        return false;
    }
    
    if (!checkMessageReceiveCount(TOTAL_MESSAGE))
    {
        LOGE("checkMessageReceiveCount Failed");
        return false;
    }

	LOGI("[establishConnectionWithServer] OUT");

    return true;
}
