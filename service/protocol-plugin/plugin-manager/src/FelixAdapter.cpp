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

/// @file FelixAdapter.cpp

/// @brief

#include "FelixAdapter.h"

using namespace OIC;

FelixAdapter *FelixAdapter::s_pinstance;

FelixAdapter::FelixAdapter()
{
    config = Config::Getinstance();
    std::string pluginpath = config->getPluginPath();
    if (pluginpath != "")
    {
        printf("Current path is %s\n", pluginpath.c_str());
    }
    else
    {
        fprintf(stderr, "Pluing path is not exist\n");
        pluginpath = "";
    }
    registerAllPlugin(pluginpath);
}

FelixAdapter::~FelixAdapter(void)
{
    s_pinstance->deleteinstance();
}

int FelixAdapter::installPlugin(const std::string path)
{
    return 1;
}

int FelixAdapter::findPluginRecursive(const std::string path)
{
    return 1;
}

int FelixAdapter::loadPluginInfoToManager(const std::string path)
{
    return 1;
}

int FelixAdapter::registerPlugin(const std::string path)
{
    return 1;
}

int FelixAdapter::registerAllPlugin(const std::string path)
{
    return 1;
}

int FelixAdapter::unregisterPlugin(Plugin *const plugin)
{
    return 1;
}

int FelixAdapter::unregisterAllPlugin(void)
{
    return 1;
}

std::vector<Plugin> &FelixAdapter::getAllPlugins(void)
{
    return m_plugins;
}

std::vector<Plugin> *FelixAdapter::findPlugins(const std::string key, const std::string value)
{
    return nullptr;
}
/*
Plugin *FelixAdapter::getPlugin(const std::string plugID)
{
    return nullptr;
}
*/
bool FelixAdapter::getFileList(File_list &list, const std::string strDir)
{
    return true;
}


void FelixAdapter::printPluginList(cp_plugin_info_t **plugins)
{

}

int FelixAdapter::start(Plugin *const plugin, void *const arg)
{
    return TRUE;
}

int FelixAdapter::stop(Plugin *const plugin)
{
    return TRUE;
}

bool FelixAdapter::isStarted(Plugin *plugin)
{
    return FALSE;
}
/*
void FelixAdapter::observePluginPath(void *str)
{

}
*/
const std::string FelixAdapter::getState(const std::string plugID)
{
    return "";
}