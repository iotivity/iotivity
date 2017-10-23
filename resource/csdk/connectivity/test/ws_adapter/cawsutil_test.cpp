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
#include "cawsutil.h"

TEST(CAWSPrepareWSData, WithValidData)
{
    const char *data = "hello";
    int dataLen = strlen(data);

    WSData *wsData = CAWSPrepareWSData(data, dataLen);
    EXPECT_NE(nullptr, wsData);

    CAWSDestroyWSData(wsData);
}

TEST(CAWSDestroyWSData, WithValidData)
{
    const char *data = "hello";
    int dataLen = strlen(data);

    WSData *wsData = CAWSPrepareWSData(data, dataLen);
    EXPECT_NE(nullptr, wsData);

    CAWSDestroyWSData(wsData);
}

TEST(CAWSAddWSDataToList, AddingOneDataToList)
{
    const char *data = "hello";
    int dataLen = strlen(data);
    WSDataList *wsDataList = NULL;
    CAResult_t ret = CA_STATUS_FAILED;

    WSData *wsData = CAWSPrepareWSData(data, dataLen);
    EXPECT_NE(nullptr, wsData);

    ret = CAWSAddWSDataToList(&wsDataList, wsData);
    EXPECT_EQ(CA_STATUS_OK, ret);

    CAWSDestroyWSDataList(wsDataList);
}

TEST(CAWSAddWSDataToList, AddingTwoDataToList)
{
    const char *data = "hello";
    int dataLen = strlen(data);
    WSDataList *wsDataList = NULL;
    CAResult_t ret = CA_STATUS_FAILED;

    // First data to list
    WSData *wsData1 = CAWSPrepareWSData(data, dataLen);
    EXPECT_NE(nullptr, wsData1);

    ret = CAWSAddWSDataToList(&wsDataList, wsData1);
    EXPECT_EQ(CA_STATUS_OK, ret);

    // Second data to list
    WSData *wsData2 = CAWSPrepareWSData(data, dataLen);
    EXPECT_NE(nullptr, wsData2);

    ret = CAWSAddWSDataToList(&wsDataList, wsData2);
    EXPECT_EQ(CA_STATUS_OK, ret);

    CAWSDestroyWSDataList(wsDataList);
}

TEST(CAWSRemoveWSDataFromList, RemovingOneDataFromList)
{
    const char *data = "hello";
    int dataLen = strlen(data);
    WSDataList *wsDataList = NULL;
    CAResult_t ret = CA_STATUS_FAILED;

    WSData *wsData = CAWSPrepareWSData(data, dataLen);
    EXPECT_NE(nullptr, wsData);

    ret = CAWSAddWSDataToList(&wsDataList, wsData);
    EXPECT_EQ(CA_STATUS_OK, ret);

    // Should return valid data pointer
    WSData *retWsData = CAWSRemoveWSDataFromList(&wsDataList);
    EXPECT_EQ(wsData, retWsData);

    // Should return null pointer as list is empty now
    retWsData = CAWSRemoveWSDataFromList(&wsDataList);
    EXPECT_EQ(NULL, retWsData);

    CAWSDestroyWSDataList(wsDataList);
    CAWSDestroyWSData(wsData);
}

TEST(CAWSRemoveWSDataFromList, RemovingTwoDataFromList)
{
    const char *data = "hello";
    int dataLen = strlen(data);
    WSDataList *wsDataList = NULL;
    CAResult_t ret = CA_STATUS_FAILED;

    WSData *wsData1 = CAWSPrepareWSData(data, dataLen);
    EXPECT_NE(nullptr, wsData1);

    ret = CAWSAddWSDataToList(&wsDataList, wsData1);
    EXPECT_EQ(CA_STATUS_OK, ret);

    WSData *wsData2 = CAWSPrepareWSData(data, dataLen);
    EXPECT_NE(nullptr, wsData2);

    ret = CAWSAddWSDataToList(&wsDataList, wsData2);
    EXPECT_EQ(CA_STATUS_OK, ret);

    // Should return first data pointer
    WSData *retWsData1 = CAWSRemoveWSDataFromList(&wsDataList);
    EXPECT_EQ(wsData1, retWsData1);

    // Should return second data pointer
    WSData *retWsData2 = CAWSRemoveWSDataFromList(&wsDataList);
    EXPECT_EQ(wsData2, retWsData2);

    // Should return null pointer as list is empty now
    EXPECT_EQ(NULL, CAWSRemoveWSDataFromList(&wsDataList));

    CAWSDestroyWSDataList(wsDataList);
    CAWSDestroyWSData(wsData1);
    CAWSDestroyWSData(wsData2);
}

