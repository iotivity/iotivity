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

TestCaseInfo CAHelper::s_tcInfo;
int CAHelper::s_networkCount;
String CAHelper::s_testCaseIdentifier;
char CAHelper::s_simulatorIp[20];
int CAHelper::s_simulatorPort;
int CAHelper::s_simulatorSecurePort;
int CAHelper::s_receiveCount[6];
bool CAHelper::s_bufferEmpty;

CAHelper::CAHelper()
{

}

void CAHelper::Connect()
{
    Connect(true);
}

void CAHelper::Connect(bool isSetConf)
{
    Serial.println("Initializing CA Helper!");
    
    if (isSetConf && !setConf())
    {
        Serial.println("Error: unable to set configuration");
    }
    
#ifdef LE_ADAPTER
    m_availableNetwork = CA_ADAPTER_GATT_BTLE;
    m_unAvailableNetwork = CA_ADAPTER_IP;
    Serial.println("Network: BLE");
#endif

#ifdef ARDUINOETH
    m_availableNetwork = CA_ADAPTER_IP;
    m_unAvailableNetwork = CA_ADAPTER_GATT_BTLE;
    // Note: ****Update the MAC address here with your shield's MAC address****
    uint8_t ETHERNET_MAC[] =
    {   0x90, 0xA2, 0xDA, 0x0F, 0xE2, 0x25};
    uint8_t error = Ethernet.begin(ETHERNET_MAC);
   
    if (error == 0)
    {
        Serial.print("Failed: ");
        Serial.println(error);
        return;
    }
    
    Serial.println("Network: Ethernet");
#endif

#ifdef ARDUINOWIFI
    m_availableNetwork = CA_ADAPTER_IP;
    m_unAvailableNetwork = CA_ADAPTER_GATT_BTLE;

    const char ssid[] = "oictest_2_4G"; // your network SSID (name)
    const char pass[] = "oictestoictest";// your network password

    int16_t status = WL_IDLE_STATUS;// the Wifi radio's status

    if (WiFi.status() == WL_NO_SHIELD)
    {
        Serial.println("ERROR:No Shield");
        return;
    }

    while (status != WL_CONNECTED)
    {
        Serial.print("connecting: ");
        Serial.println(ssid);
        status = WiFi.begin((char *)ssid, pass);
    }
    Serial.println("Network: WiFi");
#endif
    Serial.println("==========================");
}

void CAHelper::setupTestCase(MessageInOutType inOutType, MessageDataType validationMethod,
        MessageCommunicationType communicationType)
{
    memset(&s_tcInfo, 0, sizeof(TestCaseInfo));

    s_tcInfo.securityType = MESSAGE_NONSECURE;
    s_tcInfo.inOutType = inOutType;
    s_tcInfo.caMethod = CA_GET;
    s_tcInfo.validationMethod = validationMethod;
    s_tcInfo.communicationType = communicationType;
    s_testCaseIdentifier = getRandomString(10);
    s_tcInfo.identifier = (char*) s_testCaseIdentifier.c_str();

    Serial.print("Test case identifier: ");
    Serial.println(s_tcInfo.identifier);

    int i = 0;
    while (i < 6)
    {
        s_receiveCount[i] = 0;
        i++;
    }
}

bool CAHelper::parseData(char *buf, char *url, char *resourceUri)
{
    char *slash = strchr(buf, '/');
    if (!slash)
    {
        return false;
    }

    strcpy(resourceUri, slash);

    char *dot = strchr(buf, '.');
    if (dot && dot < slash)
    {
        char *colon = strchr(buf, ':');
        if (colon && colon < slash)
        {
            strncpy(url, buf, colon - buf);
            return true;
        }
    }

    strncpy(url, buf, slash - buf);
    return true;
}

String CAHelper::getRandomString(size_t length)
{
    int totalUpper = 26;
    int totalDigit = 10;
    int totalLetter = totalUpper + totalUpper;
    int totalAlphaNumeric = totalLetter + totalDigit;

    String randomString = "";

    randomSeed(analogRead(10));

    for (int i = 0; i < length; i++)
    {
        long randomNumber = random(totalAlphaNumeric);

        if (randomNumber < totalUpper)
        {
            randomString += char(randomNumber + 'A');
        }
        else if (randomNumber < totalLetter)
        {
            randomString += char(randomNumber - totalUpper + 'a');
        }
        else
        {
            randomString += char(randomNumber - totalLetter + '0');
        }
    }
    return randomString;
}

