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
 * RemoteAutomaticDocumentFeederResource.
 */

#ifndef SMARTHOME_API_CLIENT_REMOTEAUTOMATICDOCUMENTFEEDER_H_
#define SMARTHOME_API_CLIENT_REMOTEAUTOMATICDOCUMENTFEEDER_H_

#include <SHBaseRemoteResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions for callback of
             * RemoteAutomaticDocumentFeederResource class.
             */
            class RemoteAutomaticDocumentFeederResourceDelegate
            {
            public:
                /**
                 * API to handle a response about State Get request.
                 *
                 * @param[in] currentState current state
                 * @param[in] possibleStates possible state values
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onGetState(std::string currentState,
                        std::list<std::string> possibleStates, ResultCode ret) = 0;
            };

            /**
             * This class contains a set of functions to control RemoteAutomaticDocumentFeederResource.
             */
            class RemoteAutomaticDocumentFeederResource: public SHBaseRemoteResource,
                    public SHBaseRemoteResourceDelegate
            {
            public:

                /**
                 * API to get state value
                 */
                void getState();

                /**
                 * API to set remote delegate.
                 *
                 * @param[in] delegate instance of delegate class
                 */
                void setDelegate(RemoteAutomaticDocumentFeederResourceDelegate *delegate);

            public:
                RemoteAutomaticDocumentFeederResource();
                virtual ~RemoteAutomaticDocumentFeederResource();

            private:
                virtual void onGet(PropertyBundle bundle, ResultCode ret);
                virtual void onSet(PropertyBundle bundle, ResultCode ret);
                virtual void onObserve(PropertyBundle bundle, const ObserveResponse obsType,
                        ResultCode ret);

            private:
                RemoteAutomaticDocumentFeederResourceDelegate *m_delegate;
                std::string m_currentState;
                std::list<std::string> m_possibleStates;
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_REMOTEOPERATIONALSTATE_H_ */
