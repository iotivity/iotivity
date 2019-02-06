/* *****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *****************************************************************/

#include <stdlib.h>
#include <gtest/gtest.h>

extern "C" {
#include "octypes.h"
#include "psinterface.h"

#include "deviceonboardingstate.h"
#include "srmutility.h"
#include "experimental/logger.h"
#include "srmresourcestrings.h"
#include "aclresource.h"
#include "amaclresource.h"
#include "credresource.h"
#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
#include "crlresource.h"
#endif /* (__WITH_DTLS__) || (__WITH_TLS__) */
#include "experimental/doxmresource.h"
#include "pstatresource.h"
#include "tools.h"

#undef TAG
#include "../src/cloud/cloudresource.c"
#undef TAG
#include "../src/cloud/aclgroup.c"
#undef TAG
#include "../src/cloud/aclid.c"
#undef TAG
#include "../src/cloud/aclinvite.c"
#undef TAG
#include "../src/cloud/auth.c"
#undef TAG
#include "../src/cloud/config.c"
#undef TAG
#include "../src/cloud/crl.c"
#undef TAG
#include "../src/cloud/csr.c"
#undef TAG
#include "../src/cloud/utils.c"
#undef TAG
}

#ifdef TAG
#undef TAG
#endif

#define TAG  "CLOUD_UNITTEST"

#define SVR_DB_FILE_NAME TAG".dat"
#define PM_DB_FILE_NAME TAG".db"

#define STR_LEN 512
static const char *sample =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!:/";

class CLOUD : public ::testing::Test
{
    public:
        static void SetUpTestCase()
        {
            IOT_Init(PM_DB_FILE_NAME);
            EXPECT_EQ(OC_STACK_OK, InitCloudResource());
        }

        static void TearDownTestCase()
        {
            EXPECT_EQ(OC_STACK_OK, DeInitCloudResource());
            IOT_DeInit(PM_DB_FILE_NAME);
        }
};

void sessionInit(session_t *ses)
{
    ses->expireSin = 1;
    ses->accessToken = (char *)OICCalloc(STR_LEN, sizeof(char));
    ses->refreshToken = (char *)OICCalloc(STR_LEN, sizeof(char));
    ses->uid = (char *)OICCalloc(STR_LEN, sizeof(char));
    for (int i = 0; i < STR_LEN; i++)
    {
        ses->accessToken[i] = sample[rand() % STR_LEN];
        ses->refreshToken[i] = sample[rand() % STR_LEN];
        ses->uid[i] = sample[rand() % STR_LEN];
    }
}

OicCloud_t *getCloud()
{

    OicCloud_t *cloud = (OicCloud_t *)OICCalloc(1, sizeof(OicCloud_t));
    VERIFY_NOT_NULL_RETURN(TAG, cloud, ERROR, NULL);

    cloud->next = NULL;
    cloud->apn = (char *)OICCalloc(STR_LEN, sizeof(char));
    cloud->cis = (char *)OICCalloc(STR_LEN, sizeof(char));
    cloud->at = (char *)OICCalloc(STR_LEN, sizeof(char));
    cloud->sid = (char *)OICCalloc(STR_LEN, sizeof(char));
#if !defined(__MANDATORY__)
    cloud->redirectUri = (char *)OICCalloc(STR_LEN, sizeof(char));
    snprintf(cloud->redirectUri, STR_LEN, "coaps+tcp://127.0.0.1:80");
#endif // __MANDATORY__

    for (int i = 0; i < STR_LEN; i++)
    {
        cloud->apn[i] = sample[rand() % STR_LEN];
        cloud->cis[i] = sample[rand() % STR_LEN];
        cloud->at[i] = sample[rand() % STR_LEN];
        cloud->sid[i] = sample[rand() % STR_LEN];
    }

    cloud->session = (session_t *)OICCalloc(1, sizeof(session_t));
    sessionInit(cloud->session);

    cloud->stat = OC_CLOUD_OK;
    cloud->pid = 0;

    return cloud;
}

TEST_F(CLOUD, CloudToCBORPayloadNULL)
{
    OicCloud_t *cloud =  getCloud();
    size_t size = 10;
    uint8_t *payload = NULL;

    EXPECT_EQ(OC_STACK_INVALID_PARAM, CloudToCBORPayload(NULL, NULL, 0));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CloudToCBORPayload(cloud, NULL, &size));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CloudToCBORPayload(cloud, &payload, NULL));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CloudToCBORPayload(NULL, &payload, &size));
    FreeCloud(cloud);
}

TEST_F(CLOUD, CloudToCBORPayloadVALID)
{
    OicCloud_t *cloud = getCloud();

    uint8_t *payload = NULL;
    size_t size = 0;
    EXPECT_EQ(OC_STACK_OK, CloudToCBORPayload(cloud, &payload, &size));
    EXPECT_TRUE(payload != NULL);

    FreeCloud(cloud);
    OCPayloadDestroy((OCPayload *)payload);
}

TEST_F(CLOUD, CloudToCBORPayloadResourceVALID)
{
    OicCloud_t *cloud = getCloud();

    uint8_t *payload = NULL;
    size_t size = 0;
    EXPECT_EQ(OC_STACK_OK, CloudToCBORPayloadResource(cloud, &payload, &size));
    EXPECT_TRUE(payload != NULL);

    FreeCloud(cloud);
    OCPayloadDestroy((OCPayload *)payload);
}

TEST_F(CLOUD, CBORPayloadToCloudNULL)
{
    OicCloud_t *cloud = NULL;
    uint8_t *cborPayload = (uint8_t *)OICCalloc(1, sizeof(uint8_t));
    size_t size = 10;
    ASSERT_TRUE(NULL != cborPayload);
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CBORPayloadToCloud(NULL, 0, NULL));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CBORPayloadToCloud(NULL, size, &cloud));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CBORPayloadToCloud(cborPayload, size, NULL));
    EXPECT_EQ(OC_STACK_ERROR, CBORPayloadToCloud(cborPayload, size, &cloud));
    size = 0;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CBORPayloadToCloud(cborPayload, size, &cloud));
    size = 10;
    cloud =  getCloud();
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CBORPayloadToCloud(cborPayload, size, &cloud));
    FreeCloud(cloud);
    OICFree(cborPayload);
}

TEST_F(CLOUD, CBORPayloadToCloudFULL)
{
    OicCloud_t *cloud = NULL;
    uint8_t *payload = NULL;
    size_t size = 0;
    cloud =  getCloud();
    EXPECT_EQ(OC_STACK_OK, CloudToCBORPayload(cloud, &payload, &size));
    EXPECT_TRUE(payload != NULL);

    OicCloud_t *cloudX = NULL;
    EXPECT_EQ(OC_STACK_OK, CBORPayloadToCloud(payload, size, &cloudX));
    ASSERT_TRUE(cloudX != NULL);
    ASSERT_STREQ(cloudX->apn, cloud->apn);
    ASSERT_STREQ(cloudX->cis, cloud->cis);
    ASSERT_STREQ(cloudX->at, cloud->at);
    ASSERT_EQ(OC_CLOUD_OK, cloudX->stat);

    if (cloud->session && cloudX->session)
    {
        ASSERT_STREQ(cloudX->session->authProvider, cloud->session->authProvider);
        ASSERT_STREQ(cloudX->session->accessToken, cloud->session->accessToken);
        ASSERT_STREQ(cloudX->session->refreshToken, cloud->session->refreshToken);
        ASSERT_STREQ(cloudX->session->uid, cloud->session->uid);
#if !defined(__MANDATORY__)
        ASSERT_STREQ(cloudX->redirectUri, cloud->redirectUri);
#endif // __MANDATORY__
    }

    FreeCloud(cloudX);
    FreeCloud(cloud);
    OCPayloadDestroy((OCPayload *)payload);
}

TEST_F(CLOUD, strCopyFULL)
{
    EXPECT_FALSE(strCopy(NULL, NULL));
    char *sample1 = (char *)OICCalloc(1, 1024);
    EXPECT_TRUE(NULL != sample1);
    snprintf(sample1, 1024, "sample1 test string");
    char *dst = (char *)OICCalloc(1, 1024);
    EXPECT_TRUE(strCopy(sample1, &dst));
    EXPECT_TRUE(NULL != dst);
    ASSERT_STREQ(sample1, dst);
    OICFree(dst);
    dst = NULL;
    EXPECT_TRUE(strCopy(sample1, &dst));
    EXPECT_TRUE(NULL != dst);
    ASSERT_STREQ(sample1, dst);
    OICFree(dst);

    OICFree(sample1);
}

TEST_F(CLOUD, CBORPayloadToCloudResourceFULL)
{
    OicCloud_t *cloud =  getCloud();
    uint8_t *payload = NULL;
    size_t size = 0;
    EXPECT_EQ(OC_STACK_OK, CloudToCBORPayloadResource(cloud, &payload, &size));
    EXPECT_TRUE(payload != NULL);

    OicCloud_t *cloudX = NULL;
    EXPECT_EQ(OC_STACK_OK, CBORPayloadToCloud(payload, size, &cloudX));
    ASSERT_TRUE(cloudX != NULL);
    ASSERT_STREQ(cloudX->apn, cloud->apn);
    ASSERT_STREQ(cloudX->cis, cloud->cis);
    ASSERT_EQ(OC_CLOUD_OK, cloudX->stat);

    FreeCloud(cloudX);
    FreeCloud(cloud);
    OCPayloadDestroy((OCPayload *)payload);
}

