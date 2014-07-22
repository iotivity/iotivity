//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef CHARACTERISTIC_H_
#define CHARACTERISTIC_H_

// ============================================================================
// Includes
// ============================================================================
#include <string>
#include <memory>
#include <functional>
#include <stdint.h>
#include "Core.h"

// ============================================================================
// Namespace
// ============================================================================
namespace Intel {
namespace CCFL {
namespace API {

// ============================================================================
// Class
// ============================================================================
class PropertyGetResult {
	// ============================================================
	// Destructor
	// ============================================================
	public:
		virtual ~PropertyGetResult() {}

	// ============================================================
	// Public Method(s)
	// ============================================================
	public:
		virtual QueryResultType getResult() const = 0;
		virtual const std::string& getName() const = 0;
		virtual const std::string& getValue() const = 0;
};

typedef std::function<void (const PropertyGetResult&)> PropertyGetFunction;

// ============================================================================
// Class
// ============================================================================
class PropertySetResult {
	// ============================================================
	// Destructor
	// ============================================================
	public:
		virtual ~PropertySetResult() {}

	// ============================================================
	// Public Method(s)
	// ============================================================
	public:
		virtual QueryResultType getResult() const = 0;
		virtual const std::string& getName() const = 0;
		virtual const std::string& getValue() const = 0;
};

typedef std::function<void (const PropertySetResult&)> PropertySetFunction;

// ============================================================================
// Class
// ============================================================================

class Characteristic {
  // ============================================================
  // Type Definition(s)
  // ============================================================
  public:
    typedef std::shared_ptr<Characteristic> SharedPtr;
    typedef std::weak_ptr<Characteristic> WeakPtr;

  // ============================================================
  // Destructor
  // ============================================================
  public:
    virtual ~Characteristic() {}

  // ============================================================
  // Public Method(s)
  // ============================================================
  public:
    virtual const std::string& getName() const = 0;
    virtual bool isReadable() const = 0;
    virtual bool isWritable() const = 0;
    virtual bool isConstant() const = 0;
};

}
}
}

#endif
