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

#include "AirConditionerServerSample.h"

static oc_mutex g_mutex = NULL;
static oc_cond g_cond = NULL;

using namespace std;

ResultCode MyBinarySwitchDelegate::turnOnCallback()
{
    std::cout << "[AirConditionerServerSample][BinarySwitch] turnOnCallback.." << endl;
    return SUCCESS;
}

ResultCode MyBinarySwitchDelegate::turnOffCallback()
{
    std::cout << "[AirConditionerServerSample][BinarySwitch] turnOffCallback.." << endl;
    return SUCCESS;
}

ResultCode MyTemperatureDelegate::setTemperatureCallback(double temperature, std::string units)
{
    std::cout << "[AirConditionerServerSample][Temperature] temperature:" <<
        temperature << units <<std::endl;
    return SUCCESS;
}

void PrintUsage()
{
    std::cout << std::endl;
    std::cout << "Usage : AirConditionerServerSample\n";
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

static AirConditionerDevice *myAirCon = NULL;

static MyBinarySwitchDelegate *switchDelegate = NULL;
static MyTemperatureDelegate *temperatureDelegate = NULL;

int main(int argc, char* argv[])
{
    PrintUsage();

    std::cout << "======================================================" << endl;
    std::cout << "#1. Create Predefined Air Conditioner Device" << endl;
    myAirCon = new AirConditionerDevice;

    std::cout << "#2. Set BinarySwitch" << endl;
    switchDelegate = new MyBinarySwitchDelegate;
    myAirCon->m_binarySwitch.setDelegate(switchDelegate);
    myAirCon->m_binarySwitch.setState(false);

    std::cout << "#3. Set Temperature" << endl;
    temperatureDelegate = new MyTemperatureDelegate;
    myAirCon->m_temperature.setTemperatureResourceDelegate(temperatureDelegate);
    myAirCon->m_temperature.setTemperatureUnits("C");
    myAirCon->m_temperature.setTemperature(20.0);
    myAirCon->m_temperature.setMinRange(15.0);
    myAirCon->m_temperature.setMaxRange(30.0);
    std::cout << "======================================================" << endl;

    g_cond = oc_cond_new();
    g_mutex = oc_mutex_new();

    signal(SIGINT, handleSigInt);

    oc_mutex_lock(g_mutex);
    oc_cond_wait(g_cond, g_mutex);
    signal(SIGINT, SIG_IGN);
    oc_mutex_unlock(g_mutex);

    delete(myAirCon);
    delete(switchDelegate);
    delete(temperatureDelegate);

    oc_mutex_free(g_mutex);
    oc_cond_free(g_cond);

    return 0;
}