bool CAHelper::initNetwork()
{
    return initNetwork(true);
}

bool CAHelper::initNetwork(bool select)
{
    Serial.println("[initNetwork] --- started");

    CAResult_t result = CAInitialize();
    if (result != CA_STATUS_OK)
    {
        Serial.print("CAInitialize failed. return value is ");
        Serial.println(result);
        return false;
    }

    if (select)
    {
        result = CASelectNetwork(m_availableNetwork);
        if (result != CA_STATUS_OK)
        {
            Serial.print("CASelectNetwork failed. return value is ");
            Serial.println(result);
            return false;
        }
    }
      
    Serial.println("[initNetwork] --- finished");

    return true;
}

bool CAHelper::initClientNetwork()
{
    return initClientNetwork(true);
}

bool CAHelper::initClientNetwork(bool select)
{
    if (!initNetwork(select))
    {
        return false;
    }

    CARegisterHandler(handleClientRequest, handleClientResponse, handleError);

    if (select)
    {
        CAResult_t result = CAStartDiscoveryServer();
        if (result != CA_STATUS_OK)
        {
            Serial.print("Error: CAStartDiscoveryServer failed. return value is ");
            Serial.println(result);
            return false;
        }
    }
    return true;
}

bool CAHelper::initServerNetwork()
{
    return initServerNetwork(true);
}

bool CAHelper::initServerNetwork(bool select)
{
    if (!initNetwork(select))
    {
        return false;
    }

    CARegisterHandler(handleServerRequest, handleServerResponse, handleError);

    if (select)
    {
        CAResult_t result = CAStartListeningServer();
        if (result != CA_STATUS_OK)
        {
            Serial.print("Error: CAStartDiscoveryServer failed. return value is ");
            Serial.println(result);
            return false;
        }
    }
    return true;
}

bool CAHelper::checkHandler(const CAEndpoint_t* remoteEndpoint,
        const CARequestInfo_t* requestInfo)
{
    Serial.println("checkHandler in");

    if (!CAHelper::showAndCheckRemoteEndpointAttributes(remoteEndpoint))
    {
        return false;
    }

    if (CAHelper::isHostUnknown(remoteEndpoint))
    {
        return false;
    }

    if (!showAndCheckRequestInfoAttributes(requestInfo))
    {
        return false;
    }

    Serial.println("checkHandler out");

    return true;
}

bool CAHelper::checkHandler(const CAEndpoint_t* remoteEndpoint,
        const CAResponseInfo_t* responseInfo)
{
    Serial.println("checkHandler in");

    if (!CAHelper::showAndCheckRemoteEndpointAttributes(remoteEndpoint))
    {
        return false;
    }

    if (CAHelper::isHostUnknown(remoteEndpoint))
    {
        return false;
    }

    if (!showAndCheckResponseInfoAttributes(responseInfo))
    {
        return false;
    }

    Serial.println("checkHandler out");

    return true;
}

int CAHelper::compareAndIncrement(const char* remoteEndpointResourceUri,
        const char *otherResourceUri, const char* payload, int index)
{
    Serial.println("compareAndIncrement in");

    if (strstr(remoteEndpointResourceUri, otherResourceUri) != NULL)
    {
        Serial.println("Valid ResourceUri Received");
        if (!strncmp(payload, s_tcInfo.identifier, strlen(s_tcInfo.identifier)))
        {
            Serial.println("Valid payload Received");
            s_receiveCount[index]++;
        }
        else
        {
            Serial.println("Invalid payload Received");
        }

        return 1;
    }
    else
    {
        Serial.println("Valid ResourceUri Received");
    }

    Serial.println("compareAndIncrement out");

    return 0;
}

void CAHelper::comparePrefixAndIncrement(const CARequestInfo_t* requestInfo, const char *q,
        int index)
{
    Serial.println("comparePrefixAndIncrement in");

    if (strstr(requestInfo->info.resourceUri, q) != NULL)
    {
        Serial.print("Valid ResourceUri Received: ");
        Serial.println(s_tcInfo.identifier);
        s_receiveCount[index]++;
    }
    
    else
    {
        Serial.println("Invalid ResourceUri Received");
    }

    Serial.println("comparePrefixAndIncrement out");
}

