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

/// @file FelixAdapter.h

/// @brief

#ifndef __FELIXADAPTER_H__
#define __FELIXADAPTER_H__
#define DLOPEN_POSIX

#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <internal.h>

#include "Plugin.h"
#include "Config.h"

#define BUF_LEN     (int)( 1024 * ( EVENT_SIZE + 16 ) )

namespace OIC
{
    /**
    * @brief    After installing a plug-in in a directory, each plug-ins can be managed by this class.
    *
    *
    */
    class FelixAdapter
    {
        public:
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
            int registerPlugin(const std::string path);


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
            int registerAllPlugin(const std::string path);


            /**
            * Unregister plugin.
            *
            * @param plugin plugin object to be unregistered.
            * @return int, 1 is success, 0 is fail.
            */
            int unregisterPlugin(Plugin *const plugin);


            /**
            * Unregister All plugin.
            *
            * @return int, 1 is success, 0 is fail.
            */
            int unregisterAllPlugin(void);


            /**
            * get all plugins which currently registered.
            *
            * @return vector of currently registered plugins
            *
            */
            std::vector<Plugin> &getAllPlugins(void);


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
            //Plugin *getPlugin(const std::string pluginID);

            /**
            * Start plugin
            * This function will load dynamic plugin library on memory and call start function of plugin to be initialized.
            *
            * @param Plugin
            * @param Platform pointer.
            * @return int, 1 is success, 0 is fail.
            */
            int start(Plugin *const plugin, void *const arg);


            /**
            * Stop Plugin.
            * This function will call stop function of plugin and unload dynamic plugin library from memory.
            *
            * @param Plugin
            * @return int, 1 is success, 0 is fail.
            */
            int stop(Plugin *const plugin);


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
            const std::string getState(const std::string plugID);


            /**
            *
            * new Singleton pattern instance.
            *
            * @return OICPluginManager pointer Address.
            */
            static FelixAdapter *Getinstance()
            {
                if (NULL == s_pinstance)
                {
                    s_pinstance = new FelixAdapter();
                }

                return s_pinstance;
            }



        private:
            Config *config;
            typedef std::map<std::string, bool> File_list;
            std::vector<Plugin> m_plugins;
            boost::thread m_file_detect_thread;
            /*
            cp_context_t *m_context;
            cp_status_t m_status;
            cp_plugin_info_t **m_cp_plugins;
            cp_plugin_info_t *m_plugin;
            boost::thread_group m_thread_g;
            std::string m_path;
            */
            static FelixAdapter *s_pinstance;

            /**
            * Constructor for FelixAdapter.
            * During construction time, all plugins under the root plugin path will be loaded.
            *
            */
            FelixAdapter();

            /**
            * Virtual destructor
            */
            virtual ~FelixAdapter(void);

            /**
            * delete Singleton pattern instance.
            */
            static void deleteinstance()
            {
                if (NULL != s_pinstance)
                {
                    delete s_pinstance;
                    s_pinstance = NULL;
                }
            }

            /**
            * detect plugins(add, delete, move)
            *
            * @param plugin file path.
            * @return void
            */
            //void observePluginPath(void *);

            /**
            * Get whole "SO" file list.
            *
            * @param OUT, SO file list.
            * @param Root path.
            * @return true or false.
            */
            bool getFileList(File_list &list, const std::string strDir);

            /**
            * print whole plugin info.
            *
            * @param Felix plugins
            */
            void printPluginList(cp_plugin_info_t **plugins);

            /**
            * install plugin using c-pluff.
            *
            * @param Root path.
            * @return int, 1 is success, 0 is fail.
            */
            int installPlugin(const std::string path);


            /**
            * find Plugin and install plugin.(Recursive)
            *
            * @param Root path.
            * @return int, 1 is success, 0 is fail.
            */
            int findPluginRecursive(const std::string path);

            /**
            * load Plugin information to PluginManager table.
            *
            * @param path to observe
            * @return int, 1 is success, 0 is fail.
            */
            int loadPluginInfoToManager(const std::string path);
    };
}

#endif //__FELIXADAPTER_H__
