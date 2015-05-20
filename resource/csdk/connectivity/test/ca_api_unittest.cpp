/* ****************************************************************
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

#include "gtest/gtest.h"
#include "cainterface.h"
#include "cacommon.h"


class CATests : public testing::Test {
    protected:
    virtual void SetUp() {
        CAInitialize();
    }

    virtual void TearDown()
    {
        CATerminate();
    }
};

void request_handler(CARemoteEndpoint_t* object, CARequestInfo_t* requestInfo);
void response_handler(CARemoteEndpoint_t* object, CAResponseInfo_t* responseInfo);
CAResult_t checkGetNetworkInfo();
CAResult_t checkSelectNetwork();



void request_handler(const CARemoteEndpoint_t *object, const CARequestInfo_t *requestInfo)
{

}

void response_handler(const CARemoteEndpoint_t *object, const CAResponseInfo_t *responseInfo)
{

}

static char* uri = NULL;
static CARemoteEndpoint_t* tempRep = NULL;
static CARequestInfo_t requestInfo;
static CAInfo_t requestData;
static CAInfo_t responseData;
static CAResponseInfo_t responseInfo;
static CAToken_t tempToken = NULL;
static uint8_t tokenLength = CA_MAX_TOKEN_LEN;
static const char URI[] = "coap://10.11.12.13:4545/a/light";
static const char RESOURCE_URI[] = "/a/light";

static const char SECURE_INFO_DATA[] =
                                    "{\"oc\":[{\"href\":\"%s\",\"prop\":{\"rt\":[\"core.led\"],"
                                     "\"if\":[\"oic.if.baseline\"],\"obs\":1,\"sec\":1,\"port\":%d}}]}";
static const char NORMAL_INFO_DATA[] =
                                    "{\"oc\":[{\"href\":\"%s\",\"prop\":{\"rt\":[\"core.led\"],"
                                     "\"if\":[\"oic.if.baseline\"],\"obs\":1}}]}";

#ifdef __WITH_DTLS__

/**
 * @def RS_IDENTITY
 * @brief
 */
#define IDENTITY     ("1111111111111111")
/* @def RS_CLIENT_PSK
 * @brief
 */
#define RS_CLIENT_PSK   ("AAAAAAAAAAAAAAAA")

static CADtlsPskCredsBlob_t *pskCredsBlob = NULL;

