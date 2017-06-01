/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
#ifndef SMARTHOME_API_SERVER_ENTITYHANDLER_CONTEXT_H_
#define SMARTHOME_API_SERVER_ENTITYHANDLER_CONTEXT_H_

#include "octypes.h"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class SHBaseResource_Impl;
            class EntityHandlerContext
            {
            public:
                EntityHandlerContext(const SHBaseResource_Impl* resource);
                ~EntityHandlerContext();

                const SHBaseResource_Impl* getResource();

            private:
                const SHBaseResource_Impl* m_resource;
            };
        }
    }
}
#endif /* SMARTHOME_API_SERVER_ENTITYHANDLER_CONTEXT_H_ */
