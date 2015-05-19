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
#include <stdbool.h>

#include "cacommon.h"
#include "cainterface.h"
#include "oic_malloc.h"
#ifdef __WITH_DTLS__
#include "ocsecurityconfig.h"
#endif

#define MAX_BUF_LEN 1024
#define MAX_OPT_LEN 16

#define PORT_LENGTH 5

#define SECURE_DEFAULT_PORT 5684

#define RESOURCE_URI_LENGTH 14

#define SYSTEM_INVOKE_ERROR 127
#define SYSTEM_ERROR -1

/**
 * @def RS_IDENTITY
 * @brief
 */
#define IDENTITY     ("1111111111111111")
/* @def RS_CLIENT_PSK
 * @brief
 */
#define RS_CLIENT_PSK   ("AAAAAAAAAAAAAAAA")

int g_received;
uint16_t g_local_secure_port = SECURE_DEFAULT_PORT;
CATransportType_t g_selected_nw_type = CA_IPV4;
const char *MESSAGE_TYPE[] = {"CON", "NON", "ACK", "RESET"};

char get_menu();
void process();
CAResult_t get_network_type();
CAResult_t get_input_data(char *buf, int32_t length);

void start_listening_server();
void start_discovery_server();
void find_resource();
void send_request();
void send_request_all();
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
void send_response(const CARemoteEndpoint_t *endpoint, const CAInfo_t *info);
void get_resource_uri(char *URI, char *resourceURI, int length);
int get_secure_information(CAPayload_t payLoad);

static CAToken_t g_last_request_token = NULL;
static const char SECURE_COAPS_PREFIX[] = "coaps://";
static const char SECURE_INFO_DATA[] =
                                    "{\"oc\":[{\"href\":\"%s\",\"prop\":{\"rt\":[\"core.led\"],"
                                     "\"if\":[\"oic.if.baseline\"],\"obs\":1,\"sec\":1,\"port\":%d}}]}";
static const char NORMAL_INFO_DATA[] =
                                    "{\"oc\":[{\"href\":\"%s\",\"prop\":{\"rt\":[\"core.led\"],"
                                     "\"if\":[\"oic.if.baseline\"],\"obs\":1}}]}";

#ifdef __WITH_DTLS__
static CADtlsPskCredsBlob_t *pskCredsBlob = NULL;

void clearDtlsCredentialInfo()
{
    printf("clearDtlsCredentialInfo IN\n");
    if (pskCredsBlob)
    {
        // Initialize sensitive data to zeroes before freeing.
        if (pskCredsBlob->creds)
        {
            memset(pskCredsBlob->creds, 0, sizeof(OCDtlsPskCreds) * (pskCredsBlob->num));
            free(pskCredsBlob->creds);
        }

        memset(pskCredsBlob, 0, sizeof(CADtlsPskCredsBlob_t));
        free(pskCredsBlob);
        pskCredsBlob = NULL;
    }
    printf("clearDtlsCredentialInfo OUT\n");
}

// Internal API. Invoked by CA stack to retrieve credentials from this module
void CAGetDtlsPskCredentials(CADtlsPskCredsBlob_t **credInfo)
{
    printf("CAGetDtlsPskCredentials IN\n");
    if(!credInfo)
    {
        printf("Invalid credential container");
        return;
    }

    *credInfo = (CADtlsPskCredsBlob_t *)malloc(sizeof(CADtlsPskCredsBlob_t));
    if (NULL == *credInfo)
    {
        printf("Failed to allocate credential blob.");
        return;
    }

    size_t credLen = sizeof(OCDtlsPskCreds) * (pskCredsBlob->num);
    (*credInfo)->creds = (OCDtlsPskCreds *)malloc(credLen);
    if (NULL == (*credInfo)->creds)
    {
        printf("Failed to allocate credentials.");
        free(*credInfo);
        *credInfo = NULL;
        return;
    }

    memcpy((*credInfo)->identity, pskCredsBlob->identity, DTLS_PSK_ID_LEN);
    (*credInfo)->num = pskCredsBlob->num;
    memcpy((*credInfo)->creds, pskCredsBlob->creds, credLen);

    printf("CAGetDtlsPskCredentials OUT\n");
}


