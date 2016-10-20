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

#include "NSProviderCppHelper.h"

NSProviderCppHelper* NSProviderCppHelper::s_nsHelperInstance = NULL;
std::mutex NSProviderCppHelper::s_mutex;
string NSProviderCppHelper::s_consumerID = "";
NSProviderService* NSProviderCppHelper::s_pNSProviderService = nullptr;

NSProviderCppHelper::NSProviderCppHelper()
{
    OC::PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Both, CT_DEFAULT, CT_DEFAULT,
            OC::QualityOfService::LowQos };
    OC::OCPlatform::Configure(cfg);
    IOTIVITYTEST_LOG(INFO, "Platform Configuration Done!!");

    try
    {
        OC::OCPlatform::stopPresence();
        IOTIVITYTEST_LOG(INFO, "Successfully stopPresense() called !!");
    }
    catch (...)
    {
        IOTIVITYTEST_LOG(WARNING, "Can't stop presense..!!!");
    }
}

NSProviderCppHelper* NSProviderCppHelper::getInstance()
{
    if (s_nsHelperInstance == NULL)
    {
        s_mutex.lock();

        s_nsHelperInstance = new NSProviderCppHelper();

        s_mutex.unlock();
    }

    return s_nsHelperInstance;
}

string NSProviderCppHelper::getResultString(OIC::Service::NSResult result)
{
    switch (result)
    {
        case OIC::Service::NSResult::OK:
            return "OK";

        case OIC::Service::NSResult::ERROR:
            return "ERROR";

        case OIC::Service::NSResult::SUCCESS:
            return "SUCCESS";

        case OIC::Service::NSResult::FAIL:
            return "FAIL";

        default:
            return "UNKNOWN";
    }
}

void NSProviderCppHelper::waitForConsumer(int timeout)
{
    int waitTime = 0;
    while (waitTime < timeout)
    {
        if (s_consumerID == "")
        {
            CommonUtil::waitInSecond(WAIT_TIME_MIN);
            waitTime++;
        }
        else
        {
            break;
        }
    }
}

void NSProviderCppHelper::syncCallback(OIC::Service::NSSyncInfo *sync)
{
    IOTIVITYTEST_LOG(INFO, "NSProviderSyncInfoCallback() called !!");
}

void NSProviderCppHelper::subscribeRequestCallback(OIC::Service::NSConsumer *consumer)
{
    IOTIVITYTEST_LOG(INFO, "subscribeRequestCallback() called !!");

    s_consumerID = consumer->getConsumerId();
    IOTIVITYTEST_LOG(INFO, "Consumer ID: %s", s_consumerID.c_str());
}

OIC::Service::NSConsumer* NSProviderCppHelper::getConsumer(bool subcontrollability)
{
    CommonUtil::launchApp(CONSUMER_SIMULATOR_OPTION, true);
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    s_pNSProviderService = NSProviderService::getInstance();

    NSProviderService::ProviderConfig providerCfg;
    providerCfg.m_subscribeRequestCb = subscribeRequestCallback;
    providerCfg.m_syncInfoCb = syncCallback;

    if (subcontrollability == true)
    {
        providerCfg.subControllability = true;
    }
    else
    {
        providerCfg.subControllability = false;
    }

    s_pNSProviderService->start(providerCfg);
    waitForConsumer(WAIT_TIME_MAX);

    OIC::Service::NSConsumer* nsConsumer = s_pNSProviderService->getConsumer(s_consumerID);

    return nsConsumer;
}
