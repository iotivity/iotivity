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



#include "OCServer.h"

#include "CMConnector.h"
#include "CMServerConnector.h"

#include "CMCommon.h"


#define MODULE_ID CM_CORE_SERVER

namespace OC
{
    namespace Cm
    {
        namespace Core
        {
            namespace Server
            {

                OC::Cm::Connector::Server::IServerListener *Server::p_serverSessionListener = NULL ;

                Server::Server( int serverId )
                {
                    p_serverSessionListener     = NULL;
                    mServerId                   = serverId ;
                }

                Server::~Server()
                {
                    this->deinit();
                }

                int
                Server::getServerID()
                {
                    return mServerId ;
                }

                bool
                Server::initialize()
                {


                    if (registerOCResource() == false)
                    {

                        return false;
                    }

                    OC::OCPlatform::setDefaultDeviceEntityHandler(&Server::handleOCRequest) ;

                    return true;
                }

                bool
                Server::deinit()
                {
                    p_serverSessionListener = NULL;

                    return true;
                }

                bool
                Server::start()
                {
                    return true;
                }

                bool
                Server::stop()
                {
                    bool bRet = false ;

                    mSessionStore.clearStore( true, NULL );

                    return bRet;
                }

                void
                Server::addServerListener( OC::Cm::Connector::Server::IServerListener &listener )
                {
                    p_serverSessionListener = &listener ;
                }

                bool
                Server::sendResponse( OC::Cm::Connector::Server::ServerSession &session )
                {
                    OC::OCResourceResponse::Ptr ocResponse = session.m_pOCResponse ;

                    /**
                     * Filling the ocResponse from the shpResponse
                     */
                    OC::Cm::Connector::ControlResponse &controlResponse = session.getResponse();
                    ocResponse->setErrorCode(controlResponse.m_response_code);


                    /**
                     * Setting Header options
                     */
//  std::string version;
//  controlResponse.getProtocolVersion(version);
//
//  OC::HeaderOptions headerOptions;
//  // Set API version
//  OC::HeaderOption::OCHeaderOption apiVersion(OIC_HEADER_VERSION, version);
//  headerOptions.push_back(apiVersion);
//
//  ocResponse->setHeaderOptions(headerOptions);


                    //OC::OCRepresentation rep; // This is a local variable need to check if works, Do we need to allocate?
                    //rep.setAttributeMap(controlResponse.m_representation);

                    HeaderOptions headerOptions;
                    this->prepareResponseHeaders(controlResponse, headerOptions);
                    ocResponse->setHeaderOptions(headerOptions);

                    ocResponse->setResourceRepresentation(controlResponse.m_representation,
                                                          DEFAULT_INTERFACE); //Serializer will fill the OCRepresentation values ControlResponse of datamodel.

                    ocResponse->setResponseResult(OC_EH_OK);

                	OC::OCPlatform::sendResponse(ocResponse);
                    // TODO: support for sending headers
                    return true;
                }

                void
                Server::onServerStarted(int serverId, std::string address)
                {
                    TryReturnVoid( p_serverSessionListener != NULL, "Invalid server session listener" );

                    p_serverSessionListener->onServerStarted(serverId, address);
                }

                void
                Server::onServerStopped(int serverId )
                {
                    TryReturnVoid( p_serverSessionListener != NULL, "Invalid server session listener" );

                    p_serverSessionListener->onServerStopped(serverId);
                }

                void
                Server::onServerError(int serverId, OC::Cm::ErrorCode errorCode)
                {
                    TryReturnVoid( p_serverSessionListener != NULL, "Invalid server session listener" );

                    p_serverSessionListener->onServerError(serverId, errorCode);
                }

                bool
                Server::handleRequest( OC::Cm::Connector::Server::ServerSession &session )
                {
                    TryReturn( p_serverSessionListener != NULL, false, "Invalid server session listener" );

                    return p_serverSessionListener->handleRequest( session );
                }

                void
                Server::onServerSessionStarted(int serverId, OC::Cm::Connector::Server::ServerSession &session)
                {
                    mSessionStore.addSession( &session );
                }

                void
                Server::onServerSessionCompleted(int serverId, OC::Cm::Connector::Server::ServerSession &session)
                {
                    mSessionStore.removeSession( &session );
                }

                std::string
                Server::getProtocol()
                {
                    return "";
                }

                bool
                Server::registerOCResource(std::string resourceURI, std::string resourceName, bool isDiscoverable)
                {

                    bool bRet = false;
                    OCResourceHandle resourceHandle;
                    std::string resourceInterface = OC::DEFAULT_INTERFACE ;
                    uint8_t resourceProperty = OCResourceProperty::OC_ACTIVE;
                    std::string resourceTypeName = resourceName;


                    if (isDiscoverable == true)
                    {
                        resourceProperty = OCResourceProperty::OC_DISCOVERABLE;
                    }


                    OCStackResult result = OC::OCPlatform::registerResource(resourceHandle, resourceURI,
                                           resourceTypeName, resourceInterface,
                                           &Server::handleOCRequest, resourceProperty );

                    if (result == OCStackResult::OC_STACK_OK)
                    {
                        CM_LOG_DEBUG_A(MODULE_ID, "Registering %s is successful", resourceURI.c_str());
                        bRet = true;
                    }
                    else
                    {
                        CM_LOG_ERROR_A(MODULE_ID, "Registering %s failed", resourceURI.c_str());
                        bRet = false;
                    }

                    return bRet;
                }

// TODO: Is this a right place to register all the resources, Control Manager framework is doing it in resource handlers
// TODO: When new APIs from Intel available, register capability and default resource handler only
                bool
                Server::registerOCResource()
                {

                    bool bRet;

                    /**
                     * Used for Device discovery and Device Information
                     */
                    bRet = registerOCResource("/capability", "capability", true);   // Registering capability resource

                    return bRet;
                }



