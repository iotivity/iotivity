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

#ifndef INTERNALAPI_H_
#define INTERNALAPI_H_

// ============================================================================
// Includes
// ============================================================================
#include <memory>
#include <functional>
#include <string>
#include <list>
#include "Core.h"
#include "Description.hpp"
#include "Characteristic.hpp"
#include "Protocol.hpp"
#include "Device.hpp"

// ============================================================================
// Namespace
// ============================================================================
namespace Intel {
namespace CCFL {
namespace API {


// ============================================================================
//                              Device Class(es)
// ============================================================================

#if 0
// ============================================================================
// Class
// ============================================================================
class DeviceIdsGetResult {
	// ============================================================
	// Destructor
	// ============================================================
	public:
		virtual ~DeviceIdsGetResult() {}

	// ============================================================
	// Public Method(s)
	// ============================================================
	public:
		virtual QueryResultType getResult() const = 0;
		virtual const std::set<UUID_t>& getDeviceIds() const = 0;
};

typedef std::function<void (const DeviceIdsGetResult&)> DeviceIdsGetFunction;

// ============================================================================
// Class
// ============================================================================
class DeviceGetInfoResult {
	// ============================================================
	// Destructor
	// ============================================================
	public:
		virtual ~DeviceGetInfoResult() {}

	// ============================================================
	// Public Method(s)
	// ============================================================
	public:
		virtual QueryResultType getResult() const = 0;
		virtual const std::string& getName() const = 0;
		virtual const Description::SharedPtr& getDescription() const = 0;
		virtual bool isLocal() = 0;
};

typedef std::function<void (const DeviceGetInfoResult&)> DeviceGetInfoFunction;
#endif

// ============================================================================
// Class
// ============================================================================
class GetDevicesResult {
	// ============================================================
	// Destructor
	// ============================================================
	public:
		virtual ~GetDevicesResult() {}

	// ============================================================
	// Public Method(s)
	// ============================================================
	public:
		virtual QueryResultType getResult() const = 0;
		virtual const std::list<Device::SharedPtr>& getDeviceList() const = 0;
};

typedef std::function<void (const GetDevicesResult&)> GetDevicesFunction;


// ============================================================================
// Class
// ============================================================================
typedef uint32_t DeviceObserverHandle;
const DeviceObserverHandle INVALID_DEVICE_OBSERVER_HANDLE = (DeviceObserverHandle)0;

class DeviceEvent {
public:
	enum DeviceChange {
		DEVICE_CURRENT_COLLECTION,
		DEVICE_ADDED,
		DEVICE_REMOVED
	};

	struct DeviceEventInfo {
		UUID_t       deviceId;
		DeviceChange deviceChange;
	};
	// ============================================================
	// Destructor
	// ============================================================
	public:
		virtual ~DeviceEvent() {}

	// ============================================================
	// Public Method(s)
	// ============================================================
	public:
		virtual QueryResultType getResult() const = 0;
		virtual DeviceObserverHandle getObserverHandle() const = 0;
		virtual const std::list<DeviceEventInfo>& getDeviceIdList() const = 0;
};

typedef std::function<void (const DeviceEvent&)> DeviceEventFunction;


// ============================================================================
// Class
// ============================================================================
class Model : public std::enable_shared_from_this<Model> {
	// ============================================================
	// Type Definition(s)
	// ============================================================
	public:
		typedef std::shared_ptr<Model> SharedPtr;
		typedef std::weak_ptr<Model> WeakPtr;

	// ============================================================
	// Factory
	// ============================================================
	public:
		static SharedPtr createModel();

	// ============================================================
	// Destructor
	// ============================================================
	public:
		virtual ~Model() {}

	// ============================================================
	// Public Method(s)
	// ============================================================
	public:
		virtual void getDevices(GetDevicesFunction& asyncReturnFunc) = 0;

		// Add/remove device observer
		virtual void removeDeviceObserver(DeviceObserverHandle observerHandle) = 0;
		virtual void setDeviceObserver(DeviceEventFunction& asyncEventFunction) = 0;

		// Register/unregister a protocol
		virtual const Protocols::Protocol::Handle registerProtocol(const Protocols::Protocol::SharedPtr& protocol) = 0;
		virtual bool unregisterProtocol(const Protocols::Protocol::Handle protocolHandle) = 0;
    virtual Device::SharedPtr getDevice(const UUID_t& deviceId) = 0;
    virtual void signalDeviceChange(const UUID_t& deviceId, DeviceEvent::DeviceChange deviceEvent) = 0;

};


}
}
}

#endif /* INTERNALAPI_H_ */
