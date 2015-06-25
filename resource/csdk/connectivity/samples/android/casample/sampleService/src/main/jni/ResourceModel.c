#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

#include "cainterface.h"
#include "cacommon.h"

#include "org_iotivity_ca_service_RMInterface.h"

#define  LOG_TAG   "JNI_INTERFACE_SAMPLE"
#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

/**
 * @def RS_IDENTITY
 * @brief
 */
#define IDENTITY     ("1111111111111111")
/* @def RS_CLIENT_PSK
 * @brief
 */
#define RS_CLIENT_PSK   ("AAAAAAAAAAAAAAAA")

#define PORT_LENGTH 5
#define SECURE_DEFAULT_PORT 5684
#define RESOURCE_URI_LENGTH 14
#define OPTION_INFO_LENGTH 1024
#define NETWORK_INFO_LENGTH 1024

typedef struct
{
    char ipAddress[CA_IPADDR_SIZE];
    uint16_t port;
} addressSet_t;

void request_handler(const CAEndpoint_t* object, const CARequestInfo_t* requestInfo);
void response_handler(const CAEndpoint_t* object, const CAResponseInfo_t* responseInfo);
void error_handler(const CAEndpoint_t *object, const CAErrorInfo_t* errorInfo);
void get_resource_uri(const char *URI, char *resourceURI, uint32_t length);
uint32_t get_secure_information(CAPayload_t payLoad);
CAResult_t get_network_type(uint32_t selectedNetwork);
void callback(char *subject, char *receivedData);
CAResult_t get_remote_address(CATransportAdapter_t transportType, const char *address);
void parsing_coap_uri(const char* uri, addressSet_t* address, CATransportFlags_t *flags);

uint16_t g_localSecurePort = SECURE_DEFAULT_PORT;
CATransportAdapter_t g_selectedNwType = CA_ADAPTER_IP;
static CAToken_t g_lastRequestToken = NULL;
static uint8_t g_lastRequestTokenLength = 0;

static const char COAP_PREFIX[] =  "coap://";
static const char COAPS_PREFIX[] = "coaps://";
static const uint16_t COAP_PREFIX_LEN = sizeof(COAP_PREFIX) - 1;
static const uint16_t COAPS_PREFIX_LEN = sizeof(COAPS_PREFIX) - 1;

static const char SECURE_INFO_DATA[]
                                   = "{\"oc\":[{\"href\":\"%s\",\"prop\":{\"rt\":[\"core.led\"],"
                                     "\"if\":[\"oic.if.baseline\"],\"obs\":1,\"sec\":1,\"port\":"
                                     "%d}}]}";
static const char NORMAL_INFO_DATA[]
                                   = "{\"oc\":[{\"href\":\"%s\",\"prop\":{\"rt\":[\"core.led\"],"
                                     "\"if\":[\"oic.if.baseline\"],\"obs\":1}}]}";

static jobject g_responseListenerObject = NULL;
static JavaVM *g_jvm;

static CAEndpoint_t *g_clientEndpoint = NULL;
static char *g_resourceUri = NULL;
static CAToken_t g_clientToken = NULL;
static uint8_t g_clientTokenLength = 0;

static uint16_t g_clientMsgId = 0;
static char *g_remoteAddress = NULL;

// init
JNIEXPORT void JNICALL
Java_org_iotivity_ca_service_RMInterface_setNativeResponseListener(JNIEnv *env, jobject obj,
                                                                   jobject listener)
{
    LOGI("setNativeResponseListener");
    g_responseListenerObject = (*env)->NewGlobalRef(env, obj);
}

#ifdef __WITH_DTLS__
static CADtlsPskCredsBlob_t *pskCredsBlob = NULL;

void clearDtlsCredentialInfo()
{
    LOGI("clearDtlsCredentialInfo IN");
    if (pskCredsBlob)
    {
        // Initialize sensitive data to zeroes before freeing.
        if (NULL != pskCredsBlob->creds)
        {
            memset(pskCredsBlob->creds, 0, sizeof(OCDtlsPskCreds)*(pskCredsBlob->num));
            free(pskCredsBlob->creds);
        }

        memset(pskCredsBlob, 0, sizeof(CADtlsPskCredsBlob_t));
        free(pskCredsBlob);
        pskCredsBlob = NULL;
    }
    LOGI("clearDtlsCredentialInfo OUT");
}

// Internal API. Invoked by OC stack to retrieve credentials from this module
void CAGetDtlsPskCredentials(CADtlsPskCredsBlob_t **credInfo)
{
    LOGI("CAGetDtlsPskCredentials IN");
    *credInfo = (CADtlsPskCredsBlob_t *) malloc(sizeof(CADtlsPskCredsBlob_t));
    if (NULL == *credInfo)
    {
        LOGE("Failed to allocate credential blob.");
        return;
    }

    int16_t credLen = sizeof(OCDtlsPskCreds) * (pskCredsBlob->num);
    (*credInfo)->creds = (OCDtlsPskCreds *) malloc(credLen);
    if (NULL == (*credInfo)->creds)
    {
        LOGE("Failed to allocate crentials.");
        free(*credInfo);
        *credInfo = NULL;
        return;
    }

    memcpy((*credInfo)->identity, pskCredsBlob->identity, DTLS_PSK_ID_LEN);
    (*credInfo)->num = pskCredsBlob->num;
    memcpy((*credInfo)->creds, pskCredsBlob->creds, credLen);

    LOGI("CAGetDtlsPskCredentials OUT");
}

