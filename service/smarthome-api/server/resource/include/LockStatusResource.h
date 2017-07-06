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

/**
 * This file contains the declaration of classes and its members related to LockStatusResource.
 */

#ifndef SMARTHOME_API_SERVER_LOCKSTATUS_H_
#define SMARTHOME_API_SERVER_LOCKSTATUS_H_

#include <SHBaseResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions for callback of
             * LockStatusResource class.
             */
            class LockStatusResourceDelegate
            {
            public:
                virtual ~LockStatusResourceDelegate() {}

                /**
                 * API to handle a lock request .
                 *
                 * @return ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual ResultCode onLock() = 0;

                /**
                 * API to handle a unlock request .
                 *
                 * @return ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual ResultCode onUnLock() = 0;
            };

            /**
             * This class contains a set of functions to change the properties of LockStatus.
             */
            class LockStatusResource: public SHBaseResource, public SHBaseResourceDelegate
            {
            public:
                LockStatusResource();
                virtual ~LockStatusResource();

                /**
                 * API to get current lock state.
                 *
                 * @param[out] string current lock state
                 */
                std::string getState();

                /**
                 * API to set current lock state.
                 *
                 * @param[in] state current lock state
                 * @return true if success
                 */
                bool setState(std::string state);

                /**
                 * API to set delegate
                 *
                 * @param[out] delegate instance of delegate
                 */
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
