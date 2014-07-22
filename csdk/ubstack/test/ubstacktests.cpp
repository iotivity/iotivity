//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


#include "ocinternalapi.h"
extern "C" {
    #include "logger.h"
    #include "ocstack.h"
    #include "ocstackinternal.h"
}

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


static const char TAG[] = "TestHarness";
static const std::string SERVICE_URI_1 = "coap://127.0.0.1:5683/";
static const std::string SERVICE_URI_2 = "coap://127.0.0.2:5683/";
static const std::string SERVICE_URI_3 = "coap://127.0.0.3:5683/";

void discoverServicesAsync(const oc::ub::OCDiscoverServicesResult& result) {
    OC_LOG(INFO, TAG, "Entering discoverServicesAsync");

    EXPECT_EQ(oc::ub::OCQueryResultType::SUCCESS, result.getResult());

    if (result.getResult() == oc::ub::OCQueryResultType::SUCCESS) {
        OC_LOG(INFO, TAG, "discoverServicesAsync, OCQueryResultType::SUCCESS");

        std::list<std::string> serviceList = result.getServiceList();
        EXPECT_GE((int)serviceList.size(), 1);

        if (serviceList.size() > 0) {
            for (auto iter = serviceList.begin(); iter != serviceList.end(); ++iter) {
                OC_LOG_V(INFO, TAG, "discoverServicesAsync, Service = %s", iter->c_str());
            }
        }
    }
}
//-----------------------------------------------------------------------------
//  Tests
//-----------------------------------------------------------------------------
TEST(UBStackTest, ServiceDiscovery) {
    OC_LOG(INFO, TAG, "Running ServiceDiscovery");

    oc::ub::OCModel::WeakPtr modelWeak = oc::ub::OCModel::createModel();
    oc::ub::OCModel::SharedPtr modelShared = modelWeak.lock();
    EXPECT_TRUE((modelShared != 0));
    if (modelShared) {
        // Start model
        EXPECT_TRUE(modelShared->start("127.0.0.1", 5683, oc::ub::OC_CLIENT));

        // Create an async discovery function
        oc::ub::OCDiscoverServicesFunction serviceFunction = discoverServicesAsync;

        // Discover
        modelShared->discoverServices(serviceFunction);

        // Add service
        EXPECT_EQ(1, (int)modelShared->addService(SERVICE_URI_1));

        // Add service
        EXPECT_EQ(2, (int)modelShared->addService(SERVICE_URI_2));

        // Add service via OC Stack
        OCUpdateResources(SERVICE_URI_3.c_str());
    }
}