TEST(CAWSAddWSConnInfoToList, AddingOneConnectionToList)
{
    CAResult_t ret = CA_STATUS_FAILED;

    WSConnInfoList *connInfoList = NULL;
    WSConnInfo *connInfo = (WSConnInfo *) calloc(1, sizeof(WSConnInfo));

    ret = CAWSAddWSConnInfoToList(&connInfoList, connInfo);
    EXPECT_EQ(CA_STATUS_OK, ret);

    CAWSDestroyWSConnInfoList(connInfoList);
}

TEST(CAWSAddWSConnInfoToList, AddingTwoConnectionToList)
{
    CAResult_t ret = CA_STATUS_FAILED;

    WSConnInfoList *connInfoList = NULL;
    WSConnInfo *connInfo1 = (WSConnInfo *) calloc(1, sizeof(WSConnInfo));
    WSConnInfo *connInfo2 = (WSConnInfo *) calloc(1, sizeof(WSConnInfo));

    ret = CAWSAddWSConnInfoToList(&connInfoList, connInfo1);
    EXPECT_EQ(CA_STATUS_OK, ret);

    ret = CAWSAddWSConnInfoToList(&connInfoList, connInfo2);
    EXPECT_EQ(CA_STATUS_OK, ret);

    CAWSDestroyWSConnInfoList(connInfoList);
}

TEST(CAWSRemoveWSConnInfoFromList, RemovingOneConnectionFromList)
{
    CAResult_t ret = CA_STATUS_FAILED;
    bool removeState = false;

    WSConnInfoList *connInfoList = NULL;
    WSConnInfo *connInfo = (WSConnInfo *) calloc(1, sizeof(WSConnInfo));

    ret = CAWSAddWSConnInfoToList(&connInfoList, connInfo);
    EXPECT_EQ(CA_STATUS_OK, ret);

    removeState = CAWSRemoveWSConnInfoFromList(&connInfoList, connInfo);
    EXPECT_TRUE(removeState);

    // Remove again for verification
    removeState = CAWSRemoveWSConnInfoFromList(&connInfoList, connInfo);
    EXPECT_FALSE(removeState);

    CAWSDestroyWSConnInfoList(connInfoList);
    CAWSDestroyWSConnInfo(connInfo);
}

TEST(CAWSRemoveWSConnInfoFromList, RemovingTwoConnectionFromList)
{
    CAResult_t ret = CA_STATUS_FAILED;
    bool removeState = false;

    WSConnInfoList *connInfoList = NULL;

    WSConnInfo *connInfo1 = (WSConnInfo *) calloc(1, sizeof(WSConnInfo));
    ret = CAWSAddWSConnInfoToList(&connInfoList, connInfo1);
    EXPECT_EQ(CA_STATUS_OK, ret);

    WSConnInfo *connInfo2 = (WSConnInfo *) calloc(1, sizeof(WSConnInfo));
    ret = CAWSAddWSConnInfoToList(&connInfoList, connInfo2);
    EXPECT_EQ(CA_STATUS_OK, ret);

    removeState = CAWSRemoveWSConnInfoFromList(&connInfoList, connInfo1);
    EXPECT_TRUE(removeState);

    removeState = CAWSRemoveWSConnInfoFromList(&connInfoList, connInfo1);
    EXPECT_FALSE(removeState);

    removeState = CAWSRemoveWSConnInfoFromList(&connInfoList, connInfo2);
    EXPECT_TRUE(removeState);

    removeState = CAWSRemoveWSConnInfoFromList(&connInfoList, connInfo2);
    EXPECT_FALSE(removeState);

    CAWSDestroyWSConnInfoList(connInfoList);
    CAWSDestroyWSConnInfo(connInfo1);
    CAWSDestroyWSConnInfo(connInfo2);
}