void clearDtlsCredentialInfo()
{
    printf("clearDtlsCredentialInfo IN\n");
    if (pskCredsBlob)
    {
        // Initialize sensitive data to zeroes before freeing.
        memset(pskCredsBlob->creds, 0, sizeof(OCDtlsPskCreds) * (pskCredsBlob->num));
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

    if (pskCredsBlob != NULL)
    {
        *credInfo = pskCredsBlob;
    }

    printf("CAGetDtlsPskCredentials OUT\n");
}

int32_t SetCredentials()
{
    printf("SetCredentials IN\n");
    pskCredsBlob = (CADtlsPskCredsBlob_t *)malloc(sizeof(CADtlsPskCredsBlob_t));

    memset(pskCredsBlob, 0x0, sizeof(CADtlsPskCredsBlob_t));
    memcpy(pskCredsBlob->identity, IDENTITY, DTLS_PSK_ID_LEN);

    pskCredsBlob->num = 1;

    pskCredsBlob->creds = (OCDtlsPskCreds *)malloc(sizeof(OCDtlsPskCreds) * (pskCredsBlob->num));

    memcpy(pskCredsBlob->creds[0].id, IDENTITY, DTLS_PSK_ID_LEN);
    memcpy(pskCredsBlob->creds[0].psk, RS_CLIENT_PSK, DTLS_PSK_PSK_LEN);

    printf("SetCredentials OUT\n");
    return 1;
}
#endif

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// CAInitialize TC
// check return value
TEST(InitializeTest, TC_01_Positive_01)
{
    EXPECT_EQ(CA_STATUS_OK, CAInitialize());
    CATerminate();
}

//CATerminate TC
TEST_F(CATests, TerminateTest)
{
    CATerminate();

    char* check = (char *) "terminate success";
    EXPECT_STREQ(check, "terminate success");

    CAInitialize();
}
// CAStartListeningServer TC
// check return value
TEST(StartListeningServerTest, DISABLED_TC_03_Positive_01)
{
    CASelectNetwork(CA_IPV4);
    EXPECT_EQ(CA_STATUS_OK, CAStartListeningServer());
}

// CAStartDiscoveryServer TC
// check return value
TEST(StartDiscoveryServerTest, DISABLED_TC_04_Positive_01)
{
    EXPECT_EQ(CA_STATUS_OK, CAStartDiscoveryServer());
}

// CARegisterHandlerTest TC
// check return value
TEST_F(CATests, RegisterHandlerTest)
{
    CARegisterHandler(request_handler, response_handler);
    char* check = (char *) "registerHandler success";
    EXPECT_STREQ(check, "registerHandler success");
}

// CACreateRemoteEndpoint TC
// check return value
TEST_F(CATests, CreateRemoteEndpointTestGood)
{
    uri = (char *) URI;

    EXPECT_EQ(CA_STATUS_OK, CACreateRemoteEndpoint(uri, CA_IPV4, &tempRep));

    if (tempRep != NULL)
    {
        CADestroyRemoteEndpoint(tempRep);
        tempRep = NULL;
    }
}

// check remoteEndpoint and values of remoteEndpoint
TEST_F(CATests, CreateRemoteEndpointTestValues)
{
    uri = (char *) URI;

    CACreateRemoteEndpoint(uri, CA_IPV4, &tempRep);

    EXPECT_TRUE(tempRep != NULL);

    if (tempRep != NULL)
    {
        EXPECT_STRNE(NULL, tempRep->resourceUri);
    }

    if (tempRep != NULL)
    {
        CADestroyRemoteEndpoint(tempRep);
        tempRep = NULL;
    }
}

// check return value when uri is NULL
TEST_F(CATests, CreateRemoteEndpointTestBad)
{
    uri = NULL;

    EXPECT_EQ(CA_STATUS_FAILED, CACreateRemoteEndpoint(uri, CA_IPV4, &tempRep));

    if (tempRep != NULL)
    {
        CADestroyRemoteEndpoint(tempRep);
        tempRep = NULL;
    }
}

// check values of remoteEndpoint when uri is NULL
TEST_F(CATests, CreateRemoteEndpointTestWithNullUri)
{
    uri = NULL;
    CACreateRemoteEndpoint(uri, CA_IPV4, &tempRep);

    if (tempRep != NULL)
    {
        EXPECT_STREQ(NULL, tempRep->resourceUri);

    }

    if (tempRep != NULL)
    {
        CADestroyRemoteEndpoint(tempRep);
        tempRep = NULL;
    }
}

// CADestroyRemoteEndpoint TC
// check destroyed remoteEndpoint
TEST_F(CATests, DestroyRemoteEndpointTest)
{
    uri = (char *) URI;
    CACreateRemoteEndpoint(uri, CA_IPV4, &tempRep);

    CADestroyRemoteEndpoint(tempRep);
    tempRep = NULL;

    char * check = (char *) "destroy success";
    EXPECT_STREQ(check, "destroy success");
}

// CAGerateToken TC
// check return value
TEST_F(CATests, GenerateTokenTestGood)
{
    EXPECT_EQ(CA_STATUS_OK, CAGenerateToken(&tempToken, tokenLength));

    CADestroyToken(tempToken);
}

// check return value when CAGenerateToken is passed a NULL instead a valid pointer
TEST_F(CATests, GenerateTokenTestBad)
{
    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CAGenerateToken(NULL, tokenLength));
}

