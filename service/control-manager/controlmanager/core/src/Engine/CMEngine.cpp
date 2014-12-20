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



#include "CMDataTypes.h"
#include "CMEngine.h"
#include "Log.h"

#include "CMNotification.h"
#include "../Serialization/Json/JsonSerializer.h"
#include "../Serialization/Json/JsonDeserializer.h"
#include "Connector/Client/ClientSessionData.h"
#include "Connector/Server/ServerSessionData.h"
#include "CMCommon.h"

#include <sstream>
#include "CMContext.h"
#include "Utils.h"

#define MODULE_ID CM_CORE_ENGINE

#define ERROR_CODE_GENERIC_ERROR            "1000"
#define ERROR_CODE_INVALID_METHOD           "1001"
#define ERROR_CODE_INVALID_RESOURCE_PATH    "1002"
#define ERROR_CODE_INTERNAL_SERVER_ERROR    "1003"
#define ERROR_CODE_NOT_FOUND                "1004"
#define ERROR_CODE_NOT_ACCEPTABLE           "1005"
#define ERROR_CODE_DESERIALIZATION_FAILED   "1006"
#define ERROR_CODE_SERIALIZATION_FAILED     "1007"
#define ERROR_CODE_INVALID_TOKEN            "1008"
#define ERROR_CODE_INVALID_UUID             "1009"
#define ERROR_CODE_NO_DEVICE_TOKEN          "1010"

#define CONTROLLER_PORT "8889"
#define CONTROLLED_PORT "8889"
//#define TEST_LOCAL_D2D 1

#define OIC_PROTOCOL_NAME "coap"

namespace OC
{
    namespace Cm
    {
        namespace Core
        {
            namespace Engine
            {

                CMEngine::CMEngine(OC::Cm::IContext *pContext)
                {
                    m_pConfiguration            = NULL;
                    m_pControlManagerListener               = NULL ;
                    p_context                   = pContext ;
                    m_deviceType                = DeviceType_Unknown ;
                    pSerializableDataFactory    = NULL ;
                    pResourceHandlerFactory     = NULL ;
                    p_client                    = NULL ;
                    p_server                    = NULL ;

                    m_engineState = CM_UNKNOWN;

                    startRequested              = false ;
                    stopRequested               = false ;

                    isSCSChannelConnected = false;
                    isInternalStop = false;
                    m_clientProtocol = "";


                }

                CMEngine::~CMEngine()
                {
                    this->deinit() ;

                    m_stateMutex.lock();
                    m_stateMutex.unlock();
                }

                bool
                CMEngine::startClients()
                {
                    bool bRet = false ;

                    return bRet;
                }

                bool
                CMEngine::start()
                {

                    CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "Starting Control Manager Engine");

                    this->startClients();

                    onServerStarted(0, getIPAddress(m_deviceAddress));

                    return true ;
                }

                bool
                CMEngine::stop()
                {
                    CM_LOG_DEBUG_A(CM_CORE_ENGINE, "%s", "Control Manager Engine Stop called") ;
                    bool bRet = false;

                    CM_LOG_DEBUG_A(CM_CORE_ENGINE, "%s", "Stopped all clients") ;
                    if (this->p_client != NULL)
                    {
                        this->p_client->stop();
                    }
                    CM_LOG_DEBUG_A(CM_CORE_ENGINE, "%s", "Stopped Server") ;
                    if (this->p_server != NULL)
                    {
                        this->p_server->stop();
                    }

                    return bRet ;
                }

                void
                CMEngine::setCMEngineState(OC::Cm::CMStates state)
                {
                    m_stateMutex.lock() ;
                    m_engineState = state;
                    m_stateMutex.unlock();
                }

                OC::Cm::CMStates
                CMEngine::getCMEngineState()
                {
                    m_stateMutex.lock() ;
                    OC::Cm::CMStates state = m_engineState;
                    m_stateMutex.unlock();

                    return state;
                }

                bool
                CMEngine::init(Configuration *pConfiguration)
                {
                    TryReturn( pConfiguration != NULL, false, "Invalid Configuration" ) ;
                    this->m_pConfiguration = pConfiguration ;

                    this->pResourceHandlerFactory = new CMCoreResourceHandlerFactory() ;
                    TryReturn( pResourceHandlerFactory != NULL, false, "Mem alloc failed" ) ;

                    this->pSerializableDataFactory = new CMCoreSerializableDataFactory() ;
                    TryReturn( pSerializableDataFactory != NULL, false, "Mem alloc failed" ) ;

                    pResourceHandlerFactory->pApplicationFactory = pConfiguration->getResourceHandlerFactory() ;
                    pSerializableDataFactory->pApplicationFactory = pConfiguration->getSerializableDataFactory() ;

                    OC::Cm::Core::CMContext *pCMContext = (OC::Cm::Core::CMContext *)p_context ;
                    TryReturn(pCMContext != NULL, false, "Failed to get Control Manager Context Class Instance");

                    if (pCMContext->p_subscriptionManager != NULL)
                    {
                        pSerializableDataFactory->pSubscriptionFactory  =
                            pCMContext->p_subscriptionManager->getSerializationFactory() ;
                        pResourceHandlerFactory->pSubscriptionFactory   =
                            pCMContext->p_subscriptionManager->getResourceHandlerFactory() ;
                    }

                    pResourceHandlerFactory->setContext(p_context);

                    bool bRet = false ;

                    std::string ipAddr ;
                    std::string port ;

                    getIPAddrAndPort (m_deviceAddress, ipAddr, port);

                    CM_LOG_DEBUG_A(CM_CORE_ENGINE, "Initializing OCPlatform IPAddress[%s], Port[%s] ", ipAddr.c_str(),
                                   port.c_str()) ;

//  OC::PlatformConfig ocPlatformConfig(OC::ServiceType::InProc,
//                                      OC::ModeType::Both, ipAddr, atoi(port.c_str()),
//                                      OC::QualityOfService::NonConfirmable);
//  TODO : Should be removed after testing
                    OC::PlatformConfig ocPlatformConfig(OC::ServiceType::InProc,
                                                        OC::ModeType::Both, ipAddr, 0,
                                                        OC::QualityOfService::LowQos);


                    OC::OCPlatform::Configure(ocPlatformConfig);
//  TODO: Verify the platform is initialized
//  TryReturn( p_ocPlatform != NULL, false, "Mem alloc failed, Error in creating  OCPlatform" ) ;

                    CM_LOG_DEBUG_A(CM_CORE_ENGINE, "%s", "Setting OCClient configurations") ;


                    this->p_client = new OC::Cm::Core::Client::Client() ;
                    TryReturn( p_client != NULL, false, "Mem alloc failed" ) ;

                    bRet = this->p_client->init(0);
                    TryReturn( bRet != false, false, "OCClient initialization failed" ) ;
                    this->p_client->addClientListener(*this);

                    this->p_server = new OC::Cm::Core::Server::Server(0) ;
                    TryReturn( p_server != NULL, false, "Mem alloc failed" ) ;
                    this->p_server->addServerListener(*this);

                    bRet = this->p_server->initialize ();
                    TryReturn( bRet != false, false, "OCServer initialization failed" ) ;

                    return true ;
                }