CAResult_t SetCredentials()
{
    LOGI("SetCredentials IN");
    pskCredsBlob = (CADtlsPskCredsBlob_t *)malloc(sizeof(CADtlsPskCredsBlob_t));
    if (NULL == pskCredsBlob)
    {
        LOGE("Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memcpy(pskCredsBlob->identity, IDENTITY, DTLS_PSK_ID_LEN);

    pskCredsBlob->num = 1;

    pskCredsBlob->creds = (OCDtlsPskCreds *)malloc(sizeof(OCDtlsPskCreds) *(pskCredsBlob->num));
    if (NULL == pskCredsBlob->creds)
    {
        LOGE("Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memcpy(pskCredsBlob->creds[0].id, IDENTITY, DTLS_PSK_ID_LEN);
    memcpy(pskCredsBlob->creds[0].psk, RS_CLIENT_PSK, DTLS_PSK_PSK_LEN);

    LOGI("SetCredentials OUT");
    return CA_STATUS_OK;
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
    return;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_service_RMInterface_RMInitialize(JNIEnv *env, jobject obj, jobject context)
{
    LOGI("RMInitialize");

    //Currently set context for Android Platform
    CANativeJNISetContext(env, context);

    CAResult_t res = CAInitialize();

    if (CA_STATUS_OK != res)
    {
        LOGE("Could not Initialize");
    }

#ifdef __WITH_DTLS__
    if (CA_STATUS_OK != SetCredentials())
    {
        LOGE("SetCredentials failed");
        return;
    }

    res = CARegisterDTLSCredentialsHandler(CAGetDtlsPskCredentials);
    if(CA_STATUS_OK != res)
    {
        LOGE("Set credential handler fail");
        return;
    }
#endif
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_service_RMInterface_RMTerminate(JNIEnv *env, jobject obj)
{
    LOGI("RMTerminate");
    CADestroyToken(g_lastRequestToken);
    CATerminate();
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_service_RMInterface_RMStartListeningServer(JNIEnv *env, jobject obj)
{
    LOGI("RMStartListeningServer");

    if (CA_STATUS_OK != CAStartListeningServer())
    {
        LOGE("Could not start Listening server");
    }
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_service_RMInterface_RMStartDiscoveryServer(JNIEnv *env, jobject obj)
{
    LOGI("RMStartDiscoveryServer");

    if (CA_STATUS_OK != CAStartDiscoveryServer())
    {
        LOGE("Could not start discovery server");
    }
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_service_RMInterface_RMRegisterHandler(JNIEnv *env, jobject obj)
{
    LOGI("RMRegisterHandler");

    CARegisterHandler(request_handler, response_handler, error_handler);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_service_RMInterface_RMSendRequest(JNIEnv *env, jobject obj, jstring uri,
                                                       jstring payload, jint selectedNetwork,
                                                       jint isSecured, jint msgType)
{
    LOGI("selectedNetwork - %d", selectedNetwork);
    CAResult_t res = get_network_type(selectedNetwork);
    if (CA_STATUS_OK != res)
    {
        return;
    }

    const char* strUri = (*env)->GetStringUTFChars(env, uri, NULL);
    LOGI("RMSendRequest - %s", strUri);

    CATransportFlags_t flags;
    addressSet_t address = {};
    parsing_coap_uri(strUri, &address, &flags);

    //create remote endpoint
    CAEndpoint_t* endpoint = NULL;
    res = CACreateEndpoint(flags, g_selectedNwType, (const char*)address.ipAddress,
                           address.port, &endpoint);
    if (CA_STATUS_OK != res)
    {
        LOGE("Could not create remote end point");
        (*env)->ReleaseStringUTFChars(env, uri, strUri);
        return;
    }

    CAMessageType_t messageType = msgType;

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        LOGE("token generate error!!");
        // destroy remote endpoint
        CADestroyEndpoint(endpoint);
        (*env)->ReleaseStringUTFChars(env, uri, strUri);
        return;
    }

    char resourceURI[RESOURCE_URI_LENGTH + 1] = { 0 };

    get_resource_uri((const CAURI_t) strUri, resourceURI, RESOURCE_URI_LENGTH);
    (*env)->ReleaseStringUTFChars(env, uri, strUri);

    CAInfo_t requestData = { 0 };
    requestData.token = token;
    requestData.tokenLength = tokenLength;

    if (1 == isSecured)
    {
        uint32_t length = sizeof(SECURE_INFO_DATA) + strlen(resourceURI);
        requestData.payload = (CAPayload_t) malloc(length);
        if (NULL == requestData.payload)
        {
            LOGE("Memory allocation failed!");
            // destroy token
            CADestroyToken(token);
            // destroy remote endpoint
            CADestroyEndpoint(endpoint);
            return;
        }
        snprintf(requestData.payload, length, SECURE_INFO_DATA, resourceURI, g_localSecurePort);
        requestData.payloadSize = length;
    }
    else
    {
        uint32_t length = sizeof(NORMAL_INFO_DATA) + strlen(resourceURI);
        requestData.payload = (CAPayload_t) malloc(length);
        if (NULL == requestData.payload)
        {
            LOGE("Memory allocation failed!");
            // destroy token
            CADestroyToken(token);
            // destroy remote endpoint
            CADestroyEndpoint(endpoint);
            return;
        }
        snprintf(requestData.payload, length, NORMAL_INFO_DATA, resourceURI);
        requestData.payloadSize = length;
    }

    requestData.type = messageType;
    requestData.resourceUri = (CAURI_t) malloc(sizeof(resourceURI));
    if (NULL == requestData.resourceUri)
    {
        LOGE("Memory allocation failed!");
        // destroy token
        CADestroyToken(token);
        // destroy remote endpoint
        CADestroyEndpoint(endpoint);
        free(requestData.payload);
        return;
    }
    memcpy(requestData.resourceUri, resourceURI, sizeof(resourceURI));

    CARequestInfo_t requestInfo = { 0 };
    requestInfo.method = CA_GET;
    requestInfo.isMulticast = false;
    requestInfo.info = requestData;

    // send request
    if (CA_STATUS_OK != CASendRequest(endpoint, &requestInfo))
    {
        LOGE("Could not send request");
    }

    // destroy token
    CADestroyToken(token);

    // destroy remote endpoint
    CADestroyEndpoint(endpoint);

    free(requestData.payload);
    free(requestData.resourceUri);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_service_RMInterface_RMSendReqestToAll(JNIEnv *env, jobject obj, jstring uri,
                                                           jint selectedNetwork)
{
    LOGI("selectedNetwork - %d", selectedNetwork);
    CAResult_t res = get_network_type(selectedNetwork);
    if (CA_STATUS_OK != res)
    {
        return;
    }

    // create remote endpoint
    CAEndpoint_t *endpoint = NULL;
    res = CACreateEndpoint(CA_DEFAULT_FLAGS, g_selectedNwType, NULL, 0, &endpoint);

    if (CA_STATUS_OK != res)
    {
        LOGE("create remote endpoint error, error code: %d", res);
        return;
    }

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        LOGE("token generate error!!");
        // destroy remote endpoint
        CADestroyEndpoint(endpoint);
        return;
    }

    LOGI("generated token %s", token);

    CAInfo_t requestData = { 0 };
    requestData.token = token;
    requestData.tokenLength = tokenLength;
    requestData.payload = (CAPayload_t) "TempJsonPayload";
    requestData.payloadSize = strlen((const char *) requestData.payload);
    requestData.type = CA_MSG_NONCONFIRM;

    const char* strUri = (*env)->GetStringUTFChars(env, uri, NULL);
    LOGI("resourceUri - %s", strUri);
    requestData.resourceUri = (CAURI_t)strUri;

    uint8_t optionNum = 2;
    CAHeaderOption_t *headerOpt = (CAHeaderOption_t*) calloc(1,
                                                             sizeof(CAHeaderOption_t) * optionNum);
    if (NULL == headerOpt)
    {
        LOGE("Memory allocation failed");
        // destroy remote endpoint
        CADestroyEndpoint(endpoint);
        return;
    }

    char* FirstOptionData = "Hello";
    headerOpt[0].optionID = 3000;
    memcpy(headerOpt[0].optionData, FirstOptionData, strlen(FirstOptionData));
    headerOpt[0].optionLength = (uint16_t) strlen(FirstOptionData);

    char* SecondOptionData2 = "World";
    headerOpt[1].optionID = 3001;
    memcpy(headerOpt[1].optionData, SecondOptionData2, strlen(SecondOptionData2));
    headerOpt[1].optionLength = (uint16_t) strlen(SecondOptionData2);

    requestData.numOptions = optionNum;
    requestData.options = headerOpt;

    CARequestInfo_t requestInfo = { 0 };
    requestInfo.method = CA_GET;
    requestInfo.isMulticast = true;
    requestInfo.info = requestData;

    // send request to all
    res = CASendRequest(endpoint, &requestInfo);
    if (CA_STATUS_OK != res)
    {
        LOGE("Could not send request to all");
        //destroy token
        CADestroyToken(token);
    }
    else
    {
        CADestroyToken(g_lastRequestToken);
        g_lastRequestToken = token;
        g_lastRequestTokenLength = tokenLength;
    }

    //ReleaseStringUTFChars for strUri
    (*env)->ReleaseStringUTFChars(env, uri, strUri);

    free(headerOpt);

    // destroy remote endpoint
    CADestroyEndpoint(endpoint);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_service_RMInterface_RMSendResponse(JNIEnv *env, jobject obj,
                                                        jint selectedNetwork,
                                                        jint isSecured, jint msgType,
                                                        jint responseValue)
{
    LOGI("RMSendResponse");

    LOGI("selectedNetwork - %d", selectedNetwork);

    CAResult_t res = get_network_type(selectedNetwork);
    if (CA_STATUS_OK != res)
    {
        LOGE("Not supported network type");
        return;
    }

    if (NULL == g_clientEndpoint)
    {
        LOGE("No Request received");
        return;
    }

    CAMessageType_t messageType = msgType;

    CAInfo_t responseData = { 0 };
    responseData.type = messageType;
    responseData.messageId = g_clientMsgId;
    responseData.resourceUri = (CAURI_t)g_resourceUri;

    CAResponseInfo_t responseInfo = { 0 };

    if (msgType != CA_MSG_RESET)
    {
        responseData.token = g_clientToken;
        responseData.tokenLength = g_clientTokenLength;
        responseInfo.result = responseValue;

        if (1 == isSecured)
        {
            uint32_t length = strlen(SECURE_INFO_DATA) + strlen(g_resourceUri) + 1;
            responseData.payload = (CAPayload_t) malloc(length);
            sprintf(responseData.payload, SECURE_INFO_DATA, g_resourceUri,
                    g_localSecurePort);
            responseData.payloadSize = length;
        }
        else
        {
            uint32_t length = strlen(NORMAL_INFO_DATA) + strlen(g_resourceUri) + 1;
            responseData.payload = (CAPayload_t) malloc(length);
            sprintf(responseData.payload, NORMAL_INFO_DATA, g_resourceUri);
            responseData.payloadSize = length;
        }
    }
    //msgType is RESET
    else
    {
        responseInfo.result = CA_EMPTY;
    }

    responseInfo.info = responseData;

    // send response
    res = CASendResponse(g_clientEndpoint, &responseInfo);
    if (CA_STATUS_OK != res)
    {
        LOGE("Could not send response");
    }

    // destroy token
    CADestroyToken(g_clientToken);
    g_clientToken = NULL;
    g_clientTokenLength = 0;

    // destroy remote endpoint
    CADestroyEndpoint(g_clientEndpoint);
    g_clientEndpoint = NULL;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_service_RMInterface_RMSendNotification(JNIEnv *env, jobject obj, jstring uri,
                                                            jstring payload, jint selectedNetwork,
                                                            jint isSecured, jint msgType,
                                                            jint responseValue)
{
    LOGI("selectedNetwork - %d", selectedNetwork);

    CAResult_t res = get_network_type(selectedNetwork);
    if (CA_STATUS_OK != res)
    {
        LOGE("Not supported network type");
        return;
    }

    const char* strUri = (*env)->GetStringUTFChars(env, uri, NULL);
    LOGI("RMSendNotification - %s", strUri);

    CATransportFlags_t flags;
    addressSet_t address = {};
    parsing_coap_uri(strUri, &address, &flags);

    //create remote endpoint
    CAEndpoint_t* endpoint = NULL;
    if (CA_STATUS_OK != CACreateEndpoint(flags, g_selectedNwType,
                                         (const char*)address.ipAddress,
                                         address.port, &endpoint))
    {
        //ReleaseStringUTFChars for strUri
        (*env)->ReleaseStringUTFChars(env, uri, strUri);
        LOGE("Could not create remote end point");
        return;
    }

    char resourceURI[RESOURCE_URI_LENGTH + 1] = { 0 };
    get_resource_uri(strUri, resourceURI, RESOURCE_URI_LENGTH);

    //ReleaseStringUTFChars for strUri
    (*env)->ReleaseStringUTFChars(env, uri, strUri);

    CAMessageType_t messageType = msgType;

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        LOGE("token generate error!");
        CADestroyEndpoint(endpoint);
        return;
    }

    CAInfo_t responseData = { 0 };
    responseData.token = token;
    responseData.tokenLength = tokenLength;
    responseData.resourceUri = (CAURI_t) malloc(sizeof(resourceURI));
    if (NULL == responseData.resourceUri)
    {
        LOGE("Memory allocation failed!");
        // destroy token
        CADestroyToken(token);
        // destroy remote endpoint
        CADestroyEndpoint(endpoint);
        return;
    }
    memcpy(responseData.resourceUri, resourceURI, sizeof(resourceURI));

    if (1 == isSecured)
    {
        uint32_t length = sizeof(SECURE_INFO_DATA) + strlen(resourceURI);
        responseData.payload = (CAPayload_t) malloc(length);
        if (NULL == responseData.payload)
        {
            LOGE("Memory allocation failed!");
            // destroy token
            CADestroyToken(token);
            // destroy remote endpoint
            CADestroyEndpoint(endpoint);

            free(responseData.resourceUri);
            return;
        }
        snprintf(responseData.payload, length, SECURE_INFO_DATA, resourceURI, g_localSecurePort);
    }
    else
    {
        uint32_t length = sizeof(NORMAL_INFO_DATA) + strlen(resourceURI);
        responseData.payload = (CAPayload_t) malloc(length);
        if (NULL == responseData.payload)
        {
            LOGE("Memory allocation failed!");
            // destroy token
            CADestroyToken(token);
            // destroy remote endpoint
            CADestroyEndpoint(endpoint);

            free(responseData.resourceUri);
            return;
        }
        snprintf(responseData.payload, length, NORMAL_INFO_DATA, resourceURI);
    }

    responseData.type = messageType;

    CAResponseInfo_t responseInfo = { 0 };
    responseInfo.result = responseValue;
    responseInfo.info = responseData;

    // send notification
    if (CA_STATUS_OK != CASendNotification(endpoint, &responseInfo))
    {
        LOGE("Could not send notification");
    }

    LOGI("Send Notification");

    // destroy token
    CADestroyToken(token);

    // destroy remote endpoint
    CADestroyEndpoint(endpoint);

    free(responseData.payload);
    free(responseData.resourceUri);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_service_RMInterface_RMSelectNetwork(JNIEnv *env, jobject obj,
                                                         jint networkType)
{
    LOGI("RMSelectNetwork Type : %d", networkType);

    if (CA_STATUS_OK != CASelectNetwork(networkType))
    {
        LOGE("Could not select network");
    }
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_service_RMInterface_RMUnSelectNetwork(JNIEnv *env, jobject obj,
                                                           jint networkType)
{
    LOGI("RMUnSelectNetwork Type : %d", networkType);

    if (CA_STATUS_OK != CAUnSelectNetwork(networkType))
    {
        LOGE("Could not unselect network");
    }
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_service_RMInterface_RMGetNetworkInfomation(JNIEnv *env, jobject obj)
{
    LOGI("RMGetNetworkInfomation");

    CAEndpoint_t *tempInfo = NULL;
    uint32_t tempSize = 0;

    CAResult_t res = CAGetNetworkInformation(&tempInfo, &tempSize);
    if (CA_STATUS_OK != res)
    {
        LOGE("Could not start get network information");
        free(tempInfo);
        return;
    }

    LOGI("################## Network Information #######################");
    callback("######## Network Information", "#######");
    LOGI("Network info total size is %d", tempSize);

    uint32_t index;
    for (index = 0; index < tempSize; index++)
    {
        res = get_remote_address(tempInfo[index].adapter, tempInfo[index].addr);
        if (CA_STATUS_OK != res)
        {
            free(tempInfo);
            return;
        }
        if (NULL != g_responseListenerObject)
        {
            char networkInfo[NETWORK_INFO_LENGTH];
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
            LOGI("Address: %s", g_remoteAddress);
            callback("Address: ", g_remoteAddress);
            free(g_remoteAddress);
        }
        if (tempInfo[index].flags & CA_SECURE)
        {
            g_localSecurePort = tempInfo[index].port;
        }
    }

    // free
    free(tempInfo);

    LOGI("##############################################################");
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_service_RMInterface_RMHandleRequestResponse(JNIEnv *env, jobject obj)
{
    LOGI("RMHandleRequestResponse");

    if (CA_STATUS_OK != CAHandleRequestResponse())
    {
        LOGE("Could not handle request and response");
    }
}

void request_handler(const CAEndpoint_t* object, const CARequestInfo_t* requestInfo)
{

    if (!object)
    {
        LOGE("Remote endpoint is NULL!");
        return;
    }

    if (!requestInfo)
    {
        LOGE("Request info is NULL!");
        return;
    }

    if ((NULL != g_lastRequestToken) && (NULL != requestInfo->info.token) &&
            (strncmp(g_lastRequestToken, requestInfo->info.token,
                     requestInfo->info.tokenLength) == 0))
    {
        LOGI("token is same. received request of it's own. skip.. ");
        return;
    }

    CAResult_t res = get_remote_address(object->adapter, object->addr);
    if (CA_STATUS_OK != res)
    {
        return;
    }

    LOGI("##########received request from remote device #############");
    LOGI("Remote Address: %s", g_remoteAddress);
    LOGI("Uri: %s", requestInfo->info.resourceUri);
    LOGI("Data: %s", requestInfo->info.payload);
    LOGI("Token: %s", requestInfo->info.token);
    LOGI("Code: %d", requestInfo->method);
    LOGI("MessageType: %d", requestInfo->info.type);

    if (NULL != g_responseListenerObject)
    {
        callback("received request from remote device", "#######");
        char *cloneUri = NULL;
        uint32_t len = 0;

        if (NULL != requestInfo->info.resourceUri)
        {
            len = strlen(requestInfo->info.resourceUri);
            cloneUri = (char *)malloc(sizeof(char) * (len + 1));

            if (NULL == cloneUri)
            {
                LOGE("cloneUri Out of memory");
                free(g_remoteAddress);
                return;
            }

            memcpy(cloneUri, requestInfo->info.resourceUri, len + 1);
            callback("Uri: ", cloneUri);
        }

        len = strlen(g_remoteAddress);
        char *cloneRemoteAddress = (char *) malloc(sizeof(char) * (len + 1));

        if (NULL == cloneRemoteAddress)
        {
            LOGE("cloneRemoteAddress Out of memory");
            free(g_remoteAddress);
            free(cloneUri);
            return;
        }

        memcpy(cloneRemoteAddress, g_remoteAddress, len + 1);

        callback("Remote Address: ", cloneRemoteAddress);
        free(cloneRemoteAddress);
        free(g_remoteAddress);

        //clone g_clientEndpoint
        g_clientEndpoint = (CAEndpoint_t *) malloc(sizeof(CAEndpoint_t));
        if (NULL == g_clientEndpoint)
        {
            LOGE("g_clientEndpoint Out of memory");
            free(cloneUri);
            return;
        }
        memcpy(g_clientEndpoint, object, sizeof(CAEndpoint_t));

        if (NULL != cloneUri)
        {
            len = strlen(cloneUri);
            g_resourceUri = (char *) malloc(sizeof(char) * (len + 1));
            if (NULL == g_resourceUri)
            {
                LOGE("g_clientEndpoint->resourceUri Out of memory");
                free(g_clientEndpoint);
                free(cloneUri);
                return;
            }
            memcpy(g_resourceUri, cloneUri, len + 1);
            free(cloneUri);
        }
        //clone g_clientToken
        len = requestInfo->info.tokenLength;

        g_clientToken = (char *) malloc(sizeof(char) * len);
        if (NULL == g_clientToken)
        {
            LOGE("g_clientToken Out of memory");
            free(g_clientEndpoint);
            return;
        }

        if (NULL != requestInfo->info.token)
        {
            memcpy(g_clientToken, requestInfo->info.token, len);
            g_clientTokenLength = len;

        }

        //clone g_clientMsgId
        g_clientMsgId = requestInfo->info.messageId;

        if (NULL != requestInfo->info.payload)
        {
            len = strlen(requestInfo->info.payload);
            char *clonePayload = (char *) malloc(sizeof(char) * (len + 1));

            if (NULL == clonePayload)
            {
                LOGE("clonePayload Out of memory");
                free(g_clientEndpoint);
                return;
            }

            memcpy(clonePayload, requestInfo->info.payload, len + 1);

            callback("Data: ", clonePayload);
            free(clonePayload);
        }
    }

    if (requestInfo->info.options)
    {
        uint32_t len = requestInfo->info.numOptions;
        uint32_t i;

        LOGI("Option count: %d", requestInfo->info.numOptions);

        for (i = 0; i < len; i++)
        {
            LOGI("Option %d", i + 1);
            LOGI("ID : %d", requestInfo->info.options[i].optionID);
            LOGI("Data[%d]: %s", requestInfo->info.options[i].optionLength,
                 requestInfo->info.options[i].optionData);

            if (NULL != g_responseListenerObject)
            {
                char optionInfo[OPTION_INFO_LENGTH] = { 0, };
                sprintf(optionInfo, "Num[%d] - ID : %d, Option Length : %d", i + 1,
                        requestInfo->info.options[i].optionID,
                        requestInfo->info.options[i].optionLength);

                callback("Option info: ", optionInfo);

                size_t optionDataLen = strlen(requestInfo->info.options[i].optionData);
                char *cloneOptionData = (char *) malloc(sizeof(char) * (optionDataLen + 1));
                if (NULL == cloneOptionData)
                {
                    LOGE("cloneOptionData Out of memory");
                    free(g_clientEndpoint);
                    return;
                }

                memcpy(cloneOptionData, requestInfo->info.options[i].optionData,
                       optionDataLen + 1);

                callback("Option Data: ", cloneOptionData);
                free(cloneOptionData);
            }
        }
    }
    LOGI("############################################################");

    //Check if this has secure communication information
    if (requestInfo->info.payload && CA_ADAPTER_IP == object->adapter)
    {
        uint32_t securePort = get_secure_information(requestInfo->info.payload);
        if (0 < securePort) //Set the remote endpoint secure details and send response
        {
            LOGI("This is secure resource...");

            CAEndpoint_t *endpoint = NULL;
            if (CA_STATUS_OK != CACreateEndpoint(CA_SECURE,
                        object->adapter, object->addr, securePort, &endpoint))
            {
                LOGE("Failed to create duplicate of remote endpoint!");
                return;
            }
            object = endpoint;
        }
    }
}

void response_handler(const CAEndpoint_t* object, const CAResponseInfo_t* responseInfo)
{

    CAResult_t res = get_remote_address(object->adapter, object->addr);
    if (CA_STATUS_OK != res)
    {
        return;
    }

    LOGI("##########Received response from remote device #############");
    LOGI("Uri: %s", responseInfo->info.resourceUri);
    LOGI("Remote Address: %s", g_remoteAddress);
    LOGI("response result: %d", responseInfo->result);
    LOGI("Data: %s", responseInfo->info.payload);
    LOGI("Token: %s", responseInfo->info.token);
    LOGI("MessageType: %d", responseInfo->info.type);

    if (NULL != g_responseListenerObject)
    {
        uint32_t len = 0;

        if (NULL != responseInfo->info.resourceUri)
        {
            len = strlen(responseInfo->info.resourceUri);
            char *cloneUri = (char *) malloc(sizeof(char) * (len + 1));

            if (NULL == cloneUri)
            {
                LOGE("cloneUri Out of memory");
                free(g_remoteAddress);
                return;
            }

            memcpy(cloneUri, responseInfo->info.resourceUri, len + 1);

            callback("Uri: ", cloneUri);
            free(cloneUri);
        }

        len = strlen(g_remoteAddress);
        char *cloneRemoteAddress = (char *) malloc(sizeof(char) * (len + 1));

        if (NULL == cloneRemoteAddress)
        {
            LOGE("cloneRemoteAddress Out of memory");
            free(g_remoteAddress);
            return;
        }

        memcpy(cloneRemoteAddress, g_remoteAddress, len + 1);

        callback("Remote Address: ", cloneRemoteAddress);
        free(cloneRemoteAddress);
        free(g_remoteAddress);

        if (NULL != responseInfo->info.payload)
        {
            len = strlen(responseInfo->info.payload);
            char *clonePayload = (char *) malloc(sizeof(char) * (len + 1));

            if (NULL == clonePayload)
            {
                LOGE("clonePayload Out of memory");
                return;
            }

            memcpy(clonePayload, responseInfo->info.payload, len + 1);

            callback("Data: ", clonePayload);
            free(clonePayload);
        }
    }

    if (responseInfo->info.options)
    {
        uint32_t len = responseInfo->info.numOptions;
        uint32_t i;
        for (i = 0; i < len; i++)
        {
            LOGI("Option %d", i + 1);
            LOGI("ID : %d", responseInfo->info.options[i].optionID);
            LOGI("Data[%d]: %s", responseInfo->info.options[i].optionLength,
                 responseInfo->info.options[i].optionData);

            if (NULL != g_responseListenerObject)
            {
                char optionInfo[OPTION_INFO_LENGTH] = { 0, };
                sprintf(optionInfo, "Num[%d] - ID : %d, Option Length : %d", i + 1,
                        responseInfo->info.options[i].optionID,
                        responseInfo->info.options[i].optionLength);

                callback("Option info: ", optionInfo);

                size_t optionDataLen = strlen(responseInfo->info.options[i].optionData);
                char *cloneOptionData = (char *) malloc(sizeof(char) * (optionDataLen + 1));
                if (NULL == cloneOptionData)
                {
                    LOGE("cloneOptionData Out of memory");
                    return;
                }
                memcpy(cloneOptionData, responseInfo->info.options[i].optionData,
                       optionDataLen + 1);
                callback("Option Data: ", cloneOptionData);
                free(cloneOptionData);
            }
        }
    }
    LOGI("############################################################");

    //Check if this has secure communication information
    if (responseInfo->info.payload && CA_ADAPTER_IP == object->adapter)
    {
        uint32_t securePort = get_secure_information(responseInfo->info.payload);
        if (0 < securePort) //Set the remote endpoint secure details and send response
        {
            LOGI("This is secure resource...");
        }
    }
}

void error_handler(const CAEndpoint_t *rep, const CAErrorInfo_t* errorInfo)
{
    printf("+++++++++++++++++++++++++++++++++++ErrorInfo+++++++++++++++++++++++++++++++++++");

    if(errorInfo)
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

void get_resource_uri(const char *URI, char *resourceURI, uint32_t length)
{
    const char *startPos = URI;
    const char *temp = strstr(URI, "://");
    if (NULL != temp)
    {
        startPos = strchr(temp + 3, '/');
        if (!startPos)
        {
            LOGE("Resource URI is missing");
            return;
        }
    }

    const char *endPos = strchr(startPos, '?');
    if (!endPos)
    {
        endPos = URI + strlen(URI);
    }
    --endPos;

    if (endPos - startPos <= length)
    {
        memcpy(resourceURI, startPos + 1, endPos - startPos);
    }

    LOGI("URI: %s, ResourceURI: %s", URI, resourceURI);
}

uint32_t get_secure_information(CAPayload_t payLoad)
{
    LOGI("entering get_secure_information");

    if (!payLoad)
    {
        LOGE("Payload is NULL");
        return -1;
    }

    const char *subString = NULL;
    if (NULL == (subString = strstr(payLoad, "\"sec\":1")))
    {
        LOGE("This is not secure resource");
        return -1;
    }

    if (NULL == (subString = strstr(payLoad, "\"port\":")))
    {
        LOGE("This secure resource does not have port information");
        return -1;
    }

    const char *startPos = strstr(subString, ":");
    if (!startPos)
    {
        LOGE("Parsing failed !");
        return -1;
    }

    const char *endPos = strstr(startPos, "}");
    if (!endPos)
    {
        LOGE("Parsing failed !");
        return -1;
    }

    char portStr[6] = { 0 };
    memcpy(portStr, startPos + 1, (endPos - 1) - startPos);

    LOGI("secured port is: %s", portStr);
    return atoi(portStr);
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
    JNIEnv* env = NULL;
    uint32_t status = (*g_jvm)->GetEnv(g_jvm, (void **) &env, JNI_VERSION_1_6);
    uint32_t res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

    jclass cls = (*env)->GetObjectClass(env, g_responseListenerObject);
    jmethodID mid = (*env)->GetMethodID(env, cls, "OnResponseReceived",
                                        "(Ljava/lang/String;Ljava/lang/String;)V");

    jstring jsubject = (*env)->NewStringUTF(env, (char*) subject);
    jstring jreceivedData = (*env)->NewStringUTF(env, (char*) receivedData);
    (*env)->CallVoidMethod(env, g_responseListenerObject, mid, jsubject, jreceivedData);

}

CAResult_t get_remote_address(CATransportAdapter_t transportType, const char *address)
{
    uint32_t len = strlen(address);

    g_remoteAddress = (char *)malloc(sizeof (char) * (len + 1));
    if (NULL == g_remoteAddress)
    {
        LOGE("g_remoteAddress Out of memory");
        return CA_MEMORY_ALLOC_FAILED;
    }

    memcpy(g_remoteAddress, address, len + 1);

    return CA_STATUS_OK;
}


void parsing_coap_uri(const char* uri, addressSet_t* address, CATransportFlags_t *flags)
{
    if (NULL == uri || NULL == address)
    {
        LOGE("parameter is null");
        return;
    }

    // parse uri
    // #1. check prefix
    uint8_t startIndex = 0;
    if (strncmp(COAPS_PREFIX, uri, COAPS_PREFIX_LEN) == 0)
    {
        LOGI("uri has '%s' prefix", COAPS_PREFIX);
        startIndex = COAPS_PREFIX_LEN;
        *flags = CA_SECURE;
    }
    else if (strncmp(COAP_PREFIX, uri, COAP_PREFIX_LEN) == 0)
    {
        LOGI("uri has '%s' prefix", COAP_PREFIX);
        startIndex = COAP_PREFIX_LEN;
        *flags = CA_DEFAULT_FLAGS;
    }

    // #2. copy uri for parse
    int32_t len = strlen(uri) - startIndex;

    if (len <= 0)
    {
        LOGE("uri length is 0!");
        return;
    }

    char *cloneUri = (char *) calloc(len + 1, sizeof(char));
    if (NULL == cloneUri)
    {
        LOGE("Out of memory");
        return;
    }

    memcpy(cloneUri, &uri[startIndex], sizeof(char) * len);
    cloneUri[len] = '\0';

    char *pAddress = cloneUri;
    char *pResourceUri = NULL;

    int32_t i = 0;
    for (i = 0; i < len; i++)
    {
        if (cloneUri[i] == '/')
        {
            // separate
            cloneUri[i] = 0;
            pResourceUri = &cloneUri[i + 1];
            break;
        }
    }
    LOGI("pAddress : %s", pAddress);

    int res = get_address_set(pAddress, address);
    if (res == -1)
    {
        LOGE("address parse error");

        free(cloneUri);
        return;
    }
    return;
}

int get_address_set(const char *pAddress, addressSet_t* outAddress)
{
    if (NULL == pAddress || NULL == outAddress)
    {
        LOGE("parameter is null");
        return -1;
    }

    int32_t len = strlen(pAddress);
    int32_t isIp = 0;
    int32_t ipLen = 0;

    int32_t i = 0;
    for (i = 0; i < len; i++)
    {
        if (pAddress[i] == '.')
        {
            isIp = 1;
        }

        // found port number start index
        if (isIp && pAddress[i] == ':')
        {
            ipLen = i;
            break;
        }
    }

    if (isIp)
    {
        if(ipLen && ipLen < sizeof(outAddress->ipAddress))
        {
            strncpy(outAddress->ipAddress, pAddress, ipLen);
            outAddress->ipAddress[ipLen] = '\0';
        }
        else if (!ipLen && len < sizeof(outAddress->ipAddress))
        {
            strncpy(outAddress->ipAddress, pAddress, len);
            outAddress->ipAddress[len] = '\0';
        }
        else
        {
            LOGE("IP Address too long: %d", ipLen==0 ? len : ipLen);
            return -1;
        }

        if (ipLen > 0)
        {
            outAddress->port = atoi(pAddress + ipLen + 1);
        }
    }
    else
    {
        strncpy(outAddress->ipAddress, pAddress, len);
        outAddress->ipAddress[len] = '\0';
    }

    return isIp;
}
