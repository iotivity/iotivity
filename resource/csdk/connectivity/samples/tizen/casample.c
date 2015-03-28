/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "cacommon.h"
#include "cainterface.h"

static GMainLoop *mainloop = NULL;

pthread_t thread;

#define MAX_BUF_LEN 1024
#define MAX_OPT_LEN 16


/**
 * @def RS_IDENTITY
 * @brief
 */
#define IDENTITY     ("1111111111111111")
/* @def RS_CLIENT_PSK
 * @brief
 */
#define RS_CLIENT_PSK   ("AAAAAAAAAAAAAAAA")


int g_received = 0;
int g_localUnicastPort = 0;
int g_localSecurePort = 0;
CAConnectivityType_t g_selectedNwType = CA_ETHERNET;
const char* g_messageType[] = {"CON", "NON", "ACK", "RESET"};

char PrintOptions();
void Process();
CAResult_t GetNetworkType();

CAResult_t Initialize();
void StartListeningServer();
void StartDiscoveryServer();
void FindResource();
void SendRequest();
void SendRequestAll();
void SendResponse();
void AdvertiseResource();
void SendNotification();
void SelectNetwork();
void UnselectNetwork();
void HandleRequestResponse();
void FindFixedResource();
void GetNetworkInfo();

void RequestHandler(const CARemoteEndpoint_t *object, const CARequestInfo_t *requestInfo);
void ResponseHandler(const CARemoteEndpoint_t *object, const CAResponseInfo_t *responseInfo);
void SendRequestTemp(CARemoteEndpoint_t *endpoint, CAToken_t token);
void GetResourceUri(char *URI, char *resourceURI, int length);
int GetSecureInformation(CAPayload_t payLoad);

static CAToken_t g_lastRequestToken = NULL;
static const char g_secureInfoData[] = "{\"oc\":[{\"href\":\"%s\",\"prop\":{\"rt\":[\"core.led\"],"
                                     "\"if\":[\"oc.mi.def\"],\"obs\":1,\"sec\":1,\"port\":%d}}]}";
static const char g_normalInfoData[] = "{\"oc\":[{\"href\":\"%s\",\"prop\":{\"rt\":[\"core.led\"],"
                                     "\"if\":[\"oc.mi.def\"],\"obs\":1}}]}";

#ifdef __WITH_DTLS__
static OCDtlsPskCredsBlob *pskCredsBlob = NULL;

void ClearDtlsCredentialInfo()
{
    printf("clearDtlsCredentialInfo IN\n");
    if (pskCredsBlob)
    {
        // Initialize sensitive data to zeroes before freeing.
        memset(pskCredsBlob->creds, 0, sizeof(OCDtlsPskCreds) * (pskCredsBlob->num));
        free(pskCredsBlob->creds);

        memset(pskCredsBlob, 0, sizeof(OCDtlsPskCredsBlob));
        free(pskCredsBlob);
        pskCredsBlob = NULL;
    }
    printf("clearDtlsCredentialInfo OUT\n");
}

// Internal API. Invoked by OC stack to retrieve credentials from this module
void CAGetDtlsPskCredentials(OCDtlsPskCredsBlob **credInfo)
{
    printf("CAGetDtlsPskCredentials IN\n");

    if(!credInfo)
    {
        printf("Memory is not allocated for credInfo\n");
        return;
    }

    *credInfo = pskCredsBlob;

    printf("CAGetDtlsPskCredentials OUT\n");
}

bool SetCredentials()
{
    printf("SetCredentials IN\n");
    pskCredsBlob = (OCDtlsPskCredsBlob *)malloc(sizeof(OCDtlsPskCredsBlob));

    if(!pskCredsBlob)
    {
        printf("Memory allocation is failed\n");
        return 0;
    }

    memset(pskCredsBlob, 0x0, sizeof(OCDtlsPskCredsBlob));
    memcpy(pskCredsBlob->identity, IDENTITY, DTLS_PSK_ID_LEN);

    pskCredsBlob->num = 1;

    pskCredsBlob->creds = (OCDtlsPskCreds *)malloc(sizeof(OCDtlsPskCreds) * (pskCredsBlob->num));

    if(!pskCredsBlob->creds)
    {
        printf("Memory allocation is failed\n");
        return 0;
    }


    memcpy(pskCredsBlob->creds[0].id, IDENTITY, DTLS_PSK_ID_LEN);
    memcpy(pskCredsBlob->creds[0].psk, RS_CLIENT_PSK, DTLS_PSK_PSK_LEN);

    printf("SetCredentials OUT\n");
    return 1;
}
#endif