void CAHelper::comparePrefixAndIncrement(const CAResponseInfo_t* responseInfo, const char *q,
        int index)
{
    Serial.println("comparePrefixAndIncrement in");

    if (strstr(responseInfo->info.resourceUri, q) != NULL)
    {
        Serial.print("Valid ResourceUri Received: ");
        Serial.println(s_tcInfo.identifier);
        s_receiveCount[index]++;
    }
    
    else
    {
        Serial.println("Invalid ResourceUri Received");
    }
    
    Serial.println("comparePrefixAndIncrement out");
}

bool CAHelper::isHostUnknown(const CAEndpoint_t* remoteEndpoint)
{
    Serial.println("isHostUnknown in");

    if (strcmp(s_simulatorIp, remoteEndpoint->addr))
    {
        Serial.print("IP mismatch: ");
        Serial.println(remoteEndpoint->addr);
        return true;
    }

    Serial.println("isHostUnknown out");

    return false;
}

bool CAHelper::showAndCheckRemoteEndpointAttributes(const CAEndpoint_t* remoteEndpoint)
{
    Serial.println("showAndCheckRemoteEndpointAttributes in");

    if (remoteEndpoint == NULL)
    {
        Serial.println("remoteEndpoint is null");
        return false;
    }

    Serial.print("Remote Address: ");
    Serial.print(remoteEndpoint->addr);
    Serial.print(":");
    Serial.println(remoteEndpoint->port);

    Serial.println("showAndCheckRemoteEndpointAttributes out");

    return true;
}

bool CAHelper::showAndCheckResponseInfoAttributes(const CAResponseInfo_t* responseInfo)
{
    Serial.println("showAndCheckResponseInfoAttributes in");

    if (responseInfo == NULL)
    {
        Serial.println("ResponseInfo NULL");
        return false;
    }
    
    if (responseInfo->info.resourceUri == NULL)
    {
        Serial.println("Resource URI NULL");
        return false;
    }

    Serial.print("URI: ");
    Serial.println(responseInfo->info.resourceUri);

    if (!strcmp(responseInfo->info.resourceUri, ""))
    {
        Serial.println("Resource URI Empty");
        return false;
    }

    Serial.println("showAndCheckResponseInfoAttributes out");

    return true;
}

bool CAHelper::showAndCheckRequestInfoAttributes(const CARequestInfo_t* requestInfo)
{
    Serial.println("showAndCheckRequestInfoAttributes out");

    if (requestInfo == NULL)
    {
        Serial.println("RequestInfo NULL");
        return false;
    }  
    
    if (requestInfo->info.resourceUri == NULL)
    {
        Serial.println("Resource URI NULL");
        return false;
    }

    Serial.print("URI: ");
    Serial.println(requestInfo->info.resourceUri);

    if (!strcmp(requestInfo->info.resourceUri, ""))
    {
        Serial.println("Resource URI Empty");
        return false;
    }

    Serial.println("showAndCheckRequestInfoAttributes out");

    return true;
}

void CAHelper::handleError(const CAEndpoint_t *rep, const CAErrorInfo_t* errorInfo)
{
    Serial.println("+++++++++++++++++++++++++++++++++++ErrorInfo+++++++++++++++++++++++++++++++++++\n");

    if(errorInfo)
    {
        const CAInfo_t *info = &errorInfo->info;
        Serial.println("Error Handler, ErrorInfo");

        if(CA_ADAPTER_NOT_ENABLED == errorInfo->result)
        {
            Serial.println("CA_ADAPTER_NOT_ENABLED, enable the adapter\n");
        }
        
        else if(CA_SEND_FAILED == errorInfo->result)
        {
            Serial.println("CA_SEND_FAILED, unable to send the message, check parameters\n");
        }
        
        else if(CA_MEMORY_ALLOC_FAILED == errorInfo->result)
        {
            Serial.println("CA_MEMORY_ALLOC_FAILED, insufficient memory\n");
        }
        
        else if(CA_SOCKET_OPERATION_FAILED == errorInfo->result)
        {
            Serial.println("CA_SOCKET_OPERATION_FAILED, socket operation failed\n");
        }
        
        else if(CA_STATUS_FAILED == errorInfo->result)
        {
            Serial.println("CA_STATUS_FAILED, message could not be delivered, internal error\n");
        }
    }
    Serial.println("++++++++++++++++++++++++++++++++End of ErrorInfo++++++++++++++++++++++++++++++++\n");

    return;
}

