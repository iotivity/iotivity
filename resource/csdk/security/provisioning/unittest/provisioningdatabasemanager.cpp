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
#include "iotivity_config.h"
#include <gtest/gtest.h>
#include "ocprovisioningmanager.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "tools.h"
#include "../src/provisioningdatabasemanager.c"

#ifdef __cplusplus
}
#endif

#undef TAG
#define TAG "PDB"
#define PM_DB_FILE_NAME TAG".db"
#define SVR_DB_FILE_NAME "oic_svr_db_client.dat"

const char ID_1 [] = "1111111111111111";
const char ID_2 [] = "2111111111111111";
const char ID_3 [] = "3111111111111111";
const char ID_4 [] = "4111111111111111";
const char ID_5 [] = "5111111111111111";
const char ID_6 [] = "6111111111111111";
const char ID_7 [] = "7111111111111111";
const char ID_8 [] = "8111111111111111";
const char ID_9 [] = "9111111111111111";
const char ID_10[] = "1222222222222222";
const char ID_11[] = "2222222222222222";
const char ID_12[] = "3222222222222222";
const char ID_13[] = "4222222222222222";

#ifndef F_OK
#define F_OK 0
#endif

TEST(CallPDMAPIbeforeInit, BeforeInit)
{
    if (0 == access(PM_DB_FILE_NAME, F_OK))
    {
        EXPECT_EQ(0, remove(PM_DB_FILE_NAME));
    }
    EXPECT_EQ(OC_STACK_PDM_IS_NOT_INITIALIZED, PDMAddDevice(NULL));
    EXPECT_EQ(OC_STACK_PDM_IS_NOT_INITIALIZED, PDMIsDuplicateDevice(NULL, NULL));
    EXPECT_EQ(OC_STACK_PDM_IS_NOT_INITIALIZED, PDMLinkDevices(NULL, NULL));
    EXPECT_EQ(OC_STACK_PDM_IS_NOT_INITIALIZED, PDMUnlinkDevices(NULL, NULL));
    EXPECT_EQ(OC_STACK_PDM_IS_NOT_INITIALIZED, PDMDeleteDevice(NULL));
    EXPECT_EQ(OC_STACK_PDM_IS_NOT_INITIALIZED, PDMGetOwnedDevices(NULL, NULL));
    EXPECT_EQ(OC_STACK_PDM_IS_NOT_INITIALIZED, PDMGetLinkedDevices(NULL, NULL, NULL));
    EXPECT_EQ(OC_STACK_PDM_IS_NOT_INITIALIZED, PDMSetLinkStale(NULL, NULL));
    EXPECT_EQ(OC_STACK_PDM_IS_NOT_INITIALIZED, PDMGetToBeUnlinkedDevices(NULL, NULL));
    EXPECT_EQ(OC_STACK_PDM_IS_NOT_INITIALIZED, PDMIsLinkExists(NULL, NULL, NULL));
}

class PDB : public ::testing::Test
{
    public:
        static void SetUpTestCase()
        {
            unlink("PDM.db");
            IOT_Init(PM_DB_FILE_NAME);
        }

        static void TearDownTestCase()
        {
            IOT_DeInit(PM_DB_FILE_NAME);
            unlink("PDM.db");
        }
};

TEST_F(PDB, PDMInitWithNULL)
{
    EXPECT_EQ(OC_STACK_OK, PDMClose());
    EXPECT_EQ(OC_STACK_OK, OCInitPM(NULL));
    EXPECT_EQ(OC_STACK_OK, PDMClose());
    EXPECT_EQ(OC_STACK_OK, OCInitPM(PM_DB_FILE_NAME));
}

TEST_F(PDB, PDMAddDeviceNullUUID)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMAddDevice(NULL));
}

TEST_F(PDB, PDMIsDuplicateDeviceNullUUID)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMIsDuplicateDevice(NULL, NULL));
}


