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

#ifndef _ISUBSCRIPTIONMANAGER_H
#define _ISUBSCRIPTIONMANAGER_H

#include "list"
#include "string"
#include "map"
#include "NotificationResponse.h"
#include "SerializableDataFactory.h"
#include "ResourceHandlerFactory.h"

namespace OC
{
    namespace Cm
    {
        class IContext;

        namespace Client
        {
            namespace Resource
            {
                class IResourceResponseListener;
            }
        }
        namespace Serialization
        {
            class ISerializable;
        }
        namespace Notification
        {
            class INotificationListener;
            class ISubscriptionHandler;
            class ISubscriptionProvider;

            /**
             *
             * @interface   ISubscriptionManager
             * @brief       Defines methods required to use subscription and notification functionality
             *
             * This interface defines methods required to use subscription and notification functionality
             *
             */
            class ISubscriptionManager
            {
                public:

                    /**
                     *  ~ISubscriptionManager - Default destructor
                     *  @param -none
                     *  @return -none
                     */
                    virtual ~ISubscriptionManager()
                    {
                    }

                    /**
                     * init - initializes the Subscription manager.
                     *
                     * @param pContext  context of the framework instance with which this object is associated
                     * @param dbFilePath subscription store path
                     *
                     * @return  type:bool - returns true if request is sent successfully, false in case of any error
                     *
                     */
                    virtual bool init(OC::Cm::IContext *pContext, std::string &dbFilePath) = 0;

                    /**
                     * deinit - deinitializes the framework
                     * @return  type:bool returns true if request is sent successfully false in case of any error
                     *
                     */
                    virtual bool deinit() = 0;

                    /**
                     * setDeviceAddress - configures self device details
                     *
                     * @param protocol  communication protocol supported by device
                     * @param address address of the devuce
                     * @param uuid uuid of the device
                     *
                     * @return  type:bool returns  true if request is sent successfully, false in case of any error
                     *
                     */
                    virtual bool setDeviceAddress(std::string protocol, std::string address,
                                                  std::string uuid) = 0;

                    /**
                     * getMySubscriptions - returns the list of resource paths of specified device to which this device is currently subscribed
                     *
                     * @param uuid uuid of the device
                     *
                     * @return type:list< std::string > - returns list of subscribed resource paths
                     *
                     */
                    virtual std::list<std::string> getMySubscriptions(std::string uuid) = 0;

                    /**
                     * getSerializationFactory - returns serialization factory used by this implementaion
                     * @param -none
                     * @return type:SerializableDataFactory* -returns instance of serialization factory
                     *
                     */
                    virtual OC::Cm::Serialization::SerializableDataFactory *getSerializationFactory() = 0;

                    /**
                     * getResourceHandlerFactory - returns resource handler factory used by this implementaion
                     *  @param -none
                     * @return type:ResourceHandlerFactory* - returns instance of resource handler factory
                     *
                     */
                    virtual OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *getResourceHandlerFactory() = 0;

                    /**
                     * configures subscription provider
                     *
                     * @param subscriptionProvider instance of subscription provider
                     *
                     */
                    virtual void addSubscriptionProvider(
                        OC::Cm::Notification::ISubscriptionProvider &subscriptionProvider) = 0;

                    /**
                     * removes previously configured subscription provider
                     *
                     * @param[ subscriptionProvider instance of subscription provider
                     * @return -none
                     *
                     */
                    virtual void removeSubscriptionProvider(
                        OC::Cm::Notification::ISubscriptionProvider &subscriptionProvider) = 0;

                    /**
                     *  addNotificationListener - Adds a Notification listener. Application can receive notification through this listener
                     *
                     *  @param  notificationListener    instance of notification listener
                     *
                     * @return type:bool - Return  true in case of success false in case of any failure
                     */
                    virtual bool addNotificationListener(
                        OC::Cm::Notification::INotificationListener &notificationListener) = 0;

                    /**
                     *  removeNotificationListener- removes previously added notification listener
                     *
                     *  @param  notificationListener    instance of notification listener
                     *  @return -none
                     */
                    virtual void removeNotificationListener(
                        OC::Cm::Notification::INotificationListener &notificationListener) = 0;

