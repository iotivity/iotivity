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

/// @file OCObject.h 

/// @brief	This file contains the declaration of classes and its members related to 
///			OCObject.

#ifndef __OCOBJECT_H
#define __OCOBJECT_H

namespace OC 
{
	/**
	* @brief	The OCObject is the root abstract class from which many of the OC related objects derive from.
	*			It implements a common set of functionality supported by all the OC classes in the 
	*			SDK such as object naming, ID, natural sort order and comparison. 
	*/
	class OCObject
	{
	public:
		OCObject(void);
		
		virtual ~OCObject(void);
		
		/**
		* @fn	Determines if the resource should be listed in the well-known core list.
		* 
		* NOTE: This may be better handled at a different location.
		* 
		* @return Returns true if resource should be listed.
		*/
		virtual bool isVisible() = 0;
	};
}

#endif //__OCOBJECT_H

