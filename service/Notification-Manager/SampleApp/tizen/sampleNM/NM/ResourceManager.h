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

#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include <map>
#include <memory>
#include <string>
#include "OCPlatform.h"
#include "OCResource.h"
#include "OCResourceResponse.h"
#include "ocstack.h"

#include "NotificationManager.h"

using namespace OC;

class VirtualRepresentation;

class ResourceManager {

private:
	static ResourceManager *instance;

	static std::list<VirtualRepresentation> resourceList;

	static std::string extraStr;


	void foundResourceforhosting(std::shared_ptr<OCResource> resource);

	void checkResourceDBPolicy();
	void saveResourceDB();

public:
	ResourceManager();
	virtual ~ResourceManager();

	static ResourceManager *getInstance();
	VirtualRepresentation findVirtualRepresentation(std::string uri);

	OCStackResult findNMResource(const std::string& host, const std::string& resourceName, bool ishosting);
	void startHosting(std::shared_ptr<OCResource> resource);

	AttributeMap copyAttributeMap(AttributeMap &inputAttMap);
	bool isEmptyAttributeMap(AttributeMap &inputAttMap);

	void printAttributeMap(AttributeMap &inputAttMap);
	void onFoundReport(std::shared_ptr<OCResource> resource);

	void addExtraStr(std::string str);
	std::string getExtraStr();


//	void findResource();
//	void foundResource();

};

#endif /* RESOURCEMANAGER_H_ */
