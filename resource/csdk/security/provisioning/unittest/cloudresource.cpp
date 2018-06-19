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
}

#include "../src/cloud/cloudresource.c"

#ifdef TAG
#undef TAG
#endif

#include "../src/cloud/config.c"

#ifdef TAG
#undef TAG
#endif

#include "../src/cloud/auth.c"

#ifdef TAG
#undef TAG
#endif

#define TAG  "CLOUD_UNITTEST"

#define SVR_DB_FILE_NAME "oic_svr_db_client.dat"
#define PM_DB_FILE_NAME "test.db"
#define STR_LEN 512
static const char *sample =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!:/";

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

static FILE *fopen_prvnMng(const char *path, const char *mode)
{
    if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        return fopen(SVR_DB_FILE_NAME, mode);
    }
    OIC_LOG_V(DEBUG, TAG, "use db: %s", path);
    return fopen(path, mode);
}

TEST(CloudResourceTest, Init)
{
    static OCPersistentStorage gPst;
    OCStackResult result = OC_STACK_ERROR;

#ifdef HAVE_UNISTD_H
    EXPECT_EQ(0, access(SVR_DB_FILE_NAME, F_OK));
#endif // HAVE_UNISTD_H
    gPst.open = fopen_prvnMng;
    gPst.read = fread;
    gPst.write = fwrite;
    gPst.close = fclose;
    gPst.unlink = unlink;

    result = OCRegisterPersistentStorageHandler(&gPst);
    EXPECT_EQ(OC_STACK_OK, result);

    result = OCInit(NULL, 0, OC_CLIENT_SERVER);
    EXPECT_EQ(OC_STACK_OK, result);
}

TEST(CloudResourceTest, InitCloudResource)
{
    EXPECT_EQ(OC_STACK_OK, InitCloudResource());
}

TEST(CloudResourceTest, CloudToCBORPayloadNULL)
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

TEST(CloudResourceTest, CloudToCBORPayloadVALID)
{
    OicCloud_t *cloud = getCloud();

    uint8_t *payload = NULL;
    size_t size = 0;
    EXPECT_EQ(OC_STACK_OK, CloudToCBORPayload(cloud, &payload, &size));
    EXPECT_TRUE(payload != NULL);

    FreeCloud(cloud);
    OCPayloadDestroy((OCPayload *)payload);
}

TEST(CloudResourceTest, CloudToCBORPayloadResourceVALID)
{
    OicCloud_t *cloud = getCloud();

    uint8_t *payload = NULL;
    size_t size = 0;
    EXPECT_EQ(OC_STACK_OK, CloudToCBORPayloadResource(cloud, &payload, &size));
    EXPECT_TRUE(payload != NULL);

    FreeCloud(cloud);
    OCPayloadDestroy((OCPayload *)payload);
}

TEST(CloudResourceTest, CBORPayloadToCloudNULL)
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

TEST(CloudResourceTest, CBORPayloadToCloudFULL)
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

TEST(CloudResourceTest, strCopyFULL)
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

TEST(CloudResourceTest, CBORPayloadToCloudResourceFULL)
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

TEST(CloudResourceTest, ValidCloudFULL)
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

TEST(CloudResourceTest, DeleteCloudListFULL)
{
    DeleteCloudList(NULL, true);
    OicCloud_t *cloud =  getCloud();
    cloud->next =  getCloud();
    cloud->next->next =  getCloud();
    DeleteCloudList(cloud, true);
}

TEST(CloudResourceTest, CreateCloudGetPayloadFULL)
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

TEST(CloudResourceTest, StartCloudFULL)
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

TEST(CloudResourceTest, HandleCloudPostRequestFULL)
{
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

    OICFree(uuid);
    FreeCloud(cloud);
    ASSERT_TRUE(OC_STACK_OK == SetDosState(DOS_RFNOP));
    OCPayloadDestroy(ehRequest->payload);
    OICFree(ehRequest);
    exit(0);
}

TEST(CloudResourceTest, HandleCloudGetRequestFULL)
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

TEST(CloudResourceTest, HandleCloudDeleteRequestFULL)
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
    ASSERT_TRUE(OC_EH_ERROR == HandleCloudDeleteRequest(ehRequest));

    OICFree(uuid);
    ASSERT_TRUE(OC_STACK_OK == SetDosState(DOS_RFNOP));
    OCPayloadDestroy(ehRequest->payload);
    FreeCloud(cloud);
    OICFree(ehRequest);
}

TEST(CloudResourceTest, CloudEntityHandlerFULL)
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

TEST(CloudResourceTest, OCProvisionCloudConfigFULL)
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
    snprintf(pDev->endpoint.addr, sizeof(pDev->endpoint.addr), "127.0.0.1");
    pDev->securePort = 1024;
    pDev->connType = (OCConnectivityType)(CT_ADAPTER_IP | CT_FLAG_SECURE | CT_IP_USE_V4);
    ASSERT_TRUE(OC_STACK_OK == OCProvisionCloudConfig(ctx, pDev, cloud, resultCallback));

    FreeCloud(cloud);
    OICFree(ctx);
    OICFree(pDev);
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

TEST(CloudResourceTest, handleCloudStatusResponseFULL)
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

TEST(CloudResourceTest, OCRemoveCloudConfigCBFULL)
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

TEST(CloudResourceTest, OCGetCloudStatusRequestFULL)
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

TEST(CloudResourceTest, OCRemoveCloudConfigFULL)
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

TEST(CloudResourceTest, GetCloudStatusFULL)
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

TEST(CloudResourceTest, handleCloudTokenRefreshResponseFULL)
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

TEST(CloudResourceTest, OCCloudTokenRefreshFULL)
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

TEST(CloudResourceTest, handleCloudSignUpResponseFULL)
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
TEST(CloudResourceTest, CloudTokenRefreshFULL)
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

TEST(CloudResourceTest, handleCloudSignInResponseFULL)
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

TEST(CloudResourceTest, CloudSignFULL)
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

TEST(CloudResourceTest, CloudSignUpParsePayloadFULL)
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

TEST(CloudResourceTest, handleCloudSignOutResponseFULL)
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

TEST(CloudResourceTest, UpdateCloudPersistentStorageFULL)
{
    DeleteCloudList(gCloud, true);
    ASSERT_TRUE(false == UpdateCloudPersistentStorage());
    OicCloud_t *cloud = getCloud();
    LL_APPEND(gCloud, cloud);
    ASSERT_TRUE(true == UpdateCloudPersistentStorage());
    DeleteCloudList(gCloud, true);
    gCloud = NULL;
}

TEST(CloudResourceTest, DeInitCloudResource)
{
    EXPECT_EQ(OC_STACK_OK, DeInitCloudResource());
}