TEST_F(CLOUD, ValidCloudFULL)
{
    ASSERT_TRUE(false == ValidCloud(NULL));
    OicCloud_t *cloud =  getCloud();
    ASSERT_TRUE(false == ValidCloud(cloud));
    sprintf(cloud->cis, "coaps+tcp://");
    sprintf(cloud->redirectUri, "coaps+tcp://");
    ASSERT_TRUE(true == ValidCloud(cloud));
    OICFree(cloud->apn); cloud->apn = NULL;
    OICFree(cloud->at); cloud->at = NULL;
    ASSERT_TRUE(false == ValidCloud(cloud));
    FreeCloud(cloud);
}

TEST_F(CLOUD, DeleteCloudListFULL)
{
    DeleteCloudList(NULL, true);
    OicCloud_t *cloud =  getCloud();
    cloud->next =  getCloud();
    cloud->next->next =  getCloud();
    DeleteCloudList(cloud, true);
}

TEST_F(CLOUD, CreateCloudGetPayloadFULL)
{
    OCRepPayload *payload = CreateCloudGetPayload(NULL);
    ASSERT_TRUE(NULL != payload);

    OicCloud_t *cloud =  getCloud();
    ASSERT_TRUE(NULL != cloud);
    OCPayloadDestroy((OCPayload *)payload);
    payload = CreateCloudGetPayload(cloud);
    ASSERT_TRUE(NULL != payload);

    OICFree(cloud->apn);
    cloud->apn = NULL;
    OCPayloadDestroy((OCPayload *)payload);
    payload = CreateCloudGetPayload(cloud);
    ASSERT_TRUE(NULL != payload);

    FreeCloud(cloud);
    OCPayloadDestroy((OCPayload *)payload);
}

TEST_F(CLOUD, StartCloudFULL)
{
    ASSERT_TRUE(OC_STACK_OK == SetDosState(DOS_RESET));
    StartClouds();

    OicUuid_t *uuid = (OicUuid_t *)OICCalloc(1, sizeof(OicUuid_t));
    ASSERT_TRUE(OC_STACK_OK == ConvertStrToUuid("33333333-3333-3333-3333-333333333333", uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmDevOwnerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmIsOwned(true));
    ASSERT_TRUE(OC_STACK_OK == SetAclRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetCredRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetPstatRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDosState(DOS_RFPRO));
    StartClouds();
    OICFree(uuid);
}

TEST_F(CLOUD, HandleCloudPostRequestFULL)
{
    #define _MULTIPLE_CNC_
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudPostRequest(NULL));

    OCEntityHandlerRequest *ehRequest = (OCEntityHandlerRequest *)OICCalloc(1,
                                        sizeof(OCEntityHandlerRequest));
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudPostRequest(ehRequest));

    ehRequest->payload = (OCPayload *)OCRepPayloadCreate();
    ASSERT_TRUE(NULL != ehRequest->payload);
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudPostRequest(ehRequest));

    ASSERT_TRUE(OC_STACK_OK == SetDosState(DOS_RESET));
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudPostRequest(ehRequest));
    OicUuid_t *uuid = (OicUuid_t *)OICCalloc(1, sizeof(OicUuid_t));
    ASSERT_TRUE(OC_STACK_OK == ConvertStrToUuid("33333333-3333-3333-3333-333333333333", uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmDevOwnerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmIsOwned(true));
    ASSERT_TRUE(OC_STACK_OK == SetAclRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetCredRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetPstatRownerId(uuid));
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudPostRequest(ehRequest));
    ASSERT_TRUE(OC_STACK_OK == SetDosState(DOS_RFPRO));
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudPostRequest(ehRequest));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmIsOwned(false));
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudPostRequest(ehRequest));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmIsOwned(true));

    OicCloud_t *cloud =  getCloud();
    ASSERT_TRUE(NULL != cloud);

    OCRepPayload *payload = NULL;
    payload = OCRepPayloadCreate();
    ASSERT_TRUE(NULL != payload);
    ehRequest->payload = (OCPayload *)payload;

    sprintf(cloud->cis, "coaps+tcp://192.168.1.1:90");
    sprintf(cloud->redirectUri, "coaps+tcp://192.168.1.1:91");
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudPostRequest(ehRequest));
    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_APN, cloud->apn);
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudPostRequest(ehRequest));
    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_CIS, cloud->cis);
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudPostRequest(ehRequest));
    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_AT, cloud->at);
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudPostRequest(ehRequest));
    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_SID, cloud->sid);
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudPostRequest(ehRequest));
    OCRepPayloadSetPropInt(payload, OIC_JSON_CLOUD_CLEC, (int64_t)cloud->stat);
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudPostRequest(ehRequest));

    CloudsSignOut();

    OICFree(uuid);
    FreeCloud(cloud);
    ASSERT_TRUE(OC_STACK_OK == SetDosState(DOS_RFNOP));
    OCPayloadDestroy(ehRequest->payload);
    OICFree(ehRequest);
}

TEST_F(CLOUD, HandleCloudGetRequestFULL)
{
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudGetRequest(NULL));
    OCRepPayload *payload = NULL;

    OCEntityHandlerRequest *ehRequest = (OCEntityHandlerRequest *)OICCalloc(1,
                                        sizeof(OCEntityHandlerRequest));
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudGetRequest(ehRequest));

    ehRequest->payload = (OCPayload *)OCRepPayloadCreate();
    ASSERT_TRUE(NULL != ehRequest->payload);
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudGetRequest(ehRequest));

    ASSERT_TRUE(OC_STACK_OK == SetDosState(DOS_RESET));
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudGetRequest(ehRequest));

    OicUuid_t *uuid = (OicUuid_t *)OICCalloc(1, sizeof(OicUuid_t));
    ASSERT_TRUE(OC_STACK_OK == ConvertStrToUuid("33333333-3333-3333-3333-333333333333", uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmDevOwnerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmIsOwned(true));
    ASSERT_TRUE(OC_STACK_OK == SetAclRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetCredRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetPstatRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDosState(DOS_RFPRO));
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudGetRequest(ehRequest));

    OicCloud_t *cloud =  getCloud();
    ASSERT_TRUE(NULL != cloud);
    sprintf(cloud->cis, "coaps+tcp://");

    OCPayloadDestroy(ehRequest->payload);
    payload = OCRepPayloadCreate();
    ASSERT_TRUE(NULL != payload);
    ehRequest->payload = (OCPayload *)payload;

    ASSERT_TRUE(OC_EH_ERROR == HandleCloudGetRequest(ehRequest));
    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_CIS, cloud->cis);
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudGetRequest(ehRequest));
    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_APN, cloud->apn);
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudGetRequest(ehRequest));
    OCRepPayloadSetPropString(payload, OIC_JSON_CLOUD_AT, cloud->at);
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudGetRequest(ehRequest));

    OICFree(uuid);
    FreeCloud(cloud);
    ASSERT_TRUE(OC_STACK_OK == SetDosState(DOS_RFNOP));
    OCPayloadDestroy(ehRequest->payload);
    OICFree(ehRequest);
}

TEST_F(CLOUD, HandleCloudDeleteRequestFULL)
{
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudDeleteRequest(NULL));

    OCEntityHandlerRequest *ehRequest = (OCEntityHandlerRequest *)OICCalloc(1,
                                        sizeof(OCEntityHandlerRequest));
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudDeleteRequest(ehRequest));

    ehRequest->payload = (OCPayload *)OCRepPayloadCreate();
    ASSERT_TRUE(NULL != ehRequest->payload);
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudDeleteRequest(ehRequest));

    ASSERT_TRUE(OC_STACK_OK == SetDosState(DOS_RESET));
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudDeleteRequest(ehRequest));

    OicUuid_t *uuid = (OicUuid_t *)OICCalloc(1, sizeof(OicUuid_t));
    ASSERT_TRUE(OC_STACK_OK == ConvertStrToUuid("33333333-3333-3333-3333-333333333333", uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmDevOwnerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmIsOwned(true));
    ASSERT_TRUE(OC_STACK_OK == SetAclRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetCredRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetPstatRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDosState(DOS_RFPRO));
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudDeleteRequest(ehRequest));

    OCPayloadDestroy(ehRequest->payload);
    ASSERT_TRUE(NULL != ehRequest->payload);
    OicCloud_t *cloud =  getCloud();
    ASSERT_TRUE(NULL != cloud);
    sprintf(cloud->cis, "coaps+tcp://");
    ehRequest->payload = (OCPayload *)CreateCloudGetPayload(cloud);
    ASSERT_TRUE(NULL != ehRequest->payload);
    OCRepPayloadSetPropString((OCRepPayload *)ehRequest->payload, OIC_JSON_CLOUD_AT, "3453245234");
    OCRepPayloadSetPropString((OCRepPayload *)ehRequest->payload, OC_CLOUD_PROVISIONING_CIS,
                              cloud->cis);
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudDeleteRequest(ehRequest));

    OICFree(uuid);
    ASSERT_TRUE(OC_STACK_OK == SetDosState(DOS_RFNOP));
    OCPayloadDestroy(ehRequest->payload);
    FreeCloud(cloud);
    OICFree(ehRequest);
}

