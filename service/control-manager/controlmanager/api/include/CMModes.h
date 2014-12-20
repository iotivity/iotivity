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



#if !defined(_CMMODES_H)
#define _CMMODES_H

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
         * @enum        CMModes
         * @brief       This enumeration defines different Control Manager Modes
         */
        enum CMModes
        {
            NORMAL_MODE,            /**< Represents Control Manager Normal Mode and it is default Mode */
            EASY_SETUP_MODE,        /**< Represents Control Manager Easy Setup Mode */
            REGISTRATION_MODE,      /**< Represents Registration Mode */
            RE_REGISTRATION_MODE    /**< Represents Re-Registration Mode */
        };

        /** @} */
    } /* namespace Cm */

} /* namespace OC */

#endif  //_CMMODES_H
