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

#include "IUTSimulatorUtils.h"

template <>
void IUTSimulatorUtils::getInputFromUser(bool &input)
{
    std::string userInput = "";

    while (userInput.empty())
    {
        getline(std::cin, userInput);

        if (userInput == "1" || toLower(userInput) == "true")
        {
            input = true;
        }
        else if (userInput == "0" || toLower(userInput) == "false")
        {
            input = false;
        }
        else
        {
            printInput(userInput);
            std::cout << "Please provide boolean value, either 1 or 0 or equivalently true or false" << std::endl;
            userInput = "";
        }
    }

    printInput(input);
}

template <>
void IUTSimulatorUtils::getInputFromUser(std::string &input)
{
    std::string userInput = "";
    while (userInput.empty())
    {
        getline(std::cin, userInput);
        input = userInput;
    }

    printInput(input);
}

template <>
bool IUTSimulatorUtils::getInputFromUser()
{
    bool inputValue;
    std::string userInput = "";

    while (userInput.empty())
    {
        getline(std::cin, userInput);

        if (userInput == "1" || toLower(userInput) == "true")
        {
            inputValue = true;
        }
        else if (userInput == "0" || toLower(userInput) == "false")
        {
            inputValue = false;
        }
        else
        {
            printInput(userInput);
            std::cout << "Please provide boolean value, either 1 or 0 or equivalently true or false" << std::endl;
            userInput = "";
        }
    }
    printInput(inputValue);
    return inputValue;
}


template <>
std::string IUTSimulatorUtils::getInputFromUser()
{
    std::string userInput = "";
    while (userInput.empty())
    {
        getline(std::cin, userInput);
    }

    printInput(userInput);
    return userInput;
}

int IUTSimulatorUtils::getSelectionFromUser(string what, int min, int max)
{
    int selection = getInputFromUser<int>();
    while (selection < min || selection > max)
    {
        cout << "Invalid selection of " << what << ". Please select " << what << " no. between " << min << " & " << max << endl;
        getInputFromUser(selection);
    }

    return selection;
}

shared_ptr< OCResource > IUTSimulatorUtils::getResourceFromUser(vector< shared_ptr< OCResource > > resources)
{
    if (resources.size() > 0)
    {
        cout << "\t" << "Please select resource no. and press Enter:" << endl;
        cout << "\t\t" << "0. Cancel" << endl;
        for (unsigned i = 1; i <= resources.size(); i++)
        {
            cout << "\t\t" << i << ". " << resources.at(i - 1)->uniqueIdentifier() << "    (" << resources.at(i - 1)->host() << ")" << endl;
        }

        int selection = getSelectionFromUser("resource", 0, resources.size());
        if (selection != 0)
        {
            return resources.at(selection - 1);
        }
    }

    return nullptr;
}

shared_ptr< SampleResource > IUTSimulatorUtils::getResourceFromUser(vector< shared_ptr< SampleResource > > resources)
{
    if (resources.size() > 0)
    {
        setlocale(LC_ALL, "");
        cout << "\t" << "Please select resource no. and press Enter:" << endl;
        cout << "\t\t" << "0. Cancel" << endl;
        for (unsigned i = 1; i <= resources.size(); i++)
        {
            cout << "\t\t" << i << ". " << resources.at(i - 1)->getUri() << endl;
        }

        int selection = getSelectionFromUser("resource", 0, resources.size());
        if (selection != 0)
        {
            return resources.at(selection - 1);
        }
    }

    return nullptr;
}

shared_ptr< OCResource > IUTSimulatorUtils::getResourceByURI(vector< shared_ptr< OCResource > > resources, string resourceURI)
{
    for (unsigned i = 0; i < resources.size(); i++)
    {
        std::ostringstream stream;
        stream << resources.at(i)->uniqueIdentifier();
        if (stream.str().find(resourceURI) != std::string::npos)
        {
            return resources.at(i);
        }
    }

    return nullptr;
}

shared_ptr< SampleResource > IUTSimulatorUtils::getResourceByURI(vector< shared_ptr< SampleResource > > resources, string resourceURI)
{
    for (shared_ptr< SampleResource > resource : resources)
    {
        if (resource->getUri() == resourceURI)
        {
            return resource;
        }
    }

    return nullptr;
}

AttributeValue IUTSimulatorUtils::getAttributeValueFromUser()
{
    AttributeValue attributeValue;
    string valueString = "";
    string valueArray = "";
    char valueLine[MAX_ATTRIBUTE_VALUE_LENGTH];

    cout << "Please select attribute data type no. and press Enter: " << endl;
    cout << "\t\t 1. Integer" << endl;
    cout << "\t\t 2. Floating Point - Single Precision" << endl;
    cout << "\t\t 3. Floating Point - Double Precision" << endl;
    cout << "\t\t 4. Boolean" << endl;
    cout << "\t\t 5. String" << endl;
    cout << "\t\t 6. Array" << endl;

    int selection = getSelectionFromUser("data type", 1, 6);

    cout << "Please provide Attribute Value: ";
    switch (selection)
    {
        case 1:
            attributeValue = getInputFromUser<int>();
            break;
        case 2:
            attributeValue = getInputFromUser<float>();
            break;
        case 3:
            attributeValue = getInputFromUser<double>();
            break;
        case 4:
            attributeValue = getInputFromUser<bool>();
            break;
        case 5:
            cin.getline(valueLine, sizeof(attributeValue));
            getline(cin, valueString);
            printInput(valueString);
            attributeValue = valueString;
            break;
        case 6:
            cin.getline(valueLine, sizeof(attributeValue));
            getline(cin, valueArray);
            printInput(valueArray);
            attributeValue = valueArray;
            break;
    }

    return attributeValue;
}

void IUTSimulatorUtils::setResourceHostEndpointFromUser(shared_ptr< OCResource > resource, bool isSecuredClient)
{
    vector< std::string > endpointHosts = resource->getAllHosts();
    if (isSecuredClient && endpointHosts.size() > 0)
    {
        resource->setHost(ResourceHelper::getOnlySecuredHost(endpointHosts));
    }

    cout << "Select one of the endpoint:" << endl;
    cout << "\t\t" << "0. Default Endpoint: " << resource->host() << endl;
    for (unsigned i = 1; i <= endpointHosts.size(); i++)
    {
        cout << "\t\t" << (i) << ". " << endpointHosts[i - 1] << endl;
    }

    int selection = getSelectionFromUser("endpoint", 0, endpointHosts.size());
    if (selection != 0)
    {
        resource->setHost(endpointHosts[selection - 1]);
    }
}

string IUTSimulatorUtils::toLower(string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}


OCRepresentation IUTSimulatorUtils::createRepresentation(std::vector< std::pair<std::string, AttributeValue> > keysValues)
{
    OCRepresentation representation;
    for (std::pair<std::string, AttributeValue> keyValue : keysValues)
    {
        representation.setValue(keyValue.first, keyValue.second);
    }

    return representation;
}

bool IUTSimulatorUtils::isValidIpAddress(string ipAddress, OCConnectivityType ipVersion)
{
    switch(ipVersion)
    {
        case CT_IP_USE_V4:
            struct sockaddr_in sa;
            return  (inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr)) != 0);
        case CT_IP_USE_V6:
            struct sockaddr_in6 sa6;
            return  (inet_pton(AF_INET6, ipAddress.c_str(), &(sa6.sin6_addr)) != 0);
        default:
            return false;
    }
}