//******************************************************************
//
// Copyright 2017 Samsung Electronics All Rights Reserved.
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

#include "NSCppAppUtility.h"
using namespace OIC::Service;
using namespace std;

typedef OIC::Service::NSResult Result;

OIC::Service::NSProviderService* g_providerService = nullptr;
list< shared_ptr<OIC::Service::NSConsumer> > m_ConsumerList;
bool g_isAutoAccept = false;
bool g_isRemoteEnable = false;
bool g_isExit = false;
bool g_isProviderStarted = false;
string REMOTE_SERVER_ADDRESS;
int id = 0;

void onProviderSyncInfo(OIC::Service::NSSyncInfo syncInfo)
{
    cout << "Provider SyncInfo changed for providerID: " << syncInfo.getProviderId()
            << " Message ID: " << syncInfo.getMessageId() << endl;

    OIC::Service::NSSyncInfo::NSSyncType state = syncInfo.getState();

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

void onSubscribeRequest(shared_ptr<OIC::Service::NSConsumer> consumer)
{
    if (consumer){
        cout << "Subscription request received from Consumer with ID: " << consumer->getConsumerId();
        m_ConsumerList.push_back(consumer);
    }
}

// Print Menu Items
void showMainMenu()
{
    cout << endl;
    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl << endl;

    cout << "  " << setw(2) << START_PROVIDER << ".  Start the Notification Provider(Accepter: Provider)" << endl;
    cout << "  " << setw(2) << START_PROVIDER_AUTO_ACCEPT
            << ".  Start the Notification Provider(Accepter: Consumer)" << endl;
    cout << "  " << setw(2) << STOP_PROVIDER << ".  Stop Provider" << endl;
    cout << "  " << setw(2) << ACCEPT_SUBSCRIPTION << ".  Accept Subscriptions" << endl;
    cout << "  " << setw(2) << REJECT_SUBSCRIPTION << ".  Reject Subcriptions" << endl;
    cout << "  " << setw(2) << SEND_NOTIFICATION << ".  Send Notification" << endl;
    cout << "  " << setw(2) << PROVIDER_SEND_SYNC_INFO << ".  Send Sync Info" << endl;
    cout << "  " << setw(2) << ADD_TOPIC << ".  Add Topic" << endl;
    cout << "  " << setw(2) << DELETE_TOPIC << ".  Delete Topic" << endl;
    cout << "  " << setw(2) << SELECT_TOPIC << ".  Select Topic" << endl;
    cout << "  " << setw(2) << UNSELECT_TOPIC << ".  Unselect Topic" << endl;
    cout << "  " << setw(2) << CONSUMER_TOPICS << ".  Consumer Topics" << endl;
    cout << "  " << setw(2) << PROVIDER_TOPICS << ".  Show Topics" << endl;
    cout << "  " << setw(2) << ACCEPTED_CONSUMER_LIST << ".  List of Accepted Consumer" << endl;
    cout << "  " << setw(2) << PROVIDER_ENABLE_REMOTE_SERVICE << ".  Enable Remote Service" << endl;
    cout << "  " << setw(2) << PROVIDER_DISABLE_REMOTE_SERVICE << ".  Disable Remote Service"
            << endl;
    cout << endl;

    cout << "  " << setw(2) << PROVIDER_EXIT << ".  Exit" << endl;

    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
    cout << endl;

    cout << "\tPlease Choose a Menu: ";
}

void startProvider(bool policy)
{
    g_isAutoAccept = policy;
    OIC::Service::NSProviderService::ProviderConfig config;
    config.subControllability = g_isAutoAccept;
    config.m_subscribeRequestCb = onSubscribeRequest;
    config.m_syncInfoCb = onProviderSyncInfo;
    config.userInfo = "OCF_NOTIFICATION";

    OIC::Service::NSResult result = g_providerService->start(config);

    if (result == Result::OK)
    {
        g_isProviderStarted = true;
        cout << "Provider started successfully" << endl;
    }
    else
    {
        g_isProviderStarted = false;
        cout << "Can't start provider" << endl;
    }
}

void stopProvider()
{
    OIC::Service::NSResult result = g_providerService->stop();

    if (result == Result::OK)
    {
        cout << "Provider stopped successfully" << endl;
    }
    else
    {
        cout << "Cannot stop provider" << endl;
    }
}

void acceptSubscription()
{
    if (m_ConsumerList.size() > 0 ){
        cout << "Accepting Subscription: " << endl;
        for (auto consumer : m_ConsumerList){
            if (consumer != nullptr)
            {
                consumer->acceptSubscription(true);
                cout << "Accepted subscription for consumer with ID: " << consumer->getConsumerId() <<endl;
            }
        }
    }else{
        cout << "No Consumer Subscription request found!!" << endl;
    }
}

void rejectSubscription()
{
    cout << "Rejecting Subscription" << endl;
    for (auto consumer : m_ConsumerList){
        if (consumer != nullptr)
        {
            consumer->acceptSubscription(false);
        }
    }
}

void sendNotification()
{
    OIC::Service::NSMessage msg = g_providerService->createMessage();

    id++;
    msg.setTitle("TestApp title " + id);
    msg.setContentText("TestApp body " + id);

    msg.setSourceName("OCF");
    msg.setTopic(TOPIC_1);

    OIC::Service::NSResult result = g_providerService->sendMessage(msg);

    if (result == Result::OK)
    {
        cout << "Fail to send notification" << endl;
    }
}

string getConsumerId()
{
    string consumerId = "";

    if (m_ConsumerList.size() != 0)
    {
        consumerId = string(m_ConsumerList.back()->getConsumerId());
    }
    else
    {
        cout << "Subscribed consumer is empty." << endl;
    }

    return consumerId;
}

void printConsumerList(NSAcceptedConsumers* aConsumerList)
{
    cout << "Consumer List: " << endl;
    for (auto consumer : aConsumerList->getConsumers()) {
        cout << "id: " << consumer.second->getConsumerId() << endl;
    }
}

// This function is for menu selection
void menuSelection(ProviderCppAppMenu menu)
{
    switch (menu)
    {
        case START_PROVIDER:
            startProvider(false);
            break;

        case START_PROVIDER_AUTO_ACCEPT:
            startProvider(true);
            break;

        case STOP_PROVIDER:
            stopProvider();
            break;

        case ACCEPT_SUBSCRIPTION:
            acceptSubscription();
            break;

        case REJECT_SUBSCRIPTION:
            acceptSubscription();
            break;

        case SEND_NOTIFICATION:
            sendNotification();
            break;

        case PROVIDER_SEND_SYNC_INFO:
        {
            g_providerService->sendSyncInfo(1,
                    OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ);
        }
            break;

        case ADD_TOPIC:
        {
            g_providerService->registerTopic(TOPIC_1);
            g_providerService->registerTopic(TOPIC_2);
            g_providerService->registerTopic(TOPIC_3);
            g_providerService->registerTopic(TOPIC_4);
        }
            break;

        case DELETE_TOPIC:
            g_providerService->unregisterTopic("OCF_TOPIC2");
            break;

        case SELECT_TOPIC:
        {
            shared_ptr<OIC::Service::NSConsumer> consumer = g_providerService->getConsumer(getConsumerId());
            if (consumer != nullptr)
            {
                consumer->setTopic(TOPIC_1);
                consumer->setTopic(TOPIC_2);
                consumer->setTopic(TOPIC_3);
                consumer->setTopic(TOPIC_4);
                std::cout <<  "SelectTopic completed" << std::endl;
            }
        }
            break;

        case UNSELECT_TOPIC:
        {
            shared_ptr<OIC::Service::NSConsumer> consumer = g_providerService->getConsumer(getConsumerId());
            if (consumer != nullptr)
            {
                consumer->unsetTopic("OCF_TOPIC1");
                std::cout <<  "UnSelectTopic completed" << std::endl;
            }
        }
            break;

        case CONSUMER_TOPICS:
        {
            shared_ptr<OIC::Service::NSConsumer> consumer = g_providerService->getConsumer(
                    getConsumerId());
            if (consumer != nullptr)
            {
                shared_ptr<NSTopicsList> nsTopics = consumer->getConsumerTopicList();
                if (nsTopics != nullptr)
                {
                    for (auto it : nsTopics->getTopicsList())
                    {

                        std::cout << it.getTopicName() << std::endl;
                        std::cout << (int) it.getState() << std::endl;
                    }
                }
                std::cout << "GetConsumerTopics completed" << std::endl;
            }
        }
            break;

        case PROVIDER_TOPICS:
        {
            shared_ptr<NSTopicsList> nsTopics = g_providerService->getRegisteredTopicList();
            for (auto it : nsTopics->getTopicsList())
            {

                std::cout << it.getTopicName() << std::endl;
                std::cout << (int) it.getState() << std::endl;
            }
        }
            break;

        case ACCEPTED_CONSUMER_LIST:
        {
            NSAcceptedConsumers* conList = g_providerService->getAcceptedConsumers();
            printConsumerList(conList);
        }
            break;

        case PROVIDER_ENABLE_REMOTE_SERVICE:
        {
            std::cout << "Input the Server Address :";
            std::cin >> REMOTE_SERVER_ADDRESS;
            g_providerService->enableRemoteService(REMOTE_SERVER_ADDRESS);

            g_isRemoteEnable = true;
        }
            break;

        case PROVIDER_DISABLE_REMOTE_SERVICE:
        {
            std::cout << "Input the Server Address :";
            g_providerService->disableRemoteService(REMOTE_SERVER_ADDRESS);
        }
            break;

        case ProviderCppAppMenu::PROVIDER_EXIT:
        {
            if (g_isProviderStarted)
            {
                g_providerService->stop();
            }

            g_isExit = true;
            cout << "Quit from NS Provider TestApp..." << endl << endl;
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
            std::cout << "OCStack process error" << std::endl;
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
        std::cout << "OCStack init error" << std::endl;
        return 0;
    }

    pthread_create(&processThread, NULL, OCProcessThread, NULL);

    g_providerService = OIC::Service::NSProviderService::getInstance();
    while (!g_isExit)
    {
        int menuItem;
        showMainMenu();
        cin >> menuItem;
        menuSelection(ProviderCppAppMenu(menuItem));
    }

    return 0;
}