TEST_F(CLOUD, CloudEntityHandlerFULL)
{
    ASSERT_TRUE(OC_EH_ERROR == CloudEntityHandler((OCEntityHandlerFlag)0, NULL, NULL));
    ASSERT_TRUE(OC_EH_ERROR == CloudEntityHandler(OC_OBSERVE_FLAG, NULL, NULL));
    ASSERT_TRUE(OC_EH_ERROR == CloudEntityHandler(OC_REQUEST_FLAG, NULL, NULL));
    OCEntityHandlerRequest *ehRequest = (OCEntityHandlerRequest *)OICCalloc(1,
                                        sizeof(OCEntityHandlerRequest));
    ASSERT_TRUE(OC_EH_ERROR == CloudEntityHandler(OC_REQUEST_FLAG, ehRequest, NULL));
    ehRequest->method = OC_REST_POST;
    ASSERT_TRUE(OC_EH_ERROR == CloudEntityHandler(OC_REQUEST_FLAG, ehRequest, NULL));
    ehRequest->method = OC_REST_GET;
    ASSERT_TRUE(OC_EH_ERROR == CloudEntityHandler(OC_REQUEST_FLAG, ehRequest, NULL));
    ehRequest->method = OC_REST_DELETE;
    ASSERT_TRUE(OC_EH_ERROR == CloudEntityHandler(OC_REQUEST_FLAG, ehRequest, NULL));
    OICFree(ehRequest);
}

OCStackApplicationResult resultHandler(void *context, OCDoHandle handle,
                                       OCClientResponse *clientResponse)
{
    OC_UNUSED(context);
    OC_UNUSED(handle);
    OC_UNUSED(clientResponse);

    return OC_STACK_DELETE_TRANSACTION;
}

TEST_F(CLOUD, OCProvisionCloudConfigFULL)
{
    void *ctx = NULL;
    OCProvisionDev_t *pDev = NULL;
    OicCloud_t *cloud = NULL;
    OCClientResponseHandler resultCallback = NULL;

    ASSERT_TRUE(OC_STACK_INVALID_PARAM == OCProvisionCloudConfig(ctx, pDev, cloud, resultCallback));
    ctx = (void *)OICCalloc(1, 1024);
    ASSERT_TRUE(OC_STACK_INVALID_PARAM == OCProvisionCloudConfig(ctx, pDev, cloud, resultCallback));
    pDev = (OCProvisionDev_t *)OICCalloc(1, sizeof(OCProvisionDev_t));
    ASSERT_TRUE(OC_STACK_INVALID_PARAM == OCProvisionCloudConfig(ctx, pDev, cloud, resultCallback));
    FreeCloud(cloud);
    cloud =  getCloud();
    ASSERT_TRUE(OC_STACK_INVALID_PARAM == OCProvisionCloudConfig(ctx, pDev, cloud, resultCallback));
    resultCallback = resultHandler;
    ASSERT_TRUE(OC_STACK_ERROR == OCProvisionCloudConfig(ctx, pDev, cloud, resultCallback));

    pDev = createProvisionDev();
    ASSERT_TRUE(OC_STACK_OK == OCProvisionCloudConfig(ctx, pDev, cloud, resultCallback));

    freeProvisionDev(pDev);
    FreeCloud(cloud);
    OICFree(ctx);
}

static OCStackApplicationResult clientResponseHandler(void *context, OCDoHandle handle,
        OCClientResponse *clientResponse)
{
    OC_UNUSED(context);
    OC_UNUSED(handle);
    OC_UNUSED(clientResponse);
    OCStackApplicationResult ret = OC_STACK_DELETE_TRANSACTION;
    return ret;
}

TEST_F(CLOUD, handleCloudStatusResponseFULL)
{
    void *ctx = NULL;
    OCDoHandle handle = NULL;
    OCClientResponse *response = NULL;

    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudStatusResponse(ctx, handle, response));
    ctx = (void *)&clientResponseHandler;
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudStatusResponse(ctx, handle, response));
    response = (OCClientResponse *)OICCalloc(1, sizeof(*response));
    response->payload = (OCPayload *)OCRepPayloadCreate();
    response->payload->type = PAYLOAD_TYPE_REPRESENTATION;
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudStatusResponse(ctx, handle, response));
    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OIC_JSON_CLOUD_APN, "github");
    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OIC_JSON_CLOUD_CIS,
                              "http://127.0.0.1:80");
    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OIC_JSON_CLOUD_SID, "3453245234");
    OCRepPayloadSetPropInt((OCRepPayload *)response->payload, OIC_JSON_CLOUD_CLEC, 23423);
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudStatusResponse(ctx, handle, response));

    OCPayloadDestroy((OCPayload *)response->payload);
    OICFree(response);
}

TEST_F(CLOUD, OCRemoveCloudConfigCBFULL)
{
    void *ctx = NULL;
    OCDoHandle handle = NULL;
    OCClientResponse *response = NULL;

    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == OCRemoveCloudConfigCB(ctx, handle, response));
    ctx = (void *)OICCalloc(1, 1024);
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == OCRemoveCloudConfigCB(ctx, handle, response));
    response = (OCClientResponse *)OICCalloc(1, sizeof(*response));
    response->payload = (OCPayload *)OCRepPayloadCreate();
    response->payload->type = PAYLOAD_TYPE_REPRESENTATION;
    response->result = OC_STACK_RESOURCE_CHANGED;
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == OCRemoveCloudConfigCB(ctx, handle, response));

    Data_t *data = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ASSERT_TRUE(NULL != data);
    data->type = CLOUD_TYPE;
    CloudData_t *cloudData = (CloudData_t *)OICCalloc(1, sizeof(CloudData_t));
    OICFree(ctx);
    data->ctx = (void *)cloudData;
    ASSERT_TRUE(NULL != data->ctx);
    cloudData->ctx = (void *)OICCalloc(1, 1024);
    cloudData->targetDev = (OCProvisionDev_t *)OICCalloc(1, sizeof(OCProvisionDev_t));
    cloudData->numOfResults = 0;
    cloudData->credInfo = NULL;
    cloudData->resArr = (OCProvisionResult_t *)OICCalloc(1, sizeof(OCProvisionResult_t));
    ASSERT_TRUE(NULL != cloudData->resArr);
    cloudData->sid = (char *)OICCalloc(1, 1024);
    ASSERT_TRUE(NULL != cloudData->sid);
    ctx = (void *)data;
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == OCRemoveCloudConfigCB(ctx, handle, response));

    OICFree(cloudData->ctx);
    OICFree(cloudData->targetDev);
    OICFree(cloudData->resArr);
    OICFree(cloudData->sid);
    OICFree(cloudData);

    OICFree(data);

    OCPayloadDestroy((OCPayload *)response->payload);
    OICFree(response);
}

TEST_F(CLOUD, OCGetCloudStatusRequestFULL)
{
    void *ctx = NULL;
    OCProvisionDev_t *pDev = NULL;
    OicCloud_t *cloud = NULL;
    OCClientResponseHandler resultCallback = NULL;

    ASSERT_TRUE(OC_STACK_INVALID_PARAM == OCGetCloudStatusRequest(ctx, pDev, cloud, resultCallback));
    ctx = (void *)OICCalloc(1, 1024);
    ASSERT_TRUE(OC_STACK_INVALID_PARAM == OCGetCloudStatusRequest(ctx, pDev, cloud, resultCallback));
    pDev = (OCProvisionDev_t *)OICCalloc(1, sizeof(OCProvisionDev_t));
    ASSERT_TRUE(OC_STACK_INVALID_PARAM == OCGetCloudStatusRequest(ctx, pDev, cloud, resultCallback));
    cloud =  getCloud();
    ASSERT_TRUE(OC_STACK_INVALID_PARAM == OCGetCloudStatusRequest(ctx, pDev, cloud, resultCallback));
    resultCallback = resultHandler;
    ASSERT_TRUE(OC_STACK_ERROR == OCGetCloudStatusRequest(ctx, pDev, cloud, resultCallback));
    snprintf(pDev->endpoint.addr, sizeof(pDev->endpoint.addr), "127.0.0.1");
    pDev->securePort = 1024;
    pDev->connType = (OCConnectivityType)(CT_ADAPTER_IP | CT_FLAG_SECURE | CT_IP_USE_V4);
    ASSERT_TRUE(OC_STACK_OK == OCGetCloudStatusRequest(ctx, pDev, cloud, resultCallback));

    FreeCloud(cloud);
    OICFree(ctx);
    OICFree(pDev);
}


void resultCB(void *ctx, size_t nOfRes, OCProvisionResult_t *arr, bool hasError)
{
    OC_UNUSED(ctx);
    OC_UNUSED(nOfRes);
    OC_UNUSED(arr);
    OC_UNUSED(hasError);

    return;
}

TEST_F(CLOUD, OCRemoveCloudConfigFULL)
{
    void *ctx = NULL;
    OCProvisionDev_t *pDev = NULL;
    OicCloud_t *cloud = NULL;
    OCProvisionResultCB resultCallback = NULL;

    ASSERT_TRUE(OC_STACK_INVALID_PARAM == OCRemoveCloudConfig(ctx, pDev, cloud, resultCallback));
    ctx = (void *)OICCalloc(1, 1024);
    ASSERT_TRUE(OC_STACK_INVALID_PARAM == OCRemoveCloudConfig(ctx, pDev, cloud, resultCallback));
    pDev = (OCProvisionDev_t *)OICCalloc(1, sizeof(OCProvisionDev_t));
    ASSERT_TRUE(OC_STACK_INVALID_CALLBACK == OCRemoveCloudConfig(ctx, pDev, cloud, resultCallback));
    resultCallback = resultCB;
    ASSERT_TRUE(OC_STACK_INVALID_PARAM == OCRemoveCloudConfig(ctx, pDev, cloud, resultCallback));
    FreeCloud(cloud);
    cloud =  getCloud();
    ASSERT_TRUE(OC_STACK_ERROR == OCRemoveCloudConfig(ctx, pDev, cloud, resultCallback));
    snprintf(pDev->endpoint.addr, sizeof(pDev->endpoint.addr), "127.0.0.1");
    pDev->securePort = 1024;
    pDev->connType = (OCConnectivityType)(CT_ADAPTER_IP | CT_FLAG_SECURE | CT_IP_USE_V4);
    ASSERT_TRUE(OC_STACK_OK == OCRemoveCloudConfig(ctx, pDev, cloud, resultCallback));

    FreeCloud(cloud);
    OICFree(ctx);
    OICFree(pDev);
}

