//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef CORE_H_
#define CORE_H_

#include <boost/uuid/uuid.hpp>
typedef boost::uuids::uuid UUID_t;

// ============================================================================
// Namespace
// ============================================================================
namespace Intel {
namespace CCFL {
namespace API {

// ============================================================================
// Enums
// ============================================================================
enum QueryResultType {
	SUCCESS,
	FAILURE
};

}
}
}

#ifdef CPPV11
#include <mutex>
	namespace Intel {
	namespace CCFL {
	namespace API {
		typedef std::mutex mutex;
		typedef std::lock_guard<std::mutex> mutex_lock_guard;
	}
	}
	}
#else
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
	namespace Intel {
	namespace CCFL {
	namespace API {
		typedef boost::mutex mutex;
		typedef boost::lock_guard<boost::mutex> mutex_lock_guard;
	}
	}
	}
#endif

#endif /* CORE_H_ */
