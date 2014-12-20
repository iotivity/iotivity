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



#ifndef SESSIONSTORE_H_
#define SESSIONSTORE_H_

#include "map"
#include "CMConnector.h"
#include "Thread/Mutex.h"

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {

            class ISessionStoreClearListener
            {
                public:
                    virtual ~ISessionStoreClearListener() {}
                    virtual void onClearSession( OC::Cm::Connector::Session &session ) = 0 ;
            };

            class SessionStore
            {

                public:

                    SessionStore();

                    virtual ~SessionStore();

                    bool clearStore( bool freeContents, OC::Cm::Connector::ISessionStoreClearListener *pListener );

                    bool addSession( OC::Cm::Connector::Session *pSession );

                    bool removeSession( OC::Cm::Connector::Session *pSession );

                    OC::Cm::Connector::Session *getSession( int sessionId) ;

                private:

                    std::map< int, OC::Cm::Connector::Session * > m_sessions;

                    OC::Cm::Platform::Thread::Mutex mStoreMutex ;

            };

        } /* namespace Connector */
    } /* namespace Cm */
} /* namespace OC */
#endif /* SESSIONSTORE_H_ */