void CAHelper::handleClientRequest(const CAEndpoint_t* remoteEndpoint,
        const CARequestInfo_t* requestInfo)
{
    Serial.println("handleClientRequest in");

    if (!checkHandler(remoteEndpoint, requestInfo))
    {
        return;
    }

    comparePrefixAndIncrement(requestInfo, s_tcInfo.identifier, REC_CLI_NOR);

    Serial.println("handleClientRequest out");
}

void CAHelper::handleClientResponse(const CAEndpoint_t* remoteEndpoint,
        const CAResponseInfo_t* responseInfo)
{
    Serial.println("handleClientResponse in");

    if (!checkHandler(remoteEndpoint, responseInfo))
    {
        return;
    }

    s_bufferEmpty = false;

    if (s_tcInfo.inOutType == MESSAGE_OUTGOING && s_tcInfo.validationMethod == MESSAGE_URI)
    {
        comparePrefixAndIncrement(responseInfo, SIM_REQ_ACK, REC_CLI_ACK);

        return;
    }

    if (!showAndCheckResponseInfoAttributes(responseInfo))
    {
        return;
    }

    if (s_tcInfo.validationMethod == MESSAGE_PAYLOAD)
    {
        Serial.print("Payload: ");
        Serial.println((char*)responseInfo->info.payload);

        int ret = compareAndIncrement(responseInfo->info.resourceUri, SIM_REQ_ACK,
                responseInfo->info.payload, REC_CLI_ACK);

        if (!ret)
        {
            ret = compareAndIncrement(responseInfo->info.resourceUri, SIM_REQ_CONFIG,
                    responseInfo->info.payload, REC_CLI_NOR);
        }

        if (!ret)
        {
            Serial.println("Data mismatch!");
        }
    }

    if (s_tcInfo.validationMethod == MESSAGE_EMPTY)
    {
        Serial.println("validation metnod message emtpy");

        if(strstr(responseInfo->info.resourceUri, SIM_REQ_ACK) != NULL)
        {
            Serial.println("Valid Message Received");
            s_receiveCount[REC_CLI_ACK]++;
        }
    }

    Serial.println("handleClientResponse out");
}

void CAHelper::handleServerRequest(const CAEndpoint_t* remoteEndpoint,
        const CARequestInfo_t* requestInfo)
{
    Serial.println("handleServerRequest in");

    if (!checkHandler(remoteEndpoint, requestInfo))
    {
        return;
    }

    s_bufferEmpty = false;

    if (s_tcInfo.validationMethod == MESSAGE_URI && s_tcInfo.communicationType == MESSAGE_MULTICAST)
    {
        if (s_tcInfo.inOutType == MESSAGE_INCOMING)
        {
            comparePrefixAndIncrement(requestInfo, s_tcInfo.identifier, REC_SVR_NOR);
            return;
        }
        else if (s_tcInfo.inOutType == MESSAGE_OUTGOING)
        {
            comparePrefixAndIncrement(requestInfo, s_tcInfo.identifier, REC_SVR_ACK);
            return;
        }
    }

    if (s_tcInfo.inOutType == MESSAGE_OUTGOING && s_tcInfo.validationMethod == MESSAGE_URI
            && s_tcInfo.communicationType == MESSAGE_UNICAST)
    {
        comparePrefixAndIncrement(requestInfo, SIM_RES_ACK, REC_SVR_ACK);
        return;
    }

    if (s_tcInfo.validationMethod == MESSAGE_PAYLOAD)
    {
        Serial.print("Payload: ");
        Serial.println((char*)requestInfo->info.payload);

        int ret = compareAndIncrement(requestInfo->info.resourceUri, SIM_RES_ACK,
                requestInfo->info.payload, REC_SVR_ACK);

        if (!ret)
        {
            if (requestInfo->method == s_tcInfo.caMethod)
            {
                ret = compareAndIncrement(requestInfo->info.resourceUri, SIM_REQ_CONFIG,
                        requestInfo->info.payload, REC_SVR_NOR);
            }
            
            else
            {
                Serial.println("Invalid Method for SIM_REQ_CONFIG");
            }
        }

        if (!ret)
        {
            Serial.println("Data mismatch!");
        }
    }

    if (s_tcInfo.validationMethod == MESSAGE_EMPTY)
    {
        Serial.println("validation method empty");
        if(strstr(requestInfo->info.resourceUri, SIM_RES_ACK) != NULL)
        {
            Serial.println("Valid Message Received");
            s_receiveCount[REC_SVR_ACK]++;
        }
    }
    Serial.println("handleServerRequest out");
}

