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
}

#include "../src/cloud/cloudresource.c"

#ifdef TAG
#undef TAG
#endif
#define TAG  "CLOUD_UNITTEST"

#define STR_LEN 512
static const char *sample =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!:/";

void sessionInit(session_t *ses)
{
    ses->accessToken = (char *)OICCalloc(STR_LEN, sizeof(char));
    ses->refreshToken = (char *)OICCalloc(STR_LEN, sizeof(char));
    ses->uid = (char *)OICCalloc(STR_LEN, sizeof(char));
#if !defined(__MANDATORY__)
    ses->redirectUri = (char *)OICCalloc(STR_LEN, sizeof(char));
#endif // __MANDATORY__
    for (int i = 0; i < STR_LEN; i++)
    {
        ses->accessToken[i] = sample[rand() % STR_LEN];
        ses->refreshToken[i] = sample[rand() % STR_LEN];
        ses->uid[i] = sample[rand() % STR_LEN];
#if !defined(__MANDATORY__)
        ses->redirectUri[i] = sample[rand() % STR_LEN];
#endif // __MANDATORY__
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

    for (int i = 0; i < STR_LEN; i++)
    {
        cloud->apn[i] = sample[rand() % STR_LEN];
        cloud->cis[i] = sample[rand() % STR_LEN];
        cloud->at[i] = sample[rand() % STR_LEN];
        cloud->sid[i] = sample[rand() % STR_LEN];
    }

    cloud->session = (session_t *)OICCalloc(STR_LEN, sizeof(session_t));
    sessionInit(cloud->session);

    return cloud;
}

TEST(CloudResourceTest, InitCloudResource)
{
    EXPECT_EQ(OC_STACK_OK, InitCloudResource());
}

TEST(CloudResourceTest, DeInitCloudResource)
{
    EXPECT_EQ(OC_STACK_OK, DeInitCloudResource());
}

TEST(CloudResourceTest, CreateCloudResource)
{
    EXPECT_EQ(OC_STACK_OK, CreateCloudResource());
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
    OICFree(payload);
}

TEST(CloudResourceTest, CloudToCBORPayloadResourceVALID)
{
    OicCloud_t *cloud = getCloud();

    uint8_t *payload = NULL;
    size_t size = 0;
    EXPECT_EQ(OC_STACK_OK, CloudToCBORPayloadResource(cloud, &payload, &size));
    EXPECT_TRUE(payload != NULL);

    FreeCloud(cloud);
    OICFree(payload);
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
    OICFree(cborPayload);
}

TEST(CloudResourceTest, CBORPayloadToCloudFULL)
{
    OicCloud_t *cloud =  getCloud();
    uint8_t *payload = NULL;
    size_t size = 0;
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
        ASSERT_EQ(cloudX->session->expireSin, cloud->session->expireSin);
        ASSERT_STREQ(cloudX->session->uid, cloud->session->uid);
#if !defined(__MANDATORY__)
        ASSERT_STREQ(cloudX->session->redirectUri, cloud->session->redirectUri);
#endif // __MANDATORY__
    }

    FreeCloud(cloudX);
    FreeCloud(cloud);
    OICFree(payload);
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
    OICFree(payload);
}

TEST(CloudResourceTest, ValidCloudFULL)
{
    OicCloud_t *cloud =  getCloud();
    ASSERT_TRUE(false == ValidCloud(cloud));
    sprintf(cloud->cis, "coaps+tcp://");
    ASSERT_TRUE(true == ValidCloud(cloud));
    FreeCloud(cloud);
}
