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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "cacommon.h"
#include "cainterface.h"
#include "ocsecurityconfig.h"


#define MAX_BUF_LEN 1024
#define MAX_OPT_LEN 16
#define TRUE 1
#define FALSE 0

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
CAConnectivityType_t gSelectedNwType = CA_ETHERNET;

char get_menu();
void process();
CAResult_t get_network_type();

void start_listening_server();
void start_discovery_server();
void find_resource();
void send_request();
void send_request_all();
void send_response();
void advertise_resource();
void send_notification();
void select_network();
void unselect_network();
void handle_request_response();
void find_fixed_resource();
void get_network_info();
void send_secure_request();

void request_handler(const CARemoteEndpoint_t *object, const CARequestInfo_t *requestInfo);
void response_handler(const CARemoteEndpoint_t *object, const CAResponseInfo_t *responseInfo);
void send_request_tmp(CARemoteEndpoint_t *endpoint, CAToken_t token);
void get_resource_uri(char *URI, char *resourceURI, int length);
int get_secure_information(CAPayload_t payLoad);

static CAToken_t gLastRequestToken = NULL;
static const char *gSecureInfoData = "{\"oc\":[{\"href\":\"%s\",\"prop\":{\"rt\":[\"core.led\"],"
                                     "\"if\":[\"oc.mi.def\"],\"obs\":1,\"sec\":1,\"port\":%d}}]}";
static const char *gNormalInfoData = "{\"oc\":[{\"href\":\"%s\",\"prop\":{\"rt\":[\"core.led\"],"
                                     "\"if\":[\"oc.mi.def\"],\"obs\":1}}]}";

static OCDtlsPskCredsBlob *pskCredsBlob = NULL;

void clearDtlsCredentialInfo()
{
    printf("clearDtlsCredentialInfo IN\n");
    if (pskCredsBlob)
    {
        // Zero out sensitive data before freeing.
        if (pskCredsBlob->num)
        {
            memset(pskCredsBlob, 0,
                sizeof(OCDtlsPskCredsBlob) + (sizeof(OCDtlsPskCreds)*(pskCredsBlob->num - 1)));
        }
        free(pskCredsBlob);
        pskCredsBlob = NULL;
    }
    printf("clearDtlsCredentialInfo OUT\n");
}

// Internal API. Invoked by OC stack to retrieve credentials from this module
void OCGetDtlsPskCredentials(OCDtlsPskCredsBlob **credInfo)
{
    printf("OCGetDtlsPskCredentials IN\n");

    *credInfo = pskCredsBlob;

    printf("OCGetDtlsPskCredentials OUT\n");
}

int32_t SetCredentials()
{
    int32_t ret = 0;
    printf("SetCredentials IN\n");
    pskCredsBlob = (OCDtlsPskCredsBlob *)malloc(sizeof(OCDtlsPskCredsBlob));

    if (pskCredsBlob)
    {
        memset(pskCredsBlob, 0x0, sizeof(OCDtlsPskCredsBlob));

        pskCredsBlob->blobVer = DtlsPskCredsBlobVer_CurrentVersion;

        memcpy(pskCredsBlob->identity, IDENTITY, DTLS_PSK_ID_LEN);

        pskCredsBlob->num = 1;

        memcpy(pskCredsBlob->creds[0].id, IDENTITY, DTLS_PSK_ID_LEN);
        memcpy(pskCredsBlob->creds[0].psk, RS_CLIENT_PSK, DTLS_PSK_PSK_LEN);

        ret = 1;
    }

    printf("SetCredentials OUT\n");
    return ret;
}

