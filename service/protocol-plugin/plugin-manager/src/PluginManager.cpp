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

/// @file PluginManager.cpp

/// @brief


#include "PluginManager.h"
#ifdef __TIZEN__
#include <appfw/app_common.h>
#endif

using namespace OIC;

PluginManager::PluginManager()
{
#ifdef __TIZEN__
    char *app_id = NULL;
    std::string completePath = "";
    int res = app_get_id(&app_id);
    if (APP_ERROR_NONE == res)
    {
        completePath = "/opt/usr/apps/";
        completePath += app_id;
        completePath += "/lib/libpmimpl.so";
    }
    free(app_id);
    app_id = NULL;
    handle = dlopen(completePath.c_str(), RTLD_LAZY);
#else
    handle = dlopen("./libpmimpl.so", RTLD_LAZY);
#endif //#ifdef __TIZEN__

    if (!handle)
    {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    PluginManagerImpl* (*create)(void *);
    create = (PluginManagerImpl * (*)(void *))dlsym(handle, "create_object");
    destroy = (void (*)(PluginManagerImpl *))dlsym(handle, "destroy_object");
    pluginManagerImpl = (PluginManagerImpl *)create(NULL);
}

PluginManager::~PluginManager(void)
{
    destroy(pluginManagerImpl);
    dlclose(handle);
}

int PluginManager::startPlugins(const std::string key, const std::string value)
{
    return pluginManagerImpl->startPlugins(key, value);
}

int PluginManager::stopPlugins(const std::string key, const std::string value)
{
    return pluginManagerImpl->stopPlugins(key, value);
}

int PluginManager::rescanPlugin()
{
    return pluginManagerImpl->rescanPlugin();
}

std::vector<Plugin> PluginManager::getPlugins(void)
{
    return pluginManagerImpl->getAllPlugins();
}

std::string PluginManager::getState(const std::string plugID)
{
    return pluginManagerImpl->getState(plugID);
}
