//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// OCClient.cpp : Defines the entry point for the console application.
//
#include <string>

#include "OCPlatform.h"

using namespace OC;

void foundResource(std::shared_ptr<OCResource> resource){

	try
	{
		// Do some operations with resource object. 
		if(resource)
		{
			std::cout << "URI of the resource: " << resource->uri() << std::endl;
			
			std::cout << "Host address of the resource: " << resource->host() << std::endl;		
		}
        
		else
		{
			std::cout << "Resource invalid" << std::endl;
		}	
        
	}
	catch(OC::OCReflect::reflection_exception& e)
	{
		//log(e.what());
	}
	catch(std::exception& e)
	{
		//log(e.what());
	}
}


int main()
{
	// Create PlatformConfig object

	PlatformConfig cfg;
	cfg.ipAddress = "134.134.161.166";
	cfg.port = 5683;
	cfg.mode = ModeType::Client;
	cfg.serviceType = ServiceType::InProc;

	// Create a OCPlatform instance. 
	// Note: Platform creation is synchronous call. 

	try
	{
		OCPlatform platform(cfg);

		// Find all resources
		platform.findResource("", "coap://224.0.1.187/oc/core", &foundResource);

		while(1)
		{
		}

	}catch(OCException e)
	{
		//log(e.what());
	}

	return 0;
}

