/* *****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
#include <gtest/gtest.h>

#ifdef __cplusplus
extern "C" {
#endif
//#include "../../../connectivity/src/cablockwisetransfer.c"
#include "../src/secureresourceprovider.c"
#undef TAG
#include "../../src/csrresource.c"
#undef TAG
#include "../../src/credresource.c"
#undef TAG
#include "tools.h"
#undef TAG

#ifdef __cplusplus
}
#endif

#define TAG "SRP"

#define SVR_DB_FILE_NAME "oic_svr_db_client.dat"
#define PM_DB_FILE_NAME TAG".db"

static OCProvisionDev_t *pDev1 = NULL;
static OCProvisionDev_t *pDev2 = NULL;
static unsigned short timeout = 0;

class SRP : public ::testing::Test
{
    public:
        static void SetUpTestCase()
        {
            IOT_Init(PM_DB_FILE_NAME);

            pDev1 = createProvisionDev();
            pDev2 = createProvisionDev();
            pDev2->endpoint.port = 9998;
            ConvertStrToUuid("33333355-3333-3333-3333-111111111111", &pDev2->doxm->deviceID);

            timeout = 60;
        }

        static void TearDownTestCase()
        {
            ClientCB *out = NULL;
            ClientCB *tmp = NULL;
            LL_FOREACH_SAFE(g_cbList, out, tmp)
            {
                LL_DELETE(g_cbList, out);
            }

            OicSecCred_t *cred = NULL;
            OicSecCred_t *r = NULL;
            LL_FOREACH_SAFE(gCred, cred, r)
            {
                LL_DELETE(gCred, cred);
            }
            /*
                        size_t len = u_arraylist_length(g_context.dataList);
                        for (size_t i = len; i > 0; i--)
                        {
                            CABlockData_t *removedData = (CABlockData_t *)u_arraylist_remove(g_context.dataList, i - 1);
                        }*/
            IOT_DeInit(PM_DB_FILE_NAME);
        }

        static const ByteArray g_caPublicKey;

        static const ByteArray g_derCode ;

        static ByteArray g_serNum;
        //static OCProvisionDev_t *gList = NULL;
        OicSecAcl_t acl;
};

static void provisionResultCB(void *ctx, size_t nOfRes, OCProvisionResult_t *arr, bool hasError)
{
    OC_UNUSED(ctx);
    OC_UNUSED(nOfRes);
    OC_UNUSED(arr);
    OC_UNUSED(hasError);
    OIC_LOG_V(DEBUG, TAG, "%s: %d", __func__, hasError);
}

static CredentialData_t *createCredentialData()
{
    CredentialData_t *credData = (CredentialData_t *)OICCalloc(1, sizeof(CredentialData_t));
    memset(credData, 0, sizeof(CredentialData_t));
    credData->resultCallback = provisionResultCB;
    credData->numOfResults = 0;
    credData->resArr = (OCProvisionResult_t *)OICCalloc(2, sizeof(OCProvisionResult_t));
    credData->deviceInfo[0] = createProvisionDev();
    credData->deviceInfo[1] = createProvisionDev();

    return credData;
}

static void freeCredentialData(CredentialData_t *credData)
{
    freeProvisionDev((OCProvisionDev_t *)credData->deviceInfo[0]);
    freeProvisionDev((OCProvisionDev_t *)credData->deviceInfo[1]);
    OICFree(credData->resArr);
    OICFree(credData);
}

static void provisioningCB (void *UNUSED1, size_t UNUSED2, OCProvisionResult_t *UNUSED3,
                            bool UNUSED4)
{
    //dummy callback
    (void) UNUSED1;
    (void) UNUSED2;
    (void) UNUSED3;
    (void) UNUSED4;
}

TEST_F(SRP, SRPProvisionACLNullDeviceInfo)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPProvisionACL(NULL, NULL, &acl, OIC_SEC_ACL_V2,
              &provisioningCB));
}

TEST_F(SRP, SRPProvisionACLNullCallback)
{
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, SRPProvisionACL(NULL, pDev1, &acl, OIC_SEC_ACL_V2, NULL));
}

TEST_F(SRP, SRPProvisionACLNullACL)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPProvisionACL(NULL, pDev1, NULL, OIC_SEC_ACL_V2,
              &provisioningCB));
}

TEST_F(SRP, SRPProvisionACLInvalidVersion)
{
    EXPECT_EQ(OC_STACK_OK, SRPProvisionACL(NULL, pDev1, &acl, OIC_SEC_ACL_UNKNOWN,
                                           &provisioningCB));
}

TEST_F(SRP, SRPProvisionCredentialsNullDevice1)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPProvisionCredentials(NULL, SYMMETRIC_PAIR_WISE_KEY,
              OWNER_PSK_LENGTH_128, NULL,
              pDev2, NULL, NULL, NULL, &provisioningCB));
}

TEST_F(SRP, SRPProvisionCredentialsSamelDeviceId)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPProvisionCredentials(NULL, SYMMETRIC_PAIR_WISE_KEY,
              OWNER_PSK_LENGTH_128, pDev1,
              pDev1, NULL, NULL, NULL, &provisioningCB));
}

TEST_F(SRP, SRPProvisionCredentialsNullCallback)
{
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, SRPProvisionCredentials(NULL, SYMMETRIC_PAIR_WISE_KEY,
              OWNER_PSK_LENGTH_128,
              pDev1, pDev2, NULL, NULL, NULL, NULL));
}

TEST_F(SRP, SRPProvisionCredentialsInvalidKeySize)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPProvisionCredentials(NULL, SYMMETRIC_PAIR_WISE_KEY,
              0, pDev1, pDev2, NULL, NULL, NULL,
              &provisioningCB));
}

