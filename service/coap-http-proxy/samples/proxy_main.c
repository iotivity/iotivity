//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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
#include "CoapHttpHandler.h"

#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

int g_quitFlag = 0;

void handleSigInt(int signum);

/*
* This method is an entry point of CoAP-HTTP Proxy.
*/

int main()
{
    printf("CoAP-HTTP proxy is starting..\n");
    OCStackResult result = OCInit(NULL, 0, OC_SERVER);
    if (result != OC_STACK_OK)
    {
        printf("Failed starting proxy\n");
        return 0;
    }

    if (CHPInitialize() != OC_STACK_OK)
    {
        printf("Failed to start proxy.\n");
        OCStop();
        return 0;
    }

    printf("Proxy started successfully.\n");

    signal(SIGINT, handleSigInt);
    while (!g_quitFlag)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            CHPTerminate();
            OCStop();
            printf("OCStack process error\n");
            return 0;
        }
    }

    if (CHPTerminate() != OC_STACK_OK)
    {
        printf("CHPTerminate failed.\n");
    }
    else
    {
        printf("CHPTerminate success.\n");
    }

    OCStop();
    printf("Exiting proxy main loop...\n");
    return 0;

}

/*
* This is a signal handling function for SIGINT(CTRL+C).
* A Resource Directory handle the SIGINT signal for safe exit.
*
* @param[in] signal
*                 signal number of caught signal.
*/
void handleSigInt(int signum)
{
    if (signum == SIGINT)
    {
        g_quitFlag = 1;
    }
}

