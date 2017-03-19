//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

extern "C"
{
    #include "rd_server.h"
    #include "rd_client.h"
    #include "rd_database.h"
    #include "ocstack.h"
    #include "logger.h"
    #include "oic_malloc.h"
    #include "oic_string.h"
    #include "ocpayload.h"
    #include "payload_logging.h"
}

#include "gtest/gtest.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <stdint.h>

#include "gtest_helper.h"

using namespace std;

namespace itst = iotivity::test;

#define DEFAULT_CONTEXT_VALUE 0x99

//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------
#define TAG "RDDatabaseTests"

std::chrono::seconds const SHORT_TEST_TIMEOUT = std::chrono::seconds(5);

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Tests
//-----------------------------------------------------------------------------
class RDDatabaseTests : public testing::Test {
    protected:
    virtual void SetUp()
    {
        remove("RD.db");
        OCInit("127.0.0.1", 5683, OC_CLIENT_SERVER);
        EXPECT_EQ(OC_STACK_OK, OCRDDatabaseInit());
    }

    virtual void TearDown()
    {
        EXPECT_EQ(OC_STACK_OK, OCRDDatabaseClose());
        OCStop();
    }
};

typedef struct Resource
{
    const char *uri;
    const char *rt;
    const char *itf;
    const char *mt;
    uint8_t bm;
} Resource;

static OCRepPayload *CreateRDPublishPayload(const char *deviceId,
                                            Resource *resources,
                                            size_t nresources)
{
    OCRepPayload *repPayload = OCRepPayloadCreate();
    EXPECT_TRUE(repPayload != NULL);
    EXPECT_TRUE(deviceId != NULL);
    EXPECT_TRUE(OCRepPayloadSetPropString(repPayload, OC_RSRVD_DEVICE_ID, deviceId));
    EXPECT_TRUE(OCRepPayloadSetPropInt(repPayload, OC_RSRVD_DEVICE_TTL, 86400));

    const OCRepPayload *linkArr[nresources];
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = {nresources, 0, 0};

    for (size_t i = 0; i < nresources; ++i)
    {
        OCRepPayload *link = OCRepPayloadCreate();
        OCRepPayloadSetPropString(link, OC_RSRVD_HREF, resources[i].uri);
        size_t rtDim[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
        char **rt = (char **)OICMalloc(sizeof(char *) * 1);
        rt[0] = OICStrdup(resources[i].rt);
        OCRepPayloadSetStringArray(link, OC_RSRVD_RESOURCE_TYPE, (const char **)rt,
                                   rtDim);
        size_t itfDim[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
        char **itf = (char **)OICMalloc(sizeof(char *) * 1);
        itf[0] = OICStrdup(resources[i].itf);
        OCRepPayloadSetStringArray(link, OC_RSRVD_INTERFACE, (const char **)itf,
                                   itfDim);
        size_t mtDim[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
        char **mt = (char **)OICMalloc(sizeof(char *) * 1);
        mt[0] = OICStrdup(resources[i].mt);
        OCRepPayloadSetStringArray(link, OC_RSRVD_MEDIA_TYPE, (const char **)mt,
                                   mtDim);
        OCRepPayload *policy = OCRepPayloadCreate();
        OCRepPayloadSetPropInt(policy, OC_RSRVD_BITMAP, resources[i].bm);
        OCRepPayloadSetPropObjectAsOwner(link, OC_RSRVD_POLICY, policy);
        linkArr[i] = link;
    }

    OCRepPayloadSetPropObjectArray(repPayload, OC_RSRVD_LINKS, linkArr, dimensions);

    OIC_LOG_PAYLOAD(DEBUG, (OCPayload *)repPayload);
    return repPayload;
}

static OCRepPayload *CreateResources(const char *deviceId)
{
    Resource resources[] = {
        { "/a/thermostat", "core.thermostat", OC_RSRVD_INTERFACE_DEFAULT, OC_MEDIA_TYPE_APPLICATION_JSON, OC_DISCOVERABLE },
        { "/a/light", "core.light", OC_RSRVD_INTERFACE_DEFAULT, OC_MEDIA_TYPE_APPLICATION_JSON, OC_DISCOVERABLE }
    };
    return CreateRDPublishPayload(deviceId, resources, 2);
}

TEST_F(RDDatabaseTests, Create)
{
    itst::DeadmanTimer killSwitch(SHORT_TEST_TIMEOUT);
}

TEST_F(RDDatabaseTests, StoreResources)
{
    itst::DeadmanTimer killSwitch(SHORT_TEST_TIMEOUT);
    const char *deviceId = "7a960f46-a52e-4837-bd83-460b1a6dd56b";
    OCRepPayload *repPayload = CreateResources(deviceId);
    OCDevAddr address;
    address.port = 54321;
    OICStrcpy(address.addr, MAX_ADDR_STR_SIZE, "192.168.1.1");

    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseStoreResources(repPayload, &address));

    OCDiscoveryPayload *discPayload = NULL;
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseDiscoveryPayloadCreate(NULL, "core.light", &discPayload));
    OCDiscoveryPayloadDestroy(discPayload);
    discPayload = NULL;
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseDiscoveryPayloadCreate(OC_RSRVD_INTERFACE_DEFAULT, NULL, &discPayload));
    OCDiscoveryPayloadDestroy(discPayload);
    discPayload = NULL;
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseDiscoveryPayloadCreate(NULL, "core.light", &discPayload));
    OCDiscoveryPayloadDestroy(discPayload);
    discPayload = NULL;
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseDiscoveryPayloadCreate(OC_RSRVD_INTERFACE_DEFAULT, "core.light", &discPayload));
    OCDiscoveryPayloadDestroy(discPayload);
    discPayload = NULL;

    OCPayloadDestroy((OCPayload *)repPayload);
}

