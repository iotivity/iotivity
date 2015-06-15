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

#include "BundleInfo.h"

using namespace std;

namespace RC
{

    // placeholder
    class Resource
    {

    };

    class ResourceContainer
    {
        public:
            ResourceContainer();
            virtual ~ResourceContainer();
            virtual void init() = 0;
            virtual void init(string configFile) = 0;
            virtual void registerBundle(BundleInfo *bundleinfo) = 0;
            virtual void unregisterBundle(BundleInfo *bundleinfo) = 0;
            virtual void unregisterBundle(int id) = 0;
            virtual void activateBundle(int id) = 0;
            virtual void deactivateBundle(int id) = 0;
            virtual void activateBundleByName(string name) = 0;
            virtual void deactivateBundleByName(string id) = 0;
            virtual void activateBundle(BundleInfo *bundleInfo) = 0;
            virtual void deactivateBundle(BundleInfo *bundleInfo) = 0;
            virtual vector< Resource * > listBundleResources(string id) = 0;

            static ResourceContainer *getInstance();
    };
}

#endif /* RESOURCECONTAINER_H_ */
