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

/// @file PluginManagerImpl.h

/// @brief

#ifndef __PLUGINMANAGERIMPL_H__
#define __PLUGINMANAGERIMPL_H__

#include "Plugin.h"
#include "CpluffAdapter.h"
#include "FelixAdapter.h"

namespace OIC
{
    class PluginManagerImpl
    {
        public:

            /**
            * Constructor for PluginManagerImpl.
            * During construction time, all plugins under the root plugin path will be loaded.
            *
            */
            PluginManagerImpl();

            /**
            * Virtual destructor
            */
            virtual ~PluginManagerImpl(void);


            /**
            * A function to register pluins in the path.
            * This function will load plugins in plugin manager table.
            *
            * @param path plugin file path to be registered.
            * @return int, 1 is success, 0 is fail.
            *
            * NOTE:
            *
            */
            int registerPlugin(std::string path);



            /**
            * A function to register pluins in the path.
            * This function will load plugins in plugin manager table.
            *
            * @param path plugin file path to be registered.
            * recursive load plugins sub folders recursively.
            * @return int, 1 is success, 0 is fail.
            *
            * NOTE:
            *
            */
            int registerAllPlugin(std::string path);

            /**
            * Unregister plugin.
            *
            * @param plugin plugin object to be unregistered.
            * @return int, 1 is success, 0 is fail.
            */
            int unregisterPlugin(std::string path);

            /**
            * Unregister All plugin.
            *
            * @return int, 1 is success, 0 is fail.
            */
            int unregisterAllPlugin(void);

            /**
            * find plugins which have the key and value
            *
            * @return vector of currently registered plugins
            */
            std::vector<Plugin> *findPlugins(const std::string key, const std::string value);

            /**
            * Get plugin which has the id
            *
            * @param pluginID plugin id to find
            * @return Plugin instance
            */
            Plugin *getPlugin(const std::string pluginID);

            /**
            * Check whether the plugin started or not
            *
            * @param Plugin to identify the Starting.
            * @return true if started, false is stop.
            *
            */
            bool isStarted(Plugin *plugin);

            /**
            * Get Plugin state.
            *
            * @param Plugin ID
            * @return Plugin state.
            */
            virtual std::string getState(const std::string plugID);

            /**
            * Start  plugins by resource type
            *
            * @param type resouce type string to be started.
            *
            */
            virtual int startPlugins(const std::string key, const std::string value);

            /**
            * Stop  plugins by resource type
            *
            * @param type resouce type string to be started.
            *
            */
            virtual int stopPlugins(const std::string key, const std::string value);

            /**
            * Start plugin
            * This function will load dynamic plugin library on memory and call start function of plugin to be initialized.
            *
            * @param Plugin
            * @return int, 1 is success, 0 is fail.
            */
            int startPlugins(Plugin *const plugin);


            /**
            * Stop Plugin.
            * This function will call stop function of plugin and unload dynamic plugin library from memory.
            *
            * @param Plugin
            * @return int, 1 is success, 0 is fail.
            */
            int stopPlugins(Plugin *const plugin);

            /**
            * Rescan Plugin.
            * This function will call rescan function of plugins in the configuration folder
            *
            * @param Plugin
            * @return int, 1 is success, 0 is fail.
            */
            virtual int rescanPlugin();

            /**
            * get all plugins which currently registered.
            *
            * @return vector of currently registered plugins
            *
            */
            virtual std::vector<Plugin> &getAllPlugins(void);

            static PluginManagerImpl *Getinstance()
            {
                if (NULL == s_pinstance)
                {
                    s_pinstance = new PluginManagerImpl();
                }

                return s_pinstance;
            }

        private:

            CpluffAdapter *cppm;
            FelixAdapter *javappm;

            std::vector<Plugin> m_plugins;
            static PluginManagerImpl *s_pinstance;

            static void deleteinstance()
            {
                if (NULL != s_pinstance)
                {
                    delete s_pinstance;
                    s_pinstance = NULL;
                }
            }

            /*
            * refresh All Plugin information.
            *
            * @return vector<Plugin> is all Plugin.
            */
            std::vector<Plugin> refreshPluginInfo();

            /**
            * Start plugin by platform.
            * This function will load dynamic plugin library on memory and call start function of plugin to be initialized.
            *
            * @param Plugin
            * @param Platform pointer.
            * @return int, 1 is success, 0 is fail.
            */
            int startbyPlatform(Plugin *const plugin, void *const arg);

            /**
            * Stop Plugin by platform.
            * This function will call stop function of plugin and unload dynamic plugin library from memory.
            *
            * @param Plugin
            * @return int, 1 is success, 0 is fail.
            */
            int stopbyPlatform(Plugin *const plugin);
    };
}

#endif //__PLUGINMANAGERIMPL_H__