int main()
{
    system("clear");

    printf("=============================================\n");
    printf("\t\tsample main\n");
    printf("=============================================\n");

    /*
    * Read DTLS PSK credentials from persistent storage and
    * set in the OC stack.
    */
    if (SetCredentials() == 0)
    {
        printf("SetCredentials failed\n");
        return 0;
    }

    CAResult_t res = CAInitialize();
    if (res != CA_STATUS_OK)
    {
        printf("CAInitialize fail\n");
        return 0;
    }

#if 0
    // network enable
    // default
    printf("select default network(WIFI)\n");
    res = CASelectNetwork(CA_WIFI);
    if (res != CA_STATUS_OK)
    {
        printf("CASelectNetwork fail\n");
        return 0;
    }
#endif

    // set handler.
    res = CARegisterHandler(request_handler, response_handler);
    if (res != CA_STATUS_OK)
    {
        printf("CARegisterHandler fail\n");
        return 0;
    }

    process();

    CATerminate();
    clearDtlsCredentialInfo();

    return 0;
}

void process()
{
    while (1)
    {
        char menu = get_menu();

        switch (menu)
        {
            case 'm': // menu
            case 'M':
                continue;

            case 'q': // quit
            case 'Q':
                printf("quit..!!\n");
                return;

            case 's': // start server
            case 'S':
                start_listening_server();
                break;

            case 't': // send request
            case 'T':
                send_request_all();
                break;

            case 'c': // start client
            case 'D':
                start_discovery_server();
                break;

            case 'f': // find resource
            case 'F':
                find_resource();
                break;

            case 'r': // send request
            case 'R':
                send_request();
                break;

            case 'a': // advertise resource
            case 'A':
                advertise_resource();
                break;

            case 'b': // send notification
            case 'B':
                send_notification();
                break;

            case 'n': // select network
            case 'N':
                select_network();
                break;

            case 'x': // unselect network
            case 'X':
                unselect_network();
                break;

            case 'h': // handle request response
            case 'H':
                handle_request_response();
                break;
            case 'y':
            case 'Y':
                while (1)
                {
                    gReceived = 0;
                    find_fixed_resource();
                    while (gReceived == 0)
                    {
                        sleep(1);
                        handle_request_response();

                    }
                }
                break;

            case 'w':
            case 'W':
                gReceived = 0;
                start_discovery_server();
                send_secure_request();
                while (gReceived == 0)
                {
                    sleep(1);
                    handle_request_response();
                }

                break;

            case 'z':
            case 'Z':
                start_listening_server();
                while (1)
                {
                    sleep(1);
                    handle_request_response();
                }
                break;

            case 'g': // get network information
            case 'G':
                get_network_info();
                break;

            default:
                printf("not supported menu!!\n");
                break;
        }
    }

}



void start_listening_server()
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

void start_discovery_server()
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

void find_fixed_resource()
{
    char buf[MAX_BUF_LEN] =
    { 0, };
    strcpy(buf, "/a/light");

    // create token
    CAToken_t token = NULL;
    CAResult_t res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!");
        token = NULL;
    }

    printf("generated token %s\n", (token != NULL) ? token : "");

    res = CAFindResource(buf, token);
    if (res != CA_STATUS_OK)
    {
        printf("find resource error!!\n");
    }
    else
    {
        printf("find resource to %s URI\n", buf);
    }

    // delete token
    /*
     if (token != NULL)
     {
     CADestroyToken(token);
     }
     */

    printf("=============================================\n");
}

void find_resource()
{
    char buf[MAX_BUF_LEN];

    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("\n=============================================\n");
    printf("ex) /a/light\n");
    printf("reference uri : ");

    gets(buf);

    // create token
    CAToken_t token = NULL;
    CAResult_t res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!\n");
        token = NULL;
    }

    printf("generated token %s\n", (token != NULL) ? token : "");

    res = CAFindResource(buf, token);
    if (res != CA_STATUS_OK)
    {
        printf("find resource error!!\n");
    }
    else
    {
        printf("find resource to %s URI\n", buf);

        gLastRequestToken = token;
    }

    // delete token
    /*
     if (token != NULL)
     {
     CADestroyToken(token);
     }
     */

    printf("=============================================\n");
}

