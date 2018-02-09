//******************************************************************
//
// Copyright 2017 Open Connectivity Foundation
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
#include <signal.h>
#include <string>
#include <iostream>

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#include "DimmingLightServer.h"
#include "Platform.h"

using namespace OC;

#define INTERFACE_KEY "if"

#ifdef __unix__
// global needs static, otherwise it can be compiled out and then Ctrl-C does not work
static int quit = 0;
// handler for the signal to stop the application
void handle_signal(int signal)
{
    OC_UNUSED(signal);
    quit = 1;
}
#endif

// main application
// starts the server 
int main()
{
    Platform platform;
    OC_VERIFY(platform.start() == OC_STACK_OK);
    std::cout << "oic/p" << std::endl;
    // initialize "oic/p"
    if(platform.registerPlatformInfo() != OC_STACK_OK)
    {
        std::cout << "Platform Registration (oic/p) failed\n";
    }
    // initialize "oic/d"
    std::cout << "oic/d" << std::endl;
    if(platform.setDeviceInfo() != OC_STACK_OK)
    {
        std::cout << "Device Registration (oic/d) failed\n";
    }

    std::cout << "device type: " <<  platform.deviceType << std::endl;
    std::cout << "platformID: " <<  platform.getPlatformInfo()->platformID << std::endl;
    std::cout << "platform independent: " << platform.protocolIndependentID << std::endl;

    // create the server
    DimmingLightServer server;

#ifdef __unix__
    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);

    std::cout << "Press Ctrl-C to quit...." << std::endl;
    do
    {
        usleep(2000000);
    }
    while (quit != 1);
#endif

#if defined(_WIN32)
    std::cout << "Press Ctrl-C to quit...." << std::endl;
    // we will keep the server alive for at most 30 minutes
    std::this_thread::sleep_for(std::chrono::minutes(30));
#endif
    OC_VERIFY(platform.stop() == OC_STACK_OK);

    return 0;
}
