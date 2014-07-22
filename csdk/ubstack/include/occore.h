//******************************************************************
///
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef OCCORE_H_
#define OCCORE_H_


// ============================================================================
// Namespace
// ============================================================================
namespace oc {
namespace ub {

// ============================================================================
// Enums
// ============================================================================
enum OCQueryResultType {
	SUCCESS,
	FAILURE
};

enum OCStackMode {
    OC_CLIENT = 0,
    OC_SERVER,
    OC_CLIENT_SERVER
};

}
}

#ifdef CPPV11
#include <mutex>
	namespace oc {
	namespace ub {
		typedef std::mutex mutex;
		typedef std::lock_guard<std::mutex> mutex_lock_guard;
	}
	}
#else
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
	namespace oc {
	namespace ub {
		typedef boost::mutex mutex;
		typedef boost::lock_guard<boost::mutex> mutex_lock_guard;
	}
	}
#endif

#endif /* OCCORE_H_ */