TEST_F(PDB, PDMIsDuplicateDeviceValidUUID)
{
    OicUuid_t uid1 = {{0,}};
    memcpy(&uid1.id, ID_1, sizeof(uid1.id));
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&uid1));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&uid1, PDM_DEVICE_ACTIVE));
    OicUuid_t uid2 = {{0,}};
    memcpy(&uid2.id, ID_2, sizeof(uid2.id));
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&uid2));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&uid2, PDM_DEVICE_ACTIVE));
    OicUuid_t uid3 = {{0,}};
    memcpy(&uid3.id, ID_3, sizeof(uid3.id));
    bool isDuplicate = true;
    EXPECT_EQ(OC_STACK_OK, PDMIsDuplicateDevice(&uid1, &isDuplicate));
    EXPECT_TRUE(isDuplicate);

    EXPECT_EQ(OC_STACK_OK, PDMIsDuplicateDevice(&uid3, &isDuplicate));
    EXPECT_FALSE(isDuplicate);
}

TEST_F(PDB, PDMAddDeviceValidUUID)
{
    OicUuid_t uid = {{0,}};

    uint8_t id[UUID_LENGTH] = {0,};
    for (size_t i = 0 ; i < sizeof(id) ; i++)
    {
        int tem = rand() % 25;

        id[i] = (uint8_t)(tem + 65);
    }

    memcpy(&uid.id, &id, UUID_LENGTH);

    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&uid));
    EXPECT_EQ(OC_STACK_OK, PDMDeleteDevice(&uid));
}

TEST_F(PDB, PDMLinkDevicesNULLDevice1)
{
    OicUuid_t uid = {{0,}};
    memcpy(&uid.id, ID_2, sizeof(uid.id));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMLinkDevices(NULL, &uid));
}

TEST_F(PDB, PDMLinkDevicesNULLDevice2)
{
    OicUuid_t uid = {{0,}};
    memcpy(&uid.id, ID_3, sizeof(uid.id));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMLinkDevices(&uid, NULL));
}

TEST_F(PDB, PDMLinkDevicesValidCase)
{
    OicUuid_t uid1 = {{0,}};
    memcpy(&uid1.id, ID_4, sizeof(uid1.id));
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&uid1));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&uid1, PDM_DEVICE_ACTIVE));

    OicUuid_t uid2 = {{0,}};
    memcpy(&uid2.id, ID_5, sizeof(uid2.id));
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&uid2));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&uid2, PDM_DEVICE_ACTIVE));

    EXPECT_EQ(OC_STACK_OK, PDMLinkDevices(&uid1, &uid2));
}

TEST_F(PDB, PDMUnlinkDevicesNULLDevice1)
{
    OicUuid_t uid = {{0,}};
    memcpy(&uid.id, ID_3, sizeof(uid.id));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMUnlinkDevices(NULL, &uid));
}

TEST_F(PDB, PDMUnlinkDevicesNULLDevice2)
{
    OicUuid_t uid = {{0,}};
    memcpy(&uid.id, ID_3, sizeof(uid.id));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMUnlinkDevices(&uid, NULL));
}

TEST_F(PDB, PDMUnlinkDevicesValidCase)
{
    OicUuid_t uid1 = {{0,}};
    memcpy(&uid1.id, ID_4, sizeof(uid1.id));
    OicUuid_t uid2 = {{0,}};
    memcpy(&uid2.id, ID_5, sizeof(uid2.id));
    EXPECT_EQ(OC_STACK_OK, PDMUnlinkDevices(&uid1, &uid2));
}


TEST_F(PDB, PDMDeleteDeviceNULLDeviceID)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMDeleteDevice(NULL));
}

TEST_F(PDB, PDMDeleteDeviceValidButNonExistDeviceID)
{
    OicUuid_t uid = {{0,}};
    uint8_t id[UUID_LENGTH] = {0,};
    for (size_t i = 0 ; i < sizeof(id) ; i++)
    {
        int tem = rand() % 25;

        id[i] = (uint8_t)(tem + 65);
    }

    memcpy(&uid.id, &id, sizeof(uid.id));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMDeleteDevice(&uid));
}

