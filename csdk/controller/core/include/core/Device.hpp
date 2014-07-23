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

#ifndef DEVICE_H_
#define DEVICE_H_

// ============================================================================
// Includes
// ============================================================================
#include <string>
#include <list>
#include <cassert>
#include <stdint.h>

#include "Core.h"
#include "UUIDLess.hpp"
#include "Link.hpp"
#include "SimpleLogger.h"

// ============================================================================
// Namespace
// ============================================================================
namespace Intel {
namespace CCFL {
namespace API {

static const char DEVICE_TAG[] = "Device";

class Device {
	// ============================================================
	// Type Definition(s)
	// ============================================================
public:
		typedef std::shared_ptr<Device> SharedPtr;
		typedef std::weak_ptr<Device> WeakPtr;
		typedef std::list<Link::SharedPtr> LinkList;

public:
	Device(const UUID_t& deviceId) {
		deviceId_ = deviceId;
		name_ = "";
	}
	virtual ~Device() {
		linkList_.clear();
	};

public:
	const UUID_t& getId() {
		mutex_lock_guard lock(deviceMutex_);
		return deviceId_;
	};

	const std::string& getName() {
		mutex_lock_guard lock(deviceMutex_);
		return name_;
	};

	void setName(const std::string& name) {
		mutex_lock_guard lock(deviceMutex_);
		name_ = name;
	};

	void addLink(const Link::SharedPtr& link) {
		logDebug(DEVICE_TAG, "Entering Link::addLink");
		assert(link);
		mutex_lock_guard lock(deviceMutex_);

		// If list is empty, go ahead and add
		if (linkList_.empty())
	  {
			logDebug(DEVICE_TAG, "Link::addLink, linkList empty, adding");
			linkList_.push_back(link);
			return;
	  }

		// Search list for the link. If already in list, return
		for (auto iter = linkList_.begin(); iter != linkList_.end(); ++iter) {
			if (*iter == link) {
				logDebug(DEVICE_TAG, "Link::addLink, link already in linkList");
				return;
			}
		}

		// Not already in list, so add
		logDebug(DEVICE_TAG, "Link::addLink, adding link");
		linkList_.push_back(link);
	}

	bool removeLink(const Link::SharedPtr& link) {
		logDebug(DEVICE_TAG, "Entering Link::removeLink");
		assert(link);

		mutex_lock_guard lock(deviceMutex_);
		// Return immediately if the list is empty
		if (linkList_.empty()) {
			logDebug(DEVICE_TAG, "Link::removeLink, linkList empty");
			return false;
		}

		// Search list for the link. If in the list, return
		for (auto iter = linkList_.begin(); iter != linkList_.end(); ++iter) {
			if (*iter == link) {
				linkList_.erase(iter);
				logDebug(DEVICE_TAG, "Link::removeLink, removing link");
				return true;
			}
		}

		return false;
	}

	LinkList getLinks() {
		mutex_lock_guard lock(deviceMutex_);
		return linkList_;
	}

	uint32_t getLinkCount() {
		mutex_lock_guard lock(deviceMutex_);
		return (uint32_t)linkList_.size();
	}

private:
	UUID_t deviceId_;
	std::string name_;
	LinkList linkList_;
	mutex deviceMutex_;
};

}
}
}



#endif /* DEVICE_H_ */
