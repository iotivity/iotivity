//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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
			property_binding_vector properties)
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
