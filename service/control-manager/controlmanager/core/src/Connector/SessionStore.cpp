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



#include "SessionStore.h"
#include "Log.h"

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {

            SessionStore::SessionStore()
            {
            }

            SessionStore::~SessionStore()
            {
            }

            bool
            SessionStore::clearStore( bool freeContents,
                                      OC::Cm::Connector::ISessionStoreClearListener *pListener )
            {
                mStoreMutex.lock();

                if ( freeContents )
                {
                    for ( std::map< int, OC::Cm::Connector::Session * >::iterator itr = m_sessions.begin();
                          itr != m_sessions.end(); itr++ )
                    {
                        OC::Cm::Connector::Session *pSession = (itr->second) ;
                        if (pSession != NULL)
                        {
                            CM_LOG_DEBUG_A(CM_CORE_CLIENT, "Session ID [%d]", pSession->getSessionId());
                            if ( pListener != NULL )
                            {
                                pListener->onClearSession(*pSession) ;
                            }

                            delete pSession ;
                        }
                    }
                }

                m_sessions.clear();

                mStoreMutex.unlock();

                return true;
            }

            bool
            SessionStore::addSession( OC::Cm::Connector::Session *pSession )
            {
                if ( pSession == NULL )
                {
                    return false ;
                }

                CM_LOG_DEBUG_A(CM_CORE_CLIENT, "Session ID [%d]", pSession->getSessionId());

                mStoreMutex.lock();

                m_sessions.insert( std::pair< int, OC::Cm::Connector::Session * > (pSession->getSessionId(),
                                   pSession ));

                mStoreMutex.unlock();

                return true;
            }

            bool
            SessionStore::removeSession( OC::Cm::Connector::Session *pSession )
            {
                if ( pSession == NULL )
                {
                    return false ;
                }

                CM_LOG_DEBUG_A(CM_CORE_CLIENT, "Session ID [%d]", pSession->getSessionId());

                mStoreMutex.lock();

                m_sessions.erase(pSession->getSessionId()) ;

                mStoreMutex.unlock();

                return true;

            }

            OC::Cm::Connector::Session *
            SessionStore::getSession( int sessionId )
            {
                OC::Cm::Connector::Session *pSession = NULL ;

                mStoreMutex.lock();

                std::map< int, OC::Cm::Connector::Session * >::iterator itr = m_sessions.find( sessionId ) ;

                if (itr != m_sessions.end())
                {
                    pSession = itr->second ;
                    CM_LOG_DEBUG_A(CM_CORE_CLIENT, "Session ID [%d]", pSession->getSessionId());
                }

                mStoreMutex.unlock();

                return pSession ;
            }


        } /* namespace Connector */
    } /* namespace Cm */
} /* namespace OC */
