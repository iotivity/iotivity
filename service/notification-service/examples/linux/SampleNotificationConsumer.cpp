//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <vector>

#include <RCSAddress.h>

#include <OCPlatform.h>

#include <NotificationConsumer.h>
#include <NotificationDiscoveryManager.h>

using namespace OC;
using namespace OIC::Service;

constexpr int CORRECT_INPUT = 99;
constexpr int INCORRECT_INPUT = 100;

std::shared_ptr<NotificationConsumer> selectedResource;
std::vector<std::shared_ptr<NotificationConsumer>> notificationResourceList;
std::vector<std::pair<int, std::string>> notificationList;
std::vector<std::string> DeviceList;
unsigned int notifyAckId;
char userOption;

enum Menu
{
    DISCOVER_NOTIFICATION_SERVICE = 1,
    NOTIFICATION_SUBSCRIBE,
    NOTIFICATION_UNSUBSCRIBE,
    NOTIFICATION_CHECK_SUBSCRIBE_STATUS,
    NOTIFICATION_DISPLAY_LIST,
    QUIT,
    END_OF_MENU
};

void displayMenu()
{
    std::cout << "====================================================" << std::endl;
    std::cout << "1 :: Discovery Notification Service (resource)" << std::endl;
    std::cout << "2 :: Subscribe for notifications" << std::endl;
    std::cout << "3 :: Un-subscribe from notifications" << std::endl;
    std::cout << "4 :: Check notifications status (IsSubscribing ?)" << std::endl;
    std::cout << "5 :: Display notification resouce list" << std::endl;
    std::cout << "6 :: Quit Sample Application" << std::endl;
    std::cout << "====================================================" << std::endl;
}

int checkNotificationList(NotificationObject *notificationObjectPtr,
                          std::string producerHostAddress)
{
    for (unsigned int i = 0; i < notificationList.size(); i++)
    {
        if (notificationObjectPtr->getId() == notificationList[i].first)
        {
            if (producerHostAddress == notificationList[i].second)
            {
                return 1;
            }
        }
    }
    return 0;
}

void onResourceUpdated(NotificationObject *notificationObjectPtr, std::string producerHostAddress,
                       std::shared_ptr<NotificationConsumer> notificationResource)
{
    if (notificationObjectPtr == NULL)
    {
        std::cout << "ERROR: notification object pointer is NULL" << std::endl;
        return;
    }
    std::cout << "-------------Notification Received-----------------" << std::endl << std::endl;
    std::cout << "notificationId: " << notificationObjectPtr->getId() << std::endl;
    std::cout << "details: { " << std::endl;

    std::cout << "          type: " << (int)notificationObjectPtr->getType() << std::endl;
    std::cout << "          priority: " << (int)notificationObjectPtr->getPriority() << std::endl;
    std::cout << "          ttl: " << notificationObjectPtr->getTtl() << std::endl;
    std::cout << "          time: " << notificationObjectPtr->getTime() << std::endl;
    std::cout << "          senderDetails: ";

    std::vector<std::string> myvector = notificationObjectPtr->getSender();
    for (std::vector<std::string>::iterator it = myvector.begin() ; it != myvector.end(); ++it)
        std::cout << ' ' << *it;

    std::cout << std::endl;
    std::cout << "    } " << std::endl;
    TextNotification *textptr = (TextNotification *) notificationObjectPtr;
    std::cout << "payload: " << textptr ->getMessage() << std::endl;


    std::cout << "-------------Notification End-----------------" << std::endl << std::endl;
    if (checkNotificationList(notificationObjectPtr, producerHostAddress))
    {
        return;
    }

    std::cout << "           onResourceUpdated callback.........." << std::endl;
    std::string messageType;

    notificationList.push_back(std::make_pair(notificationObjectPtr->getId(),
                               producerHostAddress));
}

int processUserInput()
{
    int userInput;
    std::cin >> userInput;
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }
    return userInput;
}

void displayNotificationResourceList()
{
    unsigned int i;

    for (i = 0; i < notificationResourceList.size(); i++)
    {
        std::cout << "\t\t" << i + 1 << ". Resource Name : " << DeviceList[i] << std::endl;
        std::cout << "\t\tResource URI : " << notificationResourceList[i]->getUri() << std::endl;
        std::cout << "\t\tResource Host : " << notificationResourceList[i]->getAddress() << std::endl;
    }
}

void startSubscribeNotifications()
{
    unsigned int userInput;
    unsigned int resourceOption;
    displayNotificationResourceList();

    if (notificationResourceList.size() > 0)
    {
        std::cout << "Enter the no of resources you want to subscribe to" << std::endl;
        std::cin >> userInput;

        for (unsigned int i = 0; i < userInput; i++)
        {
            std::cout << "Enter the Resource no to which you want to subscribe to" << std::endl;
            std::cin >> resourceOption;

            if (notificationResourceList[resourceOption - 1] == NULL)
            {
                std::cout << "Error: Resource no " << resourceOption << " is NULL...." << std::endl;
                continue;
            }
            if (!notificationResourceList[resourceOption - 1]->isSubscribing())
            {
                notificationResourceList[resourceOption - 1]->subscribeNotifications(&onResourceUpdated);
                std::cout << "\tSubscribing started..." << std::endl;
            }
            else
            {
                std::cout << "\tAlready Subscribing to the resource....." << std::endl;
            }
        }
    }
    else
    {
        std::cout << "Resource List is empty....." << std::endl;
    }
}

