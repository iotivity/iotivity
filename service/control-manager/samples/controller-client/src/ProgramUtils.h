/**

 * @copyright
 * This program is subject to copyright protection in accordance with the
 * applicable law. It must not, except where allowed by law, by any means or
 * in any form be reproduced, distributed or lent. Moreover, no part of the
 * program may be used, viewed, printed, disassembled or otherwise interfered
 * with in any form, except where allowed by law, without the express written
 * consent of the copyright holder.@n
 * Copyright (C) 2014 Samsung Electronics Co., Ltd. All Rights Reserved.
 *
 * @file        ProgramUtils.h
 * @brief       This file includes all class methods and class members which are related to ProgramUtils class.
 * @author      Auto-Generated
 */

#ifndef __CMUTILS__
#define __CMUTILS__

#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>

#include "ControlManager.h"
#include "ControlManagerListener.h"
#include "Device.h"
#include "testHumidityResource.h"
#include "testLightResource.h"
#include "testConfiguration.h"
#include "testDoors.h"
#include "testEnergyConsumptionResource.h"
#include "testInformationController.h"
#include "testMode.h"
#include "testOperationResource.h"
#include "testSubscriptionListener.h"
#include "testTemperaturesResource.h"
#include "testNetworksResource.h"
#include "testWifiResource.h"
#include "testAccessPointsResource.h"
#include "testUsage.h"
#include "DeviceType.h"
#include "ITestResource.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000) /**< Represents the similar signature for sleep(sec) function call in linux */
#endif


/**
 * @class   ProgramUtils
 * @brief   This class implements functions related to network connection, soft-AP mode enable/disable,
 *          processing Control Manager notifications and performing easy setup process. This also implements Control Manager framework
 *          initialization, start and stop features of Control Manager.
 */
class ProgramUtils
{
    public:
        /**
         * This method provide a way to obtain the IP address and uuid of a device that based on the windows/linux platform.
         *
         * @param[in] ipAddressStr  IP address string to be used
         * @param[in] uuid      UUID to be used
         *
         * @return  @c True if IP address and UUID are successfully achieved @n
         *          @c False if IP address and UUID were not properly achieved
         */
        static bool getIPAddressAndUUID(std::string &ipAddressStr, std::string &uuid);

        /**
         * This method initializes Control Manager framework. It will fill configurations which required for Control Manager to start.
         *
         * @return  @c True if Control Manager framework initialized successfully @n
         *          @c False if failed to initialize Control Manager Framework.
         */
        static bool initializeFramework(char *portNum);

        /**
         * This method will start the Control Manager framework. In this call, application will wait till Control Manager Started.
         *
         * @return  @c True if Control Manager framework started successfully @n
         *          @c False if failed to start Control Manager Framework.
         */
        static bool startFramework();

        /**
         * This method will stop the Control Manager framework. In this call, application will wait till Control Manager Stopped.
         *
         * @return  @c True if Control Manager framework stopped successfully @n
         *          @c False if failed to stop Control Manager Framework.
         */
        static bool stopFramework();

        /**
         * This API being used to stop the Control Manager internally and start with specified Control Manager Mode.
         *
         * @param   mode    Represents Control Manager Mode (NORMAL_MODE, EASY_SETUP_MODE, REGISTRATION_MODE)
         *
         * @return  @c true if success @n
         *          @c false if failure
         */
        static bool restartControlManager(OC::Cm::CMModes mode);

        /**
         * This API being used to process the notification which has received by ControlManagerListener.
         *
         * @return  @c true if success @n
         *          @c false if failure
         */
        static bool processNotification();

        /**
         * Test Code added by Sundar
         */
        static bool testProfiles(int mode);

        static void RegisterDeviceListener() ;

        static int DisplayAllDevices();
        static int SelectDevices(int type);
        static void DisplayAvailableResources();
        static int ValideateResourcesType(char c);

        static ControlManagerListener
        *sp_ControlManagerListener;   /**< Represents object of ControlManagerListener class. By using this framework will notify to the application. */

        static ::MyDevice *sp_myDevice; /**< Represents object of Control Manager Device. */

        static OC::Cm::ControlManager *sp_shp;      /**< Represents object of Control Manager framework */

        static OC::Cm::Device *sp_device ;

        static OC::Cm::Device *sp_controlDevice;

        static char *currentResourecInitials;
        static int currentDeviceNo;

        static ITestResource *mResource;

};

#endif
