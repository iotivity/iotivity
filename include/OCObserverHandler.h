//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/// @file OCObserverHandler.h 

/// @brief	This file contains the declaration of classes and its members related to 
///			ObserverHandler.

#ifndef __OCOBSERVERHANDLER_H
#define __OCOBSERVERHANDLER_H

#include "OCApi.h"

namespace OC
{
	/**
	* @brief	OCObserverHandler is a pure abstract class and it can be used for 
	*			observer related callbacks
	*/

	class OCObserverHandler
	{
	public:
		virtual ~OCObserverHandler(void)
		{}

		/**
		* @fn	This function is called when the property gets updated
		* 
		* @param propertyName name of the property
		* @param value value of the property
		*/
		virtual void onObserverUpdate(std::string propertyName, void *value) = 0;
	};

}
#endif //__OCOBSERVERHANDLER_H
