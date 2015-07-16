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

using namespace std;

namespace OIC
{
    namespace Service
    {
        class ResourceContainer
        {
        public:
            ResourceContainer();
            virtual ~ResourceContainer();
            //virtual void initializeResourceContainer() = 0;
            virtual void startContainer(string configFile) = 0;
            virtual void stopContainer() = 0;

            // list of bundle ids
            virtual std::list<BundleInfo*> listBundles() = 0;
            virtual void startBundle(string bundleId) = 0;
            virtual void stopBundle(string bundleId) = 0;

            // dynamic configuration
            virtual void addBundle(string bundleId, string bundleUri, string bundlePath, std::map<string, string> params) =0;
            virtual void removeBundle(string bundleId) = 0;

            virtual void addResourceConfig(string bundleId, string resourceUri, std::map<string, string> params)  = 0;
            virtual void removeResourceConfig(string bundleId, string resourceUri)  = 0;

            virtual std::list<string> listBundleResources(string bundleId) = 0;

            static ResourceContainer *getInstance();
        };
    }
}

#endif /* RESOURCECONTAINER_H_ */
