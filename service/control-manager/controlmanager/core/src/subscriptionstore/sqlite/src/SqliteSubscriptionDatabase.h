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



#ifndef SUBSCRIPTIONDATABASE_H_
#define SUBSCRIPTIONDATABASE_H_


#include "list"
#include "SubscriptionRecord.h"
#include "ISubscriptionDB.h"
#include "SubscriptionStoreConfig.h"

class sqlite3 ;

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            /**
             * @class       SqliteSubscriptionDatabase
             * @brief       This is class implements the subscription database by using Sqlite.
             */
            class SUBSCRIPTIONSTORE_API SqliteSubscriptionDatabase : public ISubscriptionDB
            {

                public:

                    /**
                     * Default constructor
                     */
                    /************************************************************************************
                     * Table Schema                                                                     *
                     *                                                                                  *
                     *--------------------------------------    ----------------------------------------*
                     * requestId | subscribedResource | deviceId | subscriptionUri | NotificationUri    *
                     *----------------------------------------------------------------------------------*
                     *                                                                                  *
                     *  requestId is valid for pending my subscriptions, for others it is invalid       *
                     *                                                                                  *
                     *                                                                                  *
                     ************************************************************************************/
                    SqliteSubscriptionDatabase();

                    /**
                     * Default destructor
                     */
                    virtual ~SqliteSubscriptionDatabase();

                    /**
                     * Initializes the subscription store
                     *
                     * @param[in]   dbPath  Represents absolute path where DB can store into.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool init(std::string &dbPath) ;

                    /**
                     * DeInitializes the subscription store
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool deinit() ;

                    /**
                     * Saves a Pending add subscription record... this record will be deleted on receiving subscription response
                     *
                     * @param[in]   requestId   Represents the request identifier.
                     * @param[in]   subscription    Represents the subscription object of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool savePendingMySubscription( int requestId,
                                                            OC::Cm::Notification::SubscriptionRecord &subscription ) ;

                    /**
                     * Delete a pending subscription record.
                     *
                     * @param[in]   requestId   Represents the request identifier.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool deletePendingMySubscription( int requestId );

                    /**
                     * Updates subscription record.
                     *
                     * @param[in]   requestId   Represents the request identifier.
                     * @param[in]   subscriptionUri Represents the subscription URI
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool updateMySubscription(int requestId, std::string &subscriptionUri ) ;

                    /**
                     * Update subscription record of specified subscription uri.
                     *
                     * @param[in]   subscriptionUri Represents the subscription URI
                     * @param[in]   resourcePaths   Represents the subscription resource path
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool updateMySubscriptionBySubscriptionUri( std::string &subscriptionUri,
                            std::string &resourcePaths ) ;

                    /**
                     * Deletes subscription record of specified subscription uri.
                     *
                     * @param[in]   deviceId    Represents the device identifier.
                     * @param[in]   subscriptionUri Represents the subscription URI
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool deleteMySubscriptionBySubscriptionUri( std::string &deviceId,
                            std::string &subscriptionUri ) ;

                    /**
                     * Get the subscription record of specified subscription uri.
                     *
                     * @param[in]   uri             Represents the subscription URI
                     * @param[out]  subscription    Represents the subscription object of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool getMySubscriptionBySubscriptionUri( std::string &uri,
                            OC::Cm::Notification::SubscriptionRecord &subscription) ;

                    /**
                     * Gets the subscription record of specified device identifier
                     *
                     * @param[in]   deviceId    Represents the device identifier.
                     * @param[out]  subscriptions   Represents the list of subscription objects of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool getMySubscriptionByDeviceId( std::string &deviceId,
                            std::list<OC::Cm::Notification::SubscriptionRecord> &subscriptions ) ;

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
                            std::list<OC::Cm::Notification::SubscriptionRecord> &subscriptionList) ;


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
                            std::list<OC::Cm::Notification::SubscriptionRecord> &subscriptions );


                    /**
                     * Saves the subscription which has passed.
                     *
                     * @param[in] subscription  Represents subscription instance of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool saveSubscription( OC::Cm::Notification::SubscriptionRecord &subscription ) ;

                    /**
                     * Gets the list of subscription records of specified resource path
                     *
                     * @param[in]   resourcePath    Represents the resource path
                     * @param[in]   subscriptions   Represents the list of subscription objects of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool getSubscriptionContainingResource( std::string &resourcePath,
                            std::list<OC::Cm::Notification::SubscriptionRecord> &subscriptions) ;

                    /**
                     * Gets the list of subscription records of specified list of resource paths.
                     *
                     * @param[in]   resourcePathList    Represents the list of resource paths
                     * @param[in]   subscriptions   Represents the subscription object of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool getSubscriptionContainingResourceList( std::list<std::string> resourcePathList,
                            std::list<OC::Cm::Notification::SubscriptionRecord> &subscriptions) ;

                    /**
                     * Gets the subscription record of subscription identifier
                     *
                     * @param[in]   subscriptionId  Represents the subscription identifier.
                     * @param[in]   subscription    Represents the subscription object of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool getSubscriptionBySubscriptionID(std::string &subscriptionId,
                            OC::Cm::Notification::SubscriptionRecord &subscription) ;

                    /**
                     * Updating subscription record.
                     *
                     * @param[in]   subscription    Represents the subscription object of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool updateSubscription(OC::Cm::Notification::SubscriptionRecord &subscription) ;

                    /**
                     * Deletes the subscription record of specified subscription identifier
                     *
                     * @param[in]   subscriptionId  Represents the subscription identifier.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool deleteSubscriptionBySubscriptionID(std::string &subscriptionId);

                    /**
                     * Gets the list of subscription records.
                     *
                     * @param[in]   subscriptions   Represents the subscription object of SubscriptionRecord class.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    virtual bool getSubscriptions(std::list<OC::Cm::Notification::SubscriptionRecord> &subscriptions) ;

                private:

                    /**
                     * This method will create the database or opens the database if laready created.
                     *
                     * @param[in]   dbFile  Represents database file path
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    bool createOrOpenDB(std::string &dbFile);

                    /**
                     * Get the resource list from resource name.
                     *
                     * @param[in]   resourceList    Represents resource name
                     *
                     * @return Returns  list if resources
                     */
                    std::list<std::string> getListfromString(std::string resourceList);

                    /**
                     * Closed the Database
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    bool closeDB() ;

                    /**
                     * This method will execute sql statement.
                     *
                     * @param[in]   sqlStatementStr Represents Sql statement
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    bool executeSQLStatement( std::string sqlStatementStr );

                    /**
                     * This method will execute sql statement.
                     * SELECT Statement will be provided by th caller they should include columnames in Following order deviceId, subscribedResource, subscriptionUri, notificationUri, rowId
                     *
                     * @param[in]   sqlQueryStr     Represents Sql statement
                     * @param[out]  subscriptions   Represents list of subscription records.
                     *
                     * @return Returns  @c true if success@n    @c false if failure
                     */
                    bool executeSQLSelectStatement( std::string sqlQueryStr,
                                                    std::list<OC::Cm::Notification::SubscriptionRecord> &subscriptions ) ;

                    ::sqlite3 *pDBHandle ;  /**< Represents the Sqlite3 library handle */
            };




        } /* namespace Notification */
    } /* namespace Cm */
} /* namespace OC */
#endif /* SUBSCRIPTIONDATABASE_H_ */