CAResult_t SetCredentials()
{
    printf("SetCredentials IN\n");
    pskCredsBlob = (CADtlsPskCredsBlob_t *)calloc(1, sizeof(CADtlsPskCredsBlob_t));
    if (NULL == pskCredsBlob)
    {
        printf("Memory allocation failed!\n");
        return CA_MEMORY_ALLOC_FAILED;
     }
    memcpy(pskCredsBlob->identity, IDENTITY, DTLS_PSK_ID_LEN);


    pskCredsBlob->num = 1;

    pskCredsBlob->creds = (OCDtlsPskCreds *)malloc(sizeof(OCDtlsPskCreds) * (pskCredsBlob->num));
    if (NULL == pskCredsBlob->creds)
    {
        printf("Memory allocation failed!\n");
        free(pskCredsBlob);
        return CA_MEMORY_ALLOC_FAILED;
    }

    memcpy(pskCredsBlob->creds[0].id, IDENTITY, DTLS_PSK_ID_LEN);
    memcpy(pskCredsBlob->creds[0].psk, RS_CLIENT_PSK, DTLS_PSK_PSK_LEN);

    printf("SetCredentials OUT\n");
    return CA_STATUS_OK;
}
#endif

int main()
{
    int ret = system("clear");
    // shell invoke error: 127, others: -1
    if (SYSTEM_INVOKE_ERROR == ret || SYSTEM_ERROR == ret)
    {
        printf("Terminal Clear Error: %d\n", ret);
        return -1;
    }

    printf("=============================================\n");
    printf("\t\tsample main\n");
    printf("=============================================\n");

    CAResult_t res = CAInitialize();
    if (CA_STATUS_OK != res)
    {
        printf("CAInitialize fail\n");
        return -1;
    }

    /*
    * Read DTLS PSK credentials from persistent storage and
    * set in the OC stack.
    */
#ifdef __WITH_DTLS__
    res = SetCredentials();
    if (CA_STATUS_OK != res)
    {
        printf("SetCredentials failed\n");
        return -1;
    }

    res = CARegisterDTLSCredentialsHandler(CAGetDtlsPskCredentials);
    if (CA_STATUS_OK != res)
    {
        printf("Set credential handler fail\n");
        return -1;
    }
#endif

    // set handler.
    CARegisterHandler(request_handler, response_handler);

    process();

    CADestroyToken(g_last_request_token);

    g_last_request_token = NULL;

    CATerminate();
#ifdef __WITH_DTLS__
    clearDtlsCredentialInfo();
#endif
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
                break;

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
            case 'C':
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
                    g_received = 0;
                    find_fixed_resource();
                    while (g_received == 0)
                    {
                        sleep(1);
                        handle_request_response();

                    }
                }
                break;

            case 'w':
            case 'W':
                g_received = 0;
                start_discovery_server();
                send_secure_request();
                while (g_received == 0)
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
    if (CA_STATUS_OK != res)
    {
        printf("start listening server fail, error code : %d\n", res);
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
    if (CA_STATUS_OK != res)
    {
        printf("start discovery client fail, error code : %d\n", res);
    }
    else
    {
        printf("start discovery client success\n");
    }
}

void find_fixed_resource()
{
    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    CAResult_t res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        printf("Token generate error!!");
        return;
    }
    printf("Generated token %s\n", token);

    char buf[MAX_BUF_LEN] = { 0 };
    strcpy(buf, "/a/light");

    res = CAFindResource(buf, token, tokenLength);
    if (CA_STATUS_OK != res)
    {
        printf("Find resource error : %d\n", res);
    }
    else
    {
        printf("Find resource to %s URI\n", buf);
    }

    // delete token
    CADestroyToken(token);

    printf("=============================================\n");
}

