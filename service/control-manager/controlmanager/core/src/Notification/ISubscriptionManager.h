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

#include "CMConfig.h"
#include "list"
#include "string"
#include "map"
#include "NotificationResponse.h"
#include "CMSerialization.h"
#include "Serialization/SerializableDataFactory.h"
#include "CMServerResourceHandler.h"

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
            class INotificationListener ;
            class ISubscriptionHandler ;
            class ISubscriptionProvider ;

            /**
            *  @addtogroup Notification
            *  @{
            */

            /**
             *
             * @interface   ISubscriptionManager
             * @brief       Defines methods required to use subscription and notification functionality
             *
             * @since       1.0
             *
             * This interface defines methods required to use subscription and notification functionality
             *
             */
            class CMAPI ISubscriptionManager
            {
                public :

                    /**
                     * Default destructor
                     */
                    virtual ~ISubscriptionManager() {}

                    /**
                     * initializes the Subscription manager.
                     *
                     * @param[in] pContext  context of the framework instance with which this object is associated
                     * @param[in] dbFilePath subscription store path
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     * @note: framework will invoke this method during framework initialization
                     */
                    virtual bool init(OC::Cm::IContext *pContext, std::string &dbFilePath ) = 0 ;

                    /**
                     * deinitializes the framework
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     * @note: framework will invoke this method during framework deinitialization
                     */
                    virtual bool deinit() = 0;

                    /**
                     * configures self device details
                     *
                     * @param[in] protocol  communication protocol supported by device
                     * @param[in] address address of the devuce
                     * @param[in] uuid uuid of the device
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     * @note: framework will invoke this method during framework start
                     */
                    virtual bool setDeviceAddress(std::string protocol, std::string address, std::string uuid) = 0;


                    /**
                     * returns the list of resource paths of specified device to which this device is currently subscribed
                     *
                     * @param[in] uuid uuid of the device
                     *
                     * @return returns list of subscribed resource paths
                     *
                     */
                    virtual std::list< std::string > getMySubscriptions(std::string uuid) = 0;

                    /**
                     * returns serialization factory used by this implementaion
                     *
                     * @return returns instance of serialization factory
                     *
                     */
                    virtual OC::Cm::Serialization::SerializableDataFactory *getSerializationFactory() = 0;

                    /**
                     * returns resource handler factory used by this implementaion
                     *
                     * @return returns instance of resource handler factory
                     *
                     */
                    virtual OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *getResourceHandlerFactory() = 0;


                    /**
                     * configures subscription provider
                     *
                     * @param[in] subscriptionProvider instance of subscription provider
                     *
                     */
                    virtual void addSubscriptionProvider ( OC::Cm::Notification::ISubscriptionProvider
                                                           &subscriptionProvider) = 0;

                    /**
                     * removes previously configured subscription provider
                     *
                     * @param[in] subscriptionProvider instance of subscription provider
                     *
                     */
                    virtual void removeSubscriptionProvider ( OC::Cm::Notification::ISubscriptionProvider
                            &subscriptionProvider ) = 0;

                    /**
                     *  Adds a Notification listener. Application can receive notification through this listener
                     *
                     *  @param[in]  notificationListener    instance of notification listener
                     *
                     * @return Return @c true in case of success @n@c false in case of any failure
                     */
                    virtual bool addNotificationListener ( OC::Cm::Notification::INotificationListener
                                                           &notificationListener ) = 0;

                    /**
                     *  removes previously added notification listener
                     *
                     *  @param[in]  notificationListener    instance of notification listener
                     */
                    virtual void removeNotificationListener ( OC::Cm::Notification::INotificationListener
                            &notificationListener ) = 0;

                    /**
                     *
                     * Sends a subscription request
                     *
                     * @param[out] requestId        request id
                     * @param[in] protocol          protocol to be used to send the request (http or https)
                     * @param[in] duuid             uuid of the device to which request needs to be sent (destination)
                     * @param[in] deviceAddress     address of the device to which request needs to be sent
                     * @param[in] suuid             uuid of the device which wants to subscribe (source)
                     * @param[in] resourcePaths     list of resource path to subscribe
                     * @param[in] notificationUri   notification uri. framework will use internal notification uri if application specified empty string to this argument
                     * @param[in] listener          listener to receive subscription response
                     * @param[in] description       desciption of the subsciption
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     * @see OC::Cm::Client::Resource::IResourceResponseListener::onSubscribeResponseReceived()
                     *
                     */
                    virtual bool addSubscription( int &requestId,
                                                  std::string protocol,
                                                  std::string duuid,
                                                  std::string deviceAddress,
                                                  std::string suuid,
                                                  std::list<std::string> resourcePaths,
                                                  const std::string notificationUri,
                                                  OC::Cm::Client::Resource::IResourceResponseListener *listener,
                                                  std::string description) = 0;

                    /**
                     *
                     * Sends a unsubscribe request to a list of resource paths. this request will send more than one requests
                     *
                     *
                     * @param[in] uuid              uuid of the device to which request needs to be sent
                     * @param[in] deviceAddress     address of the device to which request needs to be sent
                     * @param[in] protocol          protocol to be used to send the request (http or https)
                     * @param[in] resourcePaths     list of resource paths to unsubscribe
                     * @param[in] listener          listener to receive subscription response
                     * @param[out] requestIDMap     map containing ids of the requests sent by this api.
                     *
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     * @see OC::Cm::Client::Resource::IResourceResponseListener::onUnSubscribeResponseReceived()
                     *
                     */
                    virtual bool removeSubscription(  std::string &uuid,
                                                      std::string &deviceAddress,
                                                      std::string &protocol,
                                                      std::list<std::string> resourcePaths,
                                                      OC::Cm::Client::Resource::IResourceResponseListener *listener,
                                                      std::map< int, std::list< std::string > > &requestIDMap ) = 0;


                    /**
                     *
                     * Sends a unsubscribe request to subscription uri
                     *
                     * @param[in] uuid              uuid of the device to which request needs to be sent
                     * @param[in] deviceAddress     address of the device to which request needs to be sent
                     * @param[in] subscriptionUri   subscription uri
                     * @param[in] protocol          protocol to be used to send the request (http or https)
                     * @param[in] listener          listener to receive subscription response
                     * @param[out] requestId        request id.
                     *
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     * @see OC::Cm::Client::Resource::IResourceResponseListener::onUnSubscribeResponseReceived()
                     *
                     */
                    virtual bool removeSubscription( std::string &uuid,
                                                     std::string &deviceAddress,
                                                     std::string subscriptionUri,
                                                     std::string &protocol,
                                                     OC::Cm::Client::Resource::IResourceResponseListener *listener, int &requestId ) = 0 ;

                    /**
                     *
                     * This method is used to send notifications to subscribed devices.
                     *
                     * @param[in] notificationData  notification data
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     */
                    virtual bool notify(  NotificationResponse *notificationData) = 0;

                    /**
                     *
                     * This method is used to send notifications of multiple resources to subscribed devices.
                     *
                     * @param[in] notificationDataList      List of notification data objects
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     */
                    virtual bool notify( std::list< NotificationResponse * > notificationDataList) = 0;



                    /**
                     *
                     * This method is used to broadcast notifications to all Control Manager devices.
                     *
                     * Currently Not Implemented
                     *
                     * @param[in] resourcePath      resource path
                     * @param[in] notificationData  notification data
                     * @param[in] notificationType  element type to which notification data belongs.
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     *
                     */
                    virtual bool notifyAll( std::string &resourcePath,
                                            std::string notificationType,
                                            OC::Cm::Serialization::ISerializable &notificationData) = 0;

                    /**
                     * returns subscription handler which is handling requests related to subscription
                     *
                     * @return returns instance of subscription handler
                     */
                    virtual OC::Cm::Notification::ISubscriptionHandler *getSubscriptionHandler() = 0 ;
            };
            /** @} */
        }
    }
}


#endif /* _ISUBSCRIPTIONMANAGER_H */