void pthread_func()
{
    g_main_loop_run(mainloop);
}

int main()
{
    system("clear");

    printf("\n=============================================\n");
    printf("\t\tsample main\n");
    printf("=============================================\n");

    CAResult_t res = CA_STATUS_FAILED;
    /*
    * Read DTLS PSK credentials from persistent storage and
    * set in the OC stack.
    */
#ifdef __WITH_DTLS__
    if (SetCredentials() == 0)
    {
        printf("SetCredentials failed\n");
        return 1;
    }

    res = CARegisterDTLSCredentialsHandler(CAGetDtlsPskCredentials);
    if(CA_STATUS_OK != res)
    {
        printf("Set credential handler fail\n");
        return 1;
    }
#endif
    res = Initialize();
    if (CA_STATUS_OK != res)
    {
        printf("CAInitialize fail\n");
        return 1;
    }

    // set handler.
    CARegisterHandler(RequestHandler, ResponseHandler);

    Process();

    if (g_lastRequestToken != NULL)
    {
        CADestroyToken(g_lastRequestToken);
    }
    g_lastRequestToken = NULL;

    CATerminate();
#ifdef __WITH_DTLS__
    ClearDtlsCredentialInfo();
#endif
    return 0;
}

void Process()
{
    while (1)
    {
        char menu = toupper(PrintOptions());

        switch (menu)
        {
            case 'm': // menu
            case 'M':
                break;

            case 'Q': // quits the sample program
                printf("quit..!!\n");
                g_main_loop_quit(mainloop);
                return;

            case 's': // start server
            case 'S': // start server
                StartListeningServer();
                break;

            case 't': // send request
            case 'T':
                SendRequestAll();
                break;

            case 'c': // start client
            case 'C':
                StartDiscoveryServer();
                break;

            case 'f': // find resource
            case 'F':
                FindResource();
                break;

            case 'r': // send request
            case 'R':
                SendRequest();
                break;

            case 'a': // advertise resource
            case 'A':
                AdvertiseResource();
                break;

            case 'b': // send notification
            case 'B':
                SendNotification();
                break;

            case 'n': // select network
            case 'N':
                SelectNetwork();
                break;

            case 'x': // unselect network
            case 'X':
                UnselectNetwork();
                break;

            case 'h': // handle request response
            case 'H':
                HandleRequestResponse();
                break;

            case 'y':
            case 'Y':
                while (1)
                {
                    g_received = 0;
                    FindFixedResource();
                    while (g_received == 0)
                    {
                        sleep(1);
                        HandleRequestResponse();

                    }
                }
                break;
            case 'z':
            case 'Z':
                StartListeningServer();
                while (1)
                {
                    sleep(1);
                    HandleRequestResponse();
                }
                break;

            case 'g': // get network information
            case 'G':
                GetNetworkInfo();
                break;

            default:
                printf("not supported menu!!\n");
                break;
        }
    }

}

CAResult_t Initialize()
{
    CAResult_t res = CA_STATUS_FAILED;
    mainloop = g_main_loop_new(NULL, FALSE);
    if(!mainloop)
    {
        printf("g_main_loop_new failed\n");
        return res;
    }

    int result = pthread_create(&thread, NULL, (void *) &pthread_func, NULL);
    if (result < 0)
    {
        printf("pthread_create failed in initialize\n");
        return res;
    }

    res = CAInitialize();
    if (res != CA_STATUS_OK)
    {
        printf("CAInitialize fail\n");
    }
    return res;
}

void StartListeningServer()
{
    printf("start listening server!!\n");

    CAResult_t res = CAStartListeningServer();
    if (res != CA_STATUS_OK)
    {
        printf("start listening server fail\n");
    }
    else
    {
        printf("start listening server success\n");
    }
}

void StartDiscoveryServer()
{
    printf("start discovery client!!\n");

    CAResult_t res = CAStartDiscoveryServer();
    if (res != CA_STATUS_OK)
    {
        printf("start discovery client fail\n");
    }
    else
    {
        printf("start discovery client success\n");
    }
}

