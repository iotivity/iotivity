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

#ifndef MOCKPROTOCOL_H_
#define MOCKPROTOCOL_H_

// ============================================================================
// Includes
// ============================================================================
#include <memory>
#include <string>
#include <functional>
#include "Protocol.hpp"
#include "Core.h"

// ============================================================================
// Namespace
// ============================================================================
namespace Intel {
namespace CCFL {
namespace Protocols {

// ============================================================================
// Class
// ============================================================================
class MockProtocol : public Protocol
{
public:
	MockProtocol();
	virtual ~MockProtocol();
public:
	virtual void setModel(const std::shared_ptr<Intel::CCFL::API::Model>& model);
	virtual const Handle getHandle();
	virtual void setHandle(const Handle handle);
	virtual const std::string& getName();
	virtual void setName(const std::string& name);
	virtual void forceDeviceDiscovery();

private:
	std::string name_;
	Handle handle_;
	std::weak_ptr<API::Model> model_;

// Test interface
public:
	void testAddDevice(const UUID_t& deviceId, const std::string deviceName);
	void testRemoveDevice(const UUID_t& deviceId);
private:
	static const std::string MOCK_DEVICE_ID;

};


}
}
}

#endif /* MOCKPROTOCOL_H_ */
