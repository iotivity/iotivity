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

#ifndef RESOURCECONTAINER_H_
#define RESOURCECONTAINER_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

using namespace std;

namespace RC
{
    /*
     * Describes a bundle with resources, that can be loaded dynamically.
     */
    class BundleInfo
    {
        public:
            BundleInfo();
            virtual ~BundleInfo();
        public:
            string id, name, path, version;

    };

    // Will be provided by the resource builder
    class Resource
    {

    };

    class ResourceContainer
    {
        public:
            ResourceContainer();
            virtual ~ResourceContainer();
            virtual void init() = 0;
            virtual void registerBundle(BundleInfo bundleinfo) = 0;
            virtual void activateBundle(string id) = 0;
            virtual void activateBundleByName(string name) = 0;
            virtual void deactivateBundle(string id) = 0 ;
            virtual void deactivateBundleByName(string id) = 0;
            virtual vector<Resource *> listBundleResources(string id) = 0;
    };

    typedef vector < map < std::string, std::string > > ConfigParam;
    typedef enum { CONFIG_COMMON, CONFIG_BUNDLES, CONFIG_RESOURCES } ConfigKey;

    class ResourceContainerInternal : public ResourceContainer
    {
        public:
            ResourceContainerInternal();
            virtual ~ResourceContainerInternal();
            void registerResource(Resource *resource);
            void unregisterResource(Resource *resource);

            ConfigParam getConfiguration(string configKey);
            ConfigParam getConfiguration(ConfigKey configKey, string id = "");

            void init();
            void registerBundle(BundleInfo bundleinfo);
            void activateBundle(string id);
            void activateBundleByName(string name);
            void deactivateBundle(string id);
            void deactivateBundleByName(string name);
            vector<Resource *> listBundleResources(string id);

        private:
            void getConfigDocument(std::string pathConfigFile, std::string *pConfigData);
            void getConfigBundleData(rapidxml::xml_node< char > *configData, string BundleId,
                                     ConfigParam *pConfigOutput);
            void getConfigResourceData(rapidxml::xml_node< char > *configData, string BundleId,
                                       ConfigParam *pConfigOutput);
            void getCurrentPath(std::string *path);
    };

    // TBD
    class InputPropertyDescriptor
    {

    };
}


#endif /* RESOURCECONTAINER_H_ */
