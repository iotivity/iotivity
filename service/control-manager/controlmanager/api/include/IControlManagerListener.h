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



#if !defined(_IControlManagerListener_H)
#define _IControlManagerListener_H

#include "CMConfig.h"
#include <string>
#include "CMError.h"

/**
 * Forward declarations
 */
namespace OC
{
    namespace Cm
    {
        class Device ;

        namespace Serialization
        {
            class ISerializable ;
        }
    }
}

namespace OC
{
    namespace Cm
    {

        /**
        *  @addtogroup ControlManager
        *  @{
        */
        /**
         * @interface   IControlManagerListener
         * @brief       Listener to receive events related to status of Control Manager framework
         *              This interface defines methods required to receive events related to status of Control Manager framework
         * @since       1.0
         */
        class CMAPI IControlManagerListener
        {

            public:

                /**
                 * Default destructor of this class
                 */
                virtual ~IControlManagerListener() {}

                /**
                 * This method will be invoked when framework starts
                 *
                 * @param[in] address   Represents address of the server which is started
                 */
                virtual void onStarted(std::string address) = 0;

                /**
                 * This method will be invoked when framework stops
                 */
                virtual void onStopped() = 0;

                /**
                 * This method will be invoked when framework encounters an error
                 *
                 * @param[in] error     Represents the error object related to Control Manager framework
                 */
                virtual void onError(OC::Cm::CMError error) = 0;

        };
        /** @} */
    }
}

#endif  //_IControlManagerListener_H