TEST_F(SRP, SRPProvisionCredentialsFull)
{
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&pDev1->doxm->deviceID));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&pDev1->doxm->deviceID, PDM_DEVICE_ACTIVE));
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&pDev2->doxm->deviceID));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&pDev2->doxm->deviceID, PDM_DEVICE_ACTIVE));
//    EXPECT_EQ(OC_STACK_OK, PDMLinkDevices(&pDev1->doxm->deviceID, &pDev2->doxm->deviceID));
    EXPECT_EQ(OC_STACK_OK, SRPProvisionCredentials(NULL, SYMMETRIC_PAIR_WISE_KEY,
              OWNER_PSK_LENGTH_128, pDev1, pDev2, getPemKey(), NULL, NULL,
              &provisioningCB));
    EXPECT_EQ(OC_STACK_OK, SRPProvisionCredentials(NULL, SIGNED_ASYMMETRIC_KEY,
              OWNER_PSK_LENGTH_128, pDev1, pDev2, getPemKey(), NULL, NULL,
              &provisioningCB));
}


TEST_F(SRP, SRPUnlinkDevicesTestNullDevice1)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPUnlinkDevices(NULL, NULL, pDev2, provisioningCB));
}

TEST_F(SRP, SRPUnlinkDevicesTestSamelDeviceId)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPUnlinkDevices(NULL, pDev1, pDev1, provisioningCB));
}

TEST_F(SRP, SRPUnlinkDevicesTestNullCallback)
{
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, SRPUnlinkDevices(NULL, pDev1, pDev2, NULL));
}

TEST_F(SRP, SRPRemoveDeviceTestNullTargetDevice)
{
    unsigned short waitTime = 10 ;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPRemoveDevice(NULL, waitTime, NULL, provisioningCB));
}

TEST_F(SRP, SRPRemoveDeviceTestNullResultCallback)
{
    unsigned short waitTime = 10;
    OCProvisionDev_t dev1;
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, SRPRemoveDevice(NULL, waitTime, &dev1, NULL));
}

TEST_F(SRP, SRPRemoveDeviceTestZeroWaitTime)
{
    OCProvisionDev_t dev1;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPRemoveDevice(NULL, 0, &dev1, NULL));
}
/*
TEST_F(SRP, SRPRemoveDeviceFull)
{
    PDMSetDeviceState(&pDev1->doxm->deviceID, PDM_DEVICE_ACTIVE);
    PDMSetDeviceState(&pDev2->doxm->deviceID, PDM_DEVICE_ACTIVE);
    PDMLinkDevices(&pDev1->doxm->deviceID, &pDev2->doxm->deviceID);
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPRemoveDevice(NULL, 4, pDev1, provisioningCB));
}
*/
void trustCertChainChangeCB(void *ctx, uint16_t credId, uint8_t *trustCertChain, size_t chainSize)
{
    OC_UNUSED(ctx);
    OC_UNUSED(credId);
    OC_UNUSED(trustCertChain);
    OC_UNUSED(chainSize);
    OIC_LOG_V(DEBUG, TAG, "%s", __func__);
}

TEST_F(SRP, SRPSaveTrustCertChainDER)
{
    uint16_t credId;

    SRPRegisterTrustCertChainNotifier(NULL, trustCertChainChangeCB);
    EXPECT_EQ(OC_STACK_OK, SRPSaveTrustCertChain(certData(), certDataLen(), OIC_ENCODING_DER, &credId));
}

TEST_F(SRP, SRPSaveTrustCertChainPEM)
{
    uint16_t credId;

    EXPECT_EQ(OC_STACK_OK, SRPSaveTrustCertChain((const uint8_t *)getPemCert(), certDataLen(),
              OIC_ENCODING_PEM, &credId));
}

TEST_F(SRP, SRPSaveTrustCertChainNullCertData)
{
    uint16_t credId;

    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPSaveTrustCertChain(NULL, certDataLen(), OIC_ENCODING_DER,
              &credId));
}

TEST_F(SRP, SRPSaveTrustCertChainNullCredId)
{
    int result;

    result = SRPSaveTrustCertChain(certData(), certDataLen(), OIC_ENCODING_DER, NULL);

    EXPECT_EQ(OC_STACK_INVALID_PARAM, result);
    RemoveAllCredentials();
}

TEST_F(SRP, SRPSaveTrustCertChainDoxm)
{
    uint16_t credId;

    DeInitDoxmResource();
    EXPECT_EQ(OC_STACK_ERROR, SRPSaveTrustCertChain(certData(), certDataLen(), OIC_ENCODING_DER,
              &credId));
    InitDoxmResource();
}

TEST_F(SRP, SRPSaveTrustCertChainPstat)
{
    uint16_t credId;

    DeInitPstatResource();
    EXPECT_EQ(OC_STACK_OK, SRPSaveTrustCertChain(certData(), certDataLen(), OIC_ENCODING_DER, &credId));
    InitPstatResource();
}

TEST_F(SRP, SRPSaveTrustCertChainUnknowEnc)
{
    uint16_t credId;

    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPSaveTrustCertChain(certData(), certDataLen(),
              OIC_ENCODING_UNKNOW, &credId));
}


TEST_F(SRP, SRPSaveOwnCertChainTest)
{
    uint16_t credId;
    OicSecKey_t cert;
    OicSecKey_t key;

    cert.data = certData();
    cert.len = certDataLen();
    cert.encoding = OIC_ENCODING_DER;

    key.data = keyData();
    key.len = keyDataLen();
    key.encoding = OIC_ENCODING_DER;

    EXPECT_EQ(OC_STACK_OK, SRPSaveOwnCertChain(&cert, &key, &credId));
    RemoveAllCredentials();
}

TEST_F(SRP, SRPSaveOwnCertChainTestNullCert)
{
    int result;
    uint16_t credId;
    OicSecKey_t key;

    key.data = keyData();
    key.len = keyDataLen();

    result = SRPSaveOwnCertChain(NULL, &key, &credId);

    EXPECT_EQ(OC_STACK_INVALID_PARAM, result);
    RemoveAllCredentials();
}

TEST_F(SRP, SRPSaveOwnCertChainTestNullCertData)
{
    int result;
    uint16_t credId;
    OicSecKey_t cert;
    OicSecKey_t key;

    cert.data = NULL;
    cert.len = certDataLen();
    key.data = keyData();
    key.len = keyDataLen();

    result = SRPSaveOwnCertChain(&cert, &key, &credId);

    EXPECT_EQ(OC_STACK_INVALID_PARAM, result);
    RemoveAllCredentials();
}

