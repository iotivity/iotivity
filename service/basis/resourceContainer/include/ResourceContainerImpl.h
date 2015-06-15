//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef RESOURCECONTAINERIMPL_H_
#define RESOURCECONTAINERIMPL_H_

#include "ResourceContainer.h"
#include "ResourceContainerBundleAPI.h"
#include "BundleInfoInternal.h"

using namespace RC;

namespace RC
{

    class ResourceContainerImpl: public ResourceContainer, public ResourceContainerBundleAPI
    {
        public:
            ResourceContainerImpl();
            virtual ~ResourceContainerImpl();

            // methods from ResourceContainer
            void init();
            void init(string configFile);
            void activateBundle(int id);
            void deactivateBundle(int id);
            void activateBundleByName(string name);
            void deactivateBundleByName(string id);
            void activateBundle(BundleInfo *bundleInfo);
            void deactivateBundle(BundleInfo *bundleInfo);
            vector< Resource * > listBundleResources(string id);

            // methods from ResourceContainerBundleAPI
            void registerBundle(BundleInfo *bundleinfo);
            void unregisterBundle(BundleInfo *bundleinfo);
            void unregisterBundle(int id);
            void registerResource(BundleResource *resource);
            void unregisterResource(BundleResource *resource);

            void getCommonConfiguration(configInfo *configOutput);
            void getBundleConfiguration(std::string bundleId, configInfo *configOutput);
            void getResourceConfiguration(std::string bundleId, std::vector<resourceInfo> *configOutput);

            static ResourceContainerImpl *getImplInstance();

        private:
            vector<BundleInfoInternal * > m_bundles;
            string m_configFile;
            Configuration *m_config;
    };
}

#endif
