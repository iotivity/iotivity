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
