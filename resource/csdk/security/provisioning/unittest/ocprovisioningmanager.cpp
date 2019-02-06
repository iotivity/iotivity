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
#include "ocprovisioningmanager.h"
#include "experimental/logger.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "../src/ocprovisioningmanager.c"
#include "tools.h"

#ifdef __cplusplus
}
#endif

#undef TAG
#define TAG "OTM_OCPROVISIONINGMANAGER"

#define SVR_DB_FILE_NAME TAG".dat"
#define PM_DB_FILE_NAME TAG".db"

static OCProvisionDev_t *pDev1 = NULL;
static OCProvisionDev_t *pDev2 = NULL;

class PM : public ::testing::Test
{
    public:

        static void SetUpTestCase()
        {
            IOT_Init(PM_DB_FILE_NAME);

            pDev1 = createProvisionDev();
            pDev2 = createProvisionDev();
            pDev2->endpoint.port = 9998;
            ConvertStrToUuid("33333355-3333-3333-3333-111111111111", &pDev2->doxm->deviceID);
        }

        static void TearDownTestCase()
        {
            IOT_DeInit(PM_DB_FILE_NAME);
        }

        OicSecAcl_t acl1;
        OicSecAcl_t acl2;
};

static void provisioningCB (void *UNUSED1, size_t UNUSED2, OCProvisionResult_t *UNUSED3,
                            bool UNUSED4)
{
    //dummy callback
    (void) UNUSED1;
    (void) UNUSED2;
    (void) UNUSED3;
    (void) UNUSED4;
}

static OCStackResult OTMLoadSecretCallback(OTMContext_t *otmCtx)
{
    //dummy callback
    (void) otmCtx;
    return OC_STACK_OK;
}

TEST_F(PM, OCProvisionPairwiseDevicesTestNullDevice1)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCProvisionPairwiseDevices(NULL, SYMMETRIC_PAIR_WISE_KEY,
              OWNER_PSK_LENGTH_128, NULL, &acl1,
              pDev2, &acl2, &provisioningCB));
}

TEST_F(PM, OCProvisionPairwiseDevicesTestNullDevice2)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCProvisionPairwiseDevices(NULL, SYMMETRIC_PAIR_WISE_KEY,
              OWNER_PSK_LENGTH_128, pDev1, &acl1,
              NULL, &acl2, &provisioningCB));
}

TEST_F(PM, OCProvisionPairwiseDevicesTestSamelDeviceId)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCProvisionPairwiseDevices(NULL, SYMMETRIC_PAIR_WISE_KEY,
              OWNER_PSK_LENGTH_128, pDev1, &acl1,
              pDev1, &acl2, &provisioningCB));
}

TEST_F(PM, OCProvisionPairwiseDevicesTestNullCallback)
{
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, OCProvisionPairwiseDevices(NULL, SYMMETRIC_PAIR_WISE_KEY,
              OWNER_PSK_LENGTH_128, pDev1, &acl1,
              pDev2, &acl2, NULL));
}

TEST_F(PM, OCProvisionPairwiseDevicesTestInvalidKeySize)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCProvisionPairwiseDevices(NULL, SYMMETRIC_PAIR_WISE_KEY,
              0, pDev1, &acl1,
              pDev2, &acl2, &provisioningCB));
}

TEST_F(PM, OCProvisionPairwiseDevices)
{
    PDMDeleteDevice(&pDev1->doxm->deviceID);
    PDMDeleteDevice(&pDev2->doxm->deviceID);
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&pDev1->doxm->deviceID));
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&pDev2->doxm->deviceID));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&pDev1->doxm->deviceID, PDM_DEVICE_ACTIVE));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&pDev2->doxm->deviceID, PDM_DEVICE_ACTIVE));

    EXPECT_EQ(OC_STACK_OK, OCProvisionPairwiseDevices(NULL, SYMMETRIC_PAIR_WISE_KEY,
              OWNER_PSK_LENGTH_256, pDev1, &acl1,
              pDev2, &acl2, &provisioningCB));
    EXPECT_EQ(OC_STACK_OK, PDMLinkDevices(&pDev1->doxm->deviceID, &pDev2->doxm->deviceID));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCProvisionPairwiseDevices(NULL, SYMMETRIC_PAIR_WISE_KEY,
              OWNER_PSK_LENGTH_256, pDev1, &acl1,
              pDev2, &acl2, &provisioningCB));
}

