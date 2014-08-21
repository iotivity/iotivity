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

/// @file OCSecurityModel.h 

/// @brief	This file contains the declaration of classes and its members required to provide 
///			security model support. 

#ifndef __OCSECURITYMODEL_H
#define __OCSECURITYMODEL_H

#include "OCApi.h"

namespace OC
{
	/**
	* @brief	This class provides the required security model to 
	*			access a service or a particular resource.
	*/
	class OCSecurityModel
	{
	public:
		OCSecurityModel(void);

		virtual ~OCSecurityModel(void);

		/**
		* @fn	checks the security access
		* 
		* @param sourceURI - source URI 
		* @param destinationURI - destination URI of the resoruce to access 
		* @param encryptionLevel - Encryption level
		* @param remoteIdentity - remote identity
		*/
		void checkAccess(URI sourceURI, URI destinationURI, int encryptionLevel, int remoteIdentity);
	};
}

#endif //__OCSECURITYMODEL_H
