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
 * This file contains the declaration of classes and its members related to
 * RemoteLockStatusResource.
 */

#ifndef SMARTHOME_API_CLIENT_REMOTELOCKSTAUS_H_
#define SMARTHOME_API_CLIENT_REMOTELOCKSTAUS_H_

#include <SHBaseRemoteResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions for callback of
             * RemoteLockStatusResource class.
             */
            class RemoteLockStatusResourceDelegate
            {
            public:
                virtual ~RemoteLockStatusResourceDelegate() {}

                /**
                 * API to handle a response about lock request.
                 *
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onLock(ResultCode code) = 0;

                /**
                 * API to handle a response about unlock request.
                 *
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onUnLock(ResultCode code) = 0;

                /**
                 * API to handle a response about state get request.
                 *
                 * @param[in] state lock state value
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onGetStatus(std::string status, ResultCode code) = 0;
            };

            /**
             * This class contains a set of functions to control RemoteLockStatusResource.
             */
            class RemoteLockStatusResource: public SHBaseRemoteResource,
                    public SHBaseRemoteResourceDelegate
            {
            friend class SHBaseRemoteResourceBuilder;
            public:
                virtual ~RemoteLockStatusResource();

                /**
                 * API to request lock .
                 */
                void lock();

                /**
                 * API to request unlock .
                 */
                void unlock();

                /**
                 * API to get lock statue.
                 */
                void getStatus();

                /**
                 * API to set a instance of delegate class
                 */
                void setRemoteLockStatusResourceDelegate(
                        RemoteLockStatusResourceDelegate* delegate);

            protected:
                RemoteLockStatusResource();

            private:
                virtual void onGet(PropertyBundle bundle, ResultCode ret);
                virtual void onSet(PropertyBundle bundle, ResultCode ret);
                virtual void onObserve(PropertyBundle bundle, const ObserveResponse obsType,
                        ResultCode ret);

            private:
                RemoteLockStatusResourceDelegate *m_delegate;
            };
        }
    }
}
#endif /* SMARTHOME_API_CLIENT_REMOTELOCKSTAUS_H_ */