void find_resource()
{
    printf("\n=============================================\n");
    printf("ex) /a/light\n");
    printf("reference uri : ");

    char buf[MAX_BUF_LEN] = { 0 };
    if (CA_STATUS_OK != get_input_data(buf, MAX_BUF_LEN))
    {
        return;
    }

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    CAResult_t res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        printf("Token generate error!!\n");
        return;
    }

    printf("Generated token %s\n", token);

    res = CAFindResource(buf, token, tokenLength);
    if (CA_STATUS_OK != res)
    {
        printf("Find resource error : %d\n", res);
        CADestroyToken(token);
    }
    else
    {
        printf("Find resource to %s URI\n", buf);
        CADestroyToken(g_last_request_token);
        g_last_request_token = token;
    }

    printf("=============================================\n");
}

void send_request()
{
    CAResult_t res = get_network_type();
    if (CA_STATUS_OK != res)
    {
        return;
    }

    printf("Do you want to send secure request ?.... enter (0/1): ");

    char secureRequest[MAX_BUF_LEN] = {0};
    if (CA_STATUS_OK != get_input_data(secureRequest, MAX_BUF_LEN))
    {
        return;
    }

    if (strcmp(secureRequest, "1") == 0)
    {
        printf("Enter the URI like below....\n");
        printf("coaps://10.11.12.13:4545/resource_uri ( for IP secure)\n");
    }
    else if (strcmp(secureRequest, "0") == 0)
    {
        printf("Enter the URI like below....\n");
        printf("coap://10.11.12.13:4545/resource_uri ( for IP )\n");
        printf("coap://10:11:12:13:45:45/resource_uri ( for BT )\n");
    }
    else
    {
        printf("Input data is wrong value\n");
        return;
    }

    char uri[MAX_BUF_LEN] = {'\0'};
    if (CA_STATUS_OK != get_input_data(uri, MAX_BUF_LEN))
    {
        return;
    }

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    res = CACreateRemoteEndpoint(uri, g_selected_nw_type, &endpoint);
    if (CA_STATUS_OK != res || !endpoint)
    {
        printf("Failed to create remote endpoint, error code : %d\n", res);
        return;
    }

    printf("\n=============================================\n");
    printf("0:CON, 1:NON\n");
    printf("select message type : ");

    char buf[MAX_BUF_LEN] = { 0 };
    if (CA_STATUS_OK != get_input_data(buf, MAX_BUF_LEN))
    {
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    CAMessageType_t msgType = (buf[0] == '1') ? 1 : 0;

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        printf("Token generate error, error code : %d\n", res);
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    printf("Generated token %s\n", token);

    // extract relative resourceuri from give uri
    printf("URI : %s\n", uri);

    char resourceURI[RESOURCE_URI_LENGTH + 1] = {0};
    get_resource_uri(uri, resourceURI, RESOURCE_URI_LENGTH);

    // create request data
    CAInfo_t requestData = { 0 };
    requestData.token = token;
    requestData.tokenLength = tokenLength;

    if (strcmp(secureRequest, "1") == 0)
    {
        size_t length = sizeof(SECURE_INFO_DATA) + strlen(resourceURI);
        requestData.payload = (CAPayload_t) calloc(length,  sizeof(char));
        if (NULL == requestData.payload)
        {
            printf("Memory allocation fail\n");
            CADestroyRemoteEndpoint(endpoint);
            CADestroyToken(token);
            return;
        }
        snprintf(requestData.payload, length, SECURE_INFO_DATA, resourceURI, g_local_secure_port);
    }
    else
    {
        size_t length = sizeof(NORMAL_INFO_DATA) + strlen(resourceURI);
        requestData.payload = (CAPayload_t) calloc(length, sizeof(char));
        if (NULL == requestData.payload)
        {
            printf("Memory allocation fail\n");
            CADestroyRemoteEndpoint(endpoint);
            CADestroyToken(token);
            return;
        }
        snprintf(requestData.payload, length, NORMAL_INFO_DATA, resourceURI);
    }
    requestData.type = msgType;

    CARequestInfo_t requestInfo = { 0 };
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    res = CASendRequest(endpoint, &requestInfo);
    if (CA_STATUS_OK != res)
    {
        printf("Could not send request : %d\n", res);
    }

    //destroy token
    CADestroyToken(token);
    // destroy remote endpoint
    CADestroyRemoteEndpoint(endpoint);
    free(requestData.payload);


    printf("=============================================\n");
}

void send_secure_request()
{
    char uri[MAX_BUF_LEN];
    char ipv4addr[CA_IPADDR_SIZE];

    printf("\n=============================================\n");
    printf("Enter IPv4 address of the source hosting secure resource (Ex: 11.12.13.14)\n");

    if (CA_STATUS_OK != get_input_data(ipv4addr, CA_IPADDR_SIZE))
    {
        return;
    }
    snprintf(uri, MAX_BUF_LEN, "%s%s:5684/a/light", SECURE_COAPS_PREFIX, ipv4addr);

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    CAResult_t res = CACreateRemoteEndpoint(uri, CA_IPV4, &endpoint);
    if (CA_STATUS_OK != res)
    {
        printf("Failed to create remote endpoint, error code: %d\n", res);
        goto exit;
    }

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        printf("Token generate error, error code : %d\n", res);
        goto exit;
    }

    printf("Generated token %s\n", token);

    // create request data
    CAMessageType_t msgType = CA_MSG_NONCONFIRM;
    CAInfo_t requestData = { 0 };
    requestData.token = token;
    requestData.tokenLength = tokenLength;
    requestData.type = msgType;

    CARequestInfo_t requestInfo = { 0 };
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
    CAResult_t res = get_network_type();
    if (CA_STATUS_OK != res)
    {
        return;
    }

    printf("\n=============================================\n");
    printf("ex) /a/light\n");
    printf("resource uri : ");

    char buf[MAX_BUF_LEN] = { 0 };
    if (CA_STATUS_OK != get_input_data(buf, MAX_BUF_LEN))
    {
        return;
    }

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    res = CACreateRemoteEndpoint(buf, g_selected_nw_type, &endpoint);
    if (CA_STATUS_OK != res)
    {
        printf("Create remote endpoint error, error code: %d\n", res);
        return;
    }

    CAGroupEndpoint_t *group = (CAGroupEndpoint_t *) malloc(sizeof(CAGroupEndpoint_t));
    if (NULL == group)
    {
        printf("Memory allocation failed!\n");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }
    group->transportType = endpoint->transportType;
    group->resourceUri = endpoint->resourceUri;

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        printf("Token generate error!!\n");
        CADestroyRemoteEndpoint(endpoint);
        free(group);
        return;
    }

    printf("generated token %s\n", token);

    CAInfo_t requestData = { 0 };
    requestData.token = token;
    requestData.tokenLength = tokenLength;
    requestData.payload = "Temp Json Payload";
    requestData.type = CA_MSG_NONCONFIRM;

    CARequestInfo_t requestInfo = { 0 };
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    // send request
    res = CASendRequestToAll(group, &requestInfo);
    if (CA_STATUS_OK != res)
    {
        printf("Could not send request to all\n");
        CADestroyToken(token);
    }
    else
    {
        CADestroyToken(g_last_request_token);
        g_last_request_token = token;
    }

    // destroy remote endpoint
    CADestroyRemoteEndpoint(endpoint);
    free(group);

    printf("=============================================\n");
}

