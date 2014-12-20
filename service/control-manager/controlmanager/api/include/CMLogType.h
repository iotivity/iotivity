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



#ifndef CM_LOG_TYPE_H_
#define CM_LOG_TYPE_H_

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
         * @enum        CMLogType
         * @brief       This enumeration defines types of Control Manager-Logging
         */
        enum CMLogType
        {
            CM_LOG_TYPE_SILENT  = -3,   /**< Represents log level which Disables logging itself */
            CM_LOG_TYPE_ERROR   = -2,   /**< Represents log level which logs ERROR onwards and it is default type of Control Manager-Logging */
            CM_LOG_TYPE_WARN    = -1,   /**< Represents log level which logs WARNING onwards */
            CM_LOG_TYPE_INFO    = 0,    /**< Represents log level which logs INFO onwards */
            CM_LOG_TYPE_DEBUG   = 1     /**< Represents log level which logs DEBUG onwards */
        };

        /** @} */
    } /* namespace Cm */

} /* namespace OC */


#endif /* CM_LOG_TYPE_H_ */
