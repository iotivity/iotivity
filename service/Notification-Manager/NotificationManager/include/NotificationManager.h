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

#define ISFORDEMO 1

using namespace OC;

const std::string VIRTURL_TAG = "/virtual";

class NotificationManager
{

private:

    static NotificationManager *s_instance;
    static OCPlatform *s_nmOCPlatform;

    static PlatformConfig s_cfg;

    std::function< void(AttributeMap &inputAttMap) > m_print;
    std::function< void(std::shared_ptr< OCResource > resource) > m_onfound;
    std::function< void(AttributeMap &inputAttMap) > m_onObserve;

    std::function< void(std::shared_ptr< OCResource > resource) > m_startHosting;
    std::function< void() > m_findHosting;
    std::function< void(std::string) > m_addExtraStr;

public:

    NotificationManager();
    ~NotificationManager();

    static NotificationManager *getInstance();
    static OCPlatform *getOCPlatform();

    void initialize();
    void findHostingCandidate();
    void registerHostingEvent();

    int setPrint(std::function< void(AttributeMap &inputAttMap) > func);
    int setOnFoundHostingCandidate(
            std::function< void(std::shared_ptr< OCResource > resource) > func);
    int setOnObserve(std::function< void(AttributeMap &inputAttMap) > func);

    std::function< void(AttributeMap &inputAttMap) > getPrint();
    std::function< void(std::shared_ptr< OCResource > resource) > getOnFoundHostingCandidate();
    std::function< void(AttributeMap &inputAttMap) > getOnObserve();

    int setStartHosting(std::function< void(std::shared_ptr< OCResource > resource) > &func);
    int setFindHosting(std::function< void() > &func);
    int setAddExtraStr(std::function< void(std::string) > &func);

    std::function< void(std::shared_ptr< OCResource > resource) > getStartHosting();
    std::function< void() > getFindHosting();
    std::function< void(std::string) > getAddExtraStr();

};

#endif /* NOTIFICATIONMANAGER_H_ */
