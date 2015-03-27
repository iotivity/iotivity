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

/// @file PluginManager.h

/// @brief

#ifndef __PLUGINMANAGER_H
#define __PLUGINMANAGER_H
#ifndef DLOPEN_POSIX
#define DLOPEN_POSIX
#endif

#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <dlfcn.h>
#include "PluginManagerImpl.h"
#include "Plugin.h"

namespace OIC
{

    /**
    * @brief  After installing a plug-in in a directory, each plug-in can be managed by this class.
    *
    *
    */
    class PluginManager
    {
        public:
            PluginManager();

            /**
            * Virtual destructor
            */
            ~PluginManager();

            /**
            * Start plug-ins by key-value pair.
            *
            * @param key    - key information of the plug-in to be started.
            * @param value   - value information of the plug-in to be started.
            * @return int   - 1 on success or 0 on failure.
            */
            int startPlugins(const std::string key, const std::string value);

            /**
            * Stop plug-ins by key-value pair.
            *
            * @param key    - key information of the plug-in to be stopped.
            * @param value   - value information of the plug-in to be stopped.
            * @return int   - 1 on success or 0 on failure.
            */
            int stopPlugins(const std::string key, const std::string value);

            /**
            * Rescan for plug-ins in the registered directory, installing plug-ins.
            *
            * @param void
            * @return int   - 1 on success or 0 on failure.
            */
            int rescanPlugin();

            /**
            * Get list of plug-ins.
            *
            * @param void
            * @return std::vector<Plugin>   - Plug-in vector.
            */
            std::vector<Plugin> getPlugins();

            /**
            * Get state of the plug-in.
            *
            * @param pluginID       - ID of the plug-in to get state.
            * @return std::string   - state of the plug-in.
            */
            std::string getState(const std::string plugID);

        private:
            PluginManagerImpl *pluginManagerImpl;
            void (*destroy)(PluginManagerImpl *);
            void *handle;
    };
}
#endif //__PLUGINMANAGER_H
