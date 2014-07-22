//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


extern "C" {
    #include "ocstack.h"
    #include "ocstackinternal.h"
    #include "logger.h"
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
using namespace std;

#define CTX_VAL 0x99
//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------
static const char TAG[] = "TestHarness";
static const char *SERVICE_URI = "coap://127.0.0.1:5683/";

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------
extern "C"  OCStackApplicationResult asyncDoResourcesCallback(void *ctx, OCClientResponse * clientResponse) {
    OC_LOG(INFO, TAG, "Entering asyncDoResourcesCallback");

    EXPECT_EQ(OC_STACK_OK, clientResponse->result);

    if(ctx == (void*)CTX_VAL) {
        OC_LOG_V(INFO, TAG, "Callback Context recvd successfully");
    }
    OC_LOG_V(INFO, TAG, "result = %d", clientResponse->result);
}

//-----------------------------------------------------------------------------
// Resource creation stuff
//-----------------------------------------------------------------------------
// TODO - remove, temporarily kept for reference purposes
#if 0
const char *LEDrep[] = { "state;b", "power;i", (char *)NULL};
CREATE_RESOURCE_TYPE(LED, "core.led", LEDrep);

// Create the state and power attributes
char stateValue[] = "0";
CREATE_RESOURCE_ATTRIBUTE(stateAttr, "state", stateValue);

char powerValue[] = "5";
CREATE_RESOURCE_ATTRIBUTE(powerAttr, "power", powerValue);

// Define the interface
CREATE_RESOURCE_INTERFACE(LED, "core.rw", OC_REST_GET|OC_REST_PUT);

// Define the resource; the space is allocated;  but the resource has not been fully initialized
CREATE_RESOURCE(LED, "/a/led", &LED_resourcetype, &LED_resourceinterface) ;


// Define the entity handler for the resource
void LED_ehandler(OCEntityHandlerFlag flag, const char *request, char *reponse, OCResourceHandle resource) {
    /* Add code for entity handler */
}
#endif

//-----------------------------------------------------------------------------
//  Tests
//-----------------------------------------------------------------------------
TEST(StackTest, StackInitNullAddr) {
    EXPECT_EQ(OC_STACK_OK, OCInit(0, 5683, OC_SERVER));
}

TEST(StackTest, StackInitNullPort) {
    EXPECT_EQ(OC_STACK_OK, OCInit("127.0.0.1", 0, OC_SERVER));
}

TEST(StackTest, StackInitNullAddrAndPort) {
    EXPECT_EQ(OC_STACK_OK, OCInit(0, 0, OC_SERVER));
}

TEST(StackTest, StackInitInvalidMode) {
    EXPECT_EQ(OC_STACK_ERROR, OCInit(0, 0, (OCMode)10));
}

TEST(StackTest, StackStartSuccessClient) {
    EXPECT_EQ(OC_STACK_OK, OCInit("127.0.0.1", 5683, OC_CLIENT));
}

TEST(StackTest, StackStartSuccessServer) {
    EXPECT_EQ(OC_STACK_OK, OCInit("127.0.0.1", 5683, OC_SERVER));
}

TEST(StackTest, StackStartSuccessClientServer) {
    EXPECT_EQ(OC_STACK_OK, OCInit("127.0.0.1", 5683, OC_CLIENT_SERVER));
}

TEST(StackTest, StackStartSuccessiveInits) {
    EXPECT_EQ(OC_STACK_OK, OCInit("127.0.0.1", 5683, OC_SERVER));
    EXPECT_EQ(OC_STACK_OK, OCInit("127.0.0.2", 5683, OC_SERVER));
}

TEST(StackTest, DoResourceDeviceDiscovery) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";
    OCCallbackData cbData;

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));
    OC_LOG_V(INFO, TAG, "Starting DoResourceDeviceDiscovery test on address %s",addr);

    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_CLIENT));
    /* Start a discovery query*/
    char szQueryUri[64] = { 0 };
    strcpy(szQueryUri, OC_WELL_KNOWN_QUERY);
    cbData.cb = asyncDoResourcesCallback;
    cbData.context = (void*)CTX_VAL;
    EXPECT_EQ(OC_STACK_OK, OCDoResource(OC_REST_GET, szQueryUri, 0, 0, OC_NON_CONFIRMABLE, &cbData));
    //EXPECT_EQ(OC_STACK_OK, OCUpdateResources(SERVICE_URI));
    EXPECT_EQ(OC_STACK_OK, OCStop());
}

