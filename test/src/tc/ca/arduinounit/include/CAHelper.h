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

#ifndef INCLUDE_TESTCASE_CA_CAHELPER_H_
#define INCLUDE_TESTCASE_CA_CAHELPER_H_

#include "cacommon.h"
#include "cainterface.h"
#include "oic_malloc.h"
#include "Arduino.h"
#include <ArduinoUnit.h>

#ifdef ARDUINOWIFI
#include "WiFi.h"
#elif defined ARDUINOETH
#include "Ethernet.h"
#endif

#define ETHERNET_MAC_HEX 0x90, 0xA2, 0xDA, 0x0F, 0xE2, 0x25

#define CONFIG_SIMULATOR_IP "SIMULATOR.IP"
#define CONFIG_SIMULATOR_OTHER_IP_LIST "SIMULATOR.OTHER_IP_LIST"
#define CONFIG_SIMULATOR_PORT "SIMULATOR.PORT"
#define CONFIG_SIMULATOR_SECURE_PORT "SIMULATOR.SECURE_PORT"
#define CONFIG_SIMULATOR_CA_ETHERNET "SIMULATOR.CA_ETHERNET"
#define CONFIG_SIMULATOR_CA_WIFI "SIMULATOR.CA_WIFI"
#define CONFIG_SIMULATOR_CA_EDR "SIMULATOR.CA_EDR"
#define CONFIG_SIMULATOR_CA_LE "SIMULATOR.CA_LE"

#define MAX_OPT_LEN 16

#define SLEEP_TIME 2
#define WAIT_SIM_CALLBACK 800         //waiting for given for simulators callback to be invoked (miliseconds), Default: 200
#define WAIT_SIM_DIRECT 400      //waiting time between data receive callbacks when simulator sends all at once. (miliseconds), Default: 100

#define SIM_REQ_CONFIG "Configure"
#define SIM_REQ_ACK "SendReqAck"
#define SIM_RES_ACK "SendResAck"

#define MAX_BUF_LEN 1024
#define MAX_OPT_LEN 16
#define NETWORK_OUT_OF_BOUNDARY_VALUE 0
#define MAX_SLEEP_TIME 1
#define RANDOM_STRING_LENGTH 1500
#define CH_ZERO '0'

#define WRONG_IP "107.109.214.101"

#define REC_CLI_NOR 0
#define REC_CLI_ACK 1
#define REC_SVR_NOR 2
#define REC_SVR_ACK 3
#define REC_SECOND_CLI_NOR 4
#define REC_SECOND_SVR_NOR 5

#define CASENDREPONSE 0

const int TOTAL_MESSAGE = 1;
const int MAX_ATTEMPT = 10;
const int MAX_INTERVAL = 0;
const int TRY_COUNT = 3;

typedef enum
{
    MESSAGE_NONSECURE = 0, MESSAGE_SECURE
} MessageSecurityType;

typedef enum
{
    MESSAGE_INCOMING = 0, MESSAGE_OUTGOING
} MessageInOutType;

typedef enum
{
    MESSAGE_PAYLOAD = 0, MESSAGE_URI, MESSAGE_EMPTY
} MessageDataType;

typedef enum
{
    MESSAGE_UNICAST = 0, MESSAGE_MULTICAST
} MessageCommunicationType;

typedef struct
{
    MessageSecurityType securityType;
    MessageInOutType inOutType;
    CAMethod_t caMethod;
    MessageDataType validationMethod;
    MessageCommunicationType communicationType;
    char* identifier;
} TestCaseInfo;

typedef enum
{
    SEND_MESSAGE = 0, SELECT_NETWORK, UNSELECT_NETWORK, STOP_SIM, TRANSFER_INFO
} SimulatorTask;

typedef enum
{
    MESSAGE_RESPONSE = 0, MESSAGE_REQUEST
} MessageCommandType;

class CAHelper
{
protected:
    int waitTimes[2];

private:
    static bool checkHandler(const CAEndpoint_t* remoteEndpoint,
            const CARequestInfo_t* requestInfo);
    static bool checkHandler(const CAEndpoint_t* remoteEndpoint,
            const CAResponseInfo_t* responseInfo);
    static bool isHostUnknown(const CAEndpoint_t* remoteEndpoint);
    static bool showAndCheckResponseInfoAttributes(const CAResponseInfo_t* responseInfo);
    static bool showAndCheckRequestInfoAttributes(const CARequestInfo_t* requestInfo);
    static bool showAndCheckRemoteEndpointAttributes(const CAEndpoint_t* remoteEndpoint);
    static void comparePrefixAndIncrement(const CARequestInfo_t* requestInfo, const char *q,
            int index);
    static void comparePrefixAndIncrement(const CAResponseInfo_t* responseInfo, const char *q,
                int index);
    static int compareAndIncrement(const char* remoteEndpointResourceUri,
            const char *otherResourceUri, const char* payload, int index);

public:
    CATransportAdapter_t m_availableNetwork;
    CATransportAdapter_t m_unAvailableNetwork;
//    String m_simulatorUri;
//    String m_simulatorSecureUri;
//    String m_simulatorConfigUri;
//    String m_simulatorResAckUri;
//    String m_simulatorReqAckUri;
//    String m_simulatorSecureReqAckUri;

