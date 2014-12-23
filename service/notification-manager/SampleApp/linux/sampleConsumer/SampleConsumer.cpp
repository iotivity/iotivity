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

// OCClient.cpp : Defines the entry point for the console application.
//

#include <string>
#include <cstdlib>
#include <pthread.h>
#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;

const int SUCCESS_RESPONSE = OC_STACK_OK;
static ObserveType OBSERVE_TYPE_TO_USE = ObserveType::Observe;

std::shared_ptr< OCResource > g_curResource;

OCStackResult nmfindResource(const std::string& host , const std::string& resourceName);
void onObserve(const HeaderOptions &headerOption , const OCRepresentation& rep , const int& eCode, const int& sequenceNumber);

void findResourceCandidate()
{
    try
    {
        nmfindResource("" , "coap://224.0.1.187/oc/core?rt=NotificationManager.Hosting");
        std::cout << "Finding Resource... " << std::endl;
        while(true)
        {
        	char signal;
			cin >> signal;

			switch(signal)
			{
			case 'q':
			case 'Q':
				exit(-1);
			default:
				break;
			}
        }

    }
    catch(OCException& e)
    {
    }
}

void startObserve(std::shared_ptr< OCResource > resource)
{
    g_curResource = resource;

    QueryParamsMap test;
    resource->observe(ObserveType::Observe , test , &onObserve);
}

int observe_count()
{
    static int oc = 0;
    return ++oc;
}