TEST(StackTest, StackStopWithoutInit) {
    EXPECT_EQ(OC_STACK_OK, OCInit("127.0.0.1", 5683, OC_CLIENT));
    EXPECT_EQ(OC_STACK_OK, OCStop());
    EXPECT_EQ(OC_STACK_ERROR, OCStop());
}

TEST(StackTest, UpdateResourceNullURI) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";
    OCCallbackData cbData;

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));
    OC_LOG_V(INFO, TAG, "Starting UpdateResourceNullURI test on address %s",addr);

    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_CLIENT));
    /* Start a discovery query*/
    char szQueryUri[64] = { 0 };
    strcpy(szQueryUri, OC_WELL_KNOWN_QUERY);
    cbData.cb = asyncDoResourcesCallback;
    cbData.context = (void*)CTX_VAL;
    EXPECT_EQ(OC_STACK_OK, OCDoResource(OC_REST_GET, szQueryUri, 0, 0, OC_NON_CONFIRMABLE, &cbData));
    //EXPECT_EQ(OC_STACK_INVALID_URI, OCUpdateResources(0));
    EXPECT_EQ(OC_STACK_OK, OCStop());
}

TEST(StackTest, CreateResourceBadParams) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_ERROR, OCCreateResource(NULL, //&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    EXPECT_EQ(OC_STACK_ERROR, OCCreateResource(&handle,
                                            NULL, //"core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    // Method bitmask out of range
    EXPECT_EQ(OC_STACK_ERROR, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            32, //OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    // OC_REST_NOMETHOD
    EXPECT_EQ(OC_STACK_ERROR, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_NOMETHOD, //OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    // Property bitmask out of range
    EXPECT_EQ(OC_STACK_ERROR, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            16));//OC_DISCOVERABLE|OC_OBSERVABLE));
}

TEST(StackTest, CreateResourceSuccess) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    const char *url = OCGetResourceUri(handle);
    EXPECT_STREQ("/a/led", url);
}

TEST(StackTest, CreateResourceFailDuplicateUri) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    const char *url = OCGetResourceUri(handle);
    EXPECT_STREQ("/a/led", url);

    EXPECT_EQ(OC_STACK_ERROR, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
}

TEST(StackTest, CreateResourceMultipleResources) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle1;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle1,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led1",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    OCResourceHandle handle2;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle2,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led2",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    OCResourceHandle handle3;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle3,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led3",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    const char *url = OCGetResourceUri(handle1);
    EXPECT_STREQ("/a/led1", url);

    url = OCGetResourceUri(handle2);
    EXPECT_STREQ("/a/led2", url);

    url = OCGetResourceUri(handle3);
    EXPECT_STREQ("/a/led3", url);

}

TEST(StackTest, CreateResourceBadResoureType) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_ERROR, OCCreateResource(&handle,
                                            NULL, //"core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
}

TEST(StackTest, CreateResourceGoodResourceType) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

}

TEST(StackTest, ResourceTypeName) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    uint8_t numResourceTypes;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceTypes(handle, &numResourceTypes));
    EXPECT_EQ(1, numResourceTypes);
    const char *resourceTypeName = OCGetResourceTypeName(handle, 0);
    EXPECT_STREQ("core.led", resourceTypeName);

    // try getting resource type names with an invalid index
    resourceTypeName = OCGetResourceTypeName(handle, 1);
    EXPECT_STREQ(NULL, resourceTypeName);
    // try getting resource type names with an invalid index
    resourceTypeName = OCGetResourceTypeName(handle, 10);
    EXPECT_STREQ(NULL, resourceTypeName);
}

TEST(StackTest, BindResourceTypeNameBad) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    uint8_t numResourceTypes;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceTypes(handle, &numResourceTypes));
    EXPECT_EQ(1, numResourceTypes);
    const char *resourceTypeName = OCGetResourceTypeName(handle, 0);
    EXPECT_STREQ("core.led", resourceTypeName);

    EXPECT_EQ(OC_STACK_ERROR, OCBindResourceTypeToResource(handle, NULL, "state:oc.bt.b;power:oc.bt.i"));
}

