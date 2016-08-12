/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef __COMMON_UTIL_H__
#define __COMMON_UTIL_H__
#include <malloc.h>
#include <unistd.h>
#include <string>
#include <fstream>
#ifndef __ANDROID__
#include <ifaddrs.h>
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

using namespace std;

typedef enum
{
    IPv4 = 0, IPv6
} InternetProtocolVersion;

class CommonUtil
{
    static long s_setUpDynamicMemoryUsage;
    static long s_tearDownDynamicMemoryUsage;
    static long s_memoryDiffSum;


public:

    static map<OCStackResult, string> s_OCStackResultString;
    /**
     * API for get time stamp
     *
     * @return const char*
     *
     */
    static const char* GetTimeStampString();
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
     * API for launch App
     *
     *
     * @param app - App binary path.
     *
     * @return none
     *
     * NOTE: This API will be support other platforms
     */
    static void launchApp(std::string app);

    /**
     * API for kill App
     *
     *
     * @param app - App process name.
     *
     * @return none
     *
     * NOTE: This API will be support other platforms
     */
    static void killApp(std::string app);

    /**
     * API for make directory
     *
     *
     * @param app - App binary path.
     *
     * @return none
     *
     * NOTE: This API will be support other platforms
     */
    static void mkDir(std::string dir);

    /**
     * API for Launch App
     *
     *
     * @param app - App binary path.
     *
     * @return none
     *
     * NOTE: This API will be support other platforms
     */
    static void rmDir(std::string dir);

    /**
     * API for copyfile
     *
     *
     * @param app - App binary path.
     *
     * @return none
     *
     * NOTE: This API will be support other platforms
     */
    static void copyFile(std::string source, std::string destination);

    /**
     * API for remve file
     *
     *
     * @param app - App binary path.
     *
     * @return none
     *
     * NOTE: This API will be support other platforms
     */
    static void rmFile(std::string file);

    /**
     * API for check existing
     *
     *
     * @param app - App binary path.
     *
     * @return bool
     *
     * NOTE: This API will be support other platforms
     */
    static bool isExists(std::string file);

    /**
     * API for get current allocated memory
     *
     * @return allocated memory size (byte)
     *
     * NOTE: This API will be support other platforms
     */
    static long getCurrentAllocatedMemory();

    /**
     * API for freezing current thread from execute next code for a given time
     *
     * @param seconds no of seconds to wait for
     *
     * @return bool
     *
     * NOTE: This API will be support other platforms
     */
    static void waitInSecond(unsigned int seconds);

    /**
     * API for creating file in the specified path with specified file content
     *
     * @param filePath - File path
     *
     * @param fileContent - The Content that will be written in the file
     *
     * @return bool - if Successfully writes the file, returns true, otherwise false
     *
     * NOTE: This API will be supported in other platforms
     */
    static bool writeFile(std::string filePath, std::string fileName, std::string fileContent);

    /**
     * API for Getting Local IP
     *
     * @param ipVersion - InternetProtocolVersion enum
     *
     * @return string - ip of the local machine
     *
     * NOTE: This API will be supported in other platforms
     */
    static std::string getLocalIP(InternetProtocolVersion ipVersion);

    /**
     * API for Getting current system time
     *
     * @param [out] currentTime - struct tm type object
     *
     */
    static void getCurrentTime(struct tm& currentTime);

};
#endif
