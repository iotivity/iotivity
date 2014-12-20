//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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



#if !defined(_CMSTATES_H)
#define _CMSTATES_H

#include "CMConfig.h"

namespace OC
{

    namespace Cm
    {
        /**
        *  @addtogroup ControlManager
        *  @{
        */
        /**
         *
         * @enum        CMStates
         * @brief       This enumeration defines different Control Manager States
         *
         */
        enum CMStates
        {
            CM_UNKNOWN      = 0,    /**< Represents Unknown State which is also default state for Control Manager Framework. */
            CM_STARTING     = 1,    /**< Represents Control Manager Framework staring state when framework receives Control Manager Start. */
            CM_STARTED  = 2,    /**< Represents Control Manager Framework started state when framework started successfully. */
            CM_STOPPING     = 3,    /**< Represents Control Manager Framework stopping state when framework receives Control Manager Stop. */
            CM_STOPPED  = 4,    /**< Represents Control Manager Framework stopped state when framework stopped successfully. */
            CM_FAILED       = 5,    /**< Represents Control Manager Framework failed state when some thing wrong when framework starts. */
            CM_INTERNAL_STOPPED = 6 /**< Represents Control Manager Framework stopped state when framework internal stopped successfully. */
        };

        /** @} */
    } /* namespace Cm */

} /* namespace OC */

#endif  /* _CMSTATES_H */