TEST(StackTest, BindResourceTypeNameGood) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    uint8_t numResourceTypes;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceTypes(handle, &numResourceTypes));
    EXPECT_EQ(1, numResourceTypes);
    const char *resourceTypeName = OCGetResourceTypeName(handle, 0);
    EXPECT_STREQ("core.led", resourceTypeName);

    EXPECT_EQ(OC_STACK_OK, OCBindResourceTypeToResource(handle, "core.brightled", "state:oc.bt.b;power:oc.bt.i"));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceTypes(handle, &numResourceTypes));
    EXPECT_EQ(2, numResourceTypes);
    resourceTypeName = OCGetResourceTypeName(handle, 1);
    EXPECT_STREQ("core.brightled", resourceTypeName);

    EXPECT_EQ(OC_STACK_OK, OCBindResourceTypeToResource(handle, "core.reallybrightled", "state:oc.bt.b;power:oc.bt.i"));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceTypes(handle, &numResourceTypes));
    EXPECT_EQ(3, numResourceTypes);
    resourceTypeName = OCGetResourceTypeName(handle, 2);
    EXPECT_STREQ("core.reallybrightled", resourceTypeName);

}

TEST(StackTest, ResourceTypeAttrRepresentation) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    uint8_t numResourceTypes;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceTypes(handle, &numResourceTypes));
    EXPECT_EQ(1, numResourceTypes);
    const char *resourceTypeAttrRepresentation = OCGetResourceAttributeRepresentation(handle, 0);
    EXPECT_STREQ("state:oc.bt.b;power:oc.bt.i", resourceTypeAttrRepresentation);

    // try getting resource type attribute representations with an invalid index
    resourceTypeAttrRepresentation = OCGetResourceAttributeRepresentation(handle, 1);
    EXPECT_STREQ(NULL, resourceTypeAttrRepresentation);
    // try getting resource type names with an invalid index
    resourceTypeAttrRepresentation = OCGetResourceAttributeRepresentation(handle, 10);
    EXPECT_STREQ(NULL, resourceTypeAttrRepresentation);
}

TEST(StackTest, BindResourceTypeAttribRepGood) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    uint8_t numResourceTypes;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceTypes(handle, &numResourceTypes));
    EXPECT_EQ(1, numResourceTypes);
    const char *resourceTypeAttrRepresentation = OCGetResourceAttributeRepresentation(handle, 0);
    EXPECT_STREQ("state:oc.bt.b", resourceTypeAttrRepresentation);

    EXPECT_EQ(OC_STACK_OK, OCBindResourceTypeToResource(handle, "core.brightled", "state:oc.bt.b;power:oc.bt.i"));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceTypes(handle, &numResourceTypes));
    EXPECT_EQ(2, numResourceTypes);
    resourceTypeAttrRepresentation = OCGetResourceAttributeRepresentation(handle, 1);
    EXPECT_STREQ("state:oc.bt.b;power:oc.bt.i", resourceTypeAttrRepresentation);

    EXPECT_EQ(OC_STACK_OK, OCBindResourceTypeToResource(handle, "core.reallybrightled", "state:oc.bt.b;power:oc.bt.i;temp:oc.bt.i"));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceTypes(handle, &numResourceTypes));
    EXPECT_EQ(3, numResourceTypes);
    resourceTypeAttrRepresentation = OCGetResourceAttributeRepresentation(handle, 2);
    EXPECT_STREQ("state:oc.bt.b;power:oc.bt.i;temp:oc.bt.i", resourceTypeAttrRepresentation);
}

TEST(StackTest, ResourceTypeInterface) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    uint8_t numResourceInterfaces;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceInterfaces(handle, &numResourceInterfaces));
    EXPECT_EQ(1, numResourceInterfaces);
    const char *resourceInterfaceName = OCGetResourceInterfaceName(handle, 0);
    EXPECT_STREQ("core.rw", resourceInterfaceName);

    // try getting resource interface names with an invalid index
    resourceInterfaceName = OCGetResourceInterfaceName(handle, 1);
    EXPECT_STREQ(NULL, resourceInterfaceName);
    // try getting resource interface names with an invalid index
    resourceInterfaceName = OCGetResourceInterfaceName(handle, 10);
    EXPECT_STREQ(NULL, resourceInterfaceName);
}

