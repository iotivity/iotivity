//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//#include <iostream>
#include <string.h>
#include <stdint.h>

#ifdef __linux__
	#include <thread>
	#include <pthread.h>
	#include <chrono>
#endif


#include <functional>
#include "InternalApi.h"
#include "SimpleLogger.h"
#include "MockProtocol.h"
#include "Core.h"
#include "Description.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

const std::string MOCK_DEVICE_ID0 = "7c34ad16-ae8c-415b-94cc-d8053f4f9f8e";
const std::string MOCK_DEVICE_ID1 = "646344d0-271c-49dd-8b13-3a7bec1237bd";
const std::string MOCK_DEVICE_ID2 = "74c406ca-1cfb-4106-a82f-6123e3fe3555";

static const char TAG[] = "CCFL-CoreTest.cpp";

UUID_t toUuid(const std::string& uuidStr) {
	boost::uuids::string_generator gen;
	return gen(uuidStr);
}

using namespace std;

void setPropertyAsync(const Intel::CCFL::API::PropertySetResult& result) {
	Intel::CCFL::logDebug(TAG, "\t********entering main::setPropertyAsync********");
	if (result.getResult() == Intel::CCFL::API::QueryResultType::SUCCESS) {
		Intel::CCFL::logDebug(TAG, "\t QueryResultType::SUCCESS");
		Intel::CCFL::logDebug(TAG, "\t property:");
		Intel::CCFL::logDebug(TAG, "\t\t name  = %s", result.getName().c_str());
		Intel::CCFL::logDebug(TAG, "\t\t value = %s", result.getValue().c_str());
	 }
	Intel::CCFL::logDebug(TAG, "\t********leaving main::setPropertyAsync********");
}

void getPropertyAsync(const Intel::CCFL::API::PropertyGetResult& result) {
	Intel::CCFL::logDebug(TAG, "\t********entering main::getPropertyAsync********");
	if (result.getResult() == Intel::CCFL::API::QueryResultType::SUCCESS) {
		Intel::CCFL::logDebug(TAG, "\t QueryResultType::SUCCESS");
    Intel::CCFL::logDebug(TAG, "\t property:");
		Intel::CCFL::logDebug(TAG, "\t\t name  = %s", result.getName().c_str());
		Intel::CCFL::logDebug(TAG, "\t\t value = %s", result.getValue().c_str());
	}
	Intel::CCFL::logDebug(TAG, "\t********leaving main::getPropertyAsync********");
}

void getDescriptionAsync(const Intel::CCFL::API::DescriptionGetResult& result) {
	Intel::CCFL::logDebug(TAG, "\t********entering main::getDescriptionAsync********");

  if (result.getResult() == Intel::CCFL::API::QueryResultType::SUCCESS) {
  	Intel::CCFL::logDebug(TAG, "\t QueryResultType::SUCCESS");
  	std::set<Intel::CCFL::API::Service::SharedPtr> serviceSet = result.getServices();

  	for (auto iter = serviceSet.begin(); iter != serviceSet.end(); ++iter) {
  		Intel::CCFL::API::Service::WeakPtr serviceWeak = *iter;
  		Intel::CCFL::API::Service::SharedPtr service = serviceWeak.lock();
  		if (service) {
  			Intel::CCFL::logDebug(TAG, "\t service = %s", service->getName().c_str());

  			std::set<Intel::CCFL::API::Characteristic::SharedPtr> characteristicSet = service->getCharacteristics();
  			for (auto iter = characteristicSet.begin(); iter != characteristicSet.end(); ++iter) {
  				Intel::CCFL::API::Characteristic::WeakPtr characteristicWeak = *iter;
  				Intel::CCFL::API::Characteristic::SharedPtr characteristic = characteristicWeak.lock();
  				if (characteristic) {
  					Intel::CCFL::logDebug(TAG, "\t\t characteristic = %s", characteristic->getName().c_str());
  					Intel::CCFL::logDebug(TAG, "\t\t readable = %d", characteristic->isReadable());
  					Intel::CCFL::logDebug(TAG, "\t\t writable = %d", characteristic->isWritable());
  					Intel::CCFL::logDebug(TAG, "\t\t constant = %d", characteristic->isConstant());
  				}
  			}
  		}
  	}
  }
  Intel::CCFL::logDebug(TAG, "\t********leaving main::getDescriptionAsync********");
}

