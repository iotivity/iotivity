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

#include <RCSAddress.h>

#include <OCPlatform.h>

#include <NotificationConsumer.h>
#include <NotificationDiscoveryManager.h>

using namespace OC;
using namespace OIC::Service;

constexpr int CORRECT_INPUT = 99;
constexpr int INCORRECT_INPUT = 100;

std::shared_ptr<NotificationConsumer> resource;

enum Menu
{
    DISCOVER_NOTIFICATION_SERVICE = 1,
    NOTIFICATION_SUBSCRIBE,
    NOTIFICATION_UNSUBSCRIBE,
    NOTIFICATION_CHECK_SUBSCRIBE_STATUS,
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
    std::cout << "5 :: Quit Sample Application" << std::endl;
    std::cout << "====================================================" << std::endl;
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

void onResourceDiscovered(std::shared_ptr<NotificationConsumer> foundResource)
{
    std::cout << "onResourceDiscovered callback........." << std::endl;

    std::string resourceURI = foundResource->getUri();
    std::string hostAddress = foundResource->getAddress();

    std::cout << "\t\tResource URI : " << resourceURI << std::endl;
    std::cout << "\t\tResource Host : " << hostAddress << std::endl;

    resource = foundResource;
}

void discoverResource()
{
    NotificationDiscoveryManager::getInstance()->discoverNotificationResource(RCSAddress::multicast(),
            &onResourceDiscovered);
}

void onResourceUpdated(NotificationObject *m_notificationObjectPtr)
{
    std::cout << "           onResourceUpdated callback.........." << std::endl;

    if (m_notificationObjectPtr->mNotificationObjectType == NotificationObjectType::Text)
    {
        TextNotification *textNotificationPtr = (TextNotification *) m_notificationObjectPtr;

        std::string nObjType = "text";

        std::cout << std::endl;
        std::cout << std::endl;

        std::cout << "============= NOTIFICATION RECIEVED ============" << std::endl;
        std::cout << "Type                       : " << nObjType << std::endl;
        std::cout << "Time                        : " << textNotificationPtr->mNotificationTime;
        std::cout << "Sender                     : " << textNotificationPtr->mNotificationSender <<
                  std::endl;
        std::cout << "TTL                         : " << textNotificationPtr->mNotificationTtl << std::endl;
        std::cout << "Message                  : " << textNotificationPtr->mNotificationMessage <<
                  std::endl;
        std::cout << "ID                  : " << textNotificationPtr->mNotificationId <<
                  std::endl;
        std::cout << "==========================================" << std::endl;
    }

    if (m_notificationObjectPtr->mNotificationObjectType == NotificationObjectType::Image)
    {
        ImageNotification *imageNotificationPtr = (ImageNotification *) m_notificationObjectPtr;

        std::string nObjType = "image";

        std::cout << std::endl;
        std::cout << std::endl;

        std::cout << "============= NOTIFICATION RECIEVED ============" << std::endl;
        std::cout << "Type                      : " << nObjType << std::endl;
        std::cout << "Time                      : " << imageNotificationPtr->mNotificationTime;
        std::cout << "Sender                   : " << imageNotificationPtr->mNotificationSender <<
                  std::endl;
        std::cout << "TTL                      : " << imageNotificationPtr->mNotificationTtl <<
                  std::endl;
        std::cout << "Icon                     : " << imageNotificationPtr->mNotificationIconUrl <<
                  std::endl;
        std::cout << "Message                : " << imageNotificationPtr->mNotificationMessage <<
                  std::endl;
        std::cout << "ID                  : " << imageNotificationPtr->mNotificationId <<
                  std::endl;
        std::cout << "==========================================" << std::endl;
    }

    if (m_notificationObjectPtr->mNotificationObjectType == NotificationObjectType::Video)
    {
        VideoNotification *videoNotificationPtr = (VideoNotification *) m_notificationObjectPtr;

        std::string nObjType = "video";

        std::cout << std::endl;
        std::cout << std::endl;

        std::cout << "============= NOTIFICATION RECIEVED ============" << std::endl;
        std::cout << "Type                       : " << nObjType << std::endl;
        std::cout << "Time                       : " << videoNotificationPtr->mNotificationTime;
        std::cout << "Sender                    : " << videoNotificationPtr->mNotificationSender <<
                  std::endl;
        std::cout << "TTL                       : " << videoNotificationPtr->mNotificationTtl <<
                  std::endl;
        std::cout << "Video                    : " << videoNotificationPtr->mNotificationVideoUrl <<
                  std::endl;
        std::cout << "ID                  : " << videoNotificationPtr->mNotificationId <<
                  std::endl;
        std::cout << "==========================================" << std::endl;
    }
}

void startSubscribeNotifications()
{
    resource->subscribeNotifications(&onResourceUpdated);
    std::cout << "\tSubscribing started..." << std::endl;
}

void stopSubscribeNotifications()
{
    resource->unSubscribeNotifications();
    std::cout << "\tSubscribing stopped..." << std::endl;
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

        if (ret == QUIT || ret == INCORRECT_INPUT)
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
