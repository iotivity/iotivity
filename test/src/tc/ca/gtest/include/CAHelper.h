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

#include <cstdlib>
#include <cstring>
#include <ctime>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <map>
#include <set>

#include "gtest_custom.h"
#include "gtest/gtest.h"
#include "oic_malloc.h"
#include "oic_string.h"

#include "cacommon.h"
#include "cainterface.h"
#include "cautilinterface.h"
#ifdef __WITH_DTLS__
#include "ca_adapter_net_ssl.h"
#include "ssl_ciphersuites.h"
#endif

#include "CommonUtil.h"
#include "CommonTestUtil.h"
#include "IotivityTest_Logger.h"

#ifdef __TIZEN__
#include <glib.h>
#include <pthread.h>
#endif

#define PAYLOAD_SIZE_NORMAL 10
#define PAYLOAD_SIZE_LARGE 1500

static const int BLOCKWISE_PACKET_SIZE = 1500;

#define RESOURCE_URI_LENGTH 14

#define IDENTITY     ("1111111111111111")
#define RS_CLIENT_PSK   ("AAAAAAAAAAAAAAAA")

#define SLEEP_TIME 3
#define WAIT_SIM_CALLBACK 2000000         //waiting for given for simulators callback to be invoked (seconds)
#define WAIT_SIM_DIRECT 2000000           //waiting time between data receive callbacks when simulator sends all at once. (microseconds)

#define SIM_REQ_CONFIG "Configure"
#define SIM_REQ_ACK "SendReqAck"
#define SIM_REQ_QUERY "SendReqQry"
#define SIM_RES_ACK "SendResAck"

#define MAX_BUF_LEN 2048
#define MAX_OPT_LEN 16

#define WRONG_ADDRESS "107.109.214.101"
#define SIM_REQ_ACK_WRONG_EP_URI WRONG_ADDRESS ":5283/" SIM_REQ_ACK

#define REC_NOR 0
#define REC_ACK 1
#define REC_SECOND_NOR 3

#define CASENDREPONSE 0

const int TOTAL_MESSAGE = 1;
const int MAX_ATTEMPT = 25;
const int MAX_INTERVAL = 0;
const int TRY_COUNT = 3;
const int NETWORK_OUT_OF_BOUNDARY_VALUE = 0;
const uint8_t TOKEN_MAX_LENGTH = CA_MAX_TOKEN_LEN;
const uint8_t TOKEN_MIN_LENGTH = 1;
const uint8_t TOKEN_LOBV_LENGTH = -1;
const uint8_t TOKEN_UOBV_LENGTH = CA_MAX_TOKEN_LEN + 1;
const char* const VALID_ENDPOINT_URI = (char *) "123.123.123.123:1234/b/light";
const int ENDPOINT_PORT = 6298;
const char* const ENDPOINT_IP = (char *) "107.109.214.164";
const CATransportFlags_t CA_TRANSPORT_FLAG = (CATransportFlags_t)CA_IPV4;
const CATransportFlags_t CA_INVALID_FLAG = (CATransportFlags_t)0;
const CATransportAdapter_t CA_INVALID_ADAPTER = (CATransportAdapter_t)0;
const CATransportAdapter_t CA_INVALID_UOBV_ADAPTER = (CATransportAdapter_t)4294967296LL;
const CAMethod_t CA_INVALID_METHOD = (CAMethod_t)100;

typedef enum
{
    MESSAGE_INCOMING = 0, MESSAGE_OUTGOING
} MessageInOutType;

typedef enum
{
    MESSAGE_PAYLOAD = 0, MESSAGE_URI, MESSAGE_HEADER
} MessageDataType;

typedef enum
{
    MESSAGE_UNICAST = 0, MESSAGE_MULTICAST
} MessageCommunicationType;

typedef enum
{
    SEND_MESSAGE = 0, SELECT_NETWORK, UNSELECT_NETWORK, STOP_SIM, TRANSFER_INFO
} SimulatorTask;

typedef enum
{
    URI_NULL = 0, URI_WRONG, RESOURCE_URI_NULL, RESOURCE_URI_EMPTY, RESOURCE_URI_NORMAL, RESOURCE_URI_LARGE, RESOURCE_URI_CONFIG
} TcUriType;

