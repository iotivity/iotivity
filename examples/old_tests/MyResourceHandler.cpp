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
#include "MyResourceHandler.h"

void log(const std::string& msg)
{
 std::cerr << msg << '\n';
}

MyResourceHandler::MyResourceHandler(void)
{
}


MyResourceHandler::~MyResourceHandler(void)
{
}

void MyResourceHandler::onFoundResource(OCResourceResult *update, void *params){
	using OC::OCReflect::entity;
	using OC::OCReflect::method;
	using OC::OCReflect::remote_resource;

	using OC::bind_observer;
	using OC::unbind_observer;

	try
	{
		//Step1: Find a specific resource that also contains OCReflect byte stream of methods and properties
		OCResource myResource;
		for ( auto &resource : update->resources ) {
			if(resource.getURI() == "/light/a"){
				myResource = resource;
				break;
			}
		}

		//Step2: Find a specific method and invoke it. or Find a specific property and use it.

		// Canonical one-step approach to get a callable function object:
		// OCReflect::method throw OC::OCReflect::reflect_exception if there is no setLevel method in the myResource
		auto setLevel = OC::OCReflect::method(myResource, "setLevel");

		setLevel(75, 255); 

		// Individual steps-- each may throw on failure:
		remote_resource myLight(myResource, "lights/a");	// JFW: this may be subsumed by type OCResource

		entity e_setPowered = myLight("setPowered");

		method setPowered = OC::OCReflect::narrow<OCReflect::method>(e_setPowered);

		setPowered(true);

		// Canonical one-step approach to access a property:
		std::string manufacturer = update->property<std::string>("manufacturer");

		// Individual steps:
		entity e_manufacturer = myResource.property("manufacturer");
		std::string manufacturer_2 = OC::OCReflect::narrow<std::string>(e_manufacturer);

		//Example to observe a property
		bind_observer(&myObserverHandler, myResource, "PowerState");

		//Example to unobserve a property
		unbind_observer(&myObserverHandler, myResource, "PowerState");
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

void MyResourceHandler::onCompleted(){
}

void MyResourceHandler::onFailed(){
}