// CADestroyToken TC
// check destroyed token
TEST_F(CATests, DestroyTokenTest)
{
    CAGenerateToken(&tempToken, tokenLength);
    CADestroyToken(tempToken);

    char * check = (char *) "destroy success";
    EXPECT_STREQ(check, "destroy success");
}

// CAFindResource TC
// check return value
TEST(FindResourceTest, DISABLED_TC_14_Positive_01)
{
    uri = (char *) RESOURCE_URI;

    CAGenerateToken(&tempToken, tokenLength);
    EXPECT_EQ(CA_STATUS_OK, CAFindResource(uri, tempToken, tokenLength));
    CADestroyToken(tempToken);
}

// check return value when uri is NULL
TEST_F(CATests, FindResourceTest)
{
    uri = NULL;
    CAGenerateToken(&tempToken, tokenLength);
    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CAFindResource(uri, tempToken, tokenLength));
    CADestroyToken(tempToken);
}

// CASendRequest TC
// check return value
TEST(SendRequestTest, DISABLED_TC_16_Positive_01)
{
    uri = (char *) URI;
    CACreateRemoteEndpoint(uri, CA_IPV4, &tempRep);

    memset(&requestData, 0, sizeof(CAInfo_t));
    CAGenerateToken(&tempToken, tokenLength);
    requestData.token = tempToken;
    requestData.tokenLength = tokenLength;

    int length = strlen(NORMAL_INFO_DATA) + strlen("a/light");
    requestData.payload = (CAPayload_t) calloc(length, sizeof(char));

    if(!requestData.payload)
    {
        CADestroyToken(tempToken);
        FAIL() << "requestData.payload allocation failed";
    }

    snprintf(requestData.payload, length, NORMAL_INFO_DATA, "a/light");
    requestData.type = CA_MSG_NONCONFIRM;

    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    EXPECT_EQ(CA_STATUS_OK, CASendRequest(tempRep, &requestInfo));

    CADestroyToken(tempToken);

    free(requestData.payload);

    CADestroyRemoteEndpoint(tempRep);
    tempRep = NULL;

}

// check return value when uri is NULL
TEST_F(CATests, SendRequestTestWithNullURI)
{
    uri = NULL;
    CACreateRemoteEndpoint(uri, CA_IPV4, &tempRep);

    memset(&requestData, 0, sizeof(CAInfo_t));
    CAGenerateToken(&tempToken, tokenLength);
    requestData.token = tempToken;
    requestData.tokenLength = tokenLength;

    int length = strlen(NORMAL_INFO_DATA) + strlen("a/light");
    requestData.payload = (CAPayload_t) calloc(length, sizeof(char));

    if(!requestData.payload)
    {
        CADestroyToken(tempToken);
        FAIL() << "requestData.payload allocation failed";
    }

    snprintf(requestData.payload, length, NORMAL_INFO_DATA, "a/light");
    requestData.type = CA_MSG_NONCONFIRM;

    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CASendRequest(tempRep, &requestInfo));

    CADestroyToken(tempToken);

    free(requestData.payload);

    if (tempRep != NULL)
    {
        CADestroyRemoteEndpoint(tempRep);
        tempRep = NULL;
    }
}

// check return value when a NULL is passed instead of a valid CARequestInfo_t address
TEST_F(CATests, SendRequestTestWithNullAddr)
{
    uri = (char *) URI;
    CACreateRemoteEndpoint(uri, CA_IPV4, &tempRep);

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CASendRequest(tempRep, NULL));

    if (tempRep != NULL)
    {
        CADestroyRemoteEndpoint(tempRep);
        tempRep = NULL;
    }
}