TEST_F(PM, OCGetDevInfoFromNetwork)
{
    OCProvisionDev_t *pOwnedDevList = NULL;
    OCProvisionDev_t *pUnownedDevList = NULL;
    EXPECT_EQ(OC_STACK_OK, OCGetDevInfoFromNetwork(4, &pOwnedDevList, &pUnownedDevList));
}

TEST_F(PM, OCUnlinkDevicesTestNullDevice1)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCUnlinkDevices(NULL, NULL, pDev2, provisioningCB));
}

TEST_F(PM, OCUnlinkDevicesTestNullDevice2)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCUnlinkDevices(NULL, pDev1, NULL, provisioningCB));
}

TEST_F(PM, OCUnlinkDevicesTestNullCallback)
{
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, OCUnlinkDevices(NULL, pDev1, pDev2, NULL));
}

TEST_F(PM, OCUnlinkDevicesTestSamelDeviceId)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCUnlinkDevices(NULL, pDev1, pDev1, provisioningCB));
}

TEST_F(PM, OCUnlinkDevicesTest)
{
    PDMDeleteDevice(&pDev1->doxm->deviceID);
    PDMDeleteDevice(&pDev2->doxm->deviceID);
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&pDev1->doxm->deviceID));
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&pDev2->doxm->deviceID));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&pDev1->doxm->deviceID, PDM_DEVICE_ACTIVE));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&pDev2->doxm->deviceID, PDM_DEVICE_ACTIVE));
    EXPECT_EQ(OC_STACK_OK, PDMLinkDevices(&pDev1->doxm->deviceID, &pDev2->doxm->deviceID));
    EXPECT_EQ(OC_STACK_OK, OCUnlinkDevices(NULL, pDev1, pDev2, provisioningCB));
}

TEST_F(PM, OCRemoveDeviceTestNullTargetDevice)
{
    unsigned short waitTime = 10 ;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCRemoveDevice(NULL, waitTime, NULL, provisioningCB));
}

TEST_F(PM, OCRemoveDeviceWithUuidTestNullTargetDevice)
{
    unsigned short waitTime = 10 ;
    OicUuid_t uuid;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCRemoveDeviceWithUuid(NULL, waitTime, NULL, provisioningCB));

    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, OCRemoveDeviceWithUuid(NULL, waitTime, &uuid, NULL));
}

TEST_F(PM, OCRemoveDeviceTestNullResultCallback)
{
    unsigned short waitTime = 10;
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, OCRemoveDevice(NULL, waitTime, pDev1, NULL));
}

TEST_F(PM, OCRemoveDeviceTestZeroWaitTime)
{
    unsigned short waitTime = 0;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCRemoveDevice(NULL, waitTime, pDev1, provisioningCB));
}

TEST_F(PM, OCGetDevInfoFromNetworkTestNullUnOwnedDeviceInfo)
{
    unsigned short waitTime = 10;
    OCProvisionDev_t *ownedList = NULL;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCGetDevInfoFromNetwork(waitTime, &ownedList, NULL));
}

TEST_F(PM, OCGetDevInfoFromNetworkTestNullOwnedDeviceInfo)
{
    unsigned short waitTime = 10;
    OCProvisionDev_t *unownedList = NULL;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCGetDevInfoFromNetwork(waitTime, NULL, &unownedList));
}

TEST_F(PM, OCGetDevInfoFromNetworkTestZeroWaitTime)
{
    unsigned short waitTime = 0;
    OCProvisionDev_t *ownedList = NULL;
    OCProvisionDev_t *unownedList = NULL;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCGetDevInfoFromNetwork(waitTime, &ownedList, &unownedList));
}

TEST_F(PM, OCGetLinkedStatusTestNULLDeviceID)
{
    OCUuidList_t *list = NULL;
    size_t noOfDevices = 0;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCGetLinkedStatus(NULL, &list, &noOfDevices));
}