void send_request()
{
    char secureRequest[2] = {0};
    CAResult_t res;

    res = get_network_type();
    if (res != CA_STATUS_OK)
    {
        return;
    }

    printf("Do you want to send secure request ?.... enter (0/1): ");
    gets(secureRequest);
    if ('1' == secureRequest[0])
    {
        printf("Enter the URI like below....\n");
        printf("coaps://10.11.12.13:4545/resource_uri ( for IP secure)\n");
    }
    else
    {
        printf("Enter the URI like below....\n");
        printf("coap://10.11.12.13:4545/resource_uri ( for IP )\n");
        printf("coap://10:11:12:13:45:45/resource_uri ( for BT )\n");
    }

    char uri[MAX_BUF_LEN] = {'\0'};
    gets(uri);

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    if (CA_STATUS_OK != CACreateRemoteEndpoint(uri, gSelectedNwType, &endpoint)
        || !endpoint)
    {
        printf("Failed to create remote endpoint!\n");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    //endpoint->connectivityType = gSelectedNwType;

    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("\n=============================================\n");
    printf("0:CON, 1:NON\n");
    printf("select message type : ");

    gets(buf);

    CAMessageType_t msgType = (buf[0] == '0' || buf[0] == '1') ? buf[0] - '0' : 0;

    // create token
    CAToken_t token = NULL;
    if (CA_STATUS_OK != CAGenerateToken(&token))
    {
        printf("token generate error!!\n");
        token = NULL;
    }

    printf("generated token %s\n", (token != NULL) ? token : "");

    // extract relative resourceuri from give uri
    char resourceURI[15] = {0};

    printf("URI : %s\n", uri);
    get_resource_uri(uri, resourceURI, 14);

    // create request data
    CAInfo_t requestData;
    memset(&requestData, 0, sizeof(CAInfo_t));
    requestData.token = token;
    if ('1' == secureRequest[0])
    {
        int length = strlen(resourceURI) + 1;
        requestData.payload = (CAPayload_t) malloc(length);
        sprintf(requestData.payload, gSecureInfoData, resourceURI, gLocalSecurePort);
    }
    else
    {
        int length = strlen(gNormalInfoData) + strlen(resourceURI) + 1;
        requestData.payload = (CAPayload_t) malloc(length);
        sprintf(requestData.payload, gNormalInfoData, resourceURI);
    }
    requestData.type = msgType;

    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    CASendRequest(endpoint, &requestInfo);

    if (token != NULL)
    {
        CADestroyToken(token);
    }

    // destroy remote endpoint
    CADestroyRemoteEndpoint(endpoint);
    printf("=============================================\n");
}

void send_secure_request()
{
    char uri[MAX_BUF_LEN];
    char ipv4addr[CA_IPADDR_SIZE];

    printf("\n=============================================\n");
    printf("Enter IPv4 address of the source hosting secure resource (Ex: 11.12.13.14)\n");

    fgets(ipv4addr, CA_IPADDR_SIZE, stdin);
    snprintf(uri, MAX_BUF_LEN, "coaps://%s:5684/a/light", ipv4addr);

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    if (CA_STATUS_OK != CACreateRemoteEndpoint(uri, CA_ETHERNET, &endpoint))
    {
        printf("Failed to create remote endpoint!\n");
        goto exit;
    }

    // create token
    CAToken_t token = NULL;
    if (CA_STATUS_OK != CAGenerateToken(&token))
    {
        printf("Failed to generate token !\n");
        goto exit;
    }

    // create request data
    CAMessageType_t msgType = CA_MSG_NONCONFIRM;
    CAInfo_t requestData;
    memset(&requestData, 0, sizeof(CAInfo_t));
    requestData.token = token;
    requestData.type = msgType;

    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    CASendRequest(endpoint, &requestInfo);

exit:
    // cleanup
    CADestroyToken(token);
    CADestroyRemoteEndpoint(endpoint);
    printf("=============================================\n");
}


void send_request_all()
{
    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    CAResult_t res;

    res = get_network_type();
    if (res != CA_STATUS_OK)
    {
        return;
    }

    printf("\n=============================================\n");
    printf("10.11.12.13:4545/resource_uri ( for IP )\n");
    printf("10:11:12:13:45:45/resource_uri ( for BT )\n");
    printf("uri : ");

    gets(buf);

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    res = CACreateRemoteEndpoint(buf, gSelectedNwType, &endpoint);

    if (res != CA_STATUS_OK)
    {
        printf("create remote endpoint error!!\n");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }


    CAGroupEndpoint_t *group = NULL;
    group = (CAGroupEndpoint_t *)malloc(sizeof(CAGroupEndpoint_t));
    group->connectivityType = endpoint->connectivityType;
    group->resourceUri = endpoint->resourceUri;


    // create token
    CAToken_t token = NULL;
    res = CAGenerateToken(&token);

    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!\n");
        token = NULL;
    }

    printf("generated token %s\n", (token != NULL) ? token : "");

    CAInfo_t requestData;
    memset(&requestData, 0, sizeof(CAInfo_t));
    requestData.token = token;
    requestData.payload = "Temp Json Payload";
	requestData.type = CA_MSG_NONCONFIRM;

    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    // CASendRequest(endpoint, &requestInfo);
    CASendRequestToAll(group, &requestInfo);

    if (token != NULL)
    {
        CADestroyToken(token);
    }

    // destroy remote endpoint
    if (endpoint != NULL)
    {
        CADestroyRemoteEndpoint(endpoint);
    }

    free(group);

    printf("=============================================\n");
}

