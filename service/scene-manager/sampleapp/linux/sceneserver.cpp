//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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
#include <vector>

#include "OCPlatform.h"
#include "RCSDiscoveryManager.h"
#include "RCSRemoteResourceObject.h"
#include "RCSResourceAttributes.h"
#include "RCSAddress.h"
#include "SceneList.h"

using namespace OC;
using namespace OIC::Service;

constexpr int CREATE_SCENE = 1;
constexpr int EXECUTE_SCENE = 2;

struct CloseApp {};

const std::string resourceType = "core.light";
const std::string relativetUri = OC_RSRVD_WELL_KNOWN_URI;

std::mutex mtx;
std::condition_variable cond;

RCSRemoteResourceObject::Ptr g_selectedResource;
std::shared_ptr<RCSRemoteResourceObject> g_discoveredResources;

std::vector<RCSRemoteResourceObject::Ptr> g_foundResourceList;

std::vector<RCSResourceObject::Ptr> g_memberResourceList;

SceneCollection::Ptr g_sceneColObj;
Scene::Ptr g_scene;
std::vector<SceneMemberObject::Ptr> g_sceneMemObj;

typedef std::function<void(std::shared_ptr<RCSRemoteResourceObject>)> DiscoveryCallback;
typedef std::function<void (const RCSResourceAttributes&, int)> ExecuteCallback;

void onExecute(const RCSResourceAttributes& attrs, int /*Code*/)
{
    std::cout << __func__ << std::endl;
}

void configurePlatform()
{
    PlatformConfig config
    {
        OC::ServiceType::InProc, ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos
    };
    OCPlatform::Configure(config);
}

int processUserInput(int min, int max)
{
    assert(min <= max);

    int input;

    std::cin >> input;

    if (!std::cin.fail())
    {
        if(input == max + 1) throw CloseApp();
        if(min <= input && input <= max) return input;
    }

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    throw std::runtime_error("Invalid Input, please try again");
}

void onResourceDiscovered(std::shared_ptr<RCSRemoteResourceObject> foundResource)
{
    std::cout << "onResourceDiscovered callback" << std::endl;

    std::string resourceURI = foundResource->getUri();
    std::string hostAddress = foundResource->getAddress();

    std::cout << "\t\tResource URI : " << resourceURI << std::endl;
    std::cout << "\t\tResource Host : " << hostAddress << std::endl;

    g_foundResourceList.push_back(foundResource);

    cond.notify_all();
}

bool discoverResource()
{
    std::cout << "Wait 2 seconds until discovered." << std::endl;

    try
    {
        RCSDiscoveryManager::getInstance()->discoverResourceByType(RCSAddress::multicast(),
                relativetUri, "core.light", &onResourceDiscovered);
    }
    catch(const RCSPlatformException& e)
    {
         std::cout << e.what() << std::endl;
    }
    std::unique_lock<std::mutex> lck(mtx);
    cond.wait_for(lck, std::chrono::seconds(2));

    return g_discoveredResources != nullptr;
}

void createScene()
{
    if(g_foundResourceList.size() == 0)
    {
        std::cout << "First, find a light resource" << std::endl;
        return ;
    }

    g_sceneColObj = SceneList::getInstance()->createSceneCollection();

    g_scene = g_sceneColObj->addScene("allbulbsson");
    g_scene->setCallback(onExecute);

    for(const auto &it : g_foundResourceList)
    {
        g_sceneMemObj.push_back(g_sceneColObj->addSceneMember(it));
        g_scene->addSceneAction(std::make_shared< SceneAction >(g_sceneMemObj.at(0), "power", "off"));
    }
}

void executeScene()
{
    if(g_scene != nullptr)
        g_scene->execute();
}

int main()
{
    configurePlatform();

    discoverResource();

    int command;

    while (true)
    {
        try
        {
            std::cout << "========================================================\n";
            std::cout << CREATE_SCENE  << ". Create a Scene                       \n";
            std::cout << EXECUTE_SCENE << ". Execute a Scene                      \n";
            std::cout << "========================================================\n";

            command = processUserInput(CREATE_SCENE, EXECUTE_SCENE);

            switch(command)
            {
            case CREATE_SCENE:
                createScene();
                break;
            case EXECUTE_SCENE:
                executeScene();
                break;
            }
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
        catch (const CloseApp&)
        {
            break;
        }
    }

    std::cout << "Stopping the client" << std::endl;

    return 0;
}
