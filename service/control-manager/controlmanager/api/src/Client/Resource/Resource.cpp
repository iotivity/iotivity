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



#include "CMNotification.h"
#include "CMClientResource.h"
#include "CMSerialization.h"
#include "CMContext.h"
#include "Utils.h"
#include <stdarg.h>

namespace OC
{
    namespace Cm
    {
        namespace Client
        {
            namespace Resource
            {
                Resource::Resource(IContext *context, std::string protocol, std::string &baseUrl,
                                   std::string &resourceId)
                {
                    m_deviceAddress = baseUrl ;
                    m_resourceId    = resourceId ;
                    p_context       = context ;
                    m_protocol      = protocol ;

                    recursive       = true ;
                    forceApplicationSerializables = false;
                }

                Resource::~Resource()
                {

                }

                Resource::Resource(IContext *context, std::string protocol, std::string &baseUrl,
                                   const std::string &resourceId)
                {
                    m_deviceAddress = baseUrl ;
                    m_resourceId    = resourceId ;
                    p_context       = context ;
                    m_protocol      = protocol ;

                    recursive       = true ;
                    forceApplicationSerializables = false;
                }

                Resource::Resource(IContext *context, std::string protocol, std::string &baseUrl )
                {
                    m_deviceAddress = baseUrl ;
                    p_context       = context ;
                    m_protocol      = protocol ;

                    recursive       = true ;
                    forceApplicationSerializables = false;
                }

                void
                Resource::buildTemplatePath( const char *pathTemplate, ... )
                {
                    va_list args;
                    va_start(args, pathTemplate);

                    std::string resourcePath = "" ;

                    std::list<std::string> pathTokens = getStringTokens(pathTemplate, '/') ;

                    std::list<std::string>::iterator pathtorkenIterator = pathTokens.begin() ;

                    for (; ((pathtorkenIterator != pathTokens.end())); pathtorkenIterator++)
                    {
                        std::string pToken = *(pathtorkenIterator) ;

                        if (pToken.empty() == false)
                        {
                            if ((pToken.at(0) == '{') && (pToken.at(pToken.length() - 1) == '}'))
                            {
                                const char *currentPath = va_arg(args, const char *) ;
                                resourcePath += ( "/" + std::string(currentPath) ) ;
                            }
                            else
                            {
                                resourcePath += ( "/" + pToken ) ;
                            }
                        }
                    }

                    m_resourceId = resourcePath;

                    va_end(args);
                }

                OC::Cm::CMError Resource::get( int &requestId, std::map<std::string, std::string> *queryParams,
                                               OC::Cm::Client::Resource::IResourceResponseListener &listener)
                {
                    std::string method = "GET" ;

                    std::map<std::string, std::string> params   ;

                    if (queryParams != NULL)
                    {
                        params = *queryParams ;
                    }

                    if (m_filter.size() != 0)
                    {
                        params.insert(std::pair<std::string, std::string>("filter", m_filter));
                    }

                    if (recursive == false)
                    {
                        params.insert(std::pair<std::string, std::string>("recursive", "false"));
                    }

                    OC::Cm::Core::CMContext *pContext = (OC::Cm::Core::CMContext *)p_context ;
                    return pContext->m_engine.sendClientRequest(m_protocol, m_deviceAddress, m_resourceId, method,
                            &params, NULL, requestId, listener, forceApplicationSerializables) ;
                }

                OC::Cm::CMError Resource::put( int &requestId, std::map<std::string, std::string>   *queryParams,
                                               OC::Cm::Serialization::ISerializable *data,
                                               OC::Cm::Client::Resource::IResourceResponseListener &listener)
                {
                    std::string method = "PUT" ;

                    std::map<std::string, std::string> params   ;

                    if (queryParams != NULL)
                    {
                        params = *queryParams ;
                    }

                    if (m_filter.size() != 0)
                    {
                        params.insert(std::pair<std::string, std::string>("filter", m_filter));
                    }

                    if (recursive == false)
                    {
                        params.insert(std::pair<std::string, std::string>("recursive", "false"));
                    }

                    OC::Cm::Core::CMContext *pContext = (OC::Cm::Core::CMContext *)p_context ;
                    return pContext->m_engine.sendClientRequest(m_protocol, m_deviceAddress, m_resourceId, method,
                            &params, data, requestId, listener, forceApplicationSerializables) ;
                }

                OC::Cm::CMError Resource::post( int &requestId, std::map<std::string, std::string> *queryParams,
                                                OC::Cm::Serialization::ISerializable *data,
                                                OC::Cm::Client::Resource::IResourceResponseListener &listener)
                {
                    std::string method = "POST" ;

                    std::map<std::string, std::string> params   ;

                    if (queryParams != NULL)
                    {
                        params = *queryParams ;
                    }

                    if (m_filter.size() != 0)
                    {
                        params.insert(std::pair<std::string, std::string>("filter", m_filter));
                    }

                    if (recursive == false)
                    {
                        params.insert(std::pair<std::string, std::string>("recursive", "false"));
                    }

                    OC::Cm::Core::CMContext *pContext = (OC::Cm::Core::CMContext *)p_context ;
                    return pContext->m_engine.sendClientRequest(m_protocol, m_deviceAddress, m_resourceId, method,
                            &params, data, requestId, listener, forceApplicationSerializables) ;
                }

