//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/// @file OCPlatformHandler.h 

/// @brief	This file contains the declaration of classes and its members related to 
///			OCPlatformHandler.

#ifndef __OCPLATFORMHANDLER_H
#define __OCPLATFORMHANDLER_H

#include "OCApi.h"

namespace OC
{
	class OCPlatform;

	/**
	* @brief	OCPlatformHandler is a pure abstract class and it can be used for 
	*			registering and getting callbacks
	*/
	class OCPlatformHandler
	{
	public:
		OCPlatformHandler(void);

		virtual ~OCPlatformHandler(void);

		/**
		* @fn	This function is called when the platform gets initialzed
		* 
		* @param platform pointer to OCPlatform object
		*/
		virtual void onPlatformInitialized(OCPlatform *platform) = 0;

		/**
		* @fn	This function is called when platform status is changed
		* 
		* @param platform - pointer to OCPlatform object
		* @param status - OCPlatform status
		*/
		virtual void onPlatformStatusChanged(OCPlatform *platform, OCPlatformStatus status) = 0;
	};
}

#endif // __OCPLATFORMHANDLER_H