TEST(StackTest, BindResourceInterfaceNameBad) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    uint8_t numResourceInterfaces;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceInterfaces(handle, &numResourceInterfaces));
    EXPECT_EQ(1, numResourceInterfaces);
    const char *resourceInterfaceName = OCGetResourceInterfaceName(handle, 0);
    EXPECT_STREQ("core.rw", resourceInterfaceName);

    EXPECT_EQ(OC_STACK_ERROR, OCBindResourceInterfaceToResource(handle, NULL, OC_REST_GET|OC_REST_PUT));
}

TEST(StackTest, BindResourceInterfaceNameGood) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    uint8_t numResourceInterfaces;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceInterfaces(handle, &numResourceInterfaces));
    EXPECT_EQ(1, numResourceInterfaces);
    const char *resourceInterfaceName = OCGetResourceInterfaceName(handle, 0);
    EXPECT_STREQ("core.rw", resourceInterfaceName);

    EXPECT_EQ(OC_STACK_OK, OCBindResourceInterfaceToResource(handle, "core.r", OC_REST_GET|OC_REST_PUT));

    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceInterfaces(handle, &numResourceInterfaces));
    EXPECT_EQ(2, numResourceInterfaces);
    resourceInterfaceName = OCGetResourceInterfaceName(handle, 1);
    EXPECT_STREQ("core.r", resourceInterfaceName);
}

TEST(StackTest, ResourceTypeInterfaceMethods) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    uint8_t numResourceInterfaces;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceInterfaces(handle, &numResourceInterfaces));
    EXPECT_EQ(1, numResourceInterfaces);
    uint8_t resourceInterfaceMethods = OCGetResourceInterfaceAllowedMethods(handle, 0);
    EXPECT_EQ(OC_REST_GET|OC_REST_PUT, resourceInterfaceMethods);

    // try getting resource interface methods with an invalid index
    resourceInterfaceMethods = OCGetResourceInterfaceAllowedMethods(handle, 1);
    EXPECT_EQ(0, resourceInterfaceMethods);
    // try getting resource interface methods with an invalid index
    resourceInterfaceMethods = OCGetResourceInterfaceAllowedMethods(handle, 10);
    EXPECT_EQ(0, resourceInterfaceMethods);
}

TEST(StackTest, BindResourceInterfaceMethodsBad) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    uint8_t numResourceInterfaces;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceInterfaces(handle, &numResourceInterfaces));
    EXPECT_EQ(1, numResourceInterfaces);
    uint8_t resourceInterfaceMethods = OCGetResourceInterfaceAllowedMethods(handle, 0);
    EXPECT_EQ(OC_REST_GET|OC_REST_PUT, resourceInterfaceMethods);

    EXPECT_EQ(OC_STACK_ERROR, OCBindResourceInterfaceToResource(handle, "core.rw", OC_REST_NOMETHOD));
}

TEST(StackTest, BindResourceInterfaceMethodsGood) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    uint8_t numResourceInterfaces;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceInterfaces(handle, &numResourceInterfaces));
    EXPECT_EQ(1, numResourceInterfaces);
    uint8_t resourceInterfaceMethods = OCGetResourceInterfaceAllowedMethods(handle, 0);
    EXPECT_EQ(OC_REST_GET|OC_REST_PUT, resourceInterfaceMethods);

    EXPECT_EQ(OC_STACK_OK, OCBindResourceInterfaceToResource(handle, "core.r", OC_REST_PUT));

    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceInterfaces(handle, &numResourceInterfaces));
    EXPECT_EQ(2, numResourceInterfaces);
    resourceInterfaceMethods = OCGetResourceInterfaceAllowedMethods(handle, 1);
    EXPECT_EQ(OC_REST_PUT, resourceInterfaceMethods);
}

TEST(StackTest, BindContainedResourceBad) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle containerHandle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&containerHandle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/kitchen",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    OCResourceHandle handle0;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle0,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    EXPECT_EQ(OC_STACK_ERROR, OCBindContainedResourceToResource(containerHandle, containerHandle));

    OCResourceHandle handle1;
    EXPECT_EQ(OC_STACK_ERROR, OCBindContainedResourceToResource(handle1, handle0));
}