TEST_F(CLOUD, GetCloudStatusFULL)
{
    gCloud = getCloud();
    ASSERT_TRUE(NULL != gCloud);

    for ( CloudStatus i = OC_CLOUD_OK; i <= OC_CLOUD_EXIT; i = (CloudStatus)(i + 1))
    {
        gCloud->stat = i;
        const char *mes = GetCloudStatus(gCloud);
        ASSERT_TRUE(NULL != mes);
    }

    FreeCloud(gCloud);
    gCloud = NULL;
}

TEST_F(CLOUD, handleCloudTokenRefreshResponseFULL)
{
    void *ctx = NULL;
    OCDoHandle handle = NULL;
    OCClientResponse *response = NULL;
    OicCloud_t *cloud = getCloud();

    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudTokenRefreshResponse(ctx, handle, response));
    ctx = (void *)cloud;
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudTokenRefreshResponse(ctx, handle, response));
    response = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
//    response->result = (OCStackResult)LOGIN_OK;
    response->payload = (OCPayload *)OCRepPayloadCreate();
    response->payload->type = PAYLOAD_TYPE_REPRESENTATION;
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudTokenRefreshResponse(ctx, handle, response));

    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OC_RSRVD_REFRESH_TOKEN,
                              cloud->session->refreshToken);
    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OC_RSRVD_ACCESS_TOKEN,
                              cloud->session->accessToken);
    OCRepPayloadSetPropInt((OCRepPayload *)response->payload, OC_RSRVD_EXPIRES_IN,
                           cloud->session->expireSin);

    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudTokenRefreshResponse(ctx, handle, response));

    OCPayloadDestroy((OCPayload *)response->payload);
    OICFree(response);
    FreeCloud(cloud);
}

TEST_F(CLOUD, OCCloudTokenRefreshFULL)
{
    OicCloud_t *cloud = NULL;
    ASSERT_TRUE(OC_STACK_INVALID_PARAM == OCCloudTokenRefresh(cloud));

    cloud = getCloud();
    ASSERT_TRUE(OC_STACK_OK == OCCloudTokenRefresh(cloud));
    sprintf(cloud->cis, "coaps+tcp://127.0.0.1:5683");
    ASSERT_TRUE(OC_STACK_OK == OCCloudTokenRefresh(cloud));

    OicUuid_t *uuid = (OicUuid_t *)OICCalloc(1, sizeof(OicUuid_t));
    ASSERT_TRUE(OC_STACK_OK == ConvertStrToUuid("33333333-3333-3333-3333-333333333333", uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmDevOwnerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmRownerId(uuid));
    ASSERT_TRUE(OC_STACK_OK == SetDoxmIsOwned(true));
    ASSERT_TRUE(OC_STACK_OK == OCCloudTokenRefresh(cloud));

    OICFree(uuid);
    FreeCloud(cloud);
}

TEST_F(CLOUD, handleCloudSignUpResponseFULL)
{
    void *ctx = NULL;
    OCDoHandle handle = NULL;
    OCClientResponse *response = NULL;
    OicCloud_t *cloud = getCloud();

    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudSignUpResponse(ctx, handle, response));
    ctx = (void *)&clientResponseHandler;
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudSignUpResponse(ctx, handle, response));
    response = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    response->result = (OCStackResult)LOGIN_OK;
    response->payload = (OCPayload *)OCRepPayloadCreate();
    response->payload->type = PAYLOAD_TYPE_REPRESENTATION;
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudSignUpResponse(ctx, handle, response));

    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OC_RSRVD_ACCESS_TOKEN,
                              cloud->session->accessToken);
    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OC_RSRVD_REFRESH_TOKEN,
                              cloud->session->refreshToken);
    OCRepPayloadSetPropInt((OCRepPayload *)response->payload, OC_RSRVD_EXPIRES_IN,
                           cloud->session->expireSin);
    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OC_RSRVD_USER_UUID,
                              cloud->session->uid);
    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OC_RSRVD_REDIRECT_URI,
                              cloud->redirectUri);
    SessionFree(cloud->session);
    cloud->session = NULL;
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudSignUpResponse(ctx, handle, response));

    SessionFree(NULL);

    OCPayloadDestroy((OCPayload *)response->payload);
    FreeCloud(cloud);
    OICFree(response);
}

/*
TEST_F(CLOUD, CloudTokenRefreshFULL)
{
    OicCloud_t *cloud = NULL;

    ASSERT_TRUE(NULL == CloudTokenRefresh(cloud));
    cloud = getCloud();
    ASSERT_TRUE(OC_THREAD_SUCCESS == oc_thread_new(&cloud->pid, CloudTokenRefresh, cloud));
    cloud->stat = OC_CLOUD_EXIT;
    sleep(2);
    FreeCloud(cloud);
}
*/

TEST_F(CLOUD, handleCloudSignInResponseFULL)
{
    void *ctx = NULL;
    OCDoHandle handle = NULL;
    OCClientResponse *response = NULL;
    OicCloud_t *cloud = getCloud();

    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudSignInResponse(ctx, handle, response));
    ctx = (void *)cloud;
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudSignInResponse(ctx, handle, response));
    response = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    response->result = OC_STACK_OK;
    response->payload = (OCPayload *)OCRepPayloadCreate();
    response->payload->type = PAYLOAD_TYPE_REPRESENTATION;
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudSignInResponse(ctx, handle, response));
    response->result = (OCStackResult)LOGIN_OK;
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudSignInResponse(ctx, handle, response));

    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OC_RSRVD_ACCESS_TOKEN,
                              cloud->session->accessToken);
    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OC_RSRVD_REFRESH_TOKEN,
                              cloud->session->refreshToken);
    OCRepPayloadSetPropInt((OCRepPayload *)response->payload, OC_RSRVD_EXPIRES_IN,
                           cloud->session->expireSin);
    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OC_RSRVD_USER_UUID,
                              cloud->session->uid);
    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OC_RSRVD_REDIRECT_URI,
                              cloud->redirectUri);

    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudSignInResponse(ctx, handle, response));

    OCPayloadDestroy((OCPayload *)response->payload);
    OICFree(response);
    FreeCloud(cloud);
}

TEST_F(CLOUD, CloudSignFULL)
{
    OicCloud_t *cloud = NULL;

    ASSERT_TRUE(OC_STACK_INVALID_PARAM == CloudSign(cloud, true));
    cloud = getCloud();
    ASSERT_TRUE(OC_STACK_ERROR == CloudSign(cloud, true));
    cloud->stat = OC_CLOUD_SIGNUP;
    ASSERT_TRUE(OC_STACK_OK == CloudSign(cloud, true));
    ASSERT_TRUE(OC_STACK_OK == CloudSign(cloud, false));

    FreeCloud(cloud);
}

TEST_F(CLOUD, CloudSignUpParsePayloadFULL)
{
    OicCloud_t *cloud = NULL;
    OCRepPayload *payload = NULL;

    ASSERT_FALSE(CloudSignUpParsePayload(NULL, NULL));

    payload = OCRepPayloadCreate();
    ASSERT_TRUE(NULL != payload);

    cloud = (OicCloud_t *)OICCalloc(1, sizeof(OicCloud_t));
    ASSERT_TRUE(NULL != cloud);
    cloud->session = (session_t *)OICCalloc(1, sizeof(session_t));
    ASSERT_TRUE(NULL != cloud->session);

    ASSERT_FALSE(CloudSignUpParsePayload(payload, cloud));
    FreeCloud(cloud);
    cloud = (OicCloud_t *)OICCalloc(1, sizeof(OicCloud_t));
    ASSERT_TRUE(NULL != cloud);
    cloud->session = (session_t *)OICCalloc(1, sizeof(session_t));
    ASSERT_TRUE(NULL != cloud->session);

    OCRepPayloadSetPropString(payload, OC_RSRVD_ACCESS_TOKEN, OC_RSRVD_ACCESS_TOKEN);
    ASSERT_FALSE(CloudSignUpParsePayload(payload, cloud));
    ASSERT_TRUE(NULL != cloud->session->accessToken);
    FreeCloud(cloud);
    cloud = (OicCloud_t *)OICCalloc(1, sizeof(OicCloud_t));
    ASSERT_TRUE(NULL != cloud);
    cloud->session = (session_t *)OICCalloc(1, sizeof(session_t));
    ASSERT_TRUE(NULL != cloud->session);

    OCRepPayloadSetPropString(payload, OC_RSRVD_REFRESH_TOKEN, OC_RSRVD_REFRESH_TOKEN);
    ASSERT_FALSE(CloudSignUpParsePayload(payload, cloud));
    ASSERT_TRUE(NULL != cloud);
    FreeCloud(cloud);
    cloud = (OicCloud_t *)OICCalloc(1, sizeof(OicCloud_t));
    ASSERT_TRUE(NULL != cloud);
    cloud->session = (session_t *)OICCalloc(1, sizeof(session_t));
    ASSERT_TRUE(NULL != cloud->session);

    OCRepPayloadSetPropInt(payload, OC_RSRVD_EXPIRES_IN, 10);
    ASSERT_FALSE(CloudSignUpParsePayload(payload, cloud));
    ASSERT_TRUE(NULL != cloud);
    FreeCloud(cloud);
    cloud = (OicCloud_t *)OICCalloc(1, sizeof(OicCloud_t));
    ASSERT_TRUE(NULL != cloud);
    cloud->session = (session_t *)OICCalloc(1, sizeof(session_t));
    ASSERT_TRUE(NULL != cloud->session);

    OCRepPayloadSetPropString(payload, OC_RSRVD_USER_UUID, OC_RSRVD_USER_UUID);
    ASSERT_TRUE(CloudSignUpParsePayload(payload, cloud));
    ASSERT_TRUE(NULL != cloud);
    FreeCloud(cloud);
    cloud = (OicCloud_t *)OICCalloc(1, sizeof(OicCloud_t));
    ASSERT_TRUE(NULL != cloud);
    cloud->session = (session_t *)OICCalloc(1, sizeof(session_t));
    ASSERT_TRUE(NULL != cloud->session);


    OCRepPayloadSetPropString(payload, OC_RSRVD_REDIRECT_URI,
                              cloud->redirectUri);
    ASSERT_TRUE(CloudSignUpParsePayload(payload, cloud));
    ASSERT_TRUE(NULL != cloud);

    OCPayloadDestroy((OCPayload *)payload);
    FreeCloud(cloud);
}

