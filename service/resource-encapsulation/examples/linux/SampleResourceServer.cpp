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
#include "ResourceObject.h"
#include "OCPlatform.h"
#include "OCApi.h"

using namespace std;
using namespace OC;
using namespace OC::OCPlatform;
using namespace OIC::Service;

std::string resourceUri = "/a/TempSensor";
std::string resourceType = "core.TemperatureSensor";
std::string resourceInterface = "oic.if.";
std::string attributeKey = "Temperature";
ResourceObject::Ptr server;

//display the menu on the screen
void displayMenu()
{
    std::cout << "=====================================================================\n\n";
    std::cout << "   1 - Creation of Resource [Auto control for requests]" << std::endl;
    std::cout << "   2 - Creation of Resource [Developer control for Get and Set requests]" <<std::endl;
    std::cout << "   3 - Quit" << std::endl;
    std::cout << "=====================================================================\n\n";
}

//hander for get request (if developer choose second option for resource Creation)
RCSGetResponse RequestHandlerForGet(const RCSRequest &request,
        ResourceAttributes &attrs)
{
    cout << "Recieved a Get request from Client" << std::endl;
    ResourceObject::LockGuard lock(*server);
    ResourceAttributes attr = server->getAttributes();
    ResourceAttributes::const_iterator iter = attr.begin();
    cout << "\nSending response to Client : " << std::endl;
    for (unsigned int i = 0; i < attr.size(); ++i)
    {
        std::cout << "\tkey : " << iter->key() << "\n\tvalue : " << iter->value().toString() << std::endl;
        ++iter;
    }
    return RCSGetResponse::create(attr);
}

//hander for set request (if developer choose second option for resource Creation)
RCSSetResponse RequestHandlerForSet(const RCSRequest &request,
        ResourceAttributes &attrs)
{
    cout << "Recieved a Set request from Client" << std::endl;
    ResourceAttributes::const_iterator iter = attrs.begin();
    for (unsigned int i = 0; i < attrs.size(); ++i)
    {
        std::cout << "\tkey : " << iter->key() << "\n\tvalue : " << iter->value().toString() << std::endl;
        ++iter;
    }
    iter = attrs.begin();
    cout << "\n\nSending response to Client : " << std::endl;
    for (unsigned int i = 0; i < attrs.size(); ++i)
    {
        std::cout << "\tkey : " << iter->key() << "\n\tvalue : " << iter->value().toString() << std::endl;
        ++iter;
    }
    return RCSSetResponse::create(attrs);
}

int main(void)
{

    int userInput;
    int initialTemperature = 0;
    int temperatureInput;
    bool flag = true;
    startPresence(3);

    displayMenu();

    //userInput for creation of Resource
    std::cin >> userInput;
    std::cin.clear();
    std::cin.ignore (std::numeric_limits<std::streamsize>::max(), '\n');
    try
    {
        while (flag)
        {

            switch (userInput)
            {

                case 1:
               //Creation of Resource & Auto control for all requests from Client
                    {
                        //creation of Resource
                        server = ResourceObject::Builder(resourceUri, resourceType,
                                                         resourceInterface).setDiscoverable(true).setObservable(false).build();
                        std::cout << "Resource created successfully " << std::endl;

                        //setting attribute for the Created Resource
                        server->setAttribute(attributeKey, initialTemperature);
                        flag = false;
                        break;
                    }

                case 2:
            //Creation of Resource & setting get and set handler for handling get and set request from client in application
                    {
                        server = ResourceObject::Builder(resourceUri, resourceType,
                                                         resourceInterface).setDiscoverable(true).setObservable(false).build();
                        std::cout << "Resource created successfully " << std::endl;

                        //setting attribute for the Created Resource
                        server->setAttribute(attributeKey, initialTemperature);

                        //setting handler for handling get request from the client
                        server->setGetRequestHandler(RequestHandlerForGet);

                        //  setting handler for handling set request from the client
                        server->setSetRequestHandler(RequestHandlerForSet);
                        flag = false;
                        break;
                    }
                case 3:
                    return 0;
                default :
                    std::cout << "Invalid Input" << std::endl;
                    break;
            }
        }

        while (true)
        {
            bool end = false;
            cout << endl;
            cout << "========================================================" << endl;
            cout << "1. Increase Temperature by 10 degree" << endl;
            cout << "2. Decrease Temperature by 10 degree" << endl;
            cout << "3. Stop the Sensor" << endl;
            cout << "========================================================" << endl;

            //user Input for increasing/decreasing the temperature
            cin >> temperatureInput;
            if (std::cin.fail())
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input type, please try again" << std::endl;
                continue;
            }

            switch (temperatureInput)
            {
                case 1:
                    {
                        ResourceObject::LockGuard lock(*server);
                        ResourceAttributes attrs = server->getAttributes();

                        attrs[attributeKey] =  (server->getAttribute<int>(attributeKey)  + 10);
                        server->setAttribute(attributeKey, attrs[attributeKey]);
                        cout << "\nTemperature increased by 10 degree" << endl;

                        //getting the current attribute and priniting it on the console
                        attrs = server->getAttributes();
                        cout << "\nCurrent Temperature : ";
                        ResourceAttributes::const_iterator iter = attrs.begin();
                        for (unsigned int i = 0; i < attrs.size(); ++i)
                        {
                            std::cout << iter->value().toString() << std::endl;
                            ++iter;
                        }
                        break;
                    }
                case 2:
                    {
                        ResourceObject::LockGuard lock(*server);
                        ResourceAttributes attrs = server->getAttributes();
                        attrs[attributeKey] =  (server->getAttribute<int>(attributeKey)  - 10);
                        server->setAttribute(attributeKey, attrs[attributeKey]);
                        cout << "\nTemperature decreased by 10 degree" << endl;

                        //getting the current attribute and priniting it on the console
                        attrs = server->getAttributes();
                        cout << "\nCurrent Temperature : ";
                        ResourceAttributes::const_iterator iter = attrs.begin();
                        for (unsigned int i = 0; i < attrs.size(); ++i)
                        {
                            std::cout << iter->value().toString() << std::endl;
                            ++iter;
                        }
                        break;
                    }
                case 3:
                    {
                        cout << "Stopping the Resource" << endl;
                        end = true;
                        break;
                    }
                default:
                    {
                        cout << "Invalid input. Please try again." << endl;
                        break;
                    }
            }
            if (end == true)
            {
                break;
            }
        }
    }
    catch (exception &e)
    {
        cout << "main exception  : " << e.what() << endl;
    }
}


