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



#include <functional>

#include "OCClient.h"
#include "Utils.h"



#define MODULE_ID CM_CORE_CLIENT

namespace OC
{
    namespace Cm
    {
        namespace Core
        {
            namespace Client
            {

                OC::Cm::Connector::Client::IClientListener *OC::Cm::Core::Client::Client::p_client_listener = NULL;
                OC::Cm::Connector::SessionStore OC::Cm::Core::Client::Client::mSessionStore ;


                Client::Client()
                {
                    mIsStarted  = false;
                    OC::Cm::Core::Client::Client::p_client_listener = NULL ;

//  OC::Cm::Core::Client::Client::msID = 0 ;
                }

                Client::~Client()
                {
                    this->deinit() ;
                }

                bool
                Client::init(int clientId)
                {

                    return true;
                }

                bool
                Client::deinit()
                {
                    //TODO: removeClientListener() ;
                    OC::Cm::Core::Client::Client::p_client_listener = NULL ;

                    return true;
                }

                bool
                Client::start()
                {
                    if ( true == mIsStarted )
                    {
                        CM_LOG_DEBUG_A(MODULE_ID, "%s", "Already started... returning true");
                    }
                    else
                    {
                        // Take action to start anything if required
                    }

                    return mIsStarted ;
                }

                void
                Client::onClearSession( OC::Cm::Connector::Session &session )
                {
                    // TODO: (RANJITH) Check whether do we need to send any specific error code.... currently sending CM_FAILURE
//  this->onErrorRecieved(CM_FAILURE, (Sec::Shp::Connector::Client::ClientSession&) session) ;
                }

                bool
                Client::stop()
                {
                    bool bRet = false ;

                    if ( false == mIsStarted )
                    {
                        CM_LOG_DEBUG_A(MODULE_ID, "%s", "Not yet started. returning false.");
                    }
                    else
                    {
                        // stop event processing loop


                        // send error callback for all pending client sessions
                        // free all pending sessions ...
                        OC::Cm::Core::Client::Client::mSessionStore.clearStore( true, this );
                        mIsStarted = false;
                    }

                    return bRet;
                }

                std::string
                Client::getProtocol()
                {

                    return "";
                }

                void
                Client::addClientListener(OC::Cm::Connector::Client::IClientListener &listener)
                {
                    OC::Cm::Core::Client::Client::p_client_listener = &listener ;
                }

                OC::Cm::Connector::Client::ClientSession *
                Client::createNewSession(int clientSessionId)
                {
                    OC::Cm::Connector::Client::ClientSession *pClientSesson = new
                    OC::Cm::Connector::Client::ClientSession(clientSessionId);

                    if ( pClientSesson != NULL )
                    {
                        OC::Cm::Core::Client::Client::mSessionStore.addSession( pClientSesson );
                        CM_LOG_DEBUG_A(MODULE_ID, "Session Created Successfully with SessionID [%d]",
                                       pClientSesson->getSessionId());
                    }

                    return pClientSesson ;
                }

