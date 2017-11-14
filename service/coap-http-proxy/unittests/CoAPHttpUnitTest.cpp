//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "UnitTestHelper.h"
#include "CoapHttpHandler.h"

#include <condition_variable>
#include <mutex>
#include <chrono>
#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "coap/pdu.h"
#include "cJSON.h"

#include "platform_features.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "experimental/logger.h"
#include "uarraylist.h"
#include "CoapHttpParser.h"
#include "CoapHttpMap.h"
#include "ocpayload.h"

static std::chrono::milliseconds g_waitForResponse(10000);
static std::condition_variable responseCon;
static std::mutex mutexForCondition;
static bool cbCalled = true;
static OCDevAddr serverAddr;
static OCConnectivityType connType;

class CoApHttpTest: public TestWithMock
{
protected:
    void SetUp(void)
    {
        TestWithMock::SetUp();
        OCInit(NULL, 0, OC_CLIENT_SERVER);
    }

    void TearDown(void)
    {
        OCStop();
    }
};

TEST_F(CoApHttpTest, CHPInitialize)
{
    bool secureFlag = true;
    EXPECT_EQ(OC_STACK_OK, (CHPInitialize(secureFlag)));
    CHPTerminate();

    secureFlag = false;
    EXPECT_EQ(OC_STACK_OK, (CHPInitialize(secureFlag)));
    // re-initialize
    EXPECT_EQ(OC_STACK_OK, (CHPInitialize(secureFlag)));
    CHPTerminate();
}

TEST_F(CoApHttpTest, CHPIsInitialized)
{
    bool res = false;
    EXPECT_EQ(res, (CHPIsInitialized()));
    CHPInitialize(false);
    res = true;
    EXPECT_EQ(res, (CHPIsInitialized()));
    CHPTerminate();
    res = false;
    EXPECT_EQ(res, (CHPIsInitialized()));
}

TEST_F(CoApHttpTest, CHPTerminate)
{
    EXPECT_EQ(OC_STACK_OK, (CHPTerminate()));
    CHPInitialize(false);
    EXPECT_EQ(OC_STACK_OK, (CHPTerminate()));
}

OCStackApplicationResult discoveryReqCB(void* ctx, OCDoHandle handle,
                                        OCClientResponse* clientResponse)
{
    OC_UNUSED(ctx);
    OC_UNUSED(handle);

    if (clientResponse)
    {
        OCDiscoveryPayload* payload = (OCDiscoveryPayload*) clientResponse->payload;
        if (!payload)
        {
            // Receive other discovery responses
            return OC_STACK_KEEP_TRANSACTION;
        }

        int found = 0;
        OCResourcePayload* resource = (OCResourcePayload*) payload->resources;
        while (resource)
        {
            if (resource->uri && strcmp(resource->uri, OC_RSRVD_PROXY_URI) == 0)
            {
                found = 1;
                break;
            }
            resource = resource->next;
        }

        if (!found)
        {
            return OC_STACK_KEEP_TRANSACTION;
        }
    }

    serverAddr = clientResponse->devAddr;
    connType = clientResponse->connType;
    cbCalled = true;
    return OC_STACK_DELETE_TRANSACTION;
}

int waitCallbackRet(int waitSec)
{
    struct timespec timeout;
    timeout.tv_sec  = 0;
    timeout.tv_nsec = 100000000L;

    for(int i = 0; !cbCalled && waitSec * 10 > i; ++i)
    {
        nanosleep(&timeout, NULL);
        if(OC_STACK_OK != OCProcess())
        {
            return -1;
        }
    }

    return 0;
}

OCStackApplicationResult getReqCB(void* ctx, OCDoHandle handle,
                                  OCClientResponse* clientResponse)
{
    OC_UNUSED(ctx);
    OC_UNUSED(handle);
    OC_UNUSED(clientResponse);
    cbCalled = true;
    return OC_STACK_DELETE_TRANSACTION;
}

