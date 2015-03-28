#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

#include "cainterface.h"
#include "cacommon.h"
#include "com_iotivity_service_RMInterface.h"

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

int g_received = 0;
uint16_t g_localSecurePort = SECURE_DEFAULT_PORT;

void request_handler(const CARemoteEndpoint_t* object, const CARequestInfo_t* requestInfo);
void response_handler(const CARemoteEndpoint_t* object, const CAResponseInfo_t* responseInfo);
void get_resource_uri(const char *URI, char *resourceURI, int length);
int get_secure_information(CAPayload_t payLoad);
CAResult_t get_network_type(int selectedNetwork);
void callback(char *subject, char *receivedData);

CAConnectivityType_t g_selectedNwType = CA_WIFI;
static CAToken_t g_lastRequestToken = NULL;
static uint8_t g_lastRequestTokenLength;
static const char SECURE_COAPS_PREFIX[] = "coaps://";
static const char SECURE_INFO_DATA[]
                                   = "{\"oc\":[{\"href\":\"%s\",\"prop\":{\"rt\":[\"core.led\"],"
                                     "\"if\":[\"oc.mi.def\"],\"obs\":1,\"sec\":1,\"port\":%d}}]}";
static const char NORMAL_INFO_DATA[]
                                   = "{\"oc\":[{\"href\":\"%s\",\"prop\":{\"rt\":[\"core.led\"],"
                                     "\"if\":[\"oc.mi.def\"],\"obs\":1}}]}";

static jobject g_responseListenerObject = NULL;
extern JavaVM *g_jvm;

static CAToken_t g_clientToken;
static uint8_t g_clientTokenLength = NULL;
// init
JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_setNativeResponseListener
    (JNIEnv *env, jobject obj, jobject listener){
    LOGI("setNativeResponseListener");
    g_responseListenerObject = (*env)->NewGlobalRef(env, obj);
}

#ifdef __WITH_DTLS__
static OCDtlsPskCredsBlob *pskCredsBlob = NULL;

void clearDtlsCredentialInfo()
{
    LOGI("clearDtlsCredentialInfo IN\n");
    if (pskCredsBlob)
    {
        // Initialize sensitive data to zeroes before freeing.
        if (pskCredsBlob->creds != NULL)
        {
            memset(pskCredsBlob->creds, 0, sizeof(OCDtlsPskCredsBlob)*(pskCredsBlob->num));
            free(pskCredsBlob->creds);
        }

        memset(pskCredsBlob, 0, sizeof(OCDtlsPskCredsBlob));
        free(pskCredsBlob);
        pskCredsBlob = NULL;
    }
    LOGI("clearDtlsCredentialInfo OUT\n");
}

// Internal API. Invoked by OC stack to retrieve credentials from this module
void CAGetDtlsPskCredentials(OCDtlsPskCredsBlob **credInfo)
{
    LOGI("CAGetDtlsPskCredentials IN\n");

    *credInfo = pskCredsBlob;

    LOGI("CAGetDtlsPskCredentials OUT\n");
}

int32_t SetCredentials()
{
    LOGI("SetCredentials IN\n");
    pskCredsBlob = (OCDtlsPskCredsBlob *)calloc(1, sizeof(OCDtlsPskCredsBlob));
    if (NULL == pskCredsBlob)
     {
        LOGI("Memory allocation failed!\n");
        return -1;
     }
    memcpy(pskCredsBlob->rsIdentity, IDENTITY, DTLS_PSK_ID_LEN);

    pskCredsBlob->num = 1;

    pskCredsBlob->creds = (OCDtlsPskCredsBlob *)malloc(sizeof(OCDtlsPskCredsBlob) *
            (pskCredsBlob->num));
    if (NULL == pskCredsBlob->creds)
    {
        LOGI("Memory allocation failed!\n");
        return -1;
    }

    uint32_t i;
    for (i = 0; i < pskCredsBlob->num; i++)
    {
        memcpy(pskCredsBlob->creds[i].id, IDENTITY, DTLS_PSK_ID_LEN);
        memcpy(pskCredsBlob->creds[i].psk, RS_CLIENT_PSK, DTLS_PSK_PSK_LEN);
    }
    LOGI("SetCredentials OUT\n");
    return 1;
}
#endif

JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMInitialize
  (JNIEnv *env, jobject obj, jobject context)
{
    LOGI("RMInitialize");
    //Currently set context for WiFiCore
    CAJniSetContext(context);
    CALEServerJNISetContext(env, context);
    CALEClientJNISetContext(env, context);
    CALENetworkMonitorJNISetContext(env, context);

    CAResult_t res;

#ifdef __WITH_DTLS__
    if (SetCredentials() != 1)
    {
        LOGI("SetCredentials failed\n");
        return;
    }

    res = CARegisterDTLSCredentialsHandler(CAGetDtlsPskCredentials);
    if(res != CA_STATUS_OK)
    {
        LOGI("Set credential handler fail\n");
        return;
    }
#endif

    if(CA_STATUS_OK != CAInitialize())
    {
        LOGI("Could not Initialize");
    }
}

JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMTerminate(JNIEnv *env, jobject obj)
{
    LOGI("RMTerminate");

    CATerminate();
}

JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMStartListeningServer(JNIEnv *env,
    jobject obj)
{
    LOGI("RMStartListeningServer");

    if(CA_STATUS_OK != CAStartListeningServer())
    {
        LOGI("Could not start Listening server");
    }
}

JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMStartDiscoveryServer(JNIEnv *env,
    jobject obj)
{
    LOGI("RMStartDiscoveryServer");

    if(CA_STATUS_OK != CAStartDiscoveryServer())
    {
        LOGI("Could not start discovery server");
    }
}

JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMRegisterHandler(JNIEnv *env,
    jobject obj)
{
    LOGI("RMRegisterHandler");

    CARegisterHandler(request_handler, response_handler);
}

JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMFindResource(JNIEnv *env,
        jobject obj, jstring uri)
{
    const char* strUri = (*env)->GetStringUTFChars(env, uri, NULL);
    LOGI("RMFindResource - %s", strUri);

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    CAResult_t res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        LOGE("token generate error!!");
        return;
    }

    printf("Generated token %s\n", token);

    res = CAFindResource((const CAURI_t) strUri, token, tokenLength);

    if (res != CA_STATUS_OK)
    {
        LOGE("Could not find resource");
        //destroy token
        CADestroyToken(token);
    }
    else
    {
        LOGI("find resource to %s URI", strUri);
        CADestroyToken(g_lastRequestToken);
        g_lastRequestToken = token;
        g_lastRequestTokenLength = tokenLength;
    }
}

JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMSendRequest(JNIEnv *env,
        jobject obj, jstring uri, jstring payload, jint selectedNetwork, jint isSecured,
        jint msgType)
{
    const char* strUri = (*env)->GetStringUTFChars(env, uri, NULL);
    LOGI("RMSendRequest - %s", strUri);

    CAResult_t res;

    LOGI("selectedNetwork - %d", selectedNetwork);
    res = get_network_type(selectedNetwork);
    if (res != CA_STATUS_OK)
    {
        return;
    }

    //create remote endpoint
    CARemoteEndpoint_t* endpoint = NULL;

    if(CA_STATUS_OK != CACreateRemoteEndpoint((const CAURI_t)strUri, g_selectedNwType, &endpoint))
    {
        LOGI("Could not create remote end point");
        CADestroyRemoteEndpoint(endpoint);
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
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    char resourceURI[15] = {0};

    get_resource_uri((const CAURI_t)strUri, resourceURI, 14);

    CAInfo_t requestData = {0};
    requestData.token = token;
    requestData.tokenLength = tokenLength;

    const char* strPayload = (*env)->GetStringUTFChars(env, payload, NULL);
    if (isSecured == 1)
    {
        uint32_t length = sizeof(SECURE_INFO_DATA) + strlen(resourceURI);
        requestData.payload = (CAPayload_t) malloc(length);
        if (NULL == requestData.payload)
        {
            LOGI("Memory allocation failed!\n");
            // destroy token
            CADestroyToken(token);
            // destroy remote endpoint
            CADestroyRemoteEndpoint(endpoint);
            return;
        }
        snprintf(requestData.payload, length, SECURE_INFO_DATA, resourceURI, g_localSecurePort);
    }
    else
    {
        uint32_t length = sizeof(NORMAL_INFO_DATA) + strlen(resourceURI);
        requestData.payload = (CAPayload_t) malloc(length);
        if (NULL == requestData.payload)
        {
            LOGI("Memory allocation failed!\n");
            // destroy token
            CADestroyToken(token);
            // destroy remote endpoint
            CADestroyRemoteEndpoint(endpoint);
            return;
        }
        snprintf(requestData.payload, length, NORMAL_INFO_DATA, resourceURI);
    }

    requestData.type = messageType;

    CARequestInfo_t requestInfo = {0};
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    if(CA_STATUS_OK != CASendRequest(endpoint, &requestInfo))
    {
        LOGI("Could not send request");
    }

    // destroy token
    CADestroyToken(token);

    // destroy remote endpoint
    CADestroyRemoteEndpoint(endpoint);
}

JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMSendResponse(JNIEnv *env,
    jobject obj, jstring uri, jstring payload, jint selectedNetwork, jint isSecured)
{
    LOGI("RMSendResponse");

    const char* strUri = (*env)->GetStringUTFChars(env, uri, NULL);
    LOGI("RMSendResponse - %s", strUri);

    CAResult_t res;

    LOGI("selectedNetwork - %d", selectedNetwork);

    res = get_network_type(selectedNetwork);
    if (res != CA_STATUS_OK)
    {
        return;
    }

    //create remote endpoint
    CARemoteEndpoint_t* endpoint = NULL;

    if(CA_STATUS_OK != CACreateRemoteEndpoint((const CAURI_t)strUri, g_selectedNwType, &endpoint))
    {
        LOGI("Could not create remote end point");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    CAMessageType_t messageType = CA_MSG_ACKNOWLEDGE;

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        LOGE("token generate error!");
        CADestroyRemoteEndpoint(endpoint);
    }

    char resourceURI[15] = {0};

    get_resource_uri((const CAURI_t)strUri, resourceURI, 14);

    CAInfo_t responseData = {0};
    responseData.token = token;

    const char* strPayload = (*env)->GetStringUTFChars(env, payload, NULL);
    if (isSecured == 1)
    {
        uint32_t length = sizeof(SECURE_INFO_DATA) + strlen(resourceURI);
        responseData.payload = (CAPayload_t) malloc(length);
        if (NULL == responseData.payload)
        {
            LOGI("Memory allocation failed!\n");
            // destroy token
            CADestroyToken(token);
            // destroy remote endpoint
            CADestroyRemoteEndpoint(endpoint);
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
            LOGI("Memory allocation failed!\n");
            // destroy token
            CADestroyToken(token);
            // destroy remote endpoint
            CADestroyRemoteEndpoint(endpoint);
            return;
        }
        snprintf(responseData.payload, length, NORMAL_INFO_DATA, resourceURI);
    }

    responseData.type = messageType;

    CAResponseInfo_t responseInfo = {0};
    responseInfo.result = CA_SUCCESS;
    responseInfo.info = responseData;

    // send request
    if(CA_STATUS_OK != CASendResponse(endpoint, &responseInfo))
    {
        LOGI("Could not send response");
    }

    LOGI("Send response");

    // destroy token
    CADestroyToken(token);

    // destroy remote endpoint
    CADestroyRemoteEndpoint(endpoint);
}

JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMAdvertiseResource(JNIEnv *env,
    jobject obj, jstring uri, jint selectedNetwork)
{
    LOGI("RMAdvertiseResource");

    const char* strUri = (*env)->GetStringUTFChars(env, uri, NULL);

    int optionNum = 2;

    CAHeaderOption_t *headerOpt;
    headerOpt = (CAHeaderOption_t*) calloc(1, sizeof(CAHeaderOption_t) * optionNum);
    if (NULL == headerOpt)
    {
        LOGI("Memory allocation failed!\n");
        return;
    }

    char* tmpOptionData1 = "Hello";
    headerOpt[0].optionID = 3000;
    memcpy(headerOpt[0].optionData, tmpOptionData1, strlen(tmpOptionData1));
    headerOpt[0].optionLength = (uint16_t) strlen(tmpOptionData1);

    char* tmpOptionData2 = "World";
    headerOpt[1].optionID = 3001;
    memcpy(headerOpt[1].optionData, tmpOptionData2, strlen(tmpOptionData2));
    headerOpt[1].optionLength = (uint16_t) strlen(tmpOptionData2);

    // create token
    CAToken_t token = NULL;
    CAResult_t res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        LOGI("token generate error!");
        free(headerOpt);
        return;
    }

    CAAdvertiseResource((const CAURI_t)strUri, token, headerOpt, (uint8_t) optionNum);

    free(headerOpt);
}

JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMSendNotification(JNIEnv *env,
    jobject obj, jstring uri, jstring payload, jint selectedNetwork, jint isSecured)
{
    const char* strUri = (*env)->GetStringUTFChars(env, uri, NULL);
    LOGI("RMSendNotification - %s", strUri);

    CAResult_t res;

    LOGI("selectedNetwork - %d", selectedNetwork);

    res = get_network_type(selectedNetwork);
    if (res != CA_STATUS_OK)
    {
    	LOGI("Not supported network type");
        return;
    }

    //create remote endpoint
    CARemoteEndpoint_t* endpoint = NULL;

    if(CA_STATUS_OK != CACreateRemoteEndpoint((const CAURI_t)strUri, g_selectedNwType, &endpoint))
    {
        LOGI("Could not create remote end point");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    CAMessageType_t messageType = CA_MSG_NONCONFIRM;

    // create token
    CAToken_t token = NULL;
    res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        LOGI("token generate error!");
        token = NULL;
    }

    char resourceURI[15] = {0};

    get_resource_uri((const CAURI_t)strUri, resourceURI, 14);

    CAInfo_t responseData = {0};
    responseData.token = token;

    const char* strPayload = (*env)->GetStringUTFChars(env, payload, NULL);
    if (isSecured == 1)
    {
        uint32_t length = sizeof(SECURE_INFO_DATA) + strlen(resourceURI);
        responseData.payload = (CAPayload_t) malloc(length);
        if (NULL == responseData.payload)
        {
            LOGI("Memory allocation failed!\n");
            // destroy token
            CADestroyToken(token);
            // destroy remote endpoint
            CADestroyRemoteEndpoint(endpoint);
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
            LOGI("Memory allocation failed!\n");
            // destroy token
            CADestroyToken(token);
            // destroy remote endpoint
            CADestroyRemoteEndpoint(endpoint);
            return;
        }
        snprintf(responseData.payload, length, NORMAL_INFO_DATA, resourceURI);
    }

    responseData.type = messageType;

    CAResponseInfo_t responseInfo = {0};
    responseInfo.result = CA_SUCCESS;
    responseInfo.info = responseData;

    // send request
    if(CA_STATUS_OK != CASendNotification(endpoint, &responseInfo))
    {
        LOGI("Could not send notification");
    }

    LOGI("Send Notification");

    // destroy token
    CADestroyToken(token);

    // destroy remote endpoint
    CADestroyRemoteEndpoint(endpoint);
}

JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMSelectNetwork(JNIEnv *env, jobject obj,
    jint networkType)
{
    LOGI("RMSelectNetwork Type : %d", networkType);

    if(CA_STATUS_OK != CASelectNetwork(networkType))
    {
        LOGI("Could not select network");
    }
}

JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMHandleRequestResponse(JNIEnv *env,
    jobject obj)
{
    LOGI("RMHandleRequestResponse");

    if(CA_STATUS_OK != CAHandleRequestResponse())
    {
        LOGI("Could not handle request and response");
    }
}

