//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef DESCRIPTION_H_
#define DESCRIPTION_H_

// ============================================================================
// Includes
// ============================================================================
#include <string>
#include <memory>
#include <functional>
#include "Service.hpp"

// ============================================================================
// Namespace
// ============================================================================
namespace Intel {
namespace CCFL {
namespace API {

// ============================================================================
// Class
// ============================================================================
class DescriptionGetResult {
	// ============================================================
	// Destructor
	// ============================================================
	public:
		virtual ~DescriptionGetResult() {}

	// ============================================================
	// Public Method(s)
	// ============================================================
	public:
		virtual QueryResultType getResult() const = 0;
		virtual const std::set<Service::SharedPtr>& getServices() const = 0;
};

typedef std::function<void (const DescriptionGetResult&)> DescriptionGetFunction;

// ============================================================================
// Class
// ============================================================================
class Description {
	// ============================================================
	// Type Definition(s)
	// ============================================================
	public:
		typedef std::shared_ptr<Description> SharedPtr;
		typedef std::weak_ptr<Description> WeakPtr;

	// ============================================================
	// Destructor
	// ============================================================
	public:
		virtual ~Description() {}

	// ============================================================
	// Public Method(s)
	// ============================================================
	public:
		virtual const std::set<Service::SharedPtr>& getServices() const = 0;
};

}
}
}

#endif /* DESCRIPTION_H_ */