TEST_F(CoApHttpTest, CHPMain)
{
    CHPInitialize(false);
    //Initiate Discovery
    OCStackResult ret;
    OCCallbackData cbData;
    cbData.cb = discoveryReqCB;
    cbData.context = NULL;
    cbData.cd = NULL;
    cbCalled = false;
    ret = OCDoResource(NULL, OC_REST_DISCOVER, "/oic/res", NULL, 0, CT_DEFAULT,
                       OC_LOW_QOS, &cbData, NULL, 0);
    EXPECT_EQ(OC_STACK_OK, ret);
    waitCallbackRet(5);
    EXPECT_TRUE(cbCalled);

    // Initiate GET request
    const char *httpResource = "http://httpbin.org/get";
    OCHeaderOption option;
    memset(&option, 0, sizeof(option));
    option.protocolID = OC_COAP_ID;
    option.optionID = OC_RSRVD_PROXY_OPTION_ID;
    strncpy((char*)option.optionData, httpResource, sizeof(option.optionData));
    option.optionLength = strlen(httpResource);
    // A request with proxy uri shall not have resource uri
    OCDoHandle handle;
    cbData.context = NULL;
    cbData.cd = NULL;
    cbData.cb = getReqCB;
    cbCalled = false;
    ret = OCDoResource(&handle, OC_REST_GET, OC_RSRVD_PROXY_URI, &serverAddr,
                        NULL, connType, OC_LOW_QOS, &cbData, &option, 1);
    EXPECT_EQ(OC_STACK_OK, ret);
    waitCallbackRet(20);
    CHPTerminate();
}

TEST_F(CoApHttpTest, CHPGetOptionID)
{
    EXPECT_EQ(0, (CHPGetOptionID(NULL)));
    EXPECT_EQ(COAP_OPTION_MAXAGE, (CHPGetOptionID(HTTP_OPTION_EXPIRES)));
    EXPECT_EQ(COAP_OPTION_IF_MATCH, (CHPGetOptionID(HTTP_OPTION_IF_MATCH)));
    EXPECT_EQ(COAP_OPTION_IF_NONE_MATCH, (CHPGetOptionID(HTTP_OPTION_IF_NONE_MATCH)));
    EXPECT_EQ(COAP_OPTION_ETAG, (CHPGetOptionID(HTTP_OPTION_ETAG)));
}

TEST_F(CoApHttpTest, CHPGetOCCode)
{
    OCEntityHandlerResult res;
    HttpResponseResult_t httpCode = CHP_SUCCESS;
    OCMethod method = OC_REST_GET;
    EXPECT_EQ(OC_STACK_OK, (CHPGetOCCode(httpCode, method, &res)));

    method = OC_REST_DELETE;
    EXPECT_EQ(OC_STACK_OK, (CHPGetOCCode(httpCode, method, &res)));

    method = OC_REST_OBSERVE;
    EXPECT_EQ(OC_STACK_OK, (CHPGetOCCode(httpCode, method, &res)));

    httpCode = CHP_NO_CONTENT;
    method = OC_REST_DELETE;
    EXPECT_EQ(OC_STACK_OK, (CHPGetOCCode(httpCode, method, &res)));

    httpCode = CHP_UNAUTHORIZED_REQ;
    EXPECT_EQ(OC_STACK_OK, (CHPGetOCCode(httpCode, method, &res)));

    httpCode = CHP_BAD_GATEWAY;
    EXPECT_EQ(OC_STACK_OK, (CHPGetOCCode(httpCode, method, &res)));

    httpCode = (HttpResponseResult_t)5555; // Invalid
    EXPECT_NE(OC_STACK_OK, (CHPGetOCCode(httpCode, method, &res)));
}

TEST_F(CoApHttpTest, CHPGetOCOption)
{
    const char *expVal = "Thu, 20 Oct 2025 11:45:00 GMT";
    OCHeaderOption ocOp;
    HttpHeaderOption_t httpOption;
    httpOption.optionLength = strlen(expVal) + 1;
    OICStrcpy(httpOption.optionName, sizeof(httpOption.optionName), HTTP_OPTION_EXPIRES);
    OICStrcpy(httpOption.optionData, sizeof(httpOption.optionData), expVal);

    EXPECT_NE(OC_STACK_OK, (CHPGetOCOption(NULL, NULL)));
    EXPECT_EQ(OC_STACK_OK, (CHPGetOCOption(&httpOption, &ocOp)));

    // invalid option name
    httpOption.optionName[0] = 'z';
    httpOption.optionName[1] = '\0';
    EXPECT_EQ(OC_STACK_INVALID_OPTION, (CHPGetOCOption(&httpOption, &ocOp)));
}

