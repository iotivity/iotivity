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

#include "NSConsumerCppHelper.h"

NSConsumerCppHelper* NSConsumerCppHelper::s_nsHelperInstance = NULL;
std::mutex NSConsumerCppHelper::s_mutex;
string NSConsumerCppHelper::s_providerID = "";

NSConsumerCppHelper::NSConsumerCppHelper()
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

NSConsumerCppHelper* NSConsumerCppHelper::getInstance()
{
    if (s_nsHelperInstance == NULL)
    {
        s_mutex.lock();

        s_nsHelperInstance = new NSConsumerCppHelper();

        s_mutex.unlock();
    }

    return s_nsHelperInstance;
}

void NSConsumerCppHelper::onProviderDiscovered(OIC::Service::NSProvider* provider)
{
    IOTIVITYTEST_LOG(INFO, "onProviderDiscovered called");

    s_providerID = provider->getProviderId();
    IOTIVITYTEST_LOG(INFO, "Provider ID: %s", s_providerID.c_str());
}

OIC::Service::NSProvider* NSConsumerCppHelper::getProvider(bool subcontrollability)
{
    if (subcontrollability == true)
    {
        CommonUtil::launchApp(PROVIDER_SIMULATOR_ALLOW, true);
        CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);
    }
    else
    {
        CommonUtil::launchApp(PROVIDER_SIMULATOR_NOT_ALLOW, true);
        CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);
    }

    NSConsumerService* nsConsumerService = NSConsumerService::getInstance();
    nsConsumerService->start(onProviderDiscovered);
    waitForProvider(WAIT_TIME_MAX);

    OIC::Service::NSProvider* nsProvider = nsConsumerService->getProvider(s_providerID);

    return nsProvider;
}

string NSConsumerCppHelper::getResultString(OIC::Service::NSResult result)
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

void NSConsumerCppHelper::waitForProvider(int timeout)
{
    int waitTime = 0;
    while (waitTime < timeout)
    {
        if (s_providerID == "")
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
