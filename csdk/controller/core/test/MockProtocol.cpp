//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// ============================================================================
// Includes
// ============================================================================
#include "MockProtocol.h"
#include "SimpleLogger.h"
#include "InternalApi.h"
#include "Link.hpp"
#include "Description.hpp"
#include "Service.hpp"
#include "Characteristic.hpp"
#include <set>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
// ============================================================================
// Namespace
// ============================================================================
namespace Intel {
namespace CCFL {
namespace Protocols {

static const char TAG[] = "MockProtocol";

const std::string MockProtocol::MOCK_DEVICE_ID = "7c34ad16-ae8c-415b-94cc-d8053f4f9f8e";
static std::string characteristic0Value = "CHARACTERISTIC_0_VALUE";

// ============================================================================
// PropertyGetResult Implementation Class
// ============================================================================
class PropertyGetResultImpl : public API::PropertyGetResult {
public:
	PropertyGetResultImpl() {
		name_ = "";
		value_ = "";
	}
	virtual ~PropertyGetResultImpl() {}

public:
	virtual API::QueryResultType getResult() const {
		return API::QueryResultType::SUCCESS;
	}

	virtual const std::string& getName() const {
		return name_;
	}

	virtual const std::string& getValue() const {
		return value_;
	}

public:
	std::string name_;
	std::string value_;
};

// ============================================================================
// PropertySetResult Implementation Class
// ============================================================================
class PropertySetResultImpl : public API::PropertySetResult {
public:
	PropertySetResultImpl() {
		name_ = "";
		value_ = "";
	}
	virtual ~PropertySetResultImpl() {}

public:
	virtual API::QueryResultType getResult() const {
		return API::QueryResultType::SUCCESS;
	}

	virtual const std::string& getName() const {
		return name_;
	}

	virtual const std::string& getValue() const {
		return value_;
	}

public:
	std::string name_;
	std::string value_;

};

// ============================================================================
// Characteristic Implementation Class
// ============================================================================
class CharacteristicImpl : public API::Characteristic {
public:
	CharacteristicImpl() {
		name_ = "";
		isReadable_ = true;
		isWritable_ = true;
		isConstant_ = false;
	}
	virtual ~CharacteristicImpl() {}

public:
	virtual const std::string& getName() const {
		return name_;
	}
	virtual bool isReadable() const {
		return isReadable_;
	}
	virtual bool isWritable() const {
		return isWritable_;
	}
	virtual bool isConstant() const {
		return isConstant_;
	}

public:
	std::string name_;
	bool isReadable_;
	bool isWritable_;
	bool isConstant_;
};

// ============================================================================
// Service Implementation Class
// ============================================================================
class ServiceImpl : public API::Service {
public:
	ServiceImpl() {
		name_ = "";
	}

	virtual ~ServiceImpl() {}

public:

	virtual const std::string& getName() const {
		return name_;
	}

	virtual const std::set<API::Characteristic::SharedPtr>& getCharacteristics() const {
		return characteristicSet_;
	}
public:
	std::string name_;
	std::set<API::Characteristic::SharedPtr> characteristicSet_;
};
// ============================================================================
// DescriptionGetResult Implementation Class
// ============================================================================
class DescriptionGetResultImpl : public API::DescriptionGetResult {
public:
	virtual ~DescriptionGetResultImpl() {}

public:
	virtual API::QueryResultType getResult() const {
		return API::QueryResultType::SUCCESS;
	};

