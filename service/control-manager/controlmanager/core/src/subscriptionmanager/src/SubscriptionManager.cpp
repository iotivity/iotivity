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



#include "xsd/Subscription.h"
#include "xsd/Subscriptions.h"
#include "xsd/Notification.h"

#include "SubscriptionManager.h"
#include "SubscriptionRecord.h"

#include "sstream"
#include "ControlManager.h"
#include "ISubscriptionDB.h"
#include "CMError.h"

#include "XsList.h"
#include "ResourceHandlers/SubscriptionResourceHandlerFactory.h"
#include "Serialization/SubscriptionSerializationFactory.h"

#include "ctime"
#include "Utils.h"
#include "Log.h"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            int SubscriptionManager::s_subscriptionIdCounter ;

            std::string getCurrentTimeInISO8601()
            {
                time_t currentTime ;
                time(&currentTime) ;


                std::string retValue ;
                char outBuffer[50] = {0x00,} ;
                strftime(outBuffer, sizeof(outBuffer), "%Y-%m-%dT%H:%M:%S", gmtime(&currentTime));

                retValue.append(outBuffer);

                return outBuffer ;
            }

            OC::Cm::Device *
            SubscriptionManager::getDeviceByAddress(std::string &deviceAddress)
            {
                OC::Cm::ControlManager *pCM = OC::Cm::ControlManager::getInstance(m_pContext) ;
                OC::Cm::DeviceFinder::IDeviceFinder *pDeviceFinder = pCM->getDeviceFinder() ;

                return pDeviceFinder->getDeviceByAddress(deviceAddress) ;
            }

            OC::Cm::Device *
            SubscriptionManager::getDeviceByUUID(std::string &uuid)
            {
                OC::Cm::ControlManager *pCM = OC::Cm::ControlManager::getInstance(m_pContext) ;
                OC::Cm::DeviceFinder::IDeviceFinder *pDeviceFinder = pCM->getDeviceFinder() ;

                return pDeviceFinder->getDeviceByUUID(uuid) ;
            }

            std::string
            SubscriptionManager::getNotificationLink()
            {
                std::ostringstream uriStream ;

                uriStream <<  m_protocol << "://" << m_deviceAddress << "/notifications" ;

                return uriStream.str() ;
            }

            std::string
            SubscriptionManager::getSubscriptionsLink()
            {
                //return "/subscriptions" ;
                return "/sub" ;
            }

//bool isValidUrl(std::string url)
//{
//  size_t n_pos = url.find("http://");
//
//  if (std::string::npos != n_pos)
//  {
//      return true ;
//  }
//
//  n_pos = url.find("https://");
//
//  if (std::string::npos != n_pos)
//  {
//      return true;
//  }
//
//  return false ;
//}