TEST_F(CoApHttpTest, CHPGetOCContentType)
{
    const char *httpContentType = CBOR_CONTENT_TYPE;
    EXPECT_EQ(OC_FORMAT_CBOR, (CHPGetOCContentType(httpContentType)));
    httpContentType = JSON_CONTENT_TYPE;
    EXPECT_EQ(OC_FORMAT_JSON, (CHPGetOCContentType(httpContentType)));
    httpContentType = "application/invalid";
    EXPECT_EQ(OC_FORMAT_UNSUPPORTED, (CHPGetOCContentType(httpContentType)));
}

TEST_F(CoApHttpTest, CHPGetHttpMethod)
{
    HttpMethod_t method;
    EXPECT_EQ(OC_STACK_OK, (CHPGetHttpMethod(OC_REST_GET, &method)));
    EXPECT_EQ(OC_STACK_OK, (CHPGetHttpMethod(OC_REST_PUT, &method)));
    EXPECT_EQ(OC_STACK_OK, (CHPGetHttpMethod(OC_REST_POST, &method)));
    EXPECT_EQ(OC_STACK_OK, (CHPGetHttpMethod(OC_REST_DELETE, &method)));
    // invalid
    EXPECT_NE(OC_STACK_OK, (CHPGetHttpMethod(OC_REST_DISCOVER, &method)));
}

TEST_F(CoApHttpTest, CHPGetHttpOption)
{
    OCHeaderOption ocOp = {OC_COAP_ID, COAP_OPTION_ACCEPT,
                           (uint16_t)(strlen(CBOR_CONTENT_TYPE) + 1),
                           (uint8_t *)CBOR_CONTENT_TYPE};

    HttpHeaderOption_t *httpOp;
    EXPECT_NE(OC_STACK_OK, (CHPGetHttpOption(NULL, NULL)));
    EXPECT_EQ(OC_STACK_OK, (CHPGetHttpOption(&ocOp, &httpOp)));
    OICFree(httpOp);
    httpOp = NULL;

    ocOp.optionID = COAP_OPTION_IF_MATCH;
    EXPECT_EQ(OC_STACK_OK, (CHPGetHttpOption(&ocOp, &httpOp)));
    OICFree(httpOp);
    httpOp = NULL;

    ocOp.optionID = COAP_OPTION_IF_NONE_MATCH;
    EXPECT_EQ(OC_STACK_OK, (CHPGetHttpOption(&ocOp, &httpOp)));
    OICFree(httpOp);
    httpOp = NULL;

    ocOp.optionID = COAP_OPTION_ETAG;
    EXPECT_EQ(OC_STACK_OK, (CHPGetHttpOption(&ocOp, &httpOp)));
    OICFree(httpOp);
    httpOp = NULL;

    ocOp.optionID = COAP_OPTION_CONTENT_TYPE;
    EXPECT_EQ(OC_STACK_OK, (CHPGetHttpOption(&ocOp, &httpOp)));
    OICFree(httpOp);
    httpOp = NULL;

    ocOp.optionID = COAP_OPTION_MAXAGE;
    EXPECT_EQ(OC_STACK_INVALID_OPTION, (CHPGetHttpOption(&ocOp, &httpOp)));
}