TEST_F(CLOUD, handleCloudSignOutResponseFULL)
{
    void *ctx = NULL;
    OCDoHandle handle = NULL;
    OCClientResponse *response = NULL;
    OicCloud_t *cloud = getCloud();

    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudSignOutResponse(ctx, handle, response));
    ctx = (void *)cloud;
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudSignOutResponse(ctx, handle, response));
    response = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    response->result = (OCStackResult)LOGIN_OK;
    response->payload = (OCPayload *)OCRepPayloadCreate();
    response->payload->type = PAYLOAD_TYPE_REPRESENTATION;
    ASSERT_TRUE(OC_STACK_DELETE_TRANSACTION == handleCloudSignOutResponse(ctx, handle, response));

    OCPayloadDestroy((OCPayload *)response->payload);
    OICFree(response);
}

TEST_F(CLOUD, UpdateCloudPersistentStorageFULL)
{
    DeleteCloudList(gCloud, true);
    ASSERT_TRUE(false == UpdateCloudPersistentStorage());
    OicCloud_t *cloud = getCloud();
    LL_APPEND(gCloud, cloud);
    ASSERT_TRUE(true == UpdateCloudPersistentStorage());
    DeleteCloudList(gCloud, true);
    gCloud = NULL;
}

TEST_F(CLOUD, handleAclCreateGroupResponse)
{
    OCClientResponse *response = NULL;
    char *data = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == handleAclCreateGroupResponse(NULL, NULL, NULL));

    response = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == handleAclCreateGroupResponse(NULL, (void **)&data, response));

    response->payload = (OCPayload *)OCRepPayloadCreate();
    EXPECT_TRUE(OC_STACK_MALFORMED_RESPONSE == handleAclCreateGroupResponse(NULL, (void **)&data,
                response));

    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OC_RSRVD_GROUP_ID, "val");
    EXPECT_TRUE(OC_STACK_OK == handleAclCreateGroupResponse(NULL, (void **)&data, response));

    OCPayloadDestroy((OCPayload *)response->payload);
    OICFree(response);
}

TEST_F(CLOUD, handleAclFindMyGroupResponse)
{
    OCClientResponse *response = NULL;
    char *data = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == handleAclFindMyGroupResponse(NULL, NULL, NULL));

    response = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == handleAclFindMyGroupResponse(NULL, (void **)&data, response));

    response->payload = (OCPayload *)OCRepPayloadCreate();
    EXPECT_TRUE(OC_STACK_MALFORMED_RESPONSE == handleAclFindMyGroupResponse(NULL, (void **)&data,
                response));

    const char **array = (const char **)OICCalloc(2, sizeof(const char *));
    array[0] = OICStrdup("val0");
// TODO wait for https://gerrit.iotivity.org/gerrit/#/c/29354
#if defined(BUG_FIX)
    array[1] = OICStrdup("val1");
    size_t dim[MAX_REP_ARRAY_DEPTH] = {5, 5, 0};
#else
    size_t dim[MAX_REP_ARRAY_DEPTH] = {5, 0, 0};
#endif
    EXPECT_TRUE(OCRepPayloadSetStringArray((OCRepPayload *)response->payload, OC_RSRVD_GROUP_ID_LIST,
                                           array, dim));
    EXPECT_TRUE(OC_STACK_OK == handleAclFindMyGroupResponse(NULL, (void **)&data, response));

    OCPayloadDestroy((OCPayload *)response->payload);
    OICFree((void *)array[1]);
    OICFree((void *)array[0]);
    OICFree(array);
    OICFree(response);
}

TEST_F(CLOUD, OCCloudAclCreateGroup)
{
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclCreateGroup(NULL, NULL, NULL, NULL, NULL));
    EXPECT_TRUE(OC_STACK_OK == OCCloudAclCreateGroup(NULL, "group", "grandmaster",
                "coaps+tcp://127.0.0.1:7777", NULL));
}

TEST_F(CLOUD, OCCloudAclFindMyGroup)
{
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclFindMyGroup(NULL, NULL, NULL, NULL));
    EXPECT_TRUE(OC_STACK_OK == OCCloudAclFindMyGroup(NULL, "theId", "coaps+tcp://127.0.0.1:7777",
                NULL));
}

TEST_F(CLOUD, OCCloudAclDeleteGroup)
{
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclDeleteGroup(NULL, NULL, NULL, NULL, NULL));
    EXPECT_TRUE(OC_STACK_OK == OCCloudAclDeleteGroup(NULL, "group", "grandmaster",
                "coaps+tcp://127.0.0.1:7777", NULL));
}

TEST_F(CLOUD, OCCloudAclJoinToInvitedGroup)
{
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclJoinToInvitedGroup(NULL, NULL, NULL, NULL));
    EXPECT_TRUE(OC_STACK_OK == OCCloudAclJoinToInvitedGroup(NULL, "theId", "coaps+tcp://127.0.0.1:7777",
                NULL));
}

TEST_F(CLOUD, OCCloudAclObserveGroup)
{
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclObserveGroup(NULL, NULL, NULL, NULL));
    EXPECT_TRUE(OC_STACK_OK == OCCloudAclObserveGroup(NULL, "theId", "coaps+tcp://127.0.0.1:7777",
                NULL));
}

TEST_F(CLOUD, OCCloudAclShareDeviceIntoGroup)
{
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclShareDeviceIntoGroup(NULL, NULL, NULL, NULL, NULL,
                NULL));
    stringArray_t *memberIds = (stringArray_t *)OICCalloc(1, sizeof(stringArray_t));
    stringArray_t *deviceIds = (stringArray_t *)OICCalloc(1, sizeof(stringArray_t));

    EXPECT_TRUE(OC_STACK_OK == OCCloudAclShareDeviceIntoGroup(NULL, "theId", memberIds, deviceIds,
                "coaps+tcp://127.0.0.1:7777", NULL));

    OICFree(memberIds);
    OICFree(deviceIds);
}

TEST_F(CLOUD, OCCloudAclDeleteDeviceFromGroup)
{
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclDeleteDeviceFromGroup(NULL, NULL, NULL, NULL, NULL,
                NULL));
    stringArray_t *memberIds = (stringArray_t *)OICCalloc(1, sizeof(stringArray_t));
    stringArray_t *deviceIds = (stringArray_t *)OICCalloc(1, sizeof(stringArray_t));

    EXPECT_TRUE(OC_STACK_OK == OCCloudAclDeleteDeviceFromGroup(NULL, "theId", memberIds, deviceIds,
                "coaps+tcp://127.0.0.1:7777", NULL));

    OICFree(memberIds);
    OICFree(deviceIds);
}

TEST_F(CLOUD, OCCloudAclGroupGetInfo)
{
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclGroupGetInfo(NULL, NULL, NULL, NULL, NULL));
    EXPECT_TRUE(OC_STACK_OK == OCCloudAclGroupGetInfo(NULL, "groupId", "memberId",
                "coaps+tcp://127.0.0.1:7777", NULL));
}

#define CLOUD_URI "coaps+tcp://cloud_uri.com"
#define X_UUID "33333333-3333-3333-3333-222222222221"

static void cloudResponseCB(void *ctx, OCClientResponse *response, void *data)
{
    OC_UNUSED(ctx);
    OC_UNUSED(response);
    OC_UNUSED(data);
    OIC_LOG_V(DEBUG, TAG, "%s: data: %s", __func__, (const char *)data);
}

TEST_F(CLOUD, getAclIdFromResponse)
{
    OCClientResponse *response = NULL;
    char *data = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == getAclIdFromResponse(NULL, NULL, NULL));

    response = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == getAclIdFromResponse(NULL, (void **)&data, response));

    response->payload = (OCPayload *)OCRepPayloadCreate();
    EXPECT_TRUE(OC_STACK_MALFORMED_RESPONSE == getAclIdFromResponse(NULL, (void **)&data, response));

    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OC_RSRVD_ACL_ID, "acl_id");
    EXPECT_TRUE(OC_STACK_OK == getAclIdFromResponse(NULL, (void **)&data, response));
    EXPECT_TRUE(OC_STACK_OK == handleGetAclIdByDeviceResponse(NULL, (void **)&data, response));
    EXPECT_TRUE(OC_STACK_OK == handleAclIdCreateResponse(NULL, (void **)&data, response));

    OCPayloadDestroy((OCPayload *)response->payload);
    OICFree(response);
}

