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

#include "gtest/gtest.h"
#include "cawsadapter.h"
#include "caipadapter.h"
#include "libwebsockets.h"
#include "octhread.h"
#include "experimental/logger.h"
#include "oic_malloc.h"

#define MAX_THREAD_POOL_SIZE 10
#define TAG "CA_WS_ADAPTER_API_TEST"

static void CARegisterCallback(CAConnectivityHandler_t /*handler*/)
{
}

static void CAReceivedPacketCallback(const CASecureEndpoint_t * /*sep*/,
                                     const void * /*data*/, size_t /*dataLen*/)
{
}

static void CAAdapterChangedCallback(CATransportAdapter_t /*adapter*/, CANetworkStatus_t /*status*/)
{
}

static void CAConnectionChangedCallback(const CAEndpoint_t * /*info*/, bool /*isConnected*/)
{
}

static void CAAdapterErrorHandleCallback(const CAEndpoint_t * /*endpoint*/,
        const void * /*data*/, size_t /*dataLen*/,
        CAResult_t /*result*/)
{
}

class WSAdapterAPITest : public testing::Test
{
    private:
        ca_thread_pool_t mTpHandle;

    public:
        WSAdapterAPITest()
        {
            mTpHandle = NULL;
        }

        void SetUp()
        {
            ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &mTpHandle);
            ASSERT_NE(nullptr, mTpHandle);

            // Initialize adatper
            ASSERT_EQ(CA_STATUS_OK, CAInitializeWS(CARegisterCallback, CAReceivedPacketCallback,
                                                   CAAdapterChangedCallback,
                                                   CAConnectionChangedCallback, CAAdapterErrorHandleCallback, mTpHandle));

            // Start adapter
            ASSERT_EQ(CA_STATUS_OK, CAStartWS());
        }

        void TearDown()
        {
            CAStopWS();
            CATerminateWS();
            ca_thread_pool_free(mTpHandle);
        }
};

TEST(CAInitializeWSTest, Success)
{
    ca_thread_pool_t tpHandle = NULL;
    ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &tpHandle);
    ASSERT_NE(nullptr, tpHandle);

    ASSERT_EQ(CA_STATUS_OK, CAInitializeWS(CARegisterCallback, CAReceivedPacketCallback,
                                           CAAdapterChangedCallback,
                                           CAConnectionChangedCallback, CAAdapterErrorHandleCallback, tpHandle));
    CATerminateWS();

    ca_thread_pool_free(tpHandle);
}

TEST(CAInitializeWSTest, ThreadPoolIsNull)
{
    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CAInitializeWS(CARegisterCallback, CAReceivedPacketCallback,
              CAAdapterChangedCallback,
              CAConnectionChangedCallback, CAAdapterErrorHandleCallback, NULL));
}

TEST(CAInitializeWSTest, CARegisterCallbackIsNull)
{
    ca_thread_pool_t tpHandle = NULL;
    ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &tpHandle);
    ASSERT_NE(nullptr, tpHandle);

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CAInitializeWS(NULL, CAReceivedPacketCallback,
              CAAdapterChangedCallback,
              CAConnectionChangedCallback, CAAdapterErrorHandleCallback, tpHandle));

    ca_thread_pool_free(tpHandle);
}

TEST(CAInitializeWSTest, CAReceivedPacketCallbackIsNull)
{
    ca_thread_pool_t tpHandle = NULL;
    ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &tpHandle);
    ASSERT_NE(nullptr, tpHandle);

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CAInitializeWS(CARegisterCallback, NULL,
              CAAdapterChangedCallback,
              CAConnectionChangedCallback, CAAdapterErrorHandleCallback, tpHandle));

    ca_thread_pool_free(tpHandle);
}

TEST(CAInitializeWSTest, CAAdapterChangedCallbackIsNull)
{
    ca_thread_pool_t tpHandle = NULL;
    ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &tpHandle);
    ASSERT_NE(nullptr, tpHandle);

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CAInitializeWS(CARegisterCallback, CAReceivedPacketCallback,
              NULL,
              CAConnectionChangedCallback, CAAdapterErrorHandleCallback, tpHandle));

    ca_thread_pool_free(tpHandle);
}

