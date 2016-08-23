/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

///
/// This test app is for Backward Compatibility and Interoperability test
///
#if defined(__linux__)
#include <unistd.h>
#include <string.h>
#endif

#include <limits>
#include <stdexcept>
#include <functional>
#include <condition_variable>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <map>
#include "boost/assign.hpp"
#include <iostream>
#include "OCPlatform.h"
#include "OCApi.h"
#include "RCClient.h"
#include "RCServer.h"
#include "RCSBundleInfo.h"
#include "RCSResourceContainer.h"
#include "oic_string.h"
#include <vector>

using namespace OC;
using namespace OIC;
using namespace OIC::Service;
using namespace OC::OCPlatform;
using namespace std;
using namespace boost::assign;
namespace PH = std::placeholders;

/***************************** start  define ****************************/
#define CLEAR_SCREEN                                std::system("/usr/bin/clear")
#define START_CONTAINER_STR                         "Start Container"
#define STOP_CONTAINER_STR                          "Stop Container"
#define START_BUNDLE_STR                            "Start Bundle "
#define STOP_BUNDLE_STR                             "Stop Bundle"
#define ADD_BUNDLE_STR                              "Add Bundle"
#define REMOVE_BUNDLE_STR                           "Remove Bundle"
#define ADD_RESOURCES_BUNDLE_STR                    "Add Resources"
#define REMOVE_RESOURCE_STR                         "Remove Resources"
#define DISPLAY_CURRENT_BUNDLE_LIST_STR             "Display current bundle list"
#define DISPLAY_CURRENT_RESOURCE_LIST_STR           "Display current resource list"
#define OBSERVE_RESOURCE_CONTAINER_STR              "Observe resource container"
#define EXIT_STR                                    "Exit"
#define ACTIVATE_BUNDLE_STR                         "Activate bundle"
#define DEACTIVATE_BUNDLE_STR                       "DeActivate bundle"
#define REGISTER_RESOURCE_STR                       "Register resource"
#define UNREGISTER_RESOURCE_STR                     "UnRegister resource"
#define DISCOVER_CONTAINER_LIST_STR                 "Discover container"
#define DISCOVER_CURRENT_BUNDLE_LIST_STR            "Discover current bundle list"
#define DISCOVER_CURRENT_RESOURCE_LIST_STR          "Discover current resource list"
#define CLEAR_LOG_STR                               "Clear log"
#define FREE_MEMORY(src)                            {if(src){ delete src; src=NULL;}}
#define INF                                         9999999

typedef void (*DisplayControlMenuFunc)();
struct CloseApp
{
};

pthread_cond_t m_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
/***************************** end  define ****************************/

/***************************** start declare functions signature ****************************/
int processUserInput(int min, int max);

/***************************** end declare functions signature ****************************/

/***************************** start declare global variables ****************************/
RCServer* rcServerObject;
RCSResourceContainer *m_pContainer;
RCClient* rcClientObject;
static int ADDED_BUNDLE_COUNT = 1;
static int STARTED_BUNDLE_COUNT = 1;
static bool IS_DEFAULT_CONFIGURED = false;

static std::vector< BundleIDInformation > vecBundleID;
static std::vector< ContainerResourceInfo > vecContainerResouceInfo;
const std::vector< std::string > vecResourceTypes;

enum RCTestAPPMenu
{
    SHOW_MENU = 0,
    START_CONTAINER = 1,
    ADD_BUNDLE,
    START_BUNDLE,
    ADD_RESOURCES,
    REMOVE_RESOURCES,
    STOP_BUNDLE,
    REMOVE_BUNDLE,
    DISPLAY_CURRENT_BUNDLE_LIST,
    DISPLAY_CURRENT_RESOURCE_LIST,
    STOP_CONTAINER,
    CLEAR_LOG,
    OBSERVE_RESOURCE_CONTAINER,
    EXIT
} testAppMenu;