TEST(StackTest, BindContainedResourceGood) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    uint8_t numResources = 0;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(0, numResources);

    OCResourceHandle containerHandle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&containerHandle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/kitchen",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(1, numResources);

    OCResourceHandle handle0;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle0,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led0",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(2, numResources);

    OCResourceHandle handle1;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle1,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led1",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(3, numResources);

    OCResourceHandle handle2;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle2,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led2",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(4, numResources);

    OCResourceHandle handle3;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle3,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led3",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(5, numResources);

    OCResourceHandle handle4;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle4,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led4",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(6, numResources);

    OCResourceHandle handle5;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle5,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led5",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(7, numResources);


    EXPECT_EQ(OC_STACK_OK, OCBindContainedResourceToResource(containerHandle, handle0));
    EXPECT_EQ(OC_STACK_OK, OCBindContainedResourceToResource(containerHandle, handle1));
    EXPECT_EQ(OC_STACK_OK, OCBindContainedResourceToResource(containerHandle, handle2));
    EXPECT_EQ(OC_STACK_OK, OCBindContainedResourceToResource(containerHandle, handle3));
    EXPECT_EQ(OC_STACK_OK, OCBindContainedResourceToResource(containerHandle, handle4));
    EXPECT_EQ(OC_STACK_ERROR, OCBindContainedResourceToResource(containerHandle, handle5));

    EXPECT_EQ(handle0, OCGetContainedResourceHandle(containerHandle, 0));
    EXPECT_EQ(handle1, OCGetContainedResourceHandle(containerHandle, 1));
    EXPECT_EQ(handle2, OCGetContainedResourceHandle(containerHandle, 2));
    EXPECT_EQ(handle3, OCGetContainedResourceHandle(containerHandle, 3));
    EXPECT_EQ(handle4, OCGetContainedResourceHandle(containerHandle, 4));

    EXPECT_EQ(NULL, OCGetContainedResourceHandle(containerHandle, 5));
}

TEST(StackTest, GetResourceByIndex) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    uint8_t numResources = 0;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(0, numResources);

    OCResourceHandle containerHandle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&containerHandle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/kitchen",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(1, numResources);

    OCResourceHandle handle0;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle0,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led0",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(2, numResources);

    OCResourceHandle handle1;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle1,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led1",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(3, numResources);

    OCResourceHandle handle2;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle2,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led2",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(4, numResources);

    OCResourceHandle handle3;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle3,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led3",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(5, numResources);

    OCResourceHandle handle4;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle4,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led4",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(6, numResources);

    OCResourceHandle handle5;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle5,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led5",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(7, numResources);

    EXPECT_EQ(containerHandle, OCGetResourceHandle(0));
    EXPECT_EQ(handle0, OCGetResourceHandle(1));
    EXPECT_EQ(handle1, OCGetResourceHandle(2));
    EXPECT_EQ(handle2, OCGetResourceHandle(3));
    EXPECT_EQ(handle3, OCGetResourceHandle(4));
    EXPECT_EQ(handle4, OCGetResourceHandle(5));
    EXPECT_EQ(handle5, OCGetResourceHandle(6));

}

TEST(StackTest, BindEntityHandlerBad) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    EXPECT_EQ(OC_STACK_ERROR, OCBindResourceHandler(handle, NULL));
    EXPECT_EQ(OC_STACK_ERROR, OCBindResourceHandler(NULL, NULL));
}


void entityHandler(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest) {
    OC_LOG(INFO, TAG, "Entering entityHandler");
}

TEST(StackTest, BindEntityHandlerGood) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    OCEntityHandler myHandler = entityHandler;

    EXPECT_EQ(OC_STACK_OK, OCBindResourceHandler(handle, myHandler));

    EXPECT_EQ(myHandler, OCGetResourceHandler(handle));
}

TEST(StackTest, DeleteHeadResource) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    uint8_t numResources = 0;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(0, numResources);

    OCResourceHandle handle0;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle0,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led0",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(1, numResources);

    EXPECT_EQ(OC_STACK_OK, OCDeleteResource(handle0));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(0, numResources);
}

