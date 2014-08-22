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

// ============================================================================
// Includes
// ============================================================================
#include "ModelImpl.h"
//#include "DeviceImpl.h"
#include "SimpleLogger.h"
#include <stdlib.h>
#include <time.h>
#include <cstdlib>

// ============================================================================
// Namespace
// ============================================================================
namespace Intel {
namespace CCFL {
namespace API {

static const char TAG[] = "ModelImpl";

// ============================================================================
// GetDevicesResult Implementation Class
// ============================================================================
class GetDeviceSuccess : public GetDevicesResult {
public:
	virtual ~GetDeviceSuccess() {}

// ============================================================
// Public Method(s)
// ============================================================
public:
	virtual QueryResultType getResult() const {
		return SUCCESS;
	}

	virtual const std::list<Device::SharedPtr>& getDeviceList() const {
		return deviceList_;
	}

public:
	std::list<Device::SharedPtr> deviceList_;
};

// ============================================================================
// DeviceEvent Implementation Class
// ============================================================================
class DeviceEventSuccess : public DeviceEvent {
// ============================================================
// Constructor & Destructor
// ============================================================
public:
	DeviceEventSuccess() {
		deviceObserverHandle_ = INVALID_DEVICE_OBSERVER_HANDLE;
	}
	virtual ~DeviceEventSuccess() {}

// ============================================================
// Public Method(s)
// ============================================================
public:
	virtual QueryResultType getResult() const {
		return SUCCESS;
	}

	virtual DeviceObserverHandle getObserverHandle() const {
    return deviceObserverHandle_;
	}

