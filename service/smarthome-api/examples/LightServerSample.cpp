/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <signal.h>
#include "octhread.h"
#include "LightServerSample.h"

using namespace std;

#define USER_RESOURCE_URI "/color"
#define USER_RESOURCE_TYPE "core.color"
#define USER_RESOURCE_PROP_KEY "color"

static oc_mutex g_mutex = NULL;
static oc_cond g_cond = NULL;

using namespace OIC::Service::SH;

UserColorResource::UserColorResource() :
        SHBaseResource(USER_RESOURCE_URI, USER_RESOURCE_TYPE)
{
    std::cout << "[UserColorResource] constructor" << std::endl;

    std::string color = "black";
    PropertyBundle bundle;
    bundle.setValue(USER_RESOURCE_PROP_KEY, color);
    setPropertyBundle(bundle);
}

UserColorResource::~UserColorResource()
{
}

void UserColorResource::setColor(std::string color)
{
    PropertyBundle bundle;
    bundle.setValue(USER_RESOURCE_PROP_KEY, color);
    setPropertyBundle(bundle);
}

std::string UserColorResource::getColor()
{
    PropertyBundle bundle = getPropertyBundle();

    // Get "color" property value from bundle.
    std::string color;
    bundle.getValue(USER_RESOURCE_PROP_KEY, color);
    return color;
}

void UserColorResource::setColorDelegate(MyColorDelegate* delegate)
{
    setDelegate(delegate);
}

ResultCode MyBinarySwitchDelegate::turnOnCallback()
{
    std::cout << "[LightServerSample][BinarySwitch] turnOnCallback.." << endl;
    return SUCCESS;
}

ResultCode MyBinarySwitchDelegate::turnOffCallback()
{
    std::cout << "[LightServerSample][BinarySwitch] turnOffHandler.." << endl;
    return SUCCESS;
}

ResultCode MyColorDelegate::onGet(RequestId requestId, const ResourceQuery& query)
{
    std::cout << "[LightServerSample][Color] onGet.." << endl;
    return SUCCESS;
}

ResultCode MyColorDelegate::onSet(RequestId requestId, const PropertyBundle& bundle,
                                  const ResourceQuery& query)
{
    std::cout << "[LightServerSample][Color] onSet.." << endl;
    return SUCCESS;
}

void PrintUsage()
{
    std::cout << std::endl;
    std::cout << "Usage : LightServerSample <value>\n";
    std::cout << "    1 - pre-defined light device(binaryswitch resource)\n\n";
    std::cout << "    2 - extended light device(binaryswitch + color resource)\n";
}

/* SIGINT handler: set gQuitFlag to 1 for graceful termination */
void handleSigInt(int signum)
{
    if (signum == SIGINT)
    {
        oc_mutex_lock(g_mutex);
        oc_cond_signal(g_cond);
        oc_mutex_unlock(g_mutex);
    }
}

static LightDevice *myLight = NULL;
static UserLight *userLight = NULL;

static MyBinarySwitchDelegate *switchDelegate = NULL;
static MyColorDelegate *colorDelegate = NULL;

int main(int argc, char* argv[])
{
    PrintUsage();

    bool isPredefined = true;
    if (argc == 2)
    {
        int value = atoi(argv[1]);
        switch (value)
        {
            case 1:
                isPredefined = true;
                break;
            case 2:
                isPredefined = false;
                break;
            default:
                break;
        }
    }
    else
    {
        return -1;
    }

    if (isPredefined)
    {
        std::cout << "======================================================" << endl;
        std::cout << "#1. Create Predefined Light Device" << endl;
        myLight = new LightDevice;
        std::cout << "#2. Set MyBinarySwitchDelegate" << endl;
        switchDelegate = new MyBinarySwitchDelegate;
        myLight->m_binarySwitch.setDelegate(switchDelegate);

        myLight->m_binarySwitch.getState();
        std::cout << "======================================================" << endl;

    }
    else
    {
        std::cout << "======================================================" << endl;
        std::cout << "#3. Create Extended Light Device" << endl;
        userLight = new UserLight;

        std::cout << "#4. Set MyBinarySwitchDelegate" << endl;
        switchDelegate = new MyBinarySwitchDelegate;
        userLight->m_binarySwitch.setDelegate(switchDelegate);

        std::cout << "#5. Set MyColorDelegate" << endl;
        colorDelegate = new MyColorDelegate;
        userLight->color.setColorDelegate(colorDelegate);
        std::cout << "======================================================" << endl;
    }

    g_cond = oc_cond_new();
    g_mutex = oc_mutex_new();

    signal(SIGINT, handleSigInt);

    oc_mutex_lock(g_mutex);
    oc_cond_wait(g_cond, g_mutex);
    signal(SIGINT, SIG_IGN);
    oc_mutex_unlock(g_mutex);

    if (isPredefined)
    {
        delete (myLight);
        delete (switchDelegate);
    }
    else
    {
        delete (userLight);
        delete (switchDelegate);
        delete (colorDelegate);
    }

    oc_mutex_free(g_mutex);
    oc_cond_free(g_cond);

    return 0;
}
