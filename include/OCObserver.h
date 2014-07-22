//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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