TEST(StackTest, DeleteHeadResource2) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    uint8_t numResources = 0;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(0, numResources);

    OCResourceHandle handle0;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle0,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led0",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(1, numResources);

    OCResourceHandle handle1;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle1,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led1",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(2, numResources);

    EXPECT_EQ(OC_STACK_OK, OCDeleteResource(handle0));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(1, numResources);

    EXPECT_EQ(handle1, OCGetResourceHandle(0));
}


TEST(StackTest, DeleteLastResource) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    uint8_t numResources = 0;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(0, numResources);

    OCResourceHandle handle0;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle0,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led0",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(1, numResources);

    OCResourceHandle handle1;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle1,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led1",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(2, numResources);

    EXPECT_EQ(OC_STACK_OK, OCDeleteResource(handle1));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(1, numResources);

    EXPECT_EQ(handle0, OCGetResourceHandle(0));

    OCResourceHandle handle2;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle2,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led2",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(2, numResources);
}

TEST(StackTest, DeleteMiddleResource) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    uint8_t numResources = 0;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(0, numResources);

    OCResourceHandle handle0;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle0,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led0",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(1, numResources);

    OCResourceHandle handle1;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle1,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led1",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(2, numResources);

    OCResourceHandle handle2;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle2,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led2",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(3, numResources);

    EXPECT_EQ(OC_STACK_OK, OCDeleteResource(handle1));
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(2, numResources);

    EXPECT_EQ(handle0, OCGetResourceHandle(0));
    EXPECT_EQ(handle2, OCGetResourceHandle(1));

    // Make sure the resource elements are still correct
    uint8_t numResourceInterfaces;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResourceInterfaces(handle2, &numResourceInterfaces));
    EXPECT_EQ(1, numResourceInterfaces);
    const char *resourceInterfaceName = OCGetResourceInterfaceName(handle2, 0);
    EXPECT_STREQ("core.rw", resourceInterfaceName);
}

TEST(StackTest, GetResourceProperties) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));

    EXPECT_EQ(OC_ACTIVE|OC_DISCOVERABLE|OC_OBSERVABLE, OCGetResourceProperties(handle));
    EXPECT_EQ(OC_STACK_OK, OCDeleteResource(handle));
}

TEST(StackTest, StackTestResourceDiscoverOneResourceBad) {
    // Setting URI to null which is an error
    char uri[] = "";
    char query[] = "";
    char req[1024] = {};
    char rsp[1024] = {};
    //OCServerRequestResult res;

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a1/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    const char *url = OCGetResourceUri(handle);
    EXPECT_STREQ("/a1/led", url);

    //EXPECT_EQ(OC_STACK_INVALID_URI, OCHandleServerRequest(&res, uri, query, req, rsp));
    EXPECT_EQ(OC_STACK_OK, OCDeleteResource(handle));
    uint8_t numResources = 0;
    EXPECT_EQ(OC_STACK_OK, OCGetNumberOfResources(&numResources));
    EXPECT_EQ(0, numResources);

}

TEST(StackTest, StackTestResourceDiscoverOneResource) {
    char uri[] = "/oc/core";
    char query[] = "";
    char req[1024] = {};
    char rsp[1024] = {};
    //OCServerRequestResult res;

    OCResourceHandle handle;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    const char *url = OCGetResourceUri(handle);
    EXPECT_STREQ("/a/led", url);

    //EXPECT_EQ(OC_STACK_OK, OCHandleServerRequest(&res, uri, query, req, rsp));
    EXPECT_EQ(OC_STACK_OK, OCDeleteResource(handle));
}