TEST_F(SRP, SRPSaveOwnCertChainTestNullKey)
{
    int result;
    uint16_t credId;
    OicSecKey_t cert;

    cert.data = certData();
    cert.len = certDataLen();

    result = SRPSaveOwnCertChain(&cert, NULL, &credId);

    EXPECT_EQ(OC_STACK_INVALID_PARAM, result);
    RemoveAllCredentials();
}

TEST_F(SRP, SRPSaveOwnCertChainTestNullKeyData)
{
    int result;
    uint16_t credId;
    OicSecKey_t cert;
    OicSecKey_t key;

    cert.data = certData();
    cert.len = certDataLen();
    key.data = NULL;
    key.len = keyDataLen();

    result = SRPSaveOwnCertChain(&cert, &key, &credId);

    EXPECT_EQ(OC_STACK_INVALID_PARAM, result);
    RemoveAllCredentials();
}


OicSecKey_t *getCert()
{
    OicSecKey_t *cert = (OicSecKey_t *)OICCalloc(1, sizeof(OicSecKey_t));
    cert->data = certData();
    cert->len = certDataLen();
    return cert;
}

OicSecKey_t *getKey()
{
    OicSecKey_t *key = (OicSecKey_t *)OICCalloc(1, sizeof(OicSecKey_t));
    key->data = keyData();
    key->len = keyDataLen();
    return key;
}

TEST_F(SRP, SRPSaveOwnCertChainTestDoxm)
{
    uint16_t credId;
    OicSecKey_t *cert = getCert();
    OicSecKey_t *key = getKey();

    DeInitDoxmResource();
    EXPECT_EQ(OC_STACK_ERROR, saveCertChain(cert, key, &credId, "core.der"));
    InitDoxmResource();
    RemoveAllCredentials();
    OICFree(cert);
    OICFree(key);
}


TEST_F(SRP, SRPSaveOwnCertChainTestNullCredId)
{
    int result;
    OicSecKey_t cert;
    OicSecKey_t key;

    cert.data = certData();
    cert.len = certDataLen();
    key.data = keyData();
    key.len = keyDataLen();

    result = SRPSaveOwnCertChain(&cert, &key, NULL);

    EXPECT_EQ(OC_STACK_INVALID_PARAM, result);
    RemoveAllCredentials();
}


TEST_F(SRP, SRPProvisionTrustCertChainNullSelectedDeviceInfo)
{
    int result;
    int ctx;
    OicSecCredType_t type = SIGNED_ASYMMETRIC_KEY;
    uint16_t credId = 0;

    result = SRPProvisionTrustCertChain(&ctx, type, credId, NULL, provisioningCB);
    EXPECT_EQ(OC_STACK_INVALID_PARAM, result);
    RemoveAllCredentials();
}

TEST_F(SRP, SRPProvisionTrustCertChainNullResultCallback)
{
    int result;
    int ctx;
    OicSecCredType_t type = SIGNED_ASYMMETRIC_KEY;
    uint16_t credId = 0;
    OCProvisionDev_t deviceInfo;

    result = SRPProvisionTrustCertChain(&ctx, type, credId, &deviceInfo, NULL);
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, result);
    RemoveAllCredentials();
}

TEST_F(SRP, SRPProvisionTrustCertChainInvalidOicSecCredType)
{
    int result;
    int ctx;
    OicSecCredType_t type = PIN_PASSWORD;
    uint16_t credId = 0;
    OCProvisionDev_t deviceInfo;

    result = SRPProvisionTrustCertChain(&ctx, type, credId, &deviceInfo, provisioningCB);
    EXPECT_EQ(OC_STACK_INVALID_PARAM, result);
    RemoveAllCredentials();
}


static void provisioningCB1 (void *ctx, size_t nOfRes, OCProvisionResult_t *arr, bool hasError)
{
    OC_UNUSED(ctx);
    OC_UNUSED(nOfRes);
    OC_UNUSED(arr);
    OC_UNUSED(hasError);
    OIC_LOG_V(DEBUG, TAG, "%s has error: %d", __func__, hasError);
    RemoveAllCredentials();
}

TEST_F(SRP, SRPProvisionTrustCertChainNoResource)
{
    int result;
    int ctx;
    OicSecCredType_t type = SIGNED_ASYMMETRIC_KEY;
    uint16_t credId = 0;
    OCProvisionDev_t deviceInfo;

    memset(&deviceInfo, 0, sizeof(OCProvisionDev_t));

    result = SRPProvisionTrustCertChain(&ctx, type, credId, &deviceInfo, provisioningCB1);
    EXPECT_EQ(OC_STACK_ERROR, result);
}

TEST_F(SRP, SRPGetACLResourceNoCallback)
{
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, SRPGetACLResource(NULL, pDev1, OIC_SEC_ACL_V2, NULL));
}

TEST_F(SRP, SRPGetACLResourceNoDevice)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPGetACLResource(NULL, NULL, OIC_SEC_ACL_V2, provisioningCB));
}

TEST_F(SRP, SRPGetCredResourceNoCallback)
{
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, SRPGetCredResource(NULL, pDev1, NULL));
}

TEST_F(SRP, SRPGetCredResourceNoDevice)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPGetCredResource(NULL, NULL, provisioningCB));
}

TEST_F(SRP, SRPResetDeviceNoCallback)
{
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, SRPResetDevice(pDev1, NULL));
}

TEST_F(SRP, SRPResetDeviceNoDevice)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPResetDevice(NULL, provisioningCB));
}

TEST_F(SRP, SRPSyncDeviceNoCallback)
{
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, SRPSyncDevice(NULL, timeout, pDev1, NULL));
}

TEST_F(SRP, SRPSyncDeviceNoDevice)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPSyncDevice(NULL, timeout, NULL, provisioningCB));
}

TEST_F(SRP, SRPSyncDeviceZeroWaitTime)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPSyncDevice(NULL, 0, pDev1, provisioningCB));
}

