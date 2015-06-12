//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef __PRIMITIVERESOURCE_ASSERTUTILS_H
#define __PRIMITIVERESOURCE_ASSERTUTILS_H

#include <cstdint>
#include <octypes.h>

#include <PrimitiveException.h>

namespace OIC
{
    namespace Service
    {

        inline void expectOCStackResult(OCStackResult actual, OCStackResult expected)
        {
            if (actual != expected)
            {
                throw PlatformException(actual);
            }
        }

        inline void expectOCStackResultOK(OCStackResult actual)
        {
            expectOCStackResult(actual, OC_STACK_OK);
        }

    }
}

#endif // __PRIMITIVERESOURCE_ASSERTUTILS_H
