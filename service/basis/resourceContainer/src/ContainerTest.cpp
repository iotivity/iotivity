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

#include "ResourceContainerInternal.h"
#include "BundleInfo.h"

using namespace RC;

int main(){
	printf("Test\n");
	BundleInfo* bundleInfo = BundleInfo::createBundleInfo();
	bundleInfo->setPath("/home/iotivity/development/iotivity-mj-rc/iotivity-mj/out/linux/x86/release/service/resource-container/libSampleBundle.so");
	bundleInfo->setVersion("1.0");
	bundleInfo->setName("Sample Bundle");

	ResourceContainerInternal container;
	container.registerBundle(bundleInfo);

	//bundleInfo.path = "/";
	//bundleInfo.version = "1.0";
}