TEST_F(PM, OCDeleteUuidListTestNullUuidList)
{
    OCDeleteUuidList(NULL);
    EXPECT_EQ(1, 1);
}

TEST_F(PM, OCDeleteACLListTestNullACLList)
{
    OCDeleteACLList(NULL);
    EXPECT_EQ(1, 1);
}

TEST_F(PM, OCDeleteDiscoveredDevicesTestNullProvisionDevList)
{
    OCDeleteDiscoveredDevices(NULL);
    EXPECT_EQ(1, 1);
}

TEST_F(PM, OCSetOwnerTransferCallbackDataTestNULLCallback)
{
    OicSecOxm_t ownershipTransferMethod = OIC_JUST_WORKS;
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, OCSetOwnerTransferCallbackData(ownershipTransferMethod,
              NULL));
}

TEST_F(PM, OCSetOwnerTransferCallbackDataTestInvalidOXMType)
{
    OicSecOxm_t ownershipTransferMethod = OIC_OXM_COUNT;
    OTMCallbackData_t stOTMCallbackData = { &OTMLoadSecretCallback, NULL, NULL, NULL};
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCSetOwnerTransferCallbackData(ownershipTransferMethod,
              &stOTMCallbackData));
}

TEST_F(PM, UpdateLinkResults)
{
    Linkdata_t *link = (Linkdata_t *)OICMalloc(sizeof(Linkdata_t));
    link->pDev1 = NULL;
    link->pDev1Acl = (OicSecAcl_t *)OICCalloc(1, sizeof(OicSecAcl_t));
    link->pDev2 = NULL;
    link->pDev2Acl = (OicSecAcl_t *)OICCalloc(1, sizeof(OicSecAcl_t));
//    link->ctx = ctx;
    link->numOfResults = 2;
//    link->resultCallback = resultCallback;
    link->currentCountResults = 0;
    link->resArr = (OCProvisionResult_t *)OICCalloc(link->numOfResults, sizeof(OCProvisionResult_t));

    UpdateLinkResults(NULL, 1, OC_STACK_OK);
    UpdateLinkResults(link, 1, OC_STACK_OK);
    link->pDev1 = createProvisionDev();
    UpdateLinkResults(link, 1, OC_STACK_OK);
    link->pDev2 = createProvisionDev();
    UpdateLinkResults(link, 2, OC_STACK_OK);

    freeProvisionDev((OCProvisionDev_t *)link->pDev1);
    freeProvisionDev((OCProvisionDev_t *)link->pDev2);
    OICFree(link->pDev2Acl);
    OICFree(link->pDev1Acl);
    OICFree(link->resArr);
    OICFree(link);
}

static void prcb(void *ctx, size_t nOfRes, OCProvisionResult_t *arr, bool hasError)
{
    OC_UNUSED(nOfRes);
    OC_UNUSED(arr);
    OC_UNUSED(hasError);
    OC_UNUSED(ctx);
    OIC_LOG_V(DEBUG, TAG, "%s", __func__);
}

Linkdata_t *createLinkData()
{
    Linkdata_t *link = (Linkdata_t *)OICMalloc(sizeof(Linkdata_t));
    link->pDev1 = createProvisionDev();
    link->pDev1Acl = (OicSecAcl_t *)OICCalloc(1, sizeof(OicSecAcl_t));
    link->pDev2 = createProvisionDev();
    link->pDev2Acl = (OicSecAcl_t *)OICCalloc(1, sizeof(OicSecAcl_t));
    link->numOfResults = 2;
    link->currentCountResults = 0;
    link->resArr = (OCProvisionResult_t *) OICMalloc(sizeof(OCProvisionResult_t) * link->numOfResults);
    link->resultCallback = prcb;
    return link;
}

void freeLinkData(Linkdata_t *link)
{
    freeProvisionDev((OCProvisionDev_t *)link->pDev2);
    freeProvisionDev((OCProvisionDev_t *)link->pDev1);
    OICFree(link->resArr);
    OICFree(link->pDev2Acl);
    OICFree(link->pDev1Acl);
    OICFree(link);
}

