#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

#include <../../../../api/cainterface.h>
#include <../../../../api/cacommon.h>
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

int gReceived;
CABool_t gLocalUnicastPort;
CABool_t gLocalSecurePort;

void request_handler(const CARemoteEndpoint_t* object, const CARequestInfo_t* requestInfo);
void response_handler(const CARemoteEndpoint_t* object, const CAResponseInfo_t* responseInfo);
void get_resource_uri(char *URI, char *resourceURI, int length);
int get_secure_information(CAPayload_t payLoad);
CAResult_t get_network_type(int selectedNetwork);

CAConnectivityType_t gSelectedNwType;
static CAToken_t gLastRequestToken = NULL;
static const char *gSecureInfoData = "{\"oc\":[{\"href\":\"%s\",\"prop\":{\"rt\":[\"core.led\"],"
                                     "\"if\":[\"oc.mi.def\"],\"obs\":1,\"sec\":1,\"port\":%d}}]}";
static const char *gNormalInfoData = "{\"oc\":[{\"href\":\"%s\",\"prop\":{\"rt\":[\"core.led\"],"
                                     "\"if\":[\"oc.mi.def\"],\"obs\":1}}]}";

static CADtlsPskCredsBlob_t *pskCredsBlob = NULL;


void clearDtlsCredentialInfo()
{
    printf("clearDtlsCredentialInfo IN\n");
    if (pskCredsBlob)
    {
        // Initialize sensitive data to zeroes before freeing.
        memset(pskCredsBlob->creds, 0, sizeof(CADtlsPskCreds_t)*(pskCredsBlob->num));
        free(pskCredsBlob->creds);

        memset(pskCredsBlob, 0, sizeof(CADtlsPskCredsBlob_t));
        free(pskCredsBlob);
        pskCredsBlob = NULL;
    }
    printf("clearDtlsCredentialInfo OUT\n");
}

// Internal API. Invoked by OC stack to retrieve credentials from this module
void CAGetDtlsPskCredentials(CADtlsPskCredsBlob_t **credInfo)
{
    printf("CAGetDtlsPskCredentials IN\n");

    *credInfo = pskCredsBlob;

    printf("CAGetDtlsPskCredentials OUT\n");
}

int32_t SetCredentials()
{
    printf("SetCredentials IN\n");
    pskCredsBlob = (CADtlsPskCredsBlob_t *)malloc(sizeof(CADtlsPskCredsBlob_t));

    memset(pskCredsBlob, 0x0, sizeof(CADtlsPskCredsBlob_t));
    memcpy(pskCredsBlob->rsIdentity, IDENTITY, DTLS_PSK_ID_LEN);

    pskCredsBlob->num = 1;

    pskCredsBlob->creds = (CADtlsPskCreds_t *)malloc(sizeof(CADtlsPskCreds_t) *(pskCredsBlob->num));

    memcpy(pskCredsBlob->creds[0].clientIdentity, IDENTITY, DTLS_PSK_ID_LEN);
    memcpy(pskCredsBlob->creds[0].rsClientPsk, RS_CLIENT_PSK, DTLS_PSK_PSK_LEN);

    printf("SetCredentials OUT\n");
    return 1;
}


JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMInitialize
  (JNIEnv *env, jobject obj, jobject context)
{
    LOGI("RMInitialize");
    //Currently set context for WiFiCore
    CAJniSetContext(context);
    CALEServerJNISetContext(env, context);
    CALEClientJNISetContext(env, context);
    CALENetworkMonitorJNISetContext(env, context);

    if (SetCredentials() == 0)
    {
        printf("SetCredentials failed\n");
    }

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

    if (CA_STATUS_OK != CARegisterHandler(request_handler, response_handler))
    {
        LOGI("Could not register handler");
    }
}

JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMFindResource(JNIEnv *env, jobject obj,
    jstring uri)
{
    const char* strUri = (*env)->GetStringUTFChars(env, uri, NULL);
    LOGI("RMFindResource - %s", strUri);

    // create token
    CAToken_t token = NULL;
    CAResult_t res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!\n");
        token = NULL;
    }

    LOGI("generated token %s\n", (token != NULL) ? token : "");

    if(CA_STATUS_OK != CAFindResource(strUri, token))
    {
        LOGI("Could not find resource");
    }
    else
    {
        LOGI("find resource to %s URI", strUri);
        gLastRequestToken = token;
    }
}

JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMSendRequest(JNIEnv *env, jobject obj,
    jstring uri, jint selectedNetwork, jint isSecured, jint msgType)
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

    if(CA_STATUS_OK != CACreateRemoteEndpoint(strUri, gSelectedNwType, &endpoint))
    {
        LOGI("Could not create remote end point");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    CAMessageType_t messageType = msgType;

    // create token
    CAToken_t token = NULL;
    res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!\n");
        token = NULL;
    }

    char resourceURI[15] = {0};

    get_resource_uri(strUri, resourceURI, 14);

    CAInfo_t requestData;
    memset(&requestData, 0, sizeof(CAInfo_t));
    requestData.token = token;


    if (isSecured == 1)
    {
        int length = strlen(gSecureInfoData) + strlen(resourceURI) + 1;
        requestData.payload = (CAPayload_t) malloc(length);
        sprintf(requestData.payload, gSecureInfoData, resourceURI, gLocalSecurePort);
    }
    else
    {
        int length = strlen(gNormalInfoData) + strlen(resourceURI) + 1;
        requestData.payload = (CAPayload_t) malloc(length);
        sprintf(requestData.payload, gNormalInfoData, resourceURI);
    }

    requestData.type = messageType;

    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    if(CA_STATUS_OK != CASendRequest(endpoint, &requestInfo))
    {
        LOGI("Could not send request");
    }

    // destroy token
    if (token != NULL)
    {
        CADestroyToken(token);
    }

    // destroy remote endpoint
    if (endpoint != NULL)
    {
        CADestroyRemoteEndpoint(endpoint);
    }
}

JNIEXPORT void JNICALL Java_com_iotivity_service_RMInterface_RMSendNotification(JNIEnv *env,
    jobject obj, jstring uri, jint selectedNetwork)
{
    jint isSecured = 0;

    const char* strUri = (*env)->GetStringUTFChars(env, uri, NULL);
    LOGI("RMSendResponse - %s", strUri);

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

    if(CA_STATUS_OK != CACreateRemoteEndpoint(strUri, gSelectedNwType, &endpoint))
    {
        LOGI("Could not create remote end point");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    CAMessageType_t messageType = CA_MSG_ACKNOWLEDGE;

    // create token
    CAToken_t token = NULL;
    res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        LOGI("token generate error!");
        token = NULL;
    }

    char resourceURI[15] = {0};

    get_resource_uri(strUri, resourceURI, 14);

    CAInfo_t responseData;
    memset(&responseData, 0, sizeof(CAInfo_t));
    responseData.token = token;


    if (isSecured == 1)
    {
        int length = strlen(gSecureInfoData) + strlen(resourceURI) + 1;
        responseData.payload = (CAPayload_t) malloc(length);
        sprintf(responseData.payload, gSecureInfoData, resourceURI, gLocalSecurePort);
    }
    else
    {
        int length = strlen(gNormalInfoData) + strlen(resourceURI) + 1;
        responseData.payload = (CAPayload_t) malloc(length);
        sprintf(responseData.payload, gNormalInfoData, resourceURI);
    }

    responseData.type = messageType;

    CAResponseInfo_t responseInfo;
    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
    responseInfo.result = CA_SUCCESS;
    responseInfo.info = responseData;

    // send request
    if(CA_STATUS_OK != CASendResponse(endpoint, &responseInfo))
    {
        LOGI("Could not send response");
    }

    LOGI("Send response");

    // destroy token
    if (token != NULL)
    {
        CADestroyToken(token);
    }

    // destroy remote endpoint
    if (endpoint != NULL)
    {
        CADestroyRemoteEndpoint(endpoint);
    }
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
    LOGI("Remote Address: %s Port: %d secured:%d\n", object->addressInfo.IP.ipAddress,
           object->addressInfo.IP.port, object->isSecured);

    LOGI("Data: %s\n", requestInfo->info.payload);

    if (gLastRequestToken != NULL && requestInfo->info.token != NULL
        && (strcmp((char *)gLastRequestToken, requestInfo->info.token) == 0))
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
        }
    }
    printf("############################################################\n");

    //Check if this has secure communication information
    if (requestInfo->info.payload)
    {
        int securePort = get_secure_information(requestInfo->info.payload);
        if (0 < securePort) //Set the remote endpoint secure details and send response
        {
            LOGI("This is secure resource...\n");
            char *uri = NULL;
            int length = 0;

            length = 8; //length of "coaps://"
            length += strlen(object->addressInfo.IP.ipAddress) + 5; // length of "ipaddress:port"
            length += strlen(object->resourceUri) + 1;

            uri = calloc(1,sizeof(char)*length);
            if (!uri)
            {
                printf("Failed to create new uri\n");
                return;
            }
            sprintf(uri,"coaps://%s:%d/%s",object->addressInfo.IP.ipAddress,
                      securePort, object->resourceUri);

            CARemoteEndpoint_t *endpoint = NULL;
            if (CA_STATUS_OK != CACreateRemoteEndpoint(uri, object->connectivityType, &endpoint))
            {
                LOGI("Failed to create duplicate of remote endpoint!\n");
                return;
            }
            endpoint->isSecured = CA_TRUE;
            object = endpoint;
        }
    }

    gReceived = 1;

}

