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


#ifndef OCMODELIMPL_H_
#define OCMODELIMPL_H_

// ============================================================================
// Includes
// ============================================================================
#include <memory>
#include <map>
#include <stdint.h>

#include "ocinternalapi.h"

extern "C" {
    #include "ocstack.h"
}
// ============================================================================
// Namespace
// ============================================================================
namespace oc {
namespace ub {

class OCModelImpl : public OCModel {
public:
	typedef std::shared_ptr<OCModelImpl> SharedPtr;
	typedef std::weak_ptr<OCModelImpl> WeakPtr;

public:
	/**
	 * Factory pattern to create a OCModel singleton
	 * @return Shared pointer to the OCModel
	 */
	static SharedPtr createModel();

public:
	/**
	 * Constructor
	 */
	OCModelImpl();

	/**
	 * Destructor
	 */
	virtual ~OCModelImpl();

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
    virtual bool start(const std::string ipAddr, int16_t port, OCStackMode mode);

    /**
     * Get all services that have been discovered at time of call
     *
     * @param asyncReturnFunc - asynchronous callback function that is invoked
     *                          by the OCModelImpl when service discovery
     *                          is complete.  The callback will include
     *                          status and a list of all discovered services
     */
	virtual void discoverServices(OCDiscoverServicesFunction& asyncReturnFunc);

    /**
     * Add a service to the OCModelImpl
     *
     * @param url - URL of the service
     * @return Total number of services in the OCModelImpl
     */
	virtual uint16_t addService(const std::string url);

private:
    typedef std::list<std::string> ServiceList;

private:
    /**
     * Internal methods that invokes the client callback (if it was previously set via discoverServices)
     * Called when a new service is discovered.
     */
    virtual void invokeDiscoverCallback();

    /**
     * Asynchronous callback friend function invoked by the OC Stack upon service discovery
     *
     * @param result         - OC stack result
     * @param representation - handle to the representation of the resource
     */
    friend void asyncDoResourcesCallback(OCStackResult result, OCRepresentationHandle representation);
private:
    ServiceList serviceList;
    mutex serviceListMutex;
    OCDiscoverServicesFunction discoverCallback;
};

}
}
#endif /* OCMODELIMPL_H_ */