TEST_F(SRP, SRPProvisionTrustCertChainFull)
{
    OCProvisionDev_t *pDev = createProvisionDev();
    /*
        OicUuid_t *uuid = createUuid();
        EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&pDev->doxm->deviceID));
        EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&pDev->doxm->deviceID, PDM_DEVICE_ACTIVE));
        EXPECT_EQ(OC_STACK_OK, PDMAddDevice(uuid));
        EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(uuid, PDM_DEVICE_ACTIVE));
        EXPECT_EQ(OC_STACK_OK, PDMLinkDevices(&pDev->doxm->deviceID, uuid));
    */
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPSyncDevice(NULL, 0, pDev, provisioningCB));
}

TEST_F(SRP, SRPRemoveDeviceWithoutDiscoveryNoCallback)
{
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK,
              SRPRemoveDeviceWithoutDiscovery(NULL, pDev1, pDev2, NULL));
}

TEST_F(SRP, SRPRemoveDeviceWithoutDiscoveryNoDevice)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM,
              SRPRemoveDeviceWithoutDiscovery(NULL, pDev1, NULL, provisioningCB));
}

TEST_F(SRP, SRPRemoveDeviceWithoutDiscoveryNoDeviceList)
{
    EXPECT_EQ(OC_STACK_CONTINUE,
              SRPRemoveDeviceWithoutDiscovery(NULL, NULL, pDev2, provisioningCB));
}

TEST_F(SRP, SRPRemoveDeviceWithoutDiscoveryFull)
{
    PDMSetDeviceState(&pDev1->doxm->deviceID, PDM_DEVICE_ACTIVE);
    PDMSetDeviceState(&pDev2->doxm->deviceID, PDM_DEVICE_ACTIVE);
    PDMLinkDevices(&pDev1->doxm->deviceID, &pDev2->doxm->deviceID);
    OCProvisionDev_t *pOwnedDevList = NULL;
    LL_PREPEND(pOwnedDevList, pDev1);
    LL_PREPEND(pOwnedDevList, pDev2);
    EXPECT_EQ(OC_STACK_OK, SRPRemoveDeviceWithoutDiscovery(NULL, pDev1, pOwnedDevList, provisioningCB));
}


TEST_F(SRP, registerResultForCredProvisioningFull)
{
    OCStackResult stackresult = OC_STACK_OK;
    CredProvisioningResultCause_t cause = DEVICE_LOCAL_FINISHED;
    CredentialData_t *credData = (CredentialData_t *)OICCalloc(1, sizeof(CredentialData_t));
    credData->resArr = (OCProvisionResult_t *)OICCalloc(1, sizeof(OCProvisionResult_t));
    registerResultForCredProvisioning(credData, stackresult, cause);
    OICFree(credData->resArr);
    OICFree(credData);
    RemoveAllCredentials();
}

TEST_F(SRP, provisionCredentialCB2Full)
{
    CredentialData_t *credData = createCredentialData();
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_RESOURCE_CHANGED;

    PDMSetDeviceState(&credData->deviceInfo[0]->doxm->deviceID, PDM_DEVICE_ACTIVE);
    PDMSetDeviceState(&credData->deviceInfo[1]->doxm->deviceID, PDM_DEVICE_ACTIVE);

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, provisionCredentialCB2((void *)credData, NULL,
              clientResponse));
    clientResponse->result = OC_STACK_OK;
    CredentialData_t *credData1 = createCredentialData();
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, provisionCredentialCB2((void *)credData1, NULL,
              clientResponse));

    OICFree(clientResponse);
    RemoveAllCredentials();
}

TEST_F(SRP, provisionCredentialCB1Full)
{
    CredentialData_t *credData = createCredentialData();
    OCDoHandle UNUSED = NULL;
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, provisionCredentialCB1((void *)credData, UNUSED,
              NULL));

    clientResponse->result = OC_STACK_RESOURCE_CHANGED;
    CredentialData_t *credData1 = createCredentialData();
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, provisionCredentialCB1((void *)credData1, UNUSED,
              clientResponse));

    clientResponse->result = OC_STACK_OK;
    CredentialData_t *credData2 = createCredentialData();
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, provisionCredentialCB1((void *)credData2, UNUSED,
              clientResponse));

//    OICFree(clientResponse);
    RemoveAllCredentials();
}

TEST_F(SRP, ProvisionCredentialDosCB2T1)
{
    CredentialData_t *credData = createCredentialData();
    ConvertStrToUuid("33333333-3333-3333-3333-222222222211", &credData->deviceInfo[0]->doxm->deviceID);
    ConvertStrToUuid("33333333-3333-3333-1111-222222222211", &credData->deviceInfo[1]->doxm->deviceID);
    PDMAddDevice(&credData->deviceInfo[0]->doxm->deviceID);
    PDMAddDevice(&credData->deviceInfo[1]->doxm->deviceID);
    PDMSetDeviceState(&credData->deviceInfo[0]->doxm->deviceID, PDM_DEVICE_ACTIVE);
    PDMSetDeviceState(&credData->deviceInfo[1]->doxm->deviceID, PDM_DEVICE_ACTIVE);


    Data_t *ctx = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx->ctx = credData;
    ctx->type = PSK_TYPE;
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_RESOURCE_CHANGED;

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionCredentialDosCB2((void *)ctx, NULL,
              clientResponse));

    OICFree(clientResponse);
    RemoveAllCredentials();
}

TEST_F(SRP, ProvisionCredentialDosCB2T2)
{
    CredentialData_t *credData = createCredentialData();
    ConvertStrToUuid("33333333-3333-3333-3333-222222222211", &credData->deviceInfo[0]->doxm->deviceID);
    ConvertStrToUuid("33333333-3333-3333-1111-222222222211", &credData->deviceInfo[1]->doxm->deviceID);
    PDMAddDevice(&credData->deviceInfo[0]->doxm->deviceID);
    PDMAddDevice(&credData->deviceInfo[1]->doxm->deviceID);
    PDMSetDeviceState(&credData->deviceInfo[0]->doxm->deviceID, PDM_DEVICE_ACTIVE);
    PDMSetDeviceState(&credData->deviceInfo[1]->doxm->deviceID, PDM_DEVICE_ACTIVE);


    Data_t *ctx = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx->ctx = credData;
    ctx->type = PSK_TYPE;
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_RESOURCE_CHANGED;

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionCredentialDosCB2((void *)ctx, NULL, NULL));

    OICFree(clientResponse);
    RemoveAllCredentials();
}


