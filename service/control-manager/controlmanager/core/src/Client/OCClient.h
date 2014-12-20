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



#if !defined(_CLIENT_H)
#define _CLIENT_H

const int SUCCESS_RESPONSE = 0;

//Forward declarations
#include "CMConnector.h"
#include "CMClientConnector.h"
#include "Connector/SessionStore.h"
#include "Connector/HeadersList.h"
#include "OCPlatform.h"
#include "OCApi.h"
#include "OCResource.h"
#include "CMCommon.h"
#include "Connector/Client/ClientSession.h"




namespace OC
{
    namespace Cm
    {
        namespace Core
        {
            namespace Client
            {
                class Client :  public OC::Cm::Connector::Client::IClientListener,
                    public OC::Cm::Connector::ISessionStoreClearListener
                {

                    public:

                        Client();

                        virtual ~Client();

                        bool init(int clientId);

                        bool deinit();

                        bool start();

                        bool stop();

                        std::string getProtocol() ;

                        void addClientListener(OC::Cm::Connector::Client::IClientListener &listener);

                        OC::Cm::Connector::Client::ClientSession *createNewSession( int clientSessionId );

                        int sendRequest( OC::Cm::Connector::Client::ClientSession &session );

                        virtual void onResponseReceived( OC::Cm::Connector::Client::ClientSession &client_sessioin );

                        virtual void onErrorRecieved(  OC::Cm::ErrorCode error_code,
                                                       OC::Cm::Connector::Client::ClientSession &client_sessioin );

                        virtual void onClientSessionCompleted( OC::Cm::Connector::Client::ClientSession &client_sessioin );

                        virtual void onClearSession( OC::Cm::Connector::Session &session ) ;

                        static void onResponse (const HeaderOptions &headerOptions,
                                                const OC::OCRepresentation &attributeMap, const int eCode, int requestID);

                        static void onOCDelete (const HeaderOptions &headerOptions, const int eCode, int requestID);

                        //int getCMErrorCodeFromOCResult (OCStackResult result);

                        static OC::Cm::Connector::Client::IClientListener  *p_client_listener;

                        static OC::Cm::Connector::SessionStore mSessionStore;

                        static int msID ;

                    private:

                        bool mIsStarted ;

                        static bool prepareResponseHeaders(const HeaderOptions &headerOptions,
                                                           OC::Cm::Connector::ControlResponse &response);

                        bool prepareRequestHeaders(OC::Cm::Connector::ControlRequest &request,
                                                   HeaderOptions &headerOptions);


                };


            }
        }
    }
}

#endif  //_CLIENT_H