void advertise_resource()
{
    char buf[MAX_BUF_LEN];

    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    printf("\n=============================================\n");
    printf("uri : ");

    scanf("%s", buf);

    int optionNum = 0;
    char optionData[MAX_OPT_LEN];

    printf("Option Num : ");
    scanf("%d", &optionNum);
    CAHeaderOption_t *headerOpt;
    headerOpt = (CAHeaderOption_t *) malloc(sizeof(CAHeaderOption_t) * optionNum);
    if (NULL == headerOpt)
    {
        printf("Memory allocation failed!\n");
        return;
    }
    memset(headerOpt, 0, sizeof(CAHeaderOption_t) * optionNum);

    int i;
    for (i = 0; i < optionNum; i++)
    {
        int optionID = 0;
        printf("[%d] Option ID : ", i + 1);
        scanf("%d", &optionID);
        headerOpt[i].optionID = optionID;

        memset(optionData, 0, sizeof(char) * MAX_OPT_LEN);
        printf("[%d] Option Data : ", i + 1);
        scanf("%s", optionData);
        memcpy(headerOpt[i].optionData, optionData, strlen(optionData));
        printf("[%d] inputed option : ID : %d, data : %s\n", i + 1, optionID, optionData);

        headerOpt[i].optionLength = (uint16_t) strlen(optionData);
    }
    printf("\n=============================================\n");

    // create token
    CAToken_t token = NULL;
    CAResult_t res = CAGenerateToken(&token);
    if (res != CA_STATUS_OK)
    {
        printf("token generate error!!\n");
        token = NULL;
    }

    printf("generated token %s\n", (token != NULL) ? token : "");

    CAAdvertiseResource(buf, token, headerOpt, (uint8_t) optionNum);

    // delete token
    /*
     if (token != NULL)
     {
     CADestroyToken(token);
     }
     */

    free(headerOpt);

}

