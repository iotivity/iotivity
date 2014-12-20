//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

#ifndef __SHPUTILS__
#define __SHPUTILS__

#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include <iomanip>

#include "ControlleeManager.h"


#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000) /**< Represents the similar signature for sleep(sec) function call in linux */
#endif


/**
 * @class   ProgramUtils
 * @brief   This class implements functions related to network connection, soft-AP mode enable/disable,
 *          processing SHP notifications and performing easy setup process. This also implements SHP framework
 *          initialization, start and stop features of SHP.
 */
class ProgramUtils
{
    public:


        /**
         * This method initializes SHP framework. It will fill configurations which required for SHP to start.
         *
         * @return  @c True if SHP framework initialized successfully @n
         *          @c False if failed to initialize SHP Framework.
         */
        static bool initializeFramework(char *portNum);

        /**
         * This method will start the SHP framework. In this call, application will wait till SHP Started.
         *
         * @return  @c True if SHP framework started successfully @n
         *          @c False if failed to start SHP Framework.
         */
        static bool startFramework();

        /**
         * This method will stop the SHP framework. In this call, application will wait till SHP Stopped.
         *
         * @return  @c True if SHP framework stopped successfully @n
         *          @c False if failed to stop SHP Framework.
         */
        static bool stopFramework();


        /**
         * Test Code added by Sundar
         */
//  static bool testProfiles();

        static void RegisterDeviceListener() ;

        static bool Notify();
        static void initNotify();
        static void initSubscription();

        static void generateSubscription();

        static bool getIPAddressAndUUID(std::string &ipAddressStr, std::string &uuid);

        static OC::ControlleeManager *sp_shp;   /**< Represents object of controllee */
        static ::MyDevice *sp_myDevice;         /**< Represents object of controllee Device. */
};

#endif
