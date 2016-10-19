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

#include "NSUtility.h"

void* OCProcessThread(void * ptr)
{
    (void) ptr;
    while (!g_IsExit)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            printf("OCStack process error");
            return NULL;
        }
    }

    return NULL;
}

void shareLog(string log)
{
    if (g_IsTCRunning)
    {
        cout << "TC Logging.. " << endl;
        write(g_WriteFile, log.c_str(), log.length());
    }

    cout << log << endl;
}

void onProviderSyncInfo(NSSyncInfo* syncInfo)
{
    g_syncInfo = syncInfo;

    shareLog("syncInfoCallback Called");

    shareLog(
            "Sync from Consumer. Message ID: " + to_string(syncInfo->messageId) + " State: "
                    + getSyncState(syncInfo->state));
}

void onSubscribeRequest(NSConsumer* consumer)
{
    shareLog("subRequestCallback Called");

    g_ConsumerIDList.push_back(consumer->consumerId);
    shareLog("Subscribe request from Consumer ID: " + string(consumer->consumerId));
}

void startProvider(bool policy)
{
    NSProviderConfig config;
    config.subControllability = policy;
    config.subRequestCallback = onSubscribeRequest;
    config.syncInfoCallback = onProviderSyncInfo;
    config.userInfo = OICStrdup("OCF_NOTIFICATION");

    NSResult result = NSStartProvider(config);

    if (result == NS_OK)
    {
        shareLog("Provider started successfully");
    }
    else
    {
        shareLog("Can't start provider");
    }
}

void stopProvider()
{
    NSResult result = NSStopProvider();

    if (result == NS_OK)
    {
        shareLog("Provider stopped successfully");
    }
    else
    {
        shareLog("Can't stop provider");
    }
}

void acceptSubscription(bool isAccept)
{
    if (g_ConsumerIDList.size() == 0)
    {
        shareLog("No Consumer Found...");
        return;
    }

    for (auto consumerId : g_ConsumerIDList)
    {
        NSResult result = NSAcceptSubscription(consumerId.c_str(), isAccept);

        string log = "";
        string accpt = "accept";

        if (!isAccept)
            accpt = "reject";

        if (result == NS_OK)
        {
            log = log + "Subscription " + accpt + "ed";
        }
        else
        {
            log = log + "Fail to " + accpt + " subscription";
        }

        log = log + " for Consumer ID: " + consumerId;

        shareLog(log);
    }
}

char * getNewTopic()
{
    string topic = g_TopicName + to_string(g_TopicID);

    return topic.c_str();
}

void sendNotification()
{
    NSMessage *msg = NSCreateMessage();

    string title = "TESTAPP_Title_" + to_string(++g_ID);
    msg->title = title.c_str();

    string body = "TESTAPP_Body_" + to_string(g_ID);
    msg->contentText = body.c_str();

    msg->sourceName = OICStrdup("TEST");

    msg->topic = getNewTopic();

    NSResult result = NSSendMessage(msg);

    if (result != NS_OK)
    {
        shareLog("Fail to send notification");
    }
    else
    {
        shareLog("Notification send successfully...");
    }
}

char* getConsumerId()
{
    string consumerId;

    if (g_ConsumerIDList.size() != 0)
    {
        consumerId = g_ConsumerIDList.back();
    }
    else
    {
        shareLog("Subscribed consumer is empty.");
    }

    return consumerId.c_str();
}

void printTopics(NSTopicLL * topics)
{
    if (!topics)
    {
        cout << "Topics is null";
        return;
    }

    NSTopicLL * topicIt = topics;

    while (topicIt)
    {
        string state = "UNSUBSCRIBED";

        if ((int) topicIt->state == 1)
        {
            state = "SUBSCRIBED";
        }

        shareLog("Topic name: " + string(topicIt->topicName) + ", State: " + state);
        topicIt = topicIt->next;
    }
}