                bool
                CMEngine::deinit()
                {
                    CM_LOG_DEBUG_A(CM_CORE_ENGINE, "%s", "Entered");
                    this->m_pConfiguration = NULL ;

                    if (pResourceHandlerFactory != NULL)
                    {
                        delete pResourceHandlerFactory ;
                        pResourceHandlerFactory = NULL ;
                    }

                    if (pSerializableDataFactory != NULL)
                    {
                        delete pSerializableDataFactory ;
                        pSerializableDataFactory = NULL ;
                    }


                    if (this->p_client != NULL)
                    {
                        delete this->p_client;
                        this->p_client = NULL;
                    }
                    CM_LOG_DEBUG_A(CM_CORE_ENGINE, "%s", "Stopped Server") ;
                    if (this->p_server != NULL)
                    {
                        delete this->p_server ;
                        this->p_server = NULL ;
                    }

                    CM_LOG_DEBUG_A(CM_CORE_ENGINE, "%s", "Exited");

                    return true ;
                }



                void
                CMEngine::setControlManagerListener(OC::Cm::IControlManagerListener &listener)
                {
                    m_pControlManagerListener = &listener ;
                }

                OC::Cm::IControlManagerListener *
                CMEngine::getControlManagerListener()
                {
                    return m_pControlManagerListener;
                }

                void
                CMEngine::removeControlManagerListener(OC::Cm::IControlManagerListener &listener)
                {
                    m_pControlManagerListener = NULL ;
                }

                bool
                CMEngine::setDeviceAddress(std::string &deviceAddress)
                {
                    m_deviceAddress = deviceAddress ;

                    OC::Cm::Core::CMContext *pCMContext = (OC::Cm::Core::CMContext *)p_context;
                    TryReturn(pCMContext != NULL, false, "Failed to get Control Manager Context Class Instance");
                    pCMContext->m_deviceAddress = deviceAddress ;

                    return true ;
                }

                void
                CMEngine::setDeviceType(OC::Cm::DeviceType deviceType)
                {
                    m_deviceType = deviceType;
                }

                std::string
                CMEngine::getUUID()
                {
                    return m_uuid;
                }

                void
                CMEngine::setUUID(const std::string &uuid)
                {
                    m_uuid = uuid;
                }

                void
                CMEngine::onServerStarted(int serverId, std::string address)
                {
                    CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "Server Started");

                    OC::Cm::Core::CMContext *pCMContext = (OC::Cm::Core::CMContext *)p_context ;
                    TryReturnVoid(pCMContext != NULL, "Failed to get Control Manager Context Class Instance");
                    TryReturnVoid((pCMContext->p_shp) != NULL, "Failed to get Control Manager Class Instance");
                    TryReturnVoid(m_pConfiguration != NULL,
                                  "Failed to get Control Manager Configuration Class Instance");

                    OC::Cm::CMError error ;

                    // TODO: Check Size of local endpoint

                    // Start DeviceFinder
                    bool bRet = pCMContext->m_deviceFinderImpl.init( m_pConfiguration->getDeviceFactory() ) ;
                    if (bRet == false)
                    {
                        CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "Failed to initialize devicefinder");

                        if (NULL != m_pControlManagerListener)
                        {
                            m_pControlManagerListener->onError(error.setErrorCode(
                                                                   CM_FAILURE).setErrorMessage("Failed to initialize DeviceFinder: SSDP"));
                        }

                        // TODO: Stop Server
                        setCMEngineState(CM_FAILED);
                        return ;
                    }

                    bRet = pCMContext->m_deviceFinderImpl.start(m_pConfiguration->getAppType(), m_deviceType ,
                            OIC_PROTOCOL_NAME, address, m_uuid, m_pConfiguration->getApplicationName(),
                            m_pConfiguration->getOSName()) ;
                    if (bRet == false)
                    {
                        CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "Failed to start device finder");

                        if (NULL != m_pControlManagerListener)
                        {
                            m_pControlManagerListener->onError(error.setErrorCode(
                                                                   CM_FAILURE).setErrorMessage("Failed to Start DeviceFinder: SSDP"));
                        }

