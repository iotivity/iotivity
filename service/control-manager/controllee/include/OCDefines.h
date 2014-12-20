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

#ifndef __OCDEFINES__
#define __OCDEFINES__

namespace OC
{

    enum PServiceType
    {
        InProc = 0,
        OutOfProc,
        MaxServiceType //error control
    };

    enum PModeType
    {
        Server = 0, //What does this mean?
        Client,
        Both,
        MaxModeType //error control
    };

    enum PQualityOfService
    {
        Confirmable = 0,
        NonConfirmable,
        MaxQualityOfService //error control
    };

    enum ControlleeStatus
    {
        PlatformInitComplete = 0,
        DeviceInitComplete,
        ReadyToGo,
        ControlleeError
    };


}

#endif // __OCDEFINES__

