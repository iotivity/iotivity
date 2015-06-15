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

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <unistd.h>
#include <string.h>
#include <fstream>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

#include "ResourceContainer.h"
#include "BundleInfo.h"

using namespace RC;

namespace RC
{


    class Configuration
    {
        public:
            typedef vector<std::map <std::string, std::string>> configInfo;
            struct resourceInfo
            {
                std::string name;
                std::string uri;
                std::string resourceType;
                std::map < std::string, std::vector< std::map< std::string, std::string > > > resourceProperty;
            };

            Configuration();
            ~Configuration();

            Configuration(string configFile);

            void getCommonConfiguration(configInfo *configOutput);
            void getBundleConfiguration(std::string bundleId, configInfo *configOutput);
            void getResourceConfiguration(std::string bundleId, std::vector<resourceInfo> *configOutput);
            void getConfiguredBundles(configInfo *configOutput);

        private:
            void getConfigDocument(std::string pathConfigFile, std::string *pConfigData);
            void getCurrentPath(std::string *path);

            string m_pathConfigFile, m_configFile;
    };
}

#endif