void request_handler(const CARemoteEndpoint_t* object, const CARequestInfo_t* requestInfo)
{
    if (!object)
    {
        LOGI("Remote endpoint is NULL!");
        return;
    }

    if (!requestInfo)
    {
        LOGI("Request info is NULL!");
        return;
    }

    LOGI("##########received request from remote device #############\n");
    LOGI("Uri: %s\n", object->resourceUri);
    LOGI("Remote Address: %s\n", object->addressInfo.IP.ipAddress);

    LOGI("Data: %s\n", requestInfo->info.payload);

    if (NULL != g_responseListenerObject)
    {
        callback("received request from remote device", "#######");
        callback("Uri: ", object->resourceUri);

        callback("Remote Address: ", (char *) object->addressInfo.IP.ipAddress);

        if(requestInfo->info.payload)
        {
            callback("Data: ", requestInfo->info.payload);
        }
    }

    if ((!g_lastRequestToken) && (!requestInfo->info.token) &&
            (strncmp(g_lastRequestToken, requestInfo->info.token,
                     requestInfo->info.tokenLength) == 0))
    {
        LOGI("token is same. received request of it's own. skip.. \n");
        return;
    }

    if (requestInfo->info.options)
    {
        uint32_t len = requestInfo->info.numOptions;
        uint32_t i;
        for (i = 0; i < len; i++)
        {
            LOGI("Option %d\n", i + 1);
            LOGI("ID : %d\n", requestInfo->info.options[i].optionID);
            LOGI("Data[%d]: %s\n", requestInfo->info.options[i].optionLength,
                   requestInfo->info.options[i].optionData);

            if (NULL != g_responseListenerObject)
            {
                char tmpbuf[30];
                sprintf(tmpbuf, "%d", i + 1);
                callback("Option: ", tmpbuf);

                sprintf(tmpbuf, "%d", requestInfo->info.options[i].optionID);
                callback("ID: ", tmpbuf);

                sprintf(tmpbuf, "Data:[%d]",  requestInfo->info.options[i].optionLength);
                callback("tmpbuf: ", requestInfo->info.options[i].optionData);
            }
        }
    }
    LOGI("############################################################\n");

    //Check if this has secure communication information
    if (requestInfo->info.payload)
    {
        int securePort = get_secure_information(requestInfo->info.payload);
        if (0 < securePort) //Set the remote endpoint secure details and send response
        {
            LOGI("This is secure resource...\n");
            char *uri = NULL;
            uint32_t length = 0;

            length = sizeof(SECURE_COAPS_PREFIX) - 1; //length of "coaps://"
            length += strlen(object->addressInfo.IP.ipAddress) + PORT_LENGTH; // length of "ipaddress:port"
            length += strlen(object->resourceUri) + 1;

            uri = calloc(1,sizeof(char)*length);
            if (!uri)
            {
                LOGI("Failed to create new uri\n");
                return;
            }
            sprintf(uri,"%s%s:%d/%s", SECURE_COAPS_PREFIX, object->addressInfo.IP.ipAddress,
                      securePort, object->resourceUri);

            CARemoteEndpoint_t *endpoint = NULL;
            if (CA_STATUS_OK != CACreateRemoteEndpoint(uri, object->connectivityType, &endpoint))
            {
                LOGI("Failed to create duplicate of remote endpoint!\n");
                return;
            }
            endpoint->isSecured = true;
            object = endpoint;
        }
    }

    g_received = 1;

}

void response_handler(const CARemoteEndpoint_t* object, const CAResponseInfo_t* responseInfo)
{

    LOGI("##########Received response from remote device #############\n");
    LOGI("Uri: %s\n", object->resourceUri);

    if(object->connectivityType == CA_EDR)
    {
        LOGI("Remote Address: %s\n", object->addressInfo.BT.btMacAddress);
    } else if(object->connectivityType == CA_LE)
    {
        LOGI("Remote Address: %s\n", object->addressInfo.LE.leMacAddress);
    } else if(object->connectivityType == CA_WIFI)
    {
        LOGI("Remote Address: %s\n", object->addressInfo.IP.ipAddress);
    }

    LOGI("response result: %d\n", responseInfo->result);
    LOGI("Data: %s\n", responseInfo->info.payload);

    if (NULL != g_responseListenerObject)
    {
        callback("received response from remote device", "#######");
        callback("Uri: ", object->resourceUri);

        if(object->connectivityType == CA_EDR)
        {
            callback("Remote Address: ", (char*) object->addressInfo.BT.btMacAddress);
        } else if(object->connectivityType == CA_LE)
        {
            callback("Remote Address: ", (char*) object->addressInfo.LE.leMacAddress);
        } else if(object->connectivityType == CA_WIFI)
        {
            callback("Remote Address: ", (char*) object->addressInfo.IP.ipAddress);
        }

        if(responseInfo->info.payload)
        {
            callback("Data: ", responseInfo->info.payload);
        }
    }

    if (responseInfo->info.options)
    {
        uint32_t len = responseInfo->info.numOptions;
        uint32_t i;
        for (i = 0; i < len; i++)
        {
            LOGI("Option %d\n", i + 1);
            LOGI("ID : %d\n", responseInfo->info.options[i].optionID);
            LOGI("Data[%d]: %s\n", responseInfo->info.options[i].optionLength,
                      responseInfo->info.options[i].optionData);

            if (NULL != g_responseListenerObject)
            {
                char tmpbuf[30];
                sprintf(tmpbuf, "%d", i + 1);
                callback("Option: ", tmpbuf);

                sprintf(tmpbuf, "%d", responseInfo->info.options[i].optionID);
                callback("ID: ", tmpbuf);

                sprintf(tmpbuf, "Data:[%d]",  responseInfo->info.options[i].optionLength);
                callback("tmpbuf: ", responseInfo->info.options[i].optionData);
            }
        }
    }
    LOGI("############################################################\n");
    g_received = 1;

    //Check if this has secure communication information
    if (responseInfo->info.payload)
    {
        int securePort = get_secure_information(responseInfo->info.payload);
        if (0 < securePort) //Set the remote endpoint secure details and send response
        {
            LOGI("This is secure resource...\n");
        }
    }
}

