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

#include "ProgramUtils.h"
#include <signal.h>
#include "cstdlib"

/**
 * This is signal handler. Whenever application gets some signal i.e., SIGINT, SIGABRT, SIGTERM etc.
 * application will get this handler, here application will stop the running SHP framework smoothly.
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
 * This is the main entry point of the program. In this method, application will initialize the SHP Framework
 * and starts the framework. @n
 * Once SHP framework started successfully, main process will triggers processNotication to process any pending
 * notification which has received from the framework while SHP Starting. Once most recent pending notification
 * processed, then main process will go in wait. Whenever application receives any callback there application execute
 * some task on the main thread, then application will notify through the condition_variable. Then main thread will
 * come out of the wait and process the received notification and then again goes to wait.
 */
int main(int argc, char **argv)
{

    argc--;
    argv++;

    /**
     * Initializing SHP Framework. If any failure while initializing, then exiting application.
     */
    if (false == ProgramUtils::initializeFramework(argv[0]))
    {
        std::cout << "::main(): => " << "ERROR: Failed to Initialize framework" << std::endl;
        ProgramUtils::stopFramework();
        return -1;
    }

    /**
     * Start SHP Framework. If any failure while initializing, then exiting application.
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
    while (1)
    {
        char c = getchar();
        if ((c == 'q') || (c == 'Q'))
        {
            ProgramUtils::stopFramework();
            break;
        }
        else if ((c == 'n') || (c == 'N'))
        {
            ProgramUtils::Notify();
        }
        else if ((c == 's') || (c == 'S'))
        {
            ProgramUtils::generateSubscription();
        }
    }

    return 0;
}
