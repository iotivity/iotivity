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



#if !defined(_SUBSCRIPTIONMANAGER_H)
#define _SUBSCRIPTIONMANAGER_H

#include "CMClientResource.h"
#include "CMSerialization.h"
#include "CMNotification.h"

#include "SubscriptionConfig.h"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            class ISubscriptionDB;

            /**
             * @enum    _SubscriptionReqType
             * @brief   Type of subscription request
             */
            typedef enum _SubscriptionReqType
            {
                SRT_ADD_SUBSCRIPTION,   //!< SRT_ADD_SUBSCRIPTION
                SRT_REMOVE_SUBSCRIPTION,//!< SRT_REMOVE_SUBSCRIPTION
                SRT_SEND_NOTIFICATION,  //!< SRT_SEND_NOTIFICATION
                SRT_UPDATE_SUBSCRIPTION //!< SRT_UPDATE_SUBSCRIPTION

            } SubscriptionReqType ; /**< Represents the enum _SubscriptionReqType */

            /**
             * @class       SubscriptionRequest
             * @brief       This is an class implements the subscription request
             */
            class SubscriptionRequest
            {

                public:

                    SubscriptionReqType m_reqType ; /**< Represents the type of subscription request */

                    OC::Cm::Client::Resource::IResourceResponseListener *m_pListener
                    ;  /**< Represents the resource response listener */

                    std::string m_device_address ;  /**< Represents the device address */

                    std::list<std::string> m_resource_paths;    /**< Represents the list of resource paths */

                    std::list<std::string> m_remaining_resources;   /**< Represents the list of remaining resources */

                    std::string     m_subscription_uri ;    /**< Represents the subscription uri */

                    std::string     m_uuid ;    /**< Represents the device UUID */
            } ;

            /**
             * @class       SubscriptionManager
             * @brief       This is an class implements the subscription manager
             */
            class SUBSCRIPTIONAPI SubscriptionManager : public OC::Cm::Notification::ISubscriptionManager,
                public OC::Cm::Notification::ISubscriptionHandler,
                public OC::Cm::Client::Resource::IResourceResponseListener
            {

                public:

                    /**
                     * Parameterized constructor
                     *
                     * @param[in]   pDb Represents the instance of ISubscrionDB class.
                     */
                    SubscriptionManager(  ISubscriptionDB *pDb );

                    /**
                     * Default destructor
                     */
                    ~SubscriptionManager();

                    /**
                     * Initializing subscription manager.
                     *
                     * @param[in]   pContext    Represents the instance of Control Manager Context where Control Manager has started
                     * @param[in]   dbFilePath  Represents the database file path
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    bool init( OC::Cm::IContext *pContext, std::string &dbFilePath ) ;

                    /**
                     *  De-initializing Subscription manager
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    bool deinit() ;

                    /**
                     * Setting device address, protocol and device uuid.
                     *
                     * @param[in]   protocol    Represents the device protocol
                     * @param[in]   address     Represents the device address
                     * @param[in]   uuid        Represents the device uuid
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    bool setDeviceAddress(std::string protocol, std::string address, std::string uuid) ;


                    /**
                     * Get list of subscription of specified device uuid.
                     *
                     * @param[in]   uuid    Represents the device UUID.
                     *
                     * @return  Returns the list of subscriptions as strings
                     */
                    std::list< std::string > getMySubscriptions(std::string uuid) ;

                    /**
                     * Gets the SerializableDataFactory instance.
                     *
                     * @returns Returns SerializableDataFactory instance.
                     */
                    OC::Cm::Serialization::SerializableDataFactory *getSerializationFactory();

                    /**
                     * Gets the ResourceHandlerFactory instance.
                     *
                     * @returns Returns ResourceHandlerFactory instance.
                     */
                    OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *getResourceHandlerFactory();

                    /**
                     * Adding subscription provider
                     *
                     * @param[in]   subscriptionProvider    Represents the instance of ISubscriptionProvider
                     */
                    void addSubscriptionProvider ( OC::Cm::Notification::ISubscriptionProvider &subscriptionProvider) ;

                    /**
                     * Removing subscription provider
                     *
                     * @param[in]   subscriptionProvider    Represents the instance of ISubscriptionProvider
                     */
                    void removeSubscriptionProvider ( OC::Cm::Notification::ISubscriptionProvider
                                                      &subscriptionProvider ) ;

                    /**
                     * Adding notification listener
                     *
                     * @param[in]   notificationListener    Represents the instance of INotificationListener
                     *
                     * @return Return @c true in case of success @n@c false in case of any failure
                     */
                    bool addNotificationListener ( OC::Cm::Notification::INotificationListener &notificationListener );

                    /**
                     * Removing notification listener
                     *
                     * @param[in]   notificationListener    Represents the instance of INotificationListener
                     */
                    void removeNotificationListener ( OC::Cm::Notification::INotificationListener
                                                      &notificationListener ) ;

                    /**
                     * Adding subscription to the subscription manager
                     *
                     * @param[out]  requestId       Represents the request identifier
                     * @param[in]   protocol        Represents the protocol which subscription manager used for sending notifications
                     * @param[in]   duuid           Represents  peer device uuid
                     * @param[in]   deviceAddress   Represents the peer device address
                     * @param[in]   suuid           Represents the its own device uuid
                     * @param[in]   resourcePaths   Represents the list of resource paths
                     * @param[in]   notificationUri Represents the notification uri
                     * @param[in]   listener        Represents the resource response listener to the application
                     * @param[in]   description     Represents the desciption of the subscription
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    bool addSubscription( int &requestId,
                                          std::string protocol,
                                          std::string duuid,
                                          std::string deviceAddress,
                                          std::string suuid,
                                          std::list<std::string> resourcePaths,
                                          const std::string notificationUri,
                                          OC::Cm::Client::Resource::IResourceResponseListener *listener,
                                          std::string description) ;

                    /**
                     * Removing subscription from the subscription manager
                     *
                     * @param[in]   uuid            Represents the device UUID
                     * @param[in]   deviceAddress   Represents the device address
                     * @param[in]   subscriptionUri Represents the subscription uri
                     * @param[in]   protocol        Represents the device protocol
                     * @param[in]   listener        Represents the resource response listener
                     * @param[out]  requestId       Represents the request identifier
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    bool removeSubscription( std::string &uuid,
                                             std::string &deviceAddress,
                                             std::string subscriptionUri,
                                             std::string &protocol,
                                             OC::Cm::Client::Resource::IResourceResponseListener *listener, int &requestId ) ;

                    /**
                     * Removing subscription from the subscription manager
                     *
                     * @param[in]   uuid            Represents the device UUID
                     * @param[in]   deviceAddress   Represents the device address
                     * @param[in]   protocol        Represents the device protocol
                     * @param[in]   resourcePaths   Represents the list of resource paths
                     * @param[in]   listener        Represents the resource response listener
                     * @param[out]  requestIDMap        Represents the map of request identifiers
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    bool removeSubscription( std::string &uuid,
                                             std::string &deviceAddress,
                                             std::string &protocol,
                                             std::list<std::string> resourcePaths,
                                             OC::Cm::Client::Resource::IResourceResponseListener *listener,
                                             std::map< int, std::list< std::string > > &requestIDMap ) ;

                    /**
                     * Sending notification to the notification resource
                     *
                     * @param[in]  notificationData Represents the instance of NotificationReponse class
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    bool notify(  NotificationResponse *notificationData ) ;

                    /**
                     * Sending notification to the notification resource
                     *
                     * @param[in]  notificationDataList Represents the list of instances of NotificationReponse class
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    bool notify( std::list<NotificationResponse *> notificationDataList) ;

                    /**
                     * Sending notification to the notification resource
                     *
                     * @param[in]   resourcePath        Represents the resource path
                     * @param[in]   notificationType    Represents the type of notification
                     * @param[in]   notificationData    Represents the serializable object of notification
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    bool notifyAll( std::string &resourcePath,
                                    std::string notificationType,
                                    OC::Cm::Serialization::ISerializable &notificationData) ;

                    /**
                     * Gets the subscription handler
                     *
                     * @return Returns the instance of ISubscriptionHandler class
                     */
                    OC::Cm::Notification::ISubscriptionHandler *getSubscriptionHandler() ;

                    /**
                     * Handles the GET subscription request
                     *
                     * @param[in]   response    Represents the instance of ISerializable class
                     * @param[out]  statusCode  Represents the status code of the request.
                     *
                     * @return Returns the instance of ISubscriptionHandler class
                     */
                    bool handleGetSubscriptionsRequest(OC::Cm::Serialization::ISerializable *response,
                                                       int &statusCode) ;

                    /**
                     * Handles the subscription request
                     *
                     * @param[in]   deviceAddress   Represents the device address
                     * @param[in]   request Represents the instance of ISerializable class
                     * @param[out]  location    Represents the resource location
                     * @param[out]  statusCode  Represents the status code of the request.
                     *
                     * @return Returns the instance of ISubscriptionHandler class
                     */
                    bool handleSubscriptionRequest( std::string &deviceAddress,
                                                    OC::Cm::Serialization::ISerializable *request, std::string &location, int &statusCode) ;

                    /**
                     * Handles the update subscription request
                     *
                     * @param[in]   deviceAddress   Represents the device address
                     * @param[in]   subscriptionId  Represents the subscription identifier
                     * @param[in]   request Represents the instance of ISerializable class
                     * @param[out]  statusCode  Represents the status code of the request.
                     *
                     * @return Returns the instance of ISubscriptionHandler class
                     */
                    bool handleUpdateSubscriptionRequest( std::string &deviceAddress,  std::string &subscriptionId,
                                                          OC::Cm::Serialization::ISerializable *request, int &statusCode);
                    /**
                     * Handles the delete subscription request
                     *
                     * @param[in]   deviceAddress   Represents the device address
                     * @param[in]   subscriptionId  Represents the subscription identifier
                     * @param[out]  statusCode  Represents the status code of the request.
                     *
                     * @return Returns the instance of ISubscriptionHandler class
                     */
                    bool handleDeleteSubscriptionRequest ( std::string &deviceAddress,  std::string &subscriptionId,
                                                           int &statusCode ) ;

                    /**
                     * Handles the GET subscription request
                     *
                     * @param[in]   deviceAddress   Represents the device address
                     * @param[in]   subscriptionId  Represents the subscription identifier
                     * @param[in]   response    Represents the instance of ISerializable class
                     * @param[out]  statusCode  Represents the status code of the request.
                     *
                     * @return Returns the instance of ISubscriptionHandler class
                     */
                    bool handleGetSubscriptionUri( std::string &deviceAddress, std::string &subscriptionId,
                                                   OC::Cm::Serialization::ISerializable *response, int &statusCode) ;
                    /**
                     * Handles the notification
                     *
                     * @param[in]   deviceAddress   Represents the device address
                     * @param[in]   request Represents the instance of ISerializable class
                     * @param[out]  statusCode  Represents the status code of the request.
                     *
                     * @return Returns the instance of ISubscriptionHandler class
                     */
                    bool handleNotification( std::string &deviceAddress, OC::Cm::Serialization::ISerializable *request,
                                             int &statusCode) ;

                    /**
                     * This is a callback when response on GET request received, this will called
                     *
                     * @param[in]   clientSessionId Represents the client session ID by which request sent
                     * @param[in]   status          Represents the http status code of the response
                     * @param[in]   headers         Represents the http headers for the response
                     * @param[in]   response        Represents the ISerializable object of response
                     */
                    virtual void OnGetResponseReceived( int clientSessionId, int status,
                                                        OC::Cm::Connector::HeadersList &headers,  OC::Cm::Serialization::ISerializable *response ) ;

                    /**
                     * This is a callback when response on PUT request received, this will called
                     *
                     * @param[in]   clientSessionId Represents the client session ID by which request sent
                     * @param[in]   status          Represents the http status code of the response
                     * @param[in]   headers         Represents the http headers for the response
                     * @param[in]   response        Represents the ISerializable object of response
                     */
                    virtual void OnPutResponseReceived(int clientSessionId,  int status,
                                                       OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response) ;

                    /**
                     * This is a callback when response on POST request received, this will called
                     *
                     * @param[in]   clientSessionId Represents the client session ID by which request sent
                     * @param[in]   status          Represents the http status code of the response
                     * @param[in]   headers         Represents the http headers for the response
                     * @param[in]   response        Represents the ISerializable object of response
                     */
                    virtual void OnPostResponseReceived(int clientSessionId, int status,
                                                        OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response);

                    /**
                     * This is a callback when response on DELETE request received, this will called
                     *
                     * @param[in]   clientSessionId Represents the client session ID by which request sent
                     * @param[in]   status          Represents the http status code of the response
                     * @param[in]   headers         Represents the http headers for the response
                     * @param[in]   response        Represents the ISerializable object of response
                     */
                    virtual void onDeleteResponseReceived(int clientSessionId, int status,
                                                          OC::Cm::Connector::HeadersList &headers,  OC::Cm::Serialization::ISerializable *response);

                    /**
                     * This is a callback when response on subscription received, this will called
                     *
                     * @param[in]   clientSessionId Represents the client session ID by which request sent
                     * @param[in]   status          Represents the http status code of the response
                     * @param[in]   subscriptionUri         Represents subscription uri
                     */
                    virtual void onSubscribeResponseReceived( int clientSessionId, int status,
                            std::string &subscriptionUri );

                    /**
                     * This is a callback when response on unsubscription received, this will called
                     *
                     * @param[in]   clientSessionId Represents the client session ID by which request sent
                     * @param[in]   status          Represents the http status code of the response
                     */
                    virtual void onUnSubscribeResponseReceived( int clientSessionId, int status );

                    /**
                     * This is a callback when response on notification received, this will called
                     *
                     * @param[in]   clientSessionId Represents the client session ID by which request sent
                     * @param[in]   status          Represents the http status code of the response
                     */
                    virtual void onNotifyResponseReceived( int clientSessionId, int status ) ;

                    /**
                     * This is a callback when error response received, this will called
                     *
                     * @param[in]   clientSessionId Represents the client session ID by which request sent
                     * @param[in]   status          Represents the http status code of the response
                     * @param[in]   error           Represents the Control Manager Error
                     * @param[in]   errorMessage    Represents the ISerializable object of error response
                     */
                    virtual void onErrorReceived( int clientSessionId, int status, OC::Cm::CMError error,
                                                  OC::Cm::Serialization::ISerializable *errorMessage) ;

                    /**
                     * This is a callback called when request is completed
                     *
                     * @param[in]   requestId       Represents the request identifier
                     * @param[in]   requestPayload  Represents the request payload
                     * @param[in]   responsePayload Represents the response payload
                     */
                    virtual void onRequestComplete(int requestId, std::string requestPayload,
                                                   std::string responsePayload);

                private:

                    OC::Cm::Notification::ISubscriptionDB *m_pDatabase
                    ;    /**< Represents the instance of ISubscriptionDB */

                    OC::Cm::Notification::INotificationListener *m_pNotificationListener
                    ;  /**< Represents the INotificationListener class */

                    OC::Cm::Notification::ISubscriptionProvider *m_pSubscriptionProvider
                    ;  /**< Represents the ISubscriptionProvider Class */

                    static int s_subscriptionIdCounter;     /**< Represents the counter for subscriptions */

                    std::map< int,  SubscriptionRequest > m_pResponseListeners
                    ;    /**< Represents the map of response listeners */

                    OC::Cm::IContext
                    *m_pContext;       /**< Represents teh instance of Control Manager Context where Control Manager has started */

                    std::string m_deviceAddress
                    ;       /**< Represents the device address where subscription manager is running */

                    std::string m_protocol ;            /**< Represents the device protocol */

                    std::string mSelfUUID ;             /**< Represent the device UUID */

                    /**
                     * Sending subscription request.
                     *
                     * @param[in]   uri         Represents the resource uri where request has to send
                     * @param[in]   method      Represents the REST method for the request
                     * @param[in]   data        Represents the ISerializable object of request
                     * @param[out]  requestId   Represents the request identifier
                     */
                    bool sendRequest(std::string &uri, std::string method, OC::Cm::Serialization::ISerializable *data,
                                     int &requestId );

                    /**
                     * Sending subscription request.
                     *
                     * @param[in]   protocol    Represents the device protocol
                     * @param[in]   deviceAddress   Represents the device address
                     * @param[in]   uri         Represents the resource uri where request has to send
                     * @param[in]   method      Represents the REST method for the request
                     * @param[in]   data        Represents the ISerializable object of request
                     * @param[out]  requestId   Represents the request identifier
                     */
                    bool sendRequest(std::string &protocol, std::string &deviceAddress, std::string &relativeUri,
                                     std::string method, OC::Cm::Serialization::ISerializable *data, int &requestId ) ;

                    /**
                     * Gets the device instance by using device address
                     *
                     * @param[in]   deviceAddress   Represents the device address
                     *
                     * @return Returns the instance of Device class
                     */
                    OC::Cm::Device *getDeviceByAddress(std::string &deviceAddress) ;

                    /**
                     * Gets the device instance by using device uuid
                     *
                     * @param[in]   uuid    Represents the device uuid
                     *
                     * @return Returns the instance of Device class
                     */
                    OC::Cm::Device *getDeviceByUUID(std::string &uuid) ;

                    /**
                     * Gets the subscriptions link
                     *
                     * @return Returns the subscription link
                     */
                    std::string getSubscriptionsLink();

                    /**
                     * Gets the notification link
                     *
                     * @return Returns the notification link
                     */
                    std::string getNotificationLink();

            };

        }
    }
}

#endif  //_SUBSCRIPTIONMANAGER_H
