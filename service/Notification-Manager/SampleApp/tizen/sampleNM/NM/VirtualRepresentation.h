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

#ifndef VIRTUALREPRESENTATION_H_
#define VIRTUALREPRESENTATION_H_

#include <functional>
#include <condition_variable>

#include "OCPlatform.h"
#include "OCApi.h"
#include "NotificationManager.h"

#define SUCCESS_RESPONSE 0

using namespace OC;
using namespace std;

class ResourceManager;

class VirtualRepresentation{

private:
	std::string 	virtualUri;
	std::string	originHostIP;
	std::string 	resourceTypeName;
	std::string 	resourceInterface;
	uint8_t 		resourceProperty;

	OCResourceHandle m_resourceHandle;

	static AttributeMap attributeMap;
	static std::mutex mutexAttributeMap;
	static std::condition_variable conditionAttributeMap;
	static bool isReadyAttributeMap;

	int getRepresentation(OCRepresentation& oc);

public:

	std::string 		getUri();
	std::string 		getHostIP();
	std::string 		getResourceTypeName();
	std::string 		getResourceInterface();
	uint8_t 			getResourceProperty();
	OCResourceHandle	getResourceHandle();

	int setUri(std::string uri);
	int setHostIP(std::string ip);
	int setResourceTypeName(std::string typeName);
	int setResourceInterface(std::string interface);
	int setResourceProperty(uint8_t property);
	int setResourceHandle(OCResourceHandle & handle);

public:
	VirtualRepresentation();
	virtual ~VirtualRepresentation();

	std::string addVirtualTag(std::string uri);

	void entityHandler(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response);
	void onObserve(const OCRepresentation& rep, const int& eCode, const int& sequenceNumber);

};

#endif /* VIRTUALREPRESENTATION_H_ */
