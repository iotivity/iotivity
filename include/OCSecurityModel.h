//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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