void FindFixedResource()
{
    char buf[] ="/a/light";

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    CAResult_t res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        printf("token generate error!!");
        return;
    }

    printf("Generated token %s\n", token);

    res = CAFindResource(buf, token, tokenLength);
    if (res != CA_STATUS_OK)
    {
        printf("find resource error : %d\n", res);
    }
    else
    {
        printf("find resource to %s URI\n", buf);
    }

    // delete token
    CADestroyToken(token);

    printf("=============================================\n");
}

void FindResource()
{
    char buf[MAX_BUF_LEN] = {0};

    printf("\n=============================================\n");
    printf("ex) /a/light\n");
    printf("reference uri : ");

    if(!fgets(buf, MAX_BUF_LEN, stdin))
    {
        printf("fgets error\n");
        return;
    }

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    CAResult_t res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        printf("token generate error!!\n");
        return;
    }

    printf("Generated token %s\n", token);

    res = CAFindResource(buf, token, tokenLength);
    if (res != CA_STATUS_OK)
    {
        printf("find resource error : %d\n", res);
        CADestroyToken(token);
    }
    else
    {
        printf("find resource to %s URI\n", buf);

        CADestroyToken(g_lastRequestToken);

        g_lastRequestToken = token;
    }

    printf("=============================================\n");
}

