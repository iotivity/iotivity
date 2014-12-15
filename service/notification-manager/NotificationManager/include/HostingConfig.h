
#ifndef HOSTINGCONFIG_H_
#define HOSTINGCONFIG_H_

#include "NotificationManager.h"


enum class HostingMode
{
	None,
	ManualMode,
	AutomaticMode,
};

enum class AutomaticMethod
{
	None,
	NetworkStatusChange,
	Timer,
	DeviceStatus
};

enum class NotifyMethod
{
	Frequence,
	Equalization,
	Granularity,
	None
};

enum class NotifyFrequency
{
	OnTime,
	Periodically,
	None
};


struct HostingConfig
{
	HostingMode		hostingMode;
	AutomaticMethod	automaticMethod;
	NotifyMethod		notifyMethod;
	NotifyFrequency	frequency;
};

#endif /* HOSTINGCONFIG_H_ */