TEST(CAInitializeWSTest, CAConnectionChangedCallbackIsNull)
{
    ca_thread_pool_t tpHandle = NULL;
    ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &tpHandle);
    ASSERT_NE(nullptr, tpHandle);

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CAInitializeWS(CARegisterCallback, CAReceivedPacketCallback,
              CAAdapterChangedCallback,
              NULL, CAAdapterErrorHandleCallback, tpHandle));

    ca_thread_pool_free(tpHandle);
}

TEST(CAInitializeWSTest, CAAdapterErrorHandleCallbackIsNull)
{
    ca_thread_pool_t tpHandle = NULL;
    ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &tpHandle);
    ASSERT_NE(nullptr, tpHandle);

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CAInitializeWS(CARegisterCallback, CAReceivedPacketCallback,
              CAAdapterChangedCallback,
              CAConnectionChangedCallback, NULL, tpHandle));

    ca_thread_pool_free(tpHandle);
}

TEST(CATerminateWSTest, Success)
{
    ca_thread_pool_t tpHandle = NULL;
    ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &tpHandle);
    ASSERT_NE(nullptr, tpHandle);

    ASSERT_EQ(CA_STATUS_OK, CAInitializeWS(CARegisterCallback, CAReceivedPacketCallback,
                                           CAAdapterChangedCallback,
                                           CAConnectionChangedCallback, CAAdapterErrorHandleCallback, tpHandle));

    // TODO: How to verify terminate status ?
    CATerminateWS();

    ca_thread_pool_free(tpHandle);
}

TEST(CAStartWSTest, Success)
{
    ca_thread_pool_t tpHandle = NULL;
    ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &tpHandle);
    ASSERT_NE(nullptr, tpHandle);

    ASSERT_EQ(CA_STATUS_OK, CAInitializeWS(CARegisterCallback, CAReceivedPacketCallback,
                                           CAAdapterChangedCallback,
                                           CAConnectionChangedCallback, CAAdapterErrorHandleCallback, tpHandle));

    EXPECT_EQ(CA_STATUS_OK, CAStartWS());
    CAStopWS();

    CATerminateWS();
    ca_thread_pool_free(tpHandle);
}

TEST(CAStopWSTest, Success)
{
    ca_thread_pool_t tpHandle = NULL;
    ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &tpHandle);
    ASSERT_NE(nullptr, tpHandle);

    ASSERT_EQ(CA_STATUS_OK, CAInitializeWS(CARegisterCallback, CAReceivedPacketCallback,
                                           CAAdapterChangedCallback,
                                           CAConnectionChangedCallback, CAAdapterErrorHandleCallback, tpHandle));

    EXPECT_EQ(CA_STATUS_OK, CAStartWS());
    EXPECT_EQ(CA_STATUS_OK, CAStopWS());

    CATerminateWS();
    ca_thread_pool_free(tpHandle);
}

typedef WSAdapterAPITest WSListeningServerTest;
TEST_F(WSListeningServerTest, CAStartWSListeningServer)
{
    EXPECT_EQ(CA_STATUS_OK, CAStartWSListeningServer());
    CAStopWSListeningServer();
}

TEST_F(WSListeningServerTest, CAStopWSListeningServer)
{
    EXPECT_EQ(CA_STATUS_OK, CAStartWSListeningServer());
    EXPECT_EQ(CA_STATUS_OK, CAStopWSListeningServer());
}

typedef WSAdapterAPITest WSDiscoveryServerTest;
TEST_F(WSDiscoveryServerTest, CAStartWSDiscoveryServer)
{
    EXPECT_EQ(CA_STATUS_OK, CAStartWSDiscoveryServer());
}