TEST(CAWSFindWSConnInfoFromList, RetrievingConnectionFromListHavingSingleEntry)
{
    CAResult_t ret = CA_STATUS_FAILED;
    WSConnInfoList *connInfoList = NULL;
    CAEndpoint_t endpoint;
    WSConnInfo *connInfo = NULL;
    WSConnInfo *retConnInfo = NULL;

    memset(&endpoint, 0, sizeof(endpoint));
    endpoint.port = 7681;
    endpoint.adapter = CA_ADAPTER_WS;
    endpoint.flags = (CATransportFlags_t) (CA_IPV4 | CA_IPV6);

    // strncat is optimal than strncpy as it avoids unnecessary buffer padding.
    endpoint.addr[0] = '\0';
    strncat(endpoint.addr, "127.0.0.1", sizeof(endpoint.addr) - 1);

    connInfo = (WSConnInfo *) calloc(1, sizeof(WSConnInfo));
    connInfo->secureEndpoint.endpoint = endpoint;

    // Add to list
    ret = CAWSAddWSConnInfoToList(&connInfoList, connInfo);
    EXPECT_EQ(CA_STATUS_OK, ret);

    // Remove from list
    retConnInfo = CAWSFindWSConnInfoFromList(connInfoList, &endpoint);
    EXPECT_EQ(connInfo, retConnInfo);

    CAWSDestroyWSConnInfoList(connInfoList);
}

TEST(CAWSFindWSConnInfoFromList, RetrievingConnectionFromListHavingMultipleEntry)
{
    CAResult_t ret = CA_STATUS_FAILED;
    WSConnInfoList *connInfoList = NULL;
    CAEndpoint_t endpoint1, endpoint2;
    WSConnInfo *connInfo1 = NULL;
    WSConnInfo *connInfo2 = NULL;
    WSConnInfo *retConnInfo = NULL;

    memset(&endpoint1, 0, sizeof(endpoint1));
    endpoint1.port = 7681;
    endpoint1.adapter = CA_ADAPTER_WS;
    endpoint1.flags = (CATransportFlags_t) (CA_IPV4 | CA_IPV6);

    // strncat is optimal than strncpy as it avoids unnecessary buffer padding.
    endpoint1.addr[0] = '\0';
    strncat(endpoint1.addr, "127.0.0.1", sizeof(endpoint1.addr) - 1);

    // Add first connection to list
    connInfo1 = (WSConnInfo *) calloc(1, sizeof(WSConnInfo));
    connInfo1->secureEndpoint.endpoint = endpoint1;

    ret = CAWSAddWSConnInfoToList(&connInfoList, connInfo1);
    EXPECT_EQ(CA_STATUS_OK, ret);

    // Add second connection to list
    endpoint2 = endpoint1;

    // strncat is optimal than strncpy as it avoids unnecessary buffer padding.
    endpoint2.addr[0] = '\0';
    strncat(endpoint2.addr, "192.168.0.10", sizeof(endpoint2.addr) - 1);

    connInfo2 = (WSConnInfo *) calloc(1, sizeof(WSConnInfo));
    connInfo2->secureEndpoint.endpoint = endpoint2;

    ret = CAWSAddWSConnInfoToList(&connInfoList, connInfo2);
    EXPECT_EQ(CA_STATUS_OK, ret);

    // Finding first connection from list
    retConnInfo = CAWSFindWSConnInfoFromList(connInfoList, &endpoint1);
    EXPECT_EQ(connInfo1, retConnInfo);

    // Finding second connection from list
    retConnInfo = CAWSFindWSConnInfoFromList(connInfoList, &endpoint2);
    EXPECT_EQ(connInfo2, retConnInfo);

    CAWSDestroyWSConnInfoList(connInfoList);
}

TEST(CAWSGetPeerAddress, InvalidSocketFD)
{
    char address[66];
    uint16_t port;
    CAResult_t res = CAWSGetPeerAddress(-1, address, &port, NULL);
    EXPECT_EQ(CA_STATUS_FAILED, res);
}
