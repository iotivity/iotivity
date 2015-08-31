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

#include "PrimitiveResource.h"
#include "RCSResourceObject.h"
#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
using namespace OC::OCPlatform;
using namespace OIC::Service;

constexpr int DEFALUT_VALUE = 0;

constexpr int DEFALUT_SERVER = 1;
constexpr int CUSTOM_SERVER = 2;
constexpr int STOP = 3;

constexpr int INCREASE_TEMPERATURE = 1;
constexpr int DECREASE_TEMPERATURE = 2;
constexpr int STOP_SENSOR = 3;

constexpr int CORRECT_INPUT = 1;
constexpr int INCORRECT_INPUT = 2;
constexpr int QUIT = 3;

std::string resourceUri = "/a/TempSensor";
std::string resourceType = "core.TemperatureSensor";
std::string resourceInterface = "oic.if.";
std::string attributeKey = "Temperature";

RCSResourceObject::Ptr server;

enum class Control{
    INCREASE,
    DECREASE
};

void displayMenu()
{
    std::cout << "====================================================================="
              << std::endl;
    std::cout << "   1 - Creation of Resource [Auto control for requests]" << std::endl;
    std::cout << "   2 - Creation of Resource [Developer control for Get and Set requests]"
              << std::endl;
    std::cout << "   3 - Quit" << std::endl;
    std::cout << "====================================================================="
              << std::endl;
}

void displayControlTemperatureMenu()
{
    std::cout << "========================================================" << std::endl;
    std::cout << "1. Increase Temperature by 10 degree" << std::endl;
    std::cout << "2. Decrease Temperature by 10 degree" << std::endl;
    std::cout << "3. Stop the Sensor" << std::endl;
    std::cout << "========================================================" << std::endl;
}

void printAttribute(const RCSResourceAttributes& attrs)
{
    for(const auto& attr : attrs)
    {
        std::cout << "\tkey : " << attr.key() << "\n\tvalue : "
                  << attr.value().toString() << std::endl;
    }
}

//hander for get request (if developer choose second option for resource Creation)
RCSGetResponse requestHandlerForGet(const RCSRequest& request,
        RCSResourceAttributes& attrs)
{
    std::cout << "Recieved a Get request from Client" << std::endl;

    RCSResourceObject::LockGuard lock(*server);
    RCSResourceAttributes attributes = server->getAttributes();

    std::cout << "\nSending response to Client : " << std::endl;
    printAttribute(attributes);

    return RCSGetResponse::defaultAction();
}

//hander for set request (if developer choose second option for resource Creation)
RCSSetResponse requestHandlerForSet(const RCSRequest& request,
        RCSResourceAttributes& attrs)
{
    std::cout << "Recieved a Set request from Client" << std::endl;

    std::cout << "\n\nSending response to Client : " << std::endl;
    RCSResourceObject::LockGuard lock(*server);
    printAttribute(attrs);
    return RCSSetResponse::defaultAction();
}

void createResource()
{
    server = RCSResourceObject::Builder(resourceUri, resourceType,
                             resourceInterface).setDiscoverable(true).setObservable(true).build();
}

void initServer()
{
    try
    {
        createResource();
    }
    catch (const PlatformException& e)
    {
        std::cout << "Exception in initServer : " << e.what() << std::endl;
    }

    server->setAutoNotifyPolicy(RCSResourceObject::AutoNotifyPolicy::UPDATED);
    server->setSetRequestHandlerPolicy(RCSResourceObject::SetRequestHandlerPolicy::NEVER);
    server->setAttribute(attributeKey, DEFALUT_VALUE);
}

void changeTemperature(Control control)
{
    RCSResourceObject::LockGuard lock(server);
    if(Control::INCREASE == control)
    {
        server->getAttributes()[attributeKey] =
                server->getAttribute<int>(attributeKey) + 10;
        std::cout << "\nTemperature increased by 10 degree" << std::endl;
    }
    else if(Control::DECREASE == control)
    {
        server->getAttributes()[attributeKey] =
                        server->getAttribute<int>(attributeKey) - 10;
        std::cout << "\nTemperature Decreased by 10 degree" << std::endl;
    }
    std::cout << "\nCurrent Temperature : "
            << server->getAttributeValue(attributeKey).get<int>() << std::endl;
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

int selectServerMenu()
{
    switch (processUserInput())
    {
        case DEFALUT_SERVER: // Creation of Resource & Auto control for all requests from Client.
            initServer();
            return CORRECT_INPUT;

        case CUSTOM_SERVER:
            // Creation of Resource & setting get and set handler for handling get and
            // set request from client in application.
            initServer();

            server->setGetRequestHandler(requestHandlerForGet);
            server->setSetRequestHandler(requestHandlerForSet);
            return CORRECT_INPUT;
        case STOP :
            return QUIT;

        default :
            std::cout << "Invalid input, please try again" << std::endl;
            return INCORRECT_INPUT;
    }
}

int selectControlTemperatureMenu()
{
   switch (processUserInput())
   {
       case INCREASE_TEMPERATURE:
           changeTemperature(Control::INCREASE);
           return CORRECT_INPUT;

       case DECREASE_TEMPERATURE:
           changeTemperature(Control::DECREASE);
           return CORRECT_INPUT;

       case STOP_SENSOR:
           return QUIT;

       default:
           std::cout << "Invalid input. Please try again." << std::endl;
           return INCORRECT_INPUT;
   }
}

void process()
{
    while(true)
    {
        displayMenu();

        int ret = selectServerMenu();

        if(ret == QUIT) return;
        if(ret == CORRECT_INPUT) break;
    }

    while(true)
    {
        displayControlTemperatureMenu();

        if (selectControlTemperatureMenu() == QUIT) return;
    }
}

int main(void)
{
    startPresence(3);

    try
    {
        process();
        server = NULL;
    }
    catch (const std::exception& e)
    {
        std::cout << "main exception  : " << e.what() << std::endl;
    }

    std::cout << "Stopping the Server" << std::endl;
}