void advertise_resource()
{
    printf("\n=============================================\n");
    printf("uri : ");

    char buf[MAX_BUF_LEN] = { 0 };
    if (CA_STATUS_OK != get_input_data(buf, MAX_BUF_LEN))
    {
        return;
    }

    char optionNumBuf[MAX_BUF_LEN] = { 0 };
    char optionData[MAX_OPT_LEN] = { 0 } ;

    printf("Option Num : ");
    if (CA_STATUS_OK != get_input_data(optionNumBuf, MAX_BUF_LEN))
    {
        return;
    }
    int optionNum = atoi(optionNumBuf);

    CAHeaderOption_t * headerOpt = (CAHeaderOption_t *)
            calloc(1, optionNum * sizeof(CAHeaderOption_t));
    if (NULL == headerOpt)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    int i;
    for (i = 0; i < optionNum; i++)
    {
        char getOptionID[MAX_BUF_LEN] = { 0 } ;

        printf("[%d] Option ID : ", i + 1);
        if (CA_STATUS_OK != get_input_data(getOptionID, MAX_BUF_LEN))
        {
            free(headerOpt);
            return;
        }
        int optionID = atoi(getOptionID);

        headerOpt[i].optionID = optionID;

        printf("[%d] Option Data : ", i + 1);
        if (CA_STATUS_OK != get_input_data(optionData, MAX_OPT_LEN))
        {
            free(headerOpt);
            return;
        }

        memcpy(headerOpt[i].optionData, optionData, strlen(optionData));
        printf("[%d] inputed option : ID : %d, data : %s\n", i + 1, optionID, optionData);

        headerOpt[i].optionLength = (uint16_t) strlen(optionData);
    }
    printf("\n=============================================\n");

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    CAResult_t res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        printf("Token generate error!!\n");
        free(headerOpt);
        return;
    }

    printf("Generated token %s\n", token);

    res = CAAdvertiseResource(buf, token, tokenLength, headerOpt, (uint8_t) optionNum);
    if (CA_STATUS_OK != res)
    {
        printf("Could not start advertise resource\n");
        CADestroyToken(token);
    }
    else
    {
        CADestroyToken(g_last_request_token);
        g_last_request_token = token;
    }

    free(headerOpt);
}