typedef WSAdapterAPITest CASendWSMulticastDataTest;
TEST_F(CASendWSMulticastDataTest, Success)
{
    EXPECT_EQ(CA_STATUS_OK, CASendWSMulticastData(NULL, NULL, 0, CA_REQUEST_DATA));
}

typedef WSAdapterAPITest CAReadWSDataTest;
TEST_F(CAReadWSDataTest, Success)
{
    EXPECT_EQ(CA_STATUS_OK, CAReadWSData());
}

struct lws_context *context = NULL;
int serverPort = 0;
oc_mutex dataReceivedMutex = NULL;
oc_cond lwsServiceStoppedCond = NULL;
bool dataReceived = false;
int data[] = { 0x08, 0x01, 0x89, 0x21, 0x5A, 0xD5, 0xCA, 0x17, 0xFE,
               0xD1, 0xB3, 0x6F, 0x69, 0x63, 0x03, 0x72, 0x65, 0x73,
               0x62, 0x27, 0x10, 0xE2, 0x06, 0xE3, 0x08, 0x00, 0xE1,
               0xF6, 0xE6, 0xC0
             };

int LWSCallback(struct lws *wsi, enum lws_callback_reasons reason,
                void *user, void *in, size_t len)
{
    //unused parameters
    (void) user;
    (void) len;

    uint8_t *buff = NULL;

    switch (reason)
    {
        case LWS_CALLBACK_RECEIVE:
            if (0 == memcmp(data, in, sizeof(data)))
            {
                OIC_LOG(DEBUG, TAG, "Success, Received data in LWS callback");
            }
            lws_callback_on_writable(wsi);
            return 0;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            buff = (uint8_t *)OICMalloc(LWS_PRE + sizeof(data));
            if (NULL == buff)
            {
                OIC_LOG(ERROR, TAG, "Out of memory!");
                return 1;
            }
            memcpy(buff + LWS_PRE, (uint8_t *)data, sizeof(data));
            lws_write(wsi, buff + LWS_PRE, sizeof(data), LWS_WRITE_BINARY);
            OICFree(buff);
            return 0;

        default:
            return 0;
    }
}

static struct lws_protocols protocols[] =
{
    { "coap", LWSCallback, 0 },
    { NULL, NULL, 0 }
};

void InitLWSServer()
{
    struct lws_context_creation_info contextInfo;
    memset(&contextInfo, 0, sizeof(contextInfo));

    contextInfo.port = 0;
    contextInfo.protocols = protocols;
    context = lws_create_context(&contextInfo);
    EXPECT_NE(nullptr, context);
    serverPort = contextInfo.port;
    OIC_LOG_V(DEBUG, TAG, "[LWS test] server bound to port= %d", serverPort);
}

void LWSService(void *data)
{
    //unused parameters
    (void) data;

    OIC_LOG(DEBUG, TAG, "[LWS test] starting lws service");
    int times = 0;
    while (true)
    {
        oc_mutex_lock(dataReceivedMutex);
        if (dataReceived || (times >= 50))
        {
            oc_mutex_unlock(dataReceivedMutex);
            break;
        }
        oc_mutex_unlock(dataReceivedMutex);
        lws_service(context, 50);
        ++times;
    }
    EXPECT_EQ(dataReceived, true);
    oc_cond_signal(lwsServiceStoppedCond);
}

void testPacketReceivedCallback(const CASecureEndpoint_t *sep,
                                const void *indata, size_t indataLen)
{
    //unused parameters
    (void) sep;

    oc_mutex_lock(dataReceivedMutex);
    if (sizeof(data) == indataLen && 0 == memcmp(data, indata, sizeof(data)))
    {
        dataReceived = true;
        OIC_LOG(DEBUG, TAG, "Success, Received data in CA callback");
    }
    oc_mutex_unlock(dataReceivedMutex);
}