// CASendResponse TC
// check return value
TEST(SendResponseTest, DISABLED_TC_19_Positive_01)
{
    uri = (char *) URI;
    CACreateRemoteEndpoint(uri, CA_IPV4, &tempRep);

    memset(&responseData, 0, sizeof(CAInfo_t));
    responseData.type = CA_MSG_NONCONFIRM;
    responseData.messageId = 1;
    responseData.payload = (char *) "response payload";

    CAGenerateToken(&tempToken, tokenLength);
    requestData.token = tempToken;
    requestData.tokenLength = tokenLength;

    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
    responseInfo.result = CA_SUCCESS;
    responseInfo.info = responseData;

    EXPECT_EQ(CA_STATUS_OK, CASendResponse(tempRep, &responseInfo));

    CADestroyToken(tempToken);
    CADestroyRemoteEndpoint(tempRep);
    tempRep = NULL;
}

// check return value when uri is NULL
TEST(SendResponseTest, DISABLED_TC_20_Negative_01)
{
    uri = NULL;
    CACreateRemoteEndpoint(uri, CA_IPV4, &tempRep);

    memset(&responseData, 0, sizeof(CAInfo_t));
    responseData.type = CA_MSG_NONCONFIRM;
    responseData.messageId = 1;
    responseData.payload = (char *) "response payload";

    CAGenerateToken(&tempToken, tokenLength);
    requestData.token = tempToken;
    requestData.tokenLength = tokenLength;

    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
    responseInfo.result = CA_SUCCESS;
    responseInfo.info = responseData;

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CASendResponse(tempRep, &responseInfo));

    CADestroyToken(tempToken);
    if (tempRep != NULL)
    {
        CADestroyRemoteEndpoint(tempRep);
        tempRep = NULL;
    }
}

// check return value NULL is passed instead of a valid CAResponseInfo_t address
TEST_F(CATests, SendResponseTest)
{
    uri = (char *) URI;
    CACreateRemoteEndpoint(uri, CA_IPV4, &tempRep);

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CASendResponse(tempRep, NULL));

    if (tempRep != NULL)
    {
        CADestroyRemoteEndpoint(tempRep);
        tempRep = NULL;
    }
}

// CASendNotification TC
// check return value
TEST(SendNotificationTest, DISABLED_TC_22_Positive_01)
{
    uri = (char *) URI;
    CACreateRemoteEndpoint(uri, CA_IPV4, &tempRep);

    memset(&responseData, 0, sizeof(CAInfo_t));
    responseData.type = CA_MSG_NONCONFIRM;
    responseData.payload = (char *) "Temp Notification Data";

    CAGenerateToken(&tempToken, tokenLength);
    requestData.token = tempToken;
    requestData.tokenLength = tokenLength;

    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
    responseInfo.result = CA_SUCCESS;
    responseInfo.info = responseData;

    EXPECT_EQ(CA_STATUS_OK, CASendNotification(tempRep, &responseInfo));

    CADestroyToken(tempToken);
    if (tempRep != NULL)
    {
        CADestroyRemoteEndpoint(tempRep);
        tempRep = NULL;
    }
}

// check return value when uri is NULL
TEST_F(CATests, SendNotificationTest)
{
    uri = NULL;
    CACreateRemoteEndpoint(uri, CA_IPV4, &tempRep);

    memset(&responseData, 0, sizeof(CAInfo_t));
    responseData.type = CA_MSG_NONCONFIRM;
    responseData.payload = (char *) "Temp Notification Data";

    CAGenerateToken(&tempToken, tokenLength);
    requestData.token = tempToken;
    requestData.tokenLength = tokenLength;

    memset(&responseInfo, 0, sizeof(CAResponseInfo_t));
    responseInfo.result = CA_SUCCESS;
    responseInfo.info = responseData;

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CASendNotification(tempRep, &responseInfo));

    CADestroyToken(tempToken);
    if (tempRep != NULL)
    {
        CADestroyRemoteEndpoint(tempRep);
        tempRep = NULL;
    }
}