void send_notification()
{
    CAResult_t res = get_network_type();
    if (CA_STATUS_OK != res)
    {
        return;
    }

    printf("\n=============================================\n");
    printf("Enter the URI like below....\n");
    printf("10.11.12.13:4545/resource_uri ( for IP )\n");
    printf("10:11:12:13:45:45/resource_uri ( for BT )\n");
    printf("uri : ");

    char buf[MAX_BUF_LEN] = { 0 };
    if (CA_STATUS_OK != get_input_data(buf, MAX_BUF_LEN))
    {
        return;
    }

    printf("\n=============================================\n");
    printf("\tselect message type\n");
    printf("CON     : 0\n");
    printf("NON     : 1\n");
    printf("ACK     : 2\n");
    printf("RESET   : 3\n");
    printf("select : ");

    char messageTypeBuf[MAX_BUF_LEN] = { 0 };
    if (CA_STATUS_OK != get_input_data(messageTypeBuf, MAX_BUF_LEN))
    {
        return;
    }

    int messageType = messageTypeBuf[0] - '0';

    // create remote endpoint
    CARemoteEndpoint_t *endpoint = NULL;
    res = CACreateRemoteEndpoint(buf, g_selected_nw_type, &endpoint);
    if (CA_STATUS_OK != res)
    {
        printf("Create remote endpoint error, error code: %d\n", res);
        return;
    }

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    res = CAGenerateToken(&token, tokenLength);
    if ((CA_STATUS_OK != res) || (!token))
    {
        printf("Token generate error!!\n");
        CADestroyRemoteEndpoint(endpoint);
        return;
    }

    printf("Generated token %s\n", token);

    CAInfo_t respondData = { 0 };
    respondData.token = token;
    respondData.payload = "Temp Notification Data";
    respondData.type = messageType;

    CAResponseInfo_t responseInfo = { 0 };
    responseInfo.result = CA_SUCCESS;
    responseInfo.info = respondData;

    // send request
    res = CASendNotification(endpoint, &responseInfo);
    if (CA_STATUS_OK != res)
    {
        printf("Send notification error, error code: %d\n", res);
    }
    else
    {
        printf("Send notification success\n");
    }

    // destroy token
    CADestroyToken(token);
    // destroy remote endpoint
    CADestroyRemoteEndpoint(endpoint);

    printf("\n=============================================\n");
}