                    /**
                     *
                     * addSubscription - Sends a subscription request
                     *
                     * @param requestId     request id (output)
                     * @param protocol          protocol to be used to send the request (http or https)
                     * @param duuid             uuid of the device to which request needs to be sent (destination)
                     * @param deviceAddress     address of the device to which request needs to be sent
                     * @param suuid             uuid of the device which wants to subscribe (source)
                     * @param resourcePaths     list of resource path to subscribe
                     * @param notificationUri   notification uri. framework will use internal notification uri if application specified empty string to this argument
                     * @param listener          listener to receive subscription response
                     * @param description       desciption of the subsciption
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     * @see OC::Cm::Client::Resource::IResourceResponseListener::onSubscribeResponseReceived()
                     *
                     */
                    virtual bool addSubscription(int &requestId, std::string protocol,
                                                 std::string duuid, std::string deviceAddress, std::string suuid,
                                                 std::list<std::string> resourcePaths,
                                                 const std::string notificationUri,
                                                 OC::Cm::Client::Resource::IResourceResponseListener *listener,
                                                 std::string description) = 0;

                    /**
                     *
                     * removeSubscription - Sends a unsubscribe request to a list of resource paths. this request will send more than one requests
                     *
                     *
                     * @param uuid              uuid of the device to which request needs to be sent
                     * @param deviceAddress     address of the device to which request needs to be sent
                     * @param protocol          protocol to be used to send the request (http or https)
                     * @param resourcePaths     list of resource paths to unsubscribe
                     * @param listener          listener to receive subscription response
                     * @param requestIDMap      map containing ids of the requests sent by this api.(output)
                     *
                     *
                     * @return  type:bool - returns true if request is sent successfully, false in case of any error
                     *
                     */
                    virtual bool removeSubscription(std::string &uuid,
                                                    std::string &deviceAddress, std::string &protocol,
                                                    std::list<std::string> resourcePaths,
                                                    OC::Cm::Client::Resource::IResourceResponseListener *listener,
                                                    std::map<int, std::list<std::string> >& requestIDMap) = 0;

                    /**
                     *
                     * removeSubscription - Sends a unsubscribe request to subscription uri
                     *
                     * @param uuid              uuid of the device to which request needs to be sent
                     * @param deviceAddress     address of the device to which request needs to be sent
                     * @param subscriptionUri   subscription uri
                     * @param protocol          protocol to be used to send the request (http or https)
                     * @param listener          listener to receive subscription response
                     * @param requestId     request id.(output)
                     *
                     *
                     * @return  type:bool returns true if request is sent successfully, false in case of any error
                     *
                     *
                     */
                    virtual bool removeSubscription(std::string &uuid,
                                                    std::string &deviceAddress, std::string subscriptionUri,
                                                    std::string &protocol,
                                                    OC::Cm::Client::Resource::IResourceResponseListener *listener,
                                                    int &requestId) = 0;

                    /**
                     *
                     * notify - This method is used to send notifications to subscribed devices.
                     *
                     * @param notificationData  notification data
                     *
                     * @return  returns true if request is sent successfully, false in case of any error
                     *
                     */
                    virtual bool notify(NotificationResponse *notificationData) = 0;

                    /**
                     *
                     * notify - This method is used to send notifications of multiple resources to subscribed devices.
                     *
                     * @param notificationDataList      List of notification data objects
                     *
                     * @return  type:bool returns  true if request is sent successfully, false in case of any error
                     *
                     */
                    virtual bool notify(
                        std::list<NotificationResponse *> notificationDataList) = 0;

                    /**
                     *
                     * notifyAll - This method is used to broadcast notifications to all SHP devices.
                     *
                     * Currently Not Implemented
                     *
                     * @param resourcePath      resource path
                     * @param notificationData  notification data
                     * @param notificationType  element type to which notification data belongs.
                     *
                     * @return  returns true if request is sent successfully, false in case of any error
                     *
                     */
                    virtual bool notifyAll(std::string &resourcePath,
                                           std::string notificationType,
                                           OC::Cm::Serialization::ISerializable &notificationData) = 0;

                    /**
                     *getSubscriptionHandler -  returns subscription handler which is handling requests related to subscription
                     *@param -none
                     * @return returns instance of subscription handler
                     */
                    virtual OC::Cm::Notification::ISubscriptionHandler *getSubscriptionHandler() = 0;
            };
        }
    }
}

#endif /* _ISUBSCRIPTIONMANAGER_H */