/***************************** end declare global variables ****************************/
void initializeBundleInfo()
{
    BundleIDInformation myBundleInfo;

    myBundleInfo.bundleID = TEST_BUNDLE_ID_BMISENSOR;
    myBundleInfo.bundleURI = TEST_BUNDLE_URI_BMISENSOR;
    myBundleInfo.bundlePATH = TEST_BUNDLE_PATH_BMISENSOR;
    myBundleInfo.bundleACRIVATOR = TEST_BUNDLE_ACTIVATOR_BMISENSOR;
    myBundleInfo.bundleResourceType = TEST_RESOURCE_TYPE_BMISENSOR;
    myBundleInfo.bundleResourceURI = TEST_RESOURCE_URI_BMISENSOR;
    myBundleInfo.bundleResourceAdress = TEST_ADDRESS_BMISENSOR;
    myBundleInfo.isBundleStarted = false;
    myBundleInfo.isBundleIDAdded = false;
    myBundleInfo.isBundleResourceAdded = false;

    vecBundleID.push_back(myBundleInfo);

    myBundleInfo.bundleID = TEST_BUNDLE_ID_HUESAMPLE;
    myBundleInfo.bundleURI = TEST_BUNDLE_URI_HUESAMPLE;
    myBundleInfo.bundlePATH = TEST_BUNDLE_PATH_HUESAMPLE;
    myBundleInfo.bundleACRIVATOR = TEST_BUNDLE_ACTIVATOR_HUESAMPLE;
    myBundleInfo.bundleResourceType = TEST_RESOURCE_TYPE_HUESAMPLE;
    myBundleInfo.bundleResourceURI = TEST_RESOURCE_URI_HUESAMPLE;
    myBundleInfo.bundleResourceAdress = TEST_ADDRESS_HUESAMPLE;
    myBundleInfo.isBundleStarted = false;
    myBundleInfo.isBundleIDAdded = false;
    myBundleInfo.isBundleResourceAdded = false;

    vecBundleID.push_back(myBundleInfo);
}

void initializeContainerResourceInfo()
{
    vecResourceTypes =
    {   TEST_RESOURCE_TYPE_BMISENSOR, TEST_RESOURCE_TYPE_HUESAMPLE};
}

int getARandomActiveAndStoppedBundleID()
{
    int flag = INF;

    for (int index = 0; index < vecBundleID.size(); index++)
    {
        if ((vecBundleID[index].isBundleIDAdded) && !(vecBundleID[index].isBundleStarted))
        {
            flag = index;
            break;
        }
    }

    return flag;
}

int getARandomAvailableStartedBundleID()
{
    int flag = INF;

    for (int index = 0; index < vecBundleID.size(); index++)
    {
        if (vecBundleID[index].isBundleIDAdded && vecBundleID[index].isBundleStarted)
        {
            flag = index;
            break;
        }
    }

    return flag;
}

int getARandomAvailableBundleID()
{
    int flag = INF;

    for (int index = 0; index < vecBundleID.size(); index++)
    {
        if (!vecBundleID[index].isBundleIDAdded)
        {
            flag = index;
            break;
        }
    }

    return flag;
}

int getARandomActiveBundleID()
{
    int flag = INF;

    for (int index = 0; index < vecBundleID.size(); index++)
    {
        if (vecBundleID[index].isBundleIDAdded)
        {
            flag = index;
            break;
        }
    }

    return flag;
}

int getActiveBundleIDAvailableForAddResource()
{
    int flag = INF;

    for (int index = 0; index < vecBundleID.size(); index++)
    {
        if (vecBundleID[index].isBundleIDAdded && !vecBundleID[index].isBundleResourceAdded)
        {
            flag = index;
            break;
        }
    }

    return flag;
}

int getActiveBundleIDThatUsedForAddResource()
{
    int flag = INF;

    for (int index = 0; index < vecBundleID.size(); index++)
    {
        if (vecBundleID[index].isBundleIDAdded && vecBundleID[index].isBundleResourceAdded)
        {
            flag = index;
            break;
        }
    }

    return flag;
}

