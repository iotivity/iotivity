//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


//=============================================================================
// Includes
//=============================================================================
/*extern "C" {
    #include "occoap.h"
    #include "logger.h"
}*/

#include "gtest/gtest.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <stdint.h>
using namespace std;

//=============================================================================
// Private Variables
//=============================================================================
static const char TAG[] = "TestHarness";

//=============================================================================
// Callback function
//=============================================================================
/*void discoverServicesAsync(const char *Url) {
    OC_LOG_V(INFO, TAG, "Entering discoverServicesAsync.  URL = %s", Url);
}*/

//=============================================================================
// Tests
//=============================================================================
TEST(OCCoapTest, General) {
    EXPECT_EQ(0, 0);
}

/*TEST(OCCoapTest, ServiceDiscovery) {
    OC_LOG(INFO, TAG, "Running ServiceDiscovery");

    EXPECT_EQ(0, OCInitCoAP("127.0.0.1", 0, OC_CLIENT));

    OCCoAPClientCallbackFunction asyncReturnFunc = discoverServicesAsync;
    EXPECT_EQ(0, OCDoCoAPResource(OC_REST_GET, "oc/core/d", asyncReturnFunc));

    EXPECT_EQ(0, OCProcessCoAP());

    EXPECT_EQ(0, OCStopCoAP());
}*/
