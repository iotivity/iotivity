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

#include "CAHelper.h"

std::set< std::string > CAHelper::setIp;
std::map< std::string, std::map< int, std::map< int, int > > > CAHelper::s_mapReceiveCount;
int CAHelper::s_networkCount;
std::string CAHelper::s_simulatorIp;
int CAHelper::s_simulatorPort;
int CAHelper::s_simulatorSecurePort;
bool CAHelper::s_bufferEmpty;
int CAHelper::s_identityLegth;
int CAHelper::s_pskLength;
TestCaseInfo CAHelper::s_tcInfo;
#ifdef TCP_ADAPTER
int CAHelper::keepAliveCount;
#endif

#ifdef __TIZEN__
static GMainLoop *g_mainloop = NULL;
pthread_t thread;
#endif

CAHelper::CAHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[CAHelper] IN");

    waitTimes[REC_NOR] = WAIT_SIM_DIRECT;
    waitTimes[REC_ACK] = WAIT_SIM_CALLBACK;

    IOTIVITYTEST_LOG(DEBUG, "[CAHelper] OUT");
}

void CAHelper::setupTestCase(MessageInOutType inOutType, MessageDataType validationMethod,
        MessageCommunicationType communicationType)
{
    IOTIVITYTEST_LOG(DEBUG, "[setupTestCase] IN");

    memset(&s_tcInfo, 0, sizeof(TestCaseInfo));

    s_tcInfo.caMethod = CA_GET;
    s_tcInfo.messageType = CA_MSG_NONCONFIRM;
    s_tcInfo.result = CA_STATUS_OK;
    m_responseResult = CA_VALID;

    s_tcInfo.total = TOTAL_MESSAGE;
    s_tcInfo.inOutType = inOutType;
    s_tcInfo.validationMethod = validationMethod;
    s_tcInfo.communicationType = communicationType;
    s_tcInfo.uriType = RESOURCE_URI_NORMAL;
    s_tcInfo.payloadType = PAYLOAD_NORMAL;
    s_tcInfo.securityType = SECURITY_NO;

    getRandomString(10);

    s_tcInfo.numOptions = 0;

    s_mapReceiveCount.clear();

#ifdef TCP_ADAPTER
    keepAliveCount = 0;
#endif

    IOTIVITYTEST_LOG(DEBUG, "[setupTestCase] OUT");
}

void CAHelper::clearBuffer()
{
    IOTIVITYTEST_LOG(DEBUG, "[clearBuffer] IN");

    int i = 0;

    while (i < MAX_ATTEMPT)
    {
        CAHandleRequestResponse();
        i++;
    }

    while (!s_bufferEmpty)
    {
        s_bufferEmpty = true;
        CAHandleRequestResponse();
    }

    IOTIVITYTEST_LOG(DEBUG, "[clearBuffer] OUT");
}