void getCurrentPath(std::string *pPath)
{
    char buffer[MAX_PATH];

    if (!pPath->empty())
        pPath->clear();

#if defined(__linux__)
    char *strPath = NULL;
    int length = readlink("/proc/self/exe", buffer, MAX_PATH - 1);

    if (length != -1)
    {
        buffer[length] = '\0';
        strPath = strrchr(buffer, '/');

        if (strPath != NULL)
        *strPath = '\0';
    }
#endif

    pPath->append(buffer);
}

void runStartContainer()
{
    CLEAR_SCREEN;
    testAppMenu = SHOW_MENU;

    std::string filePath;

    if(RCServer::g_bTestAppContainerStarted)
    {
        cout << "Resource container has been started already..." << endl;
        return;
    }

    cout << "Press 0 to start container with default configuration..." <<endl;
    cout << "Press 1 to start container without default configuration..." <<endl;
    int option = processUserInput(0,1);
    if(option == 0)
    {
        getCurrentPath(&filePath);
        filePath.append("/");
        filePath.append(TEST_CONFIG_PATH);
        IS_DEFAULT_CONFIGURED = true;
    }

    rcServerObject->rcStartContainer(m_pContainer,filePath);
}

void runStopContainer()
{
    CLEAR_SCREEN;
    testAppMenu = SHOW_MENU;

    if(!RCServer::g_bTestAppContainerStarted)
    {
        cout << RESOURCE_CONTAINER_IS_NOT_STARTED_MSG << endl;
        return;
    }

    rcServerObject->rcStopContainer(m_pContainer);
    initializeBundleInfo();
    initializeContainerResourceInfo();
    ADDED_BUNDLE_COUNT=1;
    STARTED_BUNDLE_COUNT=1;
    IS_DEFAULT_CONFIGURED = false;
}

void runStartBundle()
{
    CLEAR_SCREEN;
    testAppMenu = SHOW_MENU;

    if(STARTED_BUNDLE_COUNT>2)
    {
        cout << "Sorry.There is no bundle id is remaining to start.." << endl;
        return;
    }

    int flag = getARandomActiveAndStoppedBundleID();
    if(flag == INF)
    {
        cout <<"Existing bundle id is already started..."<< endl;
        return;
    }

    vecBundleID[flag].isBundleStarted = true;
    rcServerObject->rcStartBundle(m_pContainer,vecBundleID[flag].bundleID);
    STARTED_BUNDLE_COUNT++;
    if(vecBundleID[flag].isBundleResourceAdded)
    {
        std::map< std::string, std::string > resourceParams;
        resourceParams.insert(std::make_pair("resourceType", vecBundleID[flag].bundleResourceType));
        resourceParams.insert(std::make_pair("address", vecBundleID[flag].bundleResourceAdress));
        rcServerObject->rcAddBundleResource(m_pContainer,vecBundleID[flag].bundleID, vecBundleID[flag].bundleResourceURI,resourceParams,false);
    }
}

void runStopBundle()
{
    CLEAR_SCREEN;
    testAppMenu = SHOW_MENU;

    if(STARTED_BUNDLE_COUNT<=1)
    {
        cout << "Sorry.There is no bundle id is remaining to stop.." << endl;
        return;
    }

    int flag = getARandomAvailableStartedBundleID();
    if(flag == INF)
    {
        cout <<"Existing bundle id is already stopped..."<< endl;
        return;
    }

    rcServerObject->rcStopBundle(m_pContainer,vecBundleID[flag].bundleID);
    vecBundleID[flag].isBundleStarted = false;
    STARTED_BUNDLE_COUNT--;
}

void runAddBundle()
{
    CLEAR_SCREEN;
    testAppMenu = SHOW_MENU;

    std::map< std::string, std::string > bundleParams;

    if(ADDED_BUNDLE_COUNT>2)
    {
        cout << "Sorry.You can add maximum 2 bundle id for this test application..." << endl;
        return;
    }

    int flag = getARandomAvailableBundleID();
    if(flag == INF)
    {
        cout <<"Existing bundle id is already added..."<< endl;
        return;
    }

    rcServerObject->rcAddBundle(m_pContainer,vecBundleID[flag].bundleID,vecBundleID[flag].bundleURI,vecBundleID[flag].bundlePATH, vecBundleID[flag].bundleACRIVATOR,bundleParams);
    vecBundleID[flag].isBundleIDAdded = true;
    ADDED_BUNDLE_COUNT++;
}

