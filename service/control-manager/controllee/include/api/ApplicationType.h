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

#ifndef _APPLICATIONTYPE_H
#define _APPLICATIONTYPE_H

namespace OC
{
    namespace Cm
    {

        /**
         * @enum        ApplicationType
         * @brief       This enumeration defines types of Applications.
         */
        enum ApplicationType
        {
            ApplicationType_Controller = 0, /**< Represents an Application having Controller functionality i.e an application which can control other devices. */
            ApplicationType_Controllable,   /**< Represents an Application having Controllable functionality i.e an application which can be controlled by other devices like Controller. */
            ApplicationType_Both            /**< Represents an Application having both Controller and Controllable functionality i.e an application which can control other devices as well as can be controlled by other devices. */
        };

    } /* namespace Cm */
} /* namespace OC */

#endif /* APPLICATIONTYPE_H_ */