int CAHelper::readConfigurationFile()
{
    FILE *fp;
    int sz;
    int i;
    int j;
    int cnt;
    char buffer[MAX_BUF_LEN + 1];
    char *word;
    char words[10][3][50];

    char networkTypesName[][20] =
    { "CA_IP", "CA_LE", "CA_EDR", "CA_TCP" };
    int numberOfShifts[] =
    { 0, 1, 2, 4 };

    m_availableNetwork = 0;
    m_unAvailableNetwork = 0;

    fp = fopen(CONFIG, "r");
    if (fp == NULL)
    {
        IOTIVITYTEST_LOG(ERROR, "Failed to read the config.ini file");
        return -1;
    }

    cnt = 0;

    while (fgets(buffer, MAX_BUF_LEN, fp))
    {
        word = strtok(buffer, " \n");
        strcpy(words[cnt][0], word);

        for (i = 1; i < 3; i++)
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

    std::string ips;

    for (i = 0; i < cnt; i++)
    {
        if (!strcmp(words[i][0], "IP"))
        {
            s_simulatorIp = string(words[i][2]);
        }
        else if (!strcmp(words[i][0], "OTHER_IP_LIST"))
        {
            ips = string(words[i][2]);
        }
        else if (!strcmp(words[i][0], "PORT"))
        {
            s_simulatorPort = atoi(words[i][2]);
        }
        else if (!strcmp(words[i][0], "SECURE_PORT"))
        {
            s_simulatorSecurePort = atoi(words[i][2]);
        }
    }

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

    std::string delimiter = " ";
    size_t pos = 0;

    setIp.clear();
    setIp.insert(s_simulatorIp);

    std::stringstream stream(ips);
    std::string ip;

    while (getline(stream, ip, ' '))
    {
        setIp.insert(ip);
    }

    s_networkCount = setIp.size();

    printf("s_simulatorSecurePort = %d %d\n", s_simulatorPort, s_simulatorSecurePort);

    if (m_availableNetwork == CA_ADAPTER_IP)
    {
        m_simulatorUri = COAP_PREFIX + s_simulatorIp;
        m_simulatorSecureUri = COAPS_PREFIX + s_simulatorIp;
        m_simulatorUri += ":";
        m_simulatorSecureUri += ":";
        m_simulatorUri += getNumber(s_simulatorPort);
        m_simulatorSecureUri += getNumber(s_simulatorSecurePort);
        m_simulatorUri += "/";
        m_simulatorSecureUri += "/";
    }
    else if (m_availableNetwork == CA_ADAPTER_GATT_BTLE
            || m_availableNetwork == CA_ADAPTER_RFCOMM_BTEDR)
    {
        m_simulatorUri = COAP_PREFIX + s_simulatorIp;
        m_simulatorSecureUri = COAPS_PREFIX + s_simulatorIp;
        m_simulatorUri += "/";
        m_simulatorSecureUri += "/";
    }
#ifdef TCP_ADAPTER
    else if(m_availableNetwork == CA_ADAPTER_TCP)
    {
        m_simulatorUri = COAP_TCP_PREFIX + s_simulatorIp;
        m_simulatorSecureUri = COAPS_TCP_PREFIX + s_simulatorIp;
        m_simulatorUri += ":";
        m_simulatorSecureUri += ":";
        m_simulatorUri += getNumber(s_simulatorPort);
        m_simulatorSecureUri += getNumber(s_simulatorSecurePort);
        m_simulatorUri += "/";
        m_simulatorSecureUri += "/";
    }
#endif

    IOTIVITYTEST_LOG(DEBUG, "simulatorIP: %s\n", m_simulatorUri.c_str());
    m_simulatorResAckUri = m_simulatorUri + SIM_RES_ACK;
    m_simulatorReqAckUri = m_simulatorUri + SIM_REQ_ACK;
    m_simulatorConfigUri = m_simulatorUri + SIM_REQ_CONFIG;

    m_simulatorSecureReqAckUri = m_simulatorSecureUri + SIM_REQ_ACK;

    IOTIVITYTEST_LOG(INFO, "Config URI: %s", m_simulatorConfigUri.c_str());
    IOTIVITYTEST_LOG(INFO, "ReqAck URI: %s", m_simulatorReqAckUri.c_str());
    IOTIVITYTEST_LOG(INFO, "ResAck URI: %s", m_simulatorResAckUri.c_str());
    IOTIVITYTEST_LOG(INFO, "m_simulatorSecureReqAckUri: %s", m_simulatorSecureReqAckUri.c_str());
}

bool CAHelper::setConfigFile()
{
    IOTIVITYTEST_LOG(DEBUG, "[setConfigFile] IN");

    readConfigurationFile();

    IOTIVITYTEST_LOG(INFO, "[setConfigFile] OUT");

    return true;
}

#ifdef __TIZEN__
void GMainLoopThread()
{
    g_main_loop_run(g_mainloop);
}
#endif

bool CAHelper::initialize()
{
    IOTIVITYTEST_LOG(INFO, "[initialize] IN");

#ifdef __TIZEN__
    g_mainloop = g_main_loop_new(NULL, FALSE);
    if(!g_mainloop)
    {
        printf("g_main_loop_new failed\n");
        return CA_STATUS_FAILED;
    }

    int result = pthread_create(&thread, NULL, (void *) &GMainLoopThread, NULL);
    if (result < 0)
    {
        printf("pthread_create failed in initialize\n");
        return CA_STATUS_FAILED;
    }
#endif

    m_result = CAInitialize();

    if (m_result != CA_STATUS_OK)
    {
        getFailureMessage("CAInitialize");

        return false;
    }

    IOTIVITYTEST_LOG(INFO, "[initialize] OUT");

    return true;
}

bool CAHelper::selectNetwork()
{
    IOTIVITYTEST_LOG(DEBUG, "m_availableNetwork %d\n", m_availableNetwork);

    return selectNetwork(m_availableNetwork, CA_STATUS_OK);
}

bool CAHelper::selectNetwork(int interestedNetwork, CAResult_t expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[selectNetwork] IN");

    m_result = CASelectNetwork(interestedNetwork);

    if (m_result != expectedResult)
    {
        getFailureMessage("CASelectNetwork", expectedResult);

        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[selectNetwork] OUT");

    return true;
}

bool CAHelper::unselectNetwork()
{
    return unselectNetwork(m_availableNetwork, CA_STATUS_OK);
}

bool CAHelper::unselectNetwork(int interestedNetwork, CAResult_t expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[unselectNetwork] IN");

    m_result = CAUnSelectNetwork(interestedNetwork);

    if (m_result != expectedResult)
    {
        getFailureMessage("CAUnSelectNetwork", expectedResult);

        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[unselectNetwork] OUT");

    return true;
}

bool CAHelper::startDiscoveryServer()
{
    return startDiscoveryServer(CA_STATUS_OK);
}

bool CAHelper::startDiscoveryServer(CAResult_t expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "start discovery server");

    m_result = CAStartDiscoveryServer();

    if (m_result != expectedResult)
    {
        getFailureMessage("CAStartDiscoveryServer", expectedResult);

        return false;
    }

    return true;
}

bool CAHelper::startListeningServer()
{
    return startListeningServer(CA_STATUS_OK);
}

bool CAHelper::startListeningServer(CAResult_t expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "Start listening server!!");

    m_result = CAStartListeningServer();
    if (m_result != expectedResult)
    {
        getFailureMessage("CAStartListeningServer", expectedResult);

        return false;
    }

    return true;
}

bool CAHelper::createEndpoint(char* uri)
{
    return createEndpoint(uri, CA_STATUS_OK);
}

bool CAHelper::createEndpoint(char* uri, CAResult_t expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[createEndpoint] IN");

    m_endpoint = NULL;
    addressSet_t address =
    { };

    CATransportFlags_t transportFlags = CA_IPV4;

    IOTIVITYTEST_LOG(DEBUG, "[createEndpoint] URI %s", uri);
    if (!parsingCoapUri(uri, &address, &transportFlags))
    {
        return false;
    }

    if(m_availableNetwork == CA_ADAPTER_RFCOMM_BTEDR || m_availableNetwork == CA_ADAPTER_GATT_BTLE)
    {
        transportFlags = CA_DEFAULT_FLAGS;
    }

    IOTIVITYTEST_LOG(DEBUG, "transportFlags: %d", transportFlags);

    IOTIVITYTEST_LOG(DEBUG, "[createEndpoint function] Address: %s , port: %d  \n",
            address.ipAddress, address.port);
    m_result = CACreateEndpoint(transportFlags, m_availableNetwork, (const char*) address.ipAddress,
            address.port, &m_endpoint);
    if (m_result != expectedResult)
    {
        getFailureMessage("CACreateEndpoint", expectedResult);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[createEndpoint] OUT");

    return true;
}

bool CAHelper::createEndpoint(CATransportFlags_t transportFlags, char* address, int port)
{
    CAResult_t expectedResult = CA_STATUS_OK;

    IOTIVITYTEST_LOG(DEBUG, "[createEndpoint] IN");

    m_endpoint = NULL;

    m_result = CACreateEndpoint(transportFlags, m_availableNetwork, address, port, &m_endpoint);
    if (m_result != expectedResult)
    {
        getFailureMessage("CACreateEndpoint", expectedResult);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[createEndpoint] OUT");

    return true;
}

bool CAHelper::createEndpoint(CATransportFlags_t transportFlags, CATransportAdapter_t adapter,
        const char *addr, uint16_t port, CAEndpoint_t* endpointObject, CAResult_t expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[createEndpoint] IN");

    m_result = CACreateEndpoint(transportFlags, adapter, addr, port, &endpointObject);
    if (m_result != expectedResult)
    {
        getFailureMessage("CACreateEndpoint", expectedResult);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[createEndpoint] OUT");

    return true;
}

bool CAHelper::destroyEndpoint()
{
    int length = 0;

    if (m_endpoint != NULL)
    {
        CADestroyEndpoint (m_endpoint);
    }

    return true;
}

bool CAHelper::generateToken()
{
    return generateToken(CA_STATUS_OK);
}

bool CAHelper::generateToken(CAResult_t expectedResult)
{
    if (expectedResult == CA_STATUS_OK)
    {
        m_token = NULL;
        m_result = CAGenerateToken(&m_token, TOKEN_MAX_LENGTH);
        if (m_token == NULL)
        {
            m_failureMessage = "Failed to generate token.";
            return false;
        }
    }
    else
    {
        m_result = CAGenerateToken(NULL, TOKEN_MAX_LENGTH);
    }

    if (m_result != expectedResult)
    {
        getFailureMessage("CAGenerateToken", expectedResult);
        return false;
    }

    return true;
}

bool CAHelper::destroyToken()
{
    CADestroyToken (m_token);
    return true;
}

bool CAHelper::initNetwork()
{
    return initNetwork(true);
}

bool CAHelper::initNetwork(bool select)
{
    IOTIVITYTEST_LOG(DEBUG, "[initNetwork] IN");

    if (!setConfigFile())
    {
        IOTIVITYTEST_LOG(DEBUG, "Unable to read configuration file");
        return false;
    }

    if (!initialize())
    {
        IOTIVITYTEST_LOG(DEBUG, "CAInitialize failed. return value is %d", m_result);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "m_availableNetwork %d", m_availableNetwork);

    if (select)
    {
        m_result = CASelectNetwork(m_availableNetwork);

        if (m_result != CA_STATUS_OK)
        {
            IOTIVITYTEST_LOG(DEBUG, "CASelectNetwork failed. return value is %d", m_result);
            CATerminate();
            return false;
        }
    }

    showNetworkInfo();

    IOTIVITYTEST_LOG(DEBUG, "[initNetwork] OUT");

    return true;
}

bool CAHelper::initServerNetwork()
{
    return initServerNetwork(true);
}

bool CAHelper::initServerNetwork(bool select)
{
    IOTIVITYTEST_LOG(DEBUG, "[initServerNetwork] IN");

    if (!initNetwork(select))
    {
        return false;
    }

    CARegisterHandler(requestHandler, responseHandler, errorHandler);

    if (select)
    {
        m_result = CAStartListeningServer();

        if (m_result != CA_STATUS_OK)
        {
            IOTIVITYTEST_LOG(ERROR, "CAStartListeningServer failed. return value is %d", m_result);
            CATerminate();
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[initServerNetwork] OUT");

    return true;
}

bool CAHelper::initClientNetwork()
{
    return initClientNetwork(true);
}

bool CAHelper::initClientNetwork(bool select)
{
    IOTIVITYTEST_LOG(DEBUG, "[initClientNetwork] IN");

    if (!initNetwork(select))
    {
        return false;
    }

    CARegisterHandler(requestHandler, responseHandler, errorHandler);

    if (select)
    {
        m_result = CAStartDiscoveryServer();
        if (m_result != CA_STATUS_OK)
        {
            IOTIVITYTEST_LOG(ERROR, "CAStartDiscoveryServer failed. return value is %d", m_result);
            CATerminate();
            return false;
        }
    }

    usleep(5000000);

    IOTIVITYTEST_LOG(DEBUG, "[initClientNetwork] OUT");

    return true;
}

bool CAHelper::getNetworkInfo()
{
    IOTIVITYTEST_LOG(DEBUG, "[getNetworkInfo] IN");

    if (m_availableNetwork != CA_ADAPTER_TCP)
    {
        CAEndpoint_t *tempInfo = NULL;
        uint32_t tempSize = 0;

        CAResult_t res = CAGetNetworkInformation(&tempInfo, &tempSize);

        for (int i = 0; i < tempSize; i++)
        {
            IOTIVITYTEST_LOG(DEBUG, "port: %d flags = %d", tempInfo[i].port, tempInfo[i].flags);
        }
        if (CA_STATUS_OK != res || NULL == tempInfo || 0 >= tempSize)
        {
            CAEndpoint_t *tempInfo = NULL;
            uint32_t tempSize = 0;

            CAResult_t res = CAGetNetworkInformation(&tempInfo, &tempSize);
            if (CA_STATUS_OK != res || NULL == tempInfo || 0 >= tempSize)
            {
                IOTIVITYTEST_LOG(DEBUG, "Network not connected\n");
                IOTIVITYTEST_LOG(DEBUG, "CAGetNetworkInformation API Returned: %s ",
                        getResultName(res));
                free(tempInfo);
                return false;
            }

            for (int i = 0; i < tempSize; i++)
            {
                IOTIVITYTEST_LOG(DEBUG, "port: %d, flags: %d ", tempInfo[i].port,
                        tempInfo[i].flags);
            }

            free(tempInfo);
        }
    }

    if (m_availableNetwork == CA_ADAPTER_TCP)
    {
        IOTIVITYTEST_LOG(DEBUG, "port: %d, flags: CA_IPV4 ",
                CAGetAssignedPortNumber(m_availableNetwork, CA_IPV4));
    }

    IOTIVITYTEST_LOG(DEBUG, "[getNetworkInfo] OUT");

    return true;
}

bool CAHelper::showNetworkInfo()
{
    IOTIVITYTEST_LOG(DEBUG, "[printNetworkInfo] IN");

    IOTIVITYTEST_LOG(DEBUG, "is server: %d;", caglobals.server);
    IOTIVITYTEST_LOG(DEBUG, "is client: %d;", caglobals.client);

    IOTIVITYTEST_LOG(DEBUG, "Unsecured Port: %d;",
            CAGetAssignedPortNumber(m_availableNetwork, CA_IPV4));
    IOTIVITYTEST_LOG(DEBUG, "Secured Port: %d;", m_availableNetwork, CA_SECURE | CA_IPV4);

    IOTIVITYTEST_LOG(DEBUG, "Unsecured IPv6 Port: %d;",
            CAGetAssignedPortNumber(m_availableNetwork, CA_IPV6));
    IOTIVITYTEST_LOG(DEBUG, "Secured IPv6 Port: %d;", m_availableNetwork, CA_SECURE | CA_IPV6);

    if (m_availableNetwork != CA_ADAPTER_TCP)
    {
        CAEndpoint_t *tempInfo = NULL;
        uint32_t tempSize = 0;

        CAResult_t res = CAGetNetworkInformation(&tempInfo, &tempSize);
        if (CA_STATUS_OK != res || NULL == tempInfo || 0 >= tempSize)
        {
            IOTIVITYTEST_LOG(DEBUG, "Network not connected\n");
            free(tempInfo);
            return;
        }

        free(tempInfo);
    }

    IOTIVITYTEST_LOG(DEBUG, "##############################################################");

    IOTIVITYTEST_LOG(DEBUG, "[printNetworkInfo] OUT");

    return true;
}

void CAHelper::requestHandler(const CAEndpoint_t* endpoint, const CARequestInfo_t* requestInfo)
{
    IOTIVITYTEST_LOG(DEBUG, "[requestHandler] IN");

    IOTIVITYTEST_LOG(DEBUG, "Something Wrong!!! Request Handler shouldn't be called for CA testcases\n");
    IOTIVITYTEST_LOG(DEBUG, "Check whether message comes from CA simulator or somewhere else\n");

    if (!endpoint)
    {
        IOTIVITYTEST_LOG(DEBUG, "endpoint is NULL\n");
        return;
    }

    if (!requestInfo)
    {
        IOTIVITYTEST_LOG(DEBUG, "requestInfo is NULL\n");
        return;
    }

    IOTIVITYTEST_LOG(DEBUG, "IP %s, Port %d\n", endpoint->addr, endpoint->port);

    IOTIVITYTEST_LOG(DEBUG, "[requestHandler] OUT");
}

void CAHelper::responseHandler(const CAEndpoint_t* endpoint, const CAResponseInfo_t* responseInfo)
{
    IOTIVITYTEST_LOG(DEBUG, "[responseHandler] IN");

    if (!checkRequestResponseHandler(endpoint, responseInfo))
    {
        return;
    }

    s_bufferEmpty = false;

    if (s_tcInfo.messageType == CA_MSG_RESET || s_tcInfo.messageType == CA_MSG_CONFIRM)
    {
        std::string ipAddress = endpoint->addr;
        s_mapReceiveCount[ipAddress][s_simulatorPort][REC_ACK]++;
        return;
    }

    if (s_tcInfo.inOutType == MESSAGE_OUTGOING && s_tcInfo.validationMethod == MESSAGE_URI)
    {
        comparePrefixAndIncrement(endpoint, responseInfo->info.resourceUri, SIM_REQ_ACK, REC_ACK);

        return;
    }

    if (!CheckResponseInfoAttributes(responseInfo))
    {
        return;
    }
    IOTIVITYTEST_LOG(DEBUG, "payload Size %d", responseInfo->info.payloadSize);
    if (s_tcInfo.validationMethod == MESSAGE_PAYLOAD)
    {
        int ret = compareAndIncrement(responseInfo->info.resourceUri, SIM_REQ_ACK,
                responseInfo->info.payload, REC_ACK);

        if (!ret)
        {
            ret = compareAndIncrement(responseInfo->info.resourceUri, SIM_REQ_CONFIG,
                    responseInfo->info.payload, REC_NOR);
        }

        if (!ret)
        {
            IOTIVITYTEST_LOG(ERROR, "Data mismatch!");
        }
    }

    if (s_tcInfo.validationMethod == MESSAGE_HEADER)
    {
        IOTIVITYTEST_LOG(DEBUG, "[handleClientResponse] MESSAGE_HEADER");

        if (checkHeader(responseInfo->info.options, responseInfo->info.numOptions))
        {
            comparePrefixAndIncrement(endpoint, responseInfo->info.resourceUri, SIM_REQ_ACK,
                    REC_ACK);
            IOTIVITYTEST_LOG(DEBUG, "count: ",
                    s_mapReceiveCount[endpoint->addr][s_simulatorPort][REC_ACK]);
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[responseHandler] OUT");
}

void CAHelper::responseHandlerSecond(const CAEndpoint_t* endpoint,
        const CAResponseInfo_t* responseInfo)
{
    IOTIVITYTEST_LOG(DEBUG, "[responseHandlerSecond] IN");

    if (!checkRequestResponseHandler(endpoint, responseInfo))
    {
        return;
    }

    compareAndIncrement(responseInfo->info.resourceUri, SIM_REQ_CONFIG, responseInfo->info.payload,
            REC_SECOND_NOR);

    IOTIVITYTEST_LOG(DEBUG, "[responseHandlerSecond] OUT");
}

int CAHelper::compareAndIncrement(const char* endpointResourceUri, const char *otherResourceUri,
        const char* payload, int index)
{
    IOTIVITYTEST_LOG(DEBUG, "[compareAndIncrement] IN");

    if (strstr(endpointResourceUri, otherResourceUri) != NULL)
    {
        IOTIVITYTEST_LOG(DEBUG, "Resource Uri compare successful");

        int length = strlen(s_tcInfo.identifier);

        IOTIVITYTEST_LOG(DEBUG, "send payload length: %d", length);

        IOTIVITYTEST_LOG(DEBUG, "comparing payload");
        if (payload == NULL)
        {
            IOTIVITYTEST_LOG(DEBUG, "received payload is null");
            return 0;
        }
        else
        {
            IOTIVITYTEST_LOG(DEBUG, "received payload length: %d", strlen(payload));
        }

        if (!strncmp(payload, s_tcInfo.identifier, length))
        {
            IOTIVITYTEST_LOG(DEBUG, "Valid message %d length received", length);
            s_mapReceiveCount[s_simulatorIp][s_simulatorPort][index]++;
        }
        else
        {
            IOTIVITYTEST_LOG(DEBUG, "Mismatch in payload");

            for (int i = 0; i < length; i++)
            {
                if (payload[i] != s_tcInfo.identifier[i])
                {
                    IOTIVITYTEST_LOG(DEBUG, "First mismatch at %dth byte", i + 1);
                    IOTIVITYTEST_LOG(DEBUG, "send=%02x received=%02x (In Hex)",
                            s_tcInfo.identifier[i], payload[i]);
                    break;
                }
            }
        }

        return 1;
    }

    IOTIVITYTEST_LOG(DEBUG, "[compareAndIncrement] OUT");

    return 0;
}

bool CAHelper::checkRequestResponseHandler(const CAEndpoint_t* endpoint,
        const CARequestInfo_t* requestInfo)
{
    IOTIVITYTEST_LOG(DEBUG, "[checkRequestResponseHandler] IN");

    if (!CAHelper::CheckRemoteEndpointAttributes(endpoint, requestInfo->info.resourceUri))
    {
        return false;
    }

    if (CAHelper::isHostUnknown(endpoint))
    {
        return false;
    }

    if (!CheckRequestInfoAttributes(requestInfo))
    {
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[checkRequestResponseHandler] OUT");

    return true;
}

bool CAHelper::checkRequestResponseHandler(const CAEndpoint_t* endpoint,
        const CAResponseInfo_t* responseInfo)
{
    IOTIVITYTEST_LOG(DEBUG, "[checkRequestResponseHandler] IN");

    if (!CAHelper::CheckRemoteEndpointAttributes(endpoint, responseInfo->info.resourceUri))
    {
        return false;
    }

    if (CAHelper::isHostUnknown(endpoint))
    {
        return false;
    }

    if (!CheckResponseInfoAttributes(responseInfo))
    {
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[checkRequestResponseHandler] OUT");

    return true;
}

void CAHelper::comparePrefixAndIncrement(const CAEndpoint_t* endpoint, const char* resourceUri,
        const char *q, int index)
{
    IOTIVITYTEST_LOG(DEBUG, "[comparePrefixAndIncrement] IN");

    if (strstr(resourceUri, q) != NULL)
    {
        IOTIVITYTEST_LOG(DEBUG, "Valid Message Received");

        int port = endpoint->port;
        std::string ipAddress = endpoint->addr;
        IOTIVITYTEST_LOG(DEBUG, "IP: %s, port: %d, index: %d", ipAddress.c_str(), port, index);
        s_mapReceiveCount[ipAddress][s_simulatorPort][index]++;
        IOTIVITYTEST_LOG(DEBUG, "count: %d", s_mapReceiveCount[ipAddress][port][index]);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "resourceUri: %s, q: %s", resourceUri, q);
    }

    IOTIVITYTEST_LOG(DEBUG, "[comparePrefixAndIncrement] OUT");
}

bool CAHelper::isHostUnknown(const CAEndpoint_t* endpoint)
{
    IOTIVITYTEST_LOG(DEBUG, "[isHostUnknown] IN");

    IOTIVITYTEST_LOG(DEBUG, "Address: %s Port: %d", endpoint->addr, endpoint->port);

    if (setIp.find(endpoint->addr) == setIp.end())
    {
        IOTIVITYTEST_LOG(DEBUG, "Address mismatch");
        return true;
    }

    if (endpoint->port != s_simulatorPort && endpoint->port != s_simulatorSecurePort)
    {
        IOTIVITYTEST_LOG(DEBUG, "Port mismatch");
        return true;
    }

    IOTIVITYTEST_LOG(DEBUG, "[isHostUnknown] OUT");

    return false;
}

bool CAHelper::CheckRemoteEndpointAttributes(const CAEndpoint_t* endpoint, char* resourceUri)
{
    IOTIVITYTEST_LOG(DEBUG, "[CheckRemoteEndpointAttributes] IN");

    if (endpoint == NULL)
    {
        IOTIVITYTEST_LOG(DEBUG, "CAEndpoint_t endpoint");
        return false;
    }

    IOTIVITYTEST_LOG(INFO, "address: %s", endpoint->addr);
    IOTIVITYTEST_LOG(INFO, "port: %d", endpoint->port);

    if (s_tcInfo.messageType == CA_MSG_NONCONFIRM)
    {
        if (resourceUri == NULL)
        {
            IOTIVITYTEST_LOG(DEBUG, "Resource URI NULL");
            return false;
        }

        if (!strcmp(resourceUri, ""))
        {
            IOTIVITYTEST_LOG(DEBUG, "Resource URI Empty");
            return false;
        }

        IOTIVITYTEST_LOG(DEBUG, "uri : %s", resourceUri);
    }
    else if (s_tcInfo.messageType == CA_MSG_CONFIRM)
    {
        IOTIVITYTEST_LOG(DEBUG, "Message Type Confirm");
    }

    IOTIVITYTEST_LOG(DEBUG, "[CheckRemoteEndpointAttributes] OUT");

    return true;
}

bool CAHelper::CheckResponseInfoAttributes(const CAResponseInfo_t* responseInfo)
{
    IOTIVITYTEST_LOG(DEBUG, "[CheckResponseInfoAttributes] IN");

    if (responseInfo == NULL)
    {
        IOTIVITYTEST_LOG(ERROR, "ResponseInfo NULL");
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[CheckResponseInfoAttributes] OUT");

    return true;
}

bool CAHelper::CheckRequestInfoAttributes(const CARequestInfo_t* requestInfo)
{
    IOTIVITYTEST_LOG(DEBUG, "[CheckRequestInfoAttributes] IN");

    if (requestInfo == NULL)
    {
        IOTIVITYTEST_LOG(ERROR, "RequestInfo NULL");
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[CheckRequestInfoAttributes] OUT");

    return true;
}

bool CAHelper::sendSecuredRequest(CAMethod_t method, CAMessageType_t type, int totalMessages)
{
    return sendRequest((char*) m_simulatorSecureReqAckUri.c_str(), s_tcInfo.identifier, method,
            type, totalMessages);
}

bool CAHelper::sendSecuredRequest(char* payload, CAMethod_t method, CAMessageType_t type,
        int totalMessages)
{
    return sendRequest((char*) m_simulatorSecureReqAckUri.c_str(), payload, method, type,
            totalMessages);
}

bool CAHelper::sendRequest(CAMethod_t method, CAMessageType_t type, int totalMessages)
{
    return sendRequest((char*) m_simulatorReqAckUri.c_str(), s_tcInfo.identifier, method, type,
            totalMessages);
}

bool CAHelper::sendRequest(char* uri, char* payload, CAMethod_t method, CAMessageType_t type,
        int totalMessages)
{
    return sendRequest(uri, payload, method, type, totalMessages, CA_STATUS_OK);
}

bool CAHelper::sendRequest(char* uri, char* hidden_payload, CAMethod_t method, CAMessageType_t type,
        int totalMessages, CAResult_t expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[sendRequest] IN");

    s_tcInfo.caMethod = method;
    s_tcInfo.messageType = type;

    if (!generateToken())
    {
        return false;
    }

    if (!createEndpoint(uri))
    {
        return false;
    }

    char resourceURI[RESOURCE_URI_LENGTH + 1] =
    { 0 };
    char payload[MAX_BUF_LEN] =
    { 0 };
    getResourceUri(uri, resourceURI, RESOURCE_URI_LENGTH);

    if (strstr(uri, SIM_REQ_CONFIG) != NULL)
    {
        strcpy(payload, hidden_payload);
        IOTIVITYTEST_LOG(DEBUG, "payload type configure");
        IOTIVITYTEST_LOG(DEBUG, "payload: %s", payload);
    }
    else
    {
        strcpy(payload, s_tcInfo.identifier);
        IOTIVITYTEST_LOG(DEBUG, "payload type not configure");
    }

    CAInfo_t requestData =
    { 0 };
    requestData.token = m_token;
    requestData.tokenLength = CA_MAX_TOKEN_LEN;
    requestData.resourceUri = (CAURI_t) resourceURI;
    int payloadSize = 0;

    if (hidden_payload == NULL)
    {
        requestData.payload = NULL;
        requestData.payloadSize = 0;
        IOTIVITYTEST_LOG(DEBUG, "requestData.payload: NULL");
    }
    else
    {
        payloadSize = strlen(payload);
        requestData.payload = (CAPayload_t) malloc(payloadSize);
        requestData.payloadSize = payloadSize;
        memcpy(requestData.payload, payload, payloadSize);
    }

    requestData.type = type;
    requestData.dataType = CA_REQUEST_DATA;
    getOptionData(&requestData);

    CARequestInfo_t requestInfo =
    { 0 };
    requestInfo.method = method;
    requestInfo.info = requestData;
    requestInfo.isMulticast = false;

    for (int messageCount = 0; messageCount < totalMessages; messageCount++)
    {
        IOTIVITYTEST_LOG(DEBUG, "Calling CASendRequest request. Attempt No: %d", messageCount + 1);

        m_result = CASendRequest(m_endpoint, &requestInfo);

        if (m_result != expectedResult)
        {
            getFailureMessage("CASendRequest", expectedResult);
            return false;
        }
    }

    destroyEndpoint();
    destroyToken();

    if (requestData.payload)
    {
        free(requestData.payload);
    }

    IOTIVITYTEST_LOG(DEBUG, "[sendRequest] OUT");

    return true;
}

bool CAHelper::sendRequestToAll(CAMethod_t method, CAMessageType_t type, int totalMessages)
{
    return sendRequestToAll((char*) m_simulatorReqAckUri.c_str(), s_tcInfo.identifier, method, type,
            totalMessages);
}

bool CAHelper::sendRequestToAll(char* uri, char* payload, CAMethod_t method, CAMessageType_t type,
        int totalMessages)
{
    return sendRequestToAll(uri, payload, method, type, totalMessages, CA_STATUS_OK);
}

bool CAHelper::sendRequestToAll(char* uri, char* hidden_payload, CAMethod_t method,
        CAMessageType_t type, int totalMessages, CAResult_t expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[sendRequestToAll] IN");

    s_tcInfo.caMethod = method;
    s_tcInfo.messageType = type;

    if (!generateToken())
    {
        return false;
    }

    m_endpoint = NULL;
    CATransportFlags_t transportFlags = CA_IPV4;
    const char* address = "";
    uint16_t port = 0;

    if(m_availableNetwork == CA_ADAPTER_RFCOMM_BTEDR || m_availableNetwork == CA_ADAPTER_GATT_BTLE)
    {
        transportFlags = CA_DEFAULT_FLAGS;
    }

    m_result = CACreateEndpoint(transportFlags, m_availableNetwork, (const char*) address, port,
            &m_endpoint);
    if (m_result != CA_STATUS_OK)
    {
        getFailureMessage("CACreateEndpoint", CA_STATUS_OK);
        return false;
    }

    char resourceURI[RESOURCE_URI_LENGTH + 1] =
    { 0 };
    char payload[MAX_BUF_LEN] =
    { 0 };

    getResourceUri(uri, resourceURI, RESOURCE_URI_LENGTH);
    strcpy(payload, s_tcInfo.identifier);

    int payloadSize = 0;
    CAInfo_t requestData =
    { 0 };
    requestData.token = m_token;
    requestData.tokenLength = CA_MAX_TOKEN_LEN;
    requestData.resourceUri = (CAURI_t) resourceURI;

    if (hidden_payload == NULL)
    {
        requestData.payload = NULL;
        requestData.payloadSize = 0;
    }
    else
    {
        payloadSize = strlen(payload);
        requestData.payload = (CAPayload_t) malloc(payloadSize);
        requestData.payloadSize = payloadSize;
        memcpy(requestData.payload, payload, payloadSize);
    }

    requestData.type = type;
    requestData.resourceUri = (CAURI_t) resourceURI;

    getOptionData(&requestData);

    CARequestInfo_t requestInfo =
    { 0 };
    requestInfo.method = method;
    requestInfo.info = requestData;
    requestInfo.isMulticast = true;

    for (int messageCount = 0; messageCount < totalMessages; messageCount++)
    {
        IOTIVITYTEST_LOG(DEBUG, "Calling CASendRequestToAll request. Attempt No: %d",
                messageCount + 1);

        m_result = CASendRequest(m_endpoint, &requestInfo);

        if (m_result != expectedResult)
        {
            getFailureMessage("CASendRequestToAll", expectedResult);
            return false;
        }
    }

    destroyToken();

    if (requestData.payload)
    {
        free(requestData.payload);
    }

    IOTIVITYTEST_LOG(DEBUG, "[sendRequestToAll] OUT");

    return true;
}

bool CAHelper::sendConfigurationRequest(SimulatorTask taskType, MessageCommandType msgType,
        CAMethod_t method)
{
    return sendConfigurationRequest(taskType, msgType, TOTAL_MESSAGE, MAX_INTERVAL, method);
}

bool CAHelper::sendConfigurationRequest(SimulatorTask taskType, MessageCommandType msgType,
        int totalMessage, int timeInterval, CAMethod_t method)
{
    IOTIVITYTEST_LOG(DEBUG, "[sendConfigurationRequest] IN");

    s_tcInfo.caMethod = method;
    std::string payload = "";

    switch (taskType)
    {
        case SEND_MESSAGE:
            payload += "0";
            break;

        case SELECT_NETWORK:
            payload += "1";
            break;

        case UNSELECT_NETWORK:
            payload += "2";
            break;

        default:
            break;
    }

    switch (msgType)
    {
        case MESSAGE_RESPONSE:
            payload += "0";
            break;

        case MESSAGE_REQUEST:
            payload += "1";
            break;

        default:
            break;
    }

#if defined(__LINUX__) || defined(__ANDROID_NATIVE__)
    char buffer[4];
    sprintf(buffer, "%04d", totalMessage);
    payload += buffer;

    sprintf(buffer, "%04d", timeInterval);
    payload += buffer;

    sprintf(buffer, "%04d", strlen(s_tcInfo.identifier));
    payload += buffer;

    payload += s_tcInfo.identifier;
    IOTIVITYTEST_LOG(DEBUG, "Random String in structure: %s", s_tcInfo.identifier);

    IOTIVITYTEST_LOG(DEBUG, "[sendConfigurationRequest] OUT");
#endif

#ifdef __TIZEN__
    char *buffer = malloc(sizeof(char)*4);
    sprintf(buffer, "%04d", totalMessage);
    payload.append(buffer);

    sprintf(buffer, "%04d", timeInterval);
    payload.append(buffer);

    sprintf(buffer, "%04d", strlen(s_tcInfo.identifier));
    payload.append(buffer);

    free(buffer);

    payload.append(s_tcInfo.identifier);
    IOTIVITYTEST_LOG(DEBUG, "Random String in structure: %s", s_tcInfo.identifier);
#endif

    return sendRequest((char*) m_simulatorConfigUri.c_str(), (char*) payload.c_str(), method,
            CA_MSG_NONCONFIRM, 1);
}

bool CAHelper::sendResponse(CAResponseResult_t responseResult, CAMessageType_t type,
        int totalMessages)
{
    return sendResponse((char*) m_simulatorResAckUri.c_str(), s_tcInfo.identifier, responseResult,
            type, totalMessages);
}

bool CAHelper::sendResponse(char* uri, char* payload, CAResponseResult_t responseResult,
        CAMessageType_t type, int totalMessages)
{
    return sendResponse(uri, payload, responseResult, type, totalMessages, CA_STATUS_OK);
}

bool CAHelper::sendResponse(char* uri, char* hidden_payload, CAResponseResult_t responseResult,
        CAMessageType_t type, int totalMessages, CAResult_t expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[sendResponse] IN");

    s_tcInfo.messageType = type;

    if (!createEndpoint(uri))
    {
        return false;
    }

    if (!generateToken())
    {
        return false;
    }

    char resourceURI[RESOURCE_URI_LENGTH + 1] =
    { 0 };
    getResourceUri(uri, resourceURI, RESOURCE_URI_LENGTH);
    IOTIVITYTEST_LOG(DEBUG, "resourceURI : %s\n", resourceURI);

    CAInfo_t responseData =
    { 0 };
    responseData.type = type;
    responseData.dataType = CA_RESPONSE_DATA;
    responseData.resourceUri = (CAURI_t) resourceURI;
    responseData.payload = NULL;

    char payload[MAX_BUF_LEN] =
    { 0 };
    int payloadSize = 0;

    responseData.token = m_token;
    responseData.tokenLength = CA_MAX_TOKEN_LEN;

    if (hidden_payload == NULL || type == CA_MSG_RESET)
    {
        responseData.payload = NULL;
        responseData.payloadSize = 0;
    }
    else
    {
        strcpy(payload, hidden_payload);
        strcpy(s_tcInfo.identifier, payload);
        IOTIVITYTEST_LOG(DEBUG, "payload %s", payload);
        payloadSize = strlen(payload);
        responseData.payload = (CAPayload_t) malloc(payloadSize);
        responseData.payloadSize = payloadSize;
        memcpy(responseData.payload, payload, payloadSize);
    }

    getOptionData(&responseData);

    CAResponseInfo_t responseInfo =
    { 0 };
    responseInfo.result = responseResult;
    responseInfo.info = responseData;

    for (int messageCount = 0; messageCount < totalMessages; messageCount++)
    {
        IOTIVITYTEST_LOG(DEBUG, "Calling CASendResponse. Attempt No: %d", messageCount + 1);
        m_result = CASendResponse(m_endpoint, &responseInfo);
        if (m_result != expectedResult)
        {
            getFailureMessage("CASendResponse", expectedResult);
            return false;
        }
    }

    destroyEndpoint();
    destroyToken();

    if (responseData.payload)
    {
        free(responseData.payload);
    }

    IOTIVITYTEST_LOG(DEBUG, "[sendResponse] OUT");

    return true;
}

int CAHelper::getReceiveCount(int index)
{
    IOTIVITYTEST_LOG(DEBUG, "[getReceiveCount] IN");

    if (s_tcInfo.communicationType == MESSAGE_UNICAST || s_networkCount == 1)
    {
        return s_mapReceiveCount[s_simulatorIp][s_simulatorPort][index];
    }

    int minReceiveCount = MAX_ATTEMPT;

    for (auto itIp = s_mapReceiveCount.begin(); itIp != s_mapReceiveCount.end(); itIp++)
    {
        std::map< int, std::map< int, int > > mapPortMsgtype = itIp->second;

        for (auto itPort = mapPortMsgtype.begin(); itPort != mapPortMsgtype.end(); itPort++)
        {
            std::map< int, int > mapMsgtypeCount = itPort->second;

            int temp = mapMsgtypeCount[index];
            if (minReceiveCount > temp)
            {
                minReceiveCount = temp;
            }
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[getReceiveCount] OUT");

    return minReceiveCount * s_networkCount;
}

bool CAHelper::attemptReceiveMessage(int flag)
{
    return attemptReceiveMessage(TOTAL_MESSAGE, MAX_ATTEMPT, flag);
}

bool CAHelper::attemptReceiveMessage(int totalMessages, int maxAttempts, int flag)
{
    IOTIVITYTEST_LOG(DEBUG, "[receiveMessage] IN");

    m_result = CA_STATUS_OK;

    for (int i = 0; i < maxAttempts; i++)
    {
        IOTIVITYTEST_LOG(DEBUG, "Receive Attempt No. %d", (i + 1));

        usleep (waitTimes[flag % 2]);

        m_result = CAHandleRequestResponse();

        if (m_result != CA_STATUS_OK)
        {
            getFailureMessage("CAHandleRequestResponse");
            return false;
        }

        if (getReceiveCount(flag) >= totalMessages)
        {
            break;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[receiveMessage] OUT");

    return true;
}

#ifdef __WITH_DTLS__

bool CAHelper::setDtls()
{
    s_identityLegth = strlen(IDENTITY);
    s_pskLength = strlen(RS_CLIENT_PSK);

    m_result = CAregisterPskCredentialsHandler(CAHelper::getDtlsPskCredentials);
    if (m_result != CA_STATUS_OK)
    {
        m_failureMessage = "CAregisterPskCredentialsHandler failed";
        return false;
    }

    if(m_availableNetwork == CA_ADAPTER_TCP)
    {
        m_result = CAregisterSslHandshakeCallback(CAHelper::dtlsHandshakeCb);
        if (m_result != CA_STATUS_OK)
        {
            m_failureMessage = "CAregisterSslHandshakeCallback failed";
            return false;
        }
    }

    CAsetCredentialTypesCallback(CAHelper::initCipherSuiteList);

    if(m_availableNetwork == CA_ADAPTER_IP)
    {
        m_result = CASelectCipherSuite(MBEDTLS_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256, m_availableNetwork);
    }
    else if(m_availableNetwork == CA_ADAPTER_TCP)
    {
        m_result = CAsetTlsCipherSuite(MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8);
    }

    if (m_result != CA_STATUS_OK)
    {
        m_failureMessage = "CACipherSuite failed";
        return false;
    }

    return true;
}

void CAHelper::initCipherSuiteList(bool * list)
{
    IOTIVITYTEST_LOG(DEBUG, "In %s", __func__);
    if (NULL == list)
    {
        IOTIVITYTEST_LOG(DEBUG, "Out %s", __func__);
        IOTIVITYTEST_LOG(DEBUG, "NULL list param");
        return;
    }

    list[0] = true;
    list[1] = true;
    IOTIVITYTEST_LOG(DEBUG, "Out %s", __func__);
}

void CAHelper::dtlsHandshakeCb(const CAEndpoint_t *endpoint, const CAErrorInfo_t *info)
{
    if (NULL != endpoint)
    {
        IOTIVITYTEST_LOG(DEBUG, "Remote device Address %s:%d:", endpoint->addr, endpoint->port);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "endpoint is null");
    }

    if (NULL != info)
    {
        IOTIVITYTEST_LOG(DEBUG, "ErrorInfo: %d", info->result);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "ErrorInfo is null");
    }
}

int32_t CAHelper::getDtlsPskCredentials( CADtlsPskCredType_t type,
        const unsigned char *desc, size_t desc_len,
        unsigned char *result, size_t result_length)
{
    IOTIVITYTEST_LOG(DEBUG, "CAGetDtlsPskCredentials IN\n");

    int32_t ret = -1;

    if (NULL == result)
    {
        return ret;
    }

    switch (type)
    {
        case CA_DTLS_PSK_HINT:
        case CA_DTLS_PSK_IDENTITY:
        IOTIVITYTEST_LOG(DEBUG, "CAGetDtlsPskCredentials CA_DTLS_PSK_IDENTITY\n");
        if (result_length < s_identityLegth)
        {
            IOTIVITYTEST_LOG(DEBUG, "ERROR : Wrong value for result for storing IDENTITY");
            return ret;
        }

        memcpy(result, IDENTITY, s_identityLegth);
        ret = s_identityLegth;
        break;

        case CA_DTLS_PSK_KEY:
        IOTIVITYTEST_LOG(DEBUG, "CAGetDtlsPskCredentials CA_DTLS_PSK_KEY\n");
        if ((desc_len == s_identityLegth) &&
                memcmp(desc, IDENTITY, s_identityLegth) == 0)
        {
            if (result_length < s_pskLength)
            {
                IOTIVITYTEST_LOG(DEBUG, "ERROR : Wrong value for result for storing RS_CLIENT_PSK");
                return ret;
            }

            memcpy(result, RS_CLIENT_PSK, s_pskLength);
            ret = s_pskLength;
        }
        break;

        default:

        IOTIVITYTEST_LOG(DEBUG, "Wrong value passed for PSK_CRED_TYPE.");
        ret = -1;
    }

    IOTIVITYTEST_LOG(DEBUG, "CAGetDtlsPskCredentials OUT\n");
    return ret;
}

#endif

bool CAHelper::countReceiveMessage(int index)
{
    return countReceiveMessage(index, TOTAL_MESSAGE);
}

bool CAHelper::countReceiveMessage(int index, int cnt)
{
    IOTIVITYTEST_LOG(DEBUG, "countReceiveMessage in");

    if (!attemptReceiveMessage(cnt, MAX_ATTEMPT, index))
    {
        return false;
    }

    if (s_mapReceiveCount[s_simulatorIp][s_simulatorPort][index] != cnt)
    {
        getReceiveFailureMessage(index);
        IOTIVITYTEST_LOG(DEBUG, "getReceiveFailureMessage is printed");
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "countReceiveMessage out");

    return true;
}

bool CAHelper::countMulticastReceiveMessage(int index)
{
    return countMulticastReceiveMessage(index, TOTAL_MESSAGE);
}

bool CAHelper::countMulticastReceiveMessage(int index, int cnt)
{
    IOTIVITYTEST_LOG(DEBUG, "index %d", index);

    if (!attemptReceiveMessage(cnt * s_networkCount, MAX_ATTEMPT * s_networkCount, index))
    {
        return false;
    }

    for (auto itIpCount = setIp.begin(); itIpCount != setIp.end(); itIpCount++)
    {
        if (s_mapReceiveCount[*itIpCount][s_simulatorPort][index] != cnt)
        {
            getReceiveFailureMessage(index);
            return false;
        }
    }

    return true;
}

const char* CAHelper::getReceiveFailureMessage(int index)
{
    IOTIVITYTEST_LOG(DEBUG, "getReceiveFailureMessage in");

    std::ostringstream stream;
    stream << "Return Value";
    stream << ": ";
    stream << getNumber(m_result);
    stream << "   ";
    stream << ",   ";
    stream << "Total Receive Message";
    stream << ": ";
    stream << getNumber(s_mapReceiveCount[s_simulatorIp][s_simulatorPort][index]);

    const char *out = stream.str().c_str();

    IOTIVITYTEST_LOG(DEBUG, out);

    IOTIVITYTEST_LOG(DEBUG, "getReceiveFailureMessage out");

    return out;
}

const char* CAHelper::getFailureMessage()
{
    return m_failureMessage.c_str();
}

const char* CAHelper::getFailureMessage(const char* apiName)
{
    return getFailureMessage(apiName, CA_STATUS_OK);
}

const char* CAHelper::getFailureMessage(const char* apiName, CAResult_t expectedResult)
{
    return getFailureMessage(apiName, m_result, expectedResult);
}

const char* CAHelper::getFailureMessage(const char* apiName, CAResult_t returnedResult,
        CAResult_t expectedResult)
{
    std::ostringstream stream;

    stream << apiName;
    stream << " Returned";
    stream << ": ";
    stream << getResultName(returnedResult);
    stream << "; ";
    stream << "   Expected: ";

    if (returnedResult == expectedResult)
    {
        stream << "Not ";
    }

    stream << getResultName(expectedResult);

    std::string str = stream.str();

    m_failureMessage += str;

    const char *out = str.c_str();

    IOTIVITYTEST_LOG(DEBUG, out);

    IOTIVITYTEST_LOG(DEBUG, "Returned value %d", returnedResult);

    return out;
}

const char* CAHelper::getResultName(CAResult_t result)
{
    if (result == 255)
    {
        return "CA_STATUS_FAILED";
    }

    const char * resultNames[] =
    { "CA_STATUS_OK", "CA_STATUS_INVALID_PARAM", "CA_ADAPTER_NOT_ENABLED",
            "CA_SERVER_STARTED_ALREADY", "CA_SERVER_NOT_STARTED", "CA_DESTINATION_NOT_REACHABLE",
            "CA_SOCKET_OPERATION_FAILED", "CA_SEND_FAILED", "CA_RECEVIE_FAILED",
            "CA_MEMORY_ALLOC_FAILED", "CA_REQUEST_TIMEOUT", "CA_DESTINATION_DISCONNECTED",
            "CA_NOT_SUPPORTED", "CA_STATUS_NOT_INITIALIZED", "CA_DTLS_AUTHENTICATION_FAILURE", };

    int index = (int) result;

    if (index >= 0 && index < 14)
    {
        return resultNames[index];
    }

    return "Unknown Result Type";
}

std::string CAHelper::getRandomString(size_t length)
{
    IOTIVITYTEST_LOG(DEBUG, "[getRandomString] IN");

    int totalUpper = 26;
    int totalDigit = 10;
    int totalLetter = totalUpper + totalUpper;
    int totalAlphaNumeric = totalLetter + totalDigit;

    std::string randomString = "";

    srand (time(NULL));

    for(int i = 0; i < length; i++)
    {
        int randomNumber = rand() % totalAlphaNumeric;

        char ch;

        if (randomNumber < totalUpper)
        {
            ch = (randomNumber + 'A');
        }
        else if (randomNumber < totalLetter)
        {
            ch = (randomNumber - totalUpper + 'a');
        }
        else
        {
            ch = (randomNumber - totalLetter + '0');
        }

        s_tcInfo.identifier[i] = ch;
        randomString += ch;
    }

    s_tcInfo.identifier[length] = 0;

    IOTIVITYTEST_LOG(DEBUG, "[getRandomString] OUT");

    return randomString;
}

std::string CAHelper::getNumber(int num)
{
    char str[12];
    getNumber(num, str);
    return std::string(str);
}

void CAHelper::getNumber(int num, char str[])
{
    int len = 0;

    if (!num)
    {
        str[0] = 48;
        str[1] = 0;
        return;
    }

    if (num < 0)
    {
        len = 1;
        str[0] = '-';
        num = -num;
    }

    for (int i = num; i > 0; i /= 10)
    {
        len++;
    }

    str[len] = 0;

    for (int i = num; i > 0; i /= 10)
    {
        char ch = i % 10 + '0';
        str[--len] = ch;
    }
}

bool CAHelper::parsingCoapUri(const char* uri, addressSet_t* address, CATransportFlags_t *flags)
{
    if (NULL == uri)
    {
        IOTIVITYTEST_LOG(DEBUG, "parameter is null\n");
        return false;
    }

    uint8_t startIndex = 0;
    if (strncmp(COAPS_PREFIX, uri, COAPS_PREFIX_LEN) == 0)
    {
        IOTIVITYTEST_LOG(DEBUG, "uri has '%s' prefix\n", COAPS_PREFIX);
        startIndex = COAPS_PREFIX_LEN;
        *flags = CA_SECURE|CA_IPV4;
    }
    else if (strncmp(COAP_PREFIX, uri, COAP_PREFIX_LEN) == 0)
    {
        IOTIVITYTEST_LOG(DEBUG, "uri has '%s' prefix\n", COAP_PREFIX);
        startIndex = COAP_PREFIX_LEN;
        *flags = CA_IPV4;
    }
#ifdef TCP_ADAPTER
    else if (strncmp(COAPS_TCP_PREFIX, uri, COAPS_TCP_PREFIX_LEN) == 0)
    {
        IOTIVITYTEST_LOG(DEBUG, "uri has '%s' prefix\n", COAPS_TCP_PREFIX);
        startIndex = COAPS_TCP_PREFIX_LEN;
        *flags = CA_SECURE|CA_IPV4;
    }
    else if (strncmp(COAP_TCP_PREFIX, uri, COAP_TCP_PREFIX_LEN) == 0)
    {
        IOTIVITYTEST_LOG(DEBUG, "uri has '%s' prefix\n", COAP_TCP_PREFIX);
        startIndex = COAP_TCP_PREFIX_LEN;
        *flags = CA_IPV4;
    }
#endif

    int32_t len = strlen(uri) - startIndex;

    if (len <= 0)
    {
        IOTIVITYTEST_LOG(DEBUG, "uri length is 0!\n");
        return false;
    }

    char *cloneUri = (char *) calloc(len + 1, sizeof(char));
    if (NULL == cloneUri)
    {
        IOTIVITYTEST_LOG(DEBUG, "Out of memory\n");
        return false;
    }

    memcpy(cloneUri, &uri[startIndex], sizeof(char) * len);
    cloneUri[len] = '\0';

    char *pAddress = cloneUri;
    printf("pAddress : %s\n", pAddress);

    int res = getAddressSet(pAddress, address);
    if (res == -1)
    {
        IOTIVITYTEST_LOG(DEBUG, "address parse error\n");
        free(cloneUri);
        return false;
    }

    free(cloneUri);

    return true;
}

void CAHelper::errorHandler(const CAEndpoint_t *rep, const CAErrorInfo_t* errorInfo)
{
    IOTIVITYTEST_LOG(DEBUG,
            "+++++++++++++++++++++++++++++++++++ErrorInfo+++++++++++++++++++++++++++++++++++\n");

    if (errorInfo)
    {
        const CAInfo_t *info = &errorInfo->info;
        IOTIVITYTEST_LOG(DEBUG, "Error Handler, ErrorInfo :\n");
        IOTIVITYTEST_LOG(DEBUG, "Error Handler result    : %d\n", errorInfo->result);
        IOTIVITYTEST_LOG(DEBUG, "Error Handler token     : %s\n", info->token);
        IOTIVITYTEST_LOG(DEBUG, "Error Handler messageId : %d\n", (uint16_t) info->messageId);
        IOTIVITYTEST_LOG(DEBUG, "Error Handler type      : %d\n", info->type);
        IOTIVITYTEST_LOG(DEBUG, "Error Handler resourceUri : %s\n", info->resourceUri);
        IOTIVITYTEST_LOG(DEBUG, "Error Handler payload   : %s\n", info->payload);

        if (CA_ADAPTER_NOT_ENABLED == errorInfo->result)
        {
            IOTIVITYTEST_LOG(DEBUG, "CA_ADAPTER_NOT_ENABLED, enable the adapter\n");
        }
        else if (CA_SEND_FAILED == errorInfo->result)
        {
            IOTIVITYTEST_LOG(DEBUG,
                    "CA_SEND_FAILED, unable to send the message, check parameters\n");
        }
        else if (CA_MEMORY_ALLOC_FAILED == errorInfo->result)
        {
            IOTIVITYTEST_LOG(DEBUG, "CA_MEMORY_ALLOC_FAILED, insufficient memory\n");
        }
        else if (CA_SOCKET_OPERATION_FAILED == errorInfo->result)
        {
            IOTIVITYTEST_LOG(DEBUG, "CA_SOCKET_OPERATION_FAILED, socket operation failed\n");
        }
        else if (CA_STATUS_FAILED == errorInfo->result)
        {
            IOTIVITYTEST_LOG(DEBUG,
                    "CA_STATUS_FAILED, message could not be delivered, internal error\n");
        }
    }
    IOTIVITYTEST_LOG(DEBUG,
            "++++++++++++++++++++++++++++++++End of ErrorInfo++++++++++++++++++++++++++++++++\n");

    return;
}

#ifdef TCP_ADAPTER
void CAHelper::keepAliveHandler(const CAEndpoint_t *endpoint, bool isConnected)
{
    keepAliveCount ++;
    IOTIVITYTEST_LOG(DEBUG, "KeepAliveHandler in");
    IOTIVITYTEST_LOG(DEBUG, "endpoint address %s   port %d", endpoint->addr, endpoint->port);
    IOTIVITYTEST_LOG(DEBUG, "isConnected %d", isConnected);
    IOTIVITYTEST_LOG(DEBUG, "KeepAliveHandler out");
}

int CAHelper::getKeepAliveCount()
{
    return keepAliveCount;
}
#endif

int CAHelper::getAddressSet(const char *uri, addressSet_t* outAddress)
{
    if (NULL == uri || NULL == outAddress)
    {
        IOTIVITYTEST_LOG(DEBUG, "parameter is null !\n");
        return -1;
    }

    int32_t len = strlen(uri);
    if (len <= 0)
    {
        IOTIVITYTEST_LOG(DEBUG, "uri length is 0!\n");
        return -1;
    }

    int32_t isIp = 0;
    int32_t ipLen = 0;

    for (int i = 0; i < len; i++)
    {
        if (uri[i] == '.')
        {
            isIp = 1;
        }

        if (isIp && uri[i] == ':')
        {
            ipLen = i;
            outAddress->port = atoi(uri + ipLen + 1);
            break;
        }

        if (uri[i] == '/')
        {
            break;
        }

        outAddress->ipAddress[i] = uri[i];
    }

    return isIp;
}

void CAHelper::getResourceUri(char *URI, char *resourceURI, int length)
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
        OICStrcpyPartial(resourceURI, length, startPos + 1, endPos - startPos);
    }

    printf("URI: %s, ResourceURI:%s\n", URI, resourceURI);
}

void CAHelper::getOptionData(CAInfo_t* requestData)
{
    char optionData[][MAX_OPT_LEN] =
    { "optiondata1", "optiondata2", "optiondata3", "optiondata4", "optiondata5" };

    IOTIVITYTEST_LOG(DEBUG, "Option Num : ");

    int optionNum = s_tcInfo.numOptions;

    CAHeaderOption_t * headerOpt = NULL;
    if (0 >= optionNum)
    {
        IOTIVITYTEST_LOG(DEBUG, "there is no headerOption!\n");
        s_tcInfo.options = NULL;
    }
    else
    {
        headerOpt = (CAHeaderOption_t *) calloc(1, optionNum * sizeof(CAHeaderOption_t));
        if (NULL == headerOpt)
        {
            IOTIVITYTEST_LOG(DEBUG, "Memory allocation failed!\n");
            s_tcInfo.options = NULL;
            return NULL;
        }

        int i;
        for (i = 0; i < optionNum; i++)
        {
            char getOptionID[MAX_BUF_LEN] =
            { 0 };

            IOTIVITYTEST_LOG(DEBUG, "[%d] Option ID : ", i + 1);
            int optionID = i + 1;
            headerOpt[i].optionID = optionID;

            IOTIVITYTEST_LOG(DEBUG, "[%d] Option Data : ", i + 1);

            memcpy(headerOpt[i].optionData, optionData[i], strlen(optionData[i]));

            headerOpt[i].optionLength = (uint16_t) strlen(optionData[i]);
        }
        requestData->numOptions = optionNum;
        requestData->options = headerOpt;
    }

    s_tcInfo.options = headerOpt;
}

bool CAHelper::returnRequest(const CAEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo,
        char* payload, int payloadSize)
{
    return returnRequest(endPoint, responseInfo->info.resourceUri, payload, payloadSize,
            CA_MSG_NONCONFIRM, CA_GET, responseInfo->info.token, responseInfo->info.tokenLength,
            responseInfo->info.options, responseInfo->info.numOptions);
}

bool CAHelper::returnRequest(const CAEndpoint_t* endPoint, char* resourceUri, char* payload,
        int payloadSize, CAMessageType_t type, CAMethod_t method, CAToken_t token,
        uint8_t tokenLength, CAHeaderOption_t *options, uint8_t numOptions)
{
    IOTIVITYTEST_LOG(DEBUG, "[returnRequest] IN");

    CAInfo_t requestData =
    { 0 };
    requestData.token = token;
    requestData.tokenLength = tokenLength;
    requestData.payload = payload;
    if (payload != NULL)
    {
        requestData.payloadSize = payloadSize;
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

    CARequestInfo_t requestInfo =
    { 0 };
    requestInfo.method = method;
    requestInfo.info = requestData;
    requestInfo.isMulticast = false;

    CAResult_t res = CASendRequest(endPoint, &requestInfo);
    if (res != CA_STATUS_OK)
    {
        IOTIVITYTEST_LOG(DEBUG, "send request error\n");
        return false;
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "send request success\n");
    }

    IOTIVITYTEST_LOG(DEBUG, "[returnRequest] OUT");

    return true;
}

bool CAHelper::returnResponse(const CAEndpoint_t* endPoint, const CARequestInfo_t* requestInfo,
        char* payload, int payloadSize)
{
    return returnResponse(endPoint, requestInfo->info.resourceUri, payload, payloadSize,
            CA_MSG_NONCONFIRM, CA_VALID, requestInfo->info.messageId, requestInfo->info.token,
            requestInfo->info.tokenLength, requestInfo->info.options, requestInfo->info.numOptions);
}

bool CAHelper::returnResponse(const CAEndpoint_t* endPoint, char* resourceUri, char* payload,
        int payloadSize, CAMessageType_t type, CAResponseResult_t responseCode, uint16_t messageId,
        CAToken_t token, uint8_t tokenLength, CAHeaderOption_t *options, uint8_t numOptions)
{
    IOTIVITYTEST_LOG(DEBUG, "[returnResponse] IN");

    CAInfo_t responseData =
    { 0 };

    responseData.payload = payload;

    if (payload != NULL)
    {
        responseData.payloadSize = payloadSize;
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

    CAResponseInfo_t responseInfo =
    { 0 };
    responseInfo.result = responseCode;
    responseInfo.info = responseData;

    IOTIVITYTEST_LOG(DEBUG, "Sending response....\n");

    CAResult_t res = CASendResponse(endPoint, &responseInfo);

    IOTIVITYTEST_LOG(DEBUG, "Response Send....\n");

    if (res != CA_STATUS_OK)
    {
        IOTIVITYTEST_LOG(DEBUG, "send response error\n");
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "send response success\n");
    }

    IOTIVITYTEST_LOG(DEBUG, "[returnResponse] OUT");

    return true;
}

bool CAHelper::checkHeader(CAHeaderOption_t *options, uint32_t len)
{
    if (options)
    {
        IOTIVITYTEST_LOG(DEBUG, "[checkHeader] option exist");
        uint32_t i;

        for (i = 0; i < len; i++)
        {
            IOTIVITYTEST_LOG(DEBUG, "Option %d\n", i + 1);
            IOTIVITYTEST_LOG(DEBUG, "ID : %d\n", options[i].optionID);
            IOTIVITYTEST_LOG(DEBUG, "Data[%d]: %s\n", options[i].optionLength,
                    options[i].optionData);
        }

        IOTIVITYTEST_LOG(DEBUG, "[checkHeader] vaue %d", len);
        IOTIVITYTEST_LOG(DEBUG, "[checkHeader] vaue %d", s_tcInfo.numOptions);

        if (len == s_tcInfo.numOptions)
        {
            IOTIVITYTEST_LOG(DEBUG, "[checkHeader] here");
            IOTIVITYTEST_LOG(DEBUG, "[checkHeader] optionID %d", options[i].optionID);
            IOTIVITYTEST_LOG(DEBUG, "[checkHeader] optionData %d", options[i].optionData);
            IOTIVITYTEST_LOG(DEBUG, "[checkHeader] optionLength %d", options[i].optionLength);

            IOTIVITYTEST_LOG(DEBUG, "[checkHeader] optionID %d", s_tcInfo.options[i].optionID);
            IOTIVITYTEST_LOG(DEBUG, "[checkHeader] optionData %d", s_tcInfo.options[i].optionData);
            IOTIVITYTEST_LOG(DEBUG, "[checkHeader] optionLength %d",
                    s_tcInfo.options[i].optionLength);
            for (i = 0; i < len; i++)
            {
                if (options[i].optionID != s_tcInfo.options[i].optionID)
                {
                    IOTIVITYTEST_LOG(DEBUG, "[checkHeader] here");
                    break;
                }
                if (options[i].optionLength != s_tcInfo.options[i].optionLength)
                {
                    IOTIVITYTEST_LOG(DEBUG, "[checkHeader] here");
                    break;
                }
                if (strcmp(options[i].optionData, s_tcInfo.options[i].optionData))
                {
                    IOTIVITYTEST_LOG(DEBUG, "[checkHeader] here");
                    break;
                }
            }

            IOTIVITYTEST_LOG(DEBUG, "[checkHeader] len %d", len);

            if (i == len)
            {
                return true;
            }
        }
    }
    else if (len == s_tcInfo.numOptions)
    {
        IOTIVITYTEST_LOG(DEBUG, "[checkHeader] option intentionally null");
        return true;
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "[checkHeader] option unintentionally null");
        return false;
    }

    return false;
}

void CAHelper::setUriType(TcUriType uriType)
{
    s_tcInfo.uriType = uriType;
}

void CAHelper::setPayloadType(TcPayloadType payloadType)
{
    s_tcInfo.payloadType = payloadType;
}

void CAHelper::setMethodType(CAMethod_t method)
{
    s_tcInfo.caMethod = method;
}

void CAHelper::setSecurityType(TcSecurityType securityType)
{
    s_tcInfo.securityType = securityType;
}

void CAHelper::setMessageType(CAMessageType_t messageType)
{
    s_tcInfo.messageType = messageType;
}

void CAHelper::setResponseResult(CAResponseResult_t responseResult)
{
    m_responseResult = responseResult;
}

void CAHelper::setTotalMessage(int total)
{
    s_tcInfo.total = total;
}

void CAHelper::setResultStatus(CAResult_t result)
{
    s_tcInfo.result = result;
}

bool CAHelper::stopListeningServer()
{
    return stopListeningServer(CA_STATUS_OK);
}

bool CAHelper::stopListeningServer(CAResult_t expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "Stop listening server!!");

    m_result = CAStopListeningServer();
    if (m_result != expectedResult)
    {
        getFailureMessage("CAStoptListeningServer", m_result, expectedResult);
        return false;
    }

    return true;
}

bool CAHelper::setAvailableNetwork(CATransportAdapter_t interestedNetwork)
{
    m_availableNetwork = interestedNetwork;
}

bool CAHelper::establishConnectionWithServer()
{
    if (!initClientNetwork())
    {
        return false;
    }

    if (!sendRequestToAll(CA_GET, CA_MSG_NONCONFIRM, 1))
    {
        return false;
    }

    if (!countMulticastReceiveMessage(REC_ACK))
    {
        IOTIVITYTEST_LOG(DEBUG, "No response found for multicast request");
        return false;
    }

    s_mapReceiveCount.clear();

    return true;
}