TEST_F(PDB, PDMGetOwnedDevicesValidCase)
{
    OCUuidList_t *list = NULL;
    size_t noOfDevcies = 0;
    EXPECT_EQ(OC_STACK_OK, PDMGetOwnedDevices(&list, &noOfDevcies));
    PDMFreeLinkedDevices(list);
}

TEST_F(PDB, PDMGetLinkedDevicesNULLDeviceID)
{
    OCUuidList_t *list = NULL;
    size_t noOfDevices = 0;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMGetLinkedDevices(NULL, &list, &noOfDevices));
    PDMFreeLinkedDevices(list);
}

TEST_F(PDB, PDMGetLinkedDevicesValidCase)
{
    OicUuid_t uid = {{0,}};
    memcpy(&uid.id, ID_1, sizeof(uid.id));
    OCUuidList_t *list = NULL;
    size_t noOfDevices = 0;
    EXPECT_EQ(OC_STACK_OK, PDMGetLinkedDevices(&uid, &list, &noOfDevices));
    PDMFreeLinkedDevices(list);
}

TEST_F(PDB, PDMGetLinkedDevicesInvalidCase)
{
    OicUuid_t uid = {{0,}};
    memcpy(&uid.id, ID_6, sizeof(uid.id));
    OCUuidList_t *list = NULL;
    size_t noOfDevices = 0;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMGetLinkedDevices(&uid, &list, &noOfDevices));
    PDMFreeLinkedDevices(list);
}

TEST_F(PDB, PDMSetLinkStaleNULLDeviceID1)
{
    OicUuid_t uid = {{0,}};
    memcpy(&uid.id, ID_1, sizeof(uid.id));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMSetLinkStale(NULL, &uid));
}

TEST_F(PDB, PDMSetLinkStaleNULLDeviceID2)
{
    OicUuid_t uid = {{0,}};
    memcpy(&uid.id, ID_1, sizeof(uid.id));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMSetLinkStale(&uid, NULL));
}

TEST_F(PDB, PDMSetLinkStaleValidCase)
{
    OicUuid_t uid1 = {{0,}};
    memcpy(&uid1.id, ID_6, sizeof(uid1.id));
    OicUuid_t uid2 = {{0,}};
    memcpy(&uid2.id, ID_1, sizeof(uid2.id));

    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMSetLinkStale(&uid1, &uid2));

    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&uid1));

    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&uid1, PDM_DEVICE_ACTIVE));

    EXPECT_EQ(OC_STACK_OK, PDMLinkDevices(&uid1, &uid2));

    EXPECT_EQ(OC_STACK_OK, PDMSetLinkStale(&uid1, &uid2));
}

TEST_F(PDB, PDMGetToBeUnlinkedDevicesValidCase)
{
    OCPairList_t *list = NULL;
    size_t noOfDevices = 0;
    EXPECT_EQ(OC_STACK_OK, PDMGetToBeUnlinkedDevices(&list, &noOfDevices));

    OCPairList_t *p1 = NULL;
    OCPairList_t *p2 = NULL;
    LL_FOREACH_SAFE(list, p1, p2)
    {
        LL_DELETE(list, p1);
        OICFree(p1);
    }
}

TEST_F(PDB, PDMDestoryOicUuidLinkListNULLParam)
{
    PDMDestoryOicUuidLinkList(NULL);
}

TEST_F(PDB, PDMDestoryStaleLinkListNULLParam)
{
    PDMDestoryStaleLinkList(NULL);
}

TEST_F(PDB, PDMIsLinkExistsDuplicateID)
{
    OicUuid_t uid1 = {{0,}};
    memcpy(&uid1.id, ID_7, sizeof(uid1.id));
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&uid1));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&uid1, PDM_DEVICE_ACTIVE));

    OicUuid_t uid2 = {{0,}};
    memcpy(&uid2.id, ID_8, sizeof(uid2.id));
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&uid2));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&uid2, PDM_DEVICE_ACTIVE));

    bool linkExisits = true;
    OCStackResult res = PDMIsLinkExists(&uid1, &uid2, &linkExisits);
    EXPECT_EQ(OC_STACK_OK, res);
    EXPECT_FALSE(linkExisits);
}