void CAHelper::handleServerResponse(const CAEndpoint_t* remoteEndpoint,
        const CAResponseInfo_t* responseInfo)
{
    Serial.println("handleServerResponse in");
    
    if (!checkHandler(remoteEndpoint, responseInfo))
    {
        return;
    }
    
    Serial.println("handleServerResponse out");
}

void CAHelper::handleClientResponseSecond(const CAEndpoint_t* remoteEndpoint,
        const CAResponseInfo_t* responseInfo)
{
    Serial.println("handleClientResponseSecond in");

    if (!checkHandler(remoteEndpoint, responseInfo))
    {
        return;
    }

    compareAndIncrement(responseInfo->info.resourceUri, SIM_REQ_CONFIG, responseInfo->info.payload,
            REC_SECOND_CLI_NOR);
    Serial.println("handleClientResponseSecond out");
}

void CAHelper::handleServerRequestSecond(const CAEndpoint_t* remoteEndpoint,
        const CARequestInfo_t* requestInfo)
{
    Serial.println("handleServerRequestSecond in");

    if (!CAHelper::showAndCheckRemoteEndpointAttributes(remoteEndpoint))
    {
        return;
    }

    if (CAHelper::isHostUnknown(remoteEndpoint))
    {
        return;
    }

    compareAndIncrement(requestInfo->info.resourceUri, SIM_REQ_CONFIG, requestInfo->info.payload,
            REC_SECOND_SVR_NOR);
    Serial.println("handleServerRequestSecond out");
}

bool CAHelper::setConf()
{
    Serial.println("setConf in");

    char buffer[MAX_BUF_LEN] = {0};
    size_t len;

    while(1)
    {
        Serial.println("Inside loop: Input IP Address");
        
        if (Serial.available() > 0)
        {
            getData(buffer, sizeof(buffer), &len);

            if (4 >= len)
            {
                Serial.println("i/p err");
                return false;
            }
            
            else
                break;
        }

        delay(500);
    }
    
    strcpy(s_simulatorIp, buffer);

    s_simulatorPort = 55555;
    s_simulatorSecurePort = 6299;
    s_networkCount = 1;

    waitTimes[REC_CLI_NOR] = WAIT_SIM_DIRECT;
    waitTimes[REC_CLI_ACK] = WAIT_SIM_CALLBACK;
    
    Serial.println("setConf out");

    return true;
}

void CAHelper::printEndpoint(CAEndpoint_t* rep)
{
    Serial.print("Remote Endpoint: ");
    Serial.print(rep->addr);
    Serial.print(":");
    Serial.println(rep->port);
}

void CAHelper::printReceiveError(String testLog, int index)
{
    printReceiveError(testLog, TOTAL_MESSAGE, index);
}

void CAHelper::printReceiveError(String testLog, int expectedMessageCount, int index)
{
    Serial.println("Message Receive Failed!");

    Serial.print("Total Received: ");
    Serial.print(s_receiveCount[index]);
    if (expectedMessageCount == 0)
    {

        Serial.print(" where expected was ");
        Serial.println(expectedMessageCount);
    }
    
    else
    {
        Serial.print(" out of ");
        Serial.println(expectedMessageCount);
    }

    Serial.println(testLog);
}

int CAHelper::getReceiveCount(int index)
{
    return s_receiveCount[index];
}

String CAHelper::formatStringFromInteger(int number, int stringLength)
{
    String formattedString = "";
    int zeroPadding = 0;

    String numberString = String(number);
    zeroPadding = stringLength - numberString.length();

    while (zeroPadding > 0)
    {
        formattedString += "0";
        zeroPadding--;
    }
    
    formattedString += numberString;
    return formattedString;
}

CAResult_t CAHelper::sendConfigurationRequest(SimulatorTask taskType, MessageCommandType msgType,
        CAMethod_t method)
{
    return sendConfigurationRequest(taskType, msgType, TOTAL_MESSAGE, MAX_INTERVAL, method);
}