void runRemoveBundle()
{
    CLEAR_SCREEN;
    testAppMenu = SHOW_MENU;

    if(ADDED_BUNDLE_COUNT<=1)
    {
        cout << "Sorry.There is no added bundle id to remove..." << endl;
        return;
    }

    int flag = getARandomActiveBundleID();
    if(flag == INF)
    {
        cout <<"Existing bundle id is already removed..."<< endl;
        return;
    }

    rcServerObject->rcRemoveBundle(m_pContainer,vecBundleID[flag].bundleID);

    vecBundleID[flag].isBundleIDAdded = false;
    vecBundleID[flag].isBundleStarted = false;
    vecBundleID[flag].isBundleResourceAdded = false;
    STARTED_BUNDLE_COUNT--;
    ADDED_BUNDLE_COUNT--;
}

void runAddResources()
{
    CLEAR_SCREEN;
    testAppMenu = SHOW_MENU;

    std::map< std::string, std::string > resourceParams;

    if(ADDED_BUNDLE_COUNT<=1)
    {
        cout << "Please add bundle first..." <<endl;
        return;
    }

    int flag = getActiveBundleIDAvailableForAddResource();

    if(flag == INF)
    {
        cout <<"Existing bundle id is already used.Please add bundle first..."<< endl;
        return;
    }

    resourceParams.insert(std::make_pair("resourceType", vecBundleID[flag].bundleResourceType));
    resourceParams.insert(std::make_pair("address", vecBundleID[flag].bundleResourceAdress));

    rcServerObject->rcAddBundleResource(m_pContainer,vecBundleID[flag].bundleID, vecBundleID[flag].bundleResourceURI,resourceParams,true);

    vecBundleID[flag].isBundleResourceAdded = true;
}

void runRemoveResources()
{
    CLEAR_SCREEN;
    testAppMenu = SHOW_MENU;

    int flag = getActiveBundleIDThatUsedForAddResource();
    if(flag == INF)
    {
        cout <<"Sorry.There is no active bundle having this resource..."<< endl;
        return;
    }

    rcServerObject->rcRemoveBundleResource(m_pContainer,vecBundleID[flag].bundleID, vecBundleID[flag].bundleResourceURI);

    vecBundleID[flag].isBundleResourceAdded = false;
}

void runDisplayCurrentBundleList()
{
    CLEAR_SCREEN;
    testAppMenu = SHOW_MENU;

    rcServerObject->displayCurrentBundleList(m_pContainer);
}

void runDisplayCurrentResourceList()
{
    CLEAR_SCREEN;
    testAppMenu = SHOW_MENU;

    std::string bundleId;

    cout << "Existing bundle resource's are : " << endl;

    for(int index=0;index<vecBundleID.size();index++)
    {
        if(vecBundleID[index].isBundleIDAdded && vecBundleID[index].isBundleResourceAdded)
        {
            rcServerObject->displayCurrentResourceList(m_pContainer,vecBundleID[index].bundleID);
        }
    }

    if(IS_DEFAULT_CONFIGURED)
    {
        rcServerObject->displayCurrentResourceList(m_pContainer,TEST_BUNDLE_ID_DEFAULT_BUNDLE_ID);
    }
}

void runObserveResourceContainer()
{
    CLEAR_SCREEN;
    testAppMenu = SHOW_MENU;

    rcClientObject->rcObserveResourceContainer(m_pContainer,vecResourceTypes);
}

void runClearLog()
{
    CLEAR_SCREEN;
    testAppMenu = SHOW_MENU;
    RCServer::m_errorMessage = "";
}

