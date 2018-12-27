/******************************************************************
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
 *
 ******************************************************************/

#include "NSCppAppUtility.h"
using namespace OIC::Service;
using namespace std;

typedef OIC::Service::NSResult Result;

OIC::Service::NSConsumerService* g_consumerService = nullptr;
shared_ptr< OIC::Service::NSProvider > g_provider;
list< shared_ptr< OIC::Service::NSProvider > > g_providerList;
OIC::Service::NSMessage* g_message = nullptr;
bool g_isConsumerStarted = false, g_isExit = false;
string g_remoteAddress;

void printNotification(OIC::Service::NSMessage notification)
{
    cout << "ProviderID: " << notification.getProviderId() << " Message ID: "
            << notification.getMessageId() << " Title: " << notification.getTitle() << " Body: "
            << notification.getContentText() << endl;
}

void onProviderStateChanged(OIC::Service::NSProviderState state)
{
    cout << "NSProviderStateCallback called" << endl;
    if (state == OIC::Service::NSProviderState::ALLOW)
    {
        cout << "Provider changed to NS_ALLOW State. " << endl;
    }
    else if (state == OIC::Service::NSProviderState::DENY)
    {
        cout << "Provider changed to NS_DENY State. " << endl;
        cout << "------------------------------------" << endl;
    }
    else if (state == OIC::Service::NSProviderState::STOPPED)
    {
        cout << "Provider changed to NS_STOPPED State. " << endl;
    }
    else if (state == OIC::Service::NSProviderState::TOPIC)
    {
        shared_ptr< OIC::Service::NSProvider > provider = g_consumerService->getProvider(
                g_provider->getProviderId());
        if (provider)
        {
            cout << "Provider changed to NS_TOPIC State. " << endl;
            shared_ptr< NSTopicsList > topicList = provider->getTopicList();
            if (topicList)
            {
                for (auto topic : topicList->getTopicsList())
                {
                    cout << "Topic Name: " << topic.getTopicName() << endl;
                    cout << "Topic state: " << (int) topic.getState() << endl;
                }
            }
        }
    }
}

void onNotificationPosted(OIC::Service::NSMessage notification)
{
    cout << "NSMessageReceivedCallback called" << endl;
    g_message = &notification;
    cout << "Receiving Notification" << endl;
    printNotification(notification);
}

void onNotificationSync(OIC::Service::NSSyncInfo sync)
{
    cout << "NSSyncInfoReceivedCallback called" << endl;
    cout << "Consumer SyncInfo changed for providerID: " << sync.getProviderId() << " Message ID: "
            << sync.getMessageId() << endl;

    OIC::Service::NSSyncInfo::NSSyncType state = sync.getState();

    switch (state)
    {
        case OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ:
            cout << "Sync for Read" << endl;
            break;

        case OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_UNREAD:
            cout << "Sync for UnRead" << endl;
            break;

        case OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED:
            cout << "Sync for Delete" << endl;
            break;

        default:
            cout << "Unknown Sync" << endl;
    }
}

void onDiscoverNotificationProvider(shared_ptr< OIC::Service::NSProvider > provider)
{
    if (provider)
    {
        cout << "Notification Resource Discovered" << endl;
        g_provider = provider;
        g_providerList.push_back(provider);

        cout << "Provider Discovered." << endl;
        cout << "Provider ID: " << g_providerList.back()->getProviderId() << endl;
        g_providerList.back()->setListener(onProviderStateChanged, onNotificationPosted,
                onNotificationSync);
    }
}

// Print Menu Items
void showMainMenu()
{
    cout << endl;
    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl << endl;

    cout << "  " << setw(2) << START_CONSUMER << ".  Start Consumer" << endl;
    cout << "  " << setw(2) << STOP_CONSUMER << ".  Stop Consumer" << endl;
    cout << "  " << setw(2) << CONSUMER_SUBSCRIBE << ".  Subscribe As Consumer" << endl;
    cout << "  " << setw(2) << CONSUMER_UNSUBSCRIBE << ".  Unsubscribe As Consumer" << endl;
    cout << "  " << setw(2) << RESCAN_PROVIDER << ".  Manual Scan for Provider" << endl;
    cout << "  " << setw(2) << SEND_SYNCINFO << ".  Send SyncInfo" << endl;
    cout << "  " << setw(2) << CONSUMER_GET_TOPICS << ".  Get Topics" << endl;
    cout << "  " << setw(2) << CONSUMER_UPDATE_TOPICS << ".  Update Topics" << endl;
    cout << "  " << setw(2) << CONSUMER_ENABLE_REMOTE_SERVICE << ".  Enable Remote Service" << endl;
    cout << endl;

    cout << "  " << setw(2) << ConsumerCppAppMenu::CONSUMER_EXIT << ".  Exit" << endl;

    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
    cout << endl;

    cout << "\tPlease Choose a Menu: ";
}

void startConsumer()
{

    Result result = g_consumerService->start(onDiscoverNotificationProvider);

    if (result == Result::OK)
    {
        cout << "Consumer Starts Successfully" << endl;
        g_isConsumerStarted = true;
    }
    else
    {
        cout << "Cannot Start Consumer" << endl;
    }
}

