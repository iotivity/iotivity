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



#ifndef _SESSION_H
#define _SESSION_H

#include "CMConnector.h"

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {

            /**
             * @class       Session
             * @brief       This class holds the information related to a Control Manager Session.
             * @since       1.0
             */
            class CMAPI Session
            {

                public:

                    /**
                     *  Default constructor of this class
                     *
                     *  @param[in] id   Represents id of the server/client to which this session belongs
                     */
                    Session( int id );

                    /**
                     * Default destructor of this class
                     */
                    virtual ~Session();


                    /**
                     * This method returns id of this session
                     *
                     * @return  id of this session
                     */
                    int getSessionId() const;

                    /**
                     * This method returns object reference which contains the details of the request received in this session
                     *
                     * @return Reference to the request object
                     */
                    OC::Cm::Connector::ControlRequest &getRequest() ;

                    /**
                     * This method returns object which contains the details of the response sent in this session.
                     *
                     * @return Reference to the response object
                     */
                    OC::Cm::Connector::ControlResponse &getResponse() ;

                protected:

                    OC::Cm::Connector::ControlRequest       m_request ;     /**< Represents object of ControlRequest */

                    OC::Cm::Connector::ControlResponse  m_response ;    /**< Represents object of ControlResponse */

                    int m_Id;                                           /**< Represents id of request */

                    int m_sessionId;                                    /**< Represents session id of request */
            };

        } /* namespace Connector */
    } /* namespace Cm */
} /* namespace OC */

#endif /* _SESSION_H */
