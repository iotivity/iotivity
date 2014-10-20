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

#ifndef SERVICE_H_
#define SERVICE_H_

// ============================================================================
// Includes
// ============================================================================
#include <string>
#include <memory>
#include <set>
#include "Characteristic.hpp"

// ============================================================================
// Namespace
// ============================================================================
namespace Intel {
namespace CCFL {
namespace API {

class Service {
	// ============================================================
	// Type Definition(s)
	// ============================================================
	public:
		typedef std::shared_ptr<Service> SharedPtr;
		typedef std::weak_ptr<Service> WeakPtr;

	// ============================================================
	// Destructor
	// ============================================================
	public:
		virtual ~Service() {}

	// ============================================================
	// Public Method(s)
	// ============================================================
	public:

		virtual const std::string& getName() const = 0;
		virtual const std::set<Characteristic::SharedPtr>& getCharacteristics() const = 0;
};

}
}
}


#endif /* SERVICE_H_ */
