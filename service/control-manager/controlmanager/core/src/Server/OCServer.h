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



#if !defined(_SERVER_H)
#define _SERVER_H

#include "CMServerConnector.h"
#include "CMConnector.h"
#include "Connector/SessionStore.h"
#include "OCPlatform.h"
#include "ocstack.h"
#include "Connector/Server/ServerSession.h"
#include "Connector/Server/IServerListener.h"
#include "CMCommon.h"
#include "OCResourceRequest.h"
#include "OCResourceResponse.h"


namespace OC
{
    namespace Cm
    {
        namespace Core
        {
            namespace Server
            {
                class Server : public OC::Cm::Connector::Server::IServerListener
                {

                    public:

                        Server( int serverId ) ;

                        virtual ~Server() ;

                        int getServerID() ;

                        bool initialize();

                        bool deinit( );

                        bool start();

                        bool stop();

                        void addServerListener(OC::Cm::Connector::Server::IServerListener &listener);

                        bool sendResponse(  OC::Cm::Connector::Server::ServerSession &session );

                        virtual void onServerStarted(int serverId, std::string address);

                        virtual void onServerStopped(int serverId );

                        virtual void onServerError(int serverId, OC::Cm::ErrorCode errorCode);

                        virtual bool handleRequest( OC::Cm::Connector::Server::ServerSession &session );

                        virtual void onServerSessionStarted(int serverId,
                                                            OC::Cm::Connector::Server::ServerSession &session) ;

                        virtual void onServerSessionCompleted(int serverId,
                                                              OC::Cm::Connector::Server::ServerSession &session) ;

                        std::string getProtocol() ;

                        static OCEntityHandlerResult handleOCRequest(const std::shared_ptr<OC::OCResourceRequest> request);

//  static void handleOCRequest(const std::shared_ptr<OC::OCResourceRequest> request, std::shared_ptr<OC::OCResourceResponse> response);

                        static OC::Cm::Connector::Server::IServerListener *p_serverSessionListener ;

                    private:

                        int mServerId ;

                        OC::Cm::Connector::SessionStore mSessionStore ;

                        bool addSession(OC::Cm::Connector::Server::ServerSession *session);

                        bool removeSession(OC::Cm::Connector::Server::ServerSession *session);

                        OC::Cm::Connector::Server::ServerSession *getSession(int sessionid);

                        bool registerOCResource(std::string resourceURI, std::string resourceName, bool isDiscoverable);

                        bool registerOCResource();


                        static bool prepareRequestHeaders(const HeaderOptions &headerOptions,
                                                          OC::Cm::Connector::ControlRequest &request);

                        bool prepareResponseHeaders(OC::Cm::Connector::ControlResponse &response,
                                                    HeaderOptions &headerOptions);

                };
            }
        }
    }
}

#endif  //_SERVER_H