	virtual const std::list<DeviceEventInfo>& getDeviceIdList() const {
		return deviceInfoList_;
	}

public:
	DeviceObserverHandle deviceObserverHandle_;
	std::list<DeviceEventInfo> deviceInfoList_;
};

// ============================================================================
// Model Factory
// ============================================================================
Model::SharedPtr Model::createModel() {
	logDebug(TAG, "Entering Model::createModel");
	return ModelImpl::createModel();
}

ModelImpl::SharedPtr ModelImpl::createModel() {
	// Get singleton of ModelImpl.
	logDebug(TAG, "Entering ModelImpl::createModel");
	static SharedPtr instance = std::make_shared<ModelImpl>();

	return instance;
}
// ============================================================================
// Constructors & Destructors
// ============================================================================
ModelImpl::ModelImpl() {
	logDebug(TAG, "Entering ModelImpl::ModelImpl");

	// initialize random seed for use with random protocol handles
	srand(time(NULL));
}

ModelImpl::~ModelImpl() {
	logDebug(TAG, "Entering ModelImpl::~ModelImpl");
	protocolMap_.clear();
	deviceList_.clear();
	deviceObserverMap_.clear();
}

// ============================================================================
// Public methods
// ============================================================================

void ModelImpl::getDevices(GetDevicesFunction& asyncReturnFunc) {
	logDebug(TAG, "Entering ModelImpl::getDevices");

	mutex_lock_guard lock(deviceListMutex_);

	GetDeviceSuccess result;
	// Insert from the model device list to the result device list
  result.deviceList_.insert(result.deviceList_.end(), deviceList_.begin(), deviceList_.end());

  // Invoke the callback
  asyncReturnFunc(result);
}

// Add/remove device observer
void ModelImpl::removeDeviceObserver(DeviceObserverHandle observerHandle) {
	logDebug(TAG, "Entering ModelImpl::removeDeviceObserver");

	mutex_lock_guard lock(deviceObserverMapMutex_);
	for (auto iter = deviceObserverMap_.begin(); iter != deviceObserverMap_.end(); ++iter) {
		if (iter->first == observerHandle) {
			// Remove protocol
			deviceObserverMap_.erase(iter);
			logDebug(TAG, "ModelImpl::removeDeviceObserver, removed observer handle %d", (uint32_t)observerHandle);
			return;
		}
	}
}

void ModelImpl::setDeviceObserver(DeviceEventFunction& asyncEventFunction) {
	// TODO:  anyway to ensure that asyncEventFunction is valid?  nullptr check?
	logDebug(TAG, "Entering ModelImpl::setDeviceObserver");

	// Create a local scope here so that the mutex will be released at the end of the scope
	{
		mutex_lock_guard lock(deviceObserverMapMutex_);
		// Get a handle that is not already in use
		DeviceObserverHandle observerHandle;
		do {
			observerHandle = generateDeviceObserverHandle();
		} while (isDeviceObserverHandleInUse(observerHandle));

		// Add observer to the map.
		// Insert returns a pair with the second a bool set to true if a new element
		// was inserted or false if an equivalent key already existed.
		if (deviceObserverMap_.insert(DeviceObserverMap::value_type(observerHandle, asyncEventFunction)).second) {
			logDebug(TAG, "Entering ModelImpl::setDeviceObserver, observer added");
		}
		else {
			logDebug(TAG, "ModelImpl::setDeviceObserver:  Error inserting observer");
		}
	}

	// Notify the observers
	DeviceIdList deviceIdList = getDeviceIdList();
	notifyDeviceObservers(DeviceEvent::DeviceChange::DEVICE_CURRENT_COLLECTION, deviceIdList);
}

const Protocols::Protocol::Handle ModelImpl::registerProtocol(const Protocols::Protocol::SharedPtr& protocol) {
	logDebug(TAG, "Entering ModelImpl::registerProtocol");

	// Validate protocol
	assert(protocol);

	logDebug(TAG, "ModelImpl::registerProtocol, trying to register protocol %s", protocol->getName().c_str());

	// Iterate through map and ensure that the protocol is not already registered
	Protocols::Protocol::Handle handle = Protocols::Protocol::INVALID_HANDLE;
	bool protocolRegistered = false;

	mutex_lock_guard lock(protocolMapMutex_);

	for (auto iter = protocolMap_.begin(); iter != protocolMap_.end(); ++iter) {
		// TODO:  need to handle a protocol that no longer is in scope.
		// Remove from map and maybe notify apps that devices and/or connections are no longer present
		Protocols::Protocol::SharedPtr sharedProtocol = iter->second.lock();
		if (sharedProtocol) {
			if (sharedProtocol->getName() == protocol->getName()) {
				logDebug(TAG, "ModelImpl::registerProtocol, protocol %s already registered", protocol->getName().c_str());
				protocolRegistered = true;
				break;
			}
		}
	}

	// If the protocol isn't registered, create a new handle and add
	// the protocol to the map
	if (!protocolRegistered) {

		// Get a handle that is not already in use
		do {
			handle = generateProtocolHandle();
		} while (isHandleInUse(handle));

		// Add protocol to the map.
		// Insert returns a pair with the second a bool set to true if a new element
		// was inserted or false if an equivalent key already existed.
		Protocols::Protocol::WeakPtr weakProtocol = protocol;
		if (!protocolMap_.insert(ProtocolMap::value_type(handle, weakProtocol)).second) {
			logDebug(TAG, "ModelImpl::registerProtocol:  Error inserting protocol %s", protocol->getName().c_str());
			handle = Protocols::Protocol::INVALID_HANDLE;
		}
		else {
			logDebug(TAG, "ModelImpl::registerProtocol: registered protocol = %s", protocol->getName().c_str());
			protocol->setHandle(handle);
		}
	}

	return handle;
}

bool ModelImpl::unregisterProtocol(const Protocols::Protocol::Handle protocolHandle) {
	logDebug(TAG, "Entering ModelImpl::unregisterProtocol");

	bool handleFound = false;

	mutex_lock_guard lock(protocolMapMutex_);

	for (auto iter = protocolMap_.begin(); iter != protocolMap_.end(); ++iter) {
		if (iter->first == protocolHandle) {
			handleFound = true;
			// Remove protocol
			protocolMap_.erase(iter);
			logDebug(TAG, "ModelImpl::unregisterProtocol, unregistering protocol handle %d", (uint32_t)protocolHandle);
			break;
		}
	}

	return handleFound;
}

Device::SharedPtr ModelImpl::getDevice(const UUID_t& deviceId) {
	logDebug(TAG, "Entering ModelImpl::getDevice");

  // Get a device from the device list.  If the device already exists, return that
	// device.  If not, create a new device with the specified device id, add it to
	// the device list, and then return the device
	mutex_lock_guard lock(deviceListMutex_);
	if (deviceList_.empty())
  {
		logDebug(TAG, "ModelImpl::getDevice, device list empty, adding device");
  	return addDeviceToList(deviceId);
  }

	// Search list for the device based on device id
	for (auto iter = deviceList_.begin(); iter != deviceList_.end(); ++iter) {
		Device::SharedPtr device = *iter;
		if (device->getId() == deviceId) {
			logDebug(TAG, "ModelImpl::getDevice, device found in list");
			return device;
		}
	}

	// If here, device not found in list, so add and return
	logDebug(TAG, "ModelImpl::getDevice, device not found in list, adding device");
	return addDeviceToList(deviceId);
}

void ModelImpl::signalDeviceChange(const UUID_t& deviceId, DeviceEvent::DeviceChange deviceEvent) {

	uint32_t linkCount;
	DeviceIdList deviceIdList;
	bool sendNotification = false;

	// Create a local scope so that the deviceListMutex_ will be released at the end
	// of the local scope
	{
		mutex_lock_guard lock(deviceListMutex_);
		// Search list for the device based on device id
		for (auto iter = deviceList_.begin(); iter != deviceList_.end(); ++iter) {
			Device::SharedPtr device = *iter;
			if (device->getId() == deviceId) {
				linkCount = device->getLinkCount();
				logDebug(TAG, "ModelImpl::signalDeviceChange, device found, number of links = %d", linkCount);

				if (deviceEvent == DeviceEvent::DeviceChange::DEVICE_ADDED) {
					// If DEVICE_ADDED and only one link, then that indicates that this is the first
					// time that the device was found by a protocol, so a notification must be sent.
					// Multiple links in the device indicate that that the device was previously found
					// by other protocols, so a notification is not required
					if (linkCount == 1) {
						deviceIdList.push_back(deviceId);
						sendNotification = true;
					}
				}
				else if (deviceEvent == DeviceEvent::DeviceChange::DEVICE_REMOVED) {
					// If DEVICE_REMOVED and no links remain in the device, then the
					// device needs to be removed from the device list and
					// a device removal notification must be sent.
					if (linkCount == 0) {
						deviceIdList.push_back(deviceId);
						deviceList_.erase(iter);
						sendNotification = true;
					}
				}
				break;
			}
		}
	}

	// Since a device was added or removed, notify the device observers
	if (sendNotification) {
		notifyDeviceObservers(deviceEvent, deviceIdList);
	}
}

// ============================================================================
// Private methods
// ============================================================================
const Protocols::Protocol::Handle ModelImpl::generateProtocolHandle() {
  // Handle between 1 and (RAND_MAX -1)
  Protocols::Protocol::Handle handle = (Protocols::Protocol::Handle)(rand() % (RAND_MAX - 1) + 1);

  return handle;
}

bool ModelImpl::isHandleInUse(Protocols::Protocol::Handle handle) {
	bool handleFound = false;

	for (auto iter = protocolMap_.begin(); iter != protocolMap_.end(); ++iter) {
		if (iter->first == handle) {
			handleFound = true;
			break;
		}
	}
	return handleFound;
}

Device::SharedPtr ModelImpl::addDeviceToList(const UUID_t& deviceId) {
	logDebug(TAG, "Entering ModelImpl::addDeviceToList");

	deviceList_.push_back(std::make_shared<Device>(deviceId));
	return deviceList_.back();
}

DeviceObserverHandle ModelImpl::generateDeviceObserverHandle() {
  // Handle between 1 and (RAND_MAX -1)
	DeviceObserverHandle handle = (DeviceObserverHandle)(rand() % (RAND_MAX - 1) + 1);

  return handle;
}

bool ModelImpl::isDeviceObserverHandleInUse(DeviceObserverHandle observerHandle) {
	bool handleFound = false;

	for (auto iter = deviceObserverMap_.begin(); iter != deviceObserverMap_.end(); ++iter) {
		if (iter->first == observerHandle) {
			handleFound = true;
			break;
		}
	}
	return handleFound;
}

void ModelImpl::notifyDeviceObservers(DeviceEvent::DeviceChange event, const DeviceIdList& deviceIdList) {
	logDebug(TAG, "Entering ModelImpl::notifyDeviceObservers");

	// Iterate through the device id list that was passed to the notifier
	// and create a list of DeviceEvent::DeviceEventInfo objects.
	// Set the device id and the event in the DeviceEvent::DeviceEventInfo object
	std::list<DeviceEvent::DeviceEventInfo> deviceEventInfoList;
	DeviceEvent::DeviceEventInfo info;
	for (auto iter = deviceIdList.begin(); iter != deviceIdList.end(); ++iter) {
		info.deviceId = *iter;
		info.deviceChange = event;

		deviceEventInfoList.push_back(info);
	}

	mutex_lock_guard lock(deviceObserverMapMutex_);

	// Nothing to do if there are no observers
	if (deviceObserverMap_.empty()) {
		return;
	}

	DeviceObserverHandle observerHandle;
	DeviceEventFunction eventFunction;

	// For every observer in the observer map:
	for (auto iter = deviceObserverMap_.begin(); iter != deviceObserverMap_.end(); ++iter) {
		observerHandle = iter->first;
		eventFunction = iter->second;

		// 1. Create a DeviceEventSuccess object and set the observer handle
		DeviceEventSuccess deviceEventSuccess;
		deviceEventSuccess.deviceObserverHandle_ = observerHandle;

		// 2. Insert the deviceEventInfoList into the DeviceEventSuccess object list
		deviceEventSuccess.deviceInfoList_.insert(deviceEventSuccess.deviceInfoList_.end(), deviceEventInfoList.begin(), deviceEventInfoList.end());

		// 3. Invoke the observer's callback function
		eventFunction(deviceEventSuccess);
	}

}

ModelImpl::DeviceIdList ModelImpl::getDeviceIdList() {

	mutex_lock_guard lock(deviceListMutex_);

	DeviceIdList deviceIdList;

	// Search list for the device based on device id
	for (auto iter = deviceList_.begin(); iter != deviceList_.end(); ++iter) {
		Device::SharedPtr device = *iter;
		deviceIdList.push_back(device->getId());
	}

	return deviceIdList;
}

}
}
}