void select_network()
{
    printf("\n=============================================\n");
    printf("\tselect network\n");
    printf("IPv4 : 0\n");
    printf("EDR  : 2\n");
    printf("LE   : 3\n");
    printf("select : ");

    char buf[MAX_BUF_LEN] = { 0 };
    if (CA_STATUS_OK != get_input_data(buf, MAX_BUF_LEN))
    {
        return;
    }

    int number = buf[0] - '0';

    if (number < 0 || number > 3)
    {
        printf("Invalid network type\n");
        return;
    }

    CAResult_t res = CASelectNetwork(1 << number);
    if (CA_STATUS_OK != res)
    {
        printf("Select network error\n");
    }
    else
    {
        printf("Select network success\n");
    }

    printf("=============================================\n");
}

void unselect_network()
{
    printf("\n=============================================\n");
    printf("\tunselect enabled network\n");
    printf("IPv4 : 0\n");
    printf("EDR : 2\n");
    printf("LE : 3\n");
    printf("select : ");

    char buf[MAX_BUF_LEN] = { 0 };
    if (CA_STATUS_OK != get_input_data(buf, MAX_BUF_LEN))
    {
        return;
    }

    int number = buf[0] - '0';

    if (number < 0 || number > 3)
    {
        printf("Invalid network type\n");
        return;
    }

    CAResult_t res = CAUnSelectNetwork(1 << number);
    if (CA_STATUS_OK != res)
    {
        printf("Unselect network error\n");
    }
    else
    {
        printf("Unselect network success\n");
    }

    printf("=============================================\n");
}

char get_menu()
{
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

    char buf[MAX_BUF_LEN] = { 0 };
    if (CA_STATUS_OK != get_input_data(buf, MAX_BUF_LEN))
    {
        printf("Failed to get input data\n");
    }

    return buf[0];
}

void handle_request_response()
{
    printf("Handle_request_response\n");

    CAResult_t res = CAHandleRequestResponse();
    if (CA_STATUS_OK != res)
    {
        printf("Handle request error, error code: %d\n", res);
    }
    else
    {
        printf("Handle request success\n");
    }
}

void get_network_info()
{
    CALocalConnectivity_t *tempInfo = NULL;
    uint32_t tempSize = 0;

    CAResult_t res = CAGetNetworkInformation(&tempInfo, &tempSize);
    if (CA_STATUS_OK != res || NULL == tempInfo || 0 >= tempSize)
    {
        printf("Network not connected\n");
        free(tempInfo);
        return;
    }

    printf("################## Network Information #######################\n");
    printf("Network info total size is %d\n\n", tempSize);

    int index;
    for (index = 0; index < tempSize; index++)
    {
        printf("Type: %d\n", tempInfo[index].type);
        if (CA_IPV4 == tempInfo[index].type)
        {
            printf("Address: %s\n", tempInfo[index].addressInfo.IP.ipAddress);
            printf("Port: %d\n", tempInfo[index].addressInfo.IP.port);
        }
        else if (CA_EDR == tempInfo[index].type)
        {
            printf("Address: %s\n", tempInfo[index].addressInfo.BT.btMacAddress);
        }
        else if (CA_LE == tempInfo[index].type)
        {
            printf("Address: %s\n", tempInfo[index].addressInfo.LE.leMacAddress);
        }
        printf("Secured: %s\n\n", tempInfo[index].isSecured ? "true" : "false");

        if (tempInfo[index].isSecured)
        {
            g_local_secure_port = tempInfo[index].addressInfo.IP.port;
            printf("Secured: in global %d\n\n", g_local_secure_port);
        }
    }

    free(tempInfo);
    printf("##############################################################");
}

