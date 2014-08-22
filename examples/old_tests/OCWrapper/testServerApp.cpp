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

///
/// This sample provides steps to define an interface for a resource
/// (properties and methods) and host this resource on the server. 
///

#include <functional>

#include <OCServer.h>
#include <OCReflect.h>
#include <OCPlatform.h>
#include <OCApi.h>
#include <iostream>

using namespace std;

using namespace OC;
using namespace OC::OCReflect;

/// This class represents a single resource named 'light'. This resource has 
/// two simple properties named 'state' and 'power' and they have respective setter
/// and getter methods.

class light
{
private:
	/// Access this property from a TB client 
	bool m_state;
	int m_power;

public:
	light()
		: m_power(0),
		m_state(false)
	{}

public:
	/// Setter method for the setting the power of this light resource
	void setPower(int powerValue)             
	{ 
		m_power = powerValue; 
	}

	/// Getter method for the getting the power of this light resource
	int getPower() const                           
	{ 
		return m_power; 
	}

	/// Setter method for the setting the state of this light resource
	void setState(bool state)             
	{ 
		m_state = state; 
	}

	/// Getter method for the getting the state of this light resource
	bool getState() const                           
	{ 
		return m_state; 
	}
	
public:
	/* Note that this does not need to be a member function: for classes you do not have
	access to, you can accomplish this with a free function: */
	
	/// This function binds the properties and methods to the server. 
	void bindTo(OC::OCPlatform& platform)
	{
		using OC::OCReflect::property_type;
		using OC::OCReflect::property_binding;

		property_binding_vector properties {
			property_binding("state", property_type::boolean),
			property_binding("power", property_type::integer)
		};

		std::string resourceURI = "/a/light";
		std::string resourceTypeName = "light";
		platform.registerResource(resourceURI, resourceTypeName, properties);
	}
};

int main()
{
	// Step1: Create a OCPlatform instance. 
	// Step1.1: The constructor of OCPlatform class takes PlatformConfig object.
	// Note: Platform creation is synchronous call. 

	PlatformConfig cfg;
	cfg.ipAddress = "192.168.1.5";
	cfg.port = 8080;
	cfg.mode = ModeType::Server;
	cfg.serviceType = ServiceType::InProc;

    cout << "Creating OCPlatform .. \n";
	
	OCPlatform platform(cfg);
	
	// Step2: Create the instance of the resource class (in this case instance of class 'light'). 
	// Step2.1: Invoke bindTo function of class light. 
	
	light myLight;
	myLight.bindTo(platform);

	while(true)
	{
		//some tasks
	}

	// No explicit call to stop the platform. 
	// When OCPlatform destructor is invoked, internally we do platform cleanup
}