void onObserve(const HeaderOptions &headerOption , const OCRepresentation& rep , const int& eCode, const int& sequenceNumber)
{
	std::cout << "onObserve" << std::endl;
//    if(eCode == SUCCESS_RESPONSE)
	if(eCode <= OC_STACK_RESOURCE_DELETED)
    {

        AttributeMap attributeMap = rep.getAttributeMap();

        for(auto it = attributeMap.begin() ; it != attributeMap.end() ; ++it)
        {
            if(attributeMap.find(it->first) == attributeMap.end())
            {
                return;
            }
        }

        if(rep.getUri().empty())
        {
        	cout << "uri is null\n";
            return;
        }

        std::cout << std::endl;
        std::cout << "========================================================" << std::endl;
        std::cout << "Receive OBSERVE RESULT:" << std::endl;
        std::cout << "\tSequenceNumber: " << sequenceNumber << std::endl;
        for(auto it = attributeMap.begin() ; it != attributeMap.end() ; ++it)
        {
            std::cout << "\tAttribute name: " << it->first << " value: ";
            for(auto valueItr = it->second.begin() ; valueItr != it->second.end() ; ++valueItr)
            {
                std::cout << "\t" << *valueItr << " ";
            }

            std::cout << std::endl;
        }

        if(observe_count() > 30)
        {
            std::cout << "Cancelling Observe..." << std::endl;
            OCStackResult result = g_curResource->cancelObserve();

            std::cout << "Cancel result: " << result << std::endl;
            sleep(10);
            std::cout << "DONE" << std::endl;
            std::exit(0);
        }
    }
    else
    {
        std::cout << "onObserve Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

void foundResource(std::shared_ptr< OCResource > resource)
{
    std::string resourceURI;
    std::string hostAddress;
    try
    {
        if(resource)
        {
//            if(resource->uri().find("/a/NM/TempHumSensor/virtual") != std::string::npos)
			if(resource->uri().find("/a/NM/TempHumSensor") != std::string::npos)
            {
                std::cout << std::endl;
                std::cout << "========================================================" << std::endl;
                std::cout << "DISCOVERED Resource(Consumer):" << std::endl;

                resourceURI = resource->uri();
                std::cout << "\tURI of the resource: " << resourceURI << std::endl;

                hostAddress = resource->host();
                std::cout << "\tHost address of the resource: " << hostAddress << std::endl;

                startObserve(resource);
            }
        }
        else
        {
            std::cout << "Resource is invalid" << std::endl;
        }

    }
    catch(std::exception& e)
    {
    }
}

OCStackResult nmfindResource(const std::string& host , const std::string& resourceName)
{
    return OCPlatform::findResource(host , resourceName , &foundResource);
}

void getRepresentation(std::shared_ptr< OCResource > resource)
{
    if(resource)
    {
        std::cout << "Getting Light Representation..." << std::endl;
    }
}

void onPut(const OCRepresentation& rep , const int eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "PUT request was successful" << std::endl;

        AttributeMap attributeMap = rep.getAttributeMap();

        for(auto it = attributeMap.begin() ; it != attributeMap.end() ; ++it)
        {
            std::cout << "\tAttribute name: " << it->first << " value: ";
            for(auto valueItr = it->second.begin() ; valueItr != it->second.end() ; ++valueItr)
            {
                std::cout << "\t" << *valueItr << " ";
            }

            std::cout << std::endl;
        }

        std::vector< OCRepresentation > children = rep.getChildren();

        for(auto oit = children.begin() ; oit != children.end() ; ++oit)
        {
            attributeMap = oit->getAttributeMap();

            for(auto it = attributeMap.begin() ; it != attributeMap.end() ; ++it)
            {
                std::cout << "\tAttribute name: " << it->first << " value: ";
                for(auto valueItr = it->second.begin() ; valueItr != it->second.end() ; ++valueItr)
                {
                    std::cout << "\t" << *valueItr << " ";
                }

                std::cout << std::endl;
            }
        }

        if(OBSERVE_TYPE_TO_USE == ObserveType::Observe)
            std::cout << std::endl << "Observe is used." << std::endl << std::endl;
        else if(OBSERVE_TYPE_TO_USE == ObserveType::ObserveAll)
            std::cout << std::endl << "ObserveAll is used." << std::endl << std::endl;

        QueryParamsMap test;

        g_curResource->observe(ObserveType::Observe , test , &onObserve);

    }
    else
    {
        std::cout << "onPut Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

// callback handler on GET request
void onGet(const HeaderOptions &headerOption , const OCRepresentation& rep , const int eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "GET request was successful" << std::endl;

        AttributeMap attributeMap = rep.getAttributeMap();

        std::cout << "Resource URI: " << rep.getUri() << std::endl;

        for(auto it = attributeMap.begin() ; it != attributeMap.end() ; ++it)
        {
            std::cout << "\tAttribute name: " << it->first << " value: ";
            for(auto valueItr = it->second.begin() ; valueItr != it->second.end() ; ++valueItr)
            {
                std::cout << "\t" << *valueItr << " ";
            }

            std::cout << std::endl;
        }

        std::vector< OCRepresentation > children = rep.getChildren();

        for(auto oit = children.begin() ; oit != children.end() ; ++oit)
        {
            std::cout << "Child Resource URI: " << oit->getUri() << std::endl;

            attributeMap = oit->getAttributeMap();

            for(auto it = attributeMap.begin() ; it != attributeMap.end() ; ++it)
            {
                std::cout << "\tAttribute name: " << it->first << " value: ";
                for(auto valueItr = it->second.begin() ; valueItr != it->second.end() ; ++valueItr)
                {
                    std::cout << "\t" << *valueItr << " ";
                }

                std::cout << std::endl;
            }
        }
    }
    else
    {
        std::cout << "onGET Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

void getLightRepresentation(std::shared_ptr< OCResource > resource)
{
    if(resource)
    {
        std::cout << "Getting Light Representation..." << std::endl;

        QueryParamsMap test;
        resource->get(test , &onGet);
    }
}

void PrintUsage()
{
    std::cout << std::endl;
    std::cout << "Usage : simpleclient <ObserveType>" << std::endl;
    std::cout << "   ObserveType : 1 - Observe" << std::endl;
    std::cout << "   ObserveType : 2 - ObserveAll" << std::endl;
}

int main(int argc , char* argv[])
{

    if(argc == 1)
    {
        OBSERVE_TYPE_TO_USE = ObserveType::Observe;
    }
    else if(argc == 2)
    {
        int value = atoi(argv[1]);
        if(value == 1)
            OBSERVE_TYPE_TO_USE = ObserveType::Observe;
        else if(value == 2)
            OBSERVE_TYPE_TO_USE = ObserveType::ObserveAll;
        else
            OBSERVE_TYPE_TO_USE = ObserveType::Observe;
    }
    else
    {
        PrintUsage();
        return -1;
    }

    PlatformConfig cfg;

    OCPlatform::Configure(cfg);

    std::cout << "Created Platform..." << std::endl;
    findResourceCandidate();

    return 0;
}