void SendRequest()
{
    char secureRequest[2] = {0};
    CAResult_t res = CA_STATUS_FAILED;
    char uri[MAX_BUF_LEN] = {'\0'};

    res = GetNetworkType();
    if (res != CA_STATUS_OK)
    {
        return;
    }

    printf("Do you want to send secure request ?.... enter (0/1): \n");
    if(!fgets(secureRequest, 1, stdin))
    {
        printf("fgets error\n");
        return;
    }

    if ('1' == secureRequest[0])
    {
        printf("Enter the URI like below....\n");
        printf("coaps://10.11.12.13:4545/resource_uri ( for IP secure)\n");
        printf("uri:  ");
    }
    else
    {
        printf("Enter the URI like below....\n");
        printf("coap://10.11.12.13:4545/resource_uri ( for IP )\n");
        printf("coap://10:11:12:13:45:45/resource_uri ( for BT )\n");
        printf("uri:  ");
    }

    if(!fgets(uri, MAX_BUF_LEN, stdin))
    {
        printf("fgets error\n");
        return;
    }

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    if (CA_STATUS_OK != CACreateRemoteEndpoint(uri, g_selectedNwType, &endpoint)
        || !endpoint)
    {
        printf("Failed to create remote endpoint!\n");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    char buf[MAX_BUF_LEN]={0};

    printf("\n=============================================\n");
    printf("0:CON, 1:NON\n");
    printf("select message type : ");

    if(!fgets(buf, MAX_BUF_LEN, stdin))
    {
        printf("fgets error\n");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    CAMessageType_t msgType = (buf[0] == '0' || buf[0] == '1') ? buf[0] - '0' : 0;

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        printf("token generate error, error code : %d\n", res);
        return;
    }

    printf("Generated token %s\n", token);

    // extract relative resourceuri from give uri
    char resourceURI[15] = {0};

    printf("URI : %s\n", uri);
    GetResourceUri(uri, resourceURI, 14);

    // create request data
    CAInfo_t requestData = {CA_MSG_RESET};
    requestData.token = token;
    if ('1' == secureRequest[0])
    {
        int length = strlen(g_secureInfoData) + strlen(resourceURI) + 1;
        requestData.payload = (CAPayload_t) malloc(length);
        if(!requestData.payload)
        {
            printf("Memory allocation is failed\n");
            CADestroyToken(token);
            CADestroyRemoteEndpoint(endpoint);
            return;
        }
        sprintf(requestData.payload, g_secureInfoData, resourceURI, g_localSecurePort);
    }
    else
    {
        int length = strlen(g_normalInfoData) + strlen(resourceURI) + 1;
        requestData.payload = (CAPayload_t) malloc(length);
        if(!requestData.payload)
        {
            printf("Memory allocation is failed\n");
            CADestroyToken(token);
            CADestroyRemoteEndpoint(endpoint);
            return;
        }
        sprintf(requestData.payload, g_normalInfoData, resourceURI);
    }
    requestData.type = msgType;

    CARequestInfo_t requestInfo = {CA_GET, {CA_MSG_RESET}};
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    CASendRequest(endpoint, &requestInfo);

    CADestroyToken(token);

    free(requestData.payload);

    // destroy remote endpoint
    CADestroyRemoteEndpoint(endpoint);
    printf("=============================================\n");
}

void SendRequestAll()
{
    char buf[MAX_BUF_LEN] = {0};

    CAResult_t res = CA_STATUS_FAILED;

    res = GetNetworkType();
    if (res != CA_STATUS_OK)
    {
        return;
    }

    printf("\n=============================================\n");
    printf("10.11.12.13:4545/resource_uri ( for IP )\n");
    printf("10:11:12:13:45:45/resource_uri ( for BT )\n");
    printf("uri : ");

    if(!fgets(buf, MAX_BUF_LEN, stdin))
    {
        printf("fgets error\n");
        return;
    }

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    res = CACreateRemoteEndpoint(buf, g_selectedNwType, &endpoint);

    if (res != CA_STATUS_OK)
    {
        printf("create remote endpoint error!!\n");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    CAGroupEndpoint_t *group = NULL;
    group = (CAGroupEndpoint_t *)malloc(sizeof(CAGroupEndpoint_t));
     if (NULL == group)
    {
        printf("Memory allocation failed!\n");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }
    group->connectivityType = endpoint->connectivityType;
    group->resourceUri = endpoint->resourceUri;

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        printf("token generate error!!\n");
        return;
    }

    printf("generated token %s\n", token);

    CAInfo_t requestData = {CA_MSG_RESET};
    requestData.token = token;
    requestData.tokenLength = tokenLength;
    requestData.payload = "Temp Json Payload";
    requestData.type = CA_MSG_NONCONFIRM;

    CARequestInfo_t requestInfo = {CA_GET, {CA_MSG_RESET}};
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    CASendRequestToAll(group, &requestInfo);

    CADestroyToken(token);

    // destroy remote endpoint
    CADestroyRemoteEndpoint(endpoint);

    free(group);

    printf("=============================================\n");
}

void AdvertiseResource()
{
    char buf[MAX_BUF_LEN] = {0};

    printf("\n=============================================\n");
    printf("uri : ");

    if(!fgets(buf, MAX_BUF_LEN, stdin))
    {
        printf("fgets error\n");
        return;
    }

    int optionNum = 0;
    char optionData[MAX_OPT_LEN] = {0};

    printf("Option Num : ");
    scanf("%d", &optionNum);
    CAHeaderOption_t *headerOpt = NULL;
    headerOpt = (CAHeaderOption_t *) malloc(sizeof(CAHeaderOption_t) * optionNum);
    if (NULL == headerOpt)
    {
        printf("Memory allocation failed!\n");
        return;
    }
    memset(headerOpt, 0, sizeof(CAHeaderOption_t) * optionNum);

    int i = 0;
    for (i = 0 ; i < optionNum ; i++)
    {
        int optionID = 0;
        printf("[%d] Option ID : ", i + 1);
        scanf("%d", &optionID);
        headerOpt[i].optionID = optionID;

        printf("[%d] Option Data : ", i + 1);
        scanf("%s", optionData);
        int len = strlen(optionData);
        memcpy(headerOpt[i].optionData, optionData, len + 1);
        printf("[%d] inputed option : ID : %d, data : %s\n", i + 1, optionID, optionData );

        headerOpt[i].optionLength = (uint16_t)len;
    }
    printf("\n=============================================\n");

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    CAResult_t res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        printf("token generate error!!\n");
        return;
    }

    printf("generated token %s\n", token);

    CAAdvertiseResource(buf, token, tokenLength, headerOpt, (uint8_t)optionNum);

    // delete token
    CADestroyToken(token);

    free(headerOpt);
}

