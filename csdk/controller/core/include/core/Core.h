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