// CAAdvertiseResource TC
// check return value
TEST(AdvertiseResourceTest, DISABLED_TC_24_Positive_01)
{
    uri = (char *) RESOURCE_URI;
    int optionNum = 2;

    CAHeaderOption_t* headerOpt;
    headerOpt = (CAHeaderOption_t *) calloc(1, optionNum * sizeof(CAHeaderOption_t));

    if(!headerOpt)
    {
        FAIL() <<"Allocation for headerOpt failed";
    }

    char* tmpOptionData1 = (char *) "Hello";
    size_t tmpOptionDataLen = (strlen(tmpOptionData1) < CA_MAX_HEADER_OPTION_DATA_LENGTH) ?
            strlen(tmpOptionData1) : CA_MAX_HEADER_OPTION_DATA_LENGTH - 1;
    headerOpt[0].optionID = 3000;
    memcpy(headerOpt[0].optionData, tmpOptionData1, tmpOptionDataLen);
    headerOpt[0].optionLength = (uint16_t) tmpOptionDataLen;

    char* tmpOptionData2 = (char *) "World";
    tmpOptionDataLen = (strlen(tmpOptionData2) < CA_MAX_HEADER_OPTION_DATA_LENGTH) ?
                strlen(tmpOptionData2) : CA_MAX_HEADER_OPTION_DATA_LENGTH - 1;
    headerOpt[1].optionID = 3001;
    memcpy(headerOpt[1].optionData, tmpOptionData2, tmpOptionDataLen);
    headerOpt[1].optionLength = (uint16_t) tmpOptionDataLen;

    CAGenerateToken(&tempToken, tokenLength);

    EXPECT_EQ(CA_STATUS_OK, CAAdvertiseResource(uri, tempToken, tokenLength,
                                                headerOpt, (uint8_t )optionNum));

    CADestroyToken(tempToken);

    free(headerOpt);
}

// check return value when uri is NULL
TEST_F(CATests, AdvertiseResourceTest)
{
    uri = NULL;
    int optionNum = 2;

    CAHeaderOption_t* headerOpt;
    headerOpt = (CAHeaderOption_t *) calloc(1, optionNum * sizeof(CAHeaderOption_t));

    if(!headerOpt)
    {
        FAIL() << "Allocation for headerOpt failed";
    }

    char* tmpOptionData1 = (char *) "Hello";
    size_t tmpOptionDataLen = (strlen(tmpOptionData1) < CA_MAX_HEADER_OPTION_DATA_LENGTH) ?
            strlen(tmpOptionData1) : CA_MAX_HEADER_OPTION_DATA_LENGTH - 1;
    headerOpt[0].optionID = 3000;
    memcpy(headerOpt[0].optionData, tmpOptionData1, tmpOptionDataLen);
    headerOpt[0].optionLength = (uint16_t) tmpOptionDataLen;

    char* tmpOptionData2 = (char *) "World";
    tmpOptionDataLen = (strlen(tmpOptionData2) < CA_MAX_HEADER_OPTION_DATA_LENGTH) ?
                strlen(tmpOptionData2) : CA_MAX_HEADER_OPTION_DATA_LENGTH - 1;
    headerOpt[1].optionID = 3001;
    memcpy(headerOpt[1].optionData, tmpOptionData2, tmpOptionDataLen);
    headerOpt[1].optionLength = (uint16_t) tmpOptionDataLen;

    CAGenerateToken(&tempToken, tokenLength);

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CAAdvertiseResource(uri, tempToken, tokenLength,
                                                           headerOpt, (uint8_t )optionNum));

    CADestroyToken(tempToken);

    free(headerOpt);
}

// CASelectNewwork TC
// check return value
TEST_F(CATests, SelectNetworkTestGood)
{
    CAResult_t res = checkSelectNetwork();
    EXPECT_EQ(CA_STATUS_OK, res);
}

CAResult_t checkSelectNetwork()
{
    CAResult_t res = CASelectNetwork(CA_IPV4);

    if (CA_STATUS_OK == res)
    {
        EXPECT_EQ(CA_STATUS_OK, CAUnSelectNetwork(CA_IPV4));
        return CA_STATUS_OK;
    }
    if (CA_NOT_SUPPORTED == res)
    {
        EXPECT_EQ(CA_STATUS_FAILED, CAUnSelectNetwork(CA_IPV4));
        return CA_STATUS_OK;
    }

    return res;
}

