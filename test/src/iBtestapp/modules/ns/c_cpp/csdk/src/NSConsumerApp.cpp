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

    while (true)
    {
        usleep(2000);
        if (OCProcess() != OC_STACK_OK)
        {
            OCStop();
            break;
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

void printNotification(NSMessage *notification)
{
    cout << "ProviderID: " << notification->providerId << " Message ID: "
            << (long long int) notification->messageId << " Title: " << notification->title
            << " Body: " << notification->contentText << endl;
}

void printTopicList(NSTopicLL *topics)
{
    if (topics)
    {
        NSTopicLL *topicIter = topics;

        while (topicIter)
        {
            cout << "Topic Name: " << topicIter->topicName << "\t Topic State: "
                    << getTopicState(topicIter->state) << endl;
            topicIter = topicIter->next;
        }
    }
    else
    {
        cout << "There is no topic list..." << endl;
    }
}

void onProviderChanged(NSProvider * provider, NSProviderState response)
{
    shareLog("NSProviderStateCallback called");

    g_provider = provider;

    switch (response)
    {
        case NS_DISCOVERED:
            shareLog("Notification Resource Discovered");
            break;

        case NS_TOPIC:
        {
            shareLog("Provider changed to NS_TOPIC State. ID: " + string(provider->providerId));
        }
            break;

        case NS_ALLOW:
            shareLog("Provider changed to NS_ALLOW State. ID: " + string(provider->providerId));
            break;

        case NS_DENY:
            shareLog("Provider changed to NS_DENY State. ID: " + string(provider->providerId));
            break;

        case NS_STOPPED:
            shareLog("Provider changed to NS_STOPPED State. ID: " + string(provider->providerId));
            break;

        default:
            shareLog("Provider changed to UNKNOWN State. ID: " + string(provider->providerId));
            break;
    }
}

void onNotificationPosted(NSMessage * notification)
{
    shareLog("NSMessageReceivedCallback called");

    g_NotificaitonList.push_back(notification);

    cout << "####### Receiving Notification #######" << endl;
    printNotification(notification);

    cout << "Printing topic:" << endl;

    if (notification->topic && strlen(notification->topic) > 0)
    {
        cout << "Topic : " << notification->topic << endl;
    }
}

void onNotificationSync(NSSyncInfo * sync)
{
    shareLog("NSSyncInfoReceivedCallback called");

    cout << "Sync from Provider ID: " << sync->providerId << " Message ID: " << sync->messageId
            << " State: " << getSyncState(sync->state) << endl;
}

void startConsumer()
{
    NSConsumerConfig config;
    config.changedCb = &onProviderChanged;
    config.messageCb = &onNotificationPosted;
    config.syncInfoCb = &onNotificationSync;

    NSResult result = NSStartConsumer(config);

    if (result == NS_OK)
    {
        shareLog("Consumer Starts Successfully");
        g_IsConsumerStarted = true;
    }
    else
    {
        shareLog("Can't Start Consumer");
    }
}

void stopConsumer()
{
    NSResult result = NSStopConsumer();

    if (result == NS_OK)
    {
        shareLog("Consumer Stopped Successfully");
        g_IsConsumerStarted = false;
    }
    else
    {
        shareLog("Can't Stop Consumer");
    }
}

void getTopics()
{
    cout << "Get Topics" << endl;

    if (g_provider)
    {
        g_topicLL = NSConsumerGetTopicList(g_provider->providerId);

        cout << "Printing topic list:" << endl;
        printTopicList(g_topicLL);
    }
    else
    {
        cout << "No provider found..!!!" << endl;
    }
}

void updateTopics()
{
    shareLog("Update Topics");

    if (g_provider)
    {
        if (g_topicLL)
        {
            NSResult result = NSConsumerUpdateTopicList(g_provider->providerId, g_topicLL);

            if (result == NS_OK)
            {
                shareLog("Update successfully...");

                NSTopicLL * iter = g_topicLL;
                while (iter)
                {
                    cout << "Topic Name: " << iter->topicName << "\t Topic State: "
                            << getTopicState(iter->state) << endl;
                    iter = iter->next;
                }
            }
            else
            {
                shareLog("Fail to update topic");
            }
        }
        else
        {
            shareLog("Please select Get Topics first...");
        }
    }
    else
    {
        shareLog("There is no provider found...");
    }
}

void subscribeConsumer()
{
    if (g_provider)
    {
        NSResult result = NSSubscribe(g_provider->providerId);

        if (result == NS_OK)
        {
            shareLog("Subscribe successfully...");
        }
        else
        {
            shareLog("Fail to Subscribe...");
        }
    }
    else
    {
        shareLog("No Provider found !!!");
    }
}

void sendConsumerSyncInfo()
{
    if (g_NotificaitonList.size() == 0)
    {
        shareLog("There is no notification found.");
        return;
    }

    for (auto notification : g_NotificaitonList)
    {
        NSResult result = NSConsumerSendSyncInfo(notification->providerId, notification->messageId,
                NSSyncType::NS_SYNC_READ);

        string log = "";
        if (result == NS_OK)
        {
            log = log + "Successfully sending ";
        }
        else
        {
            log = log + "Fail to send ";
        }

        log = log + "sync info to provider: " + string(notification->providerId)
                + " as NS_SYNC_READ";

        shareLog(log);
    }
}

void rescanProvider()
{
    shareLog("Rescanning...");

    NSResult result = NSRescanProvider();

    if (result == NS_OK || result == NS_SUCCESS)
    {
        shareLog("Rescan successful");
    }
    else
    {
        shareLog("Fail to Rescan");
    }
}

// This function is for menu selection
void menuSelection(ConsumerAppMenu menu)
{
    switch (menu)
    {
        case START_CONSUMER:
            startConsumer();
            break;

        case STOP_CONSUMER:
            stopConsumer();
            break;

        case SUBSCRIBE_CONSUMER:
            subscribeConsumer();
            break;

        case RESCAN_PROVIDER:
            rescanProvider();
            break;

        case CONSUMER_SEND_SYNC_INFO:
            sendConsumerSyncInfo();
            break;

        case CONSUMER_GET_TOPICS:
            getTopics();
            break;

        case CONSUMER_UPDATE_TOPICS:
            updateTopics();
            break;

        case CONSUMER_EXIT:
            cout << "Quit from NS Consumer TestApp..." << endl << endl;
            g_IsExit = true;
            break;
    }
}

// Print Menu Items
void showMainMenu()
{
    cout << endl;
    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl << endl;

    cout << "  " << setw(2) << START_CONSUMER << ".  Start Consumer" << endl;
    cout << "  " << setw(2) << STOP_CONSUMER << ".  Stop Consumer" << endl;
    cout << "  " << setw(2) << SUBSCRIBE_CONSUMER << ".  Subscribe Consumer" << endl;
    cout << "  " << setw(2) << RESCAN_PROVIDER << ".  Manual Scan for Provider" << endl;
    cout << "  " << setw(2) << CONSUMER_SEND_SYNC_INFO << ".  Send Sync Info" << endl;
    cout << "  " << setw(2) << CONSUMER_GET_TOPICS << ".  Get Topics" << endl;
    cout << "  " << setw(2) << CONSUMER_UPDATE_TOPICS << ".  Update Topics" << endl;
    cout << endl;

    cout << "  " << setw(2) << CONSUMER_EXIT << ".  Exit" << endl;

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
    mkfifo(FIFO_WRITE_FILE, 0666);

    cout << "Opening Writer...." << endl;
    g_WriteFile = open(FIFO_WRITE_FILE, O_WRONLY);
    cout << "Writer Opened...." << endl;

    cout << "Reading..." << endl;
    g_ReadFile = open(FIFO_READ_FILE, O_RDONLY);
    cout << "Read Opened" << endl;
}

void closePipe()
{
    close(g_WriteFile);
    close(g_ReadFile);

    unlink(FIFO_READ_FILE);
    unlink(FIFO_WRITE_FILE);
}

// This is the main function the NS Consumer Test Application
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
        cout << "OCStack Initializing Error" << endl;
        return 0;
    }

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

            menuSelection(
                    ConsumerAppMenu(userInputProcessing(CONSUMER_UPDATE_TOPICS, CONSUMER_EXIT)));
        }
        catch (const exception &e)
        {
            cout << "[ERROR] " << e.what() << endl;
        }
    }

    return 0;
}
