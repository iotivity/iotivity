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

/// @file OCServer.h 

/// @brief	This file contains the declaration of classes and its members required to provide 
///			functionality of a server. A server role supports resoruce registration, binding 
///			and advertisement. 

#ifndef __OCSERVER_H
#define __OCSERVER_H

#include <map>
#include <string>

#include "OCObject.h"
#include "OCPropertyTypes.h"
#include "OCSecurityModel.h"

namespace OC 
{
	/**
	@brief	A server instance which is the starting point for a UB device that wishes to host a 
			set of resource(s) for other devices. 
	*/
	class OCServer
	{
    /* 1) It may be worth considering Boost's lock-free containers here;
       2) The only reason bindings rather than the "real things" are shown in this case is because of
       an assumption that the real things lie within the C layer. In practice, there's no firm reason
       why it needs to be one way or the other-- in fact, a pure C++ implementation might as well
       just store all the actual methods and properties in one place: */
    private:
        std::map<std::string, OC::OCReflect::method_binding>      method_bindings;
        std::map<std::string, OC::OCReflect::property_binding>    property_bindings; 

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
 		void bind(const OC::OCReflect::property_binding& pb);

 		template <class T>
 		void registerResource(T *object, const std::string& base_URI)
 		{}

        // Note that these transfer the /binding information/ (signatures, etc.) not data (which could be /very/ expensive):
        public:
        const std::map<std::string, OC::OCReflect::method_binding>& methods()       const { return method_bindings; }
        const std::map<std::string, OC::OCReflect::property_binding>& properties()  const { return property_bindings; }

        // Transfer data:
        public:
        // Look up the method from the binding, call the function, return a full property as result:
        // OC::OCReflect::property call(const std::pair<std::string, OC::OCReflect::method_binding>& method); // other signatures possible, this is an example

        // Look up the property from the binding, return a full property (ie. with a data value):
        OC::OCReflect::property get(const std::string& p_name);
        OC::OCReflect::property get(const OC::OCReflect::property_binding& pb);

        // Set a property's value:
        void set(const OC::OCReflect::property& p);
	};
}

#endif //__OCSERVER_H
