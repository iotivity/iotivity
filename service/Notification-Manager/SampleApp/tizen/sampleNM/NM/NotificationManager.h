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

/*
 * NotificationCore.h
 *
 *  Created on: 2014. 8. 7.
 *      Author: jyong2
 */

#ifndef NOTIFICATIONMANAGER_H_
#define NOTIFICATIONMANAGER_H_

#include <iostream>
#include <functional>
#include <pthread.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "OCApi.h"
#include "OCPlatform.h"
#include "OCResource.h"
#include "OCResourceRequest.h"
#include "OCResourceResponse.h"
#include "ocstack.h"

#include "ResourceManager.h"
#include "RegistrationManager.h"
#include "VirtualRepresentation.h"


#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

#define ISFORLINUX 0
#define ISFORTIZEN 1

#define IN
#define OUT

using namespace OC;

const std::string VIRTURL_TAG = "/virtual";

class NotificationManager {

private:

	static NotificationManager *instance;
	static OCPlatform *nmOCPlatform;

	static PlatformConfig cfg;

	std::function<void(AttributeMap &inputAttMap)> print;
	std::function<void(std::shared_ptr<OCResource> resource)> onfound;
	std::function<void(AttributeMap &inputAttMap)> onObserve;

	std::function<void(std::shared_ptr<OCResource> resource)> startHosting;
	std::function<void()> findHosting;
	std::function<void(std::string)> addExtraStr;

public:

	NotificationManager();
	~NotificationManager();

	static NotificationManager *getInstance();
	static OCPlatform *getOCPlatform();

	void initialize();
	void findHostingCandidate();
	void registerHostingEvent();

	int setPrint( std::function<void(AttributeMap &inputAttMap)> func );
	int setOnFoundHostingCandidate( std::function<void(std::shared_ptr<OCResource> resource)> func );
	int setOnObserve( std::function<void(AttributeMap &inputAttMap)> func );

	std::function<void(AttributeMap &inputAttMap)> getPrint();
	std::function<void(std::shared_ptr<OCResource> resource)> getOnFoundHostingCandidate();
	std::function<void(AttributeMap &inputAttMap)> getOnObserve();

	int setStartHosting( std::function<void(std::shared_ptr<OCResource> resource)> &func );
	int setFindHosting( std::function<void()> &func );
	int setAddExtraStr( std::function<void(std::string)> &func );

	std::function<void(std::shared_ptr<OCResource> resource)> getStartHosting();
	std::function<void()> getFindHosting();
	std::function<void(std::string)> getAddExtraStr();

	int 	getNetInfo(IN int& sck, IN struct ifreq* item, OUT std::string& ip_addres);
	bool 	scanAndGetNetworkInterface(OUT std::string& ip_addres);

};

#endif /* NOTIFICATIONMANAGER_H_ */