//bool parseHTTPUrl( const std::string& url, std::string& protocol, std::string& host, std::string &port, std::string &path, std::string& host_header_value )
//{
//  size_t domainstart = 0;
//  size_t n_pos = url.find("http://");
//
//  if (std::string::npos == n_pos)
//  {
//      n_pos = url.find("https://");
//
//      if (std::string::npos == n_pos)
//      {
//          return false;
//      }
//      else
//      {
//          domainstart = n_pos + 8 ;
//          protocol = "https";
//      }
//  }
//  else
//  {
//      domainstart = n_pos + 7 ;
//      protocol = "http";
//  }
//
//  std::string domain_port_str ;
//  n_pos = url.find("/", domainstart);
//  if (std::string::npos == n_pos)
//  {
//      //path not found...
//      domain_port_str = url.substr(domainstart);
//      path.assign("/");
//
//  }
//  else
//  {
//      //path found...
//      domain_port_str = url.substr(domainstart, n_pos - domainstart);
//      path.assign(url.substr(n_pos));
//  }
//
//  host_header_value.assign(domain_port_str) ;
//
//  //trim ?..
//  n_pos = domain_port_str.find(":", domainstart);
//  if (std::string::npos == n_pos)
//  {
//      //port not found...
//      host.assign(domain_port_str);
//      port.assign("80");
//  }
//  else
//  {
//      host.append(domain_port_str.substr(0, n_pos));
//      port.assign(domain_port_str.substr(n_pos + 1));
//  }
//
//  return true ;
//}

            SubscriptionManager::SubscriptionManager(ISubscriptionDB *pDb)
            {
                this->m_pNotificationListener   = NULL ;
                this->m_pContext                = NULL ;
                this->m_deviceAddress           = "";
                this->m_pDatabase               = pDb ;
                this->m_pSubscriptionProvider   = NULL ;
            }

            SubscriptionManager::~SubscriptionManager()
            {
                // Close Subscription DB
            }

            bool
            SubscriptionManager::init( OC::Cm::IContext *pContext, std::string &dbFilePath )
            {
                this->m_pContext                = pContext ;
                return m_pDatabase->init( dbFilePath ) ;
            }

            bool
            SubscriptionManager::deinit()
            {
                return m_pDatabase->deinit() ;
            }

            bool
            SubscriptionManager::setDeviceAddress(std::string protocol, std::string address, std::string uuid)
            {
                this->m_deviceAddress = address ;
                this->m_protocol = protocol ;
                this->mSelfUUID = uuid ;
                return true;
            }

            OC::Cm::Serialization::SerializableDataFactory *
            SubscriptionManager::getSerializationFactory()
            {
                return new SubscriptionSerializationFactory();
            }

            OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *
            SubscriptionManager::getResourceHandlerFactory()
            {
                return new SubscriptionResourceHandlerFactory();
            }

            std::list< std::string >
            SubscriptionManager::getMySubscriptions(std::string uuid)
            {
                CM_LOG_DEBUG_A(CM_SUB_MGR, "%s", "Called");
                std::list<std::string> retList ;

                std::list<OC::Cm::Notification::SubscriptionRecord> subscriptionList ;
                if (false == m_pDatabase->getMySubscriptionByDeviceId(uuid, subscriptionList))
                {
                    return retList ;
                }

                for (std::list<OC::Cm::Notification::SubscriptionRecord>::const_iterator itr =
                         subscriptionList.begin(); itr != subscriptionList.end(); itr++)
                {
                    OC::Cm::Notification::SubscriptionRecord pSubscription = *(itr) ;

                    XsList strArray ;
                    strArray.setValue(pSubscription.m_subscribedResource);
                    std::list<std::string> tokens = strArray.getValueTokens();

                    for (std::list<std::string>::const_iterator tokenitr = tokens.begin(); tokenitr != tokens.end();
                         tokenitr++)
                    {
                        retList.push_back(*tokenitr);
                    }
                }

                return retList ;
            }

            void
            SubscriptionManager::OnGetResponseReceived( int clientSessionId, int status,
                    OC::Cm::Connector::HeadersList &headers,  OC::Cm::Serialization::ISerializable *response ) {}

            void
            SubscriptionManager::OnPutResponseReceived( int clientSessionId,  int status,
                    OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response )
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                std::map<int, SubscriptionRequest>::const_iterator itr = m_pResponseListeners.find(
                            clientSessionId );
                if ( itr == m_pResponseListeners.end() )
                {
                    // "Failed to find Subscription Request data"
                    return ;
                }

                SubscriptionRequest subReq = itr->second ;
                if ( subReq.m_reqType == SRT_UPDATE_SUBSCRIPTION )
                {
                    if ( status == 204 )
                    {
                        XsList strArray ;
                        for ( std::list<std::string>::iterator itr = subReq.m_remaining_resources.begin();
                              itr != subReq.m_remaining_resources.end(); itr++ )
                        {
                            strArray.addValueToken(*itr);
                        }

                        std::string resourceToStore ;
                        strArray.getValue(resourceToStore);
                        m_pDatabase->updateMySubscriptionBySubscriptionUri(subReq.m_subscription_uri, resourceToStore) ;
                    }

                    if ( subReq.m_pListener != NULL )
                    {
                        subReq.m_pListener->onUnSubscribeResponseReceived( clientSessionId, status );
                    }
                }
            }

            void
            SubscriptionManager::OnPostResponseReceived( int clientSessionId, int status,
                    OC::Cm::Connector::HeadersList &headers,  OC::Cm::Serialization::ISerializable *response )
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                std::map<int, SubscriptionRequest>::const_iterator itr = m_pResponseListeners.find(
                            clientSessionId );
                if ( itr == m_pResponseListeners.end() )
                {
                    // "Failed to find Subscription Request data"
                    return ;
                }

                SubscriptionRequest subReq = itr->second ;
                if ( subReq.m_reqType == SRT_ADD_SUBSCRIPTION )
                {
                    std::string locationField;

                    if ( status == 201 )
                    {
//          OC::Cm::Notification::SubscriptionRecord subscription ;
//
//          if ( false ==  m_pDatabase->getPendingMySubscription( clientSessionId, subscription ) )
//          {
//              if ( subReq.m_pListener != NULL )
//              {
//                  subReq.m_pListener->onErrorReceived( clientSessionId, OC::Cm::CM_SUBSCRIPTION_DB_RECORD_NOT_FOUND );
//              }
//
//              m_pDatabase->deletePendingMySubscription( clientSessionId );
//
//              return ;
//          }

                        if (( false == headers.getHeader( "location" , locationField)  || (locationField.empty())))
                        {
                            if ( subReq.m_pListener != NULL )
                            {
                                OC::Cm::CMError error ;
                                subReq.m_pListener->onErrorReceived( clientSessionId, -1,
                                                                     error.setErrorCode(OC::Cm::CM_SUBSCRIPTION_INVALID_RESPONSE) , NULL);
                            }

                            m_pDatabase->deletePendingMySubscription( clientSessionId );
                            return ;
                        }

                        m_pDatabase->updateMySubscription( clientSessionId, locationField ) ;
                    }

                    if ( subReq.m_pListener != NULL )
                    {
                        subReq.m_pListener->onSubscribeResponseReceived( clientSessionId, status, locationField );
                    }
                }
            }

            void
            SubscriptionManager::onDeleteResponseReceived( int clientSessionId, int status,
                    OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response )
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                std::map<int, SubscriptionRequest>::const_iterator itr = m_pResponseListeners.find(
                            clientSessionId );
                if ( itr == m_pResponseListeners.end() )
                {
                    // "Failed to find Subscription Request data"
                    return ;
                }

                SubscriptionRequest subReq = itr->second ;
                if ( subReq.m_reqType == SRT_REMOVE_SUBSCRIPTION )
                {
                    if (( status == 204 ) || ( status == 404 ))
                    {
                        m_pDatabase->deleteMySubscriptionBySubscriptionUri( subReq.m_uuid, subReq.m_subscription_uri );
                    }

                    if ( subReq.m_pListener != NULL )
                    {
                        subReq.m_pListener->onUnSubscribeResponseReceived( clientSessionId, status );
                    }
                }
            }

            void
            SubscriptionManager::onSubscribeResponseReceived(int clientSessionId, int status,
                    std::string &subscriptionUri ) {}
            void
            SubscriptionManager::onUnSubscribeResponseReceived(int clientSessionId, int status ) {}
            void
            SubscriptionManager::onNotifyResponseReceived(int clientSessionId, int status ) {}

            void
            SubscriptionManager::onErrorReceived( int clientSessionId, int status, OC::Cm::CMError error,
                                                  OC::Cm::Serialization::ISerializable *errorMessage )
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                std::map<int, SubscriptionRequest>::const_iterator itr = m_pResponseListeners.find(
                            clientSessionId );
                if ( itr == m_pResponseListeners.end() )
                {
                    // "Failed to find Subscription Request data"
                    return ;
                }

                SubscriptionRequest subReq = itr->second ;
                if ( subReq.m_reqType == SRT_ADD_SUBSCRIPTION )
                {
                    if ( subReq.m_pListener != NULL )
                    {
                        subReq.m_pListener->onErrorReceived( clientSessionId, status, error, errorMessage );
                    }

                    m_pDatabase->deletePendingMySubscription( clientSessionId );
                }
                else if ( subReq.m_reqType == SRT_REMOVE_SUBSCRIPTION )
                {
                    if ( subReq.m_pListener != NULL )
                    {
                        subReq.m_pListener->onErrorReceived( clientSessionId, status, error, errorMessage );
                    }
                }
                else if ( subReq.m_reqType == SRT_UPDATE_SUBSCRIPTION)
                {
                    if ( subReq.m_pListener != NULL )
                    {
                        subReq.m_pListener->onErrorReceived( clientSessionId, status, error, errorMessage );
                    }
                }

                if ( subReq.m_pListener != NULL )
                {
                    subReq.m_pListener->onErrorReceived( clientSessionId, status, error, errorMessage );
                }
            }

            void
            SubscriptionManager::onRequestComplete(int requestId, std::string requestPayload,
                                                   std::string responsePayload)
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                std::map<int, SubscriptionRequest>::const_iterator itr = m_pResponseListeners.find( requestId );
                if ( itr == m_pResponseListeners.end() )
                {
                    // "Failed to find Subscription Request data"
                    return ;
                }
                SubscriptionRequest subReq = itr->second ;

                if ( subReq.m_pListener != NULL )
                {
                    subReq.m_pListener->onRequestComplete(requestId, requestPayload, responsePayload) ;
                }

                m_pResponseListeners.erase(requestId);
            }

            bool
            SubscriptionManager::sendRequest(std::string &uri, std::string method,
                                             OC::Cm::Serialization::ISerializable *data, int &requestId )
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                std::string host ;
                std::string port;
                std::string path;
                std::string hostheader;
                std::string protocol ;

                if (false == parseUrl(uri, protocol, host, port, path, hostheader))
                {
                    return false;
                }

                return this->sendRequest(protocol,  hostheader, path, method, data, requestId ) ;
            }

            bool
            SubscriptionManager::sendRequest(std::string &protocol, std::string &deviceAddress,
                                             std::string &relativeUri, std::string method, OC::Cm::Serialization::ISerializable *data,
                                             int &requestId )
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                OC::Cm::Client::Resource::Resource resource (m_pContext, protocol, deviceAddress, relativeUri );
                OC::Cm::CMError error ;

                if (method.compare("GET") == 0)
                {
                    error = resource.get(requestId, NULL, *this);
                }
                else if (method.compare("PUT") == 0)
                {
                    error = resource.put(requestId, NULL, data, *this);
                }
                else if (method.compare("POST") == 0)
                {
                    error = resource.post(requestId, NULL, data, *this);
                }
                else if (method.compare("DELETE") == 0)
                {
                    error = resource.deleteResource(requestId, NULL, *this);
                }

                return (error.getErrorCode() == OC::Cm::CM_SUCCESS) ;
            }

            void
            SubscriptionManager::addSubscriptionProvider ( OC::Cm::Notification::ISubscriptionProvider
                    &subscriptionProvider )
            {
                m_pSubscriptionProvider = &subscriptionProvider ;
            }

            void
            SubscriptionManager::removeSubscriptionProvider ( OC::Cm::Notification::ISubscriptionProvider
                    &subscriptionProvider )
            {
                m_pSubscriptionProvider = NULL ;
            }

            bool
            SubscriptionManager::addNotificationListener ( OC::Cm::Notification::INotificationListener
                    &notificationListener )
            {
                m_pNotificationListener = &notificationListener ;
                return true;
            }

            void
            SubscriptionManager::removeNotificationListener ( OC::Cm::Notification::INotificationListener
                    &notigicationListener )
            {
                m_pNotificationListener = NULL ;
            }

            bool
            SubscriptionManager::addSubscription( int &requestId,
                                                  std::string protocol,
                                                  std::string duuid,
                                                  std::string deviceAddress,
                                                  std::string suuid,
                                                  std::list<std::string> resourcePaths,
                                                  const std::string notificationUri,
                                                  OC::Cm::Client::Resource::IResourceResponseListener *listener,
                                                  std::string descript)
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "Called with protocol [%s], duuid [%s], deviceAddress [%s], suuid [%s]",
                              protocol.c_str(), duuid.c_str(), deviceAddress.c_str(), suuid.c_str());

                if ( protocol.empty() || duuid.empty() || deviceAddress.empty() || suuid.empty()
                     || (resourcePaths.size() == 0))
                {
                    return false ;
                }

                std::string subscriptionLink = protocol + "://" + deviceAddress + getSubscriptionsLink();

                std::string notificationLink = notificationUri;
                if ( notificationLink.empty())
                {
                    notificationLink =  getNotificationLink();
                }

                if ( notificationLink.empty() )
                {
                    return false ;
                }

                String50    description ;
                description.value                       = descript ;

                Subscription *subscriptionData = new Subscription();
                if (subscriptionData == NULL) { return false; }

                subscriptionData->mpDescription         = &description ;
                subscriptionData->mpNotificationURI         = &notificationLink ;
                subscriptionData->mpUuid                    = new OC::Cm::Serialization::Xsd::UuidType();
                subscriptionData->mpUuid->value         = suuid ;

                subscriptionData->mpResourceURIs            = new ArrayOfAnyURI();
                for ( std::list<std::string>::iterator itr = resourcePaths.begin(); itr != resourcePaths.end();
                      itr++ )
                {
                    subscriptionData->mpResourceURIs->members.push_back(*itr);
                }

                if (isValidUri(subscriptionLink))
                {
                    if ( false == sendRequest(subscriptionLink, "POST", subscriptionData, requestId ))
                    {
                        return false ;
                    }
                }