                        // TODO: Stop Server
                        setCMEngineState(CM_FAILED);
                        return ;
                    }

                    if ( NULL != pCMContext->p_subscriptionManager )
                    {
                        pCMContext->p_subscriptionManager->setDeviceAddress(OIC_PROTOCOL_NAME, m_deviceAddress , m_uuid) ;
                    }

                    //bool raStatus = true;

                    if (m_pConfiguration->getAppType() != ApplicationType_Controllable)
                    {
                        //if (raStatus == true) {
                        setCMEngineState(CM_STARTED);
                        if (NULL != m_pControlManagerListener)
                        {
                            m_pControlManagerListener->onStarted(address) ;
                        }
                        //}
                        /*else {
                            setCMEngineState(CM_FAILED);
                            if (NULL != m_pControlManagerListener) {
                                m_pControlManagerListener->onError(error.setErrorCode(CM_REMOTE_CHANNEL_INITIALIZED_FAILED).setErrorMessage("Failed to initialize Remote Access Channel"));
                            }
                        }*/
                    }
                    else
                    {
                        setCMEngineState(CM_STARTED);
                        if (NULL != m_pControlManagerListener)
                        {
                            m_pControlManagerListener->onStarted(address) ;
                        }
                    }
                }

                void
                CMEngine::onServerStopped(int serverId)
                {
                    CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "Server Stopped");

                    OC::Cm::Core::CMContext *pCMContext = (OC::Cm::Core::CMContext *)p_context ;
                    TryReturnVoid(pCMContext != NULL, "Failed to get Control Manager Context Class Instance");
                    TryReturnVoid((pCMContext->p_shp) != NULL, "Failed to get Control Manager Class Instance");
                    TryReturnVoid(m_pConfiguration != NULL,
                                  "Failed to get Control Manager Configuration Class Instance");

                    setCMEngineState(CM_STOPPED);
                    if (NULL != m_pControlManagerListener)
                    {
                        m_pControlManagerListener->onStopped();
                    }
                }

                void
                CMEngine::onServerError(int serverId, OC::Cm::ErrorCode errorCode)
                {
                    CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "Server Error Occurred");
                    OC::Cm::CMError error ;

                    {
                        if (NULL != m_pControlManagerListener)
                        {
                            m_pControlManagerListener->onError(error.setErrorCode(
                                                                   CM_FAILURE).setErrorMessage("Failed to Start Server"));
                        }
                    }
                }

                void
                CMEngine::onServerSessionStarted(int serverId, OC::Cm::Connector::Server::ServerSession &session)
                {

                }

                void
                CMEngine::onServerSessionCompleted(int serverId, OC::Cm::Connector::Server::ServerSession &session)
                {

                }

                bool
                CMEngine::handleRequest( OC::Cm::Connector::Server::ServerSession &session )
                {
                    bool bRet = false ;
                    OC::Cm::Connector::ControlRequest &request                  = session.getRequest() ;
                    OC::Cm::Connector::Server::ServerSessionData &sessionData = session.getSessionData() ;

                    CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "Start");

                    std::string method      = request.m_methodIdentifier.getMethod() ;
                    TryReturnNoLog( method.empty() == false, false, sendErrorResponse(400, ERROR_CODE_INVALID_METHOD,
                                    "Invalid method", session););

                    std::string resourcePath    = request.m_resourceIdentifier.getResourcePath();
                    TryReturnNoLog( resourcePath.empty() == false, false, sendErrorResponse(400,
                                    ERROR_CODE_INVALID_RESOURCE_PATH, "Invalid resource path", session););

                    CM_LOG_INFO_A(CM_CORE_ENGINE, "Handling request on resource [%s, %s]", method.c_str(),
                                  resourcePath.c_str());

                    // Validate Accept_header
                    if (request.m_accept_header_values.size() > 0)
                    {
                        std::list<std::string>::iterator itr = request.m_accept_header_values.begin();

                        for (; itr != request.m_accept_header_values.end(); itr++)
                        {
                            std::string acceptValue = *itr;

                            if (acceptValue.find(CM_CONTENT_TYPE_JSON) != std::string::npos)
                                break;
                            else if (acceptValue.compare("*") == 0)
                                break;
                            else if (acceptValue.compare("*/*") == 0)
                                break;
                        }

                        TryReturnNoLog( (itr != request.m_accept_header_values.end()), false, sendErrorResponse(406,
                                        ERROR_CODE_NOT_ACCEPTABLE, "Accept Header Validation failed", session););
                    }

                    OC::Cm::Serialization::SerializableDataFactory *p_serializableFactory =
                        this->pSerializableDataFactory ;
                    TryReturnNoLog( p_serializableFactory != NULL, false, sendErrorResponse(500,
                                    ERROR_CODE_INTERNAL_SERVER_ERROR, "Failed to get SerializableDataFactory", session););

                    OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *p_resourceHandlerFactory =
                        pResourceHandlerFactory;
                    TryReturnNoLog( p_resourceHandlerFactory != NULL, false, sendErrorResponse(500,
                                    ERROR_CODE_INTERNAL_SERVER_ERROR, "Failed to getResourceHandlerFactory", session););

                    int resourceType = p_resourceHandlerFactory->getResourceType(resourcePath,
                                       session.getRequest().m_template_parameters) ;
                    TryReturnNoLog( resourceType >= 0, false, sendErrorResponse(404, ERROR_CODE_NOT_FOUND,
                                    "Resource Handler Not Found", session););

                    bRet = p_serializableFactory->createMethodSerializables(resourceType, method,
                            &(sessionData.m_pRequestPayload), &(sessionData.m_pResponsePayload)) ;
                    TryReturnNoLog( bRet == true, false, sendErrorResponse(404, ERROR_CODE_NOT_FOUND,
                                    "Failed to createMethodSerializables", session););

                    if (NULL != sessionData.m_pRequestPayload)
                    {
                        std::string errMsg ;


                        CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "Before getting JSON get value");
                        request.m_payload = request.m_representation.getValue<std::string>("json");
                        CM_LOG_INFO_A(CM_CORE_ENGINE, "After getting JSON value [%s]", request.m_payload.c_str());
                        request.m_payloadSize = request.m_payload.size();

                        int statusCode = deSerialize(request.m_payloadType, request.m_payload , request.m_payloadSize,
                                                     method,
                                                     request.m_methodIdentifier.m_queryParams, sessionData.m_pRequestPayload, errMsg) ;

                        std::string errorDescripton;
                        TryReturnNoLog( statusCode == 200, false, sendErrorResponse(statusCode,
                                        ERROR_CODE_DESERIALIZATION_FAILED, errorDescripton + "Failed to deSerialize" + errMsg , session););
                    }
                    else
                    {
                        // Ranjith : We will ignore the payload
                        // TryReturnEx( request.m_payloadSize == 0, false, sendResponse(415, NULL, session);, "Requested Resource and Method does not expect payload" );
                    }

                    OC::Cm::Server::ResourceHandler::IResourceHandler *p_resourceHandler =
                        p_resourceHandlerFactory->createResourceHandler(resourceType);
                    TryReturnNoLog(  p_resourceHandler != NULL, false, sendErrorResponse(404, ERROR_CODE_NOT_FOUND,
                                     "Resource Handler Not Found", session););

                    sessionData.m_pResourceHandler = p_resourceHandler ;

                    bRet = p_resourceHandler->handleResource(session, method, sessionData.m_pRequestPayload,
                            sessionData.m_pResponsePayload) ;
                    // TryReturnEx( bRet == true, false, sendResponse(400, NULL, session);, "Resource Handler Not Found");
                    TryReturnNoLog( bRet == true, false, sendErrorResponse(500, ERROR_CODE_INTERNAL_SERVER_ERROR,
                                    "Failed to process request", session););

                    return true;
                }


                bool
                CMEngine::sendResponse( int status_code,
                                        OC::Cm::Serialization::ISerializable *response,
                                        OC::Cm::Connector::Server::ServerSession &session)
                {
                    CM_LOG_INFO_A(CM_CORE_ENGINE, "Send Response invoked with status:%d", status_code);
                    OC::Cm::Serialization::Xsd::CMErrorMessage errorMessgae ;

                    if ((status_code >= 200) && (status_code < 300))
                    {

                    }
                    else
                    {
                        if (response != NULL)
                        {
                            if (response->getElementName().compare("error") != 0)
                            {
                                response = NULL;
                            }
                        }

                        if (response == NULL)
                        {
                            errorMessgae.mErrorCode = ERROR_CODE_GENERIC_ERROR;

                            if (status_code == 400)
                            {
                                errorMessgae.mErrorDescription = "Bad Request";
                            }
                            else if (status_code == 404)
                            {
                                errorMessgae.mErrorDescription = "Specified Resource Not found";
                            }
                            else if (status_code == 405)
                            {
                                errorMessgae.mErrorDescription = "Method Not Allowed";
                            }
                            else if (status_code == 501)
                            {
                                errorMessgae.mErrorDescription = "Method Not Implemented";
                            }
                            else
                            {
                                errorMessgae.mErrorDescription = "No description available";
                            }

                            response = &errorMessgae;
                        }
                    }

                    OC::Cm::Connector::ControlRequest &controlRequest       = session.getRequest();
                    OC::Cm::Connector::ControlResponse &controlResponse = session.getResponse();

                    std::string method = controlRequest.m_methodIdentifier.getMethod();
                    if (method.empty())
                    {
                        // Here Method id is used to check whether mandatory parameters are allowed or not... if method id is not available then we will use GET method
                        method.assign("GET");
                    }

                    controlResponse.m_response_code = status_code;
                    controlResponse.m_payloadType   = CM_CONTENT_TYPE_JSON_UTF8;
                    controlResponse.m_payloadSize   = 0;
                    controlResponse.setProtocolVersion(CM_VERSION_STRING);

                    if (response != NULL)
                    {
                        std::string m_pretty_print_payload ;
                        std::string errorMessage ;

                        if (false == serialize(response, method, &(controlRequest.m_methodIdentifier.m_queryParams),
                                               controlResponse.m_payloadType, controlResponse.m_payload, controlResponse.m_payloadSize,
                                               m_pretty_print_payload, errorMessage))
                        {
                            if (response->getElementName().compare("error") != 0)
                            {
                                return sendErrorResponse(500, ERROR_CODE_INTERNAL_SERVER_ERROR, errorMessage, session );
                            }
                            else
                            {
                                controlResponse.m_payloadSize   = 0 ;
                                controlResponse.m_response_code = 500 ;

                                sendResponse( session );
                                return false ;
                            }
                        }
                        controlResponse.m_representation.setValue("json", controlResponse.m_payload);
                    }

                    return sendResponse( session );
                }

                bool
                CMEngine::sendResponse( OC::Cm::Connector::Server::ServerSession &session )
                {
                    p_server->sendResponse(session);
                    return true;
                }

                bool
                CMEngine::sendErrorResponse( int status_code, std::string errorCode, std::string errorDescription,
                                             OC::Cm::Connector::Server::ServerSession &session )
                {
                    CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "sendErrorResponse invoked");
                    CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", errorDescription.c_str() );

                    OC::Cm::Serialization::Xsd::CMErrorMessage errorMessage(errorCode, errorDescription);

                    return sendResponse(status_code, &errorMessage, session);
                }

                bool
                validateResourcePath(std::string &resourcePath,
                                     std::map<std::string, std::string> &resourcePathParams)
                {
                    size_t n_pos = resourcePath.find("http://");
                    if (std::string::npos != n_pos)
                    {
                        return false;
                    }

                    n_pos = resourcePath.find("https://");
                    if (std::string::npos != n_pos)
                    {
                        return false;
                    }

                    n_pos = resourcePath.find("coap://");
                    if (std::string::npos != n_pos)
                    {
                        return false;
                    }

                    n_pos = resourcePath.find("://");
                    if (std::string::npos != n_pos)
                    {
                        return false;
                    }

                    n_pos = resourcePath.find("?");
                    if (std::string::npos != n_pos)
                    {
                        std::string path = resourcePath.substr(0, n_pos);

                        size_t firstIndex = n_pos + 1 ;

                        if (firstIndex < resourcePath.length())
                        {
                            size_t lastIndex = 0 ;

                            std::string queryParams = resourcePath.substr(firstIndex);

                            firstIndex = 0;
                            while (lastIndex != std::string::npos)
                            {
                                lastIndex = queryParams.find('&', firstIndex);

                                std::string token ;
                                if ( std::string::npos == lastIndex )
                                {
                                    token = queryParams.substr(firstIndex);
                                }
                                else
                                {
                                    token = queryParams.substr(firstIndex, lastIndex);
                                }

                                size_t index = token.find("=") ;

                                if ( index != std::string::npos )
                                {
                                    std::string paramName = token.substr(0, index);
                                    std::string paramValue = token.substr(index + 1) ;

                                    resourcePathParams.insert(std::pair<std::string, std::string>(paramName, paramValue));
                                }

                                firstIndex = lastIndex + 1 ;
                            }
                        }

                        resourcePath = path;
                    }

                    return true;
                }

                OC::Cm::CMError
                CMEngine::sendClientRequest(    std::string &protocol,
                                                std::string &deviceAddress,
                                                std::string &resourcePath,
                                                std::string &method,
                                                std::map<std::string, std::string> *queryParams,
                                                OC::Cm::Serialization::ISerializable  *requestData,
                                                int &clientSessionId,
                                                OC::Cm::Client::Resource::IResourceResponseListener &listener,
                                                bool forceApplicationSerializables)
                {
                    CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "Sending Client Request");
                    CM_LOG_DEBUG_A(CM_CORE_ENGINE, "Protocol [%s] DA [%s], resourceId [%s], methodId [%s]",
                                   protocol.c_str(), deviceAddress.c_str(), resourcePath.c_str(), method.c_str());

                    OC::Cm::CMError error ;
                    OC::Cm::Core::CMContext *pCMContext = (OC::Cm::Core::CMContext *)p_context ;
                    if (pCMContext == NULL)
                    {
                        CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", "Failed to get Control Manager Context Class Instance");
                        return error.setErrorCode(
                                   CM_INVALID_INPUT).setErrorMessage("Failed to get Control Manager Context Class Instance");
                    }

                    if ((pCMContext->p_shp) == NULL)
                    {
                        CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", "Failed to get Control Manager Class Instance");
                        return error.setErrorCode(
                                   CM_INVALID_INPUT).setErrorMessage("Failed to get Control Manager Class Instance");
                    }

                    OC::Cm::CMStates state = getCMEngineState();
                    CM_LOG_DEBUG_A(CM_API, "Control Manager State [%d]", state);

                    if (state == CM_STOPPING)
                    {
                        CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", "Control Manager is in Stopping State.");
                        return error.setErrorCode(CM_IN_STOPPING).setErrorMessage("Control Manager is in Stopping State");
                    }

                    std::map<std::string, std::string> resourcePathParams ;

                    if ((deviceAddress.empty() == true) || (protocol.empty() == true) ||
                        (resourcePath.empty() == true) || (method.empty() == true))
                    {
                        CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", "One/More Input Parameters are Empty");
                        return error.setErrorCode(CM_INVALID_INPUT).setErrorMessage("One/More Input Parameters are Empty") ;
                    }

                    if ( false == validateResourcePath(resourcePath, resourcePathParams) )
                    {
                        return error.setErrorCode(CM_INVALID_INPUT).setErrorMessage("Resource Path must be a relative") ;
                    }

                    OC::Cm::Connector::Client::ClientSession *pClientSession = this->p_client->createNewSession(0) ;
                    if (pClientSession == NULL)
                    {
                        CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", "Failed to create client session");
                        return error.setErrorCode(
                                   CM_MEMORY_ALLOCATION_FAILED).setErrorMessage("Failed to create client session") ;
                    }

                    pClientSession->forceApplicationSerializables = forceApplicationSerializables ;

                    OC::Cm::Connector::ControlRequest &controlRequest = pClientSession->getRequest() ;

                    controlRequest.m_methodIdentifier.setDeviceAddress(deviceAddress);

                    if (NULL != queryParams)
                    {
                        controlRequest.m_methodIdentifier.m_queryParams = *queryParams ;
                    }

                    for (std::map<std::string, std::string>::iterator iter = resourcePathParams.begin();
                         iter != resourcePathParams.end(); iter++)
                    {
                        controlRequest.m_methodIdentifier.m_queryParams.insert(std::pair<std::string, std::string>
                                (iter->first, iter->second));
                    }

                    if (requestData != NULL)
                    {
                        std::string errorMessage ;

                        if (false == serialize(requestData, method, queryParams, controlRequest.m_payloadType,
                                               controlRequest.m_payload, controlRequest.m_payloadSize, controlRequest.m_payload_pretty_print,
                                               errorMessage))
                        {
                            CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", "serialize failed");
                            return error.setErrorCode(CM_SERIALIZATION_FAILED).setErrorMessage(errorMessage) ;
                        }
                        controlRequest.m_representation.setValue("json", controlRequest.m_payload);
                    }
                    else
                    {

                        controlRequest.m_payloadSize = 0 ;
                    }

                    controlRequest.m_resourceIdentifier.setResourcePath(resourcePath);
                    controlRequest.m_methodIdentifier.setMethod(method);
                    controlRequest.setProtocolVersion(CM_VERSION_STRING);
                    controlRequest.m_accept_header_values.push_back(CM_CONTENT_TYPE_JSON_UTF8);

                    pClientSession->getSessionData().setResponseHandler(&listener);
                    clientSessionId = pClientSession->getSessionId() ;
                    cout << "\n AttributeMap Payload :" << controlRequest.m_payload;
                    if ( 0 != this->p_client->sendRequest( *pClientSession ) )
                    {
                        CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", "Failed to Send Request");
                        delete pClientSession ;
                        return error.setErrorCode(CM_FAILURE).setErrorMessage("Failed to Send Request") ;
                    }

                    //  p_mutex->unlock();
                    CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "Leave");
                    return error.setErrorCode(CM_SUCCESS).setErrorMessage("") ;
                }

                void
                CMEngine::onResponseReceived( OC::Cm::Connector::Client::ClientSession &client_sessioin )
                {
                    CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "onResponseReceived");

                    OC::Cm::Connector::ControlRequest &request  = client_sessioin.getRequest() ;
                    OC::Cm::Connector::ControlResponse &response    = client_sessioin.getResponse() ;
                    OC::Cm::Connector::Client::ClientSessionData &clientSessionData = client_sessioin.getSessionData() ;

                    std::string method = request.m_methodIdentifier.getMethod() ;
                    if ( method.empty() )
                    {
                        invokeErrorCallBack( CM_HTTP_INVALID_DATA_RECEIVED,  -1, NULL, client_sessioin,
                                             "Invalid HTTP Data Received");
                        return ;
                    }

                    std::string resourcePath = request.m_resourceIdentifier.getResourcePath();
                    if ( resourcePath.empty() )
                    {
                        invokeErrorCallBack( CM_HTTP_INVALID_DATA_RECEIVED, -1,  NULL, client_sessioin,
                                             "Invalid HTTP Data Received");
                        return ;
                    }

                    CM_LOG_INFO_A(CM_CORE_ENGINE, "Resource Path is [%s]", resourcePath.c_str());
                    CM_LOG_INFO_A(CM_CORE_ENGINE, "Payload Type [%s] Payload Size [%d]\nPayload %s",
                                  response.m_payloadType.c_str(), response.m_payloadSize, (response.m_payload).c_str());

                    if ((response.m_response_code >= 200) && (response.m_response_code < 300))
                    {
                        OC::Cm::Serialization::SerializableDataFactory *p_serializableFactory = NULL ;
                        OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *p_resourceHandlerFactory = NULL ;

                        if (client_sessioin.forceApplicationSerializables)
                        {
                            p_serializableFactory       = m_pConfiguration->getSerializableDataFactory();
                            p_resourceHandlerFactory    = m_pConfiguration->getResourceHandlerFactory() ;
                        }
                        else
                        {
                            p_serializableFactory       = pSerializableDataFactory ;
                            p_resourceHandlerFactory    = pResourceHandlerFactory;
                        }

                        if ( p_serializableFactory == NULL )
                        {
                            invokeErrorCallBack( CM_DESERIALIZE_FACTORY_NOT_FOUND,  -1, NULL, client_sessioin,
                                                 "Deserializer factory not found");
                            return ;
                        }
                        TryReturnVoidEx( p_resourceHandlerFactory != NULL, invokeErrorCallBack( CM_DESERIALIZE_FAILED,  -1,
                                         NULL, client_sessioin, "Deserializer factory not found"); , "Failed to getResourceHandlerFactory");

                        int resourceType = p_resourceHandlerFactory->getResourceType(resourcePath,
                                           client_sessioin.getRequest().m_template_parameters) ;
                        TryReturnVoidEx( resourceType >= 0, invokeErrorCallBack( CM_DESERIALIZE_FAILED,  -1, NULL,
                                         client_sessioin, "Resource Handler Not Found"); , "Resource Handler Not Found");

                        bool bRet = p_serializableFactory->createMethodSerializables(resourceType, method,
                                    &(clientSessionData.m_pRequestPayload), &(clientSessionData.m_pResponsePayload)) ;
                        TryReturnVoidEx( bRet == true, invokeErrorCallBack( CM_DESERIALIZE_FAILED,  -1, NULL,
                                         client_sessioin, "Resource Handler Not Found"); , "Failed to createMethodSerializables");

                        if ( NULL != clientSessionData.m_pResponsePayload )
                        {
                            std::string errorMessage ;

                            if ( 200 != deSerialize( response.m_payloadType, response.m_payload, response.m_payloadSize, method,
                                                     request.m_methodIdentifier.m_queryParams, clientSessionData.m_pResponsePayload, errorMessage) )
                            {
                                invokeErrorCallBack( CM_DESERIALIZE_FAILED,  -1, NULL, client_sessioin, errorMessage);
                                return ;
                            }
                        }
                        else
                        {
                            //      Ranjith: We will ignore the payload
                            //      if (response.m_payloadSize != 0)
                            //      {
                            //          // payload is received where payload is not expected...
                            //          invokeErrorCallBack( CM_DESERIALIZE_FAILED, client_sessioin);
                            //      }
                        }

                        invokeResponseCallBack( method, client_sessioin, response.m_response_code,
                                                clientSessionData.m_pResponsePayload );
                    }
                    else
                    {
                        // We might have received some json error response... lets parse it..

                        OC::Cm::Serialization::ISerializable *pErrorResponse = NULL ;
                        bool isSuccess = false ;

                        if (response.m_payloadSize > 0)
                        {
                            OC::Cm::Serialization::SerializableDataFactory *p_serializableFactory = NULL ;
                            std::string elementTYpe = "error";

                            if (client_sessioin.forceApplicationSerializables)
                            {
                                p_serializableFactory       = m_pConfiguration->getSerializableDataFactory();
                            }
                            else
                            {
                                p_serializableFactory       = pSerializableDataFactory ;
                            }

                            if (p_serializableFactory != NULL)
                            {
                                p_serializableFactory->createSerializable(elementTYpe, &pErrorResponse);
                            }

                            if (NULL != pErrorResponse)
                            {
                                std::string errorMessage ;

                                if ( 200 == deSerialize( response.m_payloadType, response.m_payload, response.m_payloadSize, method,
                                                         request.m_methodIdentifier.m_queryParams, pErrorResponse, errorMessage) )
                                {
                                    isSuccess = true;
                                }
                            }
                        }

                        if (isSuccess)
                        {
                            CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "invokeErrorCallBack");
                            invokeErrorCallBack(CM_SERVER_ERROR, response.m_response_code, pErrorResponse, client_sessioin,
                                                "Server has Sent error response");
                        }
                        else
                        {
                            invokeErrorCallBack(CM_SERVER_ERROR, response.m_response_code, NULL, client_sessioin,
                                                "Server has Sent error response");
                        }

                        if ( NULL != pErrorResponse )
                        {
                            delete pErrorResponse;
                        }
                    }
                }

                void
                CMEngine::onErrorRecieved( OC::Cm::ErrorCode error_code,
                                           OC::Cm::Connector::Client::ClientSession &client_sessioin)
                {
                    CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "onErrorRecieved");

                    invokeErrorCallBack( error_code, -1, NULL, client_sessioin, "Network/HTTP Related error" );

                }

                void
                CMEngine::onClientSessionCompleted( OC::Cm::Connector::Client::ClientSession &client_sessioin )
                {

                }

                void
                CMEngine::invokeErrorCallBack( OC::Cm::ErrorCode error_code, int status_code,
                                               OC::Cm::Serialization::ISerializable *errorResponse,
                                               OC::Cm::Connector::Client::ClientSession &client_sessioin, std::string errorMessage)
                {
                    OC::Cm::Client::Resource::IResourceResponseListener *pResponseListener =
                        client_sessioin.getSessionData().getResponseHandler();
                    if (pResponseListener != NULL)
                    {
                        OC::Cm::CMError error ;

                        pResponseListener->onErrorReceived(client_sessioin.getSessionId(), status_code,
                                                           error.setErrorCode(error_code).setErrorMessage(errorMessage), errorResponse) ;

                        pResponseListener->onRequestComplete(client_sessioin.getSessionId(),
                                                             client_sessioin.getRequest().m_headers_str + client_sessioin.getRequest().m_payload,
                                                             client_sessioin.getResponse().m_headers_str + client_sessioin.getResponse().m_payload) ;

                        client_sessioin.getSessionData().setResponseHandler(NULL);
                    }
                }

                void
                CMEngine::invokeResponseCallBack(   std::string &methodId,
                                                    OC::Cm::Connector::Client::ClientSession &client_sessioin,
                                                    int status_code, OC::Cm::Serialization::ISerializable *pResponse)
                {
                    OC::Cm::Client::Resource::IResourceResponseListener *pResponseListener =
                        client_sessioin.getSessionData().getResponseHandler();
                    if (pResponseListener != NULL)
                    {
                        if (0 == shp_strcasecmp(methodId.c_str(), "GET"))
                        {
                            pResponseListener->OnGetResponseReceived(client_sessioin.getSessionId() , status_code,
                                    client_sessioin.getResponse().m_headers, pResponse);
                        }
                        else if (0 == shp_strcasecmp(methodId.c_str(), "POST"))
                        {
                            pResponseListener->OnPostResponseReceived(client_sessioin.getSessionId(), status_code,
                                    client_sessioin.getResponse().m_headers, pResponse);
                        }
                        else if (0 == shp_strcasecmp(methodId.c_str(), "PUT"))
                        {
                            pResponseListener->OnPutResponseReceived(client_sessioin.getSessionId(), status_code,
                                    client_sessioin.getResponse().m_headers, pResponse);
                        }
                        else if (0 == shp_strcasecmp(methodId.c_str(), "DELETE"))
                        {
                            pResponseListener->onDeleteResponseReceived(client_sessioin.getSessionId(), status_code,
                                    client_sessioin.getResponse().m_headers, pResponse);
                        }

                        pResponseListener->onRequestComplete(client_sessioin.getSessionId(),
                                                             client_sessioin.getRequest().m_headers_str + client_sessioin.getRequest().m_payload,
                                                             client_sessioin.getResponse().m_headers_str + client_sessioin.getResponse().m_payload) ;
                        client_sessioin.getSessionData().setResponseHandler(NULL);
                    }
                }

                int
                CMEngine::deSerialize( std::string &contentType, std::string &payLoad, int payloadLen,
                                       std::string &method, std::map<std::string, std::string> &queryParams,
                                       OC::Cm::Serialization::ISerializable *data, std::string &errorMessage)
                {
                    CM_LOG_DEBUG_A(CM_CORE_ENGINE, "payloadLen [%d] contentType [%s]", payloadLen, contentType.c_str());
                    if ((payLoad.empty()) || (payloadLen == 0) || (NULL == data) || contentType.empty())
                    {
                        CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s",
                                       "Invalid Input parameters, payload or content type header is invalid");
                        return 400;
                    }

                    CM_LOG_DEBUG_A(CM_CORE_ENGINE, "%s", "Deserializing Payload");

                    OC::Cm::Serialization::IDeserializer *p_deserializer = NULL ;