                int
                Client::sendRequest( OC::Cm::Connector::Client::ClientSession &session )
                {
                    int bRet = shp_err_codes:: CM_FAILURE;

                    OC::Cm::Connector::ControlRequest &controlRequest = session.getRequest() ;

                    // Construct the resource types and interfaces properly.
                    const std::vector<std::string> resourceTypes = {"clientresource"} ;
                    const std::vector<std::string> interfaces = {DEFAULT_INTERFACE};

                    std::string resourceURI = "coap://" + controlRequest.m_methodIdentifier.getDeviceAddress();
                    CM_LOG_DEBUG_A(MODULE_ID, "Client::sendRequest resourceURI [%s]", resourceURI.c_str());

                    std::string resourcePath = controlRequest.m_resourceIdentifier.getResourcePath();
                    CM_LOG_DEBUG_A(MODULE_ID, "Client::sendRequest resourcePath [%s]", resourcePath.c_str());


                    std::shared_ptr<OC::OCResource> resource = OC::OCPlatform::constructResourceObject(resourceURI,
                            resourcePath, false, resourceTypes, interfaces );

                    if (resource == NULL)
                    {
                        CM_LOG_ERROR_A(MODULE_ID, "%s", "Resource creation failed");
                        return OCStackResult::OC_STACK_ERROR;
                    }

                    HeaderOptions headerOptions;
                    this->prepareRequestHeaders(controlRequest, headerOptions);
                    resource->setHeaderOptions(headerOptions);

                    std::string requestType = controlRequest.m_methodIdentifier.getMethod();

                    OCStackResult result = OC_STACK_ERROR;

                    int sessionID = session.getSessionId();


                    if (requestType.compare("GET") == 0)
                    {
                        result = resource->get(controlRequest.m_methodIdentifier.m_queryParams,
                                               GetCallback(
                                                   std::bind(&Client::onResponse, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                                             sessionID)
                                               ));
                    }
                    else if (requestType.compare("PUT") == 0)
                    {
                        OC::OCRepresentation attributeMap = controlRequest.m_representation;
                        result = resource->put(attributeMap, controlRequest.m_methodIdentifier.m_queryParams,
                                               PutCallback(
                                                   std::bind(&Client::onResponse, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                                             sessionID)
                                               ));
                    }
                    else if (requestType.compare("POST") == 0)
                    {
                        CM_LOG_DEBUG_A(MODULE_ID, "%s", "Client::sendRequest POST not handled yet");
                        OC::OCRepresentation attributeMap = controlRequest.m_representation;
                        result = resource->post(attributeMap, controlRequest.m_methodIdentifier.m_queryParams,
                                                PostCallback(
                                                    std::bind(&Client::onResponse, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                                              sessionID)
                                                ));

                    }
                    else if (requestType.compare("DELETE") == 0)
                    {
                        CM_LOG_DEBUG_A(MODULE_ID, "%s", "Client::sendRequest DELETE is called");


                        result = resource->deleteResource(
                                     DeleteCallback(
                                         std::bind(&Client::onOCDelete, std::placeholders::_1, std::placeholders::_2, sessionID)
                                     ));

                    }

                    CM_LOG_DEBUG_A(MODULE_ID, "OC Error code [%d]", (int)result);

                    bRet = getCMErrorCodeFromOCResult (result);

                    if ( shp_err_codes::CM_SUCCESS != bRet )
                    {
                        OC::Cm::Core::Client::Client::mSessionStore.removeSession( &session );
                    }

                    CM_LOG_DEBUG_A(MODULE_ID, "Session ID [%d] Return Value [%d]", session.getSessionId(), bRet);
                    return bRet ;
                }

                void
                Client::onResponseReceived( OC::Cm::Connector::Client::ClientSession &client_sessioin )
                {
                    CM_LOG_DEBUG_A(MODULE_ID, "Response Received for Session ID [%d]", client_sessioin.getSessionId());
                    TryReturnVoid( OC::Cm::Core::Client::Client::p_client_listener != NULL, "Invalid client listener") ;
                    OC::Cm::Core::Client::Client::p_client_listener->onResponseReceived( client_sessioin );
                }

                void
                Client::onErrorRecieved( OC::Cm::ErrorCode error_code,
                                         OC::Cm::Connector::Client::ClientSession &client_sessioin)
                {
                    CM_LOG_DEBUG_A(MODULE_ID, "Error Received for Session ID [%d]", client_sessioin.getSessionId());
                    TryReturnVoid( OC::Cm::Core::Client::Client::p_client_listener != NULL, "Invalid client listener") ;
                    OC::Cm::Core::Client::Client::p_client_listener->onErrorRecieved(error_code, client_sessioin);
                }

                void
                Client::onClientSessionCompleted( OC::Cm::Connector::Client::ClientSession &client_sessioin )
                {
                    CM_LOG_DEBUG_A(MODULE_ID, "Client Session is Completed with Session ID [%d]",
                                   client_sessioin.getSessionId());
                    OC::Cm::Core::Client::Client::mSessionStore.removeSession( &client_sessioin ) ;
                    delete &client_sessioin ;
                }

