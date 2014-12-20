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



#ifndef NOTIFICATIONSRESOURCEHANDLER_H_
#define NOTIFICATIONSRESOURCEHANDLER_H_

#include "CMServerResourceHandler.h"
#include "xsd/Elements.h"
namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            class NotificationsResourceHandler : public OC::Cm::Server::ResourceHandler::SyncResourceHandler
            {

                public:

                    NotificationsResourceHandler(OC::Cm::IContext *context);

                    virtual ~NotificationsResourceHandler();

                    bool handleSyncResource( OC::Cm::Connector::Server::ServerSession &session,
                                             std::string methodId,
                                             int &statusCode,
                                             OC::Cm::Serialization::ISerializable  *request,
                                             OC::Cm::Serialization::ISerializable  *response) ;

                    bool onPOST( OC::Cm::Connector::Server::ServerSession &session, int &statusCode ,
                                 ::Notification *reqData);


            };
        }
    }
}
#endif /* NOTIFICATIONSRESOURCEHANDLER_H_ */