CAResult_t CAHelper::sendConfigurationRequest(SimulatorTask taskType, MessageCommandType msgType,
        int totalMessage, int timeInterval, CAMethod_t method)
{
    s_tcInfo.caMethod = method;
    String payload = "";

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

        case STOP_SIM:
            payload += "3";
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

    payload += formatStringFromInteger(totalMessage, 4);
    payload += formatStringFromInteger(timeInterval, 4);
    payload += formatStringFromInteger(strlen(s_tcInfo.identifier), 4);

    payload += s_tcInfo.identifier;
    return sendRequest((char*) SIM_REQ_CONFIG, (char*) payload.c_str(), method,
            CA_MSG_NONCONFIRM, 1);
}

CAResult_t CAHelper::sendRequest(CAMethod_t method, CAMessageType_t type, int totalMessages)
{
    return sendRequest((char*) SIM_REQ_ACK, s_tcInfo.identifier, method, type,
            totalMessages);
}

CAResult_t CAHelper::sendRequestWithCustomUri(char* resourceUri, CAMethod_t method, CAMessageType_t type,
        int totalMessages)
{
    return sendRequest(resourceUri, s_tcInfo.identifier, method, type, totalMessages);
}

CAResult_t CAHelper::sendRequestWithCustomPayload(char* payload, CAMethod_t method,
        CAMessageType_t type, int totalMessages)
{
    return sendRequest((char*) SIM_REQ_ACK, payload, method, type, totalMessages);
}

CAResult_t CAHelper::sendRequest(char* resourceUri, char* payload, CAMethod_t method, CAMessageType_t type,
        int totalMessages)
{
    return sendRequest(s_simulatorIp, resourceUri, payload, method, type, totalMessages);
}

CAResult_t CAHelper::sendRequest(char* address, char* resourceUri, char* payload, CAMethod_t method, CAMessageType_t type,
        int totalMessages)
{
    Serial.println("sendRequest --- started");

    s_tcInfo.caMethod = method;

    CAResult_t result = CA_STATUS_OK;
    

    CAEndpoint_t* remoteEndpoint = NULL;
    result = CACreateEndpoint(CA_IPV4, m_availableNetwork, s_simulatorIp, s_simulatorPort, &remoteEndpoint);
    if (result != CA_STATUS_OK)
    {
        Serial.println("CACreateEndpoint failed");
        return result;
    }

    printEndpoint(remoteEndpoint);

    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    result = CAGenerateToken(&token, tokenLength);
    if (result != CA_STATUS_OK)
    {
        Serial.println("CAGenerateToken failed");
        return result;
    }

    CAInfo_t requestData = {CA_MSG_RESET};
    requestData.token = token;
    requestData.tokenLength = tokenLength;
    requestData.type = type;
    requestData.payload = payload;
    requestData.resourceUri = (char *)OICMalloc(strlen(resourceUri) + 1);
    strcpy(requestData.resourceUri, resourceUri);
    requestData.payloadSize = strlen(requestData.payload);

    CARequestInfo_t requestInfo = {CA_GET, {CA_MSG_RESET}};
    requestInfo.method = method;
    requestInfo.isMulticast = false;
    requestInfo.info = requestData;
    
    //Send out request messages
    for (int messageCount = 0; messageCount < totalMessages; messageCount++)
    {
        Serial.println("Sending request to simulator, request no: ");
        Serial.println(messageCount + 1);
        result = CASendRequest(remoteEndpoint, &requestInfo);
        if (result != CA_STATUS_OK)
        {
            Serial.println("CASendRequest failed");
            return result;
        }
    }

    if (token != NULL)
    {
        CADestroyToken(token);
    }

    if (remoteEndpoint != NULL)
    {
        CADestroyEndpoint(remoteEndpoint);
    }

    Serial.println("sendRequest --- finished");

    return CA_STATUS_OK;
}

CAResult_t CAHelper::sendRequestToAll(CAMethod_t method, CAMessageType_t type, int totalMessages)
{
    return sendRequestToAll((char*) SIM_REQ_ACK, s_tcInfo.identifier, method, type,
            totalMessages);
}

CAResult_t CAHelper::sendRequestToAllWithCustomUri(char* resourceUri, CAMethod_t method,
        CAMessageType_t type, int totalMessages)
{
    return sendRequestToAll(resourceUri, s_tcInfo.identifier, method, type, totalMessages);
}