void getDevicesAsync(const Intel::CCFL::API::GetDevicesResult& result) {
	Intel::CCFL::logDebug(TAG, "Entering main::getDevicesAsync");

  if (result.getResult() == Intel::CCFL::API::QueryResultType::SUCCESS) {
  	Intel::CCFL::logDebug(TAG, "main::getDevicesAsync, QueryResultType::SUCCESS");
  	std::list<Intel::CCFL::API::Device::SharedPtr> deviceList = result.getDeviceList();
  	// Search list for the device based on device id
  	if (deviceList.size() > 0) {
			for (auto iter = deviceList.begin(); iter != deviceList.end(); ++iter) {
				Intel::CCFL::API::Device::WeakPtr deviceWeak = *iter;
				Intel::CCFL::API::Device::SharedPtr device = deviceWeak.lock();
				if (device) {
					Intel::CCFL::logDebug(TAG, "\tDevice:");
					Intel::CCFL::logDebug(TAG, "\t  name = %s", device->getName().c_str());
					Intel::CCFL::logDebug(TAG, "\t  UUID = %s", to_string(device->getId()).c_str());

					// Test characteristic stuff
					if (device->getName() == "MockDevice0") {
						if (device->getLinkCount() > 0) {
							Intel::CCFL::API::Device::LinkList linkList = device->getLinks();
							Intel::CCFL::API::Link::WeakPtr linkWeak = linkList.front();
							Intel::CCFL::API::Link::SharedPtr linkShared = linkWeak.lock();
							if (linkShared) {
								// Test set/get properties
								Intel::CCFL::API::DescriptionGetFunction descriptionGetFunction = getDescriptionAsync;
								linkShared->getDescription(descriptionGetFunction);
								Intel::CCFL::API::PropertyGetFunction propertyGetFunction = getPropertyAsync;
								linkShared->getProperty("CHARACTERISTIC_0", propertyGetFunction);
								linkShared->getProperty("CHARACTERISTIC_1", propertyGetFunction);

								Intel::CCFL::API::PropertySetFunction propertySetFunction = setPropertyAsync;
								linkShared->setProperty("CHARACTERISTIC_0", "new_CHARACTERISTIC_0_Value", propertySetFunction);
								linkShared->setProperty("CHARACTERISTIC_1", "new_CHARACTERISTIC_1_Value", propertySetFunction);

								linkShared->getProperty("CHARACTERISTIC_0", propertyGetFunction);
								linkShared->getProperty("CHARACTERISTIC_1", propertyGetFunction);
							}
						}
					}
				}
			}
  	}
  	else {
  		Intel::CCFL::logDebug(TAG, "\t No devices found");
  	}
  }
}

#ifdef __ANDROID__
	int android_jni_entry() {
#elif defined __linux__
		int main() {
#endif

	Intel::CCFL::logDebug(TAG, "Starting CCFL Core Test");

	// Create MockProtocols.  Two are created to make sure that multiples are registered and unregistered correctly
	std::shared_ptr<Intel::CCFL::Protocols::MockProtocol> mockProtocol = std::make_shared<Intel::CCFL::Protocols::MockProtocol>();
	mockProtocol->setName("MockProtocol");

	// Get a model from the CCFL API so that we can access the API interface
	Intel::CCFL::API::Model::WeakPtr modelWeak = Intel::CCFL::API::Model::createModel();
	Intel::CCFL::API::Model::SharedPtr modelShared = modelWeak.lock();
	if (modelShared) {

		mockProtocol->setModel(modelShared);

		// Register the MockProtocol
		Intel::CCFL::Protocols::Protocol::Handle handle;
		handle = modelShared->registerProtocol(mockProtocol);
		Intel::CCFL::logDebug(TAG, "Protocol %s handle = %d", mockProtocol->getName().c_str(), (uint32_t)handle);

		Intel::CCFL::API::GetDevicesFunction deviceFunc = getDevicesAsync;
		modelShared->getDevices(deviceFunc);

		mockProtocol->testAddDevice(toUuid(MOCK_DEVICE_ID0), "MockDevice0");
		modelShared->getDevices(deviceFunc);

		Intel::CCFL::API::DeviceEventFunction deviceEventLambda = [](const Intel::CCFL::API::DeviceEvent& result)->void {
			Intel::CCFL::logDebug(TAG, "main::deviceEventAsyncLambda");
			if (result.getResult() == Intel::CCFL::API::QueryResultType::SUCCESS) {
				Intel::CCFL::API::DeviceObserverHandle handle = result.getObserverHandle();
				Intel::CCFL::logDebug(TAG, "main::deviceEventAsyncLambda, observer handle = %ld", (uint32_t)handle);
				std::list<Intel::CCFL::API::DeviceEvent::DeviceEventInfo> infoList = result.getDeviceIdList();
				for (auto iter = infoList.begin(); iter != infoList.end(); ++iter) {
					Intel::CCFL::API::DeviceEvent::DeviceEventInfo info = *iter;
					Intel::CCFL::logDebug(TAG, "\t Device UUID = %s", to_string(info.deviceId).c_str());
					switch (info.deviceChange) {
						case Intel::CCFL::API::DeviceEvent::DeviceChange::DEVICE_CURRENT_COLLECTION:
							Intel::CCFL::logDebug(TAG, "\t Event = DEVICE_CURRENT_COLLECTION");
							break;
						case Intel::CCFL::API::DeviceEvent::DeviceChange::DEVICE_ADDED:
							Intel::CCFL::logDebug(TAG, "\t Event = DEVICE_ADDED");
							break;
						case Intel::CCFL::API::DeviceEvent::DeviceChange::DEVICE_REMOVED:
							Intel::CCFL::logDebug(TAG, "\t Event = DEVICE_REMOVED");
							break;
						default:
							Intel::CCFL::logDebug(TAG, "\t Event = unknown");
							break;
					}
				}
			}
		};
		modelShared->setDeviceObserver(deviceEventLambda);

		mockProtocol->testAddDevice(toUuid(MOCK_DEVICE_ID1), "MockDevice1");
		modelShared->getDevices(deviceFunc);
		mockProtocol->testAddDevice(toUuid(MOCK_DEVICE_ID2), "MockDevice2");
		modelShared->getDevices(deviceFunc);
		mockProtocol->testRemoveDevice(toUuid(MOCK_DEVICE_ID0));
		modelShared->getDevices(deviceFunc);

		Intel::CCFL::logDebug(TAG, "Sleep for 5 sec");
		std::this_thread::sleep_for(std::chrono::seconds(5));
		modelShared->unregisterProtocol(handle);
		Intel::CCFL::logDebug(TAG, "Sleep for 5 sec");
	}


	Intel::CCFL::logDebug(TAG, "Exiting CCFL Core Test");
	return 0;
}