                OC::Cm::CMError Resource::deleteResource( int &requestId,
                        std::map<std::string, std::string> *queryParams,
                        OC::Cm::Client::Resource::IResourceResponseListener &listener)
                {
                    std::string method = "DELETE" ;

                    std::map<std::string, std::string> params   ;

                    if (queryParams != NULL)
                    {
                        params = *queryParams ;
                    }

                    if (m_filter.size() != 0)
                    {
                        params.insert(std::pair<std::string, std::string>("filter", m_filter));
                    }

                    if (recursive == false)
                    {
                        params.insert(std::pair<std::string, std::string>("recursive", "false"));
                    }

                    OC::Cm::Core::CMContext *pContext = (OC::Cm::Core::CMContext *)p_context ;
                    return pContext->m_engine.sendClientRequest(m_protocol, m_deviceAddress, m_resourceId, method,
                            &params, NULL, requestId, listener, forceApplicationSerializables) ;
                }

                OC::Cm::CMError Resource::subscribe(    int &requestId, std::string &uuid, std::string &suuid,
                                                        OC::Cm::Client::Resource::IResourceResponseListener &listener, std::string description )
                {
                    OC::Cm::Core::CMContext *pContext = (OC::Cm::Core::CMContext *)p_context ;
                    OC::Cm::CMError error ;

                    if (pContext->p_subscriptionManager == NULL)
                    {
                        return error.setErrorCode(
                                   CM_FC_UNINITIALIZED).setErrorMessage("Subscription Manager is not available") ;
                    }

                    std::list<std::string> resourcePaths ;
                    resourcePaths.push_back( m_resourceId );

                    if (false == pContext->p_subscriptionManager->addSubscription( requestId, m_protocol, uuid,
                            m_deviceAddress, suuid, resourcePaths, "", &listener, description ))
                    {
                        return error.setErrorCode(CM_FAILURE).setErrorMessage("Failed") ;
                    }
                    else
                    {
                        return error.setErrorCode(CM_SUCCESS).setErrorMessage("") ;
                    }
                }

                OC::Cm::CMError Resource::subscribe(int &requestId,
                                                    OC::Cm::Client::Resource::IResourceResponseListener &listener, std::string description )
                {
                    OC::Cm::Core::CMContext *pContext = (OC::Cm::Core::CMContext *)p_context ;
                    OC::Cm::CMError error ;

                    OC::Cm::Core::DeviceFinder::Impl::DeviceFinderImpl &deviceFinder = pContext->m_deviceFinderImpl ;
                    OC::Cm::Device *device = deviceFinder.getDeviceByAddress(this->m_deviceAddress);
                    if (device == NULL)
                    {
                        return error.setErrorCode(CM_FAILURE).setErrorMessage("Failed") ;
                    }

                    std::string receiverUUID = device->getUUID() ;
                    std::string selfUUID = pContext->m_engine.getUUID();

                    error = subscribe (requestId, receiverUUID, selfUUID, listener, description);

                    return error;
                }

                OC::Cm::CMError Resource::unSubscribe(  int &requestId, std::string &uuid,
                                                        OC::Cm::Client::Resource::IResourceResponseListener &listener )
                {
                    OC::Cm::Core::CMContext *pContext = (OC::Cm::Core::CMContext *)p_context ;
                    OC::Cm::CMError error ;

                    if (pContext->p_subscriptionManager == NULL)
                    {
                        return error.setErrorCode(
                                   CM_FC_UNINITIALIZED).setErrorMessage("Subscription Manager is not available") ;
                    }

                    std::list<std::string> resourcePaths ;
                    resourcePaths.push_back( m_resourceId );

                    std::map< int, std::list<std::string> > requestIDMap ;

                    bool bRet = pContext->p_subscriptionManager->removeSubscription( uuid, m_deviceAddress, m_protocol,
                                resourcePaths, &listener, requestIDMap ) ;
                    if (bRet)
                    {
                        requestId = requestIDMap.begin()->first;
                        return error.setErrorCode(CM_SUCCESS).setErrorMessage("") ;
                    }
                    else
                    {
                        return error.setErrorCode(CM_FAILURE).setErrorMessage("Failed") ;
                    }
                }

                OC::Cm::CMError Resource::unSubscribe(int &requestId,
                                                      OC::Cm::Client::Resource::IResourceResponseListener &listener )
                {
                    OC::Cm::Core::CMContext *pContext = (OC::Cm::Core::CMContext *)p_context ;
                    OC::Cm::CMError error ;

                    OC::Cm::Core::DeviceFinder::Impl::DeviceFinderImpl &deviceFinder = pContext->m_deviceFinderImpl ;
                    OC::Cm::Device *device = deviceFinder.getDeviceByAddress(this->m_deviceAddress);
                    if (device == NULL)
                    {
                        return error.setErrorCode(CM_FAILURE).setErrorMessage("Failed") ;
                    }

                    std::string receiverUUID = device->getUUID() ;

                    error = unSubscribe (requestId, receiverUUID, listener);

                    return error;
                }

                bool
                Resource::setFilter ( std::string filter )
                {
                    this->m_filter = filter ;

                    return true ;
                }


                bool
                Resource::unSetFilter ( )
                {
                    this->m_filter.assign("");

                    return true ;
                }


                bool
                Resource::setRecursive ( bool recursive )
                {
                    this->recursive = recursive ;

                    return true ;
                }


                std::string
                Resource::getResourcePath()
                {
                    return m_resourceId ;
                }

                void Resource::setResourcePath( std::string resourcePath )
                {
                    m_resourceId = resourcePath ;
                }
            }
        }
    }
}
