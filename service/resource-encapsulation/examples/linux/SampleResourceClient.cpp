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

#include<iostream>
#include "mutex"
#include "condition_variable"

#include "RCSDiscoveryManager.h"
#include "RCSRemoteResourceObject.h"
#include "RCSResourceAttributes.h"
#include "RCSAddress.h"

#include "OCPlatform.h"

using namespace OC;
using namespace OIC::Service;

constexpr int CORRECT_INPUT = 1;
constexpr int INCORRECT_INPUT = 2;
constexpr int QUIT_INPUT = 3;

std::shared_ptr<RCSRemoteResourceObject>  resource;

const std::string defaultKey = "Temperature";
const std::string resourceType = "?rt=core.TemperatureSensor";
const std::string targetUri = OC_RSRVD_WELL_KNOWN_URI + resourceType;

std::mutex mtx;
std::condition_variable cond;

void startMonitoring();
void startMonitoring();
void stopMonitoring();
void getAttributeFromRemoteServer();
void setAttributeToRemoteServer();
void startCachingWithoutCallback();
void startCachingWithCallback();
void getResourceCacheState();
void getCachedAttributes();
void getCachedAttribute();
void stopCaching();

enum Menu
{
    START_MONITORING = 1,
    STOP_MONITORING,
    GET_ATTRIBUTE,
    SET_ATTRIBUTE,
    START_CACHING_NO_UPDATE,
    START_CACHING_UPDATE,
    GET_RESOURCE_CACHE_STATE,
    GET_CACHED_ATTRIBUTES,
    GET_CACHED_ATTRIBUTE,
    STOP_CACHING,
    QUIT,
    END_OF_MENU
};

typedef void(*ClientMenuHandler)();
typedef int ReturnValue;

struct ClientMenu
{
    Menu m_menu;
    ClientMenuHandler m_handler;
    ReturnValue m_result;
};

ClientMenu clientMenu[] = {
        {Menu::START_MONITORING, startMonitoring, CORRECT_INPUT},
        {Menu::STOP_MONITORING, stopMonitoring, CORRECT_INPUT},
        {Menu::GET_ATTRIBUTE, getAttributeFromRemoteServer, CORRECT_INPUT},
        {Menu::SET_ATTRIBUTE, setAttributeToRemoteServer, CORRECT_INPUT},
        {Menu::START_CACHING_NO_UPDATE, startCachingWithoutCallback, CORRECT_INPUT},
        {Menu::START_CACHING_UPDATE, startCachingWithCallback, CORRECT_INPUT},
        {Menu::GET_RESOURCE_CACHE_STATE, getResourceCacheState, CORRECT_INPUT},
        {Menu::GET_CACHED_ATTRIBUTES, getCachedAttributes, CORRECT_INPUT},
        {Menu::GET_CACHED_ATTRIBUTE, getCachedAttribute, CORRECT_INPUT},
        {Menu::STOP_CACHING, stopCaching, CORRECT_INPUT},
        {Menu::QUIT, [](){}, QUIT_INPUT},
        {Menu::END_OF_MENU, nullptr, INCORRECT_INPUT}
    };

void onResourceDiscovered(std::shared_ptr<RCSRemoteResourceObject> foundResource)
{
    std::cout << "onResourceDiscovered callback" << std::endl;

    std::string resourceURI = foundResource->getUri();
    std::string hostAddress = foundResource->getAddress();

    std::cout << "\t\tResource URI : " << resourceURI << std::endl;
    std::cout << "\t\tResource Host : " << hostAddress << std::endl;

    resource = foundResource;

    cond.notify_all();
}

void onResourceStateChanged(const ResourceState& resourceState)
{
    std::cout << "onResourceStateChanged callback" << std::endl;

    switch(resourceState)
    {
        case ResourceState::NONE:
            std::cout << "\tState changed to : NOT_MONITORING" << std::endl;
            break;

        case ResourceState::ALIVE:
            std::cout << "\tState changed to : ALIVE" << std::endl;
            break;

        case ResourceState::REQUESTED:
            std::cout << "\tState changed to : REQUESTED" << std::endl;
            break;

        case ResourceState::LOST_SIGNAL:
            std::cout << "\tState changed to : LOST_SIGNAL" << std::endl;
            resource = nullptr;
            break;

        case ResourceState::DESTROYED:
            std::cout << "\tState changed to : DESTROYED" << std::endl;
            break;
    }
}

void onCacheUpdated(const RCSResourceAttributes& attributes)
{
    std::cout << "onCacheUpdated callback" << std::endl;

    if (attributes.empty())
    {
        std::cout << "\tAttribute is Empty" << std::endl;
        return;
    }

    for(const auto& attr : attributes)
    {
        std::cout << "\tkey : " << attr.key() << std::endl
                  << "\tvalue : " << attr.value().toString() << std::endl;
    }
}

void onRemoteAttributesReceivedCallback(const RCSResourceAttributes& attributes)
{
    std::cout << "onRemoteAttributesReceivedCallback callback" << std::endl;

    if (attributes.empty())
    {
        std::cout << "\tAttribute is Empty" << std::endl;
        return;
    }

    for(const auto& attr : attributes)
    {
        std::cout << "\tkey : " << attr.key() << std::endl
                  << "\tvalue : " << attr.value().toString() << std::endl;
    }
}