TEST_F(CLOUD, OCCloudGetAclIdByDevice)
{
    const char *deviceId = NULL;
    const char *cloudUri = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudGetAclIdByDevice(NULL, NULL, NULL, NULL));

    deviceId = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    snprintf((char *)deviceId, MAX_URI_LENGTH, X_UUID);
    cloudUri = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    size_t ret = snprintf((char *)cloudUri, MAX_URI_LENGTH, CLOUD_URI);
    memset((void *)(cloudUri + ret), 117, MAX_URI_LENGTH - ret);

    EXPECT_TRUE(OC_STACK_INVALID_URI == OCCloudGetAclIdByDevice(NULL, deviceId, cloudUri,
                cloudResponseCB));
    memset((void *)(cloudUri + ret), 0, MAX_URI_LENGTH - ret);
    EXPECT_TRUE(OC_STACK_OK == OCCloudGetAclIdByDevice(NULL, deviceId, cloudUri, cloudResponseCB));

    OICFree((void *)deviceId);
    OICFree((void *)cloudUri);
}

TEST_F(CLOUD, OCCloudAclIdCreate)
{
    const char *ownerId = NULL;
    const char *deviceId = NULL;
    const char *cloudUri = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclIdCreate(NULL, NULL, NULL, NULL, NULL));

    ownerId = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    snprintf((char *)ownerId, MAX_URI_LENGTH, X_UUID);
    deviceId = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    snprintf((char *)deviceId, MAX_URI_LENGTH, "33333333-3333-3333-3333-222222222222");
    cloudUri = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    size_t ret = snprintf((char *)cloudUri, MAX_URI_LENGTH, CLOUD_URI);
    memset((void *)(cloudUri + ret), 117, MAX_URI_LENGTH - ret);

    EXPECT_TRUE(OC_STACK_INVALID_URI == OCCloudAclIdCreate(NULL, ownerId, deviceId, cloudUri,
                cloudResponseCB));
    memset((void *)(cloudUri + ret), 0, MAX_URI_LENGTH - ret);
    EXPECT_TRUE(OC_STACK_OK == OCCloudAclIdCreate(NULL, ownerId, deviceId, cloudUri, cloudResponseCB));

    OICFree((void *)deviceId);
    OICFree((void *)cloudUri);
    OICFree((void *)ownerId);
}

TEST_F(CLOUD, OCCloudAclIdDelete)
{
    const char *aclId = NULL;
    const char *cloudUri = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclIdDelete(NULL, NULL, NULL, NULL));

    aclId = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    snprintf((char *)aclId, MAX_URI_LENGTH, X_UUID);
    cloudUri = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    size_t ret = snprintf((char *)cloudUri, MAX_URI_LENGTH, CLOUD_URI);
    memset((void *)(cloudUri + ret), 117, MAX_URI_LENGTH - ret);

    EXPECT_TRUE(OC_STACK_INVALID_URI == OCCloudAclIdDelete(NULL, aclId, cloudUri, cloudResponseCB));
    memset((void *)(cloudUri + ret), 0, MAX_URI_LENGTH - ret);
    EXPECT_TRUE(OC_STACK_OK == OCCloudAclIdDelete(NULL, aclId, cloudUri, cloudResponseCB));

    OICFree((void *)cloudUri);
    OICFree((void *)aclId);
}

TEST_F(CLOUD, handleAclGetInfoResponse)
{
    OCClientResponse *response = NULL;
    char *data = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == handleAclGetInfoResponse(NULL, NULL, NULL));

    response = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == handleAclGetInfoResponse(NULL, (void **)&data, response));

    response->payload = (OCPayload *)OCRepPayloadCreate();
    EXPECT_TRUE(OC_STACK_OK == handleAclGetInfoResponse(NULL, (void **)&data, response));

    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OC_RSRVD_ACL_ID, "acl_id");
    EXPECT_TRUE(OC_STACK_OK == handleAclGetInfoResponse(NULL, (void **)&data, response));

    OCPayloadDestroy((OCPayload *)response->payload);
    OICFree(response);
}

TEST_F(CLOUD, OCCloudAclIndividualGetInfo)
{
    const char *aclId = NULL;
    const char *cloudUri = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclIndividualGetInfo(NULL, NULL, NULL, NULL));

    aclId = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    snprintf((char *)aclId, MAX_URI_LENGTH, X_UUID);
    cloudUri = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    size_t ret = snprintf((char *)cloudUri, MAX_URI_LENGTH, CLOUD_URI);
    memset((void *)(cloudUri + ret), 117, MAX_URI_LENGTH - ret);

    EXPECT_TRUE(OC_STACK_INVALID_URI == OCCloudAclIndividualGetInfo(NULL, aclId, cloudUri,
                cloudResponseCB));
    memset((void *)(cloudUri + ret), 0, MAX_URI_LENGTH - ret);
    EXPECT_TRUE(OC_STACK_OK == OCCloudAclIndividualGetInfo(NULL, aclId, cloudUri, cloudResponseCB));

    OICFree((void *)cloudUri);
    OICFree((void *)aclId);
}

TEST_F(CLOUD, OCCloudAclIndividualAclUpdate)
{
    const char *aclId = NULL;
    const char *cloudUri = NULL;
    cloudAce_t *ace = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclIndividualAclUpdate(NULL, NULL, NULL, NULL, NULL));

    aclId = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    snprintf((char *)aclId, MAX_URI_LENGTH, X_UUID);
    cloudUri = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    size_t ret = snprintf((char *)cloudUri, MAX_URI_LENGTH, CLOUD_URI);
    memset((void *)(cloudUri + ret), 117, MAX_URI_LENGTH - ret);

    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCCloudAclIndividualAclUpdate(NULL, aclId, ace, cloudUri,
              cloudResponseCB));
    memset((void *)(cloudUri + ret), 0, MAX_URI_LENGTH - ret);

    ace = (cloudAce_t *)OICCalloc(1, sizeof(cloudAce_t));
    ace->aceId = OICStrdup("ace_id");
    EXPECT_TRUE(OC_STACK_OK == ConvertStrToUuid(X_UUID, &ace->subjectuuid));
    ace->stype = 1;
    ace->permission = 1;

    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCCloudAclIndividualAclUpdate(NULL, aclId, ace, cloudUri,
              cloudResponseCB));

    ace->resources = (OicSecRsrc_t *)OICCalloc(1, sizeof(OicSecRsrc_t));
    ace->resources->href = OICStrdup(OC_RSRVD_ACL_ID_URL);

    EXPECT_EQ(OC_STACK_OK, OCCloudAclIndividualAclUpdate(NULL, aclId, ace, cloudUri, cloudResponseCB));

    OICFree((void *)ace->resources->href);
    OICFree((void *)ace->resources);
    OICFree((void *)ace->aceId);
    OICFree((void *)ace);
    OICFree((void *)cloudUri);
    OICFree((void *)aclId);
}

TEST_F(CLOUD, OCCloudAclIndividualAceUpdate)
{
    const char *aclId = NULL;
    const char *aceId = NULL;
    const char *cloudUri = NULL;
    cloudAce_t *ace = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclIndividualAceUpdate(NULL, NULL, NULL, NULL, NULL,
                NULL));

    aclId = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    snprintf((char *)aclId, MAX_URI_LENGTH, X_UUID);
    aceId = OICStrdup("ace_id");
    cloudUri = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    size_t ret = snprintf((char *)cloudUri, MAX_URI_LENGTH, CLOUD_URI);
    memset((void *)(cloudUri + ret), 117, MAX_URI_LENGTH - ret);

    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCCloudAclIndividualAceUpdate(NULL, aclId, aceId, ace, cloudUri,
              cloudResponseCB));
    memset((void *)(cloudUri + ret), 0, MAX_URI_LENGTH - ret);

    ace = (cloudAce_t *)OICCalloc(1, sizeof(cloudAce_t));
    ace->aceId = OICStrdup("ace_id");
    EXPECT_TRUE(OC_STACK_OK == ConvertStrToUuid(X_UUID, &ace->subjectuuid));
    ace->stype = 1;
    ace->permission = 1;

    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCCloudAclIndividualAceUpdate(NULL, aclId, aceId, ace, cloudUri,
              cloudResponseCB));

    ace->resources = (OicSecRsrc_t *)OICCalloc(1, sizeof(OicSecRsrc_t));
    ace->resources->href = OICStrdup(OC_RSRVD_ACL_ID_URL);

    EXPECT_EQ(OC_STACK_OK, OCCloudAclIndividualAceUpdate(NULL, aclId, aceId, ace, cloudUri,
              cloudResponseCB));

    OICFree((void *)ace->resources->href);
    OICFree((void *)ace->resources);
    OICFree((void *)ace->aceId);
    OICFree((void *)ace);
    OICFree((void *)cloudUri);
    OICFree((void *)aceId);
    OICFree((void *)aclId);
}

TEST_F(CLOUD, OCCloudAclAcesDelete)
{
    const char *aclId = NULL;
    const char *cloudUri = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclAcesDelete(NULL, NULL, NULL, NULL));

    aclId = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    snprintf((char *)aclId, MAX_URI_LENGTH, X_UUID);
    cloudUri = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    size_t ret = snprintf((char *)cloudUri, MAX_URI_LENGTH, CLOUD_URI);
    memset((void *)(cloudUri + ret), 117, MAX_URI_LENGTH - ret);

    EXPECT_EQ(OC_STACK_INVALID_URI, OCCloudAclAcesDelete(NULL, aclId, cloudUri, cloudResponseCB));
    memset((void *)(cloudUri + ret), 0, MAX_URI_LENGTH - ret);

    EXPECT_EQ(OC_STACK_OK, OCCloudAclAcesDelete(NULL, aclId, cloudUri, cloudResponseCB));

    OICFree((void *)cloudUri);
    OICFree((void *)aclId);
}