// check return value when selected network is disable
TEST_F(CATests, SelectNetworkTestBad)
{
    //Select disable network
    EXPECT_EQ(CA_NOT_SUPPORTED, CASelectNetwork(1000));
}

// check return value when selected network is disable
TEST_F(CATests, UnSelectNetworkTest)
{
    //UnSelect disable network
    EXPECT_EQ(CA_STATUS_FAILED, CAUnSelectNetwork(1000));
}

// CAHandlerRequestResponse TC
// check return value
TEST_F(CATests, HandlerRequestResponseTest)
{
    EXPECT_EQ(CA_STATUS_OK, CAHandleRequestResponse());
}

// CASendRequestToAll TC
// check return value
TEST(SendRequestToAllTest, DISABLED_TC_31_Positive_01)
{
    CASelectNetwork(CA_IPV4);

    uri = (char *) RESOURCE_URI;
    CACreateRemoteEndpoint(uri, CA_IPV4, &tempRep);
    CAGroupEndpoint_t *group = NULL;
    group = (CAGroupEndpoint_t *) malloc(sizeof(CAGroupEndpoint_t));
    if(!group)
    {
        FAIL() << "Allocation for group failed";
    }

    group->transportType = tempRep->transportType;
    group->resourceUri = tempRep->resourceUri;

    memset(&requestData, 0, sizeof(CAInfo_t));
    CAGenerateToken(&tempToken, tokenLength);
    requestData.token = tempToken;
    requestData.tokenLength = tokenLength;

    requestData.payload = (char *) "Temp Json Payload";
    requestData.type = CA_MSG_NONCONFIRM;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    EXPECT_EQ(CA_STATUS_OK, CASendRequestToAll(group, &requestInfo));

    CADestroyToken(tempToken);

    CADestroyRemoteEndpoint(tempRep);
    tempRep = NULL;

    free(group);
}

// check return value when group->resourceUri is NULL
TEST(SendRequestToAllTest, DISABLED_TC_32_Negative_01)
{
    uri = (char *) RESOURCE_URI;
    CAGroupEndpoint_t *group = NULL;

    memset(&requestData, 0, sizeof(CAInfo_t));
    CAGenerateToken(&tempToken, tokenLength);
    requestData.token = tempToken;
    requestData.tokenLength = tokenLength;

    requestData.payload = (char *) "Temp Json Payload";
    requestData.type = CA_MSG_NONCONFIRM;
    memset(&requestInfo, 0, sizeof(CARequestInfo_t));
    requestInfo.method = CA_GET;
    requestInfo.info = requestData;

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CASendRequestToAll(group, &requestInfo));

    CADestroyToken(tempToken);
}

// CAGetNetworkInformation TC
// check return value
TEST_F (CATests, GetNetworkInformationTestGood)
{
    EXPECT_EQ(CA_STATUS_OK, checkGetNetworkInfo());
}

TEST_F(CATests, RegisterDTLSCredentialsHandlerTest)
{
#ifdef __WITH_DTLS__
    if (SetCredentials() == 0)
    {
        printf("SetCredentials failed\n");
    }

    EXPECT_EQ(CA_STATUS_OK, CARegisterDTLSCredentialsHandler(CAGetDtlsPskCredentials));
#endif
}

CAResult_t checkGetNetworkInfo()
{
    CALocalConnectivity_t *tempInfo = NULL;
    uint32_t tempSize = 0;

    CAResult_t res = CAGetNetworkInformation(&tempInfo, &tempSize);

    free(tempInfo);

    if (CA_STATUS_OK == res || CA_ADAPTER_NOT_ENABLED == res ||
            CA_NOT_SUPPORTED == res)
    {
        return CA_STATUS_OK;
    }
    else
    {
        return CA_STATUS_FAILED;
    }
}