//  else
//  {
//      if ( false == sendRequest(protocol, deviceAddress, subscriptionLink, "POST", &subscriptionData, requestId ))
//      {
//          return false ;
//      }
//  }

                if ( NULL != listener)
                {
                    // Add record response listeners
                    SubscriptionRequest reqData;
                    reqData.m_pListener         = listener ;
                    reqData.m_reqType           = SRT_ADD_SUBSCRIPTION ;
                    reqData.m_device_address    = deviceAddress ;
                    reqData.m_uuid              = duuid ;
                    reqData.m_resource_paths    = resourcePaths;

                    m_pResponseListeners.insert( std::pair< int, SubscriptionRequest >( requestId, reqData )) ;
                }

                // save pending Subscription Record
                OC::Cm::Notification::SubscriptionRecord subscription ;
                subscription.m_deviceId             = duuid ; //Change it to UUID of device
                subscription.m_noificationUri       = notificationLink ;
                subscription.m_subscriptionUri      = "";

                XsList strArray ;
                for ( std::list<std::string>::iterator itr = resourcePaths.begin(); itr != resourcePaths.end();
                      itr++ )
                {
                    strArray.addValueToken(*itr);
                }
                strArray.getValue( subscription.m_subscribedResource );

                m_pDatabase->savePendingMySubscription(requestId, subscription) ;

                subscriptionData->mpDescription         = NULL ;
                subscriptionData->mpNotificationURI     = NULL ; // To avoid freeing in destructor of Subscription
                delete subscriptionData->mpUuid; subscriptionData->mpUuid = NULL;
                delete subscriptionData->mpResourceURIs; subscriptionData->mpResourceURIs = NULL;
                delete subscriptionData; subscriptionData = NULL;

                return true ;
            }

            bool
            SubscriptionManager::removeSubscription( std::string &uuid,
                    std::string &deviceAddress,
                    std::string subscriptionUri,
                    std::string &protocol,
                    OC::Cm::Client::Resource::IResourceResponseListener *listener, int &requestId )
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                if (isValidUri(subscriptionUri))
                {
                    // Send DELETE Request
                    if (false == sendRequest( subscriptionUri, "DELETE", NULL, requestId ) )
                    {
                        return false ;
                    }
                }
                else
                {
                    if ( false == sendRequest(protocol, deviceAddress, subscriptionUri, "DELETE", NULL, requestId ))
                    {
                        return false ;
                    }
                }

                if ( NULL != listener)
                {
                    // Add record response listeners
                    SubscriptionRequest reqData;
                    reqData.m_pListener         = listener ;
                    reqData.m_reqType           = SRT_REMOVE_SUBSCRIPTION ;
                    reqData.m_device_address    = deviceAddress ;
                    reqData.m_subscription_uri  = subscriptionUri;
                    reqData.m_uuid              = uuid ;

                    m_pResponseListeners.insert( std::pair< int, SubscriptionRequest >( requestId, reqData )) ;
                }

                return true ;
            }

            bool
            SubscriptionManager::removeSubscription(
                std::string &uuid,
                std::string &deviceAddress,
                std::string &protocol,
                std::list<std::string> resourcePaths,
                OC::Cm::Client::Resource::IResourceResponseListener *listener,
                std::map< int, std::list< std::string > > &requestIDMap )
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                std::list<OC::Cm::Notification::SubscriptionRecord> subscriptionList ;
                std::string subscribedResourcePath ;

                XsList strArray ;
                strArray.values = resourcePaths ;
                strArray.getValue(subscribedResourcePath);

                if (false == m_pDatabase->getMySubscriptionContainingResourceList(uuid, resourcePaths,
                        subscriptionList))
                {
                    return false ;
                }

                for (std::list<OC::Cm::Notification::SubscriptionRecord>::const_iterator itr =
                         subscriptionList.begin(); itr != subscriptionList.end(); itr++)
                {
                    OC::Cm::Notification::SubscriptionRecord pSubscription = *(itr) ;

                    int requestId ;
                    std::list<string> resourceUris ;

                    if (false == removeSubscription(uuid, deviceAddress, pSubscription.m_subscriptionUri, protocol,
                                                    listener, requestId))
                    {
                        return false ;
                    }

                    resourceUris.push_back(pSubscription.m_subscriptionUri) ;
                    requestIDMap[requestId] = resourceUris;
                }

                return true ;

