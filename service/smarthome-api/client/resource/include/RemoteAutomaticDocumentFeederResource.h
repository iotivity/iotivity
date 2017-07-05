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
#ifndef SMARTHOME_API_CLIENT_REMOTEAUTOMATICDOCUMENTFEEDER_H_
#define SMARTHOME_API_CLIENT_REMOTEAUTOMATICDOCUMENTFEEDER_H_

#include <SHBaseRemoteResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class RemoteAutomaticDocumentFeederResourceDelegate
            {
            public:
                virtual void onGetState(std::string currentState,
                        std::list<std::string> possibleStates, ResultCode ret) = 0;
            };

            class RemoteAutomaticDocumentFeederResource: public SHBaseRemoteResource,
                    public SHBaseRemoteResourceDelegate
            {
            public:
                void getState();
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
