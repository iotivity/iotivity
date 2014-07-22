//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//******************************************************************
// File name:
//     OCPlatform.cpp
//
// Description: Implementation of the OCPlatform.
//
//
//
//*********************************************************************

#include <random>

#include "OCPlatform.h"

namespace OC 
{
	// Constructor. Internally calls private init function
	OCPlatform::OCPlatform(const PlatformConfig& config)
	{
		init(config);
	}

	// Destructor
	OCPlatform::~OCPlatform(void)
	{
		std::cout << "platform destructor called" << std::endl;
		cleanup();
	}

	void OCPlatform::init(const PlatformConfig& config)
	{
		std::unique_ptr<WrapperFactory> wrapperInstance(new WrapperFactory());
		m_WrapperInstance = std::move(wrapperInstance);

		if(config.mode == ModeType::Server)
		{
			// Call server wrapper init 
			m_server = m_WrapperInstance->CreateServerWrapper(config);
		}
		else if(config.mode == ModeType::Client)
		{
			// Call client wrapper init
			m_client = m_WrapperInstance->CreateClientWrapper(config);
		}
		else 
		{
			// This must be both server and client
			m_server = m_WrapperInstance->CreateServerWrapper(config);
			m_client = m_WrapperInstance->CreateClientWrapper(config);
		}
	}

	void OCPlatform::cleanup()
	{
		if(m_server)
		{
			//delete m_server;
		}

		if(m_client)
		{
			//delete m_client;
		}
	}

    
	void OCPlatform::findResource(const std::string& host, const std::string& resourceName, 
		std::function<void(OCResource::Ptr)> resourceHandler)
	{
		if(m_client)
		{
			m_client->ListenForResource(host, resourceName, resourceHandler);
		}
	}
    

	void OCPlatform::registerResource(const std::string& resourceURI, const std::string& resourceTypeName, 
			named_property_binding_vector properties)
	{
		if(m_server)
		{
			try{
				m_server->registerResource(resourceURI, resourceTypeName, properties); 
			}catch(std::exception e) // define our own expception. 
			{
				throw e;
			}
		}
	}

} //namespace OC
