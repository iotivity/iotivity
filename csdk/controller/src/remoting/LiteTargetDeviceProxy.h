//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef LITETARGETDEVICEPROXY_H_
#define LITETARGETDEVICEPROXY_H_

// ============================================================================
// Includes
// ============================================================================
#include <set>

#include "LiteTargetDevice.hpp"

// ============================================================================
// Namespace
// ============================================================================
namespace remoting {

  // ==========================================================================
  // Class
  // ==========================================================================

class LiteTargetDeviceProxy: public LiteTargetDevice
{
public:
	// Methods
  LiteTargetDeviceProxy(UUID_t uuid);
	virtual ~LiteTargetDeviceProxy();


private:
};

}

#endif /* LITETARGETDEVICEPROXY_H_ */