void sendSyncInfo()
{
    if (g_syncInfo)
    {
        NSResult result = NSProviderSendSyncInfo(g_syncInfo->messageId, NSSyncType::NS_SYNC_UNREAD);

        if (result == NS_OK)
        {
            shareLog("Send SyncInfo Successfully");
        }
        else
        {
            shareLog("Send Fail....");
        }
    }
    else
    {
        shareLog("No info found...");
    }
}

// This function is for menu selection
void menuSelection(ProviderAppMenu menu)
{
    switch (menu)
    {
        case START_PROVIDER_ACCEPT:
            startProvider(true);
            break;

        case START_PROVIDER_CONSUMER_ACCEPT:
            startProvider(false);
            break;

        case STOP_PROVIDER:
            stopProvider();
            break;

        case ACCEPT_SUBSCRIPTION:
            acceptSubscription(true);
            break;

        case REJECT_SUBSCRIPTION:
            acceptSubscription(false);
            break;

        case SEND_NOTIFICATION:
            sendNotification();
            break;

        case SEND_SYNC_INFO:
            sendSyncInfo();
            break;

        case ADD_TOPIC:
        {
            g_TopicID++;
            char * topicName = getNewTopic();
            cout << "Topic to add: " << topicName << endl;
            NSResult result = NSProviderRegisterTopic(topicName);

            if (result == NS_OK)
            {
                shareLog("Registered topic Successfully");
            }
            else
            {
                shareLog("Register topic failed....");
            }
        }
            break;

        case DELETE_TOPIC:
        {
            char * topicName = getNewTopic();
            cout << "Topic to delete: " << topicName << endl;
            NSResult result = NSProviderUnregisterTopic(topicName);
            if (result == NS_OK)
            {
                shareLog("Unregistered topic Successfully");
                g_TopicID--;
            }
            else
            {
                shareLog("Unregister topic failed....");
            }
        }
            break;

        case SELECT_TOPIC:
        {
            char* conId = getConsumerId();

            if (conId != nullptr)
            {
                if (NSProviderSetConsumerTopic(conId, getNewTopic()) == NS_OK)
                {
                    shareLog("Set Consumer Topic Successfully");
                }
                else
                {
                    shareLog("Can't Set Consumer Topic");
                }
            }
            else
            {
                shareLog("No consumer found.");
            }
        }
            break;

        case UNSELECT_TOPIC:
        {
            char* conId = getConsumerId();

            if (conId != nullptr)
            {
                if (NSProviderUnsetConsumerTopic(conId, getNewTopic()) == NS_OK)
                {
                    shareLog("Unset Consumer Topic Successfully");
                }
                else
                {
                    shareLog("Can't Unset Consumer Topic");
                }
            }
            else
            {
                shareLog("No consumer found.");
            }
        }
            break;

        case CONSUMER_TOPICS:
        {
            char* conId = getConsumerId();

            if (conId != nullptr)
            {
                NSTopicLL *topics = NSProviderGetConsumerTopics(conId);
                cout << "Printing consumer topics: " << endl;
                printTopics(topics);
            }
        }
            break;

        case PROVIDER_TOPICS:
        {
            NSTopicLL *topics = NSProviderGetTopics();
            printTopics(topics);
        }
            break;

        case PROVIDER_EXIT:
            cout << "Quit from NS Provider TestApp..." << endl << endl;
            g_IsExit = true;
            break;
    }
}

