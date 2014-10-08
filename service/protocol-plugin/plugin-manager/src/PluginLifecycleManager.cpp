//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

/// @file PluginLifecycleManager.cpp

/// @brief

#include <PluginLifecycleManager.h>

using namespace OIC;

PluginLifeCycleManager::PluginLifeCycleManager()
{
    //struct PluginManagerConfig pmconfig;
    //pmconfig.rootPluinPath = "./../plugins";
    pm = new PluginManager();
}

PluginLifeCycleManager::~PluginLifeCycleManager()
{
    delete(pm);
}

void PluginLifeCycleManager::startPluginsByResourceType(const std::string type)
{
    std::string resourcetype = "";
    int size = type.size();

    //Parsing Resource_Type
    for (int i = size - 1; i > -1 ; i--)
    {
        if (type.at(i) == '.' && type.at(i) == '/')
        {
            break;
        }
        resourcetype += type.at(i);
    }

    //Reverse string(Resource type)

    reverse(resourcetype.begin(), resourcetype.end());

    // printf("resource type = %s\n", resourcetype.c_str());

    //Search Plugin
    std::vector<Plugin> resource_plugin = pm->getPlugins("ResourceType", resourcetype);

    //start Plugin
    std::vector<Plugin>::iterator it;
    for (it = resource_plugin.begin(); it != resource_plugin.end(); it++)
    {
        if (!pm->isStarted(&(*it))) // started : 1, stop : 0
        {
            pm->start(&(*it));
        }
    }
}

void PluginLifeCycleManager::stopPluginsByResourceType(const std::string type)
{
    std::string resourcetype = "";
    int size = type.size();

    //Parsing Resource_Type
    for (int i = size - 1; i > -1 ; i--)
    {
        if (type.at(i) == '.' && type.at(i) == '/')
        {
            break;
        }
        resourcetype += type.at(i);
    }

    //Reverse string(Resource type)

    reverse(resourcetype.begin(), resourcetype.end());

    // printf("resource type = %s\n", resourcetype.c_str());

    //Search Plugin
    std::vector<Plugin> resource_plugin = pm->getPlugins("ResourceType", resourcetype);

    //stop Plugin
    std::vector<Plugin>::iterator it;
    for (it = resource_plugin.begin(); it != resource_plugin.end(); it++)
    {
        if (pm->isStarted(&(*it))) // started : 1, stop : 0
        {
            pm->stop(&(*it));
        }
    }
}