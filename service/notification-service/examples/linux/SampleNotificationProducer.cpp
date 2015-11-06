/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
 ******************************************************************/
#include <iostream>

#include <NotificationProducer.h>

using namespace OIC::Service;

constexpr int CORRECT_INPUT = 99;
constexpr int INCORRECT_INPUT = 100;

std::string resourceUri = "/oic/notify";
std::string resourceType = "oic.r.notify";
std::string resourceInterface = "oic.if.";
std::string notificationSender;
std::string space;
std::vector<std::pair<int, std::pair<std::string,int>>> notificationList;
#define notify_ttl 20

NotificationProducer notificationProducer(resourceUri, resourceType, resourceInterface);

enum Menu
{
    START_NOTIFICATION_PRODUCER = 1,
    SEND_NOTIFICATION,
    STOP_NOTIFICATION_PRODUCER,
    QUIT,
    END_OF_MENU
};

void displayMenu()
{
    std::cout << "====================================================" << std::endl;
    std::cout << "1 :: Start Notification Producer" << std::endl;
    std::cout << "2 :: Send Notification" << std::endl;
    std::cout << "3 :: Stop Notification Producer" << std::endl;
    std::cout << "4 :: Quit Sample Application" << std::endl;
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

void displayNotificationList()
{
    for (unsigned int i = 0; i < notificationList.size(); i++)
    {
        std::cout << "=========Notification List============" << std::endl;
        std::cout << i + 1 << " Notification Id : " << notificationList[i].first << std::endl;
        std::cout << "  Notification Read : " << notificationList[i].second.second << std::endl;
        std::cout << "===============================" << std::endl;
    }
}

void displayNotificationObjectMenu()
{
    std::cout << "=========Notification Object List==========" << std::endl;
    std::cout << "1. Text " << std::endl;
    std::cout << "2. Image " << std::endl;
    std::cout << "3. Video " << std::endl;
    std::cout << "=================================" << std::endl;
}

void notificationIdListener(int notificationId, std::string hostAddressValue)
{

    for (unsigned int i = 0; i < notificationList.size(); i++)
    {
        if (notificationList[i].first == notificationId)
        {
            if(notificationList[i].second.first != hostAddressValue)
            {
                notificationList[i].second.second++;
            }
        }
    }
    displayNotificationList();

}

void startNotificationProducer(std::string notificationSender)
{
    notificationProducer.startNotificationManager(notificationSender, &notificationIdListener);
}

void sendNotification()
{
    int userInput;
    NotificationObject *notificationObjectPtr = new NotificationObject();
    std::string notificationMessageType;
    std::string notificationMessage;
    std::string notificationIconUrl;
    std::string notificationVideoUrl;

    std::string notificationTime = notificationObjectPtr->mNotificationTime;
    int notificationTtl = notify_ttl;
    int notificationId = notificationObjectPtr->mNotificationId;

    NotificationObjectType nText = NotificationObjectType::Text;
    NotificationObjectType nImage = NotificationObjectType::Image;
    NotificationObjectType nVideo = NotificationObjectType::Video;

    std::cout << "Enter the type of object you want to create : " << std::endl;
    displayNotificationObjectMenu();
    userInput = processUserInput();

    std::getline(std::cin, space);

    switch (userInput)
    {
        case 1:
            {
                std::cout << "Enter the message you want to send" << std::endl;
                std::getline(std::cin, notificationMessage);

                TextNotification textNotificationObject;

                textNotificationObject.setTextAttributes(notificationMessage, nText ,
                        notificationTime, notificationSender,
                        notificationId,
                        notificationTtl);

                notificationObjectPtr = &textNotificationObject;
                notificationList.push_back(std::make_pair(notificationId, std::make_pair(" ",0)));

                notificationProducer.sendNotification(nText, notificationObjectPtr);
                break;
            }

        case 2 :
            {
                std::cout << "Enter the message you want to send" << std::endl;
                std::getline(std::cin, notificationMessage);

                std::cout << "Enter the icon url you want to send" << std::endl;
                std::cin >> notificationIconUrl;

                ImageNotification imageNotificationObject;

                imageNotificationObject.setImageAttributes(notificationIconUrl, notificationMessage,
                        nImage,
                        notificationTime, notificationSender,
                        notificationId,
                        notificationTtl);

                notificationObjectPtr = &imageNotificationObject;
                notificationList.push_back(std::make_pair(notificationId, std::make_pair(" ",0)));

                notificationProducer.sendNotification(nImage, notificationObjectPtr);
                break;
            }

        case 3 :
            {
                std::cout << "Enter the video url you want to send" << std::endl;
                std::cin >> notificationVideoUrl;

                VideoNotification videoNotificationObject;

                videoNotificationObject.setVideoAttributes(notificationVideoUrl, nVideo,
                        notificationTime, notificationSender,
                        notificationId,
                        notificationTtl);

                notificationObjectPtr = &videoNotificationObject;

                notificationList.push_back(std::make_pair(notificationId, std::make_pair(" ",0)));

                notificationProducer.sendNotification(nVideo, notificationObjectPtr);
                break;
            }

        default :
            std::cout << "Invalid input, please try again" << std::endl;
            break;
    }

}

void stopNotificationProducer()
{
}

int selectProviderMenu()
{
    switch (processUserInput())
    {
        // Start Provider
        case Menu::START_NOTIFICATION_PRODUCER :
            std::getline(std::cin, space);
            std::cout << "START_NOTIFICATION_PRODUCER" << std::endl;
            std::cout << "Enter the name of the device hosting the notification resource" << std::endl;
            std::getline(std::cin, notificationSender);
            startNotificationProducer(notificationSender);
            return CORRECT_INPUT;

        // Send Notification
        case Menu::SEND_NOTIFICATION:
            std::cout << "SEND_NOTIFICATION" << std::endl;
            sendNotification();
            return CORRECT_INPUT;

        // Stop Provider
        case Menu::STOP_NOTIFICATION_PRODUCER :
            std::cout << "STOP_NOTIFICATION_PRODUCER" << std::endl;
            stopNotificationProducer();
            return QUIT;

        // Exit
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

        int ret = selectProviderMenu();

        if (ret == QUIT || ret == INCORRECT_INPUT)
            break;
    }
}

int main(void)
{
    std::cout << "Starting Notification Producer Sample" << std::endl;

    try
    {
        process();
    }
    catch (const std::exception &e)
    {
        std::cout << "main exception  : " << e.what() << std::endl;
    }

    std::cout << "Terminating Notification Producer Sample" << std::endl;
}
