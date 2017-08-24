/*****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

#include "platform_features.h"
#include "gtest/gtest.h"
#include "cainterface.h"
#include "cautilinterface.h"
#include "cacommon.h"
#include "oic_string.h"
#include "oic_malloc.h"
#include "octypes.h"
#include "cafragmentation.h"
#include "caleinterface.h"
#include "cawsadapter.h"
#include <string.h>

static char *addr = NULL;
static CAEndpoint_t *tempRep = NULL;
static CARequestInfo_t requestInfo;
static CAInfo_t requestData;
static CAInfo_t responseData;
static CAResponseInfo_t responseInfo;
static CAToken_t tempToken = NULL;
static uint8_t tokenLength = CA_MAX_TOKEN_LEN;
static const char ADDRESS[] = "10.11.12.13";
static const uint16_t PORT = 7681;

class CAWSClientTests : public testing::Test
{
    protected:
        virtual void SetUp()
        {
            caglobals.client = true;
            caglobals.server = false;
            CAInitialize(CA_ADAPTER_WS);
        }

        virtual void TearDown()
        {
            CATerminate();
        }
};

class CAWSServerTests : public testing::Test
{
    protected:
        virtual void SetUp()
        {
            caglobals.client = false;
            caglobals.server = true;
            CAInitialize(CA_ADAPTER_WS);
        }

        virtual void TearDown()
        {
            CATerminate();
        }
};

// CAInitialize TC
TEST(WSInitializeTest, CAWSInitializeTest)
{
    EXPECT_EQ(CA_STATUS_OK, CAInitialize(CA_ADAPTER_WS));
    CATerminate();
}

//CATerminate TC
TEST_F(CAWSClientTests, WSClientTerminateTest)
{
    CATerminate();

    char *check = (char *) "terminate success";
    EXPECT_STREQ(check, "terminate success");

    CAInitialize(CA_ADAPTER_WS);
}

//CATerminate TC
TEST_F(CAWSServerTests, WSServerTerminateTest)
{
    CATerminate();

    char *check = (char *) "terminate success";
    EXPECT_STREQ(check, "terminate success");

    CAInitialize(CA_ADAPTER_WS);
}

// CAStartListeningServer TC
TEST_F(CAWSServerTests, WSStartListeningServerTestWithNonSelect)
{
    EXPECT_EQ(CA_STATUS_FAILED, CAStartListeningServer());
}

// CAStartListeningServer TC
TEST_F(CAWSServerTests, WSStartListeningServerTest)
{
    EXPECT_EQ(CA_STATUS_OK, CASelectNetwork(CA_ADAPTER_WS));
    EXPECT_EQ(CA_STATUS_OK, CAStartListeningServer());
}

// CAStartListeningServer TC
TEST_F(CAWSServerTests, WSStartDiscoveryServerTest)
{
    EXPECT_EQ(CA_STATUS_OK, CASelectNetwork(CA_ADAPTER_WS));
    EXPECT_EQ(CA_STATUS_OK, CAStartDiscoveryServer());
}

// CAStopListeningServer TC
TEST_F(CAWSServerTests, WSCAStopListeningServerTestWithNonSelect)
{
    EXPECT_EQ(CA_STATUS_FAILED, CAStopListeningServer());
}

// CAStopListeningServer TC
TEST_F(CAWSServerTests, WSCAStopListeningServerTest)
{
    EXPECT_EQ(CA_STATUS_OK, CASelectNetwork(CA_ADAPTER_WS));
    EXPECT_EQ(CA_STATUS_OK, CAStopListeningServer());
}

// CACreateRemoteEndpoint TC
TEST_F(CAWSClientTests, WSCreateRemoteEndpointTestGood)
{
    addr = (char *) ADDRESS;

    EXPECT_EQ(CA_STATUS_OK, CACreateEndpoint(CA_DEFAULT_FLAGS, CA_ADAPTER_WS, addr,
              PORT, &tempRep));

    CADestroyEndpoint(tempRep);
    tempRep = NULL;
}

// check remoteEndpoint and values of remoteEndpoint
TEST_F(CAWSClientTests, WSCreateRemoteEndpointTestValues)
{
    addr = (char *) ADDRESS;

    CACreateEndpoint(CA_DEFAULT_FLAGS, CA_ADAPTER_WS, addr, PORT, &tempRep);

    EXPECT_TRUE(tempRep != NULL);

    CADestroyEndpoint(tempRep);
    tempRep = NULL;
}

// CASendRequest TC
// check return value when a NULL is passed instead of a valid CARequestInfo_t address
TEST_F(CAWSClientTests, WSSendRequestTestWithNullAddr)
{
    addr = (char *) ADDRESS;
    CACreateEndpoint(CA_DEFAULT_FLAGS, CA_ADAPTER_WS, addr, PORT, &tempRep);

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CASendRequest(tempRep, NULL));

    CADestroyEndpoint(tempRep);
    tempRep = NULL;
}

// CASendRequest TC
// check return value when a request with null payload is made to loopback address
TEST_F(CAWSClientTests, WSSendRequestTestWithNullPayload)
{
    EXPECT_EQ(CA_STATUS_OK, CASelectNetwork(CA_ADAPTER_WS));
    CACreateEndpoint(CA_DEFAULT_FLAGS, CA_ADAPTER_WS, "127.0.0.1", PORT, &tempRep);

    memset(&requestData, 0, sizeof(CAInfo_t));
    CAGenerateToken(&tempToken, tokenLength);
    requestData.token = tempToken;
    requestData.tokenLength = tokenLength;
    requestData.type = CA_MSG_CONFIRM;
    requestData.resourceUri = "/a/light";
    requestData.acceptFormat = CA_FORMAT_APPLICATION_VND_OCF_CBOR;
    requestData.acceptVersion = OC_SPEC_VERSION_VALUE;

    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;
    requestInfo.isMulticast = false;

    EXPECT_EQ(CA_STATUS_OK, CASendRequest(tempRep, &requestInfo));

    CADestroyToken(tempToken);
    CADestroyEndpoint(tempRep);
    free(requestData.payload);
    tempRep = NULL;
}

// CASendResponse TC
TEST_F(CAWSServerTests, WSSendResponseTestWithInvalidCode)
{
    EXPECT_EQ(CA_STATUS_OK, CASelectNetwork(CA_ADAPTER_WS));

    addr = (char *) ADDRESS;
    CACreateEndpoint(CA_DEFAULT_FLAGS, CA_ADAPTER_WS, addr, PORT, &tempRep);

    memset(&responseData, 0, sizeof(CAInfo_t));
    responseData.type = CA_MSG_RESET;
    responseData.messageId = 1;
    responseData.payload = (CAPayload_t)OICMalloc(sizeof("response payload"));
    responseData.dataType = CA_RESPONSE_DATA;

    EXPECT_TRUE(responseData.payload != NULL);

    if (responseData.payload)
    {
        CAGenerateToken(&tempToken, tokenLength);
        requestData.token = tempToken;
        requestData.tokenLength = tokenLength;

        memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
        responseInfo.result = CA_CONTENT;
        responseInfo.info = responseData;

        EXPECT_EQ(CA_STATUS_OK, CASendResponse(tempRep, &responseInfo));

        CADestroyToken(tempToken);
        CADestroyEndpoint(tempRep);
        OICFree(responseData.payload);
        tempRep = NULL;
    }
}

// check return value NULL is passed instead of a valid CAResponseInfo_t address
TEST_F(CAWSServerTests, WSSendResponseTest)
{
    addr = (char *) ADDRESS;
    CACreateEndpoint(CA_DEFAULT_FLAGS, CA_ADAPTER_WS, addr, PORT, &tempRep);

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CASendResponse(tempRep, NULL));

    CADestroyEndpoint(tempRep);
    tempRep = NULL;
}

// check the stack response when a packet is received
// Use the CAGetWSNetworkPacketReceivedCallback() api
// provided by the websocket adapter
TEST_F(CAWSServerTests, WSServerPacketReceivedTest)
{
    CANetworkPacketReceivedCallback packetReceivedCallback =
        CAGetWSNetworkPacketReceivedCallback();

    CACreateEndpoint(CA_DEFAULT_FLAGS, CA_ADAPTER_WS, "127.0.0.1", PORT, &tempRep);
    CASecureEndpoint_t sep;
    memset(&sep, 0, sizeof(sep));
    sep.endpoint = *tempRep;

    int data[] = { 0x08, 0x01, 0x89, 0x21, 0x5A, 0xD5, 0xCA, 0x17, 0xFE,
                   0xD1, 0xB3, 0x6F, 0x69, 0x63, 0x03, 0x72, 0x65, 0x73,
                   0x62, 0x27, 0x10, 0xE2, 0x06, 0xE3, 0x08, 0x00, 0xE1,
                   0xF6, 0xE6, 0xC0
                 };
    packetReceivedCallback(&sep, data, sizeof(data));

    char *check = (char *) "success";
    EXPECT_STREQ(check, "success");
    CADestroyEndpoint(tempRep);
    tempRep = NULL;
}

// check the stack response when a packet is received
// Use the CAGetWSNetworkPacketReceivedCallback() api
// provided by the websocket adapter
TEST_F(CAWSClientTests, WSClientPacketReceivedTest)
{
    CANetworkPacketReceivedCallback packetReceivedCallback =
        CAGetWSNetworkPacketReceivedCallback();

    CACreateEndpoint(CA_DEFAULT_FLAGS, CA_ADAPTER_WS, "127.0.0.1", PORT, &tempRep);
    CASecureEndpoint_t sep;
    memset(&sep, 0, sizeof(sep));
    sep.endpoint = *tempRep;

    int data[] = { 0x08, 0x01, 0x89, 0x21, 0x5A, 0xD5, 0xCA, 0x17, 0xFE,
                   0xD1, 0xB3, 0x6F, 0x69, 0x63, 0x03, 0x72, 0x65, 0x73,
                   0x62, 0x27, 0x10, 0xE2, 0x06, 0xE3, 0x08, 0x00, 0xE1,
                   0xF6, 0xE6, 0xC0
                 };
    packetReceivedCallback(&sep, data, sizeof(data));

    char *check = (char *) "success";
    EXPECT_STREQ(check, "success");

    CADestroyEndpoint(tempRep);
    tempRep = NULL;
}

TEST_F(CAWSClientTests, SendRequestTestWithInvalidAddress)
{
    EXPECT_EQ(CA_STATUS_OK, CASelectNetwork(CA_ADAPTER_WS));
    CACreateEndpoint(CA_DEFAULT_FLAGS, CA_ADAPTER_WS, "10.11.13.13.14", PORT, &tempRep);

    memset(&requestData, 0, sizeof(CAInfo_t));
    CAGenerateToken(&tempToken, tokenLength);
    requestData.token = tempToken;
    requestData.tokenLength = tokenLength;
    requestData.type = CA_MSG_CONFIRM;

    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    EXPECT_EQ(CA_STATUS_OK, CASendRequest(tempRep, &requestInfo));

    CADestroyToken(tempToken);
    CADestroyEndpoint(tempRep);
    free(requestData.payload);
    tempRep = NULL;
}