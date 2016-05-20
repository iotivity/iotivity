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
#include "gtest/gtest.h"
#include "secureresourceprovider.h"

static OicSecAcl_t acl;
static OCProvisionDev_t pDev1;
static OCProvisionDev_t pDev2;
static OicSecCredType_t credType = SYMMETRIC_PAIR_WISE_KEY;
static OCProvisionDev_t selectedDeviceInfo;
static OicSecPconf_t pconf;
static OicSecOxm_t oicSecDoxmJustWorks = OIC_JUST_WORKS;
static OicSecOxm_t oicSecDoxmRandomPin = OIC_RANDOM_DEVICE_PIN;
static OicSecDoxm_t defaultDoxm1 =
{
    NULL,                   /* OicUrn_t *oxmType */
    0,                      /* size_t oxmTypeLen */
    &oicSecDoxmJustWorks,  /* uint16_t *oxm */
    1,                      /* size_t oxmLen */
    OIC_JUST_WORKS,         /* uint16_t oxmSel */
    SYMMETRIC_PAIR_WISE_KEY,/* OicSecCredType_t sct */
    false,                  /* bool owned */
    {{0}},            /* OicUuid_t deviceID */
    false,                  /* bool dpc */
    {{0}},            /* OicUuid_t owner */
};

static OicSecDoxm_t defaultDoxm2 =
{
    NULL,                   /* OicUrn_t *oxmType */
    0,                      /* size_t oxmTypeLen */
    &oicSecDoxmRandomPin,  /* uint16_t *oxm */
    1,                      /* size_t oxmLen */
    OIC_RANDOM_DEVICE_PIN,         /* uint16_t oxmSel */
    SYMMETRIC_PAIR_WISE_KEY,/* OicSecCredType_t sct */
    false,                  /* bool owned */
    {{0}},            /* OicUuid_t deviceID */
    false,                  /* bool dpc */
    {{0}},            /* OicUuid_t owner */
};

static void provisioningCB (void* UNUSED1, int UNUSED2, OCProvisionResult_t *UNUSED3, bool UNUSED4)
{
    //dummy callback
    (void) UNUSED1;
    (void) UNUSED2;
    (void) UNUSED3;
    (void) UNUSED4;
}

TEST(SRPProvisionACLTest, NullDeviceInfo)
{
    pDev1.doxm = &defaultDoxm1;
    uint8_t deviceId1[] = {0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x49, 0x64};
    memcpy(pDev1.doxm->deviceID.id, deviceId1, sizeof(deviceId1));

    pDev2.doxm = &defaultDoxm2;
    uint8_t deviceId2[] = {0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x49, 0x63};
    memcpy(pDev2.doxm->deviceID.id, deviceId2, sizeof(deviceId2));

    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPProvisionACL(NULL, NULL, &acl, &provisioningCB));
}

TEST(SRPProvisionACLTest, NullCallback)
{
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, SRPProvisionACL(NULL, &pDev1, &acl, NULL));
}

TEST(SRPProvisionACLTest, NullACL)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPProvisionACL(NULL, &pDev1, NULL, &provisioningCB));
}

TEST(SRPProvisionCredentialsTest, NullDevice1)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPProvisionCredentials(NULL, credType,
                                                              OWNER_PSK_LENGTH_128, NULL,
                                                              &pDev2, &provisioningCB));
}

TEST(SRPProvisionCredentialsTest, SamelDeviceId)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPProvisionCredentials(NULL, credType,
                                                              OWNER_PSK_LENGTH_128, &pDev1,
                                                              &pDev1, &provisioningCB));
}

TEST(SRPProvisionCredentialsTest, NullCallback)
{
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, SRPProvisionCredentials(NULL, credType,
                                                                 OWNER_PSK_LENGTH_128,
                                                                 &pDev1, &pDev2, NULL));
}

TEST(SRPProvisionCredentialsTest, InvalidKeySize)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPProvisionCredentials(NULL, credType,
                                                                0, &pDev1, &pDev2,
                                                                &provisioningCB));
}

TEST(SRPUnlinkDevicesTest, NullDevice1)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPUnlinkDevices(NULL, NULL, &pDev2, provisioningCB));
}

TEST(SRPUnlinkDevicesTest, NullDevice2)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPUnlinkDevices(NULL, &pDev1, NULL, provisioningCB));
}

TEST(SRPUnlinkDevicesTest, SamelDeviceId)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPUnlinkDevices(NULL, &pDev1, &pDev1, provisioningCB));
}

TEST(SRPUnlinkDevicesTest, NullCallback)
{
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, SRPUnlinkDevices(NULL, &pDev1, &pDev2, NULL));
}

TEST(SRPRemoveDeviceTest, NullTargetDevice)
{
    unsigned short waitTime = 10 ;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPRemoveDevice(NULL, waitTime, NULL, provisioningCB));
}

TEST(SRPRemoveDeviceTest, NullResultCallback)
{
    unsigned short waitTime = 10;
    OCProvisionDev_t dev1;
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, SRPRemoveDevice(NULL, waitTime, &dev1, NULL));
}

TEST(SRPRemoveDeviceTest, ZeroWaitTime)
{
    OCProvisionDev_t dev1;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPRemoveDevice(NULL, 0, &dev1, NULL));
}

TEST(SRPProvisionDirectPairingTest, NullselectedDeviceInfo)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPProvisionDirectPairing(NULL, NULL, &pconf, &provisioningCB));
}

TEST(SRPProvisionDirectPairingTest, Nullpconf)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SRPProvisionDirectPairing(NULL, &selectedDeviceInfo, NULL, &provisioningCB));
}

TEST(SRPProvisionDirectPairingTest, Nullcallback)
{
    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, SRPProvisionDirectPairing(NULL, &selectedDeviceInfo, &pconf, NULL));
}
