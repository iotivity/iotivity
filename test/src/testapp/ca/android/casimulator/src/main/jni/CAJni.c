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
#include <android/log.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cainterface.h"
#include "cacommon.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "cacommon.h"
#include "cainterface.h"
//#include "casimulator.h"

#define IDENTITY     ("1111111111111111")
#define RS_CLIENT_PSK   ("AAAAAAAAAAAAAAAA")

#define NORMAL_PAYLOAD_LENGTH 10
#define LARGE_PAYLOAD_LENGTH 1500
#define SIM_REQ_CONFIG "Configure"
#define SIM_REQ_ACK "SendReqAck"
#define SIM_REQ_QUERY "SendReqQry"
#define SIM_RES_ACK "SendResAck"

#define MAX_BUF_LEN 2048
#define MAX_OPT_LEN 16
#define MAX_SLEEP_TIME 1
#define CH_ZERO '0'

#include "org_iotivity_CAJni.h"

#define  LOG_TAG   "JNI_INTERFACE_SIMULATOR"
#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static jobject g_responseListenerObject = NULL;
static JavaVM *g_jvm;
CATransportAdapter_t g_selectedNwType = CA_ADAPTER_IP;

typedef enum
{
    SEND_MESSAGE = 0, SELECT_NETWORK, UNSELECT_NETWORK, STOP_SIM, TRANSFER_INFO
} SimulatorTask;

typedef enum
{
    MESSAGE_RESPONSE = 0,
    MESSAGE_REQUEST
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
    uint16_t messageId;
} TestConfiguration;

int g_selectedNetwork = 0;
bool g_simulatorProcess = true;
int g_messageId = -1;
bool g_firstMessage = true;
int mode;
int server = 0;
int client = 1;

int initialize(void);

#ifdef __WITH_DTLS__
int registerDtlsHandler(void);
#endif

int selectNetwork(int network);

int startServer(void);

int startClient(void);

int handleMessage(void);

void processRequestResponse(void);

void requestHandler(const CAEndpoint_t* endPoint, const CARequestInfo_t* requestInfo);

void responseHandler(const CAEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo);

void error_handler(const CAEndpoint_t *rep, const CAErrorInfo_t* errorInfo);

int registerMessageHandler(void);


JNIEXPORT void JNICALL
Java_org_iotivity_CAJni_setNativeResponseListener(JNIEnv *env, jobject obj,
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

JNIEXPORT jint JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    LOGI("JNI_OnLoad");
    (void)reserved;

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
    (void)reserved;

    JNIEnv* env;
    if (JNI_OK != (*jvm)->GetEnv(jvm, (void**) &env, JNI_VERSION_1_6))
    {
        return;
    }
    g_jvm = 0;
    return;
}

void delete_global_references(JNIEnv *env, jobject obj)
{
    LOGI("delete_global_references");
    if (!env || !obj )
    {
        LOGI("Invalid input parameter");
        return;
    }

//    (*env)->DeleteGlobalRef(env, g_responseListenerObject);
}

JNIEXPORT void JNICALL
Java_org_iotivity_CAJni_initialize(JNIEnv *env, jobject obj, jobject context, jobject activity)
{
    LOGI("RMInitialize");
    if (!env || !obj || !context)
    {
        LOGI("Invalid input parameter");
        return;
    }

    //Currently set context for Android Platform
    CANativeJNISetContext(env, context);
    CANativeSetActivity(env, activity);
//    CANativeJNISetContext(env, context);

    if(initialize() != -1)
    {
        return;
    }
#ifdef __WITH_DTLS__
    if(registerDtlsHandler() != -1)
    {
        return;
    }
#endif
}

JNIEXPORT void JNICALL
Java_org_iotivity_CAJni_terminate(JNIEnv *env, jobject obj)
{
    LOGI("RMTerminate");
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return;
    }

//    CATerminate();

    delete_global_references(env, obj);
}

JNIEXPORT void JNICALL
Java_org_iotivity_CAJni_startListeningServer(JNIEnv *env, jobject obj)
{
    LOGI("RMStartListeningServer");
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return;
    }

    if(startServer() != -1)
    {
        return;
    }
}

