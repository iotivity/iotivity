/* *****************************************************************
 *
 * Copyright 2019 Samsung Electronics All Rights Reserved.
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
#include <stdio.h>
#include <stdlib.h>
#include <gtest/gtest.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "tools.h"
#include "ocprovisioningmanager.h"
#include "../src/ownershiptransfermanager.c"
#undef UUID_LENGTH
#include "../../../connectivity/src/adapter_util/ca_adapter_net_ssl.c"
#undef TAG
#ifdef __cplusplus
}
#endif

#undef TAG
#define TAG "OWSHT_UNITTEST"

#define SVR_DB_FILE_NAME TAG".dat"
#define PM_DB_FILE_NAME TAG".db"

using namespace std;

class OWSHT : public ::testing::Test
{
    public:
        static void SetUpTestCase()
        {
            IOT_Init(PM_DB_FILE_NAME);
        }

        static void TearDownTestCase()
        {
            IOT_DeInit(PM_DB_FILE_NAME);
        }
};


TEST_F(OWSHT, PostOwnershipInformation)
{
    OTMContext_t *ctx = createOTMContext();
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PostOwnershipInformation(NULL));
    EXPECT_EQ(OC_STACK_OK, PostOwnershipInformation(ctx));
    freeOTMContext(ctx);
}

TEST_F(OWSHT, OTMSetOTCallback)
{
    OTMCallbackData_t *callbacks = (OTMCallbackData_t *)OICCalloc(1, sizeof(OTMCallbackData_t));
    EXPECT_EQ(OC_STACK_OK, OTMSetOTCallback(OIC_JUST_WORKS, callbacks));
    EXPECT_EQ(OC_STACK_OK, OTMSetOTCallback(OIC_RANDOM_DEVICE_PIN, callbacks));
    EXPECT_EQ(OC_STACK_OK, OTMSetOTCallback(OIC_RANDOM_DEVICE_PIN, callbacks));
    EXPECT_EQ(OC_STACK_OK, OTMSetOTCallback(OIC_MANUFACTURER_CERTIFICATE, callbacks));
    EXPECT_EQ(OC_STACK_INVALID_METHOD, OTMSetOTCallback(OIC_DECENTRALIZED_PUBLIC_KEY, callbacks));
    EXPECT_EQ(OC_STACK_OK, OTMSetOTCallback(OIC_MV_JUST_WORKS, callbacks));
    EXPECT_EQ(OC_STACK_OK, OTMSetOTCallback(OIC_CON_MFG_CERT, callbacks));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OTMSetOTCallback(0xFFCC, callbacks));
    OICFree(callbacks);
}

TEST_F(OWSHT, OTMSelectOwnershipTransferMethod)
{
    OwnerType_t ownerType = SUB_OWNER;
    EXPECT_EQ(OC_STACK_ERROR, OTMSelectOwnershipTransferMethod(NULL, 0, NULL, ownerType));
    size_t numberOfMethods = 3;
    OicSecOxm_t *supportedMethods = (OicSecOxm_t *)OICCalloc(numberOfMethods, sizeof(OicSecOxm_t));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OTMSelectOwnershipTransferMethod(supportedMethods,
              numberOfMethods, NULL, ownerType));
    OICFree(supportedMethods);
}

TEST_F(OWSHT, GetOxmAllowTableIdx)
{
    EXPECT_EQ(OXM_IDX_DECENTRALIZED_PUBLIC_KEY, GetOxmAllowTableIdx(OIC_DECENTRALIZED_PUBLIC_KEY));
    EXPECT_TRUE(OXM_IDX_UNKNOWN == GetOxmAllowTableIdx((OicSecOxm_t)-1));
}

TEST_F(OWSHT, getSecurePort)
{
    EXPECT_EQ(0, getSecurePort(NULL));
}

TEST_F(OWSHT, SetResult)
{
    OTMContext_t *ctx = createOTMContext();
    ctx->ctxResultArray = (OCProvisionResult_t *)OICCalloc(3, sizeof(OCProvisionResult_t));
    SetResult(ctx, OC_STACK_OK);
    ctx = createOTMContext();
    ctx->selectedDeviceInfo->doxm->oxmSel = OIC_RANDOM_DEVICE_PIN;
    SetResult(ctx, OC_STACK_OK);
    ctx = createOTMContext();
    ctx->selectedDeviceInfo->doxm->oxmSel = OIC_MANUFACTURER_CERTIFICATE;
    ctx->ocDoHandle = (OCDoHandle)OICMalloc(1);
    SetResult(ctx, OC_STACK_OK);
    ctx = createOTMContext();
    ctx->ctxHasError = true;
    SetResult(ctx, OC_STACK_OK);
}

TEST_F(OWSHT, OwnershipTransferSessionEstablished)
{
    OicSecDoxm_t *newDevDoxm = createDoxm();
    OTMContext_t *otmCtx = createOTMContext();
    CAEndpoint_t *endpoint = createEndpoint();
    newDevDoxm->oxmSel = OIC_MV_JUST_WORKS;
    EXPECT_EQ(CA_HANDLE_ERROR_OTHER_MODULE, OwnershipTransferSessionEstablished(endpoint, newDevDoxm,
              otmCtx));
    newDevDoxm->oxmSel = OIC_CON_MFG_CERT;
    EXPECT_EQ(CA_HANDLE_ERROR_OTHER_MODULE, OwnershipTransferSessionEstablished(endpoint, newDevDoxm,
              otmCtx));
    freeEndpoint(endpoint);
    freeDoxm(newDevDoxm);
    freeOTMContext(otmCtx);
}

TEST_F(OWSHT, OwnerUuidUpdateHandler)
{
    OTMContext_t *otmCtx = createOTMContext();
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_RESOURCE_CHANGED;
    otmCtx->selectedDeviceInfo->doxm->oxmSel = OIC_MV_JUST_WORKS;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, OwnerUuidUpdateHandler((void *)otmCtx, NULL,
              clientResponse));
    otmCtx = createOTMContext();
    clientResponse->result = OC_STACK_OK;
    otmCtx->selectedDeviceInfo->doxm->oxmSel = OIC_MV_JUST_WORKS;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, OwnerUuidUpdateHandler((void *)otmCtx, NULL,
              clientResponse));
    OICFree(clientResponse);
}

TEST_F(OWSHT, OwnershipTransferSessionFailed)
{
    OicSecDoxm_t *newDevDoxm = createDoxm();
    OTMContext_t *otmCtx = createOTMContext();
    CAEndpoint_t *endpoint = createEndpoint();
    CAErrorInfo_t *info = (CAErrorInfo_t *)OICCalloc(1, sizeof(CAErrorInfo_t));
    info->result = CA_DTLS_AUTHENTICATION_FAILURE;
    newDevDoxm->oxmSel = OIC_RANDOM_DEVICE_PIN;
    EXPECT_EQ(CA_HANDLE_ERROR_OTHER_MODULE, OwnershipTransferSessionFailed(endpoint, info, newDevDoxm,
              otmCtx, false));
    EXPECT_EQ(CA_HANDLE_ERROR_OTHER_MODULE, OwnershipTransferSessionFailed(endpoint, info, newDevDoxm,
              otmCtx, true));

    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&otmCtx->selectedDeviceInfo->doxm->deviceID));
    EXPECT_EQ(CA_CONTINUE_OPERATION, OwnershipTransferSessionFailed(endpoint, info, newDevDoxm, otmCtx,
              true));
    OICFree(info);
    freeEndpoint(endpoint);
    freeDoxm(newDevDoxm);
    freeOTMContext(otmCtx);
}

TEST_F(OWSHT, OperationModeUpdateHandler)
{
    OTMContext_t *otmCtx = createOTMContext();
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_OK;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, OperationModeUpdateHandler((void *)otmCtx, NULL,
              clientResponse));
    OICFree(clientResponse);
}

TEST_F(OWSHT, OwnerCredentialHandler)
{
    OTMContext_t *otmCtx = createOTMContext();
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_RESOURCE_CHANGED;
    otmCtx->selectedDeviceInfo->doxm->oxmSel = OIC_RANDOM_DEVICE_PIN;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, OwnerCredentialHandler((void *)otmCtx, NULL,
              clientResponse));
    freeProvisionDev(otmCtx->selectedDeviceInfo);
    otmCtx->selectedDeviceInfo = NULL;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, OwnerCredentialHandler((void *)otmCtx, NULL,
              clientResponse));
    OICFree(clientResponse);
    freeOTMContext(otmCtx);
}

TEST_F(OWSHT, OwnershipInformationHandler)
{
    OTMContext_t *otmCtx = createOTMContext();
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_OK;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, OwnershipInformationHandler((void *)otmCtx, NULL,
              clientResponse));
    OICFree(clientResponse);
}

TEST_F(OWSHT, ProvisioningStatusHandler)
{
    OTMContext_t *otmCtx = createOTMContext();
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_OK;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisioningStatusHandler((void *)otmCtx, NULL,
              clientResponse));
    OICFree(clientResponse);
}

TEST_F(OWSHT, ReadyForNomalStatusHandler)
{
    OTMContext_t *otmCtx = createOTMContext();
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_OK;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ReadyForNomalStatusHandler((void *)otmCtx, NULL,
              clientResponse));
    OICFree(clientResponse);
}

TEST_F(OWSHT, RownerUuidHandler)
{
    OTMContext_t *otmCtx = createOTMContext();
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_OK;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, RownerUuidHandler((void *)otmCtx, NULL, clientResponse));
    OICFree(clientResponse);
    freeOTMContext(otmCtx);
}

TEST_F(OWSHT, PostRownerUuid)
{
    OTMContext_t *otmCtx = createOTMContext();
    otmCtx->selectedDeviceInfo->pstat->dos.state = DOS_RESET;
    EXPECT_EQ(OC_STACK_OK, PostRownerUuid(otmCtx));
    freeOTMContext(otmCtx);
}

TEST_F(OWSHT, CloseSslConnection)
{
    OCProvisionDev_t *selectedDeviceInfo = createProvisionDev();
    EXPECT_TRUE(CloseSslConnection(selectedDeviceInfo));
}

TEST_F(OWSHT, DTLSHandshakeCB)
{
    EXPECT_EQ(CA_STATUS_OK, DTLSHandshakeCB(NULL, NULL));

    OTMContext_t *otmCtx = createOTMContext();
    CAErrorInfo_t *info = (CAErrorInfo_t *)OICCalloc(1, sizeof(CAErrorInfo_t));
    EXPECT_EQ(OC_STACK_OK, AddOTMContext(otmCtx, otmCtx->selectedDeviceInfo->endpoint.addr,
                                         otmCtx->selectedDeviceInfo->endpoint.port));
    OicSecDoxm_t *doxm = otmCtx->selectedDeviceInfo->doxm;
    otmCtx->selectedDeviceInfo->doxm = NULL;
    EXPECT_EQ(CA_STATUS_OK, DTLSHandshakeCB((const CAEndpoint_t *)&otmCtx->selectedDeviceInfo->endpoint,
                                            info));
    otmCtx->selectedDeviceInfo->doxm = doxm;
    CAEndpoint_t *endpoint = createEndpoint();
    endpoint->port = otmCtx->selectedDeviceInfo->endpoint.port;
    otmCtx->selectedDeviceInfo->securePort = endpoint->port + 1;
    EXPECT_EQ(CA_STATUS_OK, DTLSHandshakeCB(endpoint, info));
    OICFree(info);
    freeOTMContext(otmCtx);
}

TEST_F(OWSHT, SaveOwnerPSK)
{
    OCProvisionDev_t *selectedDeviceInfo = createProvisionDev();
    CAEndpoint_t endpoint;
    CopyDevAddrToEndpoint(&selectedDeviceInfo->endpoint, &endpoint);
    endpoint.port = getSecurePort(selectedDeviceInfo);

    OicUuid_t ownerDeviceID = OC_ZERO_UUID;
    EXPECT_EQ(OC_STACK_OK, GetDoxmDeviceID(&ownerDeviceID));

    OicSecKey_t ownerKey;
    memset(&ownerKey, 0, sizeof(ownerKey));

    uint8_t ownerPSK[OWNER_PSK_LENGTH_128] = { 0 };
    ownerKey.data = ownerPSK;
    ownerKey.len = OWNER_PSK_LENGTH_128;
    ownerKey.encoding = OIC_ENCODING_RAW;

    g_caSslContext->selectedCipher = MBEDTLS_TLS_RSA_WITH_AES_128_GCM_SHA256;
    OicSecCred_t *cred = GenerateCredential(&selectedDeviceInfo->doxm->deviceID,
                                            SYMMETRIC_PAIR_WISE_KEY, NULL,
                                            &ownerKey, NULL);

    EXPECT_NE(nullptr, cred);
    EXPECT_EQ(OC_STACK_OK, AddCredential(cred));
    EXPECT_EQ(OC_STACK_OK, SaveOwnerPSK(selectedDeviceInfo));

    EXPECT_EQ(OC_STACK_OK, DeInitDoxmResource());
    EXPECT_EQ(OC_STACK_ERROR, SaveOwnerPSK(selectedDeviceInfo));
    EXPECT_EQ(OC_STACK_OK, InitDoxmResource());

    freeProvisionDev(selectedDeviceInfo );
}

TEST_F(OWSHT, OwnerTransferModeHandler)
{
    OTMContext_t *otmCtx = createOTMContext();
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_OK;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, OwnerTransferModeHandler((void *)otmCtx, NULL,
              clientResponse));
    clientResponse->result = OC_STACK_RESOURCE_CHANGED;
    OTMContext_t *otmCtx1 = createOTMContext();
    otmCtx1->selectedDeviceInfo->endpoint.addr[0] = 0;
    otmCtx1->selectedDeviceInfo->securePort = 0;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, OwnerTransferModeHandler((void *)otmCtx1, NULL,
              clientResponse));
    OICFree(clientResponse);
}

OCStackResult loadSecret(OTMContext_t *otmCtx)
{
    OC_UNUSED(otmCtx);
    return OC_STACK_ERROR;
}

TEST_F(OWSHT, ListMethodsHandler)
{
    OTMContext_t *otmCtx = createOTMContext();
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_CONTINUE;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ListMethodsHandler((void *)otmCtx, NULL, clientResponse));
    OTMContext_t *otmCtx1 = createOTMContext();
    clientResponse->result = OC_STACK_OK;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ListMethodsHandler((void *)otmCtx1, NULL, clientResponse));
    OTMContext_t *otmCtx2 = createOTMContext();
    OCSecurityPayload *payload = (OCSecurityPayload *)OICCalloc(1, sizeof(OCSecurityPayload));
    payload->payloadSize = 0;
    clientResponse->payload = (OCPayload *)payload;
    clientResponse->payload->type = PAYLOAD_TYPE_REPRESENTATION;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ListMethodsHandler((void *)otmCtx2, NULL, clientResponse));
    OTMContext_t *otmCtx3 = createOTMContext();
    clientResponse->payload->type = PAYLOAD_TYPE_SECURITY;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ListMethodsHandler((void *)otmCtx3, NULL, clientResponse));
    OTMContext_t *otmCtx4 = createOTMContext();
    otmCtx4->otmCallback.loadSecretCB = loadSecret;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ListMethodsHandler((void *)otmCtx4, NULL, clientResponse));
    OTMContext_t *otmCtx5 = createOTMContext();
    OicSecPstat_t *pstat = createPstat();
    EXPECT_EQ(OC_STACK_OK, PstatToCBORPayload(pstat, &payload->securityData, &payload->payloadSize));
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ListMethodsHandler((void *)otmCtx5, NULL, clientResponse));
    OICFree(clientResponse);
}

TEST_F(OWSHT, SetupPDM)
{
    OCProvisionDev_t *selectedDevice = createProvisionDev();
    EXPECT_EQ(OC_STACK_OK, OCClosePM());
    EXPECT_EQ(OC_STACK_PDM_IS_NOT_INITIALIZED, SetupPDM(selectedDevice));
    EXPECT_EQ(OC_STACK_OK, OCInitPM(PM_DB_FILE_NAME));
    selectedDevice->doxm->owned = false;
    PDMAddDevice(&selectedDevice->doxm->deviceID);
    PDMSetDeviceState(&selectedDevice->doxm->deviceID, PDM_DEVICE_ACTIVE);
    EXPECT_EQ(OC_STACK_OK, SetupPDM(selectedDevice));
}

TEST_F(OWSHT, ConfigSelfOwnership)
{
    EXPECT_EQ(OC_STACK_OK, ConfigSelfOwnership());
}