//  std::map< std::string, std::list<std::string> > remainingResourceMap ;
//  std::map< std::string, std::list<std::string> > removingResourceMap ;
//
//  for ( std::list<std::string>::iterator itr = resourcePaths.begin(); itr != resourcePaths.end(); itr++ )
//  {
//      OC::Cm::Notification::SubscriptionRecord subscription ;
//
//      std::string uuid = device->getUUID() ;
//      if (false == m_pDatabase->getMySubscriptionContainingResource(uuid, (*itr), subscription))
//      {
//          return false ;
//      }
//
//      std::string subscriptionUri = subscription.m_subscriptionUri ;
//
//      std::list<std::string> remainingResources ;
//      std::list<std::string> removingResources ;
//
//      if ( remainingResourceMap.end() == remainingResourceMap.find(subscriptionUri) )
//      {
//          XsList strArray ;
//          strArray.setValue(subscription.m_subscribedResource);
//
//          remainingResources = strArray.getValueTokens();
//      }
//      else
//      {
//          remainingResources = remainingResourceMap[subscriptionUri] ;
//      }
//
//      if ( removingResourceMap.end() != removingResourceMap.find(subscriptionUri) )
//      {
//          removingResources = removingResourceMap[subscriptionUri] ;
//      }
//
//      if (remainingResources.size() > 0)
//      {
//          remainingResources.remove((*itr));
//      }
//      removingResources.push_back((*itr));
//
//      remainingResourceMap[subscriptionUri] = remainingResources ;
//      removingResourceMap[subscriptionUri]  = removingResources ;
//  }
//
//  int requestId = -1 ;
//
//  for (std::map< std::string, std::list<std::string> >::iterator itr = remainingResourceMap.begin(); itr != remainingResourceMap.end(); itr++ )
//  {
//      std::string subscriptionUri = itr->first ;
//
//      if ( (itr)->second.size() == 0 )
//      {
//          std::ostringstream uriStream ;
//          uriStream << "http://" << deviceAddress << subscriptionUri ;
//          std::string reqUri = uriStream.str() ;
//
//          // Send DELETE Request
//          if (false == sendRequest( reqUri, "DELETE", NULL, requestId ) )
//          {
//              return false ;
//          }
//
//          requestIDMap[requestId] = removingResourceMap[subscriptionUri];
//
//          if ( NULL != listener)
//          {
//              // Add record response listeners
//              SubscriptionRequest reqData;
//              reqData.m_pListener         = listener ;
//              reqData.m_reqType           = SRT_REMOVE_SUBSCRIPTION ;
//              reqData.m_device_address    = deviceAddress ;
//              reqData.m_resource_paths    = removingResourceMap[subscriptionUri];
//              reqData.m_subscription_uri  = subscriptionUri;
//              reqData.m_uuid = device->getUUID() ;
//
//              m_pResponseListeners.insert( std::pair< int, SubscriptionRequest >( requestId, reqData )) ;
//          }
//      }
//      else
//      {
//          std::string notificationLink =  getNotificationLink();
//
//          // SEND UPDATE Request
//
//          String30    description ;
//          description.value                       = "subscrptionRequest" ;
//
//          Subscription subscriptionData ;
//
//          subscriptionData.Id.value               = "subscrptionID" ;
//          subscriptionData.Description            = &description ;
//          subscriptionData.notificationURI        = &notificationLink ; // Optional to send notification link
//
//          for ( std::list<std::string>::iterator itr2 = (itr)->second.begin(); itr2 != (itr)->second.end(); itr2++ )
//          {
//              subscriptionData.ResourceURIs.members.push_back(*itr2);
//          }
//
//          std::ostringstream uriStream ;
//          uriStream << "http://" << deviceAddress << subscriptionUri;
//          std::string reqUri = uriStream.str() ;
//
//          if ( false == sendRequest( reqUri, "PUT", &subscriptionData, requestId ))
//          {
//              return false ;
//          }
//
//          requestIDMap[requestId] = removingResourceMap[subscriptionUri];
//
//          if ( NULL != listener)
//          {
//              // Add record response listeners
//              SubscriptionRequest reqData;
//              reqData.m_pListener         = listener ;
//              reqData.m_reqType           = SRT_UPDATE_SUBSCRIPTION ;
//              reqData.m_device_address    = deviceAddress ;
//              reqData.m_resource_paths    = removingResourceMap[subscriptionUri];
//              reqData.m_subscription_uri  = subscriptionUri;
//              reqData.m_remaining_resources = (itr)->second ;
//              reqData.m_uuid = device->getUUID();
//
//              m_pResponseListeners.insert( std::pair< int, SubscriptionRequest >( requestId, reqData )) ;
//          }
//
//          subscriptionData.Description        = NULL ;
//          subscriptionData.notificationURI    = NULL ;
//      }
//  }

