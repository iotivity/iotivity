/******************************************************************
*
* Copyright 2017 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#ifndef __COMMON_TEST_UTIL_H__
#define __COMMON_TEST_UTIL_H__
#include <malloc.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string>
#include <fstream>
#ifdef __GNUC__
#include <ifaddrs.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <ctime>
#include <iostream>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
#include "boost/assign.hpp"
#include "octypes.h"
#include "gtest_custom.h"
#include "gtest/gtest.h"

#define APP_START_STOP_TIMEOUT 2

using namespace std;

class CommonTestUtil
{
    static long s_setUpDynamicMemoryUsage;
    static long s_tearDownDynamicMemoryUsage;
    static long s_memoryDiffSum;


public:

    /**
     * API for common TC set up part
     *
     * @return none
     *
     */
    static void runCommonTCSetUpPart();

    /**
     * API for common TC tear down part
     *
     * @return none
     *
     */
    static void runCommonTCTearDownPart();

    /**
     * API for get current allocated memory
     *
     * @return allocated memory size (byte)
     *
     * NOTE: This API will be support other platforms
     */
    static long getCurrentAllocatedMemory();
};
#endif