typedef enum
{
    PAYLOAD_NULL = 0, PAYLOAD_EMPTY, PAYLOAD_NORMAL, PAYLOAD_LARGE, PAYLOAD_CONFIG
} TcPayloadType;

typedef enum
{
    SECURITY_NO = 0, SECURITY_YES
} TcSecurityType;

typedef enum
{
    MESSAGE_RESPONSE = 0, MESSAGE_REQUEST
} MessageCommandType;

typedef struct
{
    MessageInOutType inOutType;
    CAMethod_t caMethod;
    MessageDataType validationMethod;
    MessageCommunicationType communicationType;
    TcUriType uriType;
    TcPayloadType payloadType;
    TcSecurityType securityType;
    CAMessageType_t messageType;
    int total;
    CAResult_t result;

    char identifier[MAX_BUF_LEN];
    CAHeaderOption_t *options;
    uint8_t numOptions;
} TestCaseInfo;

class CAHelper
{
protected:
    int waitTimes[2];
    int m_clientReceivedCount;
    int m_clientAckCount;
    bool m_clientPreConditionPass;
    bool m_multicastRequest;
    std::ostringstream m_failure_message_stream;

private:
    static bool checkRequestResponseHandler(const CAEndpoint_t* endpoint,
            const CARequestInfo_t* requestInfo);
    static bool checkRequestResponseHandler(const CAEndpoint_t* endpoint,
            const CAResponseInfo_t* responseInfo);
    static bool isHostUnknown(const CAEndpoint_t* endpoint);
    static bool CheckResponseInfoAttributes(const CAResponseInfo_t* responseInfo);
    static bool CheckRequestInfoAttributes(const CARequestInfo_t* requestInfo);
    static bool CheckRemoteEndpointAttributes(const CAEndpoint_t* endpoint, char* resourceUri);
    static void comparePrefixAndIncrement(const CAEndpoint_t* endpoint, const char* resourceUri, const char *q, int index);
    static int compareAndIncrement(const char* endpointResourceUri,
            const char *otherResourceUri, const char* payload, int index);

public:
    CAHelper();
    static bool s_bufferEmpty;
    static int s_networkCount;
    static int s_isHostAddressKnown;
    static std::map< std::string, std::map< int, std::map< int, int > >>s_mapReceiveCount;
    static char s_simulatorIp[CA_MACADDR_SIZE];
    static int s_simulatorPort;
    static int s_simulatorSecurePort;
    static std::set<std::string> setIp;
    static int s_isValidAddress;

    CAResponseResult_t m_responseResult;

#ifdef __WITH_DTLS__
    static size_t s_identityLegth;
    static size_t s_pskLength;
    bool setDtls();
    static void initCipherSuiteList(bool * list, const char* deviceId);
    static CAResult_t dtlsHandshakeCb(const CAEndpoint_t *endpoint, const CAErrorInfo_t *info);
    static int32_t getDtlsPskCredentials( CADtlsPskCredType_t type, const unsigned char *desc, size_t desc_len, unsigned char *result, size_t result_length);
#endif

#ifdef TCP_ADAPTER
    static int keepAliveCount;
#endif

    static TestCaseInfo s_tcInfo;
    std::string m_failureMessage;

    CAEndpoint_t* m_endpoint;
    CAToken_t m_token;
    CAInfo_t m_caInfo;
    CAResult_t m_result;
    CARequestInfo_t m_requestInfo;
    CAResponseInfo_t m_responseInfo;

    CATransportAdapter_t m_availableNetwork;
    CATransportAdapter_t m_unAvailableNetwork;

    void setUriType(TcUriType uriType);
    void setPayloadType(TcPayloadType payloadType);
    void setMethodType(CAMethod_t method);
    void setSecurityType(TcSecurityType securityType);
    void setMessageType(CAMessageType_t messageType);
    void setResponseResult(CAResponseResult_t responseResult);
    void setTotalMessage(int total);
    void setResultStatus(CAResult_t result);
    void setMulticastRequest(bool multicastRequest);


    void clearBuffer();
    static std::string getRandomString(size_t length);
    static void getNumber(int num, char str[]);
    static std::string getNumber(int num);
    static int getReceiveCount(int index);
    void setupTestCase(MessageInOutType inOutType, MessageDataType validationMethod,
            MessageCommunicationType communicationType);