void displayMenu()
{
    std::cout << std::endl;
    std::cout << "1 :: Start Monitoring" << std::endl;
    std::cout << "2 :: Stop Monitoring" << std::endl;
    std::cout << "3 :: Get Attribute" << std::endl;
    std::cout << "4 :: Set Attribute" << std::endl;
    std::cout << "5 :: Start Caching (No update to Application)" << std::endl;
    std::cout << "6 :: Start Caching (Update the application when data change)"<< std::endl;
    std::cout << "7 :: Get Resource cache State" << std::endl;
    std::cout << "8 :: Get Cached Attributes" << std::endl;
    std::cout << "9 :: Get Cached Attribute"  << std::endl;
    std::cout << "10 :: Stop Caching" << std::endl;
    std::cout << "11 :: Stop Server" << std::endl;
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

void startMonitoring()
{
    if (!resource->isMonitoring())
    {
        resource->startMonitoring(&onResourceStateChanged);
        std::cout << "\tHosting Started..." << std::endl;
    }
    else
    {
        std::cout << "\tAlready Started..." << std::endl;
    }
}

void stopMonitoring()
{
    if (resource->isMonitoring())
    {
        resource->stopMonitoring();
        std::cout << "\tHosting stopped..." << std::endl;
    }
    else
    {
       std::cout << "\tHosting not started..." << std::endl;
    }
}

void getAttributeFromRemoteServer()
{
    resource->getRemoteAttributes(&onRemoteAttributesReceivedCallback);
}

void setAttributeToRemoteServer()
{
    std::string key;
    int value;

    RCSResourceAttributes setAttribute;

    std::cout << "\tEnter the Key you want to set : ";
    std::cin >> key;
    std::cout << "\tEnter the value you want to set :";
    std::cin >> value;

    setAttribute[key] = value;
    resource->setRemoteAttributes(setAttribute,
                                  &onRemoteAttributesReceivedCallback);
}

void startCaching(std::function <void (const RCSResourceAttributes&)>cb)
{
    if (!resource->isCaching())
    {
        if(cb) resource->startCaching(&onCacheUpdated);

        else resource->startCaching();

        std::cout << "\tCaching Started..." << std::endl;
    }
    else
    {
        std::cout << "\tAlready Started Caching..." << std::endl;
    }
}

void startCachingWithoutCallback()
{
    startCaching(nullptr);
}

void startCachingWithCallback()
{
    startCaching(onCacheUpdated);
}

void getResourceCacheState()
{
    switch(resource->getCacheState())
    {
        case CacheState::READY:
            std::cout << "\tCurrent Cache State : " << "CACHE_STATE ::READY" << std::endl;
            break;

        case CacheState::UNREADY:
            std::cout << "\tCurrent Cache State : " << "CACHE_STATE ::UNREADY" << std::endl;
            break;

        case CacheState::LOST_SIGNAL:
            std::cout << "\tCurrent Cache State : " << "CACHE_STATE ::LOST_SIGNAL" << std::endl;
            break;

        case CacheState::NONE:
            std::cout << "\tCurrent Cache State : " << "CACHE_STATE ::NONE" << std::endl;
            break;

        default:
            break;
    }
}

void getCachedAttributes()
{
    try
    {
        if (resource->getCachedAttributes().empty())
        {
            std::cout << "\tReceived cached attribute is empty" << std::endl;
        }
        else
        {
            for(const auto& attr : resource->getCachedAttributes())
            {
                std::cout << "\tkey : " << attr.key() << std::endl
                          << "\tvalue : " << attr.value().toString() << std::endl;
            }
        }
    }
    catch (const BadRequestException& e)
    {
        std::cout << "Exception in getCachedAttributes : " << e.what() << std::endl;
    }
}

void getCachedAttribute()
{
    try
    {
        std::cout << "\tkey : " << defaultKey << std::endl
                  << "\tvalue : " << resource->getCachedAttribute(defaultKey).get< int >()
                  << std::endl;
    }
    catch (const BadRequestException& e)
    {
        std::cout << "Exception in getCachedAttribute : " << e.what() << std::endl;
    }
    catch (const BadGetException& e)
    {
        std::cout << "Exception in getCachedAttribute : " << e.what() << std::endl;
    }
}

void stopCaching()
{
    if(resource->isCaching())
    {
        resource->stopCaching();
        std::cout << "\tCaching stopped..." << std::endl;
    }
    else
    {
        std::cout << "\tCaching not started..." << std::endl;
    }
}

int selectClientMenu(int selectedMenu)
{
    for(int i = 0; clientMenu[i].m_menu != Menu::END_OF_MENU; i++)
    {
        if(clientMenu[i].m_menu == selectedMenu)
        {
            clientMenu[i].m_handler();
            return clientMenu[i].m_result;
        }
    }

    std::cout << "Invalid input, please try again" << std::endl;

    return INCORRECT_INPUT;
}

void process()
{
    while(true)
    {
        displayMenu();

        if(selectClientMenu(processUserInput()) == QUIT_INPUT) break;
    }
}

void platFormConfigure()
{
    PlatformConfig config
    {
        OC::ServiceType::InProc, ModeType::Client, "0.0.0.0", 0, OC::QualityOfService::LowQos
    };
    OCPlatform::Configure(config);
}

bool discoverResource()
{
    std::cout << "Wait 2 seconds until discovered." << std::endl;

    RCSDiscoveryManager::getInstance()->discoverResource(RCSAddress::multicast(), targetUri,
                                                         &onResourceDiscovered);

    std::unique_lock<std::mutex> lck(mtx);
    cond.wait_for(lck,std::chrono::seconds(2));

    return resource != nullptr;
}

int main()
{
    platFormConfigure();

    if (!discoverResource())
    {
        std::cout << "Can't discovered Server... Exiting the Client." << std::endl;
        return -1;
    }

    try
    {
        process();
    }
    catch (const std::exception& e)
    {
        std::cout << "main exception : " << e.what() << std::endl;
    }

    std::cout << "Stopping the Client" << std::endl;

    return 0;
}