void get_resource_uri(const char *URI, char *resourceURI, int length)
{
    const char *startPos = URI;
    const char *temp = NULL;
    if (NULL != (temp = strstr(URI, "://")))
    {
        startPos = strchr(temp + 3, '/');
        if (!startPos)
        {
            LOGI("Resource URI is missing\n");
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
        memcpy(resourceURI, startPos + 1, endPos - startPos);

    LOGI("URI: %s, ResourceURI:%s\n", URI, resourceURI);
}

int get_secure_information(CAPayload_t payLoad)
{
    LOGI("entering get_secure_information\n");

    if (!payLoad)
    {
        LOGI("Payload is NULL\n");
        return -1;
    }

    const char *subString = NULL;
    if (NULL == (subString = strstr(payLoad, "\"sec\":1")))
    {
        LOGI("This is not secure resource\n");
        return -1;
    }

    if (NULL == (subString = strstr(payLoad, "\"port\":")))
    {
        LOGI("This secure resource does not have port information\n");
        return -1;
    }

    const char *startPos = strstr(subString, ":");
    if (!startPos)
    {
        LOGI("Parsing failed !\n");
        return -1;
    }

    const char *endPos = strstr(startPos, "}");
    if (!endPos)
    {
        LOGI("Parsing failed !\n");
        return -1;
    }

    char portStr[6] = {0};
    memcpy(portStr, startPos + 1, (endPos-1) - startPos);

    LOGI("secured port is: %s\n", portStr);
    return atoi(portStr);
}

CAResult_t get_network_type(int selectedNetwork)
{

    int number = selectedNetwork;

    if (!(number & 0xf))
    {
        return CA_NOT_SUPPORTED;
    }
    if (number & CA_ETHERNET)
    {
        g_selectedNwType = CA_ETHERNET;
        return CA_STATUS_OK;
    }
    if (number & CA_WIFI)
    {
        g_selectedNwType = CA_WIFI;
        return CA_STATUS_OK;
    }
    if (number & CA_EDR)
    {
        g_selectedNwType = CA_EDR;
        return CA_STATUS_OK;
    }
    if (number & CA_LE)
    {
        g_selectedNwType = CA_LE;
        return CA_STATUS_OK;
    }

    return CA_NOT_SUPPORTED;
}

void callback(char *subject, char *receicedData)
{
    JNIEnv* env = NULL;
    int status = (*g_jvm)->GetEnv(g_jvm, (void **) &env, JNI_VERSION_1_6);
    int res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

    jclass cls = (*env)->GetObjectClass(env, g_responseListenerObject);
    jmethodID mid = (*env)->GetMethodID(env, cls, "OnResponseReceived",
        "(Ljava/lang/String;Ljava/lang/String;)V");

    jstring jsubject = (*env)->NewStringUTF(env, (char*)subject);
    jstring jreceivedData = (*env)->NewStringUTF(env, (char*)receicedData);
    (*env)->CallVoidMethod(env, g_responseListenerObject, mid, jsubject, jreceivedData);
}