JNIEXPORT void JNICALL
Java_org_iotivity_CAJni_startDiscoveryServer(JNIEnv *env, jobject obj)
{
    LOGI("RMStartDiscoveryServer");
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return;
    }

    if(startClient() != -1)
    {
        return;
    }
}

JNIEXPORT void JNICALL
Java_org_iotivity_CAJni_registerHandler(JNIEnv *env, jobject obj)
{
    LOGI("RMRegisterHandler");
    if(!env || !obj)
    {
        LOGI("Invalid input parameter");
        return;
    }

    if(registerMessageHandler() != -1)
    {
        return;
    }
}

JNIEXPORT void JNICALL
Java_org_iotivity_CAJni_selectNetwork(JNIEnv *env, jobject obj,
                                                         jint networkType)
{
    LOGI("RMSelectNetwork Type : %d", networkType);
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return;
    }

    if(selectNetwork(networkType) != -1)
    {
        return;
    }
}

JNIEXPORT void JNICALL
Java_org_iotivity_CAJni_unSelectNetwork(JNIEnv *env, jobject obj,
                                                           jint networkType)
{
    LOGI("RMUnSelectNetwork Type : %d", networkType);
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return;
    }

    unSelectNetwork(networkType);
}

JNIEXPORT void JNICALL
Java_org_iotivity_CAJni_getNetworkInfomation(JNIEnv *env, jobject obj)
{
    LOGI("RMGetNetworkInfomation");
    if (!env || !obj)
    {
        LOGI("Invalid input parameter");
        return;
    }

    CAEndpoint_t *tempInfo = NULL;
    uint32_t tempSize = 0;

    CAResult_t res = CAGetNetworkInformation(&tempInfo, &tempSize);
    if (CA_STATUS_OK != res)
    {
        LOGE("Could not start get network information");
        free(tempInfo);
        return;
    }

    LOGI("################## Selected Network Information #######################");
    callback("######## Selected Network Information", "#######");
    LOGI("Network info total size is %d", tempSize);

    uint32_t index;
    for (index = 0; index < tempSize; index++)
    {
        uint32_t len = strlen(tempInfo[index].addr);

        char *remoteAddress = (char *)malloc(sizeof (char) * (len + 1));
        if (NULL == remoteAddress)
        {
            LOGE("remoteAddress Out of memory");
            return CA_MEMORY_ALLOC_FAILED;
        }

        memcpy(remoteAddress, tempInfo[index].addr, len + 1);

        if (NULL != g_responseListenerObject)
        {
            char networkInfo[1024];
            LOGI("Type: %d", tempInfo[index].adapter);
            sprintf(networkInfo, "%d",tempInfo[index].adapter);
            callback("Type :", networkInfo);
            if (CA_ADAPTER_IP == tempInfo[index].adapter)
            {
                LOGI("Port: %d", tempInfo[index].port);
                sprintf(networkInfo, "%d",tempInfo[index].port);
                callback("Port: ", networkInfo);
            }
            LOGI("Secured: %d", (tempInfo[index].flags & CA_SECURE));
            LOGI("Address: %s", remoteAddress);
            callback("Address: ", remoteAddress);
            free(remoteAddress);
        }
    }

    // free
    free(tempInfo);

    LOGI("##############################################################");
}

JNIEXPORT void JNICALL
Java_org_iotivity_CAJni_handleRequestResponse(JNIEnv *env, jobject obj)
{
    LOGI("RMHandleRequestResponse");
    if(!env || !obj)
    {
        LOGI("Invalid input parameter");
        return;
    }

    if (CA_STATUS_OK != CAHandleRequestResponse())
    {
        LOGE("Could not handle request and response");
    }
}

CAResult_t get_network_type(uint32_t selectedNetwork)
{

    uint32_t number = selectedNetwork;

    if (!(number & 0xf))
    {
        return CA_NOT_SUPPORTED;
    }
    if (number & CA_ADAPTER_IP)
    {
        g_selectedNwType = CA_ADAPTER_IP;
        return CA_STATUS_OK;
    }
    if (number & CA_ADAPTER_RFCOMM_BTEDR)
    {
        g_selectedNwType = CA_ADAPTER_RFCOMM_BTEDR;
        return CA_STATUS_OK;
    }
    if (number & CA_ADAPTER_GATT_BTLE)
    {
        g_selectedNwType = CA_ADAPTER_GATT_BTLE;
        return CA_STATUS_OK;
    }

    return CA_NOT_SUPPORTED;
}

