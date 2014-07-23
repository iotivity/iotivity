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

/// @file OCServer.h 

/// @brief	This file contains the declaration of classes and its members required to provide 
///			functionality of a server. A server role supports resoruce registration, binding 
///			and advertisement. 

#ifndef __OCSERVER_H
#define __OCSERVER_H

#include <map>
#include <string>

#include "OCObject.h"
#include "OCSecurityModel.h"

namespace OC { namespace OCReflect {

struct method_binding;
struct property_binding;

}} // namespace OC::OCReflect

namespace OC 
{
	/**
	@brief	A server instance which is the starting point for a UB device that wishes to host a 
			set of resource(s) for other devices. 
	*/
	class OCServer
	{
    private:
        std::map<std::string, OC::OCReflect::method_binding>    method_bindings;

	public:
		OCServer(void);
		virtual ~OCServer(void);

		/**
		* @fn	This registers a security model for access control to resources under this service.
		* 
		* @param model - Security Model required for access to this service and control/view of it's
		*        services.
		*/
		void setSecurityModel(OCSecurityModel model) ;

		/**
		* @fn	Registers a resource with the service.
		* 
		* @param object - The OCObject that handles the resource requests
		* @param url - The URL under the resource.
		* @param accessControl - The access control handler that determines whether the remote device is
		*        allowed to access this resource. If NULL, all access is granted.
		*/
		void registerResource(OCObject *object, std::string url/* TODO: , AccessControl accessControl */);

		/**
		* @fn	Unregisters a resource with the service.
		* 
		* @param object - The OCObject to be unregistered.
		*/
		void unregisterResource(OCObject *object);

		/**
		* @fn	Starts the open connectivity service.
		* 
		* Thread-Safety: TODO Determine if it is going to be safe to call in a UI thread
		*/
		void start();

		/**
		* Stops the open connectivity service.
		* 
		* Thread-Safety: TODO Determine if it is going to be safe to call in a UI thread
		*/
		void stop();

		public:
		void bind(const OC::OCReflect::method_binding& mb);

 		void bind(const OC::OCReflect::property_binding& pb)
 		{}

 		template <class T>
 		void registerResource(T *object, const std::string& base_URI)
 		{}

        public:
        const std::map<std::string, OC::OCReflect::method_binding>& methods() const { return method_bindings; }
	};
}

#endif //__OCSERVER_H
