//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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