CAResult_t CAHelper::sendRequestToAllWithCustomPayload(char* payload, CAMethod_t method,
        CAMessageType_t type, int totalMessages)
{
    return sendRequestToAll((char*) SIM_REQ_ACK, payload, method, type,
            totalMessages);
}

CAResult_t CAHelper::sendRequestToAll(char* resourceUri, char* payload, CAMethod_t method,
        CAMessageType_t type, int totalMessages)
{
    Serial.println("sendRequestToAll --- started");

    Serial.print("Payload: ");
    Serial.println(payload);

    Serial.print("resourceUri: ");
    Serial.println(resourceUri);

    CAResult_t result = CA_STATUS_OK;
    
    //Construct Request
    CAEndpoint_t* endpoint = NULL;
    result = CACreateEndpoint(CA_IPV4, m_availableNetwork, s_simulatorIp, s_simulatorPort, &endpoint);

    if (result != CA_STATUS_OK)
    {
        return result;
    }

    // create token
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    result = CAGenerateToken(&token, tokenLength);

    if (result != CA_STATUS_OK)
    {
        Serial.println("token generate error!!");
        token = NULL;
        return result;
    }

    CAInfo_t requestData = {CA_MSG_RESET};
    requestData.token = token;
    requestData.tokenLength = tokenLength;
    requestData.type = type;
    requestData.payload = payload;
    requestData.resourceUri = (char *)OICMalloc(strlen(resourceUri) + 1);
    strcpy(requestData.resourceUri, resourceUri);
    requestData.payloadSize = strlen((const char *) payload);

    CARequestInfo_t requestInfo = {CA_GET, {CA_MSG_RESET}};
    requestInfo.method = method;
    requestInfo.info = requestData;
    requestInfo.isMulticast = true;

    // send request
    for (int messageCount = 0; messageCount < totalMessages; messageCount++)
    {
        Serial.print("Sending Request (Multicast) Message: ");
        Serial.println(messageCount+1);
        result = CASendRequest(endpoint, &requestInfo);
        if (result != CA_STATUS_OK)
        {
            return result;
        }
    }

    if (token != NULL)
    {
        CADestroyToken(token);
    }

    // destroy remote endpoint
    if (endpoint != NULL)
    {
        CADestroyEndpoint(endpoint);
    }


    Serial.println("sendRequestToAll --- finished");

    return result;
}

CAResult_t CAHelper::sendResponse(char* address, char* resourceUri, char* payload, CAResponseResult_t responseResult,
        CAMessageType_t type, int totalMessages)
{
    Serial.println("sendResponse --- started");
    CAResult_t result = CA_STATUS_OK;

    CAEndpoint_t* remoteEndpoint = NULL;
    result = CACreateEndpoint(CA_IPV4, m_availableNetwork, address, s_simulatorPort, &remoteEndpoint);
    if (result != CA_STATUS_OK)
    {
        return result;
    }

    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;

    result = CAGenerateToken(&token, tokenLength);
    if (result != CA_STATUS_OK)
    {
        return result;
    }

    CAInfo_t responseData = {CA_MSG_RESET};
    responseData.messageId = 0;
    responseData.token = token;
    responseData.tokenLength = tokenLength;
    responseData.payload = payload;
    responseData.type = type;
    responseData.messageId =  0;
    responseData.resourceUri = (char *)OICMalloc(strlen(resourceUri) + 1);
    strcpy(responseData.resourceUri, resourceUri);
    responseData.payloadSize = strlen((const char *) payload);

    CAResponseInfo_t responseInfo = {CA_BAD_REQ, {CA_MSG_RESET}};
    responseInfo.result = responseResult;
    responseInfo.info = responseData;

    int messageCount = 0;
    for (messageCount = 0; messageCount < totalMessages; messageCount++)
    {
        Serial.print("Sending response to simulator, response no: ");
        Serial.println(messageCount + 1);
        result = CASendResponse(remoteEndpoint, &responseInfo);

    }

    if (token != NULL)
    {
        CADestroyToken(token);
    }

    if (remoteEndpoint != NULL)
    {
        CADestroyEndpoint(remoteEndpoint);
    }

    Serial.println("sendResponse --- finished");
    return CA_STATUS_OK;
}