OCRepPayload* payload;
cJSON* cj;
TEST_F(CoApHttpTest, CHPRepPayloadToJson)
{
    payload = OCRepPayloadCreate();
    cj = NULL;
    EXPECT_TRUE(payload);

    OCRepPayloadSetUri(payload, "/a/light/");
    OCRepPayloadSetPropBool(payload, "bs", false);
    OCRepPayloadSetPropBool(payload, "bs2", true);
    OCRepPayloadSetPropInt(payload, "bi", 1);
    OCRepPayloadSetPropDouble(payload, "bd", 1.1);
    OCRepPayloadSetPropString(payload, "str", "test");
    // Any data is fine
    OCByteString bytes = {(uint8_t*)HTTP_OPTION_ETAG, strlen(HTTP_OPTION_ETAG)};
    OCRepPayloadSetPropByteString(payload, "bbs", bytes);

    int64_t bia[5] = {1, 2, 3, 4, 5};
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = {sizeof(bia) / sizeof(bia[0]), 0, 0};
    OCRepPayloadSetIntArray(payload, "bia", bia, dimensions);

    double bda[5] = {1.4};
    dimensions[0] = sizeof(bda) / sizeof(bda[0]);
    OCRepPayloadSetDoubleArray(payload, "bda", bda, dimensions);

    OCRepPayload* payload2 = OCRepPayloadClone(payload);
    OCRepPayloadSetPropObject(payload, "embedded", payload2);

    OCRepPayload **payloadarr = (OCRepPayload **) OICCalloc(2, sizeof(OCRepPayload *));
    payloadarr[0] = OCRepPayloadClone(payload2);
    payloadarr[1] = OCRepPayloadClone(payload2);
    dimensions[0] = 2;
    OCRepPayloadSetPropObjectArray(payload, "payloadarr", (const OCRepPayload **)payloadarr,
                                    dimensions);
    cj = CHPRepPayloadToJson(payload);
    EXPECT_TRUE(cj);
    OCRepPayloadDestroy(payload);
    OCRepPayloadDestroy(payload2);
    if (payloadarr)
    {
        for (int k = 0; k < 2; k++)
            OCRepPayloadDestroy(payloadarr[k]);
        OICFree(payloadarr);
    }

    payload = NULL;
}

TEST_F(CoApHttpTest, CHPJsonToRepPayload)
{
    payload = OCRepPayloadCreate();
    EXPECT_TRUE(payload);
    CHPJsonToRepPayload(cj, payload);
    cJSON_Delete(cj);
    OCRepPayloadDestroy(payload);
    payload = NULL;
    cj = NULL;
}

TEST_F(CoApHttpTest, CHPParserInitialize)
{
    EXPECT_EQ(OC_STACK_OK, (CHPParserInitialize()));
}

void *parserctxt = NULL;
void parserCallback(const HttpResponse_t *response, void *context)
{
    OC_UNUSED(response);
    EXPECT_EQ(parserctxt, context);
    cbCalled = true;
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.notify_all();
}

TEST_F(CoApHttpTest, CHPPostHttpRequest)
{
    CHPInitialize(false);
    HttpRequest_t hreq = {1, 1, CHP_GET, NULL, "", NULL, 0, false,
                          JSON_CONTENT_TYPE, JSON_CONTENT_TYPE};
    parserctxt = (void *)555; // Some context value

    const char *tempRes = "http://httpbin.org/get";
    OICStrcpy(hreq.resourceUri, sizeof(hreq.resourceUri), tempRes);
    cbCalled = false;
    EXPECT_EQ(OC_STACK_OK, (CHPPostHttpRequest(&hreq, parserCallback, parserctxt)));
    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    tempRes = "google.com";
    OICStrcpy(hreq.resourceUri, sizeof(hreq.resourceUri), tempRes);
    EXPECT_EQ(OC_STACK_OK, (CHPPostHttpRequest(&hreq, parserCallback, parserctxt)));
    //std::unique_lock< std::mutex > lock2{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    hreq.method = CHP_POST;
    hreq.payload = OICMalloc(10); // Contents doesn't matter.
    hreq.payloadLength = 10;
    EXPECT_EQ(OC_STACK_OK, (CHPPostHttpRequest(&hreq, parserCallback, parserctxt)));
    //std::unique_lock< std::mutex > lock3{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);

    hreq.method = CHP_PUT;
    hreq.payload = OICMalloc(10); // Contents doesn't matter.
    hreq.payloadLength = 10;
    EXPECT_EQ(OC_STACK_OK, (CHPPostHttpRequest(&hreq, parserCallback, parserctxt)));
    //td::unique_lock< std::mutex > lock4{ mutexForCondition };
    responseCon.wait_for(lock, g_waitForResponse);
    EXPECT_TRUE(cbCalled);
}

TEST_F(CoApHttpTest, CHPParserTerminate)
{
    EXPECT_EQ(OC_STACK_OK, (CHPParserTerminate()));
}

