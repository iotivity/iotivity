//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

///
/// This sample provides steps to define an interface for a resource
/// (properties and methods) and host this resource on the server. 
///

#include <functional>

#include "OCReflect.h"
#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
using namespace OC::OCReflect;

/// This class represents a single resource named 'lightResource'. This resource has 
/// two simple properties named 'state' and 'power' and they have respective setter
/// and getter methods.

class LightResource
{
private:
	/// Access this property from a TB client 
	bool m_state;
	int m_power;

public:
	LightResource()
		: m_state(false), m_power(0)
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
	void createResourceWithPropeties(OC::OCPlatform& platform)
	{
        /*
        We could typedef to simpler namers! :)
        typedef named_property_binding_vector OCPropertyBindings;
        typedef named_property_binding OCPropertyBinding;

        OCPropertyBindings properties {
			OCPropertyBinding("state", property_type::boolean),
			OCPropertyBidning("power", property_type::integer)
		};
        */

        auto properties {
			named_property_binding{"state", property_type::boolean},
			named_property_binding{"power", property_type::integer}
        };

		std::string resourceURI = "/a/light";
		std::string resourceTypeName = "light";

		// This will internally invoke registerResource method (which would eventually create 
		// resource). 

		platform.registerResource(resourceURI, resourceTypeName, properties);
	}
};

int main()
{
	// Create PlatformConfig object

	PlatformConfig cfg;
	//cfg.ipAddress = "192.168.1.5";
	cfg.ipAddress = "134.134.161.33";
	cfg.port = 5683;
	cfg.mode = ModeType::Server;
	cfg.serviceType = ServiceType::InProc;
	
	// Create a OCPlatform instance. 
	// Note: Platform creation is synchronous call. 
	try
	{
		OCPlatform platform(cfg);

		// Create the instance of the resource class (in this case instance of class 'LightResource'). 
		// Invoke bindTo function of class light. 

		LightResource myLightResource;
		myLightResource.createResourceWithPropeties(platform);
		
		// Perform app tasks
		while(true)
		{
			// some tasks
		}
	}
	catch(OCException e)
	{
		//log(e.what());
	}

		
	// No explicit call to stop the platform. 
	// When OCPlatform destructor is invoked, internally we do platform cleanup
}
