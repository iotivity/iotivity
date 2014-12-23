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

#include "NotificationManager.h"

using namespace OC;
using namespace OCPlatform;


class OICPlatformConfig;
class VirtualRepresentation;

class ResourceManager
{

private:
    ResourceManager();
    ~ResourceManager();

    static ResourceManager *s_instance;
    static mutex s_mutexForCreation;
    static std::list< VirtualRepresentation > s_resourceList;
    static std::string s_extraStr;

    void foundResourceforhosting(std::shared_ptr< OCResource > resource);

    void checkResourceDBPolicy();
    void saveResourceDB();

public:

	std::function< void(std::shared_ptr< OCResource > resource) > m_onFoundforHosting;
	std::function< void(AttributeMap &inputAttMap, OCResourceHandle resourceHandle) > m_onObserve;
	std::function< void(OCResourceHandle resourceHandle) > m_notify;

    static ResourceManager *getInstance();

    void findNMResource(bool isHosting);

    void onFoundforHostingDefault(std::shared_ptr< OCResource > resource);
    void onObserveDefault(AttributeMap &inputAttMap, OCResourceHandle resourceHandle);
    void notifyObserversDefault(OCResourceHandle resourceHandle);

    void startHosting(std::shared_ptr< OCResource > resource);
    void notifyObservers(OCResourceHandle resourceHandle);

    VirtualRepresentation findVirtualRepresentation(std::string uri);
    AttributeMap copyAttributeMap(AttributeMap &inputAttMap);
    bool isEmptyAttributeMap(AttributeMap &inputAttMap);
    void printAttributeMap(AttributeMap &inputAttMap);

    void addExtraStr(std::string str);
    std::string getExtraStr();
};

#endif /* RESOURCEMANAGER_H_ */
