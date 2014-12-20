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
 * @file        main.cpp
 * @brief       This file includes entry point to the application and signal handler.
 * @author      Auto-Generated
 */

#include "ProgramUtils.h"
#include <signal.h>
#include "cstdlib"

using namespace std;

/**
 * This is signal handler. Whenever application gets some signal i.e., SIGINT, SIGABRT, SIGTERM etc.
 * application will get this handler, here application will stop the running Control Manager framework smoothly.
 *
 * @param[in] signal    Represents kind of signal i.e., SIGINT, SIGABRT, SIGTERM etc.
 */
void sighandler(int signal)
{
    std::cout << "Signal " << signal << " caught..." << std::endl;
    ProgramUtils::stopFramework();
    exit(0);
}

/**
 * This is the main entry point of the program. In this method, application will initialize the Control Manager Framework
 * and starts the framework. @n
 * Once Control Manager framework started successfully, main process will triggers processNotication to process any pending
 * notification which has received from the framework while Control Manager Starting. Once most recent pending notification
 * processed, then main process will go in wait. Whenever application receives any callback there application execute
 * some task on the main thread, then application will notify through the condition_variable. Then main thread will
 * come out of the wait and process the received notification and then again goes to wait.
 */
int main(int argc, char **argv)
{

    try
    {

        argc--;
        argv++;
        /**
         * Initializing Control Manager Framework. If any failure while initializing, then exiting application.
         */
        if (false == ProgramUtils::initializeFramework(argv[0]))
        {
            std::cout << "::main(): => " << "ERROR: Failed to Initialize framework" << std::endl;
            ProgramUtils::stopFramework();
            return -1;
        }

        /**
         * Start Control Manager Framework. If any failure while initializing, then exiting application.
         */
        if (false == ProgramUtils::startFramework())
        {
            ProgramUtils::stopFramework();
            std::cout << "::main(): => " << "ERROR: Failed to Start framework" << std::endl;
            return -1;
        }

        /**
         * Registering signals
         * @n
         * @note If application developer want to handle few more signals, then he needs to add.
         */
        signal(SIGABRT, &sighandler);
        signal(SIGTERM, &sighandler);
        signal(SIGINT, &sighandler);

        /** Prevent application from exiting */
        int end = 1 ;
        while (end)
        {
            int c = getchar();
            switch (c)
            {
                case 'd':
                    cout << "\n************************************************* \n" ;
                    ProgramUtils::DisplayAllDevices();
                    cout << "\n************************************************* \n" ;

                    cout << "    PLEASE ENTER DEVICE NO. \n" ;
                    int n ;
                    while (1)
                    {
                        n = getchar();
                        if (n - '0' >= 0)
                        {
                            if (ProgramUtils::SelectDevices(n - '0') == 1)
                            {
                                ProgramUtils::DisplayAvailableResources();
                            }
                            else
                            {
                                cout << "    PLEASE AGAIN ENTER DEVICE NO. \n" ;
                            }
                            break;
                        }
                    }
                    break ;
                case 'a': // AccessPoints
                case 'c': // configuration
                case 'o': // operatios
                case 'm': // mode
                case 'e': // energyconsuption
                case 'r': // doors
                case 't': // Temperatures
                case 'i': // information
                case 'l': // light
                case 'h': // Humidity
                case 'u': // UsageLimit
                case 'n': // Networks
                case 'w': // WiFi
                    if (ProgramUtils::ValideateResourcesType(c))
                    {
                        ProgramUtils::testProfiles(c);
                    }
                    else
                    {
                        cout << " Incorrect/Unsupported RESOURCE TYPE \n" ;
                        cout << " PLEASE ENTER CORRECT RESOURCE TYPE \n" ;
                    }
                    break;

                case 'q':
                case 'Q':
                    ProgramUtils::stopFramework();
                    end = 0 ;
                    break;
                case '\n':
                    break;
                default:

                    cout << " PLEASE ENTER CORRECT OPTION \n" ;
                    break;
            }
        }
    }
    catch (...)
    {
        cout << " main :: Default Exception \n" ;
    }
    return 0;
}