// Print Menu Items
void showMainMenu()
{
    cout << endl;
    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl << endl;

    cout << "  " << setw(2) << START_PROVIDER_ACCEPT << ".  Start Provider" << endl;
    cout << "  " << setw(2) << START_PROVIDER_CONSUMER_ACCEPT << ".  Start Provider for Consumer"
            << endl;
    cout << "  " << setw(2) << STOP_PROVIDER << ".  Stop Provider" << endl;
    cout << "  " << setw(2) << ACCEPT_SUBSCRIPTION << ".  Accept Subscriptions" << endl;
    cout << "  " << setw(2) << REJECT_SUBSCRIPTION << ".  Reject Subcriptions" << endl;
    cout << "  " << setw(2) << SEND_NOTIFICATION << ".  Send Notification" << endl;
    cout << "  " << setw(2) << SEND_SYNC_INFO << ".  Send Sync Info" << endl;
    cout << "  " << setw(2) << ADD_TOPIC << ".  Add Topic" << endl;
    cout << "  " << setw(2) << DELETE_TOPIC << ".  Delete Topic" << endl;
    cout << "  " << setw(2) << SELECT_TOPIC << ".  Select Topic" << endl;
    cout << "  " << setw(2) << UNSELECT_TOPIC << ".  Unselect Topic" << endl;
    cout << "  " << setw(2) << CONSUMER_TOPICS << ".  Consumer Topics" << endl;
    cout << "  " << setw(2) << PROVIDER_TOPICS << ".  Show Topics" << endl;
    cout << endl;

    cout << "  " << setw(2) << ProviderAppMenu::PROVIDER_EXIT << ".  Exit" << endl;
    cout << endl;

    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
    cout << endl;

    cout << "\tPlease Choose a Menu: ";
}

// This function is for taking input from console and processing input as necessary
int userInputProcessing(int max, int min)
{
    int input;

    if (g_IsTCRunning)
    {
        while (true)
        {
            ssize_t rd = read(g_ReadFile, g_InputBuf, MAX_BUF);

            if (rd == 0)
                break;

            if (rd != -1)
            {
                input = atoi(g_InputBuf);
                cout << input << endl;
                sleep(2);
                break;
            }
        }
    }
    else
    {
        cin >> input;
    }

    cout << endl;

    if (!cin.fail() && min <= input && input <= max)
        return input;

    cin.clear();
    cin.ignore(numeric_limits< streamsize >::max(), '\n');
    throw runtime_error("Invalid Input, please try again");
}

FILE* server_fopen(const char *path, const char *mode)
{
    (void) path;
    return fopen("oic_ns_provider_db.dat", mode);
}

void enableSecure()
{
    cout << "Running as Secured mode." << endl;

    // open oic_db
    static OCPersistentStorage ps =
    { server_fopen, fread, fwrite, fclose, unlink };
    OCRegisterPersistentStorageHandler(&ps);
}

void enablePipe()
{
    cout << "Running from TC......." << endl;
    g_IsTCRunning = true;

    sleep(2);
    mkfifo(FIFO_CONSUMER_FILE, 0666);

    cout << "Opening Writer...." << endl;
    g_WriteFile = open(FIFO_CONSUMER_FILE, O_WRONLY);
    cout << "Writer Opened...." << endl;

    cout << "Reading from FIFO..." << endl;
    g_ReadFile = open(FIFO_READ_FILE, O_RDONLY);
    cout << "Read File Opened" << endl;
}

void closePipe()
{
    close(g_WriteFile);
    close(g_ReadFile);

    unlink(FIFO_READ_FILE);
    unlink(FIFO_CONSUMER_FILE);
}

// This is the main function the NS Provider Application
int main(int argc, char **argv)
{
    int secured = 0, menu = 0;

    if (argc > 1)
    {
        try
        {
            secured = atoi(argv[1]);
        }
        catch (...)
        {
            secured = 0;
        }
    }

    if (argc > 2)
    {
        try
        {
            menu = atoi(argv[2]);
        }
        catch (...)
        {
            menu = 0;
        }
    }

    if (menu == 1)
    {
        enablePipe();
    }

    if (secured == 1)
    {
        enableSecure();
    }

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        cout << "OCStack Init Error" << endl;
        return 0;
    }

    pthread_t processThread;

    pthread_create(&processThread, NULL, OCProcessThread, unlink);

    try
    {
        OC::OCPlatform::stopPresence();
    }
    catch (...)
    {
        cout << "Can't stop presence..." << endl;
    }

    while (!g_IsExit)
    {
        try
        {
            showMainMenu();

            menuSelection(ProviderAppMenu(userInputProcessing(PROVIDER_TOPICS, PROVIDER_EXIT)));
        }
        catch (const exception &e)
        {
            cout << "[ERROR] " << e.what() << endl;
        }
    }

    closePipe();

    return 0;
}
