//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

// ============================================================================
// Includes
// ============================================================================
#include <string>
#include <set>
#include <memory>
#include <stdint.h>
#include "Device.hpp"

// ============================================================================
// Forward declaration
// ============================================================================
namespace Intel {
	namespace CCFL {
		namespace API {
			class Model;
		}
	}
}

// ============================================================================
// Namespace
// ============================================================================
namespace Intel {
namespace CCFL {
namespace Protocols {

class Protocol
{
public:
	typedef std::shared_ptr<Protocol> SharedPtr;
	typedef std::weak_ptr<Protocol> WeakPtr;
	typedef uint32_t Handle;

	static const Handle INVALID_HANDLE = (Handle)0;
public:
	virtual ~Protocol() {};

public:
	virtual void setModel(const std::shared_ptr<Intel::CCFL::API::Model>& model) = 0;
	virtual const Handle getHandle() = 0;
	virtual void setHandle(const Handle handle) = 0;
	virtual const std::string& getName() = 0;
	virtual void forceDeviceDiscovery() = 0;
};

}
}
}

#endif /* PROTOCOL_H_ */
