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
#include "provisioningdatabasemanager.h"

const char ID_1[] = "1111111111111111";
const char ID_2[] = "2111111111111111";
const char ID_3[] = "3111111111111111";
const char ID_4[] = "4111111111111111";
const char ID_5[] = "5111111111111111";
const char ID_6[] = "6111111111111111";

TEST(PDMInitTest, PDMInitWithNULL)
{
    EXPECT_EQ(OC_STACK_OK, PDMInit(NULL));
}

TEST(PDMAddDeviceTest, NullUUID)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMAddDevice(NULL));
}

TEST(PDMIsDuplicateDeviceTest, NullUUID)
{
    EXPECT_TRUE(PDMIsDuplicateDevice(NULL));
}


TEST(PDMIsDuplicateDeviceTest, ValidUUID)
{
    OicUuid_t uid1 = {{0,}};
    memcpy(&uid1.id, ID_1, sizeof(uid1.id));
    PDMAddDevice(&uid1);
    OicUuid_t uid2 = {{0,}};
    memcpy(&uid2.id, ID_2, sizeof(uid2.id));
    PDMAddDevice(&uid2);
    OicUuid_t uid3 = {{0,}};
    memcpy(&uid3.id, ID_3, sizeof(uid3.id));

    EXPECT_TRUE(PDMIsDuplicateDevice(&uid1));
    EXPECT_FALSE(PDMIsDuplicateDevice(&uid3));
}

TEST(PDMAddDeviceTest, ValidUUID)
{
    OicUuid_t uid = {{0,}};

    uint8_t id[UUID_LENGTH] = {0,};
    for (size_t i = 0 ; i < sizeof(id) ; i++)
    {
        int tem = rand() % 25;

        id[i] = tem + 65;
    }

    memcpy(&uid.id, &id, UUID_LENGTH);

    EXPECT_EQ(OC_STACK_OK, PDMAddDevice(&uid));
    PDMDeleteDevice(&uid);
}

TEST(PDMLinkDevicesTest, NULLDevice1)
{
    OicUuid_t uid = {{0,}};
    memcpy(&uid.id, ID_2, sizeof(uid.id));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMLinkDevices(NULL, &uid));
}

TEST(PDMLinkDevicesTest, NULLDevice2)
{
    OicUuid_t uid = {{0,}};
    memcpy(&uid.id, ID_3, sizeof(uid.id));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMLinkDevices(&uid, NULL));
}

TEST(PDMLinkDevicesTest, ValidCase)
{
    OicUuid_t uid1 = {{0,}};
    memcpy(&uid1.id, ID_4, sizeof(uid1.id));
    PDMAddDevice(&uid1);
    OicUuid_t uid2 = {{0,}};
    memcpy(&uid2.id, ID_5, sizeof(uid2.id));
    PDMAddDevice(&uid2);
    EXPECT_EQ(OC_STACK_OK, PDMLinkDevices(&uid1, &uid2));
}

TEST(PDMUnlinkDevicesTest, NULLDevice1)
{
    OicUuid_t uid = {{0,}};
    memcpy(&uid.id, ID_3, sizeof(uid.id));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMUnlinkDevices(NULL, &uid));
}

TEST(PDMUnlinkDevicesTest, NULLDevice2)
{
    OicUuid_t uid = {{0,}};
    memcpy(&uid.id, ID_3, sizeof(uid.id));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMUnlinkDevices(&uid, NULL));
}

TEST(PDMUnlinkDevices, ValidCase)
{
    OicUuid_t uid1 = {{0,}};
    memcpy(&uid1.id, ID_4, sizeof(uid1.id));
    OicUuid_t uid2 = {{0,}};
    memcpy(&uid2.id, ID_5, sizeof(uid2.id));
    EXPECT_EQ(OC_STACK_OK, PDMUnlinkDevices(&uid1, &uid2));
}


TEST (PDMDeleteDevice, NULLDeviceID)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMDeleteDevice(NULL));
}

TEST (PDMDeleteDevice, ValidDeviceID)
{

    OicUuid_t uid = {{0,}};
    uint8_t id[UUID_LENGTH] = {0,};
    for (size_t i = 0 ; i < sizeof(id) ; i++)
    {
        int tem = rand() % 25;

        id[i] = tem + 65;
    }

    memcpy(&uid.id, &id, sizeof(uid.id));
    EXPECT_EQ(OC_STACK_OK, PDMDeleteDevice(&uid));
}

TEST(PDMGetOwnedDevices, ValidCase)
{
    OCUuidList_t *list = NULL;
    size_t noOfDevcies = 0;
    EXPECT_EQ(OC_STACK_OK, PDMGetOwnedDevices(&list, &noOfDevcies));
}

TEST(PDMGetLinkedDevices, NULLDeviceID)
{
    OCUuidList_t *list = NULL;
    size_t noOfDevices = 0;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMGetLinkedDevices(NULL, &list, &noOfDevices));
}

TEST(PDMGetLinkedDevices, ValidCase)
{
    OicUuid_t uid = {{0,}};
    memcpy(&uid.id, ID_1, sizeof(uid.id));
    OCUuidList_t *list = NULL;
    size_t noOfDevices = 0;
    EXPECT_EQ(OC_STACK_OK, PDMGetLinkedDevices(&uid, &list, &noOfDevices));
}

TEST(PDMSetLinkStale, NULLDeviceID1)
{
    OicUuid_t uid = {{0,}};
    memcpy(&uid.id, ID_1, sizeof(uid.id));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMSetLinkStale(NULL, &uid));
}

TEST(PDMSetLinkStale, NULLDeviceID2)
{
    OicUuid_t uid = {{0,}};
    memcpy(&uid.id, ID_1, sizeof(uid.id));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PDMSetLinkStale(&uid, NULL));
}

TEST(PDMSetLinkStale, ValidCase)
{
    OicUuid_t uid1 = {{0,}};
    memcpy(&uid1.id, ID_6, sizeof(uid1.id));
    OicUuid_t uid2 = {{0,}};
    memcpy(&uid2.id, ID_1, sizeof(uid2.id));
    EXPECT_EQ(OC_STACK_OK, PDMSetLinkStale(&uid1, &uid2));
}

TEST(PDMGetToBeUnlinkedDevices, ValidCase)
{
    OCPairList_t *list = NULL;
    size_t noOfDevices = 0;
    EXPECT_EQ(OC_STACK_OK, PDMGetToBeUnlinkedDevices(&list, &noOfDevices));
}

TEST(PDMClose, ValidCase)
{
   EXPECT_EQ(OC_STACK_OK, PDMClose());
}

TEST(PDMDestoryOicUuidLinkList, NULLParam)
{
   EXPECT_EQ(OC_STACK_OK, PDMDestoryOicUuidLinkList(NULL));
}

TEST(PDMDestoryStaleLinkList, NULLParam)
{
   EXPECT_EQ(OC_STACK_OK, PDMDestoryOicUuidLinkList(NULL));
}