void SendNotification()
{
    char buf[MAX_BUF_LEN] = {0};

    CAResult_t res = CA_STATUS_FAILED;

    res = GetNetworkType();
    if (res != CA_STATUS_OK)
    {
        return;
    }

    printf("\n=============================================\n");
    printf("10.11.12.13:4545/resource_uri ( for IP )\n");
    printf("10:11:12:13:45:45/resource_uri ( for BT )\n");
    printf("uri : ");

     if(!fgets(buf, MAX_BUF_LEN, stdin))
    {
        printf("fgets error\n");
        return;
    }

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    res = CACreateRemoteEndpoint(buf, g_selectedNwType, &endpoint);
    if (res != CA_STATUS_OK)
    {
        printf("create remote endpoint error!!\n");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    CAInfo_t respondData = {CA_MSG_RESET};
    respondData.tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&respondData.token, respondData.tokenLength);
    if ((CA_STATUS_OK != res) || (!respondData.token))
    {
        printf("token generate error!!\n");
        return;
    }

    respondData.payload = "Temp Notification Data";

    CAResponseInfo_t responseInfo = {0};
    responseInfo.result = CA_SUCCESS;
    responseInfo.info = respondData;

    // send request
    res = CASendNotification(endpoint, &responseInfo);
    if (res != CA_STATUS_OK)
    {
        printf("send notification error, error code: %d\n", res);
    }
    else
    {
        printf("send notification success\n");
    }

    // delete token
    CADestroyToken(respondData.token);
    // destroy remote endpoint
    CADestroyRemoteEndpoint(endpoint);

    printf("\n=============================================\n");
}
void SelectNetwork()
{
    char buf[MAX_BUF_LEN] = {0};

    printf("\n=============================================\n");
    printf("\tselect network\n");
    printf("ETHERNET : 0\n");
    printf("WIFI : 1\n");
    printf("EDR : 2\n");
    printf("LE : 3\n");
    printf("select : ");

    if(!fgets(buf, MAX_BUF_LEN, stdin))
    {
        printf("fgets error\n");
        return;
    }

    int number = buf[0] - '0';

    number = (number < 0 || number > 3) ? 0 : number;

    CAResult_t res = CASelectNetwork(1 << number);
    if (res != CA_STATUS_OK)
    {
        printf("select network error\n");
        g_selectedNwType = 1 << number;
    }
    else
    {
        printf("select network success\n");
    }

    printf("=============================================\n");
}

void UnselectNetwork()
{
    char buf[MAX_BUF_LEN] = {0};

    printf("\n=============================================\n");
    printf("\tunselect enabled network\n");
    printf("ETHERNET : 0\n");
    printf("WIFI : 1\n");
    printf("EDR : 2\n");
    printf("LE : 3\n");
    printf("select : ");

    if(!fgets(buf, MAX_BUF_LEN, stdin))
    {
        printf("fgets error\n");
        return;
    }

    int number = buf[0] - '0';

    number = (number < 0 || number > 3) ? 1 : number;

    CAResult_t res = CAUnSelectNetwork(1 << number);
    if (res != CA_STATUS_OK)
    {
        printf("unselect network error\n");
    }
    else
    {
        printf("unselect network success\n");
    }
    printf("=============================================\n");
}

char PrintOptions()
{
    char buf[MAX_BUF_LEN] = {0};

    printf("\n=============================================\n");
    printf("\t\tMenu\n");
    printf("\tm : show menu\n");
    printf("\ts : start server\n");
    printf("\tc : start client\n");
    printf("\tf : find resource\n");
    printf("\tr : send request\n");
    printf("\tt : send request to all\n");
    printf("\ta : advertise resource\n");
    printf("\tb : send notification\n");
    printf("\tn : select network\n");
    printf("\tx : unselect network\n");
    printf("\tg : get network information\n");
    printf("\th : handle request response\n");
    printf("\ty : run static client\n");
    printf("\tz : run static server\n");
    printf("\tq : quit\n");
    printf("=============================================\n");
    printf("select : ");

    if(!fgets(buf, MAX_BUF_LEN, stdin))
    {
        printf("fgets error\n");
        return '\0';
    }

    return buf[0];
}

void HandleRequestResponse()
{
    printf("handle_request_response\n");

    CAResult_t res = CAHandleRequestResponse();
    if (res != CA_STATUS_OK)
    {
        printf("handle request error\n");
    }
    else
    {
        printf("handle request success\n");
    }
}

