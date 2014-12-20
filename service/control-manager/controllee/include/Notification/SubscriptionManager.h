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

#include "ISerializable.h"
#include "ISubscriptionManager.h"
#include "ISubscriptionHandler.h"
#include "IContext.h"

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
                SRT_REMOVE_SUBSCRIPTION,   //!< SRT_REMOVE_SUBSCRIPTION
                SRT_SEND_NOTIFICATION,  //!< SRT_SEND_NOTIFICATION
                SRT_UPDATE_SUBSCRIPTION //!< SRT_UPDATE_SUBSCRIPTION

            } SubscriptionReqType; /**< Represents the enum _SubscriptionReqType */

            /**
             * @class       SubscriptionRequest
             * @brief       This is an class implements the subscription request
             */
            class SubscriptionRequest
            {

                public:

                    SubscriptionReqType m_reqType; /**< Represents the type of subscription request */
                    std::string m_device_address; /**< Represents the device address */
                    std::string m_subscription_uri; /**< Represents the subscription uri */
                    std::string m_uuid; /**< Represents the device UUID */
            };

            /**
             * @class       SubscriptionManager
             * @brief       This is an class implements the subscription manager
             */
            class SubscriptionManager: public OC::Cm::Notification::ISubscriptionManager,
                public OC::Cm::Notification::ISubscriptionHandler
            {

                public:

                    /**
                     * SubscriptionManager - Parameterized constructor
                     *
                     * @param   pDb Represents the instance of ISubscrionDB class.
                     * @return -none
                     */
                    SubscriptionManager(ISubscriptionDB *pDb);

                    /**
                     * ~SubscriptionManager - Default destructor
                     * @param -none
                     * @return -none
                     */
                    ~SubscriptionManager();

                    /**
                     * init - Initializing subscription manager.
                     *
                     * @param   pContext    Represents the instance of SHP Context where SHP has started
                     * @param   dbFilePath  Represents the database file path
                     *
                     * @return type:bool Returns rue if success, false if failure
                     */
                    bool init(OC::Cm::IContext *pContext, std::string &dbFilePath);

                    /**
                     *  deinit - De-initializing Subscription manager
                     *
                     * @param - none
                     * @return type:bool Returns true if success, false if failure
                     */
                    bool deinit();

                    /**
                     * setDeviceAddress - Setting device address, protocol and device uuid.
                     *
                     * @param   protocol    Represents the device protocol
                     * @param   address     Represents the device address
                     * @param   uuid        Represents the device uuid
                     *
                     * @return type:bool Returns true if success, false if failure
                     */
                    bool setDeviceAddress(std::string protocol, std::string address,
                                          std::string uuid);

                    /**
                     * getMySubscriptions - Get list of subscription of specified device uuid.
                     *
                     * @param   uuid    Represents the device UUID.
                     *
                     * @return type:list<string>    Returns the list of subscriptions as strings
                     */
                    std::list<std::string> getMySubscriptions(std::string uuid);

                    /**
                     * getSerializationFactory - Gets the SerializableDataFactory instance.
                     * @param -none
                     *
                     * @returns type:SerializableDataFactory* Returns SerializableDataFactory instance.
                     */
                    OC::Cm::Serialization::SerializableDataFactory *getSerializationFactory();

                    /**
                     * Gets the ResourceHandlerFactory instance.
                     * @param -none
                     * @returns ResourceHandlerFactory* - Returns ResourceHandlerFactory instance.
                     */
                    OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *getResourceHandlerFactory();

                    /**
                     * addSubscriptionProvider - Adding subscription provider
                     *
                     * @param subscriptionProvider  Represents the instance of ISubscriptionProvider
                     * @return -none
                     */
                    void addSubscriptionProvider(
                        OC::Cm::Notification::ISubscriptionProvider &subscriptionProvider);

                    /**
                     * removeSubscriptionProvider - Removing subscription provider
                     *
                     * @param   subscriptionProvider    Represents the instance of ISubscriptionProvider
                     * @return -none
                     */
                    void removeSubscriptionProvider(
                        OC::Cm::Notification::ISubscriptionProvider &subscriptionProvider);

                    /**
                     * addNotificationListener - Adding notification listener
                     *
                     * @param   notificationListener    Represents the instance of INotificationListener
                     *
                     * @return type:bool Return true in case of success, false in case of any failure
                     */
                    bool addNotificationListener(
                        OC::Cm::Notification::INotificationListener &notificationListener);

                    /**
                     * removeNotificationListener - Removing notification listener
                     *
                     * @param   notificationListener    Represents the instance of INotificationListener
                     * @return -none
                     */
                    void removeNotificationListener(
                        OC::Cm::Notification::INotificationListener &notificationListener);

                    /**
                     * addSubscription - Adding subscription to the subscription manager
                     *
                     * @param   requestId       Represents the request identifier(output)
                     * @param   protocol        Represents the protocol which subscription manager used for sending notifications
                     * @param   duuid           Represents  peer device uuid
                     * @param   deviceAddress   Represents the peer device address
                     * @param   suuid           Represents the its own device uuid
                     * @param   resourcePaths   Represents the list of resource paths
                     * @param   notificationUri Represents the notification uri
                     * @param   listener        Represents the resource response listener to the application
                     * @param   description     Represents the desciption of the subscription
                     *
                     * @return type:bool  Returns true if success, false if failure
                     */
                    bool addSubscription(int &requestId, std::string protocol,
                                         std::string duuid, std::string deviceAddress, std::string suuid,
                                         std::list<std::string> resourcePaths,
                                         const std::string notificationUri,
                                         OC::Cm::Client::Resource::IResourceResponseListener *listener,
                                         std::string description);

                    /**
                     * removeSubscription - Removing subscription from the subscription manager
                     *
                     * @param   uuid            Represents the device UUID
                     * @param   deviceAddress   Represents the device address
                     * @param   subscriptionUri Represents the subscription uri
                     * @param   protocol        Represents the device protocol
                     * @param   listener        Represents the resource response listener
                     * @param   requestId       Represents the request identifier (output)
                     *
                     * @return type:bool Returns true if success, false if failure
                     */
                    bool removeSubscription(std::string &uuid, std::string &deviceAddress,
                                            std::string subscriptionUri, std::string &protocol,
                                            OC::Cm::Client::Resource::IResourceResponseListener *listener,
                                            int &requestId);

                    /**
                     * removeSubscription - Removing subscription from the subscription manager
                     *
                     * @param   uuid            Represents the device UUID
                     * @param   deviceAddress   Represents the device address
                     * @param   protocol        Represents the device protocol
                     * @param   resourcePaths   Represents the list of resource paths
                     * @param   listener        Represents the resource response listener
                     * @param   requestIDMap        Represents the map of request identifiers(output)
                     *
                     * @return type:bool Returns  true if success, false if failure
                     */
                    bool removeSubscription(std::string &uuid, std::string &deviceAddress,
                                            std::string &protocol, std::list<std::string> resourcePaths,
                                            OC::Cm::Client::Resource::IResourceResponseListener *listener,
                                            std::map<int, std::list<std::string> >& requestIDMap);

                    /**
                     * notify - Sending notification to the notification resource
                     *
                     * @param  notificationData Represents the instance of NotificationReponse class
                     *
                     * @return type:bool Returns true if success, false if failure
                     */
                    bool notify(NotificationResponse *notificationData);

                    /**
                     * notify - Sending notification to the notification resource
                     *
                     * @param  notificationDataList Represents the list of instances of NotificationReponse class
                     *
                     * @return type:bool Returns true if success, false if failure
                     */
                    bool notify(std::list<NotificationResponse *> notificationDataList);

                    /**
                     * notifyAll - Sending notification to the notification resource
                     *
                     * @param   resourcePath        Represents the resource path
                     * @param   notificationType    Represents the type of notification
                     * @param   notificationData    Represents the serializable object of notification
                     *
                     * @return Returns  type:bool true if success, false if failure
                     */
                    bool notifyAll(std::string &resourcePath, std::string notificationType,
                                   OC::Cm::Serialization::ISerializable &notificationData);

                    /**
                     * getSubscriptionHandler - Gets the subscription handler
                     * @param -none
                     * @return Returns the instance of ISubscriptionHandler class
                     */
                    OC::Cm::Notification::ISubscriptionHandler *getSubscriptionHandler();

                    /**
                     * handleGetSubscriptionsRequest - Handles the GET subscription request
                     *
                     * @param   response    Represents the instance of ISerializable class
                     * @param   statusCode  Represents the status code of the request.(output)
                     *
                     * @return type:bool Returns the instance of ISubscriptionHandler class
                     */
                    bool handleGetSubscriptionsRequest(
                        OC::Cm::Serialization::ISerializable *response, int &statusCode);

                    /**
                     * handleUpdateSubscriptionRequest - Handles the subscription request
                     *
                     * @param   deviceAddress   Represents the device address
                     * @param   request Represents the instance of ISerializable class
                     * @param   location    Represents the resource location (output)
                     * @param   statusCode  Represents the status code of the request.(output)
                     *
                     * @return type:bool - Returns true if request handling is successful . else false
                     */
                    bool handleSubscriptionRequest(std::string &deviceAddress,
                                                   OC::Cm::Serialization::ISerializable *request,
                                                   std::string &location, int &statusCode);

                    /**
                     * handleUpdateSubscriptionRequest - Handles the update subscription request
                     *
                     * @param   deviceAddress   Represents the device address
                     * @param   subscriptionId  Represents the subscription identifier
                     * @param   request Represents the instance of ISerializable class
                     * @param   statusCode  Represents the status code of the request.(output)
                     *
                     * @return type:bool Returns true if request handling is successful . else false
                     */
                    bool handleUpdateSubscriptionRequest(std::string &deviceAddress,
                                                         std::string &subscriptionId,
                                                         OC::Cm::Serialization::ISerializable *request, int &statusCode);
                    /**
                     * handleDeleteSubscriptionRequest - Handles the delete subscription request
                     *
                     * @param   deviceAddress   Represents the device address
                     * @param   subscriptionId  Represents the subscription identifier
                     * @param   statusCode  Represents the status code of the request.(output)
                     *
                     * @return type:bool  Returns true if request handling is successful . else false
                     */
                    bool handleDeleteSubscriptionRequest(std::string &deviceAddress,
                                                         std::string &subscriptionId, int &statusCode);

                    /**
                     * handleGetSubscriptionUri -Handles the GET subscription request
                     *
                     * @param   deviceAddress   Represents the device address
                     * @param   subscriptionId  Represents the subscription identifier
                     * @param   response    Represents the instance of ISerializable class
                     * @param   statusCode  Represents the status code of the request.(output)
                     *
                     * @return type:bool Returns true if request handling is successful . else false
                     */
                    bool handleGetSubscriptionUri(std::string &deviceAddress,
                                                  std::string &subscriptionId,
                                                  OC::Cm::Serialization::ISerializable *response, int &statusCode);
                    /**
                     * handleNotification - Handles the notification
                     *
                     * @param   deviceAddress   Represents the device address
                     * @param   request Represents the instance of ISerializable class
                     * @param   statusCode  Represents the status code of the request.(output)
                     *
                     * @return  type:bool Returns true if request handling is successful, else false
                     */
                    bool handleNotification(std::string &deviceAddress,
                                            OC::Cm::Serialization::ISerializable *request, int &statusCode);

                    /**
                     * onNotifyResponseReceived - This is a callback when response on notification received, this will called
                     *
                     * @param   clientSessionId Represents the client session ID by which request sent
                     * @param   status          Represents the http status code of the response
                     * @return -none
                     */
                    virtual void onNotifyResponseReceived(int clientSessionId, int status);

                private:
                    OC::Cm::Notification::ISubscriptionDB *m_pDatabase; /**< Represents the instance of ISubscriptionDB */
                    OC::Cm::Notification::INotificationListener *m_pNotificationListener; /**< Represents the INotificationListener class */
                    OC::Cm::Notification::ISubscriptionProvider *m_pSubscriptionProvider; /**< Represents the ISubscriptionProvider Class */
                    static int s_subscriptionIdCounter; /**< Represents the counter for subscriptions */
                    OC::Cm::IContext *m_pContext; /**< Represents teh instance of SHP Context where SHP has started */
                    std::string m_deviceAddress; /**< Represents the device address where subscription manager is running */
                    std::string m_protocol; /**< Represents the device protocol */
                    std::string mSelfUUID; /**< Represent the device UUID */
                    /**
                     * sendRequest - Sending subscription request.
                     *
                     * @param   uri         Represents the resource uri where request has to send
                     * @param   method      Represents the REST method for the request
                     * @param   data        Represents the ISerializable object of request
                     * @param   requestId   Represents the request identifier(output)
                     * @return type:bool - true if successful, else false
                     */
                    bool sendRequest(std::string &uri, std::string method,
                                     OC::Cm::Serialization::ISerializable *data, int &requestId);

                    /**
                     * sendRequest - Sending subscription request.
                     *
                     * @param   protocol    Represents the device protocol
                     * @param   deviceAddress   Represents the device address
                     * @param   uri         Represents the resource uri where request has to send
                     * @param   method      Represents the REST method for the request
                     * @param   data        Represents the ISerializable object of request
                     * @param   requestId   Represents the request identifier(output)
                     * @return type:bool - true if successful, else false
                     */
                    bool sendRequest(std::string &protocol, std::string &deviceAddress,
                                     std::string &relativeUri, std::string method,
                                     OC::Cm::Serialization::ISerializable *data, int &requestId);

                    /**
                     * getSubscriptionsLink - Gets the subscriptions link
                     * @param -none
                     * @return type:string Returns the subscription link
                     */
                    std::string getSubscriptionsLink();

                    /**
                     * getNotificationLink - Gets the notification link
                     * @param -none
                     * @return type:string Returns the notification link
                     */
                    std::string getNotificationLink();

            };

        }
    }
}

#endif  //_SUBSCRIPTIONMANAGER_H
