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

#ifndef RESOURCECONTAINERBUNDLEAPI_H_
#define RESOURCECONTAINERBUNDLEAPI_H_

#include <unistd.h>
#include <string.h>
#include <fstream>

#include "ResourceContainer.h"
#include "BundleInfo.h"
#include "Configuration.h"
#include "NotificationReceiver.h"
#include "BundleResource.h"

using namespace OIC::Service;

namespace OIC
{
    namespace Service
    {
        class ResourceContainerBundleAPI: public Configuration, public NotificationReceiver
        {
            public:
                ResourceContainerBundleAPI();
                virtual ~ResourceContainerBundleAPI();
                virtual void registerResource(BundleResource *resource) = 0;
                virtual void unregisterResource(BundleResource *resource) = 0;
                virtual void getBundleConfiguration(std::string bundleId, configInfo *configOutput) = 0;
                virtual void getResourceConfiguration(std::string bundleId,
                                                      std::vector< resourceInfo > *configOutput) = 0;

                static ResourceContainerBundleAPI *getInstance();
        };
    }
}

#endif
