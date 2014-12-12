/*
 * OICPlatformConfig.h
 *
 *  Created on: 2014. 10. 15.
 *      Author: jyong2
 */

#ifndef OICPLATFORMCONFIG_H_
#define OICPLATFORMCONFIG_H_

#include "OCApi.h"
#include "OCPlatform.h"
#include "OCResource.h"
#include "OCResourceRequest.h"
#include "OCResourceResponse.h"
#include "ocstack.h"

using namespace OC;
using namespace OCPlatform;

class OICPlatformConfig
{
private:

	OICPlatformConfig();
	~OICPlatformConfig();

	static OICPlatformConfig *s_instance;
	static mutex s_mutexForCreation;
	static PlatformConfig s_cfg;

public:

	void initialize();
	static OICPlatformConfig *getInstance();
	void setIP(std::string ipaddress);
};

#endif /* OICPLATFORMCONFIG_H_ */