CAResult_t CAHelper::sendResponse(CAResponseResult_t responseResult, CAMessageType_t type,
        int totalMessages)
{
    return sendResponse(s_simulatorIp, (char*) SIM_RES_ACK, s_tcInfo.identifier, responseResult,
            type, totalMessages);
}

CAResult_t CAHelper::sendResponseWithCustomUri(char* uri, CAResponseResult_t responseResult,
        CAMessageType_t type, int totalMessages)
{
    return sendResponse(uri, s_tcInfo.identifier, responseResult, type, totalMessages);
}

CAResult_t CAHelper::sendResponseWithCustomPayload(char* payload, CAResponseResult_t responseResult,
        CAMessageType_t type, int totalMessages)
{
    return sendResponse((char*) SIM_RES_ACK, payload, responseResult, type,
            totalMessages);
}

CAResult_t CAHelper::sendResponse(char* resourceUri, char* payload, CAResponseResult_t responseResult,
        CAMessageType_t type, int totalMessages)
{
    return sendResponse(resourceUri, payload, responseResult, type, totalMessages);
}

bool CAHelper::isReceiveError(CAResult_t result, int index)
{
    return isReceiveError(result, index, TOTAL_MESSAGE);
}

bool CAHelper::isReceiveError(CAResult_t result, int index, int cnt)
{
    return (result != CA_STATUS_OK || s_receiveCount[index] != cnt);
}

bool CAHelper::isMulticastReceiveError(CAResult_t result, int index)
{
    return isMulticastReceiveError(result, index, TOTAL_MESSAGE);
}

bool CAHelper::isMulticastReceiveError(CAResult_t result, int index, int cnt)
{
    return (result != CA_STATUS_OK || s_receiveCount[index] != cnt * s_networkCount);
}

CAResult_t CAHelper::receiveMulticastMessage(int flag)
{
    return receiveMessage(TOTAL_MESSAGE * s_networkCount, MAX_ATTEMPT * s_networkCount, flag);
}

CAResult_t CAHelper::receiveMulticastMessage(int totalMessages, int flag)
{
    int attempts;
    
    if(totalMessages < 5)
        attempts = 5;
    else
        attempts = MAX_ATTEMPT;
    return receiveMessage(TOTAL_MESSAGE * s_networkCount, attempts * s_networkCount, flag);
}

CAResult_t CAHelper::receiveMessage(int flag)
{
    return receiveMessage(TOTAL_MESSAGE, MAX_ATTEMPT, flag);
}

CAResult_t CAHelper::receiveMessage(int totalMessages, int flag)
{
    int attempts;
    
    if(totalMessages < 5)
        attempts = 5;
    else
        attempts = MAX_ATTEMPT;

    return receiveMessage(TOTAL_MESSAGE, attempts, flag);
}

CAResult_t CAHelper::receiveMessage(int totalMessages, int maxAttempts, int flag)
{
    CAResult_t result = CA_STATUS_OK;

    for (int i = 0; i < maxAttempts; i++)
    {
        Serial.print("Receive attempt: ");
        Serial.println(i + 1);

        delay (waitTimes[flag % 2]);

        result = CAHandleRequestResponse();

        if (result != CA_STATUS_OK)
        {
            return result;
        }

        if (getReceiveCount(flag) >= totalMessages)
        {
            break;
        }
    }

    return result;
}
void CAHelper::getData(char *readInput, size_t bufferLength, size_t *dataLength)
{
    if (!readInput || bufferLength == 0 || !dataLength)
    {
        Serial.println("Invalid buffer");
        return;
    }

    while (!Serial.available())
    {
        delay(500);
    }
    
    int len = 0;
    while (Serial.available())
    {
        delay(100);
        char c = Serial.read();
        if ('\n' != c && '\r' != c && len < bufferLength - 1)
        {
            readInput[len++] = c;
        }
        else
        {
            break;
        }
    }

    readInput[len] = '\0';
    Serial.flush();
    Serial.print("PD:");
    Serial.println(readInput);
    (*dataLength) = len;
}

void CAHelper::addFilter()
{
    Test::exclude("*");

    char buffer[MAX_BUF_LEN] = {0};
    size_t len;

    Serial.println("Input filter (provide * to include all)");

    getData(buffer, sizeof(buffer), &len);

    Serial.flush();

    Test::include(buffer);

    Serial.print("Included Filter: ");

    Serial.println(buffer);
}
