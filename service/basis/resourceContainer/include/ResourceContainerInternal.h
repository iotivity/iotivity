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

#ifndef RESOURCECONTAINERINTERNAL_H_
#define RESOURCECONTAINERINTERNAL_H_

#include "ResourceContainer.h"

using namespace RC;

namespace RC{
	class ResourceContainerInternal: public ResourceContainer {
		public:
			ResourceContainerInternal();
			virtual ~ResourceContainerInternal();
			void registerResource(Resource* resource);
			void unregisterResource(Resource* resource);
			ConfigParam getConfiguration(string configKey);
			void init();
			void registerBundle(BundleInfo* bundleinfo);
			void activateBundle(string id);
			void activateBundleByName(string name);
			void deactivateBundle(string id);
			void deactivateBundleByName(string id);
			vector<Resource*> listBundleResources(string id);
	};
}

#endif //RESOURCECONTAINERINTERNAL_H_