TEST_F(RDDatabaseTests, AddResources)
{
    itst::DeadmanTimer killSwitch(SHORT_TEST_TIMEOUT);
    const char *deviceId = "7a960f46-a52e-4837-bd83-460b1a6dd56b";
    OCDevAddr address;
    address.port = 54321;
    OICStrcpy(address.addr, MAX_ADDR_STR_SIZE, "192.168.1.1");

    OCRepPayload *repPayload = CreateResources(deviceId);
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseStoreResources(repPayload, &address));
    OCPayloadDestroy((OCPayload *)repPayload);

    Resource resources[] = {
        { "/a/light2", "core.light", OC_RSRVD_INTERFACE_DEFAULT, OC_MEDIA_TYPE_APPLICATION_JSON, OC_DISCOVERABLE }
    };
    repPayload = CreateRDPublishPayload(deviceId, resources, 1);
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseStoreResources(repPayload, &address));
    OCPayloadDestroy((OCPayload *)repPayload);

    OCDiscoveryPayload *discPayload = NULL;
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseDiscoveryPayloadCreate(OC_RSRVD_INTERFACE_LL, NULL, &discPayload));
    bool foundThermostat = false;
    bool foundLight = false;
    bool foundLight2 = false;
    for (OCResourcePayload *resource = discPayload->resources; resource; resource = resource->next)
    {
        if (!strcmp("/a/thermostat", resource->uri))
        {
            foundThermostat = true;
        }
        else if (!strcmp("/a/light", resource->uri))
        {
            foundLight = true;
        }
        else if (!strcmp("/a/light2", resource->uri))
        {
            foundLight2 = true;
        }
    }
    EXPECT_TRUE(foundThermostat);
    EXPECT_TRUE(foundLight);
    EXPECT_TRUE(foundLight2);
    OCDiscoveryPayloadDestroy(discPayload);
    discPayload = NULL;

    OCPayloadDestroy((OCPayload *)repPayload);
}

