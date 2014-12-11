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

void request_handler(const CARemoteEndpoint_t* object, const CARequestInfo_t* requestInfo);
void response_handler(const CARemoteEndpoint_t* object, const CAResponseInfo_t* responseInfo);

static CAToken_t gLastRequestToken = NULL;

JNIEXPORT jint JNICALL Java_com_iotivity_service_RMInterface_RMInitialize
  (JNIEnv *env, jobject obj, jobject context)
{
    LOGI("RMInitialize");
    //Currently set context for WiFiCore
    CAJniSetContext(context);

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

JNIEXPORT jint JNICALL Java_com_iotivity_service_RMInterface_RMStartListeningServer(JNIEnv *env,
    jobject obj)
{
    LOGI("RMStartListeningServer");

    if(CA_STATUS_OK != CAStartListeningServer())
    {
        LOGI("Could not start Listening server");
    }
}

JNIEXPORT jint JNICALL Java_com_iotivity_service_RMInterface_RMStartDiscoveryServer(JNIEnv *env,
    jobject obj)
{
    LOGI("RMStartDiscoveryServer");

    if(CA_STATUS_OK != CAStartDiscoveryServer())
    {
        LOGI("Could not start discovery server");
    }
}

JNIEXPORT jint JNICALL Java_com_iotivity_service_RMInterface_RMRegisterHandler(JNIEnv *env,
    jobject obj)
{
    LOGI("RMRegisterHandler");

    if (CA_STATUS_OK != CARegisterHandler(request_handler, response_handler))
    {
        LOGI("Could not register handler");
    }
}

JNIEXPORT jint JNICALL Java_com_iotivity_service_RMInterface_RMFindResource(JNIEnv *env, jobject obj,
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

    if(CA_STATUS_OK != CAFindResource(strUri, token))
    {
        LOGI("Could not find resource");
    }
    else
    {
        LOGI("find resource to %s URI", strUri);
        gLastRequestToken = "";
    }
}

JNIEXPORT jint JNICALL Java_com_iotivity_service_RMInterface_RMSendRequest(JNIEnv *env, jobject obj,
    jstring data)
{
    const char* strData = (*env)->GetStringUTFChars(env, data, NULL);
    LOGI("RMSendRequest - %s", strData);

    // create token
    CAToken_t token = NULL;
    CAResult_t res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!\n");
        token = NULL;
    }

    CAInfo_t requestData;
    memset(&requestData, 0, sizeof(CAInfo_t));
    requestData.token = token;
    requestData.payload = "Temp Json Payload";

    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    //create remote endpoint
    CARemoteEndpoint_t* endpoint = NULL;

    if(CA_STATUS_OK != CACreateRemoteEndpoint(strData, &endpoint))
    {
        LOGI("Could not create remote end point");
    }

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

JNIEXPORT jint JNICALL Java_com_iotivity_service_RMInterface_RMSendResponse(JNIEnv *env, jobject obj,
    jstring data)
{
    const char* strData = (*env)->GetStringUTFChars(env, data, NULL);
    LOGI("RMSendResponse - %s", strData);

    const CAURI_t endpoint_uri = "/a/temp_uri";

    // create token
    CAToken_t token = NULL;
    CAResult_t res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!\n");
        token = NULL;
    }

    CAInfo_t responseData;
    memset(&responseData, 0, sizeof(CAInfo_t));
    responseData.token = token;
    responseData.payload = (char*) strData;

    CAResponseInfo_t responseInfo;
    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
    responseInfo.result = 203;
    responseInfo.info = responseData;

    CARemoteEndpoint_t* endpoint = NULL;

    if(CA_STATUS_OK != CACreateRemoteEndpoint(endpoint_uri, &endpoint))
    {
        LOGI("Could not create remote end point");
    }

    if(CA_STATUS_OK != CASendResponse(endpoint, &responseInfo))
    {
        LOGI("Could not send response");
    }
}

JNIEXPORT jint JNICALL Java_com_iotivity_service_RMInterface_RMSendNotification(JNIEnv *env,
    jobject obj, jstring data)
{
    const char* strData = (*env)->GetStringUTFChars(env, data, NULL);
    LOGI("RMSendNotification - %s", strData);

    const CAURI_t endpoint_uri = "/a/temp_uri";

    // create token
    CAToken_t token = NULL;
    CAResult_t res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!\n");
        token = NULL;
    }

    CAInfo_t respondeData;
    memset(&respondeData, 0, sizeof(CAInfo_t));
    respondeData.token = token;
    respondeData.payload = (char*)strData;

    CAResponseInfo_t responseInfo;
    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
    responseInfo.result = CA_SUCCESS;
    responseInfo.info = respondeData;

    CARemoteEndpoint_t* endpoint = NULL;

    if(CA_STATUS_OK != CACreateRemoteEndpoint(endpoint_uri, &endpoint))
    {
        LOGI("Could not create remote end point");
    }

    if(CA_STATUS_OK != CASendNotification(endpoint, &responseInfo))
    {
        LOGI("Could not send notification");
    }
}

