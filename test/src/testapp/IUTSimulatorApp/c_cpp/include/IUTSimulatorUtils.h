/******************************************************************
*
* Copyright 2018 Open Connectivity Foundation All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <initializer_list>

#ifdef __LINUX__
#include <arpa/inet.h>
#endif
#ifdef __WINDOWS__
#include <winsock2.h>
#endif

#include "ResourceHelper.h"
#include "SampleResource.h"



class IUTSimulatorUtils
{

public:
    IUTSimulatorUtils() = delete;

    static int getSelectionFromUser(std::string what, int min, int max);

    static std::shared_ptr< OCResource > getResourceFromUser(std::vector< std::shared_ptr< OCResource > > resources);

    static std::shared_ptr< SampleResource > getResourceFromUser(std::vector< std::shared_ptr< SampleResource > > resources);

    static std::shared_ptr< OCResource > getResourceByURI(std::vector< std::shared_ptr< OCResource > > resources, std::string resourceType);

    static std::shared_ptr< SampleResource > getResourceByURI(std::vector< std::shared_ptr< SampleResource > > resources, std::string resourceType);

    static AttributeValue getAttributeValueFromUser();

    static void setResourceHostEndpointFromUser(std::shared_ptr< OCResource > resource, bool isSecuredClient);

    static bool isValidIpAddress(std::string ipAddress, OCConnectivityType ipVersion);

    static std::string toLower(std::string str);

    static OCRepresentation createRepresentation(std::vector< std::pair<std::string, AttributeValue> > keysValues  = {});


    template <class T>
    static void printInput(T input)
    {
        time_t t = time(0);
        struct tm *now = localtime(&t);

        char time_str [150];
        strftime(time_str, 150, "%Y-%m-%d-%H-%M-%S: ", now);
        std::cout << time_str << "Recived input: " << input << std::endl;
    }

    template <class T>
    static void getInputFromUser(T &input)
    {
        std::string userInput = "";

        getline(std::cin, userInput);
        while(!(stringstream(userInput) >> input))
        {
            if (userInput.empty())
            {
            }
            else if (input <= std::numeric_limits<T>::min() || input >= std::numeric_limits<T>::max())
            {
                printInput(userInput);
                std::cout << "This number is out of allowed scope!" << std::endl;
            }
            else
            {
                printInput(userInput);
                std::cout << "Wrong type of input, should be number!" << std::endl;
            }
            userInput = "";
            getline(std::cin, userInput);
        }

        printInput(input);
    }

    template <class T>
    static T getInputFromUser()
    {
        T inputValue;
        std::string userInput = "";

        getline(std::cin, userInput);
        while(!(stringstream(userInput) >> inputValue))
        {
            if (userInput.empty())
            {
            }
            else if (inputValue <= std::numeric_limits<T>::min() || inputValue >= std::numeric_limits<T>::max())
            {
                printInput(userInput);
                std::cout << "This number is out of allowed scope!" << std::endl;
            }
            else
            {
                printInput(userInput);
                std::cout << "Wrong type of argument, should be number!" << std::endl;
            }
            userInput = "";
            getline(std::cin, userInput);
        }

        printInput(inputValue);
        return inputValue;
    }
};