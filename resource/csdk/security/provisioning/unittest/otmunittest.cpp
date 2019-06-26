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
#include <stdio.h>
#include <stdlib.h>
#include <gtest/gtest.h>
#include "ocstack.h"
#include "utlist.h"
#include "experimental/logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocprovisioningmanager.h"
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "oxmmanufacturercert.h"
#include "provisioningdatabasemanager.h"
#ifdef MULTIPLE_OWNER
#include "multipleownershiptransfermanager.h"
#endif //MULTIPLE_OWNER
#include "srmutility.h"
#include "experimental/doxmresource.h"
#include "pmtypes.h"
#include "pmutility.h"
#include "experimental/ocrandom.h"

#ifdef __cplusplus
extern "C" {
#endif

//#include "../src/ocprovisioningmanager.c"
#include "../../../stack/src/ocpayloadparse.c"
#include "tools.h"

#undef TAG
#ifdef __cplusplus
}
#endif

#define TAG "OTM_UNITTEST"

#define OTM_TIMEOUT 60
#define DISCOVERY_TIMEOUT 10

#define SVR_DB_FILE_NAME "oic_svr_db_client.dat"
#define PM_DB_FILE_NAME TAG".db"

static bool g_doneCB;
static bool g_callbackResult;
static const char *g_otmCtx = "Test User Context";
static OCProvisionDev_t *g_unownedDevices = NULL;
static OCProvisionDev_t *g_ownedDevices = NULL;
static size_t gNumOfUnownDevice = 0;
static size_t gNumOfOwnDevice = 0;

using namespace std;

#define UUID_TEMPLATE "11111111-1234-1234-1234-12345678901"
#define UUID_TEMPLATE_LEN 35

class OTM : public ::testing::Test
{
    public:
        static void SetUpTestCase()
        {
            IOT_Init(PM_DB_FILE_NAME);

            OTMCallbackData_t otmcb;
            otmcb.loadSecretCB = LoadSecretJustWorksCallback;
            otmcb.createSecureSessionCB = CreateSecureSessionJustWorksCallback;
            otmcb.createSelectOxmPayloadCB = CreateJustWorksSelectOxmPayload;
            otmcb.createOwnerTransferPayloadCB = CreateJustWorksOwnerTransferPayload;

            EXPECT_EQ(OC_STACK_OK, OCSetOwnerTransferCallbackData(OIC_JUST_WORKS, &otmcb));

            g_doneCB = false;
            g_callbackResult = false;

            gNumOfUnownDevice = 0;
            gNumOfOwnDevice = 0;
        }

        static void TearDownTestCase()
        {
            PMDeleteDeviceList(g_unownedDevices);
            PMDeleteDeviceList(g_ownedDevices);

            IOT_DeInit(PM_DB_FILE_NAME);
        }
};

TEST_F(OTM, OCParsPayloadFull)
{
    OCPayload *outPayload = NULL;
    uint8_t payload[] =
        "\277brt\202hoic.wk.dhoic.d.tvbif\202ooic.if.baselinehoic.if.rbdix$80d642ae-9263-4a37-be4d-6ae7165bbda5cdmvrres.1.1.0,sh.1.1.0cicvjcore.1.1.0anl[TV] Samsung\377";
    size_t payloadSize = 147;
    EXPECT_EQ(OC_STACK_OK, OCParsePayload(&outPayload, OC_FORMAT_CBOR, PAYLOAD_TYPE_REPRESENTATION,
                                          payload, payloadSize));
}

TEST_F(OTM, OCDiscoverUnownedDevices)
{
    PMDeleteDeviceList(g_unownedDevices);
    g_unownedDevices = NULL;
    EXPECT_EQ(OC_STACK_OK, OCDiscoverUnownedDevices(DISCOVERY_TIMEOUT, &g_unownedDevices));

    OCProvisionDev_t *tempDev1 = NULL;
    OCProvisionDev_t *tempDev2 = NULL;
    gNumOfUnownDevice = 0;
    LL_FOREACH_SAFE(g_unownedDevices, tempDev1, tempDev2)
    {
        PMPrintOCProvisionDev(tempDev1);
        char *uuidString = NULL;
        EXPECT_EQ(OC_STACK_OK, ConvertUuidToStr((const OicUuid_t *) &tempDev1->doxm->deviceID,
                                                &uuidString));

        OIC_LOG_V(DEBUG, TAG, "%s: id:%s ip:%s:%d", __func__,
                  uuidString ? uuidString : "unknow id",
                  tempDev1->endpoint.addr, tempDev1->endpoint.port);

        if (0 != strncmp(UUID_TEMPLATE, uuidString, UUID_TEMPLATE_LEN))
        {
            LL_DELETE(g_unownedDevices, tempDev1);
        }
        else
        {
            OIC_LOG_V(DEBUG, TAG, "%s: append in list", __func__);
            gNumOfUnownDevice++;
        }
        OICFree(uuidString);
    }

    EXPECT_TRUE(gNumOfUnownDevice > 0);
}

