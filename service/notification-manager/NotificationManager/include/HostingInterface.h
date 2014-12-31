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

#ifndef HOSTINGINTERFACE_H_
#define HOSTINGINTERFACE_H_

#include "NotificationManager.h"

using namespace OC;
using namespace OCPlatform;

class HostingInterface
{

    public:
        HostingInterface();
        ~HostingInterface();

        int setOnFoundHostingCandidate(
            std::function< void(std::shared_ptr< OCResource > resource) > func);
        int setOnObserve(std::function< void(AttributeMap &inputAttMap, OCResourceHandle resourceHandle) >
                         func);

        int setFindHosting(std::function< void(bool isHosting) > &func);
        int setStartHosting(std::function< void(std::shared_ptr< OCResource > resource) > &func);
        int setNotifyObservers(std::function< void(OCResourceHandle resourceHandle) > &func);
        int setAddExtraStr(std::function< void(std::string) > &func);

};

#endif /* HOSTINGINTERFACE_H_ */