                void
                Client::onResponse(const HeaderOptions &headerOptions, const OC::OCRepresentation &ocResource,
                                   const int eCode, int requestID)
                {
                    OC::Cm::Connector::Client::ClientSession *clientSession =
                        (OC::Cm::Connector::Client::ClientSession *)
                        OC::Cm::Core::Client::Client::mSessionStore.getSession(requestID);

                    TryReturnVoid( clientSession != NULL, "clientSession is NULL") ;

                    int HTTPErrorCode = getHTTPErrorCodeFromOCResult (eCode);

                    if (HTTPErrorCode != OC_OTHER_ERROR)
                    {
                        OC::Cm::Connector::ControlRequest &request  = clientSession->getRequest();
                        OC::Cm::Connector::ControlResponse &response = clientSession->getResponse();

                        // Add OC Response headers to Control Response
                        prepareResponseHeaders(headerOptions, response);

                        response.m_methodIdentifier.setMethod(request.m_methodIdentifier.getMethod());
                        response.m_payloadType = "application/oc";
                        response.m_resourceIdentifier.setResourcePath(ocResource.getUri());
                        response.m_response_code = HTTPErrorCode; //200;//eCode;

                        std::string method = request.m_methodIdentifier.getMethod();

                        if (method.compare("POST") == 0)
                        {
                            if (HTTPErrorCode == HTTP_OK || HTTPErrorCode == HTTP_CREATED)
                            {
                                response.m_response_code = HTTP_CREATED;
                            }
                        }
                        else if (method.compare("PUT") == 0)
                        {
                            if (HTTPErrorCode == HTTP_OK || HTTPErrorCode == HTTP_NO_CONTENT)
                            {
                                response.m_response_code = HTTP_NO_CONTENT;
                            }
                        }


                        response.m_representation = ocResource;

                        CM_LOG_INFO_A(CM_CORE_ENGINE, "%s", "Before getting JSON get value");
                        response.m_payload = response.m_representation.getValue<std::string>("json");
                        CM_LOG_INFO_A(CM_CORE_ENGINE, "After getting JSON value [%s]", response.m_payload.c_str());
                        response.m_payloadSize = response.m_payload.size();



                        CM_LOG_DEBUG_A(MODULE_ID, "Response Received for Session ID [%d]", clientSession->getSessionId());
                        TryReturnVoid( OC::Cm::Core::Client::Client::p_client_listener != NULL, "Invalid client listener") ;
                        OC::Cm::Core::Client::Client::p_client_listener->onResponseReceived( *clientSession );

                        CM_LOG_DEBUG_A(MODULE_ID, "%s", "response construction is successful: ");


                    }
                    else
                    {
                        CM_LOG_DEBUG_A(MODULE_ID, "%s", "Response error: ");
                        CM_LOG_DEBUG_A(MODULE_ID, "Response Received for Session ID [%d]", clientSession->getSessionId());
                        TryReturnVoid( OC::Cm::Core::Client::Client::p_client_listener != NULL, "Invalid client listener") ;

                        int cmCode = getCMErrorCodeFromOCResult ((OCStackResult)eCode);
                        OC::Cm::Core::Client::Client::p_client_listener->onErrorRecieved((OC::Cm::ErrorCode)cmCode,
                                *clientSession);

                    }

                    CM_LOG_DEBUG_A(MODULE_ID, "Client Session is Completed with Session ID [%d]",
                                   clientSession->getSessionId());
                    OC::Cm::Core::Client::Client::mSessionStore.removeSession( clientSession ) ;

                    delete clientSession;
                }


