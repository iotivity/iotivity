//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/// @file OCResourceHandler.h 

/// @brief	This file contains the declaration of classes and its members related to 
///			ResourceHandler.

#ifndef _OCRESOURCEHANDLER_H
#define _OCRESOURCEHANDLER_H

#include "ocapi.h"
#include "OCResource.h"

namespace OC 
{
	/**
	* @brief	OCResourceHandler is a pure abstract class and it can be used for 
	*			resource related callbacks
	*/
	class OCResourceHandler
	{
	public:
		virtual ~OCResourceHandler(void){}
		/**
		* @fn	This function is called when it finds the resource
		* 
		* @param update resource result information
		* @param params reserved
		*/
		virtual void onFoundResource(OCResourceResult *update, void *params) = 0;
		/**
		* @fn	This function is called when the find is completed
		* 
		* @param propertyName name of the property
		* @param value value of the property
		*/
		virtual void onCompleted() = 0;
		/**
		* @fn	This function is called when the find is failed
		* 
		*/
		virtual void onFailed() = 0;

	};
}
#endif //_OCRESOURCEHANDLER_H

