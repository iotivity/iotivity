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

/// @file ProtocolLifecycleManager.h

/// @brief

#ifndef __PLUGINLIFECYCLEMANAGER_H
#define __PLUGINLIFECYCLEMANAGER_H

#include <PluginManager.h>
#include <Plugin.h>

namespace OIC
{

    /**
    * @brief    Protocol plugin's lifecycle can be controlled by this class.
    *
    *
    */
    class PluginLifeCycleManager
    {
        public:

            PluginLifeCycleManager();

            ~PluginLifeCycleManager();


            /**
            * Start protocol plugins by resource type
            *
            * @param type resouce type string to be started.
            *
            */
            void startPluginsByResourceType(const std::string type);

            /**
            * Stop protocol plugins by resource type
            *
            * @param type resouce type string to be started.
            *
            */
            void stopPluginsByResourceType(const std::string type);
        private:
            PluginManager *pm;

    };
}

#endif __PLUGINLIFECYCLEMANAGER_H