JNIEXPORT jint JNICALL Java_com_iotivity_service_RMInterface_RMSelectNetwork(JNIEnv *env, jobject obj,
    jint networkType)
{
    LOGI("RMSelectNetwork Type : %d", networkType);

    if(CA_STATUS_OK != CASelectNetwork(networkType))
    {
        LOGI("Could not select network");
    }
}

JNIEXPORT jint JNICALL Java_com_iotivity_service_RMInterface_RMHandleRequestResponse(JNIEnv *env,
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
    /*
    printf("[CALLBACK] request_handler, uri : %s, data : %s\n",
            (object != NULL) ? object->resourceUri : "",
            (requestInfo != NULL) ? requestInfo->info.payload : "");
    */
    LOGI("[CALLBACK] request_handler, uri: %s, data: %s, token: %s \n",
            (object != NULL) ? object->resourceUri : "",
            (requestInfo != NULL) ? requestInfo->info.payload : "",
            (requestInfo->info.token != NULL) ? requestInfo->info.token : "");

    if (gLastRequestToken != NULL && requestInfo->info.token != NULL
        && (strcmp((char*)gLastRequestToken, requestInfo->info.token) == 0))
    {
        printf("token is same. received request of it's own. skip.. \n");

        return;
    }

    LOGI("[CALLBACK] request_handler, address : %s\n",
            (object != NULL) ? object->addressInfo.IP.ipAddress : "");

    if (requestInfo->info.options)
    {
        uint32_t len = requestInfo->info.numOptions;
        uint32_t i;
        for (i = 0; i < len; i++)
        {
            LOGI("[CALLBACK] request_handler, option ID : %d\n",
                    requestInfo->info.options[i].optionID);
            LOGI("[CALLBACK] request_handler, options data length : %d\n",
                    requestInfo->info.options[i].optionLength);
            LOGI("[CALLBACK] request_handler, options data : %s\n",
                    requestInfo->info.options[i].optionData);
        }
    }

    printf("send response with URI\n");

    CAInfo_t responseData;
    //responseData = (CAInfo*) malloc(sizeof(CAInfo));
    memset(&responseData, 0, sizeof(CAInfo_t));
    if (requestInfo != NULL)
    {
        responseData.token = requestInfo->info.token;
    }
    else
    {
        responseData.token = "";
    }
    responseData.payload = "response payload";

    CAResponseInfo_t responseInfo;
    //responseInfo = (CAResponseInfo*) malloc(sizeof(CAResponseInfo));
    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
    responseInfo.result = 203;
    responseInfo.info = responseData;

    // send request (connectivityType from remoteEndpoint of request Info)
    CAResult_t res = CASendResponse(object, &responseInfo);
    if (res != CA_STATUS_OK)
    {
        LOGI("send response error\n");
    }
    else
    {
        LOGI("send response success\n");
    }

}

void response_handler(const CARemoteEndpoint_t* object, const CAResponseInfo_t* responseInfo)
{

    LOGI("[CALLBACK] response_handler, uri : %s, data : %s\n",
            (object != NULL) ? object->resourceUri : "",
            (responseInfo != NULL) ? responseInfo->info.payload : "");

    LOGI("[CALLBACK] response_handler, address : %s\n",
            (object != NULL) ? object->addressInfo.IP.ipAddress : "");

    if (responseInfo->info.options)
    {
        uint32_t len = responseInfo->info.numOptions;
        uint32_t i;
        for (i = 0; i < len; i++)
        {
            LOGI("[CALLBACK] response_handler, option ID : %d\n",
                    responseInfo->info.options[i].optionID);
            LOGI("[CALLBACK] response_handler, options data length : %d\n",
                    responseInfo->info.options[i].optionLength);
            LOGI("[CALLBACK] response_handler, options data : %s\n",
                    responseInfo->info.options[i].optionData);
        }
    }

    //printf("send request with URI\n");
    //send_request_tmp(object, (responseInfo != NULL) ? responseInfo->info.token : "");
}
