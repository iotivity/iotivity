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



#ifndef OCDEVICECOMMON_H_
#define OCDEVICECOMMON_H_

namespace OC
{
    namespace Cm
    {
        namespace Core
        {
            namespace DeviceFinder
            {
                namespace Impl
                {

                    /*
                     * OC Device Notification Type Enumeration
                     */

                    typedef enum OC_DEVICE_NOTIFICATION_TYPE
                    {
                        OC_DEVICE_ADDED   = 0,          // ADD
                        OC_DEVICE_DELETED = 1,          // DELETE
                        OC_DEVICE_ERROR = 2             // ERROR
                    } OC_DEVICE_NOTIFICATION_TYPE;

                } // Impl
            } // DeviceFinder
        } // Core
    } // Cm
} // OC


#endif /* OCDEVICECOMMON_H_ */
