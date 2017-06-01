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
#ifndef SMARTHOME_API_SERVER_LOCKSTATUS_H_
#define SMARTHOME_API_SERVER_LOCKSTATUS_H_

#include <SHBaseResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class LockStatusResourceDelegate
            {
            public:
                virtual ~LockStatusResourceDelegate() {}

                virtual ResultCode onLock() = 0;
                virtual ResultCode onUnLock() = 0;
            };

            class LockStatusResource: public SHBaseResource, public SHBaseResourceDelegate
            {
            public:
                LockStatusResource();
                virtual ~LockStatusResource();

                std::string getState();
                bool setState(std::string state);
                void setDelegate(LockStatusResourceDelegate* delegate);

            private:
                ResultCode onGet(RequestId requestId, const ResourceQuery& query);
                ResultCode onSet(RequestId requestId, const PropertyBundle& bundle,
                                 const ResourceQuery& query);

                LockStatusResourceDelegate *m_delegate;
            };
        }
    }
}

#endif /* SMARTHOME_API_SERVER_LOCKSTATUS_H_ */
