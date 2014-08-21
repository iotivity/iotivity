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
#include "ocmodelimpl.h"
extern "C" {
    #include "logger.h"
}

#include <stdlib.h>
#include <time.h>
#include <cstdlib>

// ============================================================================
// Namespace
// ============================================================================
namespace oc {
namespace ub {

static const char TAG[] = "OCModelImpl";

// Global variable used by friend async callback function to access the OCModelImpl instance
OCModelImpl *pModelImpl = 0;

/**
 * Asynchronous callback friend function invoked by the OC Stack upon service discovery
 *
 * @param result         - OC stack result
 * @param representation - handle to the representation of the resource
 */
void asyncDoResourcesCallback(OCStackResult result, OCRepresentationHandle representation) {
    OC_LOG(INFO, TAG, "Entering asyncDoResourcesCallback");

    if (representation && (result == OC_STACK_OK)) {
        OCResource *resource = (OCResource *)representation;
        OC_LOG_V(INFO, TAG, "URI = %s", resource->uri);

        // If the pointer is valid, make a call to the OCModelImpl instance method
        if (pModelImpl) {
            pModelImpl->addService(resource->uri);
        }
    }
}

// ============================================================================
// OCDiscoverServicesResult Implementation Class
// ============================================================================
class OCDiscoverServicesSuccess : public OCDiscoverServicesResult {
public:
	virtual ~OCDiscoverServicesSuccess() {}

// ============================================================
// Public Method(s)
// ============================================================
public:
	virtual OCQueryResultType getResult() const {
		return SUCCESS;
	}

	virtual const std::list<std::string>& getServiceList() const {
		return serviceList;
	}

public:
	std::list<std::string> serviceList;
};


// ============================================================================
// Model Factory
// ============================================================================
OCModel::SharedPtr OCModel::createModel() {
    OC_LOG(INFO, TAG, "Entering OCModel::createModel");
	return OCModelImpl::createModel();
}

OCModelImpl::SharedPtr OCModelImpl::createModel() {
	// Get singleton of ModelImpl.
    OC_LOG(INFO, TAG, "Entering OCModelImpl::createModel");
	static SharedPtr instance = std::make_shared<OCModelImpl>();

	return instance;
}
// ============================================================================
// Constructors & Destructors
// ============================================================================
OCModelImpl::OCModelImpl() {
    OC_LOG(INFO, TAG, "Entering OCModel::OCModelImpl");

    pModelImpl = this;
    discoverCallback = 0;
}

OCModelImpl::~OCModelImpl() {
    OC_LOG(INFO, TAG, "Entering OCModel::~OCModelImpl");
    // Call OCStack layer
    if (OCStop() != OC_STACK_OK) {
        OC_LOG(ERROR, TAG, "Stack shutdown error");
    }

    discoverCallback = 0;
	serviceList.clear();
}

// ============================================================================
// Public methods
// ============================================================================
/**
 * Start the OC Stack.
 *
 * @param ipAddr
 *     IP Address of host device
 * @param port
 *     Port of host device*
 * @param mode
 *     Host device is client, server, or client-server
 *
 * @return
 *     true  - successfully started the OC Stack
 *     false - error starting the OC Stack
 */
bool OCModelImpl::start(const std::string ipAddr, int16_t port, OCStackMode mode) {
    OC_LOG_V(INFO, TAG, "Entering OCModelImpl::start, IP = %s, port = %d", ipAddr.c_str(), (int)port);

    // Call OCStack layer - initialize stack
    if (OCInit(ipAddr.c_str(), port, (OCMode)mode) != OC_STACK_OK) {
        OC_LOG(ERROR, TAG, "Stack initialization error");
        return false;
    }

    // Call OCStack layer - enable device discovery and provide a callback
    if (OCDoResource(OC_REST_GET, OC_EXPLICIT_DEVICE_DISCOVERY_URI, 0, 0, asyncDoResourcesCallback) != OC_STACK_OK) {
        OC_LOG(ERROR, TAG, "Stack discovery error");
        return false;
    }
    return true;
}

/**
 * Get all services that have been discovered at time of call
 *
 * @param asyncReturnFunc - asynchronous callback function that is invoked
 *                          by the OCModelImpl when service discovery
 *                          is complete.  The callback will include
 *                          status and a list of all discovered services
 */
void OCModelImpl::discoverServices(OCDiscoverServicesFunction& asyncReturnFunc) {
    OC_LOG(INFO, TAG, "Entering OCModelImpl::discoverServices");

	mutex_lock_guard lock(serviceListMutex);

	// Save the callback
	discoverCallback = asyncReturnFunc;

	// Invoke the callback immediately
	invokeDiscoverCallback();
}

/**
 * Add a service to the OCModelImpl
 *
 * @param url - URL of the service
 * @return Total number of services in the OCModelImpl
 */
uint16_t OCModelImpl::addService(const std::string url) {
    OC_LOG_V(INFO, TAG, "Entering OCModelImpl::addService, service = %s", url.c_str());
    serviceList.push_back(url);

    invokeDiscoverCallback();
    return (uint16_t)serviceList.size();
}

/**
 * Internal methods that invokes the client callback (if it was previously set via discoverServices)
 * Called when a new service is discovered.
 */
void OCModelImpl::invokeDiscoverCallback() {
    OC_LOG(INFO, TAG, "Entering OCModelImpl::invokeDiscoverCallback");
    // Only invoke the callback if it has been set and and at least one service has been added
    // to the service list
    if (discoverCallback && (serviceList.size() > 0)) {
        OC_LOG(INFO, TAG, "Invoking callback");

        OCDiscoverServicesSuccess result;
        // Insert from the model device list to the result device list
        result.serviceList.insert(result.serviceList.end(), serviceList.begin(), serviceList.end());

        // Invoke the callback
        discoverCallback(result);
    }
}

}
}