TEST_F(OTM, CreatePinBasedSelectOxmPayloadFull)
{
    OicSecDoxm_t *doxm = createDoxm();
    doxm->oxm[0] = OIC_RANDOM_DEVICE_PIN;
    doxm->oxmSel = OIC_RANDOM_DEVICE_PIN;
    doxm->sct    = SYMMETRIC_PAIR_WISE_KEY | SIGNED_ASYMMETRIC_KEY;

    OTMContext_t *otmCtx = (OTMContext_t *)OICCalloc(1, sizeof(OTMContext_t));
    otmCtx->selectedDeviceInfo = (OCProvisionDev_t *)OICCalloc(1, sizeof(OCProvisionDev_t));
    otmCtx->selectedDeviceInfo->doxm = doxm;
    uint8_t *payload = NULL;
    size_t size = 0;

    EXPECT_EQ(OC_STACK_OK, CreatePinBasedSelectOxmPayload(otmCtx, &payload, &size));

    freeDoxm(doxm);
    OICFree(payload);
    OICFree(otmCtx->selectedDeviceInfo);
    OICFree(otmCtx);
}

TEST_F(OTM, JustWorksOxMTestNullParam)
{
    OTMContext_t *otmCtx = NULL;
    OCStackResult res = OC_STACK_ERROR;
    uint8_t *payloadRes = NULL;
    size_t size = 0;

    //LoadSecretJustWorksCallback always returns OC_STACK_OK.
    res = LoadSecretJustWorksCallback(otmCtx);
    EXPECT_TRUE(OC_STACK_OK == res);

    res = CreateSecureSessionJustWorksCallback(otmCtx);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateJustWorksSelectOxmPayload(otmCtx, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateJustWorksOwnerTransferPayload(otmCtx, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    OTMContext_t otmCtx2;
    otmCtx2.selectedDeviceInfo = NULL;

    //LoadSecretJustWorksCallback always returns OC_STACK_OK.
    res = LoadSecretJustWorksCallback(&otmCtx2);
    EXPECT_TRUE(OC_STACK_OK == res);

    res = CreateSecureSessionJustWorksCallback(&otmCtx2);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateJustWorksSelectOxmPayload(&otmCtx2, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateJustWorksOwnerTransferPayload(&otmCtx2, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);
}

TEST_F(OTM, RandomPinOxMTestNullParam)
{
    OTMContext_t *otmCtx = NULL;
    OCStackResult res = OC_STACK_ERROR;
    uint8_t *payloadRes = NULL;
    size_t size = 0;

    //LoadSecretJustWorksCallback always returns OC_STACK_OK.
    res = InputPinCodeCallback(otmCtx);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateSecureSessionRandomPinCallback(otmCtx);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreatePinBasedSelectOxmPayload(otmCtx, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreatePinBasedOwnerTransferPayload(otmCtx, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    OTMContext_t otmCtx2;
    otmCtx2.selectedDeviceInfo = NULL;

    //LoadSecretJustWorksCallback always returns OC_STACK_OK.
    res = InputPinCodeCallback(&otmCtx2);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateSecureSessionRandomPinCallback(&otmCtx2);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreatePinBasedSelectOxmPayload(&otmCtx2, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreatePinBasedOwnerTransferPayload(&otmCtx2, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);
}

TEST_F(OTM, ManufacturerCertOxMTestNullParam)
{
    OTMContext_t *otmCtx = NULL;
    OCStackResult res = OC_STACK_ERROR;
    uint8_t *payloadRes = NULL;
    size_t size = 0;

    res = PrepareMCertificateCallback(otmCtx);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateSecureSessionMCertificateCallback(otmCtx);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateMCertificateBasedSelectOxmPayload(otmCtx, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateMCertificateBasedOwnerTransferPayload(otmCtx, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    OTMContext_t otmCtx2;
    otmCtx2.selectedDeviceInfo = NULL;

    res = InputPinCodeCallback(&otmCtx2);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateSecureSessionMCertificateCallback(&otmCtx2);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateMCertificateBasedSelectOxmPayload(&otmCtx2, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);

    res = CreateMCertificateBasedOwnerTransferPayload(&otmCtx2, &payloadRes, &size);
    EXPECT_TRUE(OC_STACK_INVALID_PARAM == res);
}

/****************************************
 * Test the OTM modules with sample server
 ****************************************/
// callback function(s) for provisioning client using C-level provisioning API
static void ownershipTransferCB(void *ctx, size_t UNUSED1, OCProvisionResult_t *UNUSED2,
                                bool hasError)
{
    OC_UNUSED(UNUSED1);
    OC_UNUSED(UNUSED2);
    OC_UNUSED(ctx);

    if (!hasError)
    {
        OIC_LOG_V(DEBUG, TAG, "Ownership Transfer SUCCEEDED - ctx: %s", (char *) ctx);
    }
    else
    {
        OIC_LOG_V(FATAL, TAG, "Ownership Transfer FAILED - ctx: %s", (char *) ctx);
    }
    g_callbackResult = !hasError;
    g_doneCB = true;

    OIC_LOG_V(DEBUG, TAG, "%s: done(has erro: %s)", __func__, hasError ? "yes" : "no");
}

// callback function(s) for provisioning client using C-level provisioning API
static void removeDeviceCB(void *ctx, size_t UNUSED1, OCProvisionResult_t *UNUSED2, bool hasError)
{
    OC_UNUSED(UNUSED1);
    OC_UNUSED(UNUSED2);
    OC_UNUSED(ctx);

    if (!hasError)
    {
        OIC_LOG_V(DEBUG, TAG, "Remove device request SUCCEEDED - ctx: %s", (char *) ctx);
    }
    else
    {
        OIC_LOG_V(FATAL, TAG, "Remove device request FAILED - ctx: %s", (char *) ctx);
    }
    g_callbackResult = !hasError;
    g_doneCB = true;
}

static int waitCallbackRet(void)
{
    for (int i = 0; !g_doneCB && OTM_TIMEOUT > i; ++i)
    {
        sleep(1);
        if (OC_STACK_OK != OCProcess())
        {
            OIC_LOG(FATAL, TAG, "OCStack process error");
            return -1;
        }
    }

    return 0;
}

TEST_F(OTM, OCDiscoverSingleDevice)
{
    OCProvisionDev_t *foundDevice = NULL;

    OicUuid_t uuid = {0};
    ConvertStrToUuid("11111111-1234-1234-1234-123456789011", &uuid);

    EXPECT_EQ(OC_STACK_OK, OCDiscoverSingleDevice(DISCOVERY_TIMEOUT, &uuid, &foundDevice));

    int NumOfFoundDevice = 0;
    OCProvisionDev_t *tempDev = foundDevice;
    while (tempDev)
    {
        NumOfFoundDevice++;
        tempDev = tempDev->next;
    }

    OIC_LOG_V(DEBUG, TAG, "Discoveed: %d devices", NumOfFoundDevice);
    EXPECT_EQ(1, NumOfFoundDevice);
    PMPrintOCProvisionDev(foundDevice);
    PMDeleteDeviceList(foundDevice);
}

TEST_F(OTM, OCDiscoverSingleDeviceInUnicast)
{
    OicUuid_t uuid = {0};
    ConvertStrToUuid("11111111-1234-1234-1234-123456789011", &uuid);
    OCProvisionDev_t *foundDevice = NULL;
    ASSERT_EQ(OC_STACK_OK, OCDiscoverSingleDeviceInUnicast(DISCOVERY_TIMEOUT, &uuid, "", CT_ADAPTER_IP,
              &foundDevice));
    int NumOfFoundDevice = 0;
    OCProvisionDev_t *tempDev = foundDevice;
    while (tempDev)
    {
        NumOfFoundDevice++;
        tempDev = tempDev->next;
    }

    PMDeleteDeviceList(foundDevice);
    OIC_LOG_V(DEBUG, TAG, "Discoveed: %d devices", NumOfFoundDevice);
    EXPECT_GT(NumOfFoundDevice, 0);
}

TEST_F(OTM, OCDoOwnershipTransfer)
{
    ASSERT_TRUE(gNumOfUnownDevice > 0);
    ASSERT_NE(nullptr, g_unownedDevices);

    g_doneCB = false;
    EXPECT_EQ(OC_STACK_OK, OCDoOwnershipTransfer((void *)g_otmCtx, g_unownedDevices,
              ownershipTransferCB));

    if (waitCallbackRet()) // input |g_doneCB| flag implicitly
    {
        OIC_LOG(FATAL, TAG, "OCProvisionCredentials callback error");
        return;
    }

    EXPECT_EQ(true, g_callbackResult);
    EXPECT_EQ(true, g_doneCB);
}

TEST_F(OTM, OCDiscoverOwnedDevices)
{
    EXPECT_EQ(OC_STACK_OK, OCDiscoverOwnedDevices(DISCOVERY_TIMEOUT, &g_ownedDevices));

    gNumOfOwnDevice = 0;

    OCProvisionDev_t *tempDev = g_ownedDevices;
    while (tempDev)
    {
        gNumOfOwnDevice++;
        tempDev = tempDev->next;
    }

    EXPECT_TRUE(gNumOfOwnDevice > 0);
}

static void resultCallBack(void *ctx, size_t nOfRes, OCProvisionResult_t *arr, bool hasError)
{
    OC_UNUSED(ctx);
    OC_UNUSED(nOfRes);
    OC_UNUSED(arr);
    OC_UNUSED(hasError);

    OIC_LOG_V(DEBUG, TAG, "%s", __func__);
}

static void roleResultCallBack(void *ctx, size_t nOfRes, OCPMGetRolesResult_t *arr, bool hasError)
{
    OC_UNUSED(ctx);
    OC_UNUSED(nOfRes);
    OC_UNUSED(arr);
    OC_UNUSED(hasError);

    OIC_LOG_V(DEBUG, TAG, "%s", __func__);
}

static void SpResultCallBack(void *ctx, size_t nOfRes, OCPMGetSpResult_t *arr, bool hasError)
{
    OC_UNUSED(ctx);
    OC_UNUSED(nOfRes);
    OC_UNUSED(arr);
    OC_UNUSED(hasError);

    OIC_LOG_V(DEBUG, TAG, "%s", __func__);
}

void CSRResultCallBack(void *ctx, size_t nOfRes, OCPMGetCsrResult_t *arr, bool hasError)
{
    OC_UNUSED(ctx);
    OC_UNUSED(nOfRes);
    OC_UNUSED(arr);
    OC_UNUSED(hasError);

    OIC_LOG_V(DEBUG, TAG, "%s", __func__);
}

TEST_F(OTM, ACLFull)
{
    if (g_ownedDevices == 0)
    {
        OIC_LOG_V(WARNING, TAG, "%s: donn't found owned device", __func__);
        return;
    }

    bool ctx = false;
    OCProvisionDev_t *selectedDeviceInfo = g_ownedDevices;
    OicSecAcl_t *acl = (OicSecAcl_t *)OICCalloc(1, sizeof(OicSecAcl_t));

    OicSecRsrc_t *rsrc = (OicSecRsrc_t *)OICCalloc(1, sizeof(OicSecRsrc_t));
    rsrc->href = (char *)OICCalloc(16, sizeof(char));
    sprintf(rsrc->href, "/oic/sec/pstat");
    rsrc->wildcard = NO_WILDCARD;

    OicSecAce_t *ace = (OicSecAce_t *)OICCalloc(1, sizeof(OicSecAce_t));
    OicSecAce_t *acem = NULL;
    LL_FOREACH(acl->aces, acem)
    {
        ace->aceid |= acem->aceid;
    }

    ace->subjectType = OicSecAceRoleSubject;
    ace->subjectConn = ANON_CLEAR;
    ace->permission = PERMISSION_READ | PERMISSION_WRITE;
    LL_APPEND(ace->resources, rsrc);
    LL_APPEND(acl->aces, ace);

    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCProvisionACL((void *)&ctx, selectedDeviceInfo, NULL,
              resultCallBack));

    EXPECT_EQ(OC_STACK_OK, OCProvisionACL((void *)&ctx, selectedDeviceInfo, acl, resultCallBack));

    ace->subjectType = OicSecAceConntypeSubject;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCProvisionACL((void *)&ctx, selectedDeviceInfo, acl,
              resultCallBack));
    EXPECT_EQ(OC_STACK_OK, OCProvisionACL2((void *)&ctx, selectedDeviceInfo, acl, resultCallBack));
    EXPECT_EQ(OC_STACK_OK, OCSaveACL(acl));

    EXPECT_EQ(OC_STACK_OK, OCGetCredResource((void *)&ctx, selectedDeviceInfo, resultCallBack));

    EXPECT_EQ(OC_STACK_OK, OCGetACLResource((void *)&ctx, selectedDeviceInfo, resultCallBack));
    EXPECT_EQ(OC_STACK_OK, OCGetACL2Resource((void *)&ctx, selectedDeviceInfo, resultCallBack));

    EXPECT_EQ(OC_STACK_OK, OCGetCSRResource((void *)&ctx, selectedDeviceInfo, CSRResultCallBack));

    EXPECT_EQ(OC_STACK_OK, OCGetSpResource((void *)&ctx, selectedDeviceInfo, SpResultCallBack));

    EXPECT_EQ(OC_STACK_OK, OCGetRolesResource((void *)&ctx, selectedDeviceInfo, roleResultCallBack));

    uint8_t *trustCertChain = NULL;
    size_t chainSize = 0;
    EXPECT_EQ(OC_STACK_ERROR, OCReadTrustCertChain(0, &trustCertChain, &chainSize));

    EXPECT_EQ(OC_STACK_OK, OCDeleteRoleCertificateByCredId((void *)&ctx, selectedDeviceInfo,
              resultCallBack, 0));

    OICFree(rsrc->href);
    OICFree(rsrc);
    OICFree(ace);
    OICFree(acl);
}

static void provisionResultCallBack(void *ctx, size_t nOfRes, OCProvisionResult_t *arr,
                                    bool hasError)
{
    OC_UNUSED(ctx);
    OC_UNUSED(nOfRes);
    OC_UNUSED(arr);
    OC_UNUSED(hasError);

    OIC_LOG_V(DEBUG, TAG, "%s", __func__);
}

TEST_F(OTM, CredFull)
{
    if (g_ownedDevices == 0)
    {
        OIC_LOG_V(WARNING, TAG, "%s: donn't found owned device", __func__);
        return;
    }

    void *ctx = NULL;
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = 256;
    OCProvisionDev_t *pDev1 = g_ownedDevices;
    OCProvisionDev_t *pDev2 = g_ownedDevices ? g_ownedDevices->next : NULL;

    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCProvisionCredentials(ctx, type, keySize, pDev1, pDev2,
              provisionResultCallBack));
    OicSecRole_t *role1 = (OicSecRole_t *)OICCalloc(1, sizeof(OicSecRole_t));
    OicSecRole_t *role2 = (OicSecRole_t *)OICCalloc(1, sizeof(OicSecRole_t));

    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCProvisionSymmetricRoleCredentials(ctx, type, keySize, pDev1,
              pDev2, role1, role2, provisionResultCallBack));
    char *pem = (char *)OICCalloc(512, sizeof(char));
    snprintf(pem, 512, "begin cert\nend cert\n");
    EXPECT_EQ(OC_STACK_OK, OCProvisionCertificate(ctx, pDev1, pem, provisionResultCallBack));

    OICFree(pem);
    OICFree(role2);
    OICFree(role1);
    OICFree(ctx);
}

TEST_F(OTM, PerformLinkDevicesNullParam)
{
    if (gNumOfOwnDevice < 2)
    {
        OIC_LOG(WARNING, TAG, "Link can only pair owned devices");
        OIC_LOG_V(DEBUG, TAG, "Number of owned devices: %lu", gNumOfOwnDevice);
        return;
    }

    OicUuid_t myUuid;
    OCStackResult result = OC_STACK_ERROR;
    result = GetDoxmDeviceID(&myUuid);
    EXPECT_EQ(OC_STACK_OK, result);

    //Extract target device except PT to perform link devices.
    OCProvisionDev_t *dev1 = NULL;
    OCProvisionDev_t *dev2 = NULL;
    OCProvisionDev_t *tempDev = g_ownedDevices;

    while (tempDev)
    {
        if (memcmp(tempDev->doxm->deviceID.id, myUuid.id, UUID_LENGTH) != 0)
        {
            if (NULL == dev1)
            {
                dev1 = tempDev;
            }
            else if (NULL == dev2)
            {
                dev2 = tempDev;
                break;
            }
        }
        tempDev = tempDev->next;
    }
    EXPECT_TRUE(NULL != dev1);
    EXPECT_TRUE(NULL != dev2);

    // TODO: Pairwise provisioning (Cred & ACL)
    // TODO: This part will be updated after ACL and credential data-structure is updated.

    EXPECT_EQ(OC_STACK_OK, result);
}


TEST_F(OTM, PerformUnlinkDevicesNullParam)
{
    OCStackResult result = OC_STACK_OK;

    // TODO: Unlink devices
    // TODO: This part will be updated after ACL and credential data-structure is updated.

    EXPECT_EQ(OC_STACK_OK, result);
}

//TODO will fix after IOT-2106
#if 0
//#ifdef MULTIPLE_OWNER
static OCProvisionDev_t *g_motEnabledDevices = NULL;
static OCProvisionDev_t *g_multiplOwnedDevices = NULL;

static void updateDoxmForMOTCB(void *ctx, size_t nOfRes, OCProvisionResult_t *arr, bool hasError)
{
    OC_UNUSED(nOfRes);
    OC_UNUSED(arr);

    if (!hasError)
    {
        OIC_LOG_V(DEBUG, TAG, "POST 'doxm' SUCCEEDED - ctx: %s", (char *) ctx);
    }
    else
    {
        OIC_LOG_V(FATAL, TAG, "POST 'doxm'  FAILED - ctx: %s", (char *) ctx);
    }
    g_callbackResult = !hasError;
    g_doneCB = true;
}

static void provisionPreconfiguredPinCB(void *ctx, size_t nOfRes, OCProvisionResult_t *arr,
                                        bool hasError)
{
    OC_UNUSED(nOfRes);
    OC_UNUSED(arr);

    if (!hasError)
    {
        OIC_LOG_V(DEBUG, TAG, "Provision Preconfigured-PIN SUCCEEDED - ctx: %s", (char *) ctx);
    }
    else
    {
        OIC_LOG_V(FATAL, TAG, "Provision Preconfigured-PIN FAILED - ctx: %s", (char *) ctx);
    }
    g_callbackResult = !hasError;
    g_doneCB = true;
}

TEST_F(OTM, EnableMOTNullParam)
{
    OCStackResult result = OC_STACK_OK;

    if (NULL == g_ownedDevices)
    {
        OIC_LOG(DEBUG, TAG, "Discovering Only Owned Devices on Network..");
        result = OCDiscoverOwnedDevices(DISCOVERY_TIMEOUT, &g_ownedDevices);
        EXPECT_EQ(OC_STACK_OK, result);
        RemoveUnknownDeviceFromDevList(g_ownedDevices);
    }

    EXPECT_NE((OCProvisionDev_t *)NULL, g_ownedDevices);

    g_doneCB = false;
    ASSERT_EQ(OC_STACK_OK, OCChangeMOTMode(NULL, g_ownedDevices, OIC_MULTIPLE_OWNER_ENABLE,
                                           updateDoxmForMOTCB));
    if (waitCallbackRet()) // input |g_doneCB| flag implicitly
    {
        OIC_LOG(FATAL, TAG, "OCChangeMOTMode callback error");
        return;
    }

    EXPECT_TRUE(g_callbackResult);
}

TEST_F(OTM, DiscoverMOTEnabledDevicesNullParam)
{
    if (g_motEnabledDevices)
    {
        PMDeleteDeviceList(g_motEnabledDevices);
    }

    ASSERT_EQ(OC_STACK_OK, OCDiscoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT,
              &g_motEnabledDevices));

    RemoveUnknownDeviceFromDevList(g_motEnabledDevices);

    EXPECT_EQ(true, g_motEnabledDevices != NULL);
}

TEST_F(OTM, RegisterPreconfiguredPINNullParam)
{
    OCStackResult result = SetPreconfigPin("12341234", strlen("12341234"));
    EXPECT_EQ(OC_STACK_OK, result);
}

TEST_F(OTM, ProvisonPreconfiguredPINNullParam)
{
    ASSERT_EQ(true, g_motEnabledDevices != NULL);
    OCStackResult result = OC_STACK_OK;

    g_doneCB = false;
    result = OCProvisionPreconfigPin(NULL, g_motEnabledDevices, "12341234", strlen("12341234"),
                                     provisionPreconfiguredPinCB);
    ASSERT_EQ(OC_STACK_OK, result);
    if (waitCallbackRet()) // input |g_doneCB| flag implicitly
    {
        OIC_LOG(FATAL, TAG, "OCProvisionPreconfigPin callback error");
        return;
    }

    EXPECT_EQ(true, g_callbackResult);
}

TEST_F(OTM, SelectMOTMethodNullParam)
{
    ASSERT_EQ(true, g_motEnabledDevices != NULL);
    OCStackResult result = OC_STACK_OK;

    g_doneCB = false;
    result = OCSelectMOTMethod(NULL, g_motEnabledDevices, OIC_PRECONFIG_PIN, updateDoxmForMOTCB);
    ASSERT_EQ(OC_STACK_OK, result);
    if (waitCallbackRet()) // input |g_doneCB| flag implicitly
    {
        OIC_LOG(FATAL, TAG, "OCSelectMOTMethod callback error");
        return;
    }

    EXPECT_EQ(true, g_callbackResult);
}

// TODO: Need to new server to perform MOT
TEST_F(OTM, PerformMOTNullParam)
{
    ASSERT_EQ(true, g_motEnabledDevices != NULL);
    OCStackResult result = OC_STACK_OK;

    g_doneCB = false;
    result = OCDoMultipleOwnershipTransfer(NULL, g_motEnabledDevices, ownershipTransferCB);
    ASSERT_EQ(OC_STACK_OK, result);
    if (waitCallbackRet()) // input |g_doneCB| flag implicitly
    {
        OIC_LOG(FATAL, TAG, "OCDoMultipleOwnershipTransfer callback error");
        return;
    }
    EXPECT_EQ(true, g_callbackResult);
}

TEST_F(OTM, DiscoverMultipleOwnedDevicesNullParam)
{
    OCStackResult result = OC_STACK_OK;

    if (g_multiplOwnedDevices)
    {
        PMDeleteDeviceList(g_multiplOwnedDevices);
    }

    OIC_LOG(DEBUG, TAG, "Discovering MOT Enabled Devices on Network..");
    result = OCDiscoverMultipleOwnedDevices(DISCOVERY_TIMEOUT, &g_multiplOwnedDevices);
    ASSERT_EQ(OC_STACK_OK, result);
    RemoveUnknownDeviceFromDevList(&g_multiplOwnedDevices);
    EXPECT_TRUE(NULL != g_multiplOwnedDevices);
}
#endif //MULTIPLE_OWNER

TEST_F(OTM, OCRemoveDevice)
{
    ASSERT_TRUE(gNumOfOwnDevice > 0);
    ASSERT_NE(nullptr, g_ownedDevices);

    OicUuid_t myUuid;
    OCStackResult result = OC_STACK_ERROR;
    result = GetDoxmDeviceID(&myUuid);
    EXPECT_EQ(OC_STACK_OK, result);

    //Extract target device except PT to perform remove device.
    OCProvisionDev_t *removeDev = g_ownedDevices;
    while (removeDev)
    {
        if (memcmp(removeDev->doxm->deviceID.id, myUuid.id, UUID_LENGTH) != 0)
        {
            break;
        }
        removeDev = removeDev->next;
    }
    EXPECT_NE(nullptr, removeDev);

    g_doneCB = false;
    g_callbackResult = false;

    EXPECT_EQ(OC_STACK_OK, OCResetDevice((void *)g_otmCtx, DISCOVERY_TIMEOUT, removeDev,
                                         removeDeviceCB));

    EXPECT_TRUE(g_callbackResult);
    EXPECT_TRUE(g_doneCB);
}

TEST_F(OTM, OCSetOxmAllowStatusFull)
{
    EXPECT_EQ(OC_STACK_OK, OCSetOxmAllowStatus(OIC_JUST_WORKS, true));
    EXPECT_EQ(OC_STACK_OK, OCSetOxmAllowStatus(OIC_RANDOM_DEVICE_PIN, true));
    EXPECT_EQ(OC_STACK_OK, OCSetOxmAllowStatus(OIC_MANUFACTURER_CERTIFICATE, true));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCSetOxmAllowStatus(OIC_OXM_COUNT, true));
#ifdef MULTIPLE_OWNER
    EXPECT_EQ(OC_STACK_ERROR, OCSetOxmAllowStatus(OIC_PRECONFIG_PIN, true));
#endif //MULTIPLE_OWNER
    EXPECT_EQ(OC_STACK_OK, OCSetOxmAllowStatus(OIC_MV_JUST_WORKS, true));
    EXPECT_EQ(OC_STACK_OK, OCSetOxmAllowStatus(OIC_CON_MFG_CERT, true));
}

TEST_F(OTM, OCPDMCleanupForTimeoutFull)
{
    EXPECT_EQ(OC_STACK_OK, OCPDMCleanupForTimeout());
}

TEST_F(OTM, OCSelectOwnershipTransferMethodFull)
{
    OicSecOxm_t *supportedMethods = (OicSecOxm_t *)OICCalloc(1, sizeof(OicSecOxm_t));
    OicSecOxm_t *selectedMethod = (OicSecOxm_t *)OICCalloc(1, sizeof(OicSecOxm_t));
    *supportedMethods = OIC_JUST_WORKS;
    EXPECT_EQ(OC_STACK_OK, OCSelectOwnershipTransferMethod(supportedMethods, 1, selectedMethod,
              SUPER_OWNER));
    OICFree(selectedMethod);
    OICFree(supportedMethods );
}

TEST_F(OTM, OCProvisionTrustCertChainFull)
{
    if (gNumOfOwnDevice == 0)
    {
        OIC_LOG_V(WARNING, TAG, "%s: donn't found owned device", __func__);
        return;
    }

    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCProvisionTrustCertChain(NULL, SYMMETRIC_PAIR_WISE_KEY, 0,
              g_ownedDevices, resultCallBack));
}

TEST_F(OTM, OCProvisionSecurityProfileInfoFull)
{
    if (gNumOfOwnDevice == 0)
    {
        OIC_LOG_V(WARNING, TAG, "%s: donn't found owned device", __func__);
        return;
    }

    OicSecSp_t *sp = (OicSecSp_t *)OICCalloc(1, sizeof(OicSecSp_t));
    EXPECT_EQ(OC_STACK_OK, OCProvisionSecurityProfileInfo(NULL, sp, g_ownedDevices, resultCallBack));
    OICFree(sp);
}

TEST_F(OTM, OCSaveTrustCertChainFull)
{
    size_t chainSize = 2048;
    uint8_t *trustCertChain = (uint8_t *)getPemCert();
    uint16_t credId = 0;
    EXPECT_EQ(OC_STACK_OK, OCSaveTrustCertChain(trustCertChain, chainSize, OIC_ENCODING_PEM, &credId));
}

TEST_F(OTM, OCSaveOwnRoleCertFull)
{
    uint16_t credId = 0;
    char *cert = (char *)getPemCert();
    EXPECT_EQ(OC_STACK_OK, OCSaveOwnRoleCert(cert, &credId));
}

static void trustCertChainChangeCB(void *ctx, uint16_t credId, uint8_t *trustCertChain,
                                   size_t chainSize)
{
    OC_UNUSED(ctx);
    OC_UNUSED(credId);
    OC_UNUSED(trustCertChain);
    OC_UNUSED(chainSize);
}

static CAResult_t peerCNVerifyCallback(const unsigned char *cn, size_t cnLen)
{
    OC_UNUSED(cn);
    OC_UNUSED(cnLen);
    return CA_STATUS_OK;
}

TEST_F(OTM, OCRegisterTrustCertChainNotifierFull)
{
    OCRemoveTrustCertChainNotifier();
    EXPECT_EQ(OC_STACK_OK, OCRegisterTrustCertChainNotifier(NULL, trustCertChainChangeCB));
    OCRemoveTrustCertChainNotifier();
    OCSetPeerCNVerifyCallback(peerCNVerifyCallback);
}

TEST_F(OTM, OCSaveOwnCertChainFull)
{
    char *cert = (char *)getPemCert();
    char *key = (char *)getPemKey();
    uint16_t credId = 0;

    EXPECT_EQ(OC_STACK_OK, OCSaveOwnCertChain(cert, key, &credId));
}

TEST_F(OTM, OCRemoveCredentialFull)
{
    OicUuid_t uuid = {0};
    ConvertStrToUuid("11111111-1234-1234-1234-123456789011", &uuid);

    EXPECT_EQ(OC_STACK_RESOURCE_DELETED, OCRemoveCredential(&uuid));
}

TEST_F(OTM, OCDiscoverSingleDeviceFull)
{
    OicUuid_t *deviceID = createUuid();
    OCProvisionDev_t *ppFoundDevice = NULL;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCDiscoverSingleDevice(0, deviceID, &ppFoundDevice));
    EXPECT_EQ(OC_STACK_OK, OCDiscoverSingleDevice(3, deviceID, &ppFoundDevice));
    freeUuid(deviceID);
}

TEST_F(OTM, OCDiscoverSingleDeviceInUnicastFull)
{
    OicUuid_t *deviceID = createUuid();
    OCProvisionDev_t *ppFoundDevice = NULL;
    const char hostAddress[] = "127.0.0.1";
    OCConnectivityType connType = CT_ADAPTER_IP;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCDiscoverSingleDeviceInUnicast(0, deviceID, hostAddress,
              connType, &ppFoundDevice));
    EXPECT_EQ(OC_STACK_OK, OCDiscoverSingleDeviceInUnicast(3, deviceID, hostAddress, connType,
              &ppFoundDevice));
    freeUuid(deviceID);
}

TEST_F(OTM, OCDiscoverUnownedDevicesFull)
{
    OCProvisionDev_t *ppFoundDevice = NULL;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCDiscoverUnownedDevices(0, &ppFoundDevice));
    EXPECT_EQ(OC_STACK_OK, OCDiscoverUnownedDevices(3, &ppFoundDevice));
}

TEST_F(OTM, OCDiscoverOwnedDevicesFull)
{
    OCProvisionDev_t *ppFoundDevice = NULL;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCDiscoverOwnedDevices(0, &ppFoundDevice));
}


