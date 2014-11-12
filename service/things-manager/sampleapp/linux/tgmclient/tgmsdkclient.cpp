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

#include <string>
#include <cstdlib>
#include <pthread.h>
#include "OCPlatform.h"
#include "OCApi.h"
#include "TGMClient.h"

using namespace OC;

void onGet(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
{
    if (eCode == OC_STACK_OK)
    {
        std::cout << "GET request was successful" << std::endl;
        std::cout << "Resource URI: " << rep.getUri() << std::endl;

        AttributeMap dataMap = rep.getAttributeMap();
        for (auto it = dataMap.begin(); it != dataMap.end(); ++it)
        {
            std::cout << "\t KEY:" << it->first << "  //  VALUE:" << it->second << std::endl;
        }

    }
    else
    {
        std::cout << "onGET Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

void onFoundCandidate(std::vector< std::shared_ptr< OCResource > > resources)
{
    std::cout << "\n\n  FOUND CANDIDATE  \n\n";
    for (unsigned int i = 0; i < resources.size(); ++i)
    {
        QueryParamsMap test;
        resources.at(i)->get(test, &onGet);
    }
}

int main(int argc, char* argv[])
{

    // Create PlatformConfig object
    PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Client, "0.0.0.0", 0, OC::QualityOfService::LowQos };

    OCPlatform::Configure(cfg);
    TGMClient *tgmclient = new TGMClient();

    int selectedMenu;

    try
    {
        std::vector < std::string > types;

        while (true)
        {
            // some operations
            std::cout << "ADD  | (1)TV | (2)LIGHT | (3)CURTAIN | (4)RESET |" << std::endl;
            std::cout << "FIND | (5) ONCE | (6) 10sec |" << std::endl;

            std::cin >> selectedMenu;

            if (selectedMenu == 1)
            {
                types.push_back("core.tv");
            }
            else if (selectedMenu == 2)
            {
                types.push_back("core.light");
            }
            else if (selectedMenu == 3)
            {
                types.push_back("core.curtain");
            }
            else if (selectedMenu == 4)
            {
                types.clear();
            }
            else if (selectedMenu == 5)
            {
                tgmclient->findCandidateResources(types, &onFoundCandidate);
            }
            else if (selectedMenu == 6)
            {
                tgmclient->findCandidateResources(types, &onFoundCandidate, 10);
            }

        }

    }
    catch (OCException& e)
    {
        //log(e.what());
    }

    return 0;
}

