/*
 * HostingInterface.h
 *
 *  Created on: 2014. 10. 15.
 *      Author: jyong2
 */

#ifndef HOSTINGINTERFACE_H_
#define HOSTINGINTERFACE_H_

#include "NotificationManager.h"

using namespace OC;
using namespace OCPlatform;

class HostingInterface
{

public:
	HostingInterface();
	~HostingInterface();

	int setOnFoundHostingCandidate(
			std::function< void(std::shared_ptr< OCResource > resource) > func);
	int setOnObserve(std::function< void(AttributeMap &inputAttMap, OCResourceHandle resourceHandle) > func);

	int setFindHosting(std::function< void(bool isHosting) > &func);
	int setStartHosting(std::function< void(std::shared_ptr< OCResource > resource) > &func);
	int setNotifyObservers(std::function< void(OCResourceHandle resourceHandle) > &func);
	int setAddExtraStr(std::function< void(std::string) > &func);

};

#endif /* HOSTINGINTERFACE_H_ */