TEST_F(SRP, ProvisionCredentialDosCB1Full)
{
    Data_t *ctx = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx->ctx = createCredentialData();
    ctx->type = PSK_TYPE;
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_RESOURCE_CHANGED;

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionCredentialDosCB1((void *)ctx, NULL,
              clientResponse));
//    OICFree(clientResponse);
    RemoveAllCredentials();
}

TEST_F(SRP, ProvisionCredentialDosCB1ClientNull)
{
    Data_t *ctx = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx->ctx = createCredentialData();
    ctx->type = PSK_TYPE;
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_RESOURCE_CHANGED;

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionCredentialDosCB1((void *)ctx, NULL, NULL));
//    OICFree(clientResponse);
    RemoveAllCredentials();
}


static OCStackApplicationResult clientResponseHandler(void *context, OCDoHandle handle,
        OCClientResponse *clientResponse)
{
    OC_UNUSED(context);
    OC_UNUSED(handle);
    OC_UNUSED(clientResponse);
    return OC_STACK_DELETE_TRANSACTION;
}

TEST_F(SRP, provisionCredentialsFull)
{
    OicSecCred_t *cred = (OicSecCred_t *)OICCalloc(1, sizeof(OicSecCred_t ));
    CredentialData_t *credData4 = createCredentialData();

    EXPECT_EQ(OC_STACK_OK, provisionCredentials(cred, pDev1, credData4, clientResponseHandler));

    freeCredentialData(credData4);

    RemoveAllCredentials();
}

TEST_F(SRP, provisionCredentialsdeviceInfo)
{
    OicSecCred_t *cred = (OicSecCred_t *)OICCalloc(1, sizeof(OicSecCred_t ));
    CredentialData_t *credData4 = createCredentialData();

    EXPECT_EQ(OC_STACK_OK, provisionCredentials(cred, NULL, credData4, clientResponseHandler));

    RemoveAllCredentials();
}

TEST_F(SRP, ProvisionCredentialsDosFull)
{
    OicSecCred_t *cred = (OicSecCred_t *)OICCalloc(1, sizeof(OicSecCred_t ));

    EXPECT_EQ(OC_STACK_OK, ProvisionCredentialsDos(NULL, cred, pDev1, clientResponseHandler));

    RemoveAllCredentials();
}

TEST_F(SRP, ProvisionCredentialsDosdeviceInfo)
{
    OicSecCred_t *cred = (OicSecCred_t *)OICCalloc(1, sizeof(OicSecCred_t ));

    EXPECT_EQ(OC_STACK_ERROR, ProvisionCredentialsDos(NULL, cred, NULL, clientResponseHandler));

    RemoveAllCredentials();
}

TEST_F(SRP, ProvisionLocalCredentialFull)
{
    Data_t *ctx = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx->ctx = createCredentialData();
    ctx->type = PSK_TYPE;
    OicSecCred_t *cred = (OicSecCred_t *)OICCalloc(1, sizeof(OicSecCred_t ));

    EXPECT_EQ(OC_STACK_OK, ProvisionLocalCredential((void *)ctx, cred));

    FreeData(ctx);
    RemoveAllCredentials();
}

TEST_F(SRP, SetReadyForNormalOperationCBChainType)
{
    Data_t *ctx = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx->type = CHAIN_TYPE;
    TrustChainData_t *chainData = (TrustChainData_t *)OICCalloc(1, sizeof(TrustChainData_t));
    ctx->ctx = (TrustChainData_t *)chainData;
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, SetReadyForNormalOperationCB((void *)ctx, NULL,
              clientResponse));

    OICFree(clientResponse);
    OICFree(chainData);
    OICFree(ctx);
}

TEST_F(SRP, SetReadyForNormalOperationCBSpType)
{
    Data_t *ctx = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx->type = SP_TYPE;
    SpData_t *pData = (SpData_t *)OICCalloc(1, sizeof(SpData_t));
    ctx->ctx = (SpData_t *)pData;
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, SetReadyForNormalOperationCB((void *)ctx, NULL,
              clientResponse));

    OICFree(clientResponse);
    OICFree(pData);
    OICFree(ctx);
}

TEST_F(SRP, SetReadyForNormalOperationCBAclType)
{
    Data_t *ctx = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx->type = ACL_TYPE;
    ACLData_t *pData = (ACLData_t *)OICCalloc(1, sizeof(ACLData_t));
    ctx->ctx = (ACLData_t *)pData;
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, SetReadyForNormalOperationCB((void *)ctx, NULL,
              clientResponse));

    OICFree(clientResponse);
    OICFree(pData);
    OICFree(ctx);
}

TEST_F(SRP, SetReadyForNormalOperationCBCredType)
{
    Data_t *ctx = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx->type = PSK_TYPE;
    CredentialData_t *pData = createCredentialData();
    ctx->ctx = (CredentialData_t *)pData;
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, SetReadyForNormalOperationCB((void *)ctx, NULL,
              clientResponse));

    OICFree(clientResponse);
}

TEST_F(SRP, SetReadyForNormalOperationCBCertType)
{
    Data_t *ctx = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx->type = CERT_TYPE;
    CertData_t *pData = (CertData_t *)OICCalloc(1, sizeof(CertData_t));
    ctx->ctx = (CertData_t *)pData;
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, SetReadyForNormalOperationCB((void *)ctx, NULL,
              clientResponse));

    OICFree(clientResponse);
}

TEST_F(SRP, SetReadyForNormalOperationCBDefault)
{
    Data_t *ctx = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx->type = (DataType_t)55;
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, SetReadyForNormalOperationCB((void *)ctx, NULL,
              clientResponse));

    OICFree(clientResponse);
}

TEST_F(SRP, ProvisionCBFull)
{
    Data_t *ctx = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx->type = CHAIN_TYPE;
    TrustChainData_t *chainData = (TrustChainData_t *)OICCalloc(1, sizeof(TrustChainData_t));
    chainData->targetDev = pDev1;
    ctx->ctx = (TrustChainData_t *)chainData;
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionCB((void *)ctx, NULL, clientResponse));

    OICFree(clientResponse);
    OICFree(chainData);
    OICFree(ctx);
}

