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
int id = 0;

void onProviderSync(NSSyncInfo* syncInfo)
{
    IOTIVITYTEST_LOG(INFO, "NSProviderSyncInfoCallback() called !!");
}

void onSubscribeRequest(NSConsumer* consumer)
{
    IOTIVITYTEST_LOG(INFO, "NSSubscribeRequestCallback() called !!");

    NSAcceptSubscription(consumer->consumerId, true);
}

void sendNotification()
{
    NSMessage *msg = NSCreateMessage();

    msg->messageId = ++id;
    string fullString = "TestApp title " + id;
    msg->title = OICStrdup(fullString.c_str());

    fullString = "TestApp body " + id;
    msg->contentText = OICStrdup(fullString.c_str());

    msg->sourceName = OICStrdup("OCF");

    fullString = "TestApp topic " + id;
    msg->topic = OICStrdup(fullString.c_str());

    NSResult result = NSSendMessage(msg);

    if (result != NS_OK)
    {
        cout << "Fail to send notification" << endl;
    } else {
        cout << "Notification send..." << endl;
    }
}

void starProvider(bool policy) {
    NSProviderConfig config;

    config.subRequestCallback = &onSubscribeRequest;
    config.syncInfoCallback = &onProviderSync;
    config.subControllability = policy;
    config.userInfo = OICStrdup("OCF_NOTIFICATION");

    NSResult result = NSStartProvider(config);

    if (result == NS_ERROR) {
        IOTIVITYTEST_LOG(ERROR, "Fail to start provider");
        isStarted = false;
    }
    else {
        IOTIVITYTEST_LOG(INFO, "Provider Started.....");
        isStarted = true;

        char* topic = OICStrdup("Simulator Topic 1");
        NSProviderRegisterTopic(topic);
        topic = OICStrdup("Simulator Topic 2");
        NSProviderRegisterTopic(topic);
        topic = OICStrdup("Simulator Topic 3");
        NSProviderRegisterTopic(topic);

        sendNotification();
    }
}

void stopProvider() {
    if (isStarted) {
        NSStopProvider();
    }
}

void chooseOption(int a) {
    switch (a) {
        case 0:
            starProvider(true);
            break;

        case 1:
            starProvider(false);
            break;

        default:
            IOTIVITYTEST_LOG(INFO, "Simulator will configured as default...");
            starProvider(true);
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

    stopProvider();

    return 0;
}
