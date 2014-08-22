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

/// @file OCObserver.h 

/// @brief	This file contains the declaration of classes and its members related to 
///			OCObserver.

#ifndef __OCOBSERVER_H
#define __OCOBSERVER_H

#include <string>

#include "OCObserver.h"

namespace OC 
{
	/* It allows an app to register a method handler to monitor for 
	state changes on a resource. Not all resource attributes are observable. 
	*/
	class OCObserver
	{
	public:
		OCObserver(void);

		virtual ~OCObserver(void);
	};

	/* It is likely that these signatures will change somewhat: */
	template <typename HandlerT, typename ResourceT>
	void bind_observer(HandlerT handler, ResourceT& resource, const std::string& name)
	{};

	// unbind the specified handler:
	template <typename HandlerT, typename ResourceT>
	void unbind_observer(HandlerT handler, ResourceT& resource)
	{};

	// unbind by name:
	template <typename HandlerT, typename ResourceT>
	void unbind_observer(HandlerT handler, ResourceT& resource, const std::string& name)
	{};

} // namespace OC

#endif //__OCOBSERVER_H
