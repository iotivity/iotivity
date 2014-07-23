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

#ifndef LINK_HPP_
#define LINK_HPP_

// ============================================================================
// Includes
// ============================================================================
#include <string>
#include <memory>
#include "Description.hpp"
#include "Service.hpp"
#include "Characteristic.hpp"

// ============================================================================
// Namespace
// ============================================================================
namespace Intel {
namespace CCFL {
namespace API {

class Link {
	// ============================================================
	// Type Definition(s)
	// ============================================================
public:
		typedef std::shared_ptr<Link> SharedPtr;
		typedef std::weak_ptr<Link> WeakPtr;

		typedef std::function<void (const std::string& propertyName, const PropertyGetFunction& asyncReturnFunc)> RegisteredGetPropFunction;
		typedef std::function<void (const std::string& propertyName, const std::string& propertyValue, const PropertySetFunction& asyncReturnFunc)> RegisteredSetPropFunction;
		typedef std::function<void (const DescriptionGetFunction& asyncReturnFunc)> RegisteredGetDescriptionFunction;

public:
	Link() {
		name_ = "unknown link name";
		registeredGetPropFunction_ = nullptr;
		registeredSetPropFunction_ = nullptr;
		registeredGetDescriptionFunction_ = nullptr;
	};

	virtual ~Link() {};

	virtual const std::string& getName() {
		return name_;
	}

	virtual void setName(const std::string name) {
		name_ = name;
	}

	// Used at application interface
	virtual void getProperty(const std::string& propertyName, PropertyGetFunction& asyncReturnFunc) {
		// Call registered function
		if (nullptr != registeredGetPropFunction_) {
			registeredGetPropFunction_(propertyName, asyncReturnFunc);
		}
	}

	virtual void setProperty(const std::string& propertyName, const std::string& propertyValue, PropertySetFunction& asyncReturnFunc) {
		if (nullptr != registeredSetPropFunction_) {
			registeredSetPropFunction_(propertyName, propertyValue, asyncReturnFunc);
		}
	}

	virtual void getDescription(DescriptionGetFunction& asyncReturnFunc) {
		if (nullptr != registeredGetDescriptionFunction_) {
			registeredGetDescriptionFunction_(asyncReturnFunc);
		}
	}

	// Used at protocol interface
	virtual void registerGetPropertyFunction(RegisteredGetPropFunction& registeredGetPropFunction) {
		registeredGetPropFunction_ = registeredGetPropFunction;
	}

	virtual void registerSetPropertyFunction(RegisteredSetPropFunction& registeredSetPropFunction) {
		registeredSetPropFunction_ = registeredSetPropFunction;
	}

	virtual void registerGetDescriptionFunction(RegisteredGetDescriptionFunction& registeredGetDescriptionFunction) {
		registeredGetDescriptionFunction_ = registeredGetDescriptionFunction;
	}

private:
	std::string name_;
	RegisteredGetPropFunction registeredGetPropFunction_;
	RegisteredSetPropFunction registeredSetPropFunction_;
	RegisteredGetDescriptionFunction registeredGetDescriptionFunction_;
};

}
}
}

#endif /* LINK_HPP_ */
