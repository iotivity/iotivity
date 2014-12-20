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



#if !defined(_CMEngine_H)
#define _CMEngine_H

#include "Configuration.h"

#include "IControlManagerListener.h"
#include "CMSerialization.h"
#include "CMClientResource.h"
#include "CMServerResourceHandler.h"

#include "CMConnector.h"
#include "CMServerConnector.h"
#include "CMClientConnector.h"
#include "DeviceType.h"
#include "DeviceDomain.h"
#include "Thread/Mutex.h"
#include "list"
#include "string"
#include "CMStates.h"
#include "CMError.h"
#include "CMCoreSerializableDataFactory.h"
#include "CMCoreResourceHandlerFactory.h"


//OIC
#include "OCApi.h"
#include "OCPlatform.h"
#include "OCResource.h"
#include "Connector/Client/IClientListener.h"
#include "Connector/Server/IServerListener.h"
#include "Client/OCClient.h"
#include "Server/OCServer.h"

namespace OC
{
    namespace Cm
    {
        namespace Core
        {
            namespace Engine
            {

                class CMEngine : public OC::Cm::Connector::Server::IServerListener,
                    public OC::Cm::Connector::Client::IClientListener
                {
                    public:

                        CMEngine(OC::Cm::IContext *pContext);

                        virtual ~CMEngine();


                        bool init(OC::Cm::Configuration *configuration );

                        bool deinit();

                        bool setDeviceAddress(std::string &deviceAddress) ;

                        void setUUID(const std::string &uuid);

                        std::string getUUID();

                        void setDeviceType(OC::Cm::DeviceType deviceType);

                        bool setServerConnector(OC::Cm::Connector::Server::IServerConnector &connector, bool isInternal);

                        bool setClientConnector(OC::Cm::Connector::Client::IClientConnector &connector, bool isInternal);


                        bool start();

                        bool stop();

                        bool startClients();

                        void setCMEngineState(OC::Cm::CMStates state);

                        OC::Cm::CMStates getCMEngineState();

                        void setControlManagerListener(OC::Cm::IControlManagerListener &listener);

                        OC::Cm::IControlManagerListener *getControlManagerListener();

                        void removeControlManagerListener(OC::Cm::IControlManagerListener &listener);

                        bool sendResponse( OC::Cm::Connector::Server::ServerSession &session );

                        bool sendResponse(  int status_code,
                                            OC::Cm::Serialization::ISerializable *response,
                                            OC::Cm::Connector::Server::ServerSession &session);
                        bool sendDirectResponse(OC::Cm::Connector::Server::ServerSession &session);

                        CMError sendClientRequest(      std::string &protocol,
                                                        std::string &deviceAddress,
                                                        std::string &resourceId,
                                                        std::string &methodId,
                                                        std::map<std::string, std::string> *queryParams,
                                                        OC::Cm::Serialization::ISerializable  *requestData,
                                                        int &clientSessionId,
                                                        OC::Cm::Client::Resource::IResourceResponseListener &listener,
                                                        bool forceApplicationSerializables = false) ;

                        virtual void onServerStarted(int serverId, std::string address);

                        virtual void onServerStopped(int serverId );

                        virtual void onServerError(int serverId, OC::Cm::ErrorCode errorCode);

                        virtual void onServerSessionStarted(int serverId,
                                                            OC::Cm::Connector::Server::ServerSession &session);

                        virtual void onServerSessionCompleted(int serverId,
                                                              OC::Cm::Connector::Server::ServerSession &session);

                        virtual bool handleRequest( OC::Cm::Connector::Server::ServerSession &session ) ;

                        virtual void onResponseReceived( OC::Cm::Connector::Client::ClientSession &client_sessioin );

                        virtual void onErrorRecieved( OC::Cm::ErrorCode error_code,
                                                      OC::Cm::Connector::Client::ClientSession &client_sessioin) ;

                        virtual void onClientSessionCompleted( OC::Cm::Connector::Client::ClientSession &client_sessioin );

                        OC::Cm::Serialization::SerializableDataFactory *getSerializableDataFactory() ;

                        OC::Cm::Configuration *getCMConfiguration() { return m_pConfiguration; }



                        bool                        isSCSChannelConnected;
                        bool                        isInternalStop;


                    private:

                        OC::Cm::CMStates            m_engineState;

                        OC::Cm::Platform::Thread::Mutex     m_stateMutex ;

                        OC::Cm::IControlManagerListener             *m_pControlManagerListener;

                        OC::Cm::Configuration               *m_pConfiguration;

                        std::string                         m_deviceAddress;

                        OC::Cm::IContext                    *p_context ;

                        std::string                         m_uuid;
                        std::string                         m_clientProtocol;

                        OC::Cm::DeviceType              m_deviceType ;

                        CMCoreSerializableDataFactory       *pSerializableDataFactory ;

                        CMCoreResourceHandlerFactory        *pResourceHandlerFactory ;

                        bool                                startRequested ;

                        bool                                stopRequested ;



                        OC::Cm::Core::Client::Client        *p_client ;

                        OC::Cm::Core::Server::Server        *p_server ;


                        bool serialize( OC::Cm::Serialization::ISerializable *data, std::string &method,
                                        std::map<std::string, std::string> *queryParams, std::string &contentType,
                                        std::string &payLoad, int &payloadLen, std::string &prettyPrintOutput, std::string &errorMessage);

                        int deSerialize( std::string &contentType, std::string &payLoad, int payloadLen,
                                         std::string &method, std::map<std::string, std::string> &queryParams,
                                         OC::Cm::Serialization::ISerializable *data, std::string &errorMessage);

                        void invokeErrorCallBack( OC::Cm::ErrorCode error_code, int status_code,
                                                  OC::Cm::Serialization::ISerializable *errorResponse,
                                                  OC::Cm::Connector::Client::ClientSession &client_sessioin, std::string errorMessage) ;

                        void invokeResponseCallBack(    std::string &methodId,
                                                        OC::Cm::Connector::Client::ClientSession &client_sessioin,
                                                        int status_code, OC::Cm::Serialization::ISerializable *pResponse);

                        bool sendErrorResponse( int status_code, std::string errorCode, std::string errorDescription,
                                                OC::Cm::Connector::Server::ServerSession &session);

                };



            }
        }
    }
}

#endif  //_CMEngine_H
