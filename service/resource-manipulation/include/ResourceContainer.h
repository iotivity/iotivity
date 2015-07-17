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
#include <list>

#include "BundleInfo.h"

namespace OIC
{
    namespace Service
    {
        class ResourceContainer
        {
            public:
                ResourceContainer();
                virtual ~ResourceContainer();
                virtual void startContainer(std::string configFile) = 0;
                virtual void stopContainer() = 0;

                // list of bundle ids
                virtual std::list<BundleInfo *> listBundles() = 0;
                virtual void startBundle(std::string bundleId) = 0;
                virtual void stopBundle(std::string bundleId) = 0;

                // dynamic configuration
                virtual void addBundle(std::string bundleId, std::string bundleUri, std::string bundlePath,
                                       std::map<std::string, std::string> params) = 0;
                virtual void removeBundle(std::string bundleId) = 0;

                virtual void addResourceConfig(std::string bundleId, std::string resourceUri,
                                               std::map<std::string, std::string> params) = 0;
                virtual void removeResourceConfig(std::string bundleId, std::string resourceUri) = 0;

                virtual std::list<std::string> listBundleResources(std::string bundleId) = 0;

                static ResourceContainer *getInstance();
        };
    }
}

#endif /* RESOURCECONTAINER_H_ */
