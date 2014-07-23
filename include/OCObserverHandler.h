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
