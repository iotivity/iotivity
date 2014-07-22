//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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