TEST_F(CLOUD, OCCloudAclIndividualAceDelete)
{
    const char *aclId = NULL;
    const char *aceId = NULL;
    const char *cloudUri = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclIndividualAceDelete(NULL, NULL, NULL, NULL, NULL));

    aclId = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    snprintf((char *)aclId, MAX_URI_LENGTH, X_UUID);
    aceId = OICStrdup("ace_id");
    cloudUri = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    size_t ret = snprintf((char *)cloudUri, MAX_URI_LENGTH, CLOUD_URI);
    memset((void *)(cloudUri + ret), 117, MAX_URI_LENGTH - ret);

    EXPECT_EQ(OC_STACK_INVALID_URI, OCCloudAclIndividualAceDelete(NULL, aclId, aceId, cloudUri,
              cloudResponseCB));
    memset((void *)(cloudUri + ret), 0, MAX_URI_LENGTH - ret);

    EXPECT_EQ(OC_STACK_OK, OCCloudAclIndividualAceDelete(NULL, aclId, aceId, cloudUri,
              cloudResponseCB));

    OICFree((void *)cloudUri);
    OICFree((void *)aceId);
    OICFree((void *)aclId);
}

//-------------------aclinvite-----------------
TEST_F(CLOUD, parseInvitePayload)
{
    OCRepPayload *payload = (OCRepPayload *)OCRepPayloadCreate();
    stringArrayPair_t *out = (stringArrayPair_t *)OICCalloc(1, sizeof(stringArrayPair_t));
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = {0, 0, 0};

    EXPECT_EQ(OC_STACK_MALFORMED_RESPONSE, parseInvitePayload(NULL, NULL, NULL));

    EXPECT_EQ(OC_STACK_MALFORMED_RESPONSE, parseInvitePayload(payload, OC_RSRVD_RESOURCES, out));

    OCRepPayload **helperPayload = (OCRepPayload **)OICCalloc(1, sizeof(OCRepPayload *));
    *helperPayload = OCRepPayloadCreate();
    OCRepPayloadSetPropString(*helperPayload, OC_RSRVD_GROUP_ID, "12");
    OCRepPayloadSetPropString(*helperPayload, OC_RSRVD_MEMBER_ID, "2");
    dimensions[0] = 1;
    EXPECT_TRUE(OCRepPayloadSetPropObjectArray(payload, "arrId",
                (const OCRepPayload **)helperPayload, dimensions));

    EXPECT_EQ(OC_STACK_OK, parseInvitePayload(payload, "arrId", out));
    OICFree(out);
}

TEST_F(CLOUD, handleAclGetInvitationResponse)
{
    OCClientResponse *response = NULL;
    char *data = NULL;
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = {0, 0, 0};
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == handleAclGetInvitationResponse(NULL, NULL, NULL));

    response = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == handleAclGetInvitationResponse(NULL, (void **)&data,
                response));
    response->payload = (OCPayload *)OCRepPayloadCreate();
    EXPECT_TRUE(OC_STACK_MALFORMED_RESPONSE == handleAclGetInvitationResponse(NULL, (void **)&data,
                response));

    OCRepPayload **helperPayload = (OCRepPayload **)OICCalloc(1, sizeof(OCRepPayload *));
    *helperPayload = OCRepPayloadCreate();
    OCRepPayloadSetPropString(*helperPayload, OC_RSRVD_GROUP_ID, "12");
    OCRepPayloadSetPropString(*helperPayload, OC_RSRVD_MEMBER_ID, "2");
    dimensions[0] = 1;
    EXPECT_TRUE(OCRepPayloadSetPropObjectArray((OCRepPayload *)response->payload, OC_RSRVD_INVITE,
                (const OCRepPayload **)helperPayload, dimensions));

    OCRepPayload **helperPayload1 = (OCRepPayload **)OICCalloc(1, sizeof(OCRepPayload *));
    *helperPayload1 = OCRepPayloadCreate();
    OCRepPayloadSetPropString(*helperPayload1, OC_RSRVD_GROUP_ID, "11");
    OCRepPayloadSetPropString(*helperPayload1, OC_RSRVD_MEMBER_ID, "1");
    dimensions[0] = 1;
    EXPECT_TRUE(OCRepPayloadSetPropObjectArray((OCRepPayload *)response->payload, OC_RSRVD_INVITED,
                (const OCRepPayload **)helperPayload1, dimensions));

    EXPECT_EQ(OC_STACK_OK, handleAclGetInvitationResponse(NULL, (void **)&data, response));

    OCPayloadDestroy((OCPayload *)response->payload);
    OICFree(response);
}

TEST_F(CLOUD, handleAclPolicyCheckResponse)
{
    OCClientResponse *response = NULL;
    char *data = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == handleAclPolicyCheckResponse(NULL, NULL, NULL));

    response = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == handleAclPolicyCheckResponse(NULL, (void **)&data, response));

    response->payload = (OCPayload *)OCRepPayloadCreate();
    EXPECT_TRUE(OC_STACK_MALFORMED_RESPONSE == handleAclPolicyCheckResponse(NULL, (void **)&data,
                response));

    OCRepPayloadSetPropString((OCRepPayload *)response->payload, OC_RSRVD_GROUP_PERMISSION, "0644");
    EXPECT_EQ(OC_STACK_OK, handleAclPolicyCheckResponse(NULL, (void **)&data, response));

    OCPayloadDestroy((OCPayload *)response->payload);
    OICFree(response);
}

TEST_F(CLOUD, OCCloudAclInviteUser)
{
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclInviteUser(NULL, NULL, NULL, NULL, NULL, NULL));

    stringArray_t *memberIds = (stringArray_t *)OICCalloc(1, sizeof(stringArray_t));
    stringArray_t *groupIds = (stringArray_t *)OICCalloc(1, sizeof(stringArray_t));
    groupIds->length = 1;
    groupIds->array = (char **)OICCalloc(groupIds->length, sizeof(char *));
    groupIds->array[groupIds->length - 1] = OICStrdup("root");

    memberIds->length = 1;
    memberIds->array = (char **)OICCalloc(memberIds ->length, sizeof(char *));
    memberIds->array[memberIds ->length - 1] = OICStrdup("root");

    EXPECT_EQ(OC_STACK_OK, OCCloudAclInviteUser(NULL, "007", groupIds, memberIds, CLOUD_URI,
              cloudResponseCB));
}

TEST_F(CLOUD, OCCloudAclGetInvitation)
{
    const char *userId = NULL;
    const char *cloudUri = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclGetInvitation(NULL, NULL, NULL, NULL));

    userId = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    snprintf((char *)userId, MAX_URI_LENGTH, X_UUID);
    cloudUri = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    size_t ret = snprintf((char *)cloudUri, MAX_URI_LENGTH, CLOUD_URI);
    memset((void *)(cloudUri + ret), 117, MAX_URI_LENGTH - ret);

    EXPECT_TRUE(OC_STACK_INVALID_URI == OCCloudAclGetInvitation(NULL, userId, cloudUri,
                cloudResponseCB));
    memset((void *)(cloudUri + ret), 0, MAX_URI_LENGTH - ret);
    EXPECT_TRUE(OC_STACK_OK == OCCloudAclGetInvitation(NULL, userId, cloudUri, cloudResponseCB));

    OICFree((void *)cloudUri);
    OICFree((void *)userId);

}

TEST_F(CLOUD, OCCloudAclDeleteInvitation)
{
    const char *ownerId = NULL;
    const char *deviceId = NULL;
    const char *cloudUri = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclDeleteInvitation(NULL, NULL, NULL, NULL, NULL));

    ownerId = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    snprintf((char *)ownerId, MAX_URI_LENGTH, X_UUID);
    deviceId = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    snprintf((char *)deviceId, MAX_URI_LENGTH, "33333333-3333-3333-3333-222222222222");
    cloudUri = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    size_t ret = snprintf((char *)cloudUri, MAX_URI_LENGTH, CLOUD_URI);
    memset((void *)(cloudUri + ret), 117, MAX_URI_LENGTH - ret);

    EXPECT_TRUE(OC_STACK_INVALID_URI == OCCloudAclDeleteInvitation(NULL, ownerId, deviceId, cloudUri,
                cloudResponseCB));
    memset((void *)(cloudUri + ret), 0, MAX_URI_LENGTH - ret);
    EXPECT_TRUE(OC_STACK_OK == OCCloudAclDeleteInvitation(NULL, ownerId, deviceId, cloudUri,
                cloudResponseCB));

    OICFree((void *)deviceId);
    OICFree((void *)cloudUri);
    OICFree((void *)ownerId);
}

TEST_F(CLOUD, OCCloudAclCancelInvitation)
{
    const char *userId = NULL;
    const char *groupId = NULL;
    const char *memberId = NULL;
    const char *cloudUri = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclCancelInvitation(NULL, NULL, NULL, NULL, NULL,
                NULL));

    userId = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    snprintf((char *)userId, MAX_URI_LENGTH, X_UUID);
    groupId = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    snprintf((char *)groupId, MAX_URI_LENGTH, "33333333-3333-3333-3333-222222222222");
    memberId = OICStrdup("root");
    cloudUri = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    size_t ret = snprintf((char *)cloudUri, MAX_URI_LENGTH, CLOUD_URI);
    memset((void *)(cloudUri + ret), 117, MAX_URI_LENGTH - ret);

    EXPECT_TRUE(OC_STACK_INVALID_URI == OCCloudAclCancelInvitation(NULL, userId, groupId, memberId,
                cloudUri,
                cloudResponseCB));
    memset((void *)(cloudUri + ret), 0, MAX_URI_LENGTH - ret);
    EXPECT_TRUE(OC_STACK_OK == OCCloudAclCancelInvitation(NULL, userId, groupId, memberId, cloudUri,
                cloudResponseCB));

    OICFree((void *)cloudUri);
    OICFree((void *)memberId);
    OICFree((void *)groupId);
    OICFree((void *)userId);
}

