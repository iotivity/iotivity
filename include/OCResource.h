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

/// @file OCResource.h 

/// @brief	This file contains the declaration of classes and its members related to 
///			Resource.

#ifndef __OCRESOURCE_H
#define __OCRESOURCE_H

#include <memory>
#include <random>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <OCApi.h>
#include <ResourceInitException.h>

namespace OC
{
	/**
	*	@brief	OCResource represents an OC resource. A resource could be a light controller, 
	*			temperature sensor, smoke detector, etc. A resource comes with a well-defined 
	*			contract or interface onto which you can perform different operations, such as 
	*			turning on the light, getting the current temperature or subscribing for event 
	*			notifications from the smoke detector. A resource can be composed of one or
	*			more resources. 
	*/
	class OCResource
	{
	public:
		typedef std::shared_ptr<OCResource> Ptr;
		
		OCResource(std::string host, boost::property_tree::ptree& resourceNode);
		virtual ~OCResource(void);
		
		// Gets the device host URI
		std::string host() const {return m_host;}
		// Gets the device-relative URI for this resource
		std::string uri() const {return m_uri;}
		// bool whether it is possible to attach an observer to this resource
		bool isObservable() const {return m_isObservable;}
		// bool whether this is a collection type, and will have children that can be queried
		bool isCollection() const {return m_isCollection;}
		// a collection of the resource-type names
		const std::vector<std::string> resourceTypes() const {return m_resourceTypes;}
		// a collection of the interfaces supported by this resource
		const std::vector<std::string> interfaces() const { return m_interfaces;}
		// a collection of property objects that allow calling of properties on this Resource
		const std::vector<std::string> properties() const { return m_properties;}
		// a collection of the names of the children of this resource, assuming it supports a collection interface
		const std::vector<std::string> children() const {return m_children;}
		
		
		// a method which allows 
	private:	
		std::string m_uri;
		std::string m_host;
		bool m_isObservable;
		bool m_isCollection;
		std::vector<std::string> m_resourceTypes;
		std::vector<std::string> m_interfaces;
		std::vector<std::string> m_properties; // TODO: give a better type once we have the ability to call one!
		std::vector<std::string> m_children;
	};

} // namespace OC

#endif //__OCRESOURCE_H
