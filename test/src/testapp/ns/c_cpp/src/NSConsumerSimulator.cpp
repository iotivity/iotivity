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

#include <NSSimulatorUtil.h>

bool isStarted = false;

void onProviderChanged(NSProvider * provider, NSProviderState response)
{
    IOTIVITYTEST_LOG(INFO, "onProviderChanged() called !!");
    NSSubscribe(provider->providerId);
}

void onNotificationPosted(NSMessage * notification)
{
    IOTIVITYTEST_LOG(INFO, "onNotificationPosted() called !!");
}

void onNotificationSync(NSSyncInfo * sync)
{
    IOTIVITYTEST_LOG(INFO, "onNotificationSync() called !!");
}

void startConsumer() {
    IOTIVITYTEST_LOG(INFO, "Starting Consumer.......");

    NSConsumerConfig config;

    config.changedCb = &onProviderChanged;
    config.messageCb = &onNotificationPosted;
    config.syncInfoCb = &onNotificationSync;

    NSResult result = NSStartConsumer(config);

    if (result == NS_OK) {
        isStarted = true;

        IOTIVITYTEST_LOG(INFO, "Consumer started successfully !!");
    } else {
        isStarted = false;

        IOTIVITYTEST_LOG(INFO, "Can't start Consumer !!");
    }
}

void stopConsumer() {
    if (isStarted)
        NSStopConsumer();
}

void chooseOption(int a) {
    switch (a) {
        case 0:
            startConsumer();
            break;

        default:
            IOTIVITYTEST_LOG(INFO, "Simulator will configured as default...");
            startConsumer();
            break;
    }
}

int main(int argc, char **argv) {
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

    if (argc > 1) {
        try {
            inCase = atoi(argv[1]);
        } catch (...) {
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