void GetNetworkInfo()
{
    int index = 0;
    CALocalConnectivity_t *tempInfo = NULL;
    uint32_t tempSize = 0;

    CAGetNetworkInformation(&tempInfo, &tempSize);
    if (tempInfo == NULL || tempSize <= 0)
    {
        printf("network not connected\n");
        return;
    }

    printf("################## Network Information #######################\n");
    printf("network info total size is %d\n\n", tempSize);

    for (index = 0; index < tempSize; index++)
    {
        printf("Type: %d\n", tempInfo[index].type);
        printf("Address: %s\n", tempInfo[index].addressInfo.IP.ipAddress);
        printf("Port: %d\n", tempInfo[index].addressInfo.IP.port);
        printf("Secured: %d\n\n", tempInfo[index].isSecured);

        if (true == tempInfo[index].isSecured)
        {
            g_localSecurePort = tempInfo[index].addressInfo.IP.port;
        }
        else
        {
            g_localUnicastPort = tempInfo[index].addressInfo.IP.port;
        }
    }

    // free
    free(tempInfo);

    printf("##############################################################");
}

void RequestHandler(const CARemoteEndpoint_t *object, const CARequestInfo_t *requestInfo)
{
    printf("##########received request from remote device #############\n");
    if (!object)
    {
        printf("Remote endpoint is NULL!\n");
        return;
    }

    if (!requestInfo)
    {
        printf("Request info is NULL!\n");
        return;
    }

    printf("Uri: %s\n", object->resourceUri);
    printf("Remote Address: %s Port: %d secured:%d\n", object->addressInfo.IP.ipAddress,
           object->addressInfo.IP.port, object->isSecured);

    printf("Data: %s\n", requestInfo->info.payload);
    printf("Message type: %s\n", g_messageType[requestInfo->info.type]);
    if (g_lastRequestToken != NULL && requestInfo->info.token != NULL
        && (strcmp((char *)g_lastRequestToken, requestInfo->info.token) == 0))
    {
        printf("token is same. received request of it's own. skip.. \n");
        return;
    }

    if (requestInfo->info.options)
    {
        uint32_t len = requestInfo->info.numOptions;
        uint32_t i = 0;
        for (i = 0; i < len; i++)
        {
            printf("Option %d\n", i + 1);
            printf("ID : %d\n", requestInfo->info.options[i].optionID);
            printf("Data[%d]: %s\n", requestInfo->info.options[i].optionLength,
                   requestInfo->info.options[i].optionData);
        }
    }
    printf("############################################################\n");

    //Check if this has secure communication information
    if (requestInfo->info.payload)
    {
        int securePort = GetSecureInformation(requestInfo->info.payload);
        if (0 < securePort) //Set the remote endpoint secure details and send response
        {
            printf("This is secure resource...\n");
            char *uri = NULL;
            int length = 0;

            length = 8; //length of "coaps://"
            length += strlen(object->addressInfo.IP.ipAddress) + 5; // length of "ipaddress:port"
            length += strlen(object->resourceUri) + 1;

            uri = calloc(1, sizeof(char) * length);
            if (!uri)
            {
                printf("Failed to create new uri\n");
                return;
            }
            sprintf(uri, "coaps://%s:%d/%s", object->addressInfo.IP.ipAddress,
                    securePort, object->resourceUri);

            CARemoteEndpoint_t *endpoint = NULL;
            if (CA_STATUS_OK != CACreateRemoteEndpoint(uri, object->connectivityType, &endpoint))
            {
                printf("Failed to create duplicate of remote endpoint!\n");
                return;
            }
            //endpoint->connectivityType = object->connectivityType;
            endpoint->isSecured = true;
            object = endpoint;

            free(uri);
        }
    }

    printf("send response with URI\n");
    SendResponse(object, &requestInfo->info);

    g_received = 1;
}