TEST_F(SRP, ProvisionPskCBFull)
{
    Data_t *ctx = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx->ctx = createCredentialData();
    ctx->type = PSK_TYPE;
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionPskCB((void *)ctx, NULL, clientResponse));

    Data_t *ctx1 = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx1->ctx = createCredentialData();
    ctx1->type = PSK_TYPE;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionPskCB((void *)ctx1, NULL, NULL));

    OICFree(clientResponse);
}

TEST_F(SRP, provisionCertificateCBFull)
{
    CredentialData_t *credData = createCredentialData();
    OCDoHandle UNUSED = NULL;
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, provisionCertificateCB((void *)credData, UNUSED,
              clientResponse));

    OICFree(clientResponse);
}

TEST_F(SRP, ProvisionTrustChainCBFull)
{
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_RESOURCE_CHANGED;

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionTrustChainCB(NULL, NULL, clientResponse));

    Data_t *ctx = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx->type = ACL_TYPE;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionTrustChainCB((void *)ctx, NULL, clientResponse));

    clientResponse->result = OC_STACK_OK;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionTrustChainCB((void *)ctx, NULL, clientResponse));
    clientResponse->result = OC_STACK_RESOURCE_CHANGED;

    ctx->type = CHAIN_TYPE;
    TrustChainData_t *chainData = (TrustChainData_t *)OICCalloc(1, sizeof(TrustChainData_t));
    chainData->targetDev = pDev1;
    ctx->ctx = (TrustChainData_t *)chainData;
    /*
        if (NULL != gCred)
        {
            OicSecCred_t *cred = NULL;
            OicSecCred_t *r = NULL;
            LL_FOREACH_SAFE(gCred, cred, r)
            {
                LL_DELETE(gCred, cred);
            }
        }
    */
    //RemoveAllCredentials();

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionTrustChainCB((void *)ctx, NULL, clientResponse));

    OicSecCred_t *credA = createCred();
    EXPECT_EQ(OC_STACK_OK, AddCredential(credA));
    chainData->credId = credA->credId;

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionTrustChainCB((void *)ctx, NULL, clientResponse));

    OICFree(clientResponse);
    OICFree(chainData);
    OICFree(ctx);
}

TEST_F(SRP, DeleteUnlinkData_tFull)
{
    UnlinkData_t *u = (UnlinkData_t *)OICCalloc(1, sizeof(UnlinkData_t));
    u->unlinkDev = (OCProvisionDev_t *)OICCalloc(1, sizeof(OCProvisionDev_t));
    u->unlinkRes = (OCProvisionResult_t *)OICCalloc(1, sizeof(OCProvisionResult_t));
    DeleteUnlinkData_t(u);
}

TEST_F(SRP, SRPGetCredResourceCBFull)
{
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    GetSecData_t *getSecData = (GetSecData_t *)OICCalloc(1, sizeof(GetSecData_t));
    getSecData->deviceInfo = createProvisionDev();
    getSecData->resArr = (OCProvisionResult_t *)OICCalloc(1, sizeof(OCProvisionResult_t));

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, SRPGetCredResourceCB(getSecData, NULL, clientResponse));
}

TEST_F(SRP, registerResultForGetCSRResourceCBFull)
{
    GetCsrData_t *getCsrData = (GetCsrData_t *)OICCalloc(1, sizeof(GetCsrData_t));
    getCsrData->deviceInfo = createProvisionDev();
    getCsrData->numOfResults = 0;
    getCsrData->resArr = (OCPMGetCsrResult_t *)OICCalloc(1, sizeof(OCPMGetCsrResult_t));
    getCsrData->resArr->csr = certData();
    getCsrData->resArr->csrLen = certDataLen();
    getCsrData->resArr->encoding = OIC_ENCODING_DER;

    uint8_t *payload = NULL;
    size_t payloadSize = 0;
    OCStackResult stackresult = OC_STACK_OK;

    EXPECT_EQ(OC_STACK_OK, CSRToCBORPayload(getCsrData->resArr->csr,
                                            getCsrData->resArr->csrLen, getCsrData->resArr->encoding, &payload, &payloadSize));
    registerResultForGetCSRResourceCB(getCsrData, stackresult, payload, payloadSize);
}

TEST_F(SRP, registerResultForGetCSRResourceCBNull)
{
    GetCsrData_t *getCsrData = (GetCsrData_t *)OICCalloc(1, sizeof(GetCsrData_t));
    getCsrData->deviceInfo = createProvisionDev();
    getCsrData->numOfResults = 0;
    getCsrData->resArr = (OCPMGetCsrResult_t *)OICCalloc(1, sizeof(OCPMGetCsrResult_t));
    getCsrData->resArr->csr = certData();
    getCsrData->resArr->csrLen = certDataLen();
    getCsrData->resArr->encoding = OIC_ENCODING_DER;

    uint8_t *payload = NULL;
    OCStackResult stackresult = OC_STACK_OK;

    registerResultForGetCSRResourceCB(getCsrData, stackresult, payload, 0);
}


TEST_F(SRP, RegisterProvResultFull)
{
    OCProvisionDev_t *targetDev = createProvisionDev();
    int numOfResults = 0;
    OCProvisionResult_t *resArr = (OCProvisionResult_t *)OICCalloc(1 + numOfResults,
                                  sizeof(OCProvisionResult_t));
    EXPECT_EQ(OC_STACK_OK, RegisterProvResult(targetDev, resArr, &numOfResults, OC_STACK_OK));
    OICFree(resArr);
}

