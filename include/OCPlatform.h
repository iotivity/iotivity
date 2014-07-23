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

/// @file OCPlatform.h 

/// @brief	This file contains the declaration of classes and its members related to 
///			OCPlatform.

#ifndef __OCPLATFORM_H
#define __OCPLATFORM_H

#include <map>

#include "OCApi.h"
#include "OCResource.h"
#include "OCPlatformHandler.h"
#include "WrapperFactory.h"
#include <OCReflect.h>

using namespace OC::OCReflect;

namespace OC
{
	using OC::OCReflect::property_type;
	using OC::OCReflect::named_property_binding;

	/**
	*	@brief	The core OC platform object initialized by the application or service via a static accessor.
	*			On successful initialization an instance of the OCPlatform is returned.  
	*			Handler registered that receives various platform/stack updates. The stack object 
	*			also provides methods for managing advertisement and discovery.  
	*/
	class OCPlatform
	{
	public:
		/**
		* @fn	 Constructor for OCPlatform. Constructs a new OCPlatform from a given PlatformConfig with 
		*		 appropriate fields
		* @param config - PlatformConfig struct which has details such as modeType (server/client/both),
		*				  in-proc/out-of-proc etc. 
		*/
		OCPlatform(const PlatformConfig& config);

		/**
		* @fn	 Virtual destructor
		*/
		virtual ~OCPlatform(void);

		/**
		* @fn	 API for Service and Resource Discovery
		* 
		* @param host - Host IP Address of a service to direct resource discovery query. If null or
		*        empty, performs service discovery OR resource discovery query to ALL group members.
		* @param filter<std::string FilterType, std::string Value> - Filter Type - Can be one of two of the link
		*        format equivalent attribute types declared in table 7 of the
		*        OC-Lite-Product-Specification-V1 Document. Either "n" (i.e. Resource Name) or "rt"
		*        (i.e. Resource Type). Value - Can be any valid std::string that's defined to work with CoRE
		*        Link Attributes as defined in the RFC6690.
		* @param handler - Handles callbacks, success states and failure states.
		* 
		*        Four modes of discovery defined as follows: 
		*        (NULL/Empty, NULL/Empty) - Performs ALL service discovery AND ALL resource discovery. 
		*        (NULL/Empty, Not Empty) - Performs query for a filtered/scoped/particular resource(s) 
		*                                  from ALL services. 
		*        (Not Empty, NULL/Empty) - Performs ALL resource discovery on a particular service. 
		*        (Not Empty, Not Empty) - Performs query for a filtered/scoped/particular resource(s) 
		*                                  from a particular service.
		* 
		*        Thread-Safety: TODO Determine if it is going to be safe to call in a UI thread
		*/
		void findResource(const std::string& host, const std::string& resourceName, 
			std::function<void(OCResource::Ptr)> resourceHandler);

		/**
		* @fn	Queries the network for available resources.
		* 
		* NOTE: This gets posted to a new thread so it doesn't hold the UI or network thread.
		* 
		* @param serviceURL - The URL to the service. NULL for a broadcast/multicast.
		* @param resourceType - The resource type filter. NULL for match any.
		* 
		* @throws This may throw if the service is not started, network is bad, etc.
		* 
		*         Thread-Safety: TODO Determine if it is going to be safe to call in a UI thread
		*/
		void findResourceByType(const std::string& serviceURL, const std::string& resourceType,
			std::function<void(OCResource::Ptr)> resourceHandler);

		/**
		* @fn	Registers the resource with its name, type and list of properties
		* 
		* NOTE: Properties are prepared prior to this call using generic functions from OCReflect
		* 
		* @param resourceURI - The URI of the resoruce.
		* @param resourceTypeName - The resource type. 
		* @param properties - vector of properties for this resource
		* 
		*/
		void registerResource(const std::string& resourceURI, const std::string& resourceTypeName, 
			named_property_binding_vector properties);

	private:
		std::unique_ptr<WrapperFactory> m_WrapperInstance;
		IServerWrapper::Ptr m_server;
		IClientWrapper::Ptr m_client;

		/**
		*  @fn	Private function to initalize the platfrom 
		*/
		void init(const PlatformConfig& config);

		/**
		*  @fn	Private function cleanup the platform
		*/
		void cleanup();
	};
}

#endif //__OCPLATFORM_H