void send_notification()
{
    char buf[MAX_BUF_LEN];
    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    CAResult_t res;

    res = get_network_type();
    if (res != CA_STATUS_OK)
    {
        return;
    }

    printf("\n=============================================\n");
    printf("10.11.12.13:4545/resource_uri ( for IP )\n");
    printf("10:11:12:13:45:45/resource_uri ( for BT )\n");
    printf("uri : ");

    gets(buf);

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    res = CACreateRemoteEndpoint(buf, gSelectedNwType, &endpoint);

    if (res != CA_STATUS_OK)
    {
        printf("create remote endpoint error!!\n");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    CAInfo_t respondeData;
    memset(&respondeData, 0, sizeof(CAInfo_t));
    respondeData.token = "client token";
    respondeData.payload = "Temp Notification Data";

    CAResponseInfo_t responseInfo;
    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
    responseInfo.result = CA_SUCCESS;
    responseInfo.info = respondeData;

    // send request
    res = CASendNotification(endpoint, &responseInfo);
    if (res != CA_STATUS_OK)
    {
        printf("send notification error\n");
    }
    else
    {
        printf("send notification success\n");
    }

    // destroy remote endpoint
    if (endpoint != NULL)
    {
        CADestroyRemoteEndpoint(endpoint);
    }

    printf("\n=============================================\n");
}

void select_network()
{
    char buf[MAX_BUF_LEN];

    printf("\n=============================================\n");
    printf("\tselect network\n");
    printf("ETHERNET : 0\n");
    printf("WIFI : 1\n");
    printf("EDR : 2\n");
    printf("LE : 3\n");
    printf("select : ");

    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);
    gets(buf);

    int number = buf[0] - '0';

    number = (number < 0 || number > 3) ? 0 : number;

    CAResult_t res = CASelectNetwork(1 << number);
    if (res != CA_STATUS_OK)
    {
        printf("select network error\n");
        gSelectedNwType = 1 << number;
    }
    else
    {
        printf("select network success\n");
    }

    printf("=============================================\n");
}

void unselect_network()
{
    char buf[MAX_BUF_LEN];

    printf("\n=============================================\n");
    printf("\tunselect enabled network\n");
    printf("ETHERNET : 0\n");
    printf("WIFI : 1\n");
    printf("EDR : 2\n");
    printf("LE : 3\n");
    printf("select : ");

    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);
    gets(buf);

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

char get_menu()
{
    char buf[MAX_BUF_LEN];

    printf("\n=============================================\n");
    printf("\t\tMenu\n");
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
    printf("\tw : send secure request\n");
    printf("\tq : quit\n");
    printf("=============================================\n");
    printf("select : ");

    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);

    gets(buf);

    return buf[0];
}

void handle_request_response()
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

void get_network_info()
{
    int index;

    CALocalConnectivity_t *tempInfo = NULL;
    uint32_t tempSize = 0;

    if((CAGetNetworkInformation(&tempInfo, &tempSize) != CA_STATUS_OK) ||
        !tempInfo || !tempSize)
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

        if (CA_TRUE == tempInfo[index].isSecured)
        {
            gLocalSecurePort = tempInfo[index].addressInfo.IP.port;
        }
        else
        {
            gLocalUnicastPort = tempInfo[index].addressInfo.IP.port;
        }
    }

    free(tempInfo);
    printf("##############################################################");
}

void request_handler(const CARemoteEndpoint_t *object, const CARequestInfo_t *requestInfo)
{
    if (!object)
    {
        printf("Remote endpoint is NULL!");
        return;
    }

    if (!requestInfo)
    {
        printf("Request info is NULL!");
        return;
    }

    printf("##########received request from remote device #############\n");
    printf("Uri: %s\n", object->resourceUri);
    printf("Remote Address: %s Port: %d secured:%d\n", object->addressInfo.IP.ipAddress,
           object->addressInfo.IP.port, object->isSecured);

    printf("Data: %s\n", requestInfo->info.payload);

    if (gLastRequestToken != NULL && requestInfo->info.token != NULL
        && (strcmp((char *)gLastRequestToken, requestInfo->info.token) == 0))
    {
        printf("token is same. received request of it's own. skip.. \n");
        return;
    }

    if (requestInfo->info.options)
    {
        uint32_t len = requestInfo->info.numOptions;
        uint32_t i;
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
        int securePort = get_secure_information(requestInfo->info.payload);
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
            endpoint->isSecured = CA_TRUE;
            object = endpoint;
        }
    }

    printf("send response with URI\n");
    send_response(object, (requestInfo != NULL) ? &requestInfo->info : NULL);

    gReceived = 1;
}

