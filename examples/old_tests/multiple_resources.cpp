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
/// This sample provides steps to define an interface for multiple resources
/// (properties and methods) and host these resources on the server. 
///
#include <functional>

#include "OCServer.h"
#include "OCReflect.h"

using namespace OC::OCReflect;

class dimmer
{
private:
	int m_level;
	int m_color;

public:
	bool m_powered;				// This is an observable property
	std::string manufacturer;	// This is a non-observable property

public:
	dimmer()
		: m_level(0),
		m_color(0),
		m_powered(false),
		manufacturer("Integrated Electronics")
	{}

public:
	/// Setter method for the setting the power state of this dimmer resource
	void setPowered(const bool newState)             
	{ 
		m_powered = newState; 
	}

	/// Getter method for the setting the power state of this dimmer resource
	bool getPowered() const                           
	{ 
		return m_powered; 
	}

	/// Setter method for the setting the intensity level of this dimmer resource
	/// Along with above basic method, this method takes two arguments
	/// with first argument as intensity level and second level as color.
	void setLevel(const int newLevel, int color)     
	{ 
		m_level = newLevel; 
		m_color = color; 
	}

	/// Getter method for getting the intensity level of the dimmer resoruce
	int getLevel() const                              
	{ 
		return m_level; 
	}

	/// Note that this does not need to be a member function: for classes you do not have
	/// access to, you can accomplish this with a free function:
public:
	void bindTo(OC::OCServer& server, const std::string& base_URI)
	{
		/*using OC::OCReflect::method_signature;
		using OC::OCReflect::method_binding;
		using OC::OCReflect::property_signature;
		using OC::OCReflect::property_binding;
		using OC::OCReflect::bind_service;
		using OC::OCReflect::bind_property;

		using OC::OCReflect::property_type;*/

		// Register the URI with the server
		server.registerResource(this, base_URI);

		// Steps to map the property

		// The canonical way to bind a property to a server in one step:
		bind_property(
			server,                                      // server to bind to
			this->manufacturer,                          // pointer to property
			"manufacturer",                              // property binding name
			OC::OCReflect::property_type::string,        // property
			OC::OCReflect::property_attribute::r         // type decoration
			);

		// The canonical way to bind a property to a server in one step:
		bind_property(
			server,                                      // server to bind to
			this->m_powered,			     // pointer to property
			"powerState",				     // property binding name
			OC::OCReflect::property_type::boolean, 	     // property
			OC::OCReflect::property_attribute::r         // type decoration
			);

		// Steps to map the methods

		// Example to map getPowered method using the canonical way in one step:
		bind_service(server,			// server to bind to
			this,						// instance (ourself) to bind 
			&dimmer::getPowered,			// address of the method to bind
			"getPowered",				// name to bind with service URL
			property_type::boolean);	// return type of the method

		// Example to map setPowered method using the canonical way in one step:
		bind_service(server,			// server to bind to
			this,						// instance (ourself) to bind 
			&dimmer::setPowered,			// address of the method to bind
			"setPowered",				// name to bind with service URL
			property_type::nil,			// return type of the method
			property_type::boolean);	// parameter type for setPowered method

		// Example to map setLevel method using the canonical way in one step:
		bind_service(
			server,                         // server to bind with
			this,                           // instance to bind
			&dimmer::setLevel,              // method to bind
			"setLevel",                     // service name
			property_type::nil,             // input type
			property_type::integer,         // parameter list starts (parameter type for level)
			property_type::integer);        // parameter list (parameter type for color) 

		// Example to map getLevel method using the canonical way in one step:
		bind_service(server, this, &dimmer::getLevel, "getLevel", property_type::integer);
	}
};

class door
{
 bool m_locked;

public:
	std::string manufacturer;

public:
	door()
		: m_locked(true),
		manufacturer("Integrated Electronics")
	{}

public:
	/// Setter method for the setting the lock state of this door resource
	void setLockState(const bool lockState)             
	{ 
		m_locked = lockState; 
	}

	/// Getter method for the setting the lock state of this door resource
	bool getLockState() const                           
	{ 
		return m_locked; 
	}

	/* Note that this does not need to be a member function: for classes you do not have
	access to, you can accomplish this with a free function: */
public:
	void bindTo(OC::OCServer& server, const std::string& base_URI)
	{
		/*using OC::OCReflect::method_signature;
		using OC::OCReflect::method_binding;
		using OC::OCReflect::property_signature;
		using OC::OCReflect::property_binding;
		using OC::OCReflect::bind_service;
		using OC::OCReflect::bind_property;

		using OC::OCReflect::property_type;*/

		// Register the URI with the server
		server.registerResource(this, base_URI);

		// Steps to map the property

		// The canonical way to bind a property to a server in one step:
		bind_property(
			server,                                      // server to bind to
			this->manufacturer,                          // pointer to property
			"manufacturer",                              // property binding name
			OC::OCReflect::property_type::string,        // property
			OC::OCReflect::property_attribute::r         // type decoration
			);

		// Steps to map the methods

		// Example to map getPowered method using the canonical way in one step:
		bind_service(server,			// server to bind to
			this,						// instance (ourself) to bind 
			&door::getLockState,		// address of the method to bind
			"getLockState",				// name to bind with service URL
			property_type::boolean);	// return type of the method

		// Example to map setPowered method using the canonical way in one step:
		bind_service(server,			// server to bind to
			this,						// instance (ourself) to bind 
			&door::setLockState,		// address of the method to bind
			"setLockState",				// name to bind with service URL
			property_type::nil,			// return type of the method
			property_type::boolean);	// parameter type for setPowered method
	}
};

class garage 
{
private:
	dimmer myDimmer;
	door myDoor;

public:
	void bindTo(OC::OCServer& server, const std::string& base_URI)
	{
		// Register the URI with the server
		server.registerResource(this, base_URI);

		// Bind child resource 'dimmer' to the server 
		myDimmer.bindTo(server, base_URI + "/dimmer/a");

		// Bind child resource 'door' to the server
		myDoor.bindTo(server, base_URI + "/door/a");
	}
};

int main()
{
	//Step1: create a server
	OC::OCSecurityModel securityModel;
	OC::OCServer server;
	server.setSecurityModel(securityModel);

	//Step2: Create a Garage resource
	garage myGarage;

	//Step3: Bind garage resource to the server
	//Note: Garage resource has child resources -- dimmer and door.
	myGarage.bindTo(server, "/garage/");

	//Step4: Start the server
	server.start();

	while(true)
	{
		//do something here
	}

	//Step4: Stop the server
	server.stop();
}