static UnlinkData_t *createUlinkData()
{
    UnlinkData_t *u = (UnlinkData_t *)OICCalloc(1, sizeof(UnlinkData_t));
    if (NULL == u)
    {
        return NULL;
    }
    u->unlinkDev = (OCProvisionDev_t *)OICCalloc(2, sizeof(OCProvisionDev_t));
    u->unlinkRes = (OCProvisionResult_t *)OICCalloc(1, sizeof(OCProvisionResult_t));
    u->numOfResults = 0;
    u->resultCallback = provisionResultCB;
    ConvertStrToUuid("33333333-3333-3333-3333-222222222222", &u->unlinkRes[(u->numOfResults)].deviceId);
    u->unlinkDev[0].doxm = createDoxm();
    u->unlinkDev[1].doxm = createDoxm();

    ConvertStrToUuid("33333333-3333-3333-3333-222222222211", &u->unlinkDev[0].doxm->deviceID);
    ConvertStrToUuid("33333333-3333-3333-3333-222222222233", &u->unlinkDev[1].doxm->deviceID);
    PDMAddDevice(&u->unlinkDev[0].doxm->deviceID);
    PDMSetDeviceState(&u->unlinkDev[0].doxm->deviceID, PDM_DEVICE_ACTIVE);
    PDMAddDevice(&u->unlinkDev[1].doxm->deviceID);
    PDMSetDeviceState(&u->unlinkDev[1].doxm->deviceID, PDM_DEVICE_ACTIVE);
    PDMLinkDevices(&u->unlinkDev[0].doxm->deviceID, &u->unlinkDev[1].doxm->deviceID);
    return u;
}
/*
static void freeUlinkData(UnlinkData_t *u)
{
    PDMUnlinkDevices(&u->unlinkDev[0].doxm->deviceID, &u->unlinkDev[1].doxm->deviceID);
    PDMDeleteDevice(&u->unlinkDev[0].doxm->deviceID);
    PDMDeleteDevice(&u->unlinkDev[1].doxm->deviceID);
    OICFree(u->unlinkDev);
    OICFree(u->unlinkRes);
    OICFree(u);
}
*/
TEST_F(SRP, registerResultForUnlinkDevices)
{
    UnlinkData_t *u = createUlinkData();
    registerResultForUnlinkDevices(u, OC_STACK_OK, IDX_DB_UPDATE_RES);
    PDMUnlinkDevices(&u->unlinkDev[0].doxm->deviceID, &u->unlinkDev[1].doxm->deviceID);
    PDMDeleteDevice(&u->unlinkDev[0].doxm->deviceID);
    OICFree(u->unlinkDev);
    OICFree(u->unlinkRes);
    OICFree(u);
}

TEST_F(SRP, SendDeleteACLRequest)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SendDeleteACLRequest(NULL, clientResponseHandler, pDev1, pDev2));
    EXPECT_EQ(OC_STACK_OK, SendDeleteACLRequest((void *)clientResponseHandler, clientResponseHandler,
              pDev1, pDev2));
}

TEST_F(SRP, SRPUnlinkDevice2CBNull)
{
    UnlinkData_t *u = createUlinkData();
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, SRPUnlinkDevice2CB(u, NULL, NULL));
}

TEST_F(SRP, SRPUnlinkDevice2CBFull)
{
    UnlinkData_t *u = createUlinkData();
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_CONTINUE;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, SRPUnlinkDevice2CB(u, NULL, clientResponse));
    OICFree(clientResponse);
}

TEST_F(SRP, SRPUnlinkDevice1CBNull)
{
    UnlinkData_t *u = createUlinkData();
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, SRPUnlinkDevice1CB(u, NULL, NULL));
}

TEST_F(SRP, SRPUnlinkDevice1CBResult)
{
    UnlinkData_t *u = createUlinkData();
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_CONTINUE;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, SRPUnlinkDevice1CB(u, NULL, clientResponse));
    OICFree(clientResponse);
}

TEST_F(SRP, SRPUnlinkDevice1CB)
{
    UnlinkData_t *u = createUlinkData();
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_RESOURCE_DELETED;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, SRPUnlinkDevice1CB(u, NULL, clientResponse));
    OICFree(clientResponse);
}

static RemoveData_t *createRemoveData()
{
    RemoveData_t *pRemoveData = (RemoveData_t *)OICMalloc(sizeof(RemoveData_t));
    pRemoveData->revokeTargetDev = createProvisionDev();
    pRemoveData->linkedDevList = createProvisionDev();
    pRemoveData->removeRes = (OCProvisionResult_t *)OICCalloc(1, sizeof(OCProvisionResult_t));
    pRemoveData->numOfResults = 0;
    return pRemoveData;
}

static void freeRemoveData(RemoveData_t *pRemoveData)
{
    freeProvisionDev(pRemoveData->revokeTargetDev);
    freeProvisionDev(pRemoveData->linkedDevList);
    OICFree(pRemoveData->removeRes);
    pRemoveData->numOfResults = 0;
    OICFree(pRemoveData);
}

TEST_F(SRP, DeleteRemoveData_t)
{
    RemoveData_t *pRemoveData = createRemoveData();
    DeleteRemoveData_t(pRemoveData);
}

TEST_F(SRP, registerResultForRemoveDevice)
{
    RemoveData_t *removeData = createRemoveData();
    OicUuid_t *pLinkedDevId = createUuid();

    registerResultForRemoveDevice(removeData, pLinkedDevId, OC_STACK_OK, false);
    freeUuid(pLinkedDevId);
    freeRemoveData(removeData);
}

TEST_F(SRP, registerResultForResetDevice)
{
    RemoveData_t *removeData = createRemoveData();
    OicUuid_t *pLinkedDevId = createUuid();

    registerResultForResetDevice(removeData, pLinkedDevId, OC_STACK_OK, false);
    freeUuid(pLinkedDevId);
    freeRemoveData(removeData);
}

static void linkDevs(OicUuid_t *uuid1, OicUuid_t *uuid2)
{
    PDMAddDevice(uuid1);
    PDMSetDeviceState(uuid1, PDM_DEVICE_ACTIVE);
    PDMAddDevice(uuid2);
    PDMSetDeviceState(uuid2, PDM_DEVICE_ACTIVE);
    PDMLinkDevices(uuid1, uuid2);
}

