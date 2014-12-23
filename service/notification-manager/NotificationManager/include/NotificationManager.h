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
#include <list>
#include <condition_variable>

#include "OICPlatformConfig.h"
#include "HostingConfig.h"
#include "ResourceManager.h"
#include "RegistrationManager.h"
#include "VirtualRepresentation.h"
#include "HostingHandler.h"
#include "HostingInterface.h"

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

#define IN
#define OUT

using namespace OC;
using namespace OCPlatform;

const std::string VIRTURL_TAG = "/virtual";

class NotificationManager
{

private:

    NotificationManager();
    NotificationManager(HostingConfig cfg);
    ~NotificationManager();

    static NotificationManager *s_instance;
    static mutex s_mutexForCreation;

    int getNetInfo(IN int& sck, IN struct ifreq* item, OUT std::string& ip_addres);
    bool scanAndGetNetworkInterface(OUT std::string& ip_addres);

public:

    static void initialize();
    static void initialize(HostingConfig cfg);
    static NotificationManager *getInstance();

    void registerHostingEventListener();

};

#endif /* NOTIFICATIONMANAGER_H_ */