void stopSubscribeNotifications()
{
    unsigned int userInput;
    unsigned int resourceOption;
    displayNotificationResourceList();

    while (1)
    {
        if (notificationResourceList.size() > 0)
        {
            std::cout << "Do you want to unSubscribe to all the resources (y/n) :" << std::endl;
            std::cin >> userOption;

            if (userOption == 'y' || userOption == 'Y')
            {
                for (unsigned int i = 0; i < notificationResourceList.size(); i++)
                {
                    if (notificationResourceList[i] == NULL)
                    {
                        std::cout << "Error: Resource no " << i + 1 << " is NULL...." << std::endl;
                        continue;
                    }
                    if (notificationResourceList[i]->isSubscribing())
                    {
                        notificationResourceList[i]->unSubscribeNotifications();
                        std::cout << "\tSubscribing stopped..." << std::endl;
                    }
                    else
                    {
                        std::cout << "\tError: Not Subscribing to the selected resource...." << std::endl;
                    }
                }
                notificationList.clear();
                break;
            }
            else if (userOption == 'n' || userOption == 'N')
            {
                std::cout << "Enter the no of resources you want to unSubscribe to" << std::endl;
                std::cin >> userInput;

                for (unsigned int i = 0; i < userInput; i++)
                {
                    std::cout << "Enter the Resource no to which you want to unSubscribe to" << std::endl;
                    std::cin >> resourceOption;

                    if (notificationResourceList[resourceOption - 1] == NULL)
                    {
                        std::cout << "Error: Resource no " << i + 1 << " is NULL...." << std::endl;
                        continue;
                    }
                    if (notificationResourceList[resourceOption - 1]->isSubscribing())
                    {
                        notificationResourceList[resourceOption - 1]->unSubscribeNotifications();
                        std::cout << "\tSubscribing stopped..." << std::endl;
                    }
                    else
                    {
                        std::cout << "\tNot Subscribing to the selected resource...." << std::endl;
                    }
                }
                break;
            }
            else
            {
                std::cout << "Invalid input, please try again" << std::endl;
            }
        }
        else
        {
            std::cout << "Resource List is empty....." << std::endl;
            break;
        }
    }
}

void onResourceDiscovered(std::shared_ptr<NotificationConsumer> foundResource)
{
    std::cout << "onResourceDiscovered callback........." << std::endl;

    std::string resourceUri = foundResource->getUri();
    DeviceList.push_back(resourceUri);
    notificationResourceList.push_back(foundResource);
}

void discoverResource()
{
    notificationResourceList.clear();
    DeviceList.clear();
    notificationList.clear();

    NotificationDiscoveryManager::getInstance()->discoverNotificationResource(RCSAddress::multicast(),
            &onResourceDiscovered);
}

void checkSubscribeStatus()
{

}

int selectConsumerMenu()
{
    switch (processUserInput())
    {
            // Start Discovery
        case Menu::DISCOVER_NOTIFICATION_SERVICE:
            std::cout << "DISCOVER_NOTIFICATION_SERVICE" << std::endl;
            discoverResource();
            return CORRECT_INPUT;

            // Send Subscription Request
        case Menu::NOTIFICATION_SUBSCRIBE:
            std::cout << "NOTIFICATION_SUBSCRIBE" << std::endl;
            startSubscribeNotifications();
            return CORRECT_INPUT;

            // Send Un-Subscription Request
        case Menu::NOTIFICATION_UNSUBSCRIBE :
            std::cout << "NOTIFICATION_UNSUBSCRIBE" << std::endl;
            stopSubscribeNotifications();
            return CORRECT_INPUT;

            // Check state of subscription
        case Menu::NOTIFICATION_CHECK_SUBSCRIBE_STATUS :
            std::cout << "NOTIFICATION_CHECK_SUBSCRIBE_STATUS" << std::endl;
            checkSubscribeStatus();
            return CORRECT_INPUT;

            // Check state of subscription
        case Menu::NOTIFICATION_DISPLAY_LIST :
            std::cout << "NOTIFICATION_DISPLAY_LIST" << std::endl;
            displayNotificationResourceList();
            return CORRECT_INPUT;

            // Check state of subscription
        case Menu::QUIT :
            std::cout << "QUIT" << std::endl;
            return QUIT;

        default :
            std::cout << "Invalid input, please try again" << std::endl;
            return INCORRECT_INPUT;
    }
}

void process()
{
    while (true)
    {
        displayMenu();

        int ret = selectConsumerMenu();

        if (ret == QUIT )
            break;
    }
}

int main()
{
    std::cout << "Starting the Consumer Sample Application" << std::endl;

    try
    {
        process();
    }
    catch (const std::exception &e)
    {
        std::cout << "main exception : " << e.what() << std::endl;
    }

    std::cout << "Stopping the Consumer Sample Application" << std::endl;
    return 0;
}