TEST_F(RDDatabaseTests, UpdateResources)
{
    itst::DeadmanTimer killSwitch(SHORT_TEST_TIMEOUT);
    const char *deviceId = "7a960f46-a52e-4837-bd83-460b1a6dd56b";
    OCDevAddr address;
    address.port = 54321;
    OICStrcpy(address.addr, MAX_ADDR_STR_SIZE, "192.168.1.1");

    OCRepPayload *repPayload = CreateResources(deviceId);
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseStoreResources(repPayload, &address));
    OCPayloadDestroy((OCPayload *)repPayload);

    Resource resources[] = {
        { "/a/thermostat", "x.core.r.thermostat", "x.core.if.thermostat", "application/cbor", OC_DISCOVERABLE | OC_OBSERVABLE },
        { "/a/light", "x.core.r.light", "x.core.if.light", "application/cbor", OC_DISCOVERABLE | OC_OBSERVABLE }
    };
    repPayload = CreateRDPublishPayload(deviceId, resources, 2);
    address.port = 12345;
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseStoreResources(repPayload, &address));
    OCPayloadDestroy((OCPayload *)repPayload);

    OCDiscoveryPayload *discPayload = NULL;
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseDiscoveryPayloadCreate(OC_RSRVD_INTERFACE_LL, NULL, &discPayload));
    EXPECT_STREQ("192.168.1.1:12345", discPayload->baseURI);
    bool foundThermostat = false;
    bool foundLight = false;
    for (OCResourcePayload *resource = discPayload->resources; resource; resource = resource->next)
    {
        if (!strcmp("/a/thermostat", resource->uri))
        {
            foundThermostat = true;
            EXPECT_STREQ("x.core.r.thermostat", resource->types->value);
            EXPECT_TRUE(resource->types->next == NULL);
            EXPECT_STREQ("x.core.if.thermostat", resource->interfaces->value);
            EXPECT_TRUE(resource->interfaces->next == NULL);
            EXPECT_EQ(OC_DISCOVERABLE | OC_OBSERVABLE, resource->bitmap);
        }
        else if (!strcmp("/a/light", resource->uri))
        {
            foundLight = true;
            EXPECT_STREQ("x.core.r.light", resource->types->value);
            EXPECT_TRUE(resource->types->next == NULL);
            EXPECT_STREQ("x.core.if.light", resource->interfaces->value);
            EXPECT_TRUE(resource->interfaces->next == NULL);
            EXPECT_EQ(OC_DISCOVERABLE | OC_OBSERVABLE, resource->bitmap);
        }
    }
    EXPECT_TRUE(foundThermostat);
    EXPECT_TRUE(foundLight);
    OCDiscoveryPayloadDestroy(discPayload);
    discPayload = NULL;

    OCPayloadDestroy((OCPayload *)repPayload);
}