//  return true ;
            }

            bool
            SubscriptionManager::notify(  NotificationResponse *notificationData)
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                std::list<OC::Cm::Notification::SubscriptionRecord> subscriptionList ;
                std::string resPath = (notificationData->getResourcePath());

                std::string resPath1 = resPath;
                std::list<std::string> resourcePathList;
                std::string delimiter = "/";

                int pos = 0;
                std::string token;
                while ((pos = resPath1.find(delimiter)) != std::string::npos)
                {
                    token = resPath1.substr(0, pos);
                    if (token != "")
                    {
                        resourcePathList.push_back(resPath.substr(0, resPath.find(token) + token.length()));
                    }
                    resPath1.erase(0, pos + delimiter.length());
                }
                resourcePathList.push_back(resPath);

                if (false == m_pDatabase->getSubscriptionContainingResourceList(resourcePathList, subscriptionList))
                {
                    return false ;
                }

                NotificationEventType notificationTypeValidate;
                notificationTypeValidate.value = notificationData->getNotificationType();
                if ( false == notificationTypeValidate.validateContent() )
                {
                    return false ;
                }

                for (std::list<OC::Cm::Notification::SubscriptionRecord>::const_iterator itr =
                         subscriptionList.begin(); itr != subscriptionList.end(); itr++)
                {
                    OC::Cm::Notification::SubscriptionRecord pSubscription = *(itr) ;

                    ::Notification notification ;
                    int requestId ;

                    //notification.Id.value                 = "Notification" ;
                    //notification.Description          = NULL ;

                    std::string subscriptionsLink = getSubscriptionsLink() ;
                    std::ostringstream uriStream ;
                    uriStream << subscriptionsLink << "/" << pSubscription.m_rowId ;
                    notification.mpSubscriptionURI  = new std::string (uriStream.str()) ;

                    Events *pEvent                  = new Events();
                    pEvent->mpEvent                 = new NotificationEventType();
                    pEvent->mpEvent->value          = notificationData->getNotificationType();
                    pEvent->mpEventTime             = new OC::Cm::Serialization::Xsd::DateTimeType();
                    pEvent->mpEventTime->value      = getCurrentTimeInISO8601();
                    pEvent->mpResourceURI           = new std::string(notificationData->getResourcePath());
                    pEvent->mpUuid                  = new OC::Cm::Serialization::Xsd::UuidType() ;
                    pEvent->mpUuid->value           = notificationData->getUuid() ;

                    if (notificationData->getNotificationData() != NULL)
                    {
                        pEvent->mpChangedResource       = notificationData->getNotificationData();
//          pEvent->mpChangedResource->mpChangedResourceType = new ResourceType();
//          pEvent->mpChangedResource->mpChangedResourceType->value = notificationData->getElementName();
//          pEvent->mpChangedResource->m_schema_instance_type       = notificationData->getElementName();
//          pEvent->mpChangedResource->m_pSchema_instance_data  = notificationData;
                    }

                    notification.mpEvents.push_back( pEvent ) ;
                    sendRequest(pSubscription.m_noificationUri, "POST", &notification, requestId) ;

                    if (pEvent->mpChangedResource)
                    {
                        pEvent->mpChangedResource = NULL; // Ranjith : To Avoid freeing..... on destructor of ISerializable
                    }
                }