void ResponseHandler(const CARemoteEndpoint_t *object, const CAResponseInfo_t *responseInfo)
{
    printf("##########Received response from remote device #############\n");
    if(!object)
    {
        printf("response_handler object is null\n");
        return;
    }

     if(!responseInfo)
    {
        printf("response_handler responseInfo is null\n");
        return;
    }

    printf("Uri: %s\n", object->resourceUri);
    printf("Remote Address: %s Port: %d secured:%d\n", object->addressInfo.IP.ipAddress,
           object->addressInfo.IP.port, object->isSecured);
    printf("response result : %d\n", responseInfo->result);
    printf("Data: %s\n", responseInfo->info.payload);
    printf("Message type: %s\n", g_messageType[responseInfo->info.type]);
    printf("Token: %s\n", responseInfo->info.token);
    if (responseInfo->info.options)
    {
        uint32_t len = responseInfo->info.numOptions;
        uint32_t i = 0;
        for (i = 0; i < len; i++)
        {
            printf("Option %d\n", i + 1);
            printf("ID : %d\n", responseInfo->info.options[i].optionID);
            printf("Data[%d]: %s\n", responseInfo->info.options[i].optionLength,
                   responseInfo->info.options[i].optionData);
        }
    }
    printf("############################################################\n");
    g_received = 1;

    //Check if this has secure communication information
    if (responseInfo->info.payload)
    {
        int securePort = GetSecureInformation(responseInfo->info.payload);
        if (0 < securePort) //Set the remote endpoint secure details and send response
        {
            printf("This is secure resource...\n");
        }
    }
}

void SendResponse(CARemoteEndpoint_t *endpoint, CAInfo_t *info)
{
    printf("entering send_response\n");

     if(!endpoint)
    {
        printf("endpoint is null\n");
        return;
    }

    CAInfo_t responseData = {CA_MSG_RESET};
    responseData.type =
        (info != NULL) ?
        ((info->type == CA_MSG_CONFIRM) ? CA_MSG_ACKNOWLEDGE : CA_MSG_NONCONFIRM) :
            CA_MSG_NONCONFIRM;
    responseData.messageId = 0;
    responseData.token = "";
    if(info)
    {
        responseData.messageId = info->messageId;
        responseData.token = info->token;
        responseData.tokenLength = info->tokenLength;
    }
    responseData.payload = "response payload";

    CAResponseInfo_t responseInfo = {0};
    responseInfo.result = 203;
    responseInfo.info = responseData;

    if (true == endpoint->isSecured)
    {
        printf("Sending response on secure communication\n");
    }
    else
    {
        printf("Sending response on non-secure communication\n");
    }

    // send request (connectivityType from remoteEndpoint of request Info)
    CAResult_t res = CASendResponse(endpoint, &responseInfo);
    if (res != CA_STATUS_OK)
    {
        printf("send response failed\n");
    }
    else
    {
        printf("send response success\n");
    }

    printf("=============================================\n");
}

void SendRequestTemp(CARemoteEndpoint_t *endpoint, CAToken_t token, uint8_t tokenLength)
{
    printf("\n=============================================\n");

    if(!endpoint)
    {
        printf("endpoint is null\n");
        return;
    }

    CAInfo_t requestData ={CA_MSG_RESET};
    requestData.token = token;
    requestData.tokenLength = tokenLength;
    requestData.payload = "Temp Json Payload";

    CARequestInfo_t requestInfo ={CA_GET, {CA_MSG_RESET}};
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    endpoint->connectivityType = CA_WIFI;

    CAResult_t res = CASendRequest(endpoint, &requestInfo);
    if (res != CA_STATUS_OK)
    {
        printf("send request failed\n");
    }
    else
    {
        printf("send request success\n");
    }

    printf("=============================================\n");

}

int GetSecureInformation(CAPayload_t payLoad)
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

    if(((endPos - 1) - startPos) > 4)
    {
        printf("port length is not proper.Exceeding length 4\n");
        return -1;
    }

    char portStr[4] = {0};
    memcpy(portStr, startPos + 1, (endPos - 1) - startPos);

    printf("secured port is: %s\n", portStr);
    return atoi(portStr);
}

void GetResourceUri(char *URI, char *resourceURI, int length)
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

CAResult_t GetNetworkType()
{
    char buf[MAX_BUF_LEN] = {0};

    printf("\n=============================================\n");
    printf("\tselect network type\n");
    printf("ETHERNET : 0\n");
    printf("WIFI : 1\n");
    printf("EDR : 2\n");
    printf("LE : 3\n");
    printf("select : ");

    if(!fgets(buf, MAX_BUF_LEN, stdin))
    {
        printf("fgets error\n");
        return CA_STATUS_FAILED;
    }

    int number = buf[0] - '0';

    number = (number < 0 || number > 3) ? 0 : 1 << number;

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

    printf("\n=============================================\n");

    return CA_STATUS_FAILED;
}


