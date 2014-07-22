//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef MODELIMPL_H_
#define MODELIMPL_H_

// ============================================================================
// Includes
// ============================================================================
#include <memory>
#include <map>
#include <list>
#include <stdint.h>

#include "Protocol.hpp"
#include "UUIDLess.hpp"
#include "InternalApi.h"
#include "Core.h"


// ============================================================================
// Namespace
// ============================================================================
namespace Intel {
namespace CCFL {
namespace API {

class ModelImpl : public Model
{
public:
	typedef std::shared_ptr<ModelImpl> SharedPtr;
	typedef std::weak_ptr<ModelImpl> WeakPtr;

public:
	// Factory
	static SharedPtr createModel();

public:
	// Constructor & Destructor
	ModelImpl();
	virtual ~ModelImpl();

	// Get the device list
	virtual void getDevices(GetDevicesFunction& asyncReturnFunc);

	// Add/remove device observer
	virtual void removeDeviceObserver(DeviceObserverHandle observerHandle);
	virtual void setDeviceObserver(DeviceEventFunction& asyncEventFunction);

	// Register/unregister a protocol
	virtual const Protocols::Protocol::Handle registerProtocol(const Protocols::Protocol::SharedPtr& protocol);
	virtual bool unregisterProtocol(const Protocols::Protocol::Handle protocolHandle);
	virtual Device::SharedPtr getDevice(const UUID_t& deviceId);
	virtual void signalDeviceChange(const UUID_t& deviceId, DeviceEvent::DeviceChange deviceEvent);

private:
	typedef std::map<Protocols::Protocol::Handle, Protocols::Protocol::WeakPtr> ProtocolMap;
	typedef std::list<Device::SharedPtr> DeviceList;
	typedef std::map<DeviceObserverHandle, DeviceEventFunction> DeviceObserverMap;
	typedef std::list<UUID_t> DeviceIdList;
private:
	const Protocols::Protocol::Handle generateProtocolHandle();
	bool isHandleInUse(Protocols::Protocol::Handle handle);
	Device::SharedPtr addDeviceToList(const UUID_t& deviceId);

	DeviceObserverHandle generateDeviceObserverHandle();
  bool isDeviceObserverHandleInUse(DeviceObserverHandle observerHandle);
  DeviceIdList getDeviceIdList();
  void notifyDeviceObservers(DeviceEvent::DeviceChange event, const DeviceIdList& deviceIdList);

private:
	ProtocolMap protocolMap_;
	DeviceList deviceList_;
	DeviceObserverMap deviceObserverMap_;

	mutex protocolMapMutex_;
	mutex deviceListMutex_;
	mutex deviceObserverMapMutex_;
};

}
}
}
#endif /* MODELIMPL_H_ */