void response_handler(const CARemoteEndpoint_t* object, const CAResponseInfo_t* responseInfo)
{

    LOGI("##########Received response from remote device #############\n");
    LOGI("Uri: %s\n", object->resourceUri);
    LOGI("Remote Address: %s Port: %d secured:%d\n", object->addressInfo.IP.ipAddress,
              object->addressInfo.IP.port, object->isSecured);
    LOGI("response result : %d\n", responseInfo->result);
    LOGI("Data: %s\n", responseInfo->info.payload);

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
        }
    }
    LOGI("############################################################\n");
    gReceived = 1;

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

void get_resource_uri(char *URI, char *resourceURI, int length)
{
    char *startPos = URI;
    char *temp = NULL;
    if (NULL != (temp = strstr(URI, "://")))
    {
        startPos = strchr(temp + 3, '/');
        if (!startPos)
        {
            printf("Resource URI is missing\n");
            return;
        }
    }

    char *endPos = strchr(startPos, '?');
    if (!endPos)
    {
        endPos = URI + strlen(URI);
    }
    endPos -= 1;

    if (endPos - startPos <= length)
        memcpy(resourceURI, startPos + 1, endPos - startPos);

    printf("URI: %s, ResourceURI:%s\n", URI, resourceURI);
}

int get_secure_information(CAPayload_t payLoad)
{
    printf("entering get_secure_information\n");

    if (!payLoad)
    {
        printf("Payload is NULL\n");
        return -1;
    }

    char *subString = NULL;
    if (NULL == (subString = strstr(payLoad, "\"sec\":1")))
    {
        printf("This is not secure resource\n");
        return -1;
    }

    if (NULL == (subString = strstr(payLoad, "\"port\":")))
    {
        printf("This secure resource does not have port information\n");
        return -1;
    }

    char *startPos = strstr(subString, ":");
    if (!startPos)
    {
        printf("Parsing failed !\n");
        return -1;
    }

    char *endPos = strstr(startPos, "}");
    if (!endPos)
    {
        printf("Parsing failed !\n");
        return -1;
    }

    char portStr[4] = {0};
    memcpy(portStr, startPos + 1, (endPos-1) - startPos);

    printf("secured port is: %s\n", portStr);
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
        gSelectedNwType = CA_ETHERNET;
        return CA_STATUS_OK;
    }
    if (number & CA_WIFI)
    {
        gSelectedNwType = CA_WIFI;
        return CA_STATUS_OK;
    }
    if (number & CA_EDR)
    {
        gSelectedNwType = CA_EDR;
        return CA_STATUS_OK;
    }
    if (number & CA_LE)
    {
        gSelectedNwType = CA_LE;
        return CA_STATUS_OK;
    }

    return CA_NOT_SUPPORTED;
}