                OCEntityHandlerResult
                Server::handleOCRequest(const std::shared_ptr<OC::OCResourceRequest> ocRequest)
                {
                    if (!ocRequest)
                    {
                        CM_LOG_ERROR_A(MODULE_ID, "%s", "OCResourceRequest is null in Server::handleOCRequest");
                        return OC_EH_ERROR;
                    }
                    /**
                     * Initializing the ServerSession
                     */
                    int serverId = 0 ;
                    std::string clientProtocol = "coap";
                    std::string clientAddress = "127.0.0.1";
                    OC::OCRepresentation ocRep = ocRequest->getResourceRepresentation();

                    OC::Cm::Connector::Server::ServerSession *session = new OC::Cm::Connector::Server::ServerSession(
                        serverId, clientProtocol, clientAddress) ;

                    if (session == NULL)
                    {
                        CM_LOG_ERROR_A(MODULE_ID, "%s", "Failed to create ServerSession, no memory");
                        return OC_EH_ERROR ;
                    }

                    session->m_pOCRequest = ocRequest;
                	auto pResponse = std::make_shared<OC::OCResourceResponse>();
                    pResponse->setRequestHandle(ocRequest->getRequestHandle());
                    pResponse->setResourceHandle(ocRequest->getResourceHandle());
                	session->m_pOCResponse = pResponse;
                    /**
                     * constructing the controlRequest from ocRequest
                     */
                    OC::Cm::Connector::ControlRequest &controlRequest  = session->getRequest();


                    CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "Before getting JSON get value");
                    controlRequest.m_payload = controlRequest.m_representation.getValue<std::string>("json");
                    CM_LOG_INFO_A(CM_CORE_ENGINE, "After getting JSON value [%s]", controlRequest.m_payload.c_str());
                    controlRequest.m_payloadSize = controlRequest.m_payload.size();

                    controlRequest.m_methodIdentifier.setMethod(ocRequest->getRequestType());
                    controlRequest.m_payloadType = CM_CONTENT_TYPE_JSON_UTF8;

                    std::string uri = ocRequest->getResourceUri();
                    if (uri.empty())
                    {
                        // OIC TODO: remove this once Intel provides getResourceUri for both default and non default handler
                        controlRequest.m_resourceIdentifier.setResourcePath("/capability");
                    }
                    else
                    {
                        controlRequest.m_resourceIdentifier.setResourcePath(uri);
                    }

                    controlRequest.m_methodIdentifier.setDeviceAddress(clientAddress);
                    controlRequest.m_representation = ocRep
                                                      ; // Deserializer will convert the attributeMap values and fill the controlRequest

                    //OIC TODO: support headers and query

                    //  shpRequest.m_headers_str = "";
                    //  shpRequest.m_headers_str_length = 0;
                    //  shpRequest.m_accept_header_values = // Needs to check and fill it
                    //  shpRequest.m_payload_pretty_print = "";
                    //  session.m_request.m_template_parameters =
                    HeaderOptions headerOptions = ocRequest->getHeaderOptions();
                    prepareRequestHeaders(headerOptions, controlRequest);

                    /**
                     * Calling the handleRequest of listener(CMEngine) and filling the session values inside.
                     */
                    p_serverSessionListener->handleRequest(*session);

                    delete session;

                    return OC_EH_OK;

                }

                bool Server::prepareRequestHeaders(const HeaderOptions &headerOptions,
                                                   OC::Cm::Connector::ControlRequest &request)
                {
                    for (auto it = headerOptions.begin(); it != headerOptions.end(); ++it)
                    {
                        if (it->getOptionID() == OIC_VERSION_HEADER)
                        {
                            std::string value = it->getOptionData();
                            request.setProtocolVersion(value.c_str());
                            CM_LOG_DEBUG_A(MODULE_ID, "prepareRequestHeaders [%d] : [%s]", OIC_VERSION_HEADER, value.c_str());
                        }
                        else if (it->getOptionID() == OIC_LOCATION_HEADER)
                        {
                            std::string value = it->getOptionData();
                            request.m_headers.setHeader("location", value);
                            CM_LOG_DEBUG_A(MODULE_ID, "prepareRequestHeaders [%d] : [%s]", OIC_LOCATION_HEADER, value.c_str());
                        }
                    }

                    return true;
                }

                bool Server::prepareResponseHeaders(OC::Cm::Connector::ControlResponse &response,
                                                    HeaderOptions &headerOptions)
                {
                    std::string version;
                    response.getProtocolVersion(version);
                    CM_LOG_DEBUG_A(MODULE_ID, "prepareResponseHeaders [%d] : [%s]", OIC_VERSION_HEADER,
                                   version.c_str());
                    OC::HeaderOption::OCHeaderOption apiVersion(OIC_VERSION_HEADER, version);
                    headerOptions.push_back(apiVersion);
                    return true;
                }



            }
        }
    }
}