void callback(char *subject, char *receivedData)
{
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


int initialize()
{
    CAResult_t result = CAInitialize();

   if (result != CA_STATUS_OK)
   {
       LOGI("CAInitialize failed");
       return 0;
   }

   return 1;
}

#ifdef __WITH_DTLS__
int32_t getDtlsPskCredentials( CADtlsPskCredType_t type, const unsigned char *desc,
        size_t desc_len, unsigned char *result, size_t result_length)
{
    LOGI("CAGetDtlsPskCredentials IN\n");

    int32_t ret = -1;

    if (NULL == result)
    {
        return ret;
    }

    switch (type)
    {
        case CA_DTLS_PSK_HINT:
        case CA_DTLS_PSK_IDENTITY:
            LOGI("CAGetDtlsPskCredentials CA_DTLS_PSK_IDENTITY\n");
            if (result_length < sizeof(IDENTITY))
            {
                LOGI("ERROR : Wrong value for result for storing IDENTITY");
                return ret;
            }

            memcpy(result, IDENTITY, sizeof(IDENTITY));
            ret = sizeof(IDENTITY);
            break;

        case CA_DTLS_PSK_KEY:
            LOGI("CAGetDtlsPskCredentials CA_DTLS_PSK_KEY\n");
            if ((desc_len == sizeof(IDENTITY)) &&
                memcmp(desc, IDENTITY, sizeof(IDENTITY)) == 0)
            {
                if (result_length < sizeof(RS_CLIENT_PSK))
                {
                    LOGI("ERROR : Wrong value for result for storing RS_CLIENT_PSK");
                    return ret;
                }

                memcpy(result, RS_CLIENT_PSK, sizeof(RS_CLIENT_PSK));
                ret = sizeof(RS_CLIENT_PSK);
            }
            break;

        default:

            LOGI("Wrong value passed for PSK_CRED_TYPE.");
            ret = -1;
    }

    LOGI("CAGetDtlsPskCredentials OUT\n");
    return ret;
}

int registerDtlsHandler()
{

    CAResult_t result = CARegisterDTLSCredentialsHandler(getDtlsPskCredentials);
    if (result != CA_STATUS_OK)
    {
        LOGI("CARegisterDTLSCredentialsHandler FAILED");
        return;
    }

    return 1;
}
#endif

int selectNetwork(int network)
{
    CAResult_t result = CASelectNetwork(network);

    if (result != CA_STATUS_OK)
    {
        LOGI("CASelectNetwork Failed");
        return 0;
    }

    return 1;
}

int startServer()
{
    CAResult_t result = CAStartListeningServer();
    if (result != CA_STATUS_OK)
    {
        LOGI("CAStartListeningServer Failed");
        return 0;
    }

    mode = server;

    return 1;
}

int startClient()
{
    CAResult_t result = CAStartDiscoveryServer();
    if (result != CA_STATUS_OK)
    {
        LOGI("CAStartDiscoveryServer");
        return 0;
    }

    mode = client;

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

bool returnRequest(const CAEndpoint_t* endPoint, char* resourceUri, char* payload,
        CAMessageType_t type, CAMethod_t method, CAToken_t token, uint8_t tokenLength, CAHeaderOption_t *options, uint8_t numOptions)
{
    LOGI("[returnRequest] IN");
    callback("returnRequest: ", "IN");

    CAInfo_t requestData = { 0 };
    requestData.token = token;
    requestData.tokenLength = tokenLength;
    requestData.payload = payload;
    if(payload != NULL)
    {
        requestData.payloadSize = strlen(payload);
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

    CARequestInfo_t requestInfo = { 0 };
    requestInfo.method = method;
    requestInfo.info = requestData;
    requestInfo.isMulticast = false;

    callback("CASendRequest: ", "calling ...");

    CAResult_t res = CASendRequest(endPoint, &requestInfo);
    if (res != CA_STATUS_OK)
    {
        LOGI("send request error\n");
        callback("CASendRequest: ", "failed");
        return false;
    }
    else
    {
        LOGI("send request success\n");
        callback("CASendRequest: ", "success");
    }

    LOGI("[returnRequest] OUT");
    callback("returnRequest: ", "OUT");

    return true;
}

bool returnResponse(const CAEndpoint_t* endPoint, char* resourceUri, char* payload,
        CAMessageType_t type, CAResponseResult_t responseCode, uint16_t messageId, CAToken_t token,
        uint8_t tokenLength, CAHeaderOption_t *options, uint8_t numOptions)
{
    LOGI("[returnResponse] IN");
    callback("returnResponse: ", "in");

    CAInfo_t responseData = { 0 };

    responseData.payload = payload;

    if(payload != NULL)
    {
        responseData.payloadSize = strlen(payload);
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

    CAResponseInfo_t responseInfo = { 0 };
    responseInfo.result = responseCode;
    responseInfo.info = responseData;


    LOGI("Sending response....\n");

    callback("CASendResponse: ", "calling ...");
    CAResult_t res = CASendResponse(endPoint, &responseInfo);

    LOGI("Response Send....\n");

    if (res != CA_STATUS_OK)
    {
        LOGI("send response error\n");
        callback("CASendResponse: ", "failed");
    }
    else
    {
        LOGI("send response success\n");
        callback("CASendResponse: ", "success");
    }

    LOGI("[returnResponse] OUT");
    callback("returnResponse: ", "out");

    return true;
}

void processRequestResponse(void)
{
    bool showSuccess = true;
    bool showError = true;

    while (g_simulatorProcess)
    {
        if(!handleMessage())
        {
            if (showError)
            {
                LOGI("handle request error\n");
                showError = false;
                showSuccess = true;
            }
        }
        else
        {
            if (showSuccess)
            {
                LOGI("handle request success\n");
                showSuccess = false;
                showError = true;
            }
        }

        sleep(MAX_SLEEP_TIME);
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
    LOGI("unselected network !!! ");
    CAResult_t res = CAUnSelectNetwork(1 << g_selectedNetwork);
    if (res != CA_STATUS_OK)
    {
        LOGI("UNSelectNetwork fail\n");
    }
}

void sendToServer(const CAEndpoint_t* endPoint, TestConfiguration* testConfig)
{
    int index = 0;
    callback("SendToServer: ", "IN");

    char msg[20];

    sprintf(msg, "%d", testConfig->messageType);
    callback("messageType: ", msg);

    sprintf(msg, "%d", testConfig->numberOfTimes);
    callback("numberOfTimes: ", msg);

    switch (testConfig->messageType)
    {
        case MESSAGE_RESPONSE:
            callback("Case: ", "MESSAGE_RESPONSE");
            for (index = 0; index < testConfig->numberOfTimes; index++)
            {
                returnResponse(endPoint, testConfig->resourceUri, testConfig->payload,
                    CA_MSG_NONCONFIRM, CA_VALID, testConfig->messageId, testConfig->token, testConfig->tokenLength, NULL, 0);
                sleep(testConfig->interval);
            }
            break;
        case MESSAGE_REQUEST:
            callback("Case: ", "MESSAGE_REQUEST");
            for (index = 0; index < testConfig->numberOfTimes; index++)
            {
                returnRequest(endPoint, testConfig->resourceUri, testConfig->payload,
                        CA_MSG_NONCONFIRM, testConfig->caMethod, testConfig->token,
                        testConfig->tokenLength, NULL, 0);
                sleep(testConfig->interval);
            }
            break;
        default:
            break;
    }
    callback("SendToServer: ", "OUT");
}

void sendToClient(const CAEndpoint_t* endPoint, TestConfiguration* testConfig)
{
    int index = 0;

    LOGI("IP %s, Port %d", endPoint->addr, endPoint->port);
    LOGI("TestConfig messageType %d", testConfig->messageType);

    switch (testConfig->messageType)
    {
        case MESSAGE_RESPONSE:
            for (index = 0; index < testConfig->numberOfTimes; index++)
            {
                returnResponse(endPoint, testConfig->resourceUri, testConfig->payload,
                        CA_MSG_NONCONFIRM, CA_VALID, testConfig->messageId, testConfig->token,
                        testConfig->tokenLength, NULL, 0);
                sleep(testConfig->interval);
            }
            break;
        case MESSAGE_REQUEST:
            for (index = 0; index < testConfig->numberOfTimes; index++)
            {
                returnRequest(endPoint, testConfig->resourceUri, testConfig->payload,
                        CA_MSG_NONCONFIRM, CA_GET, testConfig->token, testConfig->tokenLength, NULL, 0);
                sleep(testConfig->interval);
            }
            break;
        default:
            break;
    }
}

void showEndpoint(CAEndpoint_t* endPoint)
{
    char msg[11];

    LOGI("IP %s, Port %d", endPoint->addr, endPoint->port);

    callback("IP: ", endPoint->addr);

    sprintf(msg, "%d", endPoint->port);

    callback("Port: ", msg);
}

void showCaInfo(CAInfo_t info)
{
    if(info.resourceUri == NULL)
    {
        LOGI("ResourceUri: NULL");
        callback("ResourceUri: " , "NULL");
    }
    else
    {
        LOGI("ResourceUri: %s", info.resourceUri);
        callback("ResourceUri: " , info.resourceUri);
    }

    char msg[21];

    LOGI("MessageId %d", info.messageId);
    sprintf(msg, "%d", info.messageId);
    callback("MessageId: " , msg);
}

void showHeaderOptions(CAHeaderOption_t* options, uint8_t len)
{
    char msg[50];

    if(options)
    {
        uint8_t i;

        sprintf(msg, "%d", len);

        callback("Total Header Options: ", msg);

        for(i = 0; i < len; i++)
        {
            LOGI("Option %d\n", i + 1);
            LOGI("ID : %d\n", options[i].optionID);
            LOGI("Data[%d]: %s\n", options[i].optionLength,options[i].optionData);

            sprintf(msg, "%d %d %s", i+1, options[i].optionLength, options[i].optionData);

            callback("Header Option: ", msg);
        }
    }
    else
    {
        callback("Header Option: ", "None");
    }
}

void showPayload(char* payload, int length)
{
    if(payload == NULL)
    {
        LOGI("Payload is NULL");
        callback("Payload: ", "NULL");
        return;
    }

    LOGI("Payload: %s\n\n", payload);

    char msg[12];

    sprintf(msg, "%d", length);

    callback("Payload Length: ", msg);

    if(length == LARGE_PAYLOAD_LENGTH)
    {
        callback("Payload Type: ", "Block-Wise");
    }
    else if(length == NORMAL_PAYLOAD_LENGTH)
    {
        callback("Payload Type: ", "Normal");
    }
    else
    {
        callback("Payload Type: ","Configure");
    }

    if(length > 0)
    {
        char str[14];
        int i = 0;

        for(i = 0; i < NORMAL_PAYLOAD_LENGTH; i++)
        {
            str[i] = payload[i];
        }

        if(length > NORMAL_PAYLOAD_LENGTH)
        {
            str[i++] = '.';
            str[i++] = '.';
            str[i++] = '.';
        }

        str[i++] = 0;

        callback("Payload: ", str);
    }

}

void requestHandler(const CAEndpoint_t* endPoint, const CARequestInfo_t* requestInfo)
{
    TestConfiguration testConfig;
    char numConversion[4];

    LOGI("[CALLBACK] RequestHandler");
    callback("### RequestHandler ", "###");

    showEndpoint(endPoint);
    showCaInfo(requestInfo->info);
    showPayload(requestInfo->info.payload, requestInfo->info.payloadSize);
    showHeaderOptions(requestInfo->info.options, requestInfo->info.numOptions);

    if(requestInfo->info.type == CA_MSG_RESET)
    {
        char str[12];
        callback("Msg Type: ", "CA_MSG_RESET");
        sprintf(str, "%d", requestInfo->info.messageId);
        returnResponse(endPoint, SIM_RES_ACK, str, CA_MSG_NONCONFIRM,
                    CA_VALID, requestInfo->info.messageId, requestInfo->info.token, requestInfo->info.tokenLength,
                    requestInfo->info.options, requestInfo->info.numOptions);
        return;
    }



    if(requestInfo->info.resourceUri == NULL)
    {
        return;
    }

    if (strstr(requestInfo->info.resourceUri, SIM_REQ_CONFIG) != NULL)
    {
        LOGI("ResourceUri Type: config");
        callback("ResourceUri Type: ", "config start");

        testConfig.operationType = (requestInfo->info.payload[0] - CH_ZERO);

        switch (testConfig.operationType)
        {
            case SEND_MESSAGE:
                callback("Config Type: ", "SEND_MESSAGE");
                if(mode == server)
                {
                    callback("Mode: ", "SERVER");
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

                    testConfig.caMethod = requestInfo->method;
                    testConfig.resourceUri = requestInfo->info.resourceUri;
                    testConfig.messageId = requestInfo->info.messageId;

                    if (requestInfo->info.token != NULL)
                    {
                        testConfig.token = requestInfo->info.token;
                    }
                    else
                    {
                        testConfig.token = "";
                    }
                    sendToClient(endPoint, &testConfig);
                }
                else if(mode == client)
                {
                    callback("Mode: ", "CLIENT");
                    testConfig.messageType = (requestInfo->info.payload[1] - CH_ZERO);

                    memset(&numConversion, 0, sizeof(numConversion));
                    strncpy(numConversion, &requestInfo->info.payload[2], sizeof(numConversion));
                    testConfig.numberOfTimes = atoi(numConversion);

                    memset(&numConversion, 0, sizeof(numConversion));
                    strncpy(numConversion, &requestInfo->info.payload[6], sizeof(numConversion));
                    testConfig.interval = atoi(numConversion);

                    memset(&numConversion, 0, sizeof(numConversion));
                    strncpy(numConversion, &requestInfo->info.payload[10], sizeof(numConversion));
                    testConfig.bufLength = atoi(numConversion);

                    memset(&testConfig.payload, 0, sizeof(char) * MAX_BUF_LEN);

                    strncpy(testConfig.payload, &requestInfo->info.payload[14], testConfig.bufLength);
                    testConfig.payload[testConfig.bufLength + 1] = '\0';

                    testConfig.caMethod = requestInfo->method;
                    testConfig.resourceUri = requestInfo->info.resourceUri;

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

                    sendToServer(endPoint, &testConfig);
                }

                break;

            case SELECT_NETWORK:
                selectNetworkWithParam(&requestInfo->info.payload[1]);
                break;

            case UNSELECT_NETWORK:
                selectNetworkWithParam(&requestInfo->info.payload[1]);
                break;

            case STOP_SIM:
                g_simulatorProcess = false;
                break;

            default:
                break;
        }
        callback("ResourceUri Type: ", "config end");
    }
    else if (strstr(requestInfo->info.resourceUri, SIM_REQ_ACK) != NULL)
    {
        if (requestInfo != NULL)
        {
            LOGI("[RECV] SIM_REQ_ACK");
            LOGI("Message Type: %d\n", requestInfo->info.type);

            if (requestInfo->info.type == CA_MSG_NONCONFIRM)
            {
                callback("Msg Type: ", "CA_MSG_NONCONFIRM");

                LOGI("calling return response");

                returnResponse(endPoint, requestInfo->info.resourceUri, requestInfo->info.payload, CA_MSG_NONCONFIRM,
                            CA_CONTENT, requestInfo->info.messageId, requestInfo->info.token, requestInfo->info.tokenLength,
                            requestInfo->info.options, requestInfo->info.numOptions);
                LOGI("Returned return response");
            }
            else if (requestInfo->info.type == CA_MSG_CONFIRM)
            {
                LOGI("[REQUEST HANDLER CALLBACK] CONFIRM type\n");
                LOGI("[REQUEST HANDLER CALLBACK] Message ID: %d\n", requestInfo->info.messageId);

                callback("Msg Type: ", "CA_MSG_CONFIRM");

                if (g_messageId != requestInfo->info.messageId)
                {
                    LOGI("[REQUEST HANDLER] CONFIRM type, Ignoring first message.\n");
                    g_messageId = requestInfo->info.messageId;
                    return;
                }
                returnResponse(endPoint, requestInfo->info.resourceUri, requestInfo->info.payload, CA_MSG_NONCONFIRM,
                            CA_CONTENT, requestInfo->info.messageId, requestInfo->info.token, requestInfo->info.tokenLength,
                            requestInfo->info.options, requestInfo->info.numOptions);
            }
            LOGI("end of response");
        }
    }
    LOGI("end of callback");
}

void responseHandler(const CAEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo)
{
    LOGI("[CALLBACK] ResponseHandler");
    callback("### ResponseHandler ", "###");

    showEndpoint(endPoint);
    showCaInfo(responseInfo->info);
    showPayload(responseInfo->info.payload, responseInfo->info.payloadSize);
    showHeaderOptions(responseInfo->info.options, responseInfo->info.numOptions);

    if (responseInfo->info.type == CA_MSG_RESET)
    {
        CAInfo_t caInfo;
        memset(&caInfo, 0, sizeof(CAInfo_t));
        char str[12];
        sprintf(str, "%d", responseInfo->info.messageId);
        returnRequest(endPoint, SIM_REQ_ACK, str, CA_MSG_NONCONFIRM, CA_GET,
                    responseInfo->info.token, responseInfo->info.tokenLength, responseInfo->info.options, responseInfo->info.numOptions);

        return;
    }

    if(responseInfo->info.resourceUri == NULL)
    {
        return;
    }

    if (strstr(responseInfo->info.resourceUri, SIM_RES_ACK) != NULL)
    {
        if (responseInfo != NULL)
        {
            if (responseInfo->info.type == CA_MSG_NONCONFIRM)
            {
                returnRequest(endPoint, responseInfo->info.resourceUri, responseInfo->info.payload, CA_MSG_NONCONFIRM, CA_GET,
                        responseInfo->info.token, responseInfo->info.tokenLength, responseInfo->info.options, responseInfo->info.numOptions);
            }
            if (responseInfo->info.type == CA_MSG_CONFIRM)
            {
                LOGI("[CALLBACK] ResponseHandler Here");
                LOGI("[CALLBACK] ResponseHandler message %d ", responseInfo->info.messageId);

                if (g_firstMessage == true)
                {
                    LOGI("[REQUEST HANDLER CALLBACK] CONFIRM type, Ignoring first message.\n");
                    g_firstMessage = false;
                    return;
                }

                returnRequest(endPoint, responseInfo->info.resourceUri, responseInfo->info.payload, CA_MSG_NONCONFIRM, CA_GET,
                        responseInfo->info.token, responseInfo->info.tokenLength, responseInfo->info.options, responseInfo->info.numOptions);
                g_firstMessage = true;
            }
        }
    }
}

void error_handler(const CAEndpoint_t *rep, const CAErrorInfo_t* errorInfo)
{
    LOGI("+++++++++++++++++++++++++++++++++++ErrorInfo+++++++++++++++++++++++++++++++++++\n");

    if(errorInfo)
    {
        const CAInfo_t *info = &errorInfo->info;
        LOGI("Error Handler, ErrorInfo :\n");
        LOGI("Error Handler result    : %d\n", errorInfo->result);
        LOGI("Error Handler token     : %s\n", info->token);
        LOGI("Error Handler messageId : %d\n", (uint16_t) info->messageId);
        LOGI("Error Handler type      : %d\n", info->type);
        LOGI("Error Handler resourceUri : %s\n", info->resourceUri);
        LOGI("Error Handler payload   : %s\n", info->payload);

        if(CA_ADAPTER_NOT_ENABLED == errorInfo->result)
        {
            LOGI("CA_ADAPTER_NOT_ENABLED, enable the adapter\n");
        }
        else if(CA_SEND_FAILED == errorInfo->result)
        {
            LOGI("CA_SEND_FAILED, unable to send the message, check parameters\n");
        }
        else if(CA_MEMORY_ALLOC_FAILED == errorInfo->result)
        {
            LOGI("CA_MEMORY_ALLOC_FAILED, insufficient memory\n");
        }
        else if(CA_SOCKET_OPERATION_FAILED == errorInfo->result)
        {
            LOGI("CA_SOCKET_OPERATION_FAILED, socket operation failed\n");
        }
        else if(CA_STATUS_FAILED == errorInfo->result)
        {
            LOGI("CA_STATUS_FAILED, message could not be delivered, internal error\n");
        }
    }
    LOGI("++++++++++++++++++++++++++++++++End of ErrorInfo++++++++++++++++++++++++++++++++\n");
}

int registerMessageHandler()
{
    CARegisterHandler(requestHandler, responseHandler, error_handler);

    return 1;
}