#ifdef ISO_APP_NOTI_HANDLE
                    if (contentType.find(CM_CONTENT_TYPE_JSON) != std::string::npos
                        || contentType.find("applciation/json") != std::string::npos)
#else
                    if (contentType.find(CM_CONTENT_TYPE_OC) != std::string::npos
                        || contentType.find(CM_CONTENT_TYPE_JSON) != std::string::npos
                        || contentType.find("applciation/json") != std::string::npos)
#endif
                    {
                        p_deserializer = new OC::Cm::Core::Serialization::Json::JsonDeserializer(p_context);
                    }

                    if (p_deserializer == NULL)
                    {
                        return 415;
                    }

                    if (queryParams.find("filter") != queryParams.end())
                    {
                        std::string filter = queryParams["filter"];

                        if (filter.size() > 0)
                        {
                            p_deserializer->setFilter(filter);
                        }
                    }


                    if (queryParams.find("recursive") != queryParams.end())
                    {
                        std::string recursive = queryParams["recursive"];

                        if (0 == recursive.compare("false"))
                        {
                            p_deserializer->setRecursive(false);
                        }
                        else
                        {
                            p_deserializer->setRecursive(true);
                        }
                    }

                    if (0 == shp_strcasecmp(method.c_str(), "PUT"))
                    {
                        p_deserializer->setValidateMandatory(false) ;
                    }
                    else if (0 == shp_strcasecmp(method.c_str(), "POST"))
                    {
                        p_deserializer->setIDExcemption(true) ;
                    }

                    //TO DO : Need to pass OCRepresentation Object to parse api in place of 1st Argument. payloadLen is not being used - Vinay
                    if (false == p_deserializer->parse(payLoad, payloadLen))
                    {
                        CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", "Error in parsing json");
                        CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", p_deserializer->getErrorMessage().c_str());

                        errorMessage = p_deserializer->getErrorMessage();

                        delete p_deserializer;
                        return 400;
                    }


                    if (false == data->deSerialize( *p_deserializer ))
                    {
                        CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", "Error in deserialization");
                        CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", p_deserializer->getErrorMessage().c_str());

                        errorMessage = p_deserializer->getErrorMessage();

                        delete p_deserializer;
                        return 400;
                    }

                    delete p_deserializer ;

                    return 200 ;
                }

                bool
                CMEngine::serialize( OC::Cm::Serialization::ISerializable *data, std::string &method,
                                     std::map<std::string, std::string> *queryParams, std::string &contentType,
                                     std::string &payLoad, int &payloadLen, std::string &prettyPrintOutput, std::string &errorMessage)
                {
                    if (data == NULL)
                    {
                        return false ;
                    }

                    /* OIC : Using OCSerializer */
                    OC::Cm::Serialization::ISerializer *p_serializer = new
                    OC::Cm::Core::Serialization::Json::JsonSerializer();
                    if (p_serializer == NULL)
                    {
                        return false ;
                    }

                    if (NULL != queryParams)
                    {
                        if (queryParams->find("filter") != queryParams->end())
                        {
                            std::string filter = (*queryParams)["filter"];

                            if (filter.size() > 0)
                            {
                                p_serializer->setFilter(filter);
                            }
                        }

                        if (queryParams->find("recursive") != queryParams->end())
                        {
                            std::string recursive = (*queryParams)["recursive"];

                            if (0 == recursive.compare("false"))
                            {
                                p_serializer->setRecursive(false);
                            }
                            else
                            {
                                p_serializer->setRecursive(true);
                            }
                        }
                    }

                    if (0 == shp_strcasecmp(method.c_str(), "PUT"))
                    {
                        p_serializer->setValidateMandatory(false) ;
                    }
                    else if (0 == shp_strcasecmp(method.c_str(), "POST"))
                    {
                        p_serializer->setIDExcemption(true) ;
                    }

                    if (false == data->serialize(*p_serializer))
                    {
                        errorMessage = p_serializer->getErrorMessage();

                        CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", "Error in serialization");
                        CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", errorMessage.c_str());

                        delete p_serializer ;
                        return false;
                    }

                    //TODO : Attributemap needs to pass to getOutput() Api
                    if (false == p_serializer->getOutput(payLoad, payloadLen))
                    {
                        errorMessage = p_serializer->getErrorMessage();

                        CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", "Error in serialization");
                        CM_LOG_ERROR_A(CM_CORE_ENGINE, "%s", errorMessage.c_str());

                        delete p_serializer ;
                        return false;
                    }

                    cout << "\nPayload : " << payLoad << "\n";
                    if (false == p_serializer->getPrettyPrintOutput(prettyPrintOutput))
                    {
                        //delete p_serializer ;
                        //return false;
                    }

                    contentType = p_serializer->getContentType() ;
                    delete p_serializer ;

                    return true ;
                }

                OC::Cm::Serialization::SerializableDataFactory *
                CMEngine::getSerializableDataFactory()
                {
                    return pSerializableDataFactory ;
                }


            }
        }
    }
}
