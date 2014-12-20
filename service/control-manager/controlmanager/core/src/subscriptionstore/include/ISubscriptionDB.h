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



#ifndef ISUBSCRIPTIONDB_H_
#define ISUBSCRIPTIONDB_H_

#include "SubscriptionRecord.h"
#include "SubscriptionStoreConfig.h"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {
            /**
             * @class       ISubscriptionDB
             * @brief       This is an interface which implements Subscription database.
             */
            class SUBSCRIPTIONSTORE_API ISubscriptionDB
            {

                public:

                    /**
                     * Default destructor
                     */
                    virtual ~ISubscriptionDB() {} ;

                    /**
                     * Initializes the subscription store
                     *
                     * @param[in]   dbPath  Represents absolute path where DB can store into.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool init(std::string &dbPath) = 0 ;

                    /**
                     * DeInitializes the subscription store
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool deinit() = 0 ;

                    /**
                     * Saves a Pending add subscription record... this record will be deleted on receiving subscription response
                     *
                     * @param[in]   requestId   Represents the request identifier.
                     * @param[in]   subscription    Represents the subscription object of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool savePendingMySubscription( int requestId,
                                                            OC::Cm::Notification::SubscriptionRecord &subscription ) = 0;

                    /**
                     * Delete a pending subscription record.
                     *
                     * @param[in]   requestId   Represents the request identifier.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool deletePendingMySubscription( int requestId ) = 0;

                    /**
                     * Updates subscription record.
                     *
                     * @param[in]   requestId   Represents the request identifier.
                     * @param[in]   subscriptionUri Represents the subscription URI
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool updateMySubscription(int requestId, std::string &subscriptionUri ) = 0 ;

                    /**
                     * Update subscription record of specified subscription uri.
                     *
                     * @param[in]   subscriptionUri Represents the subscription URI
                     * @param[in]   resourcePaths   Represents the subscription resource path
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool updateMySubscriptionBySubscriptionUri( std::string &subscriptionUri,
                            std::string &resourcePaths ) = 0;

                    /**
                     * Deletes subscription record of specified subscription uri.
                     *
                     * @param[in]   deviceId    Represents the device identifier.
                     * @param[in]   subscriptionUri Represents the subscription URI
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool deleteMySubscriptionBySubscriptionUri( std::string &deviceId,
                            std::string &subscriptionUri ) = 0;

                    /**
                     * Get the subscription record of specified subscription uri.
                     *
                     * @param[in]   uri             Represents the subscription URI
                     * @param[out]  subscription    Represents the subscription object of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool getMySubscriptionBySubscriptionUri( std::string &uri,
                            OC::Cm::Notification::SubscriptionRecord &subscription) = 0;

                    /**
                     * Gets the subscription record of specified device identifier
                     *
                     * @param[in]   deviceId    Represents the device identifier.
                     * @param[out]  subscriptions   Represents the list of subscription objects of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool getMySubscriptionByDeviceId( std::string &deviceId,
                            std::list<OC::Cm::Notification::SubscriptionRecord> &subscriptions ) = 0;

                    /**
                     * Gets list of subscription records of specified device identifier and resource path
                     *
                     * @param[in]   deviceId    Represents the device identifier.
                     * @param[in]   subscribedResourcePath  Represents the subscription resource path.
                     * @param[out]  subscriptionList    Represents the list of subscription objects of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool getMySubscriptionByResourcePaths(std::string &deviceId,
                            std::string &subscribedResourcePath,
                            std::list<OC::Cm::Notification::SubscriptionRecord> &subscriptionList) = 0 ;

                    /**
                     * Gets list of subscription records of specified device identifier and resource paths
                     *
                     * @param[in]   deviceUUID  Represents the device identifier.
                     * @param[in]   resourcePathList    Represents the list of subscription resource path.
                     * @param[out]  subscriptions   Represents the list of subscription objects of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool getMySubscriptionContainingResourceList( std::string deviceUUID,
                            std::list<std::string> resourcePathList,
                            std::list<OC::Cm::Notification::SubscriptionRecord> &subscriptions ) = 0;

                    /**
                     * Saves the subscription which has passed.
                     *
                     * @param[in] subscription  Represents subscription instance of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool saveSubscription( OC::Cm::Notification::SubscriptionRecord &subscription ) = 0;

                    /**
                     * Gets the list of subscription records of specified resource path
                     *
                     * @param[in]   resourcePath    Represents the resource path
                     * @param[in]   subscriptions   Represents the list of subscription objects of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool getSubscriptionContainingResource( std::string &resourcePath,
                            std::list<OC::Cm::Notification::SubscriptionRecord> &subscriptions) = 0 ;

                    /**
                     * Gets the list of subscription records of specified list of resource paths.
                     *
                     * @param[in]   resourcePathList    Represents the list of resource paths
                     * @param[in]   subscriptions   Represents the subscription object of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool getSubscriptionContainingResourceList( std::list<std::string> resourcePathList,
                            std::list<OC::Cm::Notification::SubscriptionRecord> &subscriptions) = 0;

                    /**
                     * Gets the subscription record of subscription identifier
                     *
                     * @param[in]   subscriptionId  Represents the subscription identifier.
                     * @param[in]   subscription    Represents the subscription object of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool getSubscriptionBySubscriptionID( std::string &subscriptionId,
                            OC::Cm::Notification::SubscriptionRecord &subscription) = 0;

                    /**
                     * Updating subscription record.
                     *
                     * @param[in]   subscription    Represents the subscription object of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool updateSubscription(OC::Cm::Notification::SubscriptionRecord &subscription) = 0;

                    /**
                     * Deletes the subscription record of specified subscription identifier
                     *
                     * @param[in]   subscriptionId  Represents the subscription identifier.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool deleteSubscriptionBySubscriptionID(std::string &subscriptionId) = 0;

                    /**
                     * Gets the list of subscription records.
                     *
                     * @param[in]   subscriptions   Represents the subscription object of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool getSubscriptions(std::list<OC::Cm::Notification::SubscriptionRecord> &subscriptions) =
                        0;
            };

        }
    }
}

#endif /* ISUBSCRIPTIONDB_H_ */
