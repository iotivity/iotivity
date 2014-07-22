//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef UUIDLESS_HPP_
#define UUIDLESS_HPP_

// ============================================================================
// Includes
// ============================================================================

#include <cstring>
#include "Core.h"

	// ============================================================================
	// Namespace
	// ============================================================================
	namespace Intel {
	namespace CCFL {

  class UUIDLess
  {
  public:
     bool operator()(const UUID_t &uuid1, const UUID_t &uuid2) const
     {
        return memcmp(&uuid1, &uuid2, sizeof(uuid1))<0;
     }
  };

}
}


#endif /* UUIDLESS_HPP_ */