TEST_F(PM, AclProv2CB)
{
    OCProvisionResult_t *arr = (OCProvisionResult_t *)OICCalloc(1, sizeof(OCProvisionResult_t));

    AclProv2CB(NULL, 1, arr, false);

    Linkdata_t *link = createLinkData();
    AclProv2CB((void *)link, 1, arr, false);

    Linkdata_t *link1 = createLinkData();
    AclProv2CB((void *)link1, 1, arr, true);

    OICFree(arr);
}

TEST_F(PM, ProvisionCredsCB)
{
    Linkdata_t *link = createLinkData();
    OCProvisionResult_t *arr = (OCProvisionResult_t *)OICCalloc(1, sizeof(OCProvisionResult_t));

    ProvisionCredsCB(NULL, 1, arr, false);
    ProvisionCredsCB((void *)link, 1, arr, false);

    Linkdata_t *link1 = createLinkData();
    ProvisionCredsCB((void *)link1, 1, arr, true);

    OICFree(arr);
}

TEST_F(PM, AclProv1CB)
{
    Linkdata_t *link = createLinkData();
    OCProvisionResult_t *arr = (OCProvisionResult_t *)OICCalloc(1, sizeof(OCProvisionResult_t));

    AclProv1CB(NULL, 1, arr, false);
    AclProv1CB((void *)link, 1, arr, true);

    Linkdata_t *link1 = createLinkData();
    AclProv1CB((void *)link1, 1, arr, false);

    OICFree(arr);
}

TEST_F(PM, RemoveDeviceInfoFromLocal)
{
    OCProvisionDev_t *pDev = createProvisionDev();
    PDMAddDevice(&pDev->doxm->deviceID);
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&pDev->doxm->deviceID, PDM_DEVICE_ACTIVE));
    EXPECT_EQ(OC_STACK_OK, RemoveDeviceInfoFromLocal(pDev));
    freeProvisionDev(pDev);
}

static void provisionResultCB(void *ctx, size_t nOfRes, OCProvisionResult_t *arr, bool hasError)
{
    OC_UNUSED(ctx);
    OC_UNUSED(nOfRes);
    OC_UNUSED(arr);
    OC_UNUSED(hasError);
    OIC_LOG_V(DEBUG, TAG, "%s: has error: %d", __func__, hasError);
}
TEST_F(PM, OCRemoveDevice)
{
    void *ctx = NULL;
    OCProvisionDev_t *pDev = createProvisionDev();
//    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&pDev->doxm->deviceID));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&pDev->doxm->deviceID, PDM_DEVICE_ACTIVE));
    EXPECT_EQ(OC_STACK_OK, OCRemoveDevice(ctx, 1, pDev, provisionResultCB));
    freeProvisionDev(pDev);
}

TEST_F(PM, OCRemoveDeviceWithUuid)
{
    void *ctx = NULL;

    OicUuid_t *uuid1 = createUuidWith("33333333-3333-3333-3333-000000000000");
    OicUuid_t *uuid2 = createUuidWith("33333333-3333-3333-3333-000000000001");

    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(uuid1));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(uuid1, PDM_DEVICE_ACTIVE));
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(uuid2));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(uuid2, PDM_DEVICE_ACTIVE));

    EXPECT_EQ(OC_STACK_OK, PDMLinkDevices(uuid1, uuid2));

    EXPECT_EQ(OC_STACK_OK, OCRemoveDeviceWithUuid(ctx, 1, uuid1, provisionResultCB));
    freeUuid(uuid1);
    freeUuid(uuid2);
}

TEST_F(PM, OCResetDevice)
{
    void *ctx = NULL;
    OCProvisionDev_t *pDev = createProvisionDev();
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&pDev->doxm->deviceID, PDM_DEVICE_ACTIVE));

    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCResetDevice(ctx, 0, pDev, provisionResultCB));
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, OCResetDevice(ctx, 1, pDev, NULL));
    EXPECT_EQ(OC_STACK_OK, OCResetDevice(ctx, 1, pDev, provisionResultCB));
    freeProvisionDev(pDev);
}