void request_handler(const CARemoteEndpoint_t *object, const CARequestInfo_t *requestInfo)
{
    if (NULL == object || NULL == requestInfo)
    {
        printf("Input parameter is NULL\n");
        return;
    }

    if ((NULL != g_last_request_token) && (NULL != requestInfo->info.token)
        && (memcmp(g_last_request_token, requestInfo->info.token,
                   CA_MAX_TOKEN_LEN) == 0))
    {
        printf("token is same. received request of it's own. skip.. \n");
        return;
    }

    printf("##########received request from remote device #############\n");
    printf("Uri: %s\n", object->resourceUri);
    if (CA_IPV4 == object->transportType)
    {
        printf("Remote Address: %s Port: %d secured:%d\n", object->addressInfo.IP.ipAddress,
               object->addressInfo.IP.port, object->isSecured);
    }
    else if (CA_EDR == object->transportType)
    {
        printf("Remote Address: %s \n", object->addressInfo.BT.btMacAddress);
    }
    else if (CA_LE == object->transportType)
    {
        printf("Remote Address: %s \n", object->addressInfo.LE.leMacAddress);
    }
    printf("Data: %s\n", requestInfo->info.payload);
    printf("Message type: %s\n", MESSAGE_TYPE[requestInfo->info.type]);

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
    if (requestInfo->info.payload &&
            (CA_IPV4 == object->transportType))
    {
        int securePort = get_secure_information(requestInfo->info.payload);
        if (0 < securePort) //Set the remote endpoint secure details and send response
        {
            printf("This is secure resource...\n");

            //length of "coaps://"
            size_t length = sizeof(SECURE_COAPS_PREFIX) - 1;

            // length of "ipaddress:port"
            length += strlen(object->addressInfo.IP.ipAddress) + PORT_LENGTH;
            length += strlen(object->resourceUri) + 1;

            char *uri = calloc(1, sizeof(char) * length);
            if (!uri)
            {
                printf("Failed to create new uri\n");
                return;
            }
            sprintf(uri, "%s%s:%d/%s", SECURE_COAPS_PREFIX, object->addressInfo.IP.ipAddress,
                    object->addressInfo.IP.port, object->resourceUri);

            CARemoteEndpoint_t *endpoint = NULL;
            if (CA_STATUS_OK != CACreateRemoteEndpoint(uri, object->transportType, &endpoint))
            {
                printf("Failed to create duplicate of remote endpoint!\n");
                return;
            }
            endpoint->isSecured = true;
            object = endpoint;

            free(uri);
        }
    }

    printf("Send response with URI\n");
    send_response(object, &requestInfo->info);

    g_received = 1;
}

