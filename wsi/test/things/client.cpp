//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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
#include "common.h"
typedef std::map<OCResourceIdentifier, std::shared_ptr<OCResource>> DiscoveredResourceMap;

DiscoveredResourceMap discoveredResources;
std::shared_ptr<OCResource> curResource;
std::mutex curResourceLock;

class Client
{
	public:
		string name;
		std::string m_id;
		map<string, double> props;
};

Client myclient;

void onPut(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{
	try
	{
		if(eCode == OC_STACK_OK)
		{
			std::cout << "PUT request was successful" << std::endl;
			typedef std::map<std::string, double>::iterator it_type;
			for(it_type iterator = things[chosenThing].props.begin(); iterator != things[chosenThing].props.end(); iterator++) {
				std::cout << "Key  : " << iterator->first << " Value = "<<rep[iterator->first]<<std::endl;
			}
		}
		else
		{
			std::cout << "onPut Response error: " << eCode << std::endl;
			std::exit(-1);
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Exception: " << e.what() << " in onPut" << std::endl;
	}
}

void putClientRepresentation(std::shared_ptr<OCResource> resource, string &key, double value)
{
	if(resource)
	{
		OCRepresentation rep;
		std::cout << "Putting light representation..."<<std::endl;
		rep.setValue(key, value);
		resource->put(rep, QueryParamsMap(), &onPut);
	}
}

void onGet(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{
	try
	{
		if(eCode == OC_STACK_OK)
		{
			std::cout << "GET request was successful" << std::endl;
			std::cout << "Resource URI: " << rep.getUri() << std::endl;
			std::cout << "Attributes : " << rep.numberOfAttributes() << std::endl;
			typedef std::map<std::string, double>::iterator it_type;
			for(it_type iterator = things[chosenThing].props.begin(); iterator != things[chosenThing].props.end(); iterator++) {
				std::cout << "Key  : " << iterator->first << " Value = "<<rep[iterator->first]<<std::endl;
			}
		}
		else
		{
			std::cout << "onGET Response error: " << eCode << std::endl;
			std::exit(-1);
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Exception: " << e.what() << " in onGet" << std::endl;
	}
}

void getClientRepresentation(std::shared_ptr<OCResource> resource)
{
	if(resource)
	{
		std::cout << "Getting Client Representation..."<<std::endl;
		// Invoke resource's get API with the callback parameter

		QueryParamsMap test;
		resource->get(test, &onGet);
	}
}

void foundResource(std::shared_ptr<OCResource> resource)
{
	std::cout << "In foundResource\n";
	std::string resourceURI;
	std::string hostAddress;
	try
	{
		std::lock_guard<std::mutex> lock(curResourceLock);
		if(discoveredResources.find(resource->uniqueIdentifier()) == discoveredResources.end())
		{
			std::cout << "Found resource " << resource->uniqueIdentifier() <<
				" for the first time on server with ID: "<< resource->sid()<<std::endl;
			discoveredResources[resource->uniqueIdentifier()] = resource;
		}
		else
		{
			std::cout<<"Found resource "<< resource->uniqueIdentifier() << " again!"<<std::endl;
		}

		if(curResource)
		{
			std::cout << "Found another resource, ignoring"<<std::endl;
			return;
		}
		if(resource)
		{
			std::cout<<"DISCOVERED Resource:"<<std::endl;
			resourceURI = resource->uri();
			std::cout << "\tURI of the resource: " << resourceURI << std::endl;
			hostAddress = resource->host();
			std::cout << "\tHost address of the resource: " << hostAddress << std::endl;
			std::cout << "\tList of resource types: " << std::endl;
			for(auto &resourceTypes : resource->getResourceTypes())
			{
				std::cout << "\t\t" << resourceTypes << std::endl;
			}
			std::cout << "\tList of resource interfaces: " << std::endl;
			for(auto &resourceInterfaces : resource->getResourceInterfaces())
			{
				std::cout << "\t\t" << resourceInterfaces << std::endl;
			}
			if(resourceURI == things[chosenThing].uri)
			{
				curResource = resource;
			}
		}
		else
		{
			std::cout << "Resource is invalid" << std::endl;
		}
	}
	catch(std::exception& e)
	{
		std::cerr << "Exception in foundResource: "<< e.what() << std::endl;
	}
	std::cout << "-----------------------Exit foundResource\n";
}

static FILE* client_open(const char* /*path*/, const char *mode)
{
	//return fopen("./oic_svr_db_client.json", mode);
	return NULL;
}

void printMenu()
{
	sleep(3);
	printf("\nProvide OCF Operation\n");
	printf("1 : Get Resource\n");
	printf("2 : Put Resource\n");
	return;
}


int main(int argc, char* argv[]) {

	std::ostringstream requestURI;
	OCPersistentStorage ps {client_open, fread, fwrite, fclose, unlink };

	if(argc<2)
	{
		printf("\nUsage : Provide Client Type\n");
		for(int i = 0; i<RVI+1; i++)
			printf("%d: %s\n", i, things[i].desc.c_str());
		return -1;
	}

	chosenThing = atoi(argv[1]);

	// Create PlatformConfig object
	PlatformConfig cfg {
		OC::ServiceType::InProc,
			OC::ModeType::Both,
			"0.0.0.0",
			0,
			OC::QualityOfService::LowQos,
			&ps
	};

	OCPlatform::Configure(cfg);
	try
	{
		std::cout.setf(std::ios::boolalpha);
		requestURI << OC_RSRVD_WELL_KNOWN_URI;
		OCPlatform::findResource("", requestURI.str(), CT_DEFAULT, &foundResource);
		std::cout<< "Finding Resource... " <<std::endl;
		int i = 0;
		do{
			if(i == 1){
				getClientRepresentation(curResource);
			}
			if(i == 2){
				string key;
				double value = 0.0;
				cout << endl << "Enter the key : "<<endl;
				cin >> key;
				cout << endl << "Enter the value : "<<endl;
				cin >> value;
				putClientRepresentation(curResource, key, value);
			}
			i = 0;
			printMenu();
			scanf("%d", &i);
		}while(i!=0);
	}catch(OCException& e)
	{
		oclog() << "Exception in main: "<<e.what();
	}

	return 0;
}