TEST_F(RDDatabaseTests, AddAndUpdateResources)
{
    itst::DeadmanTimer killSwitch(SHORT_TEST_TIMEOUT);
    const char *deviceId = "7a960f46-a52e-4837-bd83-460b1a6dd56b";
    OCDevAddr address;
    address.port = 54321;
    OICStrcpy(address.addr, MAX_ADDR_STR_SIZE, "192.168.1.1");

    OCRepPayload *repPayload = CreateResources(deviceId);
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseStoreResources(repPayload, &address));
    OCPayloadDestroy((OCPayload *)repPayload);

    Resource resources[] = {
        { "/a/thermostat", "x.core.r.thermostat", "x.core.if.thermostat", "application/cbor", OC_DISCOVERABLE | OC_OBSERVABLE },
        { "/a/light", "x.core.r.light", "x.core.if.light", "application/cbor", OC_DISCOVERABLE | OC_OBSERVABLE },
        { "/a/light2", "core.light", OC_RSRVD_INTERFACE_DEFAULT, OC_MEDIA_TYPE_APPLICATION_JSON, OC_DISCOVERABLE }
    };
    repPayload = CreateRDPublishPayload(deviceId, resources, 3);
    address.port = 12345;
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseStoreResources(repPayload, &address));
    OCPayloadDestroy((OCPayload *)repPayload);

    OCDiscoveryPayload *discPayload = NULL;
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseDiscoveryPayloadCreate(OC_RSRVD_INTERFACE_LL, NULL, &discPayload));
    EXPECT_STREQ("192.168.1.1:12345", discPayload->baseURI);
    bool foundThermostat = false;
    bool foundLight = false;
    bool foundLight2 = false;
    for (OCResourcePayload *resource = discPayload->resources; resource; resource = resource->next)
    {
        if (!strcmp("/a/thermostat", resource->uri))
        {
            foundThermostat = true;
            EXPECT_STREQ("x.core.r.thermostat", resource->types->value);
            EXPECT_TRUE(resource->types->next == NULL);
            EXPECT_STREQ("x.core.if.thermostat", resource->interfaces->value);
            EXPECT_TRUE(resource->interfaces->next == NULL);
            EXPECT_EQ(OC_DISCOVERABLE | OC_OBSERVABLE, resource->bitmap);
        }
        else if (!strcmp("/a/light", resource->uri))
        {
            foundLight = true;
            EXPECT_STREQ("x.core.r.light", resource->types->value);
            EXPECT_TRUE(resource->types->next == NULL);
            EXPECT_STREQ("x.core.if.light", resource->interfaces->value);
            EXPECT_TRUE(resource->interfaces->next == NULL);
            EXPECT_EQ(OC_DISCOVERABLE | OC_OBSERVABLE, resource->bitmap);
        }
        else if (!strcmp("/a/light2", resource->uri))
        {
            foundLight2 = true;
            EXPECT_STREQ("core.light", resource->types->value);
            EXPECT_TRUE(resource->types->next == NULL);
            EXPECT_STREQ(OC_RSRVD_INTERFACE_DEFAULT, resource->interfaces->value);
            EXPECT_TRUE(resource->interfaces->next == NULL);
            EXPECT_EQ(OC_DISCOVERABLE, resource->bitmap);
        }
    }
    EXPECT_TRUE(foundThermostat);
    EXPECT_TRUE(foundLight);
    EXPECT_TRUE(foundLight2);
    OCDiscoveryPayloadDestroy(discPayload);
    discPayload = NULL;

    OCPayloadDestroy((OCPayload *)repPayload);
}

TEST_F(RDDatabaseTests, DeleteResourcesDevice)
{
    itst::DeadmanTimer killSwitch(SHORT_TEST_TIMEOUT);
    const char *deviceIds[2] =
    {
        "7a960f46-a52e-4837-bd83-460b1a6dd56b",
        "983656a7-c7e5-49c2-a201-edbeb7606fb5",
    };
    OCRepPayload *payloads[2];
    payloads[0] = CreateResources(deviceIds[0]);
    payloads[1] = CreateResources(deviceIds[1]);
    OCDevAddr address;
    address.port = 54321;
    OICStrcpy(address.addr, MAX_ADDR_STR_SIZE, "192.168.1.1");
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseStoreResources(payloads[0], &address));
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseStoreResources(payloads[1], &address));

    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseDeleteResources(deviceIds[0], NULL, 0));

    OCDiscoveryPayload *discPayload = NULL;
    EXPECT_EQ(OC_STACK_OK, OCRDDatabaseDiscoveryPayloadCreate(OC_RSRVD_INTERFACE_LL, NULL, &discPayload));
    bool found0 = false;
    bool found1 = false;
    for (OCDiscoveryPayload *payload = discPayload; payload; payload = payload->next)
    {
        if (!strcmp((const char *) deviceIds[0], payload->sid))
        {
            found0 = true;
        }
        if (!strcmp((const char *) deviceIds[1], payload->sid))
        {
            found1 = true;
        }
    }
    EXPECT_FALSE(found0);
    EXPECT_TRUE(found1);
    OCDiscoveryPayloadDestroy(discPayload);
    discPayload = NULL;

    OCPayloadDestroy((OCPayload *)payloads[0]);
    OCPayloadDestroy((OCPayload *)payloads[1]);
}
