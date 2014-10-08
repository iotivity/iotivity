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

#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include "OCApi.h"
#include "OCPlatform.h"
#include "NotificationManager.h"

using namespace OC;

class NotificationService
{

public:

    static std::function< void(std::shared_ptr< OCResource > resource) > s_startHosting;
    static std::function< void() > s_findHostingCandidate;
    static std::function< void(std::string) > s_addExtraStr;

    void onObserve(AttributeMap &inputAttMap)
    {
    	std::cout << endl;
    	std::cout << "========================================================" << endl;
    	std::cout << "On Observe:\n";
    	for(auto it = inputAttMap.begin() ; it != inputAttMap.end() ; ++it)
        {
            std::cout << "\tAttribute name: " << it->first << " value: ";

            for(auto valueItr = it->second.begin() ; valueItr != it->second.end() ; ++valueItr)
            {
                std::cout << "\t" << *valueItr << " ";
            }

            std::cout << std::endl;
        }
    }

    void onFoundCandidate(std::shared_ptr< OCResource > resource)
    {
        s_startHosting(resource);
    }

};

std::function< void(std::shared_ptr< OCResource > resource) > NotificationService::s_startHosting;
std::function< void() > NotificationService::s_findHostingCandidate;
std::function< void(std::string) > NotificationService::s_addExtraStr;

int main(void)
{

    std::cout << endl;
    std::cout << "========================================================" << endl;
    std::cout << "Start Notification Manager : Hosting v0.5\n";

    NotificationService a;

    NotificationManager *Ptr = NotificationManager::getInstance();
    Ptr->initialize();

    Ptr->setOnFoundHostingCandidate(
            std::function< void(std::shared_ptr< OCResource > resource) >(
                    std::bind(&NotificationService::onFoundCandidate , a , std::placeholders::_1)));
    Ptr->setOnObserve(
            std::function< void(AttributeMap &inputAttMap) >(
                    std::bind(&NotificationService::onObserve , a , std::placeholders::_1)));

    Ptr->setFindHosting(a.s_findHostingCandidate);
    Ptr->setStartHosting(a.s_startHosting);
    Ptr->setAddExtraStr(a.s_addExtraStr);

    a.s_addExtraStr("visual");
    a.s_findHostingCandidate();

    while(true)
    {
        // todo something
    }

    std::cout << endl;
    std::cout << "========================================================" << endl;
    std::cout << "End Notification Manager : Hosting v0.5\n";

    return true;
}