    static TestCaseInfo s_tcInfo;
    static String s_testCaseIdentifier;
    static char s_simulatorIp[20];
    static int s_simulatorPort;
    static int s_simulatorSecurePort;
    static int s_receiveCount[6];
    static bool s_bufferEmpty;
    static int s_networkCount;

    CAHelper();
    void Connect();
    void Connect(bool isSetConf);
    bool setConf();
    void setupTestCase(MessageInOutType inOutType, MessageDataType validationMethod,
            MessageCommunicationType communicationType);
    static String getRandomString(size_t length);
    String formatStringFromInteger(int number, int stringLength);
    void printEndpoint(CAEndpoint_t* rep);
    void printReceiveError(String testLog, int index);
    void printReceiveError(String testLog, int expectedMessageCount, int index);
    static int getReceiveCount(int index);

    bool initNetwork();
    bool initNetwork(bool select);
    bool initClientNetwork();
    bool initClientNetwork(bool select);
    bool initServerNetwork();
    bool initServerNetwork(bool select);

    CAResult_t sendConfigurationRequest(SimulatorTask taskType, MessageCommandType msgType,
            CAMethod_t method);
    CAResult_t sendConfigurationRequest(SimulatorTask taskType, MessageCommandType msgType,
            int TOTAL_MESSAGE, int timeInterval, CAMethod_t method);

    CAResult_t sendRequest(CAMethod_t method, CAMessageType_t type, int totalMessages);
    CAResult_t sendRequest(char* uri, char* payload, CAMethod_t method, CAMessageType_t type,
            int totalMessages);
    CAResult_t sendRequest(char* address, char* uri, char* payload, CAMethod_t method, CAMessageType_t type,
                int totalMessages);
    CAResult_t sendRequestWithCustomUri(char* uri, CAMethod_t method, CAMessageType_t type,
            int totalMessages);
    CAResult_t sendRequestWithCustomPayload(char* payload, CAMethod_t method, CAMessageType_t type,
            int totalMessages);

    CAResult_t sendRequestToAll(CAMethod_t method, CAMessageType_t type, int totalMessages);
    CAResult_t sendRequestToAll(char* uri, char* payload, CAMethod_t method, CAMessageType_t type,
            int totalMessages);
    CAResult_t sendRequestToAllWithCustomUri(char* uri, CAMethod_t method, CAMessageType_t type,
            int totalMessages);
    CAResult_t sendRequestToAllWithCustomPayload(char* payload, CAMethod_t method,
            CAMessageType_t type, int totalMessages);

    CAResult_t sendResponse(CAResponseResult_t responseResult, CAMessageType_t type,
            int totalMessages);
    CAResult_t sendResponse(char* uri, char* payload, CAResponseResult_t responseResult,
            CAMessageType_t type, int totalMessages);
    CAResult_t sendResponseWithCustomUri(char* uri, CAResponseResult_t responseResult,
            CAMessageType_t type, int totalMessages);
    CAResult_t sendResponseWithCustomPayload(char* payload, CAResponseResult_t responseResult,
            CAMessageType_t type, int totalMessages);
    CAResult_t sendResponse(char* resourceUri, char* uri, char* payload, CAResponseResult_t responseResult,
                CAMessageType_t type, int totalMessages);

    CAResult_t receiveMessage(int flag);
    CAResult_t receiveMessage(int totalMessages, int flag);
    CAResult_t receiveMessage(int totalMessages, int maxAttempts, int flag);
    CAResult_t receiveMulticastMessage(int flag);
    CAResult_t receiveMulticastMessage(int totalMessages, int flag);

    bool isReceiveError(CAResult_t result, int index);
    bool isReceiveError(CAResult_t result, int index, int cnt);
    bool isMulticastReceiveError(CAResult_t result, int index);
    bool isMulticastReceiveError(CAResult_t result, int index, int cnt);

    static void handleClientResponse(const CAEndpoint_t* remoteEndpoint,
            const CAResponseInfo_t* responseInfo);
    static void handleClientRequest(const CAEndpoint_t* remoteEndpoint,
            const CARequestInfo_t* requestInfo);

    static void handleServerResponse(const CAEndpoint_t* remoteEndpoint,
            const CAResponseInfo_t* responseInfo);
    static void handleServerRequest(const CAEndpoint_t* remoteEndpoint,
            const CARequestInfo_t* requestInfo);

    static void handleClientResponseSecond(const CAEndpoint_t* remoteEndpoint,
            const CAResponseInfo_t* responseInfo);
    static void handleServerRequestSecond(const CAEndpoint_t* remoteEndpoint,
            const CARequestInfo_t* requestInfo);
    
    static void handleError(const CAEndpoint_t *object, const CAErrorInfo_t* errorInfo);

    static void getData(char *readInput, size_t bufferLength, size_t *dataLength);
    bool parseData(char *buf, char *url, char *resourceUri);

    static void addFilter();
};

#endif /* INCLUDE_TESTCASE_CA_CAHELPER_H_ */