void response_handler(const CARemoteEndpoint_t *object, const CAResponseInfo_t *responseInfo)
{
    printf("##########Received response from remote device #############\n");
    printf("Uri: %s\n", object->resourceUri);
    printf("Remote Address: %s Port: %d secured:%d\n", object->addressInfo.IP.ipAddress,
           object->addressInfo.IP.port, object->isSecured);
    printf("response result : %d\n", responseInfo->result);
    printf("Data: %s\n", responseInfo->info.payload);

    if (responseInfo->info.options)
    {
        uint32_t len = responseInfo->info.numOptions;
        uint32_t i;
        for (i = 0; i < len; i++)
        {
            printf("Option %d\n", i + 1);
            printf("ID : %d\n", responseInfo->info.options[i].optionID);
            printf("Data[%d]: %s\n", responseInfo->info.options[i].optionLength,
                   responseInfo->info.options[i].optionData);
        }
    }
    printf("############################################################\n");
    gReceived = 1;

    //Check if this has secure communication information
    if (responseInfo->info.payload)
    {
        int securePort = get_secure_information(responseInfo->info.payload);
        if (0 < securePort) //Set the remote endpoint secure details and send response
        {
            printf("This is secure resource...\n");
        }
    }
}

void send_response(CARemoteEndpoint_t *endpoint, CAInfo_t *info)
{
    printf("entering send_response\n");

    CAInfo_t responseData;
    memset(&responseData, 0, sizeof(CAInfo_t));
    responseData.type =
        (info != NULL) ?
        ((info->type == CA_MSG_CONFIRM) ? CA_MSG_ACKNOWLEDGE : CA_MSG_NONCONFIRM) :
            CA_MSG_NONCONFIRM;
    responseData.messageId = (info != NULL) ? info->messageId : 0;
    responseData.token = (info != NULL) ? info->token : "";
    responseData.payload = "response payload";

    CAResponseInfo_t responseInfo;
    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
    responseInfo.result = 203;
    responseInfo.info = responseData;

    if (CA_TRUE == endpoint->isSecured)
        printf("Sending response on secure communication\n");
    else
        printf("Sending response on non-secure communication\n");

    // send request (connectivityType from remoteEndpoint of request Info)
    CAResult_t res = CASendResponse(endpoint, &responseInfo);
    if (res != CA_STATUS_OK)
{
        printf("send response error\n");
    }
    else
    {
        printf("send response success\n");
    }

    printf("=============================================\n");
}

void send_request_tmp(CARemoteEndpoint_t *endpoint, CAToken_t token)
{

    printf("\n=============================================\n");

    CAInfo_t requestData;
    memset(&requestData, 0, sizeof(CAInfo_t));
    requestData.token = token;
    requestData.payload = "Temp Json Payload";

    CARequestInfo_t requestInfo;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    endpoint->connectivityType = CA_WIFI;

    CAResult_t res = CASendRequest(endpoint, &requestInfo);
    if (res != CA_STATUS_OK)
    {
        printf("send request error\n");
    }
    else
    {
        printf("send request success\n");
    }

    printf("=============================================\n");

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
    memcpy(portStr, startPos + 1, (endPos - 1) - startPos);

    printf("secured port is: %s\n", portStr);
    return atoi(portStr);
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

CAResult_t get_network_type()
{
    char buf[MAX_BUF_LEN];

    printf("\n=============================================\n");
    printf("\tselect network type\n");
    printf("ETHERNET : 0\n");
    printf("WIFI : 1\n");
    printf("EDR : 2\n");
    printf("LE : 3\n");
    printf("select : ");

    memset(buf, 0, sizeof(char) * MAX_BUF_LEN);
    gets(buf);

    int number = buf[0] - '0';

    number = (number < 0 || number > 3) ? 0 : 1 << number;

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

    printf("\n=============================================\n");
}
