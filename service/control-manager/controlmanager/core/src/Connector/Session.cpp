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



#include "Connector/Session.h"
#include "Utils.h"

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {

            Session::Session( int id )
            {
                this->m_Id = id ;
                this->m_sessionId = generateSessionId() ;
            }

            Session::~Session()
            {
                // TODO Auto-generated destructor stub
            }

            int
            Session::getSessionId() const
            {
                return m_sessionId ;
            }

            OC::Cm::Connector::ControlRequest &
            Session::getRequest()
            {
                return m_request;
            }

            OC::Cm::Connector::ControlResponse &
            Session::getResponse()
            {
                return m_response;
            }

        } /* namespace Connector */
    } /* namespace Cm */
} /* namespace OC */