TEST_F(CLOUD, OCCloudAclPolicyCheck)
{
    const char *cloudUri = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudAclPolicyCheck(NULL, NULL, NULL, NULL, NULL, NULL,
                NULL));

    cloudUri = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    size_t ret = snprintf((char *)cloudUri, MAX_URI_LENGTH, CLOUD_URI);
    memset((void *)(cloudUri + ret), 117, MAX_URI_LENGTH - ret);

    EXPECT_TRUE(OC_STACK_INVALID_URI == OCCloudAclPolicyCheck(NULL, X_UUID, X_UUID, "GET", "uuri",
                cloudUri, cloudResponseCB));
    memset((void *)(cloudUri + ret), 0, MAX_URI_LENGTH - ret);
    EXPECT_EQ(OC_STACK_OK, OCCloudAclPolicyCheck(NULL, X_UUID, X_UUID, "GET", "uuri", cloudUri,
              cloudResponseCB));

    OICFree((void *)cloudUri);
}
//---------------------auth--------------------
TEST_F(CLOUD, CloudTokenRefresh)
{
    OicCloud_t *cloud = (OicCloud_t *)OICCalloc(1, sizeof(OicCloud_t));
    cloud->stat = OC_CLOUD_TOKEN_REFRESH0;
    cloud->session = (session_t *)OICCalloc(1, sizeof(session_t));
    cloud->session->expireSin = 3;
    CloudTokenRefresh((void *)cloud);
}
//---------------------auth--------------------
TEST_F(CLOUD, CloudsSignOut)
{
    CloudsSignOut();

    DeleteCloudAccount();
}
//--------------------crl------------------
TEST_F(CLOUD, handleCrlGetResponse)
{
    OCClientResponse *response = NULL;
    char *data = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == handleCrlGetResponse(NULL, NULL, NULL));

    response = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == handleCrlGetResponse(NULL, (void **)&data, response));

    OicSecCrl_t *crl = (OicSecCrl_t *)OICCalloc(1, sizeof(OicSecCrl_t));
    crl->CrlId = 3;
    crl->ThisUpdate.data = certData();
    crl->ThisUpdate.len = certDataLen();
    crl->CrlData.data = keyData();
    crl->CrlData.len = keyDataLen();

    OCSecurityPayload *payload = (OCSecurityPayload *)OICCalloc(1, sizeof(OCSecurityPayload));
    payload->base.type = PAYLOAD_TYPE_SECURITY;
    response->payload = (OCPayload *)payload;

    EXPECT_TRUE(OC_STACK_OK == CrlToCBORPayload(crl, (uint8_t **)&payload->securityData,
                &payload->payloadSize, NULL));
    response->payload->type = PAYLOAD_TYPE_SECURITY;

    EXPECT_TRUE(OC_STACK_OK == handleCrlGetResponse(NULL, (void **)&data, response));

    OCPayloadDestroy((OCPayload *)response->payload);
    OICFree(response);
}

TEST_F(CLOUD, OCCloudGetCRL)
{
    const char *cloudUri = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudGetCRL(NULL, NULL, NULL));

    cloudUri = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    size_t ret = snprintf((char *)cloudUri, MAX_URI_LENGTH, CLOUD_URI);
    memset((void *)(cloudUri + ret), 117, MAX_URI_LENGTH - ret);

    EXPECT_TRUE(OC_STACK_INVALID_URI == OCCloudGetCRL(NULL, cloudUri, cloudResponseCB));
    memset((void *)(cloudUri + ret), 0, MAX_URI_LENGTH - ret);
    EXPECT_EQ(OC_STACK_OK, OCCloudGetCRL(NULL, cloudUri, cloudResponseCB));

    OICFree((void *)cloudUri);
}

TEST_F(CLOUD, OCCloudPostCRL)
{
    const char *thisUpdate = OICStrdup("this");
    const char *nextUpdate = OICStrdup("next");
    OCByteString *crl = (OCByteString *)OICCalloc(1, sizeof(OCByteString ));
    stringArray_t *serialNumbers = (stringArray_t *)OICCalloc(1, sizeof(stringArray_t));
    const char *cloudUri = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudPostCRL(NULL, NULL, NULL, NULL, NULL, NULL, NULL));

    crl->bytes = keyData();
    crl->len = keyDataLen();

    serialNumbers->length = 1;
    serialNumbers->array = (char **)OICCalloc(serialNumbers->length, sizeof(char *));
    serialNumbers->array[serialNumbers->length - 1] = OICStrdup("root");

    cloudUri = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    size_t ret = snprintf((char *)cloudUri, MAX_URI_LENGTH, CLOUD_URI);
    memset((void *)(cloudUri + ret), 117, MAX_URI_LENGTH - ret);

    EXPECT_TRUE(OC_STACK_INVALID_URI == OCCloudPostCRL(NULL, thisUpdate, nextUpdate, crl, serialNumbers,
                cloudUri, cloudResponseCB));
    memset((void *)(cloudUri + ret), 0, MAX_URI_LENGTH - ret);
    EXPECT_TRUE(OC_STACK_OK == OCCloudPostCRL(NULL, thisUpdate, nextUpdate, crl, serialNumbers,
                cloudUri, cloudResponseCB));

    OICFree((void *)cloudUri);
}
//--------------------csr------------------
TEST_F(CLOUD, CSRMakeSubject)
{
    char *subject = (char *)OICCalloc(1, 2048);
    CSRMakeSubject(subject, "UA", "Samsung", "Sec", NULL);
    CSRMakeSubject(subject, "UA", "Samsung", "Sec", X_UUID);
    OICFree(subject);
}

TEST_F(CLOUD, ecdsaGenKeypair)
{
    mbedtls_pk_context pkCtx;
    EXPECT_EQ(0, ecdsaGenKeypair(&pkCtx));
    mbedtls_pk_free(&pkCtx);

    char *subject = NULL;
    OCByteString csr = {.bytes = NULL, .len = 0};

    EXPECT_EQ(-1, GenerateCSR(subject, &csr));
    subject = (char *)X_UUID;
    EXPECT_EQ(0, GenerateCSR(subject, &csr));
}

TEST_F(CLOUD, HandleCertificateIssueRequest)
{
    OCClientResponse *response = NULL;
    char *data = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == HandleCertificateIssueRequest(NULL, NULL, NULL));

    response = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    OCRepPayload *payload = OCRepPayloadCreate();
    response->payload = (OCPayload *)payload;

    EXPECT_EQ(OC_STACK_ERROR, HandleCertificateIssueRequest(NULL, (void **)&data, response));

    response->result = OC_STACK_RESOURCE_CHANGED;

    EXPECT_EQ(OC_STACK_ERROR, HandleCertificateIssueRequest(NULL, (void **)&data, response));

    OCRepPayloadSetPropString(payload, OC_RSRVD_DEVICE_ID, X_UUID);

    EXPECT_EQ(OC_STACK_ERROR, HandleCertificateIssueRequest(NULL, (void **)&data, response));

    OicSecKey_t *key = (OicSecKey_t *)OICCalloc(1, sizeof(OicSecKey_t));
    key->data = keyData();
    key->len = keyDataLen();
    key->encoding = OIC_ENCODING_DER;
    OCRepPayloadSetPropPubDataType(payload, OC_RSRVD_CERT, key);

    EXPECT_EQ(OC_STACK_ERROR, HandleCertificateIssueRequest(NULL, (void **)&data, response));

    OicSecKey_t *caCert = (OicSecKey_t *)OICCalloc(1, sizeof(OicSecKey_t));
    caCert->data = keyData();
    caCert->len = keyDataLen();
    caCert->encoding = OIC_ENCODING_DER;
    OCRepPayloadSetPropPubDataType(payload, OC_RSRVD_CACERT, caCert);

    EXPECT_EQ(OC_STACK_INVALID_PARAM, HandleCertificateIssueRequest(NULL, (void **)&data, response));

    OCPayloadDestroy((OCPayload *)response->payload);
    OICFree(response);
}

TEST_F(CLOUD, OCCloudCertificateIssueRequest)
{
    const char *cloudUri = NULL;

    EXPECT_TRUE(OC_STACK_INVALID_PARAM == OCCloudCertificateIssueRequest(NULL, NULL, NULL));

    cloudUri = (const char *)OICCalloc(1, MAX_URI_LENGTH);
    size_t ret = snprintf((char *)cloudUri, MAX_URI_LENGTH, CLOUD_URI);
    memset((void *)(cloudUri + ret), 117, MAX_URI_LENGTH - ret);

    EXPECT_TRUE(OC_STACK_INVALID_URI == OCCloudCertificateIssueRequest(NULL, cloudUri,
                cloudResponseCB));
    memset((void *)(cloudUri + ret), 0, MAX_URI_LENGTH - ret);
    EXPECT_TRUE(OC_STACK_OK == OCCloudCertificateIssueRequest(NULL, cloudUri, cloudResponseCB));

    OICFree((void *)cloudUri);
}
//--------------------util-----------------
TEST_F(CLOUD, utill)
{
    setCoapPrefix(false);
    setCoapPrefix(true);

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, handleResponse(NULL, NULL, NULL));

    stringArray_t *serialNumbers = (stringArray_t *)OICCalloc(1, sizeof(stringArray_t));

    clearStringArray(serialNumbers);

    serialNumbers->length = 1;
    serialNumbers->array = (char **)OICCalloc(serialNumbers->length, sizeof(char *));
    serialNumbers->array[serialNumbers->length - 1] = OICStrdup("root");
    clearStringArray(serialNumbers);
}
