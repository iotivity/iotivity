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
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#include <NSSimulatorCppUtil.h>

bool isStarted = false;
NSConsumerService* g_pNSConsumerServiceInstance = nullptr;

static void onProviderDiscovered(shared_ptr<OIC::Service::NSProvider> provider)
{
    IOTIVITYTEST_LOG(INFO, "%s is called", __func__);
    string providerId = provider->getProviderId();
    IOTIVITYTEST_LOG(INFO, "Provider ID is: %s", providerId.c_str());
}

void startConsumer()
{
    IOTIVITYTEST_LOG(INFO, "Starting Consumer.......");

    g_pNSConsumerServiceInstance = OIC::Service::NSConsumerService::getInstance();

    try
    {
        g_pNSConsumerServiceInstance->start(onProviderDiscovered);
    }
    catch (exception &e)
    {
        IOTIVITYTEST_LOG(INFO, "Exception occured while starting consumer....");
    }
}

void stopConsumer()
{
    try
    {
        g_pNSConsumerServiceInstance->stop();
    }
    catch (exception &e)
    {
        IOTIVITYTEST_LOG(WARNING, "Exception occurs while calling stop. Exception is: %s",e.what());
    }
}

void chooseOption(int a)
{
    switch (a)
    {
        case 0:
            startConsumer();
            break;

        default:
            IOTIVITYTEST_LOG(INFO, "Simulator will configured as default...");
            startConsumer();
            break;
    }
}

int main(int argc, char **argv)
{
    PlatformConfig config
    { ServiceType::InProc, ModeType::Both, CT_DEFAULT, CT_DEFAULT, QualityOfService::HighQos };
    OCPlatform::Configure(config);
    IOTIVITYTEST_LOG(INFO, "Platform Configuration Done!!");

    try
    {
        OCPlatform::stopPresence();
        IOTIVITYTEST_LOG(INFO, "Successfully stopPresense() called !!");
    }
    catch (...)
    {
        IOTIVITYTEST_LOG(WARNING, "Can't stop presense..!!!");
    }

    int inCase = 0;

    if (argc > 1)
    {
        try
        {
            inCase = atoi(argv[1]);
        }
        catch (...)
        {
            inCase = 0;
        }
    }

    chooseOption(inCase);

    IOTIVITYTEST_LOG(INFO, "Press any key and the enter to stop the servers :-)....");

    char n;
    cin >> n;

    stopConsumer();

    return 0;
}