TEST(CASendReceiveWSUnicastDataTest, Success)
{
    ca_thread_pool_t tpHandle = NULL;
    ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &tpHandle);
    EXPECT_NE(nullptr, tpHandle);

    dataReceivedMutex = oc_mutex_new();
    EXPECT_NE(nullptr, dataReceivedMutex);

    lwsServiceStoppedCond = oc_cond_new();
    EXPECT_NE(nullptr, lwsServiceStoppedCond);

    caglobals.client = true;
    caglobals.server = false;

    InitLWSServer();
    EXPECT_EQ(CA_STATUS_OK, ca_thread_pool_add_task(tpHandle, LWSService, NULL));

    CAEndpoint_t *ep;
    CACreateEndpoint(CA_DEFAULT_FLAGS, CA_ADAPTER_WS, "127.0.0.1", serverPort, &ep);

    ASSERT_EQ(CA_STATUS_OK, CAInitializeWS(CARegisterCallback, testPacketReceivedCallback,
                                           CAAdapterChangedCallback,
                                           CAConnectionChangedCallback, CAAdapterErrorHandleCallback, tpHandle));

    EXPECT_EQ(CA_STATUS_OK, CAStartWS());

    EXPECT_EQ(sizeof(data), CASendWSUnicastData(ep, data, sizeof(data), CA_REQUEST_DATA));

    oc_mutex_lock(dataReceivedMutex);
    oc_cond_wait(lwsServiceStoppedCond, dataReceivedMutex);
    oc_mutex_unlock(dataReceivedMutex);

    CAStopWS();
    CATerminateWS();

    lws_context_destroy(context);
    oc_mutex_free(dataReceivedMutex);
    oc_cond_free(lwsServiceStoppedCond);
    ca_thread_pool_free(tpHandle);
    CADestroyEndpoint(ep);
}

static oc_mutex g_errorHandlerCalledMutex = NULL;
static oc_cond g_errorHandlerCalledCond = NULL;

static void CAAdapterSignalErrorHandleCallback(const CAEndpoint_t * /*endpoint*/,
    const void * /*data*/, size_t /*dataLen*/,
    CAResult_t /*result*/)
{
    OIC_LOG(DEBUG, TAG, "Success, Error callback called");
    oc_mutex_lock(g_errorHandlerCalledMutex);
    oc_cond_signal(g_errorHandlerCalledCond);
    oc_mutex_unlock(g_errorHandlerCalledMutex);
}

TEST(CASendReceiveWSUnicastDataTest, Failure)
{
    ca_thread_pool_t tpHandle = NULL;
    ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &tpHandle);
    EXPECT_NE(nullptr, tpHandle);

    g_errorHandlerCalledMutex = oc_mutex_new();
    EXPECT_NE(nullptr, g_errorHandlerCalledMutex);

    g_errorHandlerCalledCond = oc_cond_new();
    EXPECT_NE(nullptr, g_errorHandlerCalledCond);

    caglobals.client = true;
    caglobals.server = false;

    CAEndpoint_t *ep;
    CACreateEndpoint(CA_DEFAULT_FLAGS, CA_ADAPTER_WS, "127.0.0.1", serverPort, &ep);

    ASSERT_EQ(CA_STATUS_OK, CAInitializeWS(CARegisterCallback, testPacketReceivedCallback,
                                           CAAdapterChangedCallback,
                                           CAConnectionChangedCallback, CAAdapterSignalErrorHandleCallback, tpHandle));

    EXPECT_EQ(CA_STATUS_OK, CAStartWS());

    oc_mutex_lock(g_errorHandlerCalledMutex);
    EXPECT_EQ(sizeof(data), CASendWSUnicastData(ep, data, sizeof(data), CA_REQUEST_DATA));
    OCWaitResult_t ret = oc_cond_wait_for(g_errorHandlerCalledCond, g_errorHandlerCalledMutex, 5000000);
    oc_mutex_unlock(g_errorHandlerCalledMutex);
    EXPECT_EQ(OC_WAIT_SUCCESS, ret);

    CAStopWS();
    CATerminateWS();

    oc_mutex_free(g_errorHandlerCalledMutex);
    oc_cond_free(g_errorHandlerCalledCond);
    ca_thread_pool_free(tpHandle);
    CADestroyEndpoint(ep);
}

