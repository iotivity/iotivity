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

#include <iostream>

#include "ocapi.h"
#include "OCReflect.h"
#include "OCObserver.h"
#include "OCException.h"
#include "MyMultiResourceHandler.h"

namespace {

void log(const std::string& msg)
{
 std::cerr << msg << '\n';
}

} // namespace

void OC::MyMultiResourceHandler::onFoundResource(OCResourceResult *update, void *params){

	//Step1: Find a specific resource that also contains OCReflect byte stream of methods and properties
	for( auto &resource : update->resources ) 
	{
		if(resource.getURI() == "/garage/dimmer/a")
		{
			performDimmerOperations(resource, update);
		}
		else if(resource.getURI() == "/garage/door/a")
		{
			performDoorOperations(resource, update);
		}
	}
}

void OC::MyMultiResourceHandler::performDimmerOperations(OC::OCResource myResource, OC::OCResourceResult *update)
{
        using OC::OCReflect::entity;
        using OC::OCReflect::method;
        using OC::OCReflect::remote_resource;

        using OC::bind_observer;
        using OC::unbind_observer;

	try
	{
		// Perform resource operation
		// Note that this resource dimmer has 
		// 'manufacturer' as a property
		// 'powerState' as a observable property
		// 'setPowered' as a method 
		// 'getPowered' as a method 

		//Step1: Find a specific method and invoke it. or Find a specific property and use it.

		// Canonical one-step approach to get a callable function object:
		// OCReflect::method throw OC::OCReflect::reflect_exception if there is no setLevel method in the myResource
		auto setPowered = OCReflect::method(myResource, "setPowered");

		// invoke the remote method, 
		// invalid arguments return as an OC::OCReflect::reflect_exception
		setPowered(true); 

		// Canonical one-step approach to get a callable function object:
		// OCReflect::method throw OC::OCReflect::reflect_exception if there is no setLevel method in the myResource
		auto getPowered = OCReflect::method(myResource, "getPowered");

		// invoke the remote method, 
		// invalid arguments return as an OC::OCReflect::reflect_exception
		// bool power = getPowered(); // Use power variable to do other operations

		// Canonical one-step approach to access a property:
		std::string manufacturer = update->property<std::string>("manufacturer");

		//Example to observe a property
		bind_observer(&myObserverHandler, myResource, "powerState");

		//Example to unobserve a property
		unbind_observer(&myObserverHandler, myResource, "powerState");
	}
	catch(OC::reflection_exception& e)
	{
		log(e.what());
	}
	catch(std::exception& e)
	{
		log(e.what());
	}
}

void OC::MyMultiResourceHandler::performDoorOperations(OC::OCResource myResource, OC::OCResourceResult *update)
{
	// Perform resource operation
	// Note that this resource door has 
	// 'manufacturer' as a property
	// 'setLockState' as a method 
	// 'getLockState' as a method

	try
	{
		// Step1: Find a specific method and invoke it or find a specific property and use it.

		// Canonical one-step approach to get a callable function object:
		// OCReflect::method throw OC::OCReflect::reflect_exception if there is no setLevel method in the myResource
		auto setLockState = OCReflect::method(myResource, "setLockState");

		// invoke the remote method, 
		// invalid arguments return as an OC::OCReflect::reflect_exception
		setLockState(true); 

		// Canonical one-step approach to get a callable function object:
		// OCReflect::method throw OC::OCReflect::reflect_exception if there is no setLevel method in the myResource
		auto getLockState = OCReflect::method(myResource, "getLockState");

		// invoke the remote method, 
		// invalid arguments return as an OC::OCReflect::reflect_exception
		// bool lockState = getLockState(); // use lockState variable for any other operations

		// Canonical one-step approach to access a property
		std::string manufacturer = update->property<std::string>("manufacturer");
	}
	catch(OC::reflection_exception& e)
	{
		log(e.what());
	}
	catch(std::exception& e)
	{
		log(e.what());
	}
}

void OC::MyMultiResourceHandler::MyMultiResourceHandler::onCompleted()
{

}

void OC::MyMultiResourceHandler::MyMultiResourceHandler::onFailed()
{

}
