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

void startNotificationProducer()
{
    std::string notificationSender = "Coffee maker";
    notificationProducer.startNotificationManager(notificationSender);
}

void sendNotification()
{
    NotificationObject *notificationObjectPtr = new NotificationObject();
    std::string notificationObjectType;
    std::string notificationMessageType;
    std::string notificationMessage;
    std::string notificationIconUrl;
    std::string notificationVideoUrl;

    NotificationObjectType nText = NotificationObjectType::Text;
    NotificationObjectType nImage = NotificationObjectType::Image;
    NotificationObjectType nVideo = NotificationObjectType::Video;

    std::string space;
    std::cout << "Enter the type of object you want to create (text, image, video)" << std::endl;
    std::cin >> notificationObjectType;

    ///TODO: Message Type attribute is to be handled
    //std::cout << "Enter the type of message (Information, Warning, Emergency)" << std::endl;
    //std::cin >> notificationMessageType;

    std::getline(std::cin, space);

    std::string notificationTime = notificationObjectPtr->mNotificationTime;
    std::string notificationSender = "Coffee maker";
    int notificationTtl = 9;
    int notificationId = notificationObjectPtr->mNotificationId;

    if (notificationObjectType == "text")
    {
        std::cout << "Enter the message you want to send" << std::endl;
        std::getline(std::cin, notificationMessage);

        TextNotification textNotificationObject;

        textNotificationObject.setTextAttributes(notificationMessage, nText ,
                ///TODO: Message Type attribute is to be handled
                //notificationMessageType,
                notificationTime, notificationSender,
                notificationId,
                notificationTtl);

        notificationObjectPtr = &textNotificationObject;

        notificationProducer.sendNotification(nText, notificationObjectPtr);

    }

    if (notificationObjectType == "image")
    {
        std::cout << "Enter the icon url you want to send" << std::endl;
        std::cin >> notificationIconUrl;

        std::cout << "Enter the message you want to send" << std::endl;
        std::cin >> notificationMessage;

        ImageNotification imageNotificationObject;

        imageNotificationObject.setImageAttributes(notificationIconUrl, notificationMessage,
                nImage,
                ///TODO: Message Type attribute is to be handled
                //notificationMessageType ,
                notificationTime,
                notificationSender,
                notificationId,
                notificationTtl);

        notificationObjectPtr = &imageNotificationObject;

        notificationProducer.sendNotification(nImage, notificationObjectPtr);

    }

    if (notificationObjectType == "video")
    {
        std::cout << "Enter the video url you want to send" << std::endl;
        std::cin >> notificationVideoUrl;

        VideoNotification videoNotificationObject;

        videoNotificationObject.setVideoAttributes(notificationVideoUrl, nVideo,
                ///TODO: Message Type attribute is to be handled
                //notificationMessageType,
                notificationTime, notificationSender,
                notificationId,
                notificationTtl);

        notificationObjectPtr = &videoNotificationObject;

        notificationProducer.sendNotification(nVideo, notificationObjectPtr);
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
            std::cout << "START_NOTIFICATION_PRODUCER" << std::endl;
            startNotificationProducer();
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
