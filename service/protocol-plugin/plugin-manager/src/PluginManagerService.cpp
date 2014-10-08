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

/// @file PluginManagerService.cpp

/// @brief

#include "PluginManager.h"
#include <iostream>

using namespace OIC;
using namespace std;
using namespace OC;

OCPlatform *g_platform;

void printMenu()
{
    cout << endl << "====== Menu ======" << endl;
    cout << "1. Set Plugins Directory" << endl;
    cout << "2. Add a Plugin " << endl;
    cout << "3. Delete a Plugin" << endl;
    cout << "4. Start " << endl;
    cout << "5. Stop" << endl;
    cout << "6. List" << endl;
    cout << "0. Exit " << endl << endl;
}

void loading(const int recursive)
{
    PluginManager *pm = PluginManager::Getinstance();
    string str;
    if (recursive == 1)
    {
        cout << "Enter Plugins Directory Path = ";
        cin >> str;
        cout << "All plugins in all sub-directories of the path will be added and new ";
        cout << "plugins will be automatically detected and added from now on." << endl;
        cout << "To see the current list of plugins, select \"List\" menu." << endl;
        pm->registerAllPlugin(str);
    }
    else
    {
        cout << "Enter Path of the plugin = ";
        cin >> str;
        pm->registerPlugin(str);
    }

}

void unregister()
{
    PluginManager *pm = PluginManager::Getinstance();
    Plugin *plugin = new Plugin;
    string str1;
    cout << "Enter Plugin ID = " ;
    cin >> str1;
    plugin->setValue("Id", str1);
    //pm->unregisterPlugin(plugin);
    pm->unregisterAllPlugin();
    delete plugin;
}

void pluginstart()
{
    PluginManager *pm = PluginManager::Getinstance();
    Plugin *plugin = new Plugin;
    string str1;
    cout << "Enter the id of the plugin = ";
    cin >> str1;
    plugin->setValue("Id", str1);
    pm->start(plugin, (void *)g_platform);
    delete plugin;
}

void pluginstop()
{
    PluginManager *pm = PluginManager::Getinstance();
    Plugin *plugin = new Plugin;
    string str1;
    cout << "Enter the id of the plugin = ";
    cin >> str1;
    plugin->setValue("Id", str1);
    pm->stop(plugin);
    delete plugin;
}

void printfplugs()
{
    PluginManager *pm = PluginManager::Getinstance();
    const char format[] = "  %-30s %-15s %-15s %-20s\n";
    printf("\n====== Plugins List ======\n");
    printf(format,
           "ID",
           "NAME",
           "STATE",
           "TYPE");
    std::vector<Plugin> plugins = pm->getAllPlugins();
    std::vector<Plugin>::iterator it;

    for (it = plugins.begin(); it != plugins.end() ; it++)
    {
        printf(format,
               it->getID().c_str(),
               it->getName().c_str() != NULL ? it->getName().c_str() : "",
               pm->getState(it->getID()).c_str(),
               it->getValueByAttribute("ResourceType").c_str() != NULL ?
               it->getValueByAttribute("ResourceType").c_str() : "");
    }
    printf("\n");
}

PluginManager *PluginManager::s_pinstance = NULL;

int main(int argc, char *argv[])
{
    //PluginManager *pm = PluginManager::Getinstance();
    //Setting OCPlatform
    /////////////////////////////////

    PlatformConfig cfg
    {
        OC::ServiceType::InProc,
        OC::ModeType::Server,
        "192.168.2.5",
        5683,
        OC::QualityOfService::NonConfirmable
    };
    /////////////////////////////////

    g_platform = new OCPlatform(cfg);
    int num = 0;
    while (true)
    {
        printMenu();
        cout << "Enter the number of Menu = ";
        cin >> num;
        switch (num)
        {
            case 1:
                loading(1);
                break;
            case 2:
                loading(0);
                break;
            case 3:
                unregister();
                break;
            case 4:
                pluginstart();
                break;
            case 5:
                pluginstop();
                break;
            case 6:
                printfplugs();
                break;
            case 0:
                return 0;
                break;
            default:
                cout << "Wrong number." << endl;
                break;
        }
    }
    return 0;
}