void response_handler(const CARemoteEndpoint_t *object, const CAResponseInfo_t *responseInfo)
{
    printf("##########Received response from remote device #############\n");
    printf("Uri: %s\n", object->resourceUri);
    if (CA_IPV4 == object->transportType)
    {
        printf("Remote Address: %s Port: %d secured:%d\n", object->addressInfo.IP.ipAddress,
               object->addressInfo.IP.port, object->isSecured);
    }
    else if (CA_EDR == object->transportType)
    {
        printf("Remote Address: %s \n", object->addressInfo.BT.btMacAddress);
    }
    else if (CA_LE == object->transportType)
    {
        printf("Remote Address: %s \n", object->addressInfo.LE.leMacAddress);
    }
    printf("response result : %d\n", responseInfo->result);
    printf("Data: %s\n", responseInfo->info.payload);
    printf("Message type: %s\n", MESSAGE_TYPE[responseInfo->info.type]);
    printf("Token: %s\n", responseInfo->info.token);
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
    g_received = 1;

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

void send_response(const CARemoteEndpoint_t *endpoint, const CAInfo_t *info)
{
    printf("entering send_response\n");

    printf("\n=============================================\n");
    printf("\tselect message type\n");
    printf("CON     : 0\n");
    printf("NON     : 1\n");
    printf("ACK     : 2\n");
    printf("RESET   : 3\n");
    printf("select : ");

    char buf[MAX_BUF_LEN] = { 0 };
    if (CA_STATUS_OK != get_input_data(buf, MAX_BUF_LEN))
    {
        return;
    }

    int messageType = buf[0] - '0';
    if (0 > messageType || 3 < messageType)
    {
        printf("Invalid message type\n");
        return;
    }

    int responseCode = 0 ;
    char responseCodeBuf[MAX_BUF_LEN] = { 0 };
    if (CA_MSG_RESET != messageType)
    {
        printf("\n=============================================\n");
        printf("\tselect response code\n");
        printf("EMPTY                    :   0\n");
        printf("SUCCESS                  : 200\n");
        printf("CREATED                  : 201\n");
        printf("DELETED                  : 202\n");
        printf("BAD_REQ                  : 400\n");
        printf("BAD_OPT                  : 402\n");
        printf("NOT_FOUND                : 404\n");
        printf("INTERNAL_SERVER_ERROR    : 500\n");
        printf("RETRANSMIT_TIMEOUT       : 504\n");
        printf("select : ");

        if (CA_STATUS_OK != get_input_data(responseCodeBuf, MAX_BUF_LEN))
        {
            return;
        }
        responseCode = atoi(responseCodeBuf);
    }
    CAInfo_t responseData = { 0 };
    responseData.type = messageType;
    responseData.messageId = (info != NULL) ? info->messageId : 0;

    if(CA_MSG_RESET != messageType)
    {
        responseData.token = (info != NULL) ? info->token : NULL;
        responseData.tokenLength = (info != NULL) ? info->tokenLength : 0;

        if (endpoint->isSecured)
        {
            printf("Sending response on secure communication\n");

            uint32_t length = sizeof(SECURE_INFO_DATA) + strlen(endpoint->resourceUri);
            responseData.payload = (CAPayload_t) calloc(length,  sizeof(char));
            if (NULL == responseData.payload)
            {
                printf("Memory allocation fail\n");
                return;
            }
            snprintf(responseData.payload, length, SECURE_INFO_DATA, endpoint->resourceUri,
                     g_local_secure_port);
        }
        else
        {
            printf("Sending response on non-secure communication\n");

            uint32_t length = sizeof(NORMAL_INFO_DATA) + strlen(endpoint->resourceUri);
            responseData.payload = (CAPayload_t) calloc(length, sizeof(char));
            if (NULL == responseData.payload)
            {
                printf("Memory allocation fail\n");
                return;
            }
            snprintf(responseData.payload, length, NORMAL_INFO_DATA, endpoint->resourceUri);
        }
    }

    CAResponseInfo_t responseInfo = { 0 };
    responseInfo.result = responseCode;
    responseInfo.info = responseData;

    // send response (transportType from remoteEndpoint of request Info)
    CAResult_t res = CASendResponse(endpoint, &responseInfo);
    if (CA_STATUS_OK != res)
    {
        printf("Send response error\n");
    }
    else
    {
        printf("Send response success\n");
    }

    printf("=============================================\n");
}

int get_secure_information(CAPayload_t payLoad)
{
    printf("Entering get_secure_information\n");

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
    {
        memcpy(resourceURI, startPos + 1, endPos - startPos);
    }

    printf("URI: %s, ResourceURI:%s\n", URI, resourceURI);
}

CAResult_t get_network_type()
{
    char buf[MAX_BUF_LEN] = { 0 };

    printf("\n=============================================\n");
    printf("\tselect network type\n");
    printf("IPv4 : 0\n");
    printf("BT : 2\n");
    printf("LE : 3\n");
    printf("select : ");

    if (CA_STATUS_OK != get_input_data(buf, MAX_BUF_LEN))
    {
        return CA_NOT_SUPPORTED ;
    }

    int number = buf[0] - '0';

    number = (number < 0 || number > 3) ? 0 : 1 << number;

    if (!(number & 0xf))
    {
        return CA_NOT_SUPPORTED;
    }
    if (number & CA_IPV4)
    {
        g_selected_nw_type = CA_IPV4;
        return CA_STATUS_OK;
    }
    if (number & CA_EDR)
    {
        g_selected_nw_type = CA_EDR;
        return CA_STATUS_OK;
    }
    if (number & CA_LE)
    {
        g_selected_nw_type = CA_LE;
        return CA_STATUS_OK;
    }

    printf("\n=============================================\n");

    return CA_STATUS_FAILED;
}

CAResult_t get_input_data(char *buf, int32_t length)
{
    if (!fgets(buf, length, stdin))
    {
        printf("fgets error\n");
        return CA_STATUS_FAILED;
    }

    char *p = NULL;
    if ( (p = strchr(buf, '\n')) != NULL )
    {
        *p = '\0';
    }

    return CA_STATUS_OK;
}