void stopConsumer()
{
    Result result = g_consumerService->stop();

    if (result == Result::OK)
    {
        cout << "Consumer Stopped Successfully" << endl;
        g_isConsumerStarted = false;
    }
    else
    {
        cout << "Cannot Stop Consumer" << endl;
    }
}

void subscribeConsumer()
{
    if (g_isConsumerStarted)
    {
        for (shared_ptr< OIC::Service::NSProvider > provider : g_providerList)
        {
            Result result = provider->subscribe();

            if (result == Result::OK)
            {
                cout << "Subscription is successful" << endl;
            }
            else
            {
                cout << "Subscription is fail" << endl;
            }
        }

        if (g_providerList.size() == 0)
        {
            cout << "Please rescan for provider" << endl;
        }
    }
    else
    {
        cout << "Start consumer first" << endl;
    }
}

void unsubscribeConsumer()
{
    if (g_isConsumerStarted)
    {
        for (shared_ptr< OIC::Service::NSProvider > provider : g_providerList)
        {
            Result result = g_provider->unsubscribe();

            if (result == Result::OK)
            {
                cout << "Unsubscription is successful" << endl;
            }
            else
            {
                cout << "Unsubscription is fail" << endl;
            }
        }

        if (g_providerList.size() == 0)
        {
            cout << "Please try again later" << endl;
        }
    }
    else
    {
        cout << "Start consumer first" << endl;
    }
}

void sendSyncInfo()
{
    Result result;
    if (g_message)
    {
        for (shared_ptr< OIC::Service::NSProvider > provider : g_providerList)
        {
            result = provider->sendSyncInfo(g_message->getMessageId(),
                    OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ);
            if (result == Result::OK)
            {
                cout << "Consumer Send SyncInfo is successful for provide with ID: "
                        << provider->getProviderId() << endl;
            }
            else
            {
                cout << "Consumer Send SyncInfo is fail for provide with ID: "
                        << provider->getProviderId() << endl;
            }
        }

    }
    else
    {
        cout << "No notification found from provider" << endl;
    }
}

void getTopics()
{
    cout << "Getting Topic List" << endl;
    for (shared_ptr< OIC::Service::NSProvider > provider : g_providerList)
    {
        shared_ptr< NSTopicsList > topicList = provider->getTopicList();
        if (topicList)
        {
            for (auto topic : topicList->getTopicsList())
            {
                cout << "Topic Name: " << topic.getTopicName() << endl;
                cout << "Topic state: " << (int) topic.getState() << endl;
            }
        }
    }
}

void updateTopics()
{
    cout << "Updating Topic List" << endl;
    for (shared_ptr< OIC::Service::NSProvider > provider : g_providerList)
    {
        shared_ptr< NSTopicsList > topicList;
        topicList->addTopic(TOPIC_1, NSTopic::NSTopicState::SUBSCRIBED);
        topicList->addTopic(TOPIC_2, NSTopic::NSTopicState::SUBSCRIBED);
        topicList->addTopic(TOPIC_3, NSTopic::NSTopicState::UNSUBSCRIBED);
        topicList->addTopic(TOPIC_4, NSTopic::NSTopicState::SUBSCRIBED);

        provider->updateTopicList(topicList);
    }
}

// This function is for menu selection
void menuSelection(ConsumerCppAppMenu menu)
{
    switch (menu)
    {
        case START_CONSUMER:
            startConsumer();
            break;

        case STOP_CONSUMER:
            stopConsumer();
            break;

        case CONSUMER_SUBSCRIBE:
            subscribeConsumer();
            break;

        case CONSUMER_UNSUBSCRIBE:
            unsubscribeConsumer();
            break;

        case RESCAN_PROVIDER:
            g_consumerService->rescanProvider();
            break;

        case SEND_SYNCINFO:
            sendSyncInfo();
            break;

        case CONSUMER_GET_TOPICS:
            getTopics();
            break;

        case CONSUMER_UPDATE_TOPICS:
            updateTopics();
            break;

        case CONSUMER_ENABLE_REMOTE_SERVICE:
        {
            cout << "Remote Server Address: ";
            cin >> g_remoteAddress;

            g_consumerService->enableRemoteService(g_remoteAddress);
        }
            break;

        case ConsumerCppAppMenu::CONSUMER_EXIT:
        {
            if (g_isConsumerStarted)
            {
                stopConsumer();
            }

            g_isExit = true;
            cout << "Quit from NS Consumer TestApp..." << endl << endl;
        }
            break;

        default:
            cout << "Invalid manu selection, please select a valid menu..." << endl << endl;
    }

}

void *OCProcessThread(void *ptr)
{
    (void) ptr;
    while (!g_isExit)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            cout << "OCStack process error" << endl;
            return NULL;
        }
    }

    return NULL;
}

// This is the main function the RE Test Application
int main(void)
{
    pthread_t processThread;

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        cout << "OCStack init error" << endl;
        return 0;
    }

    pthread_create(&processThread, NULL, OCProcessThread, NULL);

    try
    {
        OC::OCPlatform::stopPresence();
    }
    catch (...)
    {
        cout << "Can't stop presence..." << endl;
    }

    g_consumerService = OIC::Service::NSConsumerService::getInstance();
    while (!g_isExit)
    {
        int menuItem;
        showMainMenu();
        cin >> menuItem;
        menuSelection(ConsumerCppAppMenu(menuItem));
    }

    return 0;
}