//  for (std::list<OC::Cm::Notification::SubscriptionRecord*>::const_iterator itr = subscriptionList.begin(); itr != subscriptionList.end(); itr++)
//  {
//      OC::Cm::Notification::SubscriptionRecord* pSubscription = (*itr) ;
//      if (pSubscription != NULL)
//      {
//          delete pSubscription ;
//      }
//  }

                return true ;
            }

            bool
            SubscriptionManager::notify(  std::list<NotificationResponse *> notificationDataList )
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                std::list<std::string> resourcePathList;

                NotificationEventType notificationTypeValidate;
                for (std::list<OC::Cm::Notification::NotificationResponse *>::iterator itr =
                         notificationDataList.begin(); itr != notificationDataList.end(); itr++ )
                {
                    notificationTypeValidate.value = (*itr)->getNotificationType();
                    if ( false == notificationTypeValidate.validateContent() )
                    {
                        return false ;
                    }

                    resourcePathList.push_back((*itr)->getResourcePath());

                    std::string resPath = (*itr)->getResourcePath();
                    std::string resPath1 = resPath;
                    std::string delimiter = "/";

                    int pos = 0;
                    std::string token;
                    while ((pos = resPath1.find(delimiter)) != std::string::npos)
                    {
                        token = resPath1.substr(0, pos);
                        if (token != "")
                        {
                            resourcePathList.push_back(resPath.substr(0, resPath.find(token) + token.length()));
                        }
                        resPath1.erase(0, pos + delimiter.length());
                    }
                }

                std::list<OC::Cm::Notification::SubscriptionRecord> subscriptionList ;
                if (false == m_pDatabase->getSubscriptionContainingResourceList( resourcePathList,
                        subscriptionList ))
                {
                    return false ;
                }

                for ( std::list<OC::Cm::Notification::SubscriptionRecord>::const_iterator itr =
                          subscriptionList.begin(); itr != subscriptionList.end(); itr++)
                {
                    OC::Cm::Notification::SubscriptionRecord pSubscription = *(itr) ;

                    ::Notification notification ;
                    int requestId ;

                    std::string subscriptionsLink = getSubscriptionsLink() ;
                    std::ostringstream uriStream ;
                    uriStream << subscriptionsLink << "/" << pSubscription.m_rowId ;
                    notification.mpSubscriptionURI  = new std::string (uriStream.str()) ;

                    for (std::list<OC::Cm::Notification::NotificationResponse *>::iterator nitr =
                             notificationDataList.begin(); nitr != notificationDataList.end(); nitr++ )
                    {
                        bool isSubscribed = false;
                        std::string resPath = (*nitr)->getResourcePath();
                        std::string resPath1 = resPath;
                        std::string delimiter = "/";
                        std::list<std::string> resPathList;

                        int pos = 0;
                        std::string token;
                        while ((pos = resPath1.find(delimiter)) != std::string::npos)
                        {
                            token = resPath1.substr(0, pos);
                            if (token != "")
                            {
                                resPathList.push_back(resPath.substr(0, resPath.find(token) + token.length()));
                            }
                            resPath1.erase(0, pos + delimiter.length());
                        }
                        resPathList.push_back(resPath);

                        for (std::list<std::string>::iterator strItr = resPathList.begin() ; strItr != resPathList.end() ;
                             strItr++)
                        {
                            if ((*itr).m_subscribedResource.find((*strItr)) != std::string::npos)
                            {
                                isSubscribed = true;
                                break;
                            }
                        }

                        //(*itr).m_subscribedResource.find((*nitr)->getResourcePath())!=std::string::npos

                        if (isSubscribed)
                        {
                            Events *pEvent                  = new Events();
                            pEvent->mpEvent                 = new NotificationEventType();
                            pEvent->mpEvent->value          = (*nitr)->getNotificationType();
                            pEvent->mpEventTime             = new OC::Cm::Serialization::Xsd::DateTimeType();
                            pEvent->mpEventTime->value      = getCurrentTimeInISO8601();
                            pEvent->mpResourceURI           = new std::string((*nitr)->getResourcePath());
                            pEvent->mpUuid                  = new OC::Cm::Serialization::Xsd::UuidType() ;
                            pEvent->mpUuid->value           = (*nitr)->getUuid();

                            if ((*nitr)->getNotificationData() != NULL)
                            {
                                pEvent->mpChangedResource       = (*nitr)->getNotificationData();
                            }


                            notification.mpEvents.push_back( pEvent ) ;
                        }
                    }

                    sendRequest(pSubscription.m_noificationUri, "POST", &notification, requestId) ;

                    for ( std::list< Events * >::const_iterator eitr = notification.mpEvents.begin();
                          eitr != notification.mpEvents.end(); eitr++)
                    {
                        if ((*eitr)->mpChangedResource)
                        {
                            (*eitr)->mpChangedResource = NULL; // Ranjith : To Avoid freeing..... on destructor of ISerializable
                        }
                    }
                }

                return true ;
            }


            bool
            SubscriptionManager::notifyAll( std::string &resourcePath,
                                            std::string notificationType,
                                            OC::Cm::Serialization::ISerializable &notificationData)
            {
                // TODO: Multicast notifications not implemented
                return true ;
            }

            OC::Cm::Notification::ISubscriptionHandler *
            SubscriptionManager::getSubscriptionHandler()
            {
                return this ;
            }

            bool
            SubscriptionManager::handleGetSubscriptionUri( std::string &deviceAddress,
                    std::string &subscriptionId,
                    OC::Cm::Serialization::ISerializable *response, int &statusCode)
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                OC::Cm::Notification::SubscriptionRecord subscription ;
                Subscription *subscriptionResponse = (Subscription *) response ;


                if (false == m_pDatabase->getSubscriptionBySubscriptionID(subscriptionId, subscription))
                {
                    statusCode = 404 ;
                    return false ;
                }

                subscriptionResponse->mpId                  = new String50();
                subscriptionResponse->mpId->value           = subscriptionId ;
                subscriptionResponse->mpDescription         = NULL ;
                subscriptionResponse->mpNotificationURI     = new std::string(subscription.m_noificationUri) ;

                subscriptionResponse->mpUuid                = new OC::Cm::Serialization::Xsd::UuidType() ;
                subscriptionResponse->mpUuid->value         = subscription.m_deviceId ;

                XsList strArray ;
                strArray.setValue(subscription.m_subscribedResource);

                //remainingResources = strArray.getValueTokens();
                subscriptionResponse->mpResourceURIs = new ArrayOfAnyURI();
                subscriptionResponse->mpResourceURIs->members = strArray.getValueTokens();

                statusCode = 200 ;

                return true ;
            }

            bool
            SubscriptionManager::handleDeleteSubscriptionRequest ( std::string &deviceAddress,
                    std::string &subscriptionId, int &statusCode )
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                OC::Cm::Notification::SubscriptionRecord prevSubscription ;
                if (false == m_pDatabase->getSubscriptionBySubscriptionID(subscriptionId, prevSubscription))
                {
                    statusCode = 404 ;
                    return false ;
                }

                if (false == m_pDatabase->deleteSubscriptionBySubscriptionID(subscriptionId))
                {
                    statusCode = 404 ;
                    return false ;
                }

                statusCode = 204 ;

                return true ;
            }

            bool
            SubscriptionManager::handleUpdateSubscriptionRequest( std::string &deviceAddress,
                    std::string &subscriptionId,
                    OC::Cm::Serialization::ISerializable *request, int &statusCode)
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                Subscription *subscriptionReq = (Subscription *) request ;
                if (subscriptionReq == NULL)
                {
                    statusCode = 400 ;
                    return false ;
                }

                OC::Cm::Notification::SubscriptionRecord prevSubscription ;
                if (false == m_pDatabase->getSubscriptionBySubscriptionID(subscriptionId, prevSubscription))
                {
                    statusCode = 404 ;
                    return false ;
                }

                if ( NULL != subscriptionReq->mpNotificationURI )
                {
                    prevSubscription.m_noificationUri       = *(subscriptionReq->mpNotificationURI) ;
                }

                if ( NULL != subscriptionReq->mpResourceURIs )
                {
                    XsList strArray ;
                    strArray.values = subscriptionReq->mpResourceURIs->members ;
                    strArray.getValue(prevSubscription.m_subscribedResource);
                }

                if ( true == m_pDatabase->updateSubscription( prevSubscription ) )
                {
                    statusCode = 204 ;
                    return true ;
                }
                else
                {
                    statusCode = 500 ;
                    return false ;
                }
            }

            bool
            SubscriptionManager::handleGetSubscriptionsRequest(OC::Cm::Serialization::ISerializable *response,
                    int &statusCode)
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                Subscriptions *subscriptionsResp = (Subscriptions *) response ;
                if (subscriptionsResp == NULL)
                {
                    statusCode = 400 ;
                    return true ;
                }

                if (m_pDatabase == NULL)
                {
                    statusCode = 500 ;
                    return true ;
                }

                std::list<OC::Cm::Notification::SubscriptionRecord> subscriptionList ;
                if (false == m_pDatabase->getSubscriptions(subscriptionList))
                {
                    statusCode = 500 ;
                    return true ;
                }

                for (std::list<OC::Cm::Notification::SubscriptionRecord>::const_iterator itr =
                         subscriptionList.begin(); itr != subscriptionList.end(); itr++)
                {
                    OC::Cm::Notification::SubscriptionRecord pSubscription = *(itr) ;

                    Subscription *subscriptionResponse          = new Subscription() ;

                    if (NULL != subscriptionResponse)
                    {
                        std::ostringstream idStream ;
                        idStream << pSubscription.m_rowId ;

                        subscriptionResponse->mpId                  = new String50() ;
                        subscriptionResponse->mpId->value           = idStream.str() ;

                        subscriptionResponse->mpDescription         = NULL ;
                        subscriptionResponse->mpNotificationURI     = new std::string(pSubscription.m_noificationUri) ;

                        XsList strArray ;
                        strArray.setValue(pSubscription.m_subscribedResource);

                        subscriptionResponse->mpResourceURIs = new ArrayOfAnyURI() ;
                        subscriptionResponse->mpResourceURIs->members = strArray.getValueTokens();

                        subscriptionResponse->mpUuid = new OC::Cm::Serialization::Xsd::UuidType() ;
                        subscriptionResponse->mpUuid->value = pSubscription.m_deviceId ;

                        subscriptionsResp->mpSubscription.push_back(subscriptionResponse) ;
                    }
                }

                statusCode = 200 ;
                return true ;
            }

            bool
            SubscriptionManager::handleSubscriptionRequest( std::string &deviceAddress,
                    OC::Cm::Serialization::ISerializable *request,
                    std::string &location, int &statusCode )
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                Subscription *subscriptionReq = (Subscription *) request ;
                if ((subscriptionReq == NULL) || (NULL == subscriptionReq->mpNotificationURI)
                    || (NULL == subscriptionReq->mpUuid))
                {
                    statusCode = 400 ;
                    return false ;
                }

                {
                    // Validating notification URL

                    std::string host ;
                    std::string port;
                    std::string path;
                    std::string hostheader;
                    std::string protocol ;

                    if (false == parseUrl(*(subscriptionReq->mpNotificationURI), protocol, host, port, path,
                                          hostheader))
                    {
                        statusCode = 400 ;
                        return false;
                    }
                }


                /**
                    OC::Cm::Device*pDevice = getDeviceByAddress(hostheader);
                    if (NULL == pDevice)
                    {
                        statusCode = 400 ;
                        return false;
                    }

                    // Check whether subscription is already available or not... if available
                    std::list< std::string > resourcePaths = subscriptionReq->ResourceURIs.members;
                    for (std::list< std::string >::iterator itr = resourcePaths.begin(); itr != resourcePaths.end(); itr++)
                    {
                        OC::Cm::Notification::SubscriptionRecord subscription ;
                        std::string path = *itr ;
                        std::string deviceId = pDevice->getUUID() ;

                        if (m_pDatabase->getSubscriptionContainingResource( deviceId, path, subscription))
                        {
                            //Resource already subscribed....
                            XsList strArray ;
                            strArray.setValue(subscription.m_subscribedResource);
                            strArray.removeValueToken(path);

                            if (strArray.getValueTokens().size() > 0)
                            {
                                //update subscription
                                strArray.getValue(subscription.m_subscribedResource) ;
                                if (false == m_pDatabase->updateSubscription(subscription))
                                {

                                }
                            }
                            else
                            {
                                // delete record
                                if (false == m_pDatabase->deleteSubscriptionBySubscriptionID(subscription.m_rowId))
                                {

                                }
                            }
                        }
                    }

                **/

                // Create new subscription
                OC::Cm::Notification::SubscriptionRecord subscription ;
                subscription.m_deviceId = subscriptionReq->mpUuid->value ;
                if (NULL != subscriptionReq->mpNotificationURI)
                {
                    subscription.m_noificationUri       = *(subscriptionReq->mpNotificationURI) ;
                }
                subscription.m_subscriptionUri = "";    // TODO: need to remove subscriptionUri from subscription db

                XsList strArray ;
                if (subscriptionReq->mpResourceURIs != NULL)
                {
                    strArray.values = subscriptionReq->mpResourceURIs->members ;
                }
                strArray.getValue(subscription.m_subscribedResource) ;

                //save subscription
                if (true == m_pDatabase->saveSubscription(subscription))
                {
                    std::string subscriptionsLink = getSubscriptionsLink() ;

                    std::ostringstream uriStream ;
                    uriStream << subscriptionsLink << "/" << subscription.m_rowId ;

                    location = uriStream.str() ;
                    statusCode = 201 ;
                    return true ;
                }
                else
                {
                    statusCode = 500 ;
                    return false ;
                }
            }

            bool
            SubscriptionManager::handleNotification( std::string &deviceAddress,
                    OC::Cm::Serialization::ISerializable *request, int &statusCode )
            {
                CM_LOG_INFO_A(CM_SUB_MGR, "%s", "Called");
                ::Notification *notification = (::Notification *) request ;

                if (( NULL != notification ) && ( NULL != notification->mpSubscriptionURI ))
                {
                    if ( NULL != m_pNotificationListener )
                    {
                        OC::Cm::Notification::SubscriptionRecord subscription ;
#if 0
                        if (false == m_pDatabase->getMySubscriptionBySubscriptionUri(*(notification->mpSubscriptionURI),
                                subscription))
                        {
                            statusCode = 404;
                            return false;
                        }
#endif
                        for ( std::list< Events * >::iterator itr = notification->mpEvents.begin();
                              itr != notification->mpEvents.end(); itr++)
                        {
                            std::string uuid ;
                            if ((*itr)->mpUuid != NULL)
                            {
                                uuid = (*itr)->mpUuid->value ;
                            }

                            std::string resourceUri ;
                            if ((*itr)->mpResourceURI != NULL)
                            {
                                resourceUri = *((*itr)->mpResourceURI) ;
                            }

                            std::string notificationType;
                            if ((*itr)->mpEvent != NULL)
                            {
                                notificationType = (*itr)->mpEvent->value ;
                            }

                            if ( NULL != (*itr)->mpChangedResource )
                            {
                                m_pNotificationListener->onNotificationReceived( uuid, resourceUri, notificationType,
                                        (*itr)->mpChangedResource, *(notification->mpSubscriptionURI), (*itr)->mpEventTime ) ;
                            }
                            else
                            {
                                m_pNotificationListener->onNotificationReceived( uuid, resourceUri, notificationType, NULL,
                                        *(notification->mpSubscriptionURI), (*itr)->mpEventTime) ;
                            }

                        }
                    }
                }

                statusCode = 201 ;

                return true ;
            }

        }
    }
}