TEST(StackTest, StackTestResourceDiscoverManyResources) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";
    char uri[] = "/oc/core";
    char query[] = "";
    char req[1024] = {};
    char rsp[1024] = {};
    //OCServerRequestResult res;

    /*Get Ip address on defined interface and initialize coap on it with random port number
     * this port number will be used as a source port in all coap communications*/
    OCGetInterfaceAddress(ifname, sizeof(ifname), AF_INET, addr, sizeof(addr));

    OC_LOG_V(INFO, TAG, "Starting ocserver on address %s:%d",addr,port);
    EXPECT_EQ(OC_STACK_OK, OCInit((char *) addr, port, OC_SERVER));

    OCResourceHandle handle1;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle1,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led1",
                                            0,
                                            OC_DISCOVERABLE));
    const char *url = OCGetResourceUri(handle1);
    EXPECT_STREQ("/a/led1", url);

    OCResourceHandle handle2;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle2,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led2",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    url = OCGetResourceUri(handle2);
    EXPECT_STREQ("/a/led2", url);

    EXPECT_EQ(OC_STACK_OK, OCBindResourceTypeToResource(handle2, "core.brightled",
                                                        "state:oc.bt.b;power:oc.bt.i"));
    EXPECT_EQ(OC_STACK_OK, OCBindResourceTypeToResource(handle2, "core.colorled",
                                                        "state:oc.bt.b;power:oc.bt.i"));

    OCResourceHandle handle3;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle3,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led3",
                                            0,
                                            OC_DISCOVERABLE|OC_OBSERVABLE));
    url = OCGetResourceUri(handle3);
    EXPECT_STREQ("/a/led3", url);

    EXPECT_EQ(OC_STACK_OK, OCBindResourceInterfaceToResource(handle3, "oc.mi.ll",
                                                             OC_REST_GET|OC_REST_PUT));
    EXPECT_EQ(OC_STACK_OK, OCBindResourceInterfaceToResource(handle3, "oc.mi.b",
                                                             OC_REST_GET|OC_REST_PUT));

    OCResourceHandle handle4;
    EXPECT_EQ(OC_STACK_OK, OCCreateResource(&handle4,
                                            "core.led",
                                            "state:oc.bt.b;power:oc.bt.i",
                                            "core.rw",
                                            OC_REST_GET|OC_REST_PUT,
                                            "/a/led4",
                                            0,
                                            OC_DISCOVERABLE));
    url = OCGetResourceUri(handle4);
    EXPECT_STREQ("/a/led4", url);

    EXPECT_EQ(OC_STACK_OK, OCBindResourceTypeToResource(handle4, "core.brightled",
                                                        "state:oc.bt.b;power:oc.bt.i"));
    EXPECT_EQ(OC_STACK_OK, OCBindResourceInterfaceToResource(handle4, "oc.mi.ll",
                                                             OC_REST_GET|OC_REST_PUT));
    EXPECT_EQ(OC_STACK_OK, OCBindResourceInterfaceToResource(handle4, "oc.mi.b",
                                                             OC_REST_GET|OC_REST_PUT));

    //EXPECT_EQ(OC_STACK_OK, OCHandleServerRequest(&res, uri, query, req, rsp));
}

TEST(StackTest, StackTestResourceDiscoverIfFilteringBad) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";
    char uri[] = "/oc/core";
    char query[] = "if";
    char req[1024] = {};
    char rsp[1024] = {};
    //OCServerRequestResult res;

    //EXPECT_EQ(OC_STACK_INVALID_QUERY, OCHandleServerRequest(&res, uri, query, req, rsp));
}

TEST(StackTest, StackTestResourceDiscoverRtFilteringBad) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";
    char uri[] = "/oc/core";
    char query[] = "rt";
    char req[1024] = {};
    char rsp[1024] = {};
    //OCServerRequestResult res;

    //EXPECT_EQ(OC_STACK_INVALID_QUERY, OCHandleServerRequest(&res, uri, query, req, rsp));
}
TEST(StackTest, StackTestResourceDiscoverIfFiltering) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";
    char uri[] = "/oc/core";
    char query[] = "if=oc.mi.ll";
    char req[1024] = {};
    char rsp[1024] = {};
    //OCServerRequestResult res;

    //EXPECT_EQ(OC_STACK_OK, OCHandleServerRequest(&res, uri, query, req, rsp));
}

TEST(StackTest, StackTestResourceDiscoverRtFiltering) {
    uint8_t addr[20];
    uint16_t port = USE_RANDOM_PORT;
    uint8_t ifname[] = "eth0";
    char uri[] = "/oc/core";
    char query[] = "rt=core.brightled";
    char req[1024] = {};
    char rsp[1024] = {};
    //OCServerRequestResult res;

    //EXPECT_EQ(OC_STACK_OK, OCHandleServerRequest(&res, uri, query, req, rsp));
}
