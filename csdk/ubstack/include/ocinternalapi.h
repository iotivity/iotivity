//******************************************************************
///
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

#ifndef INTERNALUBAPI_H_
#define INTERNALUBAPI_H_

// ============================================================================
// Includes
// ============================================================================
#include <memory>
#include <functional>
#include <string>
#include <list>
#include <stdint.h>
#include "occore.h"

// ============================================================================
// Namespace
// ============================================================================
namespace oc {
namespace ub {


// ============================================================================
// Class
// ============================================================================
class OCDiscoverServicesResult {
	// ============================================================
	// Destructor
	// ============================================================
	public:
		virtual ~OCDiscoverServicesResult() {}

	// ============================================================
	// Public Method(s)
	// ============================================================
	public:
		virtual OCQueryResultType getResult() const = 0;
		virtual const std::list<std::string>& getServiceList() const = 0;
};

typedef std::function<void (const OCDiscoverServicesResult&)> OCDiscoverServicesFunction;

// ============================================================================
// Class
// ============================================================================
class OCModel : public std::enable_shared_from_this<OCModel> {
	// ============================================================
	// Type Definition(s)
	// ============================================================
	public:
		typedef std::shared_ptr<OCModel> SharedPtr;
		typedef std::weak_ptr<OCModel> WeakPtr;

	// ============================================================
	// Factory
	// ============================================================
	public:
		static SharedPtr createModel();

	// ============================================================
	// Destructor
	// ============================================================
	public:
		virtual ~OCModel() {}

	// ============================================================
	// Public Method(s)
	// ============================================================
	public:
		/**
		 * Start the OC Stack.
		 *
		 * @param ipAddr
		 *     IP Address of host device
		 * @param port
		 *     Port of host device
		 * @param mode
		 *     Host device is client, server, or client-server
		 *
		 * @return
		 *     true  - successfully started the OC Stack
		 *     false - error starting the OC Stack
		 */
		virtual bool start(const std::string ipAddr, int16_t port, OCStackMode mode) = 0;

		/**
		 * Get all services that have been discovered at time of call
		 *
		 * @param asyncReturnFunc - asynchronous callback function that is invoked
		 *                          by the OCModel when service discovery
		 *                          is complete.  The callback will include
		 *                          status and a list of all discovered services
		 */
		virtual void discoverServices(OCDiscoverServicesFunction& asyncReturnFunc) = 0;

		/**
		 * Add a service to the OCModel
		 *
		 * @param url
		 *     URL of the service
		 *
		 * @return
		 *     Total number of services in the OCModel
		 */
		virtual uint16_t addService(const std::string url) = 0;

};


}
}

#endif /* INTERNALUBAPI_H_ */