                void
                Client::onOCDelete (const HeaderOptions &headerOptions, const int eCode, int requestID)
                {
                    CM_LOG_DEBUG_A(MODULE_ID, "%s", "onDelete is called ");
                    OC::Cm::Connector::Client::ClientSession *clientSession =
                        (OC::Cm::Connector::Client::ClientSession *)
                        OC::Cm::Core::Client::Client::mSessionStore.getSession(requestID);

                    TryReturnVoid( clientSession != NULL, "clientSession is NULL") ;

                    int HTTPErrorCode = getHTTPErrorCodeFromOCResult (eCode);

                    if (HTTPErrorCode != OC_OTHER_ERROR)
                    {

                        OC::Cm::Connector::ControlRequest &request  = clientSession->getRequest();
                        OC::Cm::Connector::ControlResponse &response = clientSession->getResponse();

                        response.m_methodIdentifier.setMethod(request.m_methodIdentifier.getMethod());
                        response.m_payloadType = "application/oc";
                        response.m_resourceIdentifier.setResourcePath(request.m_resourceIdentifier.getResourcePath());
                        response.m_response_code = HTTPErrorCode;//eCode;

                        if ((HTTPErrorCode == HTTP_OK) || (HTTPErrorCode == HTTP_NO_CONTENT))
                        {
                            response.m_response_code = HTTP_NO_CONTENT;
                        }

                        CM_LOG_DEBUG_A(MODULE_ID, "Response Received for Session ID [%d]", clientSession->getSessionId());
                        TryReturnVoid( OC::Cm::Core::Client::Client::p_client_listener != NULL, "Invalid client listener") ;
                        OC::Cm::Core::Client::Client::p_client_listener->onResponseReceived( *clientSession );

                        CM_LOG_DEBUG_A(MODULE_ID, "%s", "response construction is successful: ");

                    }
                    else
                    {
                        CM_LOG_DEBUG_A(MODULE_ID, "%s", "Response error: ");
                        TryReturnVoid( OC::Cm::Core::Client::Client::p_client_listener != NULL, "Invalid client listener") ;

                        int cmCode = getCMErrorCodeFromOCResult ((OCStackResult)eCode);
                        OC::Cm::Core::Client::Client::p_client_listener->onErrorRecieved((OC::Cm::ErrorCode)cmCode,
                                *clientSession);
                    }

                    CM_LOG_DEBUG_A(MODULE_ID, "Client Session is Completed with Session ID [%d]",
                                   clientSession->getSessionId());
                    OC::Cm::Core::Client::Client::mSessionStore.removeSession( clientSession ) ;

                    delete clientSession;
                }


                bool Client::prepareResponseHeaders(const HeaderOptions &headerOptions,
                                                    OC::Cm::Connector::ControlResponse &response)
                {
                    for (auto it = headerOptions.begin(); it != headerOptions.end(); ++it)
                    {
                        if (it->getOptionID() == OIC_VERSION_HEADER)
                        {
                            std::string value = it->getOptionData();
                            response.setProtocolVersion(value.c_str());
                            CM_LOG_DEBUG_A(MODULE_ID, "prepareResponseHeaders [%d] : [%s]", OIC_VERSION_HEADER, value.c_str());
                        }
                        else if (it->getOptionID() == OIC_LOCATION_HEADER)
                        {
                            std::string value = it->getOptionData();
                            response.m_headers.setHeader("location", value);
                            CM_LOG_DEBUG_A(MODULE_ID, "prepareResponseHeaders [%d] : [%s]", OIC_LOCATION_HEADER, value.c_str());
                        }
                    }

                    return true;
                }

                bool Client::prepareRequestHeaders(OC::Cm::Connector::ControlRequest &request,
                                                   HeaderOptions &headerOptions)
                {
                    std::string version;
                    request.getProtocolVersion(version);
                    CM_LOG_DEBUG_A(MODULE_ID, "prepareRequestHeaders [%d] : [%s]", OIC_VERSION_HEADER, version.c_str());
                    OC::HeaderOption::OCHeaderOption apiVersion(OIC_VERSION_HEADER, version);
                    headerOptions.push_back(apiVersion);
                    return true;
                }
            }
        }
    }
}