// TODO: IP Adapter runs network monitor.
// Thus its required to initialize IP adapter too.
TEST(CAGetWSInterfaceInformationTest, Success)
{
    ca_thread_pool_t tpHandle1 = NULL;
    ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &tpHandle1);
    EXPECT_NE(nullptr, tpHandle1);

    EXPECT_EQ(CA_STATUS_OK, CAInitializeIP(CARegisterCallback, CAReceivedPacketCallback,
                                           CAAdapterChangedCallback,
                                           CAAdapterErrorHandleCallback, tpHandle1));

    EXPECT_EQ(CA_STATUS_OK, CAStartIP());

    ca_thread_pool_t tpHandle2 = NULL;
    ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &tpHandle2);
    EXPECT_NE(nullptr, tpHandle2);

    EXPECT_EQ(CA_STATUS_OK, CAInitializeWS(CARegisterCallback, CAReceivedPacketCallback,
                                           CAAdapterChangedCallback,
                                           CAConnectionChangedCallback, CAAdapterErrorHandleCallback, tpHandle2));

    EXPECT_EQ(CA_STATUS_OK, CAStartWS());

    CAEndpoint_t *info = NULL;
    size_t size = 0;

    EXPECT_EQ(CA_STATUS_OK, CAGetWSInterfaceInformation(&info, &size));
    EXPECT_NE(nullptr, info);
    EXPECT_GT(size, 0);

    CAStopIP();
    CATerminateIP();
    ca_thread_pool_free(tpHandle1);

    CAStopWS();
    CATerminateWS();
    ca_thread_pool_free(tpHandle2);

    OICFree(info);
}

#ifdef __WITH_TLS__
TEST(CAGetWSInterfaceInformationTest, EndpointsWithSecureStack)
{
    ca_thread_pool_t tpHandle1 = NULL;
    ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &tpHandle1);
    EXPECT_NE(nullptr, tpHandle1);

    EXPECT_EQ(CA_STATUS_OK, CAInitializeIP(CARegisterCallback, CAReceivedPacketCallback,
                                           CAAdapterChangedCallback,
                                           CAAdapterErrorHandleCallback, tpHandle1));

    EXPECT_EQ(CA_STATUS_OK, CAStartIP());

    ca_thread_pool_t tpHandle2 = NULL;
    ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &tpHandle2);
    EXPECT_NE(nullptr, tpHandle2);

    EXPECT_EQ(CA_STATUS_OK, CAInitializeWS(CARegisterCallback, CAReceivedPacketCallback,
                                           CAAdapterChangedCallback,
                                           CAConnectionChangedCallback, CAAdapterErrorHandleCallback, tpHandle2));

    EXPECT_EQ(CA_STATUS_OK, CAStartWS());

    CAEndpoint_t *info = NULL;
    size_t size = 0;

    EXPECT_EQ(CA_STATUS_OK, CAGetWSInterfaceInformation(&info, &size));
    EXPECT_NE(nullptr, info);
    EXPECT_GT(size, 0);

    // Secure endpoint should be present
    bool secureEP = false;
    for (size_t i = 0; i < size; i++)
    {
        if ((info[i].flags & CA_SECURE) == CA_SECURE)
        {
            secureEP = true;
            break;
        }
    }

    EXPECT_TRUE(secureEP);

    CAStopIP();
    CATerminateIP();
    ca_thread_pool_free(tpHandle1);

    CAStopWS();
    CATerminateWS();
    ca_thread_pool_free(tpHandle2);

    OICFree(info);
}
#endif

TEST(CAGetWSInterfaceInformationTest, WithoutAdapterInitialize)
{
    CAEndpoint_t *info = NULL;
    size_t size = 0;

    EXPECT_EQ(CA_STATUS_FAILED, CAGetWSInterfaceInformation(&info, &size));
    EXPECT_EQ(nullptr, info);
    EXPECT_EQ(0, size);
}