TEST_F(SRP, SRPRemoveDeviceCB)
{
    RemoveData_t *pRemoveData = createRemoveData();
    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_RESOURCE_DELETED;
    clientResponse->identity.id_length = UUID_LENGTH;

    OicUuid_t *uuid = createUuid();
    ConvertStrToUuid("33333333-3333-3333-3333-112222222222", uuid);
    memcpy(clientResponse->identity.id, uuid->id, sizeof(OicUuid_t));
    ConvertStrToUuid("33333333-3333-3333-3333-662222222222",
                     &pRemoveData->revokeTargetDev->doxm->deviceID);
    linkDevs(uuid, &pRemoveData->revokeTargetDev->doxm->deviceID);

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, SRPRemoveDeviceCB(pRemoveData, NULL, clientResponse));
    freeUuid(uuid);
    OICFree(clientResponse );
    freeRemoveData(pRemoveData);
}

TEST_F(SRP, SRPSyncDeviceCredCB)
{
    RemoveData_t *pRemoveData = createRemoveData();
    pRemoveData->resultCallback = provisionResultCB;

    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_RESOURCE_DELETED;
    clientResponse->identity.id_length = UUID_LENGTH;

    OicUuid_t *uuid = createUuid();
    ConvertStrToUuid("33333333-3333-3333-3333-112222222222", uuid);
    memcpy(clientResponse->identity.id, uuid->id, sizeof(OicUuid_t));
    ConvertStrToUuid("33333333-3333-3333-3333-662222222222",
                     &pRemoveData->revokeTargetDev->doxm->deviceID);
    linkDevs(uuid, &pRemoveData->revokeTargetDev->doxm->deviceID);
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, SRPSyncDeviceCredCB(pRemoveData, NULL, clientResponse));
    freeUuid(uuid);
    OICFree(clientResponse );
    freeRemoveData(pRemoveData);
}

TEST_F(SRP, GetListofDevToReqDeleteCred)
{
    OCProvisionDev_t *pRevokeTargetDev = createProvisionDev();
    OCProvisionDev_t *pOwnedDevList = createProvisionDev();
    OCUuidList_t *pLinkedUuidList = (OCUuidList_t *)OICMalloc(sizeof(OCUuidList_t));
    pLinkedUuidList->next = NULL;
    ConvertStrToUuid("33333333-3333-3333-3333-111111111111", &pLinkedUuidList->dev);
    OCProvisionDev_t *ppLinkedDevList = NULL;
    size_t numOfLinkedDev;

    PDMAddDevice(&pLinkedUuidList->dev);
    PDMAddDevice(&pRevokeTargetDev->doxm->deviceID);

    EXPECT_EQ(OC_STACK_OK, GetListofDevToReqDeleteCred(pRevokeTargetDev, pOwnedDevList, pLinkedUuidList,
              &ppLinkedDevList, &numOfLinkedDev));
    OICFree(pLinkedUuidList);
    freeProvisionDev(pRevokeTargetDev);
    freeProvisionDev(pOwnedDevList);
}

TEST_F(SRP, SRPRemoveDevice)
{
    OCProvisionDev_t *pTargetDev = createProvisionDev();
    OicUuid_t *uuid = createUuid();
    ConvertStrToUuid("33333333-3333-3333-3333-112222222222", uuid);
    linkDevs(uuid, &pTargetDev->doxm->deviceID);

    EXPECT_EQ(OC_STACK_CONTINUE, SRPRemoveDevice((void *)provisionResultCB, 3, pTargetDev,
              provisionResultCB));
    freeUuid(uuid);
    freeProvisionDev(pTargetDev);
}


TEST_F(SRP, SRPRemoveDeviceWithoutDiscovery)
{
    OCProvisionDev_t *pTargetDev = createProvisionDev();

    OicUuid_t *uuid = createUuid();
    ConvertStrToUuid(UUID_SRV1, uuid);
    linkDevs(uuid, &pTargetDev->doxm->deviceID);

    EXPECT_EQ(OC_STACK_CONTINUE,
              SRPRemoveDeviceWithoutDiscovery((void *)provisionResultCB, pTargetDev, pDev2, provisioningCB));
    freeUuid(uuid);
    freeProvisionDev(pTargetDev);
}


TEST_F(SRP, ProvisionSecurityProfileInfoCB)
{
    SpData_t *pData = (SpData_t *)OICCalloc(1, sizeof(SpData_t));
    pData->sp = getSpDefault();
    pData->targetDev = createProvisionDev();
    pData->resArr = (OCProvisionResult_t *)OICCalloc(1, sizeof(OCProvisionResult_t));

    Data_t *ctx = (Data_t *)OICCalloc(1, sizeof(Data_t));
    ctx->ctx = (SpData_t *)pData;
    ctx->type = SP_TYPE;

    OCClientResponse *clientResponse = (OCClientResponse *)OICCalloc(1, sizeof(OCClientResponse));
    clientResponse->result = OC_STACK_RESOURCE_CHANGED;

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionSecurityProfileInfoCB(NULL, NULL, clientResponse));
    clientResponse->result = OC_STACK_OK;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionSecurityProfileInfoCB(ctx, NULL, clientResponse));
    clientResponse->result = OC_STACK_RESOURCE_CHANGED;
    ctx->type = ACL_TYPE;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionSecurityProfileInfoCB(ctx, NULL, clientResponse));
    ctx->type = SP_TYPE;
    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, ProvisionSecurityProfileInfoCB(ctx, NULL, clientResponse));

    FreeData(ctx);
    OICFree(clientResponse);
}

TEST_F(SRP, SetDOS)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SetDOS(NULL, DOS_RFNOP, NULL));

    Data_t *data = (Data_t *)OICCalloc(1, sizeof(Data_t));

    data->ctx = NULL;
    data->type = (DataType_t)25;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SetDOS(data, DOS_RFNOP, clientResponseHandler));

    SpData_t *pData = (SpData_t *)OICCalloc(1, sizeof(SpData_t));
    pData->sp = getSpDefault();
    pData->targetDev = createProvisionDev();
    pData->resArr = (OCProvisionResult_t *)OICCalloc(1, sizeof(OCProvisionResult_t));
    data->ctx = pData;
    data->type = SP_TYPE;
    EXPECT_EQ(OC_STACK_OK, SetDOS(data, DOS_RFNOP, clientResponseHandler));

    OICFree(data);
}
