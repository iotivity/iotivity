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

int id = 0;
OIC::Service::NSProviderService* g_nsProviderService;
const string TEST_TOPIC_1 = "TEST_TOPIC_1";

void syncCallback(OIC::Service::NSSyncInfo *sync)
{
    IOTIVITYTEST_LOG(INFO, "NSProviderSyncInfoCallback() called !!");
}

void subscribeRequestCallback(OIC::Service::NSConsumer *consumer)
{
    IOTIVITYTEST_LOG(INFO, "subscribeRequestCallback() called !!");

    IOTIVITYTEST_LOG(INFO, "Consumer ID: %s", consumer->getConsumerId().c_str());

    consumer->acceptSubscription(true);
    g_nsProviderService->registerTopic(TEST_TOPIC_1);
}

void startProvider(bool policy)
{
    NSProviderService::ProviderConfig cfg;
    cfg.m_subscribeRequestCb = subscribeRequestCallback;
    cfg.m_syncInfoCb = syncCallback;

    if (policy == true)
    {
        cfg.subControllability = true;
    }
    else
    {
        cfg.subControllability = false;
    }

    g_nsProviderService = NSProviderService::getInstance();
    g_nsProviderService->start(cfg);
}

void chooseOption(int a)
{
    switch (a)
    {
        case 0:
            startProvider(true);
            break;

        case 1:
            startProvider(false);
            break;

        default:
            IOTIVITYTEST_LOG(INFO, "Simulator will configured as default...");
            startProvider(true);
            break;
    }
}

int main(int argc, char **argv)
{
    PlatformConfig config
    { ServiceType::InProc, ModeType::Both, "0.0.0.0", 0, QualityOfService::LowQos };
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

    g_nsProviderService->stop();

    return 0;
}