	virtual const std::set<API::Service::SharedPtr>& getServices() const {
		return serviceSet_;
	}

public:
	std::set<API::Service::SharedPtr> serviceSet_;
};

// ============================================================================
// RegisteredSetPropFunction
// ============================================================================
void setPropertyFunction(const std::string& propertyName, const std::string& propertyValue, const API::PropertySetFunction& asyncReturnFunc) {
	logDebug(TAG, "Entering MockProtocol::setPropertyFunction");

	PropertySetResultImpl result;

	if (propertyName == "CHARACTERISTIC_0") {
		// CHARACTERISTIC_0 is read/write, non-constant, so allow it to be set to new value
		result.name_ = "CHARACTERISTIC_0";
		characteristic0Value = propertyValue;
		result.value_ = characteristic0Value;
	}
	else if (propertyName == "CHARACTERISTIC_1") {
		// CHARACTERISTIC_1 is constant, so ignore new value
		result.name_ = "CHARACTERISTIC_1";
		result.value_ = "CHARACTERISTIC_1_VALUE";
	}

	asyncReturnFunc(result);
}
// ============================================================================
// RegisteredGetPropFunction
// ============================================================================
void getPropertyFunction(const std::string& propertyName, const API::PropertyGetFunction& asyncReturnFunc) {
	logDebug(TAG, "Entering MockProtocol::getPropertyFunction");

	PropertyGetResultImpl result;

	if (propertyName == "CHARACTERISTIC_0") {
		result.name_ = "CHARACTERISTIC_0";
		result.value_ = characteristic0Value;
	}
	else if (propertyName == "CHARACTERISTIC_1") {
		result.name_ = "CHARACTERISTIC_1";
		result.value_ = "CHARACTERISTIC_1_VALUE";
	}

	asyncReturnFunc(result);
}

// ============================================================================
// RegisteredGetDescriptionFunction
// ============================================================================
void getDescriptionFunction(const API::DescriptionGetFunction& asyncReturnFunc) {
	logDebug(TAG, "Entering MockProtocol::getDescriptionFunction");

	DescriptionGetResultImpl result;

	std::shared_ptr<ServiceImpl> service = std::make_shared<ServiceImpl>();
	if (service) {
		service->name_ = "SERVICE_A";
		std::shared_ptr<CharacteristicImpl> characteristic0 = std::make_shared<CharacteristicImpl>();
		if (characteristic0) {
			characteristic0->name_ = "CHARACTERISTIC_0";
			service->characteristicSet_.insert(characteristic0);
		}
		std::shared_ptr<CharacteristicImpl> characteristic1 = std::make_shared<CharacteristicImpl>();
		if (characteristic1) {
			characteristic1->name_ = "CHARACTERISTIC_1";
			characteristic1->isReadable_ = true;
			characteristic1->isWritable_ = false;
			characteristic1->isConstant_ = true;

			service->characteristicSet_.insert(characteristic1);
		}

		result.serviceSet_.insert(service);
	}


  // Invoke the callback
  asyncReturnFunc(result);
}

// ============================================================================
// Class
// ============================================================================
MockProtocol::MockProtocol()
{
	logDebug(TAG, "Entering MockProtocol::MockProtocol");

	name_ = "Mock Protocol";
	handle_ = Protocol::INVALID_HANDLE;
}

MockProtocol::~MockProtocol()
{
	logDebug(TAG, "Entering MockProtocol::~MockProtocol");
}

void MockProtocol::setModel(const std::shared_ptr<Intel::CCFL::API::Model>& model) {
	logDebug(TAG, "Entering MockProtocol::setModel");
	model_ = model;
}

const Intel::CCFL::Protocols::Protocol::Handle MockProtocol::getHandle() {
	logDebug(TAG, "Entering MockProtocol::getHandle");
	return 0;
}

void MockProtocol::setHandle(const Handle handle) {
	logDebug(TAG, "Entering MockProtocol::setHandle");
	handle_ = handle;
}

const std::string& MockProtocol::getName() {
//	logDebug(TAG, "Entering MockProtocol::getName");
	return name_;
}

void MockProtocol::setName(const std::string& name)
{
	logDebug(TAG, "Entering MockProtocol::setName, name = %s", name.c_str());
	name_ = name;
}

void MockProtocol::forceDeviceDiscovery() {
	logDebug(TAG, "Entering MockProtocol::forceDeviceDiscovery");
}

// Test interface
void MockProtocol::testAddDevice(const UUID_t& deviceId, const std::string deviceName) {
	logDebug(TAG, "Entering MockProtocol::testAddDevice, device name = %s, device UUID = %s", deviceName.c_str(), to_string(deviceId).c_str());

	std::shared_ptr<API::Model> sharedModel = model_.lock();
	if (sharedModel) {
		API::Device::SharedPtr device = sharedModel->getDevice(deviceId);
		if (device) {
			device->setName(deviceName);
			// Add a link to the device
			API::Link::SharedPtr link = std::make_shared<API::Link>();

			// Register property functions for the link
			API::Link::RegisteredGetDescriptionFunction regGetDescriptionFunction = getDescriptionFunction;
			link->registerGetDescriptionFunction(regGetDescriptionFunction);
			API::Link::RegisteredGetPropFunction regGetPropFunction = getPropertyFunction;
			link->registerGetPropertyFunction(regGetPropFunction);
			API::Link::RegisteredSetPropFunction regSetPropFunction = setPropertyFunction;
			link->registerSetPropertyFunction(regSetPropFunction);

			// Add the link to the device and signal the model that the device changed
			device->addLink(link);
			sharedModel->signalDeviceChange(deviceId, API::DeviceEvent::DeviceChange::DEVICE_ADDED);
		}
	}
}

void MockProtocol::testRemoveDevice(const UUID_t& deviceId) {
	logDebug(TAG, "Entering MockProtocol::testRemoveDevice, device UUID = %s", to_string(deviceId).c_str());

	std::shared_ptr<API::Model> sharedModel = model_.lock();
	if (sharedModel) {
		API::Device::SharedPtr device = sharedModel->getDevice(deviceId);
		if (device) {
			// Remove the link
			uint32_t linkCnt = device->getLinkCount();
			logDebug(TAG, "MockProtocol::testRemoveDevice, link count = %d", linkCnt);
			if (linkCnt > 0) {
				API::Device::LinkList linkList = device->getLinks();
				API::Link::SharedPtr link = linkList.front();
				if (link) {
					logDebug(TAG, "MockProtocol::testRemoveDevice, removing link");
					if (device->removeLink(link)) {
						logDebug(TAG, "MockProtocol::testRemoveDevice, link removed, signaling device change");
						sharedModel->signalDeviceChange(deviceId, API::DeviceEvent::DeviceChange::DEVICE_REMOVED);
					}
				}
			}
		}
	}
}

}
}
}