void clearMemory()
{
    FREE_MEMORY(rcServerObject);
    m_pContainer = nullptr;
    RCServer::g_intBundleStartCounter = 0;
    RCServer::g_bTestAppContainerStarted = false;
    RCServer::g_bTestBundleStarted = false;
    RCServer::m_errorMessage = "";
    RCFoundResourcesListener::g_foundResource = false;
}

int processUserInput(int min, int max)
{
    assert(min <= max);

    int input;

    std::cin >> input;

    if (!std::cin.fail())
    {
        if (min <= input && input <= max)
            return input;
    }

    std::cin.clear();
    std::cin.ignore(std::numeric_limits < std::streamsize > ::max(), '\n');

    throw std::runtime_error("Invalid Input, please try again");
}

void printTestAppMenu()
{
    cout << endl;
    cout << "************ Server Menu **************" << endl;
    cout << "1. " << START_CONTAINER_STR << endl;
    cout << "2. " << ADD_BUNDLE_STR << endl;
    cout << "3. " << START_BUNDLE_STR << endl;
    cout << "4. " << ADD_RESOURCES_BUNDLE_STR << endl;
    cout << "5. " << REMOVE_RESOURCE_STR << endl;
    cout << "6. " << STOP_BUNDLE_STR << endl;
    cout << "7. " << REMOVE_BUNDLE_STR << endl;
    cout << "8. " << DISPLAY_CURRENT_BUNDLE_LIST_STR << endl;
    cout << "9. " << DISPLAY_CURRENT_RESOURCE_LIST_STR << endl;
    cout << "10." << STOP_CONTAINER_STR << endl;
    cout << "11." << CLEAR_LOG_STR << endl;
    cout << "************ Client Menu **************" << endl;
    cout << "12." << OBSERVE_RESOURCE_CONTAINER_STR << endl;
    cout << "13." << EXIT_STR << endl;
    cout << "************************************" << endl;
}

int main()
{
    rcServerObject->getInstance();
    rcClientObject->getInstance();
    m_pContainer = RCSResourceContainer::getInstance();

    initializeBundleInfo();
    initializeContainerResourceInfo();

    bool exit = false;

    int option = 0;

    while (!exit)
    {
        try
        {
            switch (testAppMenu)
            {
                case SHOW_MENU:
                    printTestAppMenu();
                    option = processUserInput(START_CONTAINER, EXIT);
                    testAppMenu = (RCTestAPPMenu) option;
                    break;

                case START_CONTAINER:
                    runStartContainer();
                    break;

                case STOP_CONTAINER:
                    runStopContainer();
                    break;

                case START_BUNDLE:
                    runStartBundle();
                    break;

                case STOP_BUNDLE:
                    runStopBundle();
                    break;

                case ADD_BUNDLE:
                    runAddBundle();
                    break;

                case REMOVE_BUNDLE:
                    runRemoveBundle();
                    break;

                case ADD_RESOURCES:
                    runAddResources();
                    break;

                case REMOVE_RESOURCES:
                    runRemoveResources();
                    break;

                case DISPLAY_CURRENT_BUNDLE_LIST:
                    runDisplayCurrentBundleList();
                    break;

                case DISPLAY_CURRENT_RESOURCE_LIST:
                    runDisplayCurrentResourceList();
                    break;

                case CLEAR_LOG:
                    runClearLog();
                    break;

                case OBSERVE_RESOURCE_CONTAINER:
                    runObserveResourceContainer();
                    break;

                case EXIT:
                    exit = true;
                    break;

                default:
                    break;
            }
        }
        catch (OCException e)
        {
            std::cout << "Caught OCException [Code: " << e.code() << " Reason: " << e.reason()
                    << std::endl;
        }
        catch (const CloseApp&)
        {
            break;
        }
        catch (std::runtime_error& e)
        {
            CLEAR_SCREEN;
            cout << e.what() << endl;
        }
    }

    clearMemory();
    std::cout << "RC Test App is going to stop..." << std::endl;
    cin.get();

    return 0;
}
