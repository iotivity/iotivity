/*
 * HostingHandler.h
 *
 *  Created on: 2014. 10. 15.
 *      Author: jyong2
 */

#ifndef HOSTINGHANDLER_H_
#define HOSTINGHANDLER_H_

#include "NotificationManager.h"

class HostingHandler
{
public:

	static void initialize();
	static void initialize(HostingConfig cfg);
	static HostingHandler *getInstance();

	void setHostingConfig(HostingConfig cfg);

	void changeHostingMode(HostingMode hostingMode, AutomaticMethod autoMethod = AutomaticMethod::None);
	void changeAutomaticHostingMethod(AutomaticMethod autoMethod);
	void changeNotifiyMethod(NotifyMethod notifyMethod);
	void changeNotifyFrequencyType(NotifyFrequency notifyFrequency);

private:

	HostingHandler();
	~HostingHandler();

	static HostingHandler *s_instance;
	static mutex s_mutexForCreation;

	HostingConfig hostingCfg;

	static std::function< void(bool isHosting) > s_findHostingCandidate;
	static std::function< void(std::string) > s_addExtraStr;
	static std::function< void(std::shared_ptr< OCResource > resource) > s_startHosting;
	static std::function< void(OCResourceHandle resourceHandle) > s_notify;

	void startFindHost();
	void onObserve(AttributeMap &AttMap, OCResourceHandle resourceHandle);
	void onFoundCandidate(std::shared_ptr< OCResource > resource);

	void runAutomaticHosting(AutomaticMethod autoMethod);
	void stopAutomaticHosting();

	void notifyFrequence(OCResourceHandle resourceHandle);

};

#endif /* HOSTINGHANDLER_H_ */