    bool initialize();
    bool selectNetwork();
    CATransportAdapter_t getselectNetwork();
    bool selectNetwork(int interestedNetwork, CAResult_t expectedResult);
    bool unselectNetwork();
    bool unselectNetwork(int interestedNetwork, CAResult_t expectedResult);
    bool startDiscoveryServer();
    bool startDiscoveryServer(CAResult_t expectedResult);
    bool startListeningServer();
    bool startListeningServer(CAResult_t expectedResult);
    bool createEndpoint(bool isMulticast, bool isSecure);
    bool createEndpoint(bool isMulticast, bool isSecure, CATransportFlags_t transportFlags);
    bool createEndpoint(bool isMulticast, bool isSecure, CAResult_t expectedResult);
    bool createEndpoint(CATransportFlags_t transportFlags, char* address, int port);
    bool createEndpoint(CATransportFlags_t transportFlags, CATransportAdapter_t adapter, const char *addr, uint16_t port, CAEndpoint_t* endpointObject, CAResult_t expectedResult);
    bool destroyEndpoint();
    bool generateToken();
    bool generateToken(CAResult_t expectedResult);
    bool destroyToken();

    bool initNetwork();
    bool initNetwork(bool select);
    bool initClientNetwork();
    bool initClientNetwork(bool select);
    bool initServerNetwork();
    bool initServerNetwork(bool select);
    bool getNetworkInfo();
    bool showNetworkInfo();

    static void errorHandler(const CAEndpoint_t *object, const CAErrorInfo_t* errorInfo);

    static void responseHandler(const CAEndpoint_t* endpoint,
            const CAResponseInfo_t* responseInfo);
    static void requestHandler(const CAEndpoint_t* endpoint,
            const CARequestInfo_t* requestInfo);
    static void responseHandlerSecond(const CAEndpoint_t* endpoint,
            const CAResponseInfo_t* responseInfo);

    bool sendSecuredRequest(CAMethod_t method, CAMessageType_t type);
    bool sendSecuredRequest(char* payload, CAMethod_t method, CAMessageType_t type);

    bool sendRequest(CAMethod_t method, CAMessageType_t type);
    bool sendRequest(char* uri, char* payload, CAMethod_t method, CAMessageType_t type);
    bool sendRequest(char* uri, char* payload, CAMethod_t method, CAMessageType_t type, bool isSecure,
                CAResult_t expectedResult);

    bool sendConfigurationRequest(SimulatorTask taskType, MessageCommandType msgType,
            CAMethod_t method);
    bool sendConfigurationRequest(SimulatorTask taskType, MessageCommandType msgType,
            int TOTAL_MESSAGE, int timeInterval, CAMethod_t method);

    bool sendResponse(CAResponseResult_t responseResult, CAMessageType_t type);
    bool sendResponse(char* uri, char* payload, CAResponseResult_t responseResult,
            CAMessageType_t type);
    bool sendResponse(char* uri, char* payload, CAResponseResult_t responseResult,
                CAMessageType_t type, CAResult_t expectedResult);

    bool attemptReceiveMessage(int flag);
    bool attemptReceiveMessage(int totalMessages, int maxAttempts, int flag);

    bool countReceiveMessage(int index);
    bool countReceiveMessage(int index, int cnt);
    bool countMulticastReceiveMessage(int index);
    bool countMulticastReceiveMessage(int index, int cnt);

    const char* getResultName(CAResult_t result);

    const char* getFailureMessage();
    const char* getReceiveFailureMessage(int index);
    const char* getFailureMessage(const char* apiName);
    const char* getFailureMessage(const char* apiName, CAResult_t expectedResult);
    const char* getFailureMessage(const char* apiName, CAResult_t returnedResult, CAResult_t expectedResult);

    static bool checkHeader(CAHeaderOption_t *options, uint32_t len);

    void getOptionData(CAInfo_t* requestData);

    bool stopListeningServer();
    bool stopListeningServer(CAResult_t expectedResult);
    void setAvailableNetwork(CATransportAdapter_t interestedNetwork);
    bool establishConnectionWithServer();
    bool establishConnectionWithServer(bool isClientAlreadyStarted);
    static bool parseAddress(const CAEndpoint_t* endpoint, const CAInfo_t info);

#ifdef TCP_ADAPTER
    static void keepAliveHandler(const CAEndpoint_t *endpoint, bool isConnected, bool isClient);
    int getKeepAliveCount();
#endif

};
#endif