TEST_F(PDB, PDMSetDeviceStaleNULLUUID)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMSetDeviceState(NULL, PDM_DEVICE_STALE));
}

TEST_F(PDB, PDMSetDeviceStaleVALIDUUID)
{
    OicUuid_t uid1 = {{0,}};
    memcpy(&uid1.id, ID_9, sizeof(uid1.id));
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&uid1));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&uid1, PDM_DEVICE_STALE));
}

TEST_F(PDB, PDMSetDeviceStaleStaleDeviceNotinDeviceList)
{
    OicUuid_t uid1 = {{0,}};
    memcpy(&uid1.id, ID_10, sizeof(uid1.id));
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&uid1));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&uid1, PDM_DEVICE_STALE));

    OCUuidList_t *list = NULL;
    size_t noOfDevcies = 0;
    EXPECT_EQ(OC_STACK_OK, PDMGetOwnedDevices(&list, &noOfDevcies));

    while (list)
    {
        EXPECT_FALSE(0 == memcmp(list->dev.id, uid1.id, sizeof(uid1.id)));
        list = list->next;
    }
    PDMFreeLinkedDevices(list);
}

TEST_F(PDB, PDMSetDeviceStaleStaleDeviceNotinLinkedDevice)
{
    OicUuid_t uid1 = {{0,}};
    memcpy(&uid1.id, ID_11, sizeof(uid1.id));

    OicUuid_t uid2 = {{0,}};
    memcpy(&uid2.id, ID_12, sizeof(uid2.id));

    OicUuid_t uid3 = {{0,}};
    memcpy(&uid3.id, ID_13, sizeof(uid3.id));

    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&uid1));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&uid1, PDM_DEVICE_ACTIVE));
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&uid2));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&uid2, PDM_DEVICE_ACTIVE));
    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&uid3));
    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&uid3, PDM_DEVICE_ACTIVE));

    EXPECT_EQ(OC_STACK_OK, PDMLinkDevices(&uid1, &uid2));
    EXPECT_EQ(OC_STACK_OK, PDMLinkDevices(&uid2, &uid3));
    EXPECT_EQ(OC_STACK_OK, PDMLinkDevices(&uid1, &uid3));

    EXPECT_EQ(OC_STACK_OK, PDMSetDeviceState(&uid1, PDM_DEVICE_STALE));

    OCUuidList_t *list1 = NULL;
    size_t noOfDevices1 = 0;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMGetLinkedDevices(&uid1, &list1, &noOfDevices1));

    OCUuidList_t *list2 = NULL;
    size_t noOfDevices2 = 0;
    EXPECT_EQ(OC_STACK_OK, PDMGetLinkedDevices(&uid2, &list2, &noOfDevices2));
    OCUuidList_t *ptr = list2;
    while (ptr)
    {
        EXPECT_FALSE(0 == memcmp(ptr->dev.id, uid1.id, sizeof(uid1.id)));
        ptr = ptr->next;
    }
    ptr = list2;
    while (ptr)
    {
        EXPECT_TRUE(0 == memcmp(ptr->dev.id, uid3.id, sizeof(uid3.id)));
        ptr = ptr->next;
    }

    OCUuidList_t *list3 = NULL;
    size_t noOfDevices3 = 0;
    EXPECT_EQ(OC_STACK_OK, PDMGetLinkedDevices(&uid3, &list3, &noOfDevices3));
    ptr = list3;
    while (ptr)
    {
        EXPECT_FALSE(0 == memcmp(ptr->dev.id, uid1.id, sizeof(uid1.id)));
        ptr = ptr->next;
    }

    ptr = list3;
    while (ptr)
    {
        EXPECT_TRUE(0 == memcmp(ptr->dev.id, uid2.id, sizeof(uid2.id)));
        ptr = ptr->next;
    }
    PDMFreeLinkedDevices(list3);
    PDMFreeLinkedDevices(list2);
    PDMFreeLinkedDevices(list1);
}

/*
TEST_F(PDB, PDMCloseValidCase)
{
    EXPECT_EQ(OC_STACK_OK, PDMClose());
}
*/